#include "interfaceCmd.h"
#include "includes.h"
#include "RRFSendCmd.h"

#define CMD_QUEUE_SIZE 20

typedef struct
{
  CMD gcode;
  SERIAL_PORT_INDEX port_index;  // 0: for SERIAL_PORT, 1: for SERIAL_PORT_2 etc...
} GCODE_INFO;

typedef struct
{
  GCODE_INFO queue[CMD_QUEUE_SIZE];
  uint8_t index_r;  // ring buffer read position
  uint8_t index_w;  // ring buffer write position
  uint8_t count;    // count of commands in the queue
} GCODE_QUEUE;

typedef enum
{
  NO_WRITING = 0,
  TFT_WRITING,
  ONBOARD_WRITING
} WRITING_MODE;

GCODE_QUEUE infoCmd;
GCODE_QUEUE infoCacheCmd;  // only when heatHasWaiting() is false the cmd in this cache will move to infoCmd queue
char * cmd_ptr;
uint8_t cmd_len;
SERIAL_PORT_INDEX cmd_port_index;  // index of serial port originating the gcode
uint8_t cmd_port;                  // physical port (e.g. _USART1) related to serial port index
uint8_t cmd_base_index;            // base index in case the gcode has checksum ("Nxxx " is present at the beginning of gcode)
uint8_t cmd_index;
WRITING_MODE writing_mode = NO_WRITING;
FIL file;

bool isFullCmdQueue(void)
{
  return (infoCmd.count >= CMD_QUEUE_SIZE);
}

bool isNotEmptyCmdQueue(void)
{
  return (infoCmd.count != 0 || infoHost.wait == true);
}

bool isEnqueued(const CMD cmd)
{
  bool found = false;

  for (int i = 0; i < infoCmd.count && !found; ++i)
  {
    found = strcmp(cmd, infoCmd.queue[(infoCmd.index_r + i) % CMD_QUEUE_SIZE].gcode) == 0;
  }

  return found;
}

bool isWritingMode(void)
{
  return (writing_mode != NO_WRITING);
}

// Common store cmd.
void commonStoreCmd(GCODE_QUEUE * pQueue, const char * format, va_list va)
{
  vsnprintf(pQueue->queue[pQueue->index_w].gcode, CMD_MAX_SIZE, format, va);

  pQueue->queue[pQueue->index_w].port_index = PORT_1;  // port index for SERIAL_PORT
  pQueue->index_w = (pQueue->index_w + 1) % CMD_QUEUE_SIZE;
  pQueue->count++;
}

// Store gcode cmd to infoCmd queue.
// This command will be sent to the printer by sendQueueCmd().
// If the infoCmd queue is full, a reminder message is displayed and the command is discarded.
bool storeCmd(const char * format, ...)
{
  if (format[0] == 0) return false;

  if (infoCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, SYS_STATUS_BUSY);
    return false;
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(&infoCmd, format, va);
  va_end(va);

  return true;
}

// Store gcode cmd to infoCmd queue.
// This command will be sent to the printer by sendQueueCmd().
// If the infoCmd queue is full, a reminder message is displayed
// and it will for wait the queue to be able to store the command.
void mustStoreCmd(const char * format, ...)
{
  if (format[0] == 0) return;

  if (infoCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, SYS_STATUS_BUSY);
    loopProcessToCondition(&isFullCmdQueue);  // wait for a free slot in the queue in case the queue is currently full
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(&infoCmd, format, va);
  va_end(va);
}

// Store Script cmd to infoCmd queue.
// For example: "M502\nM500\n" will be split into two commands "M502\n", "M500\n".
void mustStoreScript(const char * format, ...)
{
  if (format[0] == 0) return;

  char script[256];
  va_list va;
  va_start(va, format);
  vsnprintf(script, 256, format, va);
  va_end(va);

  char * p = script;
  uint16_t i = 0;
  CMD cmd;

  for (;;)
  {
    char c = *p++;
    if (!c) return;
    cmd[i++] = c;

    if (c == '\n')
    {
      cmd[i] = 0;
      mustStoreCmd("%s", cmd);
      i = 0;
    }
  }
}

// Store gcode cmd received from UART (e.g. ESP3D, OctoPrint, other TouchScreen etc...) to infoCmd queue.
// This command will be sent to the printer by sendQueueCmd().
// If the infoCmd queue is full, a reminder message is displayed and the command is discarded.
bool storeCmdFromUART(SERIAL_PORT_INDEX portIndex, const CMD cmd)
{
  if (cmd[0] == 0) return false;

  if (infoCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, SYS_STATUS_BUSY);
    return false;
  }

  strncpy(infoCmd.queue[infoCmd.index_w].gcode, cmd, CMD_MAX_SIZE);

  infoCmd.queue[infoCmd.index_w].port_index = portIndex;
  infoCmd.index_w = (infoCmd.index_w + 1) % CMD_QUEUE_SIZE;
  infoCmd.count++;

  return true;
}

// Store gcode cmd to infoCacheCmd queue.
// This command will be moved to infoCmd queue by loopPrintFromTFT() -> moveCacheToCmd().
// This function is used only to restore the printing status after a power failed.
void mustStoreCacheCmd(const char * format, ...)
{
  if (infoCacheCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, SYS_STATUS_BUSY);
    loopProcessToCondition(&isFullCmdQueue);  // wait for a free slot in the queue in case the queue is currently full
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(&infoCacheCmd, format, va);
  va_end(va);
}

// Move gcode cmd from infoCacheCmd to infoCmd queue.
bool moveCacheToCmd(void)
{
  if (infoCmd.count >= CMD_QUEUE_SIZE) return false;
  if (infoCacheCmd.count == 0) return false;

  storeCmd("%s", infoCacheCmd.queue[infoCacheCmd.index_r].gcode);
  infoCacheCmd.count--;
  infoCacheCmd.index_r = (infoCacheCmd.index_r + 1) % CMD_QUEUE_SIZE;

  return true;
}

// Clear all gcode cmd in infoCmd queue when printing is aborted.
void clearCmdQueue(void)
{
  infoCmd.count = infoCmd.index_w = infoCmd.index_r = 0;
  infoCacheCmd.count = infoCacheCmd.index_w = infoCacheCmd.index_r = 0;
  heatSetUpdateWaiting(false);
  printSetUpdateWaiting(false);
}

static inline bool getCmd(void)
{
  cmd_ptr = &infoCmd.queue[infoCmd.index_r].gcode[0];          // gcode
  cmd_len = strlen(cmd_ptr);                                   // length of gcode
  cmd_port_index = infoCmd.queue[infoCmd.index_r].port_index;  // index of serial port originating the gcode
  cmd_port = serialPort[cmd_port_index].port;                  // physical port (e.g. _USART1) related to serial port index
  cmd_base_index = cmd_index = 0;

  return (cmd_port_index == PORT_1);  // if gcode is originated by TFT (SERIAL_PORT), return true
}

// Send gcode cmd to printer and remove leading gcode cmd from infoCmd queue.
bool sendCmd(bool purge, bool avoidTerminal)
{
  char * purgeStr = "[Purged] ";

  if (GET_BIT(infoSettings.general_settings, INDEX_LISTENING_MODE) == 1 &&  // if TFT is in listening mode and FW type was already detected,
      infoMachineSettings.firmwareType != FW_NOT_DETECTED)                  // purge the command
    purge = true;

  #if defined(SERIAL_DEBUG_PORT) && defined(DEBUG_SERIAL_COMM)
    // dump serial data sent to debug port
    Serial_Puts(SERIAL_DEBUG_PORT, serialPort[cmd_port_index].id);  // serial port ID (e.g. "2" for SERIAL_PORT_2)
    Serial_Puts(SERIAL_DEBUG_PORT, ">>");

    if (purge)
      Serial_Puts(SERIAL_DEBUG_PORT, purgeStr);

    Serial_Puts(SERIAL_DEBUG_PORT, cmd_ptr);
  #endif

  if (!purge)  // if command is not purged, send it to printer
  {
    if (infoMachineSettings.firmwareType != FW_REPRAPFW)
      Serial_Puts(SERIAL_PORT, cmd_ptr);
    else
      rrfSendCmd(cmd_ptr);

    setCurrentAckSrc(cmd_port_index);
  }

  if (!avoidTerminal && MENU_IS(menuTerminal))
  {
    if (purge)
      terminalCache(purgeStr, strlen(purgeStr), cmd_port_index, SRC_TERMINAL_GCODE);

    terminalCache(cmd_ptr, cmd_len, cmd_port_index, SRC_TERMINAL_GCODE);
  }

  infoCmd.count--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_QUEUE_SIZE;

  return !purge;  // return true if command was sent. Otherwise, return false
}

// Check the presence of the specified "keyword" string in the current gcode command
// starting the search from index "index".
static bool cmd_seen_from(uint8_t index, const char * keyword)
{
  if (index >= cmd_len)
    return false;

  char * strPtr = strstr(cmd_ptr + index, keyword);

  if (strPtr != NULL)
  {
    cmd_index = (strPtr - cmd_ptr) + strlen(keyword);
    return true;
  }

  return false;
}

// Check the presence of the specified "code" character in the current gcode command.
static bool cmd_seen(char code)
{
  for (cmd_index = cmd_base_index; cmd_index < cmd_len; cmd_index++)
  {
    if (cmd_ptr[cmd_index] == code)
    {
      cmd_index += 1;
      return true;
    }
  }

  return false;
}

// Get the int after "code". Call after cmd_seen(code).
static int32_t cmd_value(void)
{
  return (strtol(&cmd_ptr[cmd_index], NULL, 10));
}

// Get the int after "/", if any.
static int32_t cmd_second_value(void)
{
  char * secondValue = strchr(&cmd_ptr[cmd_index], '/');

  if (secondValue != NULL)
    return (strtol(secondValue + 1, NULL, 10));
  else
    return -0.5;
}

// Get the float after "code". Call after cmd_seen(code).
static float cmd_float(void)
{
  return (strtod(&cmd_ptr[cmd_index], NULL));
}

bool initRemoteTFT()
{
  // examples:
  //
  // "cmd_ptr" = "N1 M23 SD:/test/cap2.gcode*36\n"
  // "cmd_ptr" = "N1 M23 S /test/cap2.gcode*36\n"
  //
  // "infoFile.path" = "SD:/test/cap2.gcode"

  // e.g. "N1 M23 SD:/test/cap2.gcode*36\n" -> "SD:/test/cap2.gcode*36\n"
  //
  if (cmd_seen_from(cmd_base_index, "SD:") || cmd_seen_from(cmd_base_index, "S "))
    infoFile.source = FS_TFT_SD;   // set source first
  else if (cmd_seen_from(cmd_base_index, "U:") || cmd_seen_from(cmd_base_index, "U "))
    infoFile.source = FS_TFT_USB;  // set source first
  else
    return false;

  // present just to make the code robust. It should never be used printing from remote TFT media
  infoFile.onboardSource = BOARD_SD;

  CMD path;  // temporary working buffer (cmd_ptr buffer must always remain unchanged)

  // cmd_index was set by cmd_seen_from function
  strcpy(path, &cmd_ptr[cmd_index]);  // e.g. "N1 M23 SD:/test/cap2.gcode*36\n" -> "/test/cap2.gcode*36\n"
  stripChecksum(path);                // e.g. "/test/cap2.gcode*36\n" -> /test/cap2.gcode"

  resetInfoFile();               // then reset infoFile (source is restored)
  enterFolder(stripHead(path));  // set path as last

  return true;
}

bool openRemoteTFT(bool writingMode)
{
  bool open = false;

  Serial_Puts(cmd_port, "echo:Now fresh file: ");
  Serial_Puts(cmd_port, infoFile.path);
  Serial_Puts(cmd_port, "\n");

  if (!writingMode)  // if reading mode
  {
    // mount FS and open the file (infoFile.source and infoFile.path are used)
    if (mountFS() == true && f_open(&file, infoFile.path, FA_OPEN_EXISTING | FA_READ) == FR_OK)
    {
      char buf[10];

      sprintf(buf, "%d", f_size(&file));
      f_close(&file);

      Serial_Puts(cmd_port, "File opened: ");
      Serial_Puts(cmd_port, infoFile.path);
      Serial_Puts(cmd_port, " Size: ");
      Serial_Puts(cmd_port, buf);
      Serial_Puts(cmd_port, "\nFile selected\n");

      open = true;
    }
  }
  else  // if writing mode
  {
    // mount FS and open the file (infoFile.source and infoFile.path are used)
    if (mountFS() == true && f_open(&file, infoFile.path, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK)
    {
      Serial_Puts(cmd_port, "Writing to file: ");
      Serial_Puts(cmd_port, infoFile.path);
      Serial_Puts(cmd_port, "\n");

      open = true;
    }
  }

  if (!open)
  {
    Serial_Puts(cmd_port, "open failed, File: ");
    Serial_Puts(cmd_port, infoFile.path);
    Serial_Puts(cmd_port, "\n");
  }

  Serial_Puts(cmd_port, "ok\n");

  return open;
}

void writeRemoteTFT()
{
  // examples:
  //
  // "cmd_ptr" = "N1 G28*46\n"
  // "cmd_ptr" = "N2 G29*56\n"
  // "cmd_ptr" = "N3 M29*66\n"

  // if M29, stop writing mode. cmd_index (used by cmd_value function) was set by sendQueueCmd function
  if (cmd_ptr[cmd_base_index] == 'M' && cmd_value() == 29)  // e.g. "N3 M29*66\n" -> "M29*66\n"
  {
    f_close(&file);

    Serial_Puts(cmd_port, "Done saving file.\n");

    writing_mode = NO_WRITING;
  }
  else
  {
    UINT br;
    CMD cmd;  // temporary working buffer (cmd_ptr buffer must always remain unchanged)

    strcpy(cmd, &cmd_ptr[cmd_base_index]);  // e.g. "N1 G28*46\n" -> "G28*46\n"
    stripChecksum(cmd);                     // e.g. "G28*46\n" -> "G28"

    f_write(&file, cmd, strlen(cmd), &br);

    // "\n" is always removed by stripChecksum function even if there is no checksum, so we need to write it on file separately
    f_write(&file, "\n", 1, &br);
    f_sync(&file);
  }

  Serial_Puts(cmd_port, "ok\n");
}

void setWaitHeating(uint8_t index)
{
  if (cmd_seen('R'))
  {
    cmd_ptr[cmd_index - 1] = 'S';
    heatSetIsWaiting(index, true);
  }
  else if (cmd_seen('S'))
  {
    heatSetIsWaiting(index, (cmd_value() > heatGetCurrentTemp(index) - TEMPERATURE_RANGE));
  }
}

void syncTargetTemp(uint8_t index)
{
  uint16_t temp;

  if (cmd_seen('S'))
  {
    temp = cmd_value();

    if (temp != heatGetTargetTemp(index))
      heatSetTargetTemp(index, temp, FROM_CMD);  
  }
}


// Parse and send gcode cmd in infoCmd queue.
void sendQueueCmd(void)
{
  if (infoHost.wait == true) return;
  if (infoCmd.count == 0) return;

  bool avoid_terminal = false;
  bool fromTFT = getCmd();  // retrieve leading gcode in the queue and check if it is originated by TFT or other hosts
  char * strPtr = cmd_ptr;  // cmd_ptr was set by getCmd function

  // skip leading spaces
  while (*strPtr == ' ') strPtr++;           // e.g. "  N1   G28*46\n" -> "N1   G28*46\n"

  // skip N[-0-9] (line number) if included in the command line
  if (*strPtr == 'N' && NUMERIC(strPtr[1]))  // e.g. "N1   G28*46\n" -> "G28*46\n"
  {
    strPtr += 2;                             // skip N[-0-9]
    while (NUMERIC(*strPtr)) ++strPtr;       // skip [0-9]*
    while (*strPtr == ' ') ++strPtr;         // skip [ ]*
  }

  // set cmd_base_index with index of gcode command
  cmd_base_index = strPtr - cmd_ptr;         // e.g. "  N1   G28*46\n" -> "G28*46\n"

  // set cmd_index with index of gcode value
  cmd_index = cmd_base_index + 1;            // e.g. "G28*46\n" -> "28*46\n"

  if (writing_mode != NO_WRITING)  // if writing mode (previously triggered by M28)
  {
    if (fromTFT)  // ignore any command from TFT media
    {
      sendCmd(true, avoid_terminal);
    }
    else if (writing_mode == TFT_WRITING)  // if the command is from remote to TFT media
    {
      writeRemoteTFT();

      sendCmd(true, avoid_terminal);
    }
    else  // otherwise, if the command is from remote to onboard media
    {
      if (cmd_ptr[cmd_base_index] == 'M' && cmd_value() == 29)  // if M29, stop writing mode
        writing_mode = NO_WRITING;

      if (sendCmd(false, avoid_terminal) == true)  // if the command was sent
        infoHost.wait = infoHost.connected;
    }

    return;
  }

  switch (cmd_ptr[cmd_base_index])
  {
    // parse M-codes
    case 'M':
      switch (cmd_value())
      {
        case 0:
        case 1:
          if (isPrinting() && infoMachineSettings.firmwareType != FW_REPRAPFW)  // abort printing by "M0" in RepRapFirmware
          {
            // pause if printing from TFT media and purge M0/M1 command
            if (infoFile.source < FS_ONBOARD_MEDIA)
            {
              sendCmd(true, avoid_terminal);
              printPause(true, PAUSE_M0);
              return;
            }
          }
          break;

        case 18:  // M18/M84 disable steppers
        case 84:
          // do not mark coordinate as unknown in case of a M18/M84 S<timeout> command that
          // doesn't disable the motors right away but will set their idling timeout
          if (!(cmd_seen('S') && !cmd_seen('Y') && !cmd_seen('Z') && !cmd_seen('E')))
          {
            // this is something else than an "M18/M84 S<timeout>", this will disable at least one stepper,
            // set coordinate as unknown
            coordinateSetKnown(false);
          }
          break;

        #ifdef SERIAL_PORT_2
          case 20:  // M20
            if (!fromTFT)
            {
              if (initRemoteTFT())  // examples: "M20 SD:/test\n", "M20 S /test\n"
              {
                Serial_Puts(cmd_port, "Begin file list\n");

                // then mount FS and scan for files (infoFile.source and infoFile.path are used)
                if (mountFS() == true && scanPrintFiles() == true)
                {
                  for (uint16_t i = 0; i < infoFile.fileCount; i++)
                  {
                    Serial_Puts(cmd_port, infoFile.file[i]);
                    Serial_Puts(cmd_port, "\n");
                  }

                  for (uint16_t i = 0; i < infoFile.folderCount; i++)
                  {
                    Serial_Puts(cmd_port, "/");
                    Serial_Puts(cmd_port, infoFile.folder[i]);
                    Serial_Puts(cmd_port, "/\n");
                  }
                }

                Serial_Puts(cmd_port, "End file list\nok\n");

                sendCmd(true, avoid_terminal);
                return;
              }
            }
            break;

          case 23:  // M23
            if (!fromTFT)
            {
              if (initRemoteTFT())  // examples: "M23 SD:/test/cap2.gcode\n", "M23 S /test/cap2.gcode\n"
              {
                openRemoteTFT(false);

                sendCmd(true, avoid_terminal);
                return;
              }
            }
            break;

          case 24:  // M24
            if (!fromTFT)
            {
              // NOTE: If the file was selected (with M23) from onboard media, infoFile.source will be set to FS_ONBOARD_MEDIA_REMOTE
              //       by the startRemotePrint function called in parseAck.c during M23 ACK parsing

              if (infoFile.source < FS_ONBOARD_MEDIA)  // if a file was selected from TFT media with M23
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printPause()
                Serial_Puts(cmd_port, "ok\n");
                sendCmd(true, avoid_terminal);

                if (!isPrinting())  // if not printing, start a new print
                {
                  startPrint();  // start print and open Printing menu
                }
                else  // if printing, resume the print, in case it is paused, or continue to print
                {
                  printPause(false, PAUSE_NORMAL);
                }

                return;
              }
            }
            break;

          case 25:  // M25
            if (!fromTFT)
            {
              if (isTFTPrinting())  // if printing from TFT media
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printPause()
                Serial_Puts(cmd_port, "ok\n");
                sendCmd(true, avoid_terminal);

                printPause(true, PAUSE_NORMAL);
                return;
              }
            }
            break;

          case 27:  // M27
            if (rrfStatusIsMacroBusy())
            {
              sendCmd(true, avoid_terminal);
              return;
            }

            if (!fromTFT)
            {
              if (isTFTPrinting())  // if printing from TFT media
              {
                char buf[55];

                if (cmd_seen('C'))
                {
                  Serial_Puts(cmd_port, "Current file: ");
                  Serial_Puts(cmd_port, infoFile.path);
                  Serial_Puts(cmd_port, ".\n");
                }

                sprintf(buf, "%s printing byte %d/%d\n", (infoFile.source == FS_TFT_SD) ? "TFT SD" : "TFT USB", getPrintCur(), getPrintSize());
                Serial_Puts(cmd_port, buf);
                Serial_Puts(cmd_port, "ok\n");

                sendCmd(true, avoid_terminal);
                return;
              }
            }
            else
            {
              printSetUpdateWaiting(false);
            }
            break;

          case 28:  // M28
            if (!fromTFT)
            {
              if (initRemoteTFT())  // examples: "M28 SD:/test/cap2.gcode\n", "M28 S /test/cap2.gcode\n"
              {
                if (openRemoteTFT(true))  // if file was successfully open, switch to TFT writing mode
                {
                  writing_mode = TFT_WRITING;
                  reminderMessage(LABEL_LISTENING, SYS_STATUS_LISTENING);
                }

                sendCmd(true, avoid_terminal);
                return;
              }
              else  // if it's a request to onboard media, switch to onboard writing mode and forward the command to onboard
              {
                writing_mode = ONBOARD_WRITING;
                reminderMessage(LABEL_LISTENING, SYS_STATUS_LISTENING);
              }
            }
            break;

          case 29:  // M29
            if (!fromTFT)
            {
              // NOTE: this scenario is reachable only if not already in writing mode (no M28 was previously received).
              //       So, we only need to send back and ACK message

              Serial_Puts(cmd_port, "ok\n");
              sendCmd(true, avoid_terminal);
              return;
            }
            break;

          case 30:  // M30
            if (!fromTFT)
            {
              if (initRemoteTFT())  // examples: "M30 SD:/test/cap2.gcode\n", "M30 S /test/cap2.gcode\n"
              {
                // then mount FS and delete the file (infoFile.source and infoFile.path are used)
                if (mountFS() == true && f_unlink(infoFile.path) == FR_OK)
                  Serial_Puts(cmd_port, "File deleted: ");
                else
                  Serial_Puts(cmd_port, "Deletion failed, File: ");

                Serial_Puts(cmd_port, infoFile.path);
                Serial_Puts(cmd_port, ".\nok\n");

                sendCmd(true, avoid_terminal);
                return;
              }
            }
            break;

          case 98:  // RRF macro execution, do not wait for it to complete
            sendCmd(false, avoid_terminal);
            return;

          case 115:  // M115
            if (!fromTFT && cmd_seen_from(cmd_base_index, "TFT"))  // "M115 TFT"
            {
              char buf[50];

              Serial_Puts(cmd_port,
                          "FIRMWARE_NAME: " FIRMWARE_NAME
                          " SOURCE_CODE_URL:https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware\n");
              sprintf(buf, "Cap:HOTEND_NUM:%d\n", infoSettings.hotend_count);
              Serial_Puts(cmd_port, buf);
              sprintf(buf, "Cap:EXTRUDER_NUM:%d\n", infoSettings.ext_count);
              Serial_Puts(cmd_port, buf);
              sprintf(buf, "Cap:FAN_NUM:%d\n", infoSettings.fan_count);
              Serial_Puts(cmd_port, buf);
              sprintf(buf, "Cap:FAN_CTRL_NUM:%d\n", infoSettings.ctrl_fan_en ? MAX_CRTL_FAN_COUNT : 0);
              Serial_Puts(cmd_port, buf);
              Serial_Puts(cmd_port, "ok\n");

              sendCmd(true, avoid_terminal);
              return;
            }
            break;

          case 125:  // M125
            if (!fromTFT)
            {
              if (isTFTPrinting())  // if printing from TFT media
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printPause()
                Serial_Puts(cmd_port, "ok\n");
                sendCmd(true, avoid_terminal);

                printPause(true, PAUSE_NORMAL);
                return;
              }
            }
            break;

          case 524:  // M524
            if (!fromTFT)
            {
              if (isTFTPrinting())  // if printing from TFT media
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printAbort()
                Serial_Puts(cmd_port, "ok\n");
                sendCmd(true, avoid_terminal);

                printAbort();
                return;
              }
            }
            break;

        #else  // not SERIAL_PORT_2
          case 27:  // M27
            printSetUpdateWaiting(false);
            break;
        #endif  // not SERIAL_PORT_2

        case 73:
          if (cmd_seen('P'))
            setPrintProgressPercentage(cmd_value());

          if (cmd_seen('R'))
          {
            setPrintRemainingTime((cmd_value() * 60));
            setTimeFromSlicer(true);  // disable parsing remaning time from gcode comments
            if (getPrintProgSource() == PROG_FILE && infoSettings.prog_source == 1)
              setPrintProgSource(PROG_TIME);
          }

          if (!infoMachineSettings.buildPercent)  // if M73 is not supported by Marlin, skip it
          {
            sendCmd(true, avoid_terminal);
            return;
          }
          break;

        case 80:  // M80
          #ifdef PS_ON_PIN
            PS_ON_On();
          #endif
          break;

        case 81:  // M81
          #ifdef PS_ON_PIN
            PS_ON_Off();
          #endif
          break;

        case 82:  // M82
          eSetRelative(false);
          break;

        case 83:  // M83
          eSetRelative(true);
          break;

        case 105:  // M105
          if (rrfStatusIsMacroBusy())
          {
            sendCmd(true, avoid_terminal);
            return;
          }

          if (fromTFT)
          {
            heatSetUpdateWaiting(false);
            avoid_terminal = !infoSettings.terminal_ack;
          }
          break;

        case 155:  // M155
          if (rrfStatusIsMacroBusy())
          {
            sendCmd(true, avoid_terminal);
            return;
          }

          if (fromTFT)
          {
            heatSetUpdateWaiting(false);

            if (cmd_seen('S'))
              heatSyncUpdateSeconds(cmd_value());

          }
          break;

        case 106:  // M106
        {
          uint8_t i = cmd_seen('P') ? cmd_value() : 0;

          if (cmd_seen('S')) fanSetCurSpeed(i, cmd_value());
          break;
        }

        case 107:  // M107
        {
          uint8_t i = cmd_seen('P') ? cmd_value() : 0;

          fanSetCurSpeed(i, 0);
          break;
        }

        case 109:  // M109
          if (fromTFT)
          {
            if (GET_BIT(infoSettings.general_settings, INDEX_EMULATED_M109_M190) == 0)  // if emulated M109 / M190 is disabled
              break;

            cmd_ptr[cmd_base_index + 3] = '4';  // avoid to send M109 to Marlin, send M104
            setWaitHeating(cmd_seen('T') ? cmd_value() : heatGetCurrentHotend());
          }
        // no break here. The data processing of M109 is the same as that of M104 below
        case 104:  // M104
          if (fromTFT)
          {
            syncTargetTemp(cmd_seen('T') ? cmd_value() : heatGetCurrentHotend());
          }
          break;

        case 114:  // M114
          #ifdef FIL_RUNOUT_PIN
            if (fromTFT)
              FIL_PosE_SetUpdateWaiting(false);
          #endif
          break;

        case 117:  // M117
          if (cmd_seen_from(cmd_base_index, "Time Left"))  // parsing printing time left
          {
            // format: Time Left <XX>h<YY>m<ZZ>s (e.g. Time Left 02h04m06s)
            parsePrintRemainingTime(&cmd_ptr[cmd_index]);  // cmd_index was set by cmd_seen_from function
            setTimeFromSlicer(true);
          }
          else if (cmd_seen_from(cmd_base_index, "Layer Left"))  // parsing printing layer left
          {
            // format: Layer Left <XXXX>/<YYYY> (e.g. Layer Left 51/940)
            setPrintLayerNumber(cmd_value());
            setPrintLayerCount(cmd_second_value());
          }
          else if (cmd_seen_from(cmd_base_index, "Data Left"))  // parsing printing data left
          {
            // format: Data Left <XXXX>/<YYYY> (e.g. Data Left 123/12345)
            setPrintProgress(cmd_value(), cmd_second_value());
          }
          else
          {
            CMD message;

            strncpy(message, &cmd_ptr[cmd_base_index + 4], CMD_MAX_SIZE);
            stripChecksum(message);

            statusScreen_setMsg((uint8_t *)"M117", (uint8_t *)&message);

            if (MENU_IS_NOT(menuStatus))
              addToast(DIALOG_TYPE_INFO, message);
          }
          break;

        case 190:  // M190
          if (fromTFT)
          {
            if (GET_BIT(infoSettings.general_settings, INDEX_EMULATED_M109_M190) == 0)  // if emulated M109 / M190 is disabled
              break;

            cmd_ptr[cmd_base_index + 2] = '4';  // avoid to send M190 to Marlin, send M140 
            setWaitHeating(BED);
          }
          // no break here. The data processing of M190 is the same as that of M140 below
          case 140:  // M140
            if (fromTFT)
            {
              syncTargetTemp(BED);
            }
            break;

        case 191:  // M191
          if (fromTFT)
          {
            cmd_ptr[cmd_base_index + 2] = '4';  // avoid to send M191 to Marlin, send M141
            setWaitHeating(CHAMBER);
          }
          // no break here. The data processing of M191 is the same as that of M141 below
          case 141:  // M141
            if (fromTFT)
            {
              syncTargetTemp(CHAMBER);
            }
            break;

        case 200:  // M200 filament diameter
        {
          if (cmd_seen('S')) setParameter(P_FILAMENT_DIAMETER, 0, cmd_float());

          uint8_t i = (cmd_seen('T')) ? cmd_value() : 0;

          if (cmd_seen('D')) setParameter(P_FILAMENT_DIAMETER, 1 + i, cmd_float());

          if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
          {
            // filament_diameter > 0.01 to enable volumetric extrusion. Otherwise (<= 0.01), disable volumetric extrusion
            setParameter(P_FILAMENT_DIAMETER, 0, getParameter(P_FILAMENT_DIAMETER, 1) > 0.01f ? 1 : 0);
          }
          break;
        }

        case 92:   // M92 axis steps-per-unit (steps/mm)
        case 201:  // M201 max acceleration (units/s2)
        case 203:  // M203 max feedrate (units/s)
        {
          uint8_t param = P_STEPS_PER_MM;

          if (cmd_value() == 201) param = P_MAX_ACCELERATION;  // P_MAX_ACCELERATION
          if (cmd_value() == 203) param = P_MAX_FEED_RATE;     // P_MAX_FEED_RATE

          if (cmd_seen('X')) setParameter(param, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(param, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(param, AXIS_INDEX_Z, cmd_float());

          uint8_t i = (cmd_seen('T')) ? cmd_value() : 0;

          if (cmd_seen('E')) setParameter(param, AXIS_INDEX_E0 + i, cmd_float());
          break;
        }

        case 204:  // M204 acceleration (units/s2)
          if (cmd_seen('P')) setParameter(P_ACCELERATION, 0, cmd_float());
          if (cmd_seen('R')) setParameter(P_ACCELERATION, 1, cmd_float());
          if (cmd_seen('T')) setParameter(P_ACCELERATION, 2, cmd_float());
          break;

        case 205:  // M205 advanced settings
          if (cmd_seen('X')) setParameter(P_JERK, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_JERK, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_JERK, AXIS_INDEX_Z, cmd_float());
          if (cmd_seen('E')) setParameter(P_JERK, AXIS_INDEX_E0, cmd_float());
          if (cmd_seen('J')) setParameter(P_JUNCTION_DEVIATION, 0, cmd_float());
          break;

        case 206:  // M206 home offset
        case 218:  // M218 hotend offset
        case 851:  // M851 probe offset
        {
          uint8_t param = P_HOME_OFFSET;

          if (cmd_value() == 218) param = P_HOTEND_OFFSET;  // P_HOTEND_OFFSET
          if (cmd_value() == 851) param = P_PROBE_OFFSET;   // P_PROBE_OFFSET

          if (cmd_seen('X')) setParameter(param, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(param, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(param, AXIS_INDEX_Z, cmd_float());
          break;
        }

        case 207:  // M207 FW retraction
        case 208:  // M208 FW recover
        {
          uint8_t param = P_FWRETRACT;

          if (cmd_value() == 208) param = P_FWRECOVER;  // P_FWRECOVER

          if (cmd_seen('S')) setParameter(param, 0, cmd_float());
          if (cmd_seen('W')) setParameter(param, 1, cmd_float());
          if (cmd_seen('F')) setParameter(param, 2, cmd_float());

          if (param == P_FWRETRACT)
          {
            if (cmd_seen('Z')) setParameter(param, 3, cmd_float());
          }
          else  // P_FWRECOVER
          {
            if (cmd_seen('R')) setParameter(param, 3, cmd_float());
          }
          break;
        }

        case 209:  // M209 auto retract
          if (cmd_seen('S')) setParameter(P_AUTO_RETRACT, 0, cmd_float());
          break;

        case 220:  // M220
          if (cmd_seen('S'))
            speedSetCurPercent(0, cmd_value());
          break;

        case 221:  // M221
          if (cmd_seen('S'))
            speedSetCurPercent(1, cmd_value());
          break;

        #ifdef BUZZER_PIN
          case 300:  // M300
            if (cmd_seen('S'))
            {
              uint16_t hz = cmd_value();

              if (cmd_seen('P'))
              {
                uint16_t ms = cmd_value();

                Buzzer_TurnOn(hz, ms);

                if (!fromTFT && cmd_seen_from(cmd_base_index, "TFT"))  // "M300 TFT"
                {
                  sendCmd(true, avoid_terminal);
                  return;
                }
              }
            }
            break;
        #endif

        case 301:  // Hotend PID
        {
          if (cmd_seen('P')) setParameter(P_HOTEND_PID, 0, cmd_float());
          if (cmd_seen('I')) setParameter(P_HOTEND_PID, 1, cmd_float());
          if (cmd_seen('D')) setParameter(P_HOTEND_PID, 2, cmd_float());
          break;
        }

        case 304:  // Bed PID
        {
          if (cmd_seen('P')) setParameter(P_BED_PID, 0, cmd_float());
          if (cmd_seen('I')) setParameter(P_BED_PID, 1, cmd_float());
          if (cmd_seen('D')) setParameter(P_BED_PID, 2, cmd_float());
          break;
        }

        case 306:  // M306
          if (getMpcTuningStatus() == REQUESTED && cmd_seen('T'))  // only if requested by GUI
            setMpcTuningStatus(STARTED);

          break;

        case 355:  // M355
          if (cmd_seen('S'))
            caseLightSetState(cmd_value() > 0);

          if (cmd_seen('P'))
            caseLightSetBrightness(cmd_value());
          break;

        case 376:  // M376 (Reprap FW)
          if (infoMachineSettings.firmwareType == FW_REPRAPFW && cmd_seen('H'))
            setParameter(P_ABL_STATE, 1, cmd_float());
          break;

        case 292:
        case 408:
          // RRF does not send "ok" while executing M98
          if (rrfStatusIsMacroBusy())
          {
            sendCmd(false, avoid_terminal);
            return;
          }
          break;

        //case 420:  // M420
        //  // ABL state and Z fade height will be set through parsACK.c after receiving confirmation
        //  // message from the printer to prevent wrong state and/or value in case of error
        //  break;

        case 569:  // M569 TMC stepping mode
        {
          uint8_t k = (cmd_seen('S')) ? cmd_value() : 0;
          uint8_t i = (cmd_seen('I')) ? cmd_value() : 0;

          if (cmd_seen('X')) setParameter(P_STEALTH_CHOP, STEPPER_INDEX_X + i, k);
          if (cmd_seen('Y')) setParameter(P_STEALTH_CHOP, STEPPER_INDEX_Y + i, k);
          if (cmd_seen('Z')) setParameter(P_STEALTH_CHOP, STEPPER_INDEX_Z + i, k);

          i = (cmd_seen('T')) ? cmd_value() : 0;

          if (cmd_seen('E')) setParameter(P_STEALTH_CHOP, STEPPER_INDEX_E0 + i, k);
          break;
        }

        case 600:  // M600 filament change
        #ifdef NOZZLE_PAUSE_M601
          case 601:  // M601 print pause (PrusaSlicer)
        #endif
          if (isPrinting())
          {
            // purge and pause only if emulated M600 is enabled.
            // if emulated M600 is disabled then let the printer pause the print to avoid premature pause
            if (GET_BIT(infoSettings.general_settings, INDEX_EMULATED_M600) == 1)
            {
              sendCmd(true, avoid_terminal);
              printPause(true, PAUSE_NORMAL);
              return;
            }
          }
          break;

        case 665:  // Delta configuration / Delta tower angle
        case 666:  // Delta endstop adjustments
        {
          uint8_t param = P_DELTA_TOWER_ANGLE;

          if (cmd_value() == 666) param = P_DELTA_ENDSTOP;  // P_DELTA_ENDSTOP

          if (param < P_DELTA_ENDSTOP)  // options not supported by M666
          {
            if (cmd_seen('H')) setParameter(P_DELTA_CONFIGURATION, 0, cmd_float());
            if (cmd_seen('S')) setParameter(P_DELTA_CONFIGURATION, 1, cmd_float());
            if (cmd_seen('R')) setParameter(P_DELTA_CONFIGURATION, 2, cmd_float());
            if (cmd_seen('L')) setParameter(P_DELTA_CONFIGURATION, 3, cmd_float());
            if (cmd_seen('A')) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_X, cmd_float());
            if (cmd_seen('B')) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_Y, cmd_float());
            if (cmd_seen('C')) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_Z, cmd_float());
          }

          if (cmd_seen('X')) setParameter(param, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(param, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(param, AXIS_INDEX_Z, cmd_float());
          break;
        }

        case 710:  // M710 controller fan
          if (cmd_seen('S')) fanSetCurSpeed(MAX_COOLING_FAN_COUNT, cmd_value());
          if (cmd_seen('I')) fanSetCurSpeed(MAX_COOLING_FAN_COUNT + 1, cmd_value());
          break;

        case 900:  // M900 linear advance factor
        {
          uint8_t i = 0;

          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('K')) setParameter(P_LIN_ADV, i, cmd_float());
          break;
        }

        case 906:  // M906 stepper motor current
        case 913:  // M913 TMC hybrid threshold speed
        case 914:  // M914 TMC bump sensitivity
        {
          uint8_t param = P_CURRENT;

          if (cmd_value() == 913) param = P_HYBRID_THRESHOLD;  // P_HYBRID_THRESHOLD
          if (cmd_value() == 914) param = P_BUMPSENSITIVITY;   // P_BUMPSENSITIVITY

          uint8_t i = (cmd_seen('I')) ? cmd_value() : 0;

          if (i > 0)  // "X1"->0, "X2"->1, "Y1"->0, "Y2"->1, "Z1"->0, "Z2"->1, "Z3"->2, "Z4"->3
            i--;

          if (cmd_seen('X')) setParameter(param, STEPPER_INDEX_X + i, cmd_value());
          if (cmd_seen('Y')) setParameter(param, STEPPER_INDEX_Y + i, cmd_value());
          if (cmd_seen('Z')) setParameter(param, STEPPER_INDEX_Z + i, cmd_value());

          if (param < P_BUMPSENSITIVITY)  // T and E options not supported by M914
          {
            i = (cmd_seen('T')) ? cmd_value() : 0;

            if (cmd_seen('E')) setParameter(param, STEPPER_INDEX_E0 + i, cmd_value());
          }
          break;
        }
      }
      break;  // end parsing M-codes

    case 'G':
      switch (cmd_value())
      {
        case 0:  // G0
        case 1:  // G1
        case 2:  // G2
        case 3:  // G3
        {
          for (AXIS i = X_AXIS; i < TOTAL_AXIS; i++)
          {
            if (cmd_seen(axis_id[i]))
              coordinateSetAxisTarget(i, cmd_float());
          }

          if (cmd_seen('F'))
            coordinateSetFeedRate(cmd_value());
          break;
        }

        case 28:  // G28
          coordinateSetKnown(true);
          babystepReset();
          storeCmd("M503 S0\n");
          break;

        #if BED_LEVELING_TYPE > 0  // if not Disabled
          case 29:  // G29
          {
            if (infoMachineSettings.firmwareType != FW_REPRAPFW)
            {
              if (cmd_seen('A'))
              {
                setParameter(P_ABL_STATE, 0, 1);
                storeCmd("M117 UBL active\n");
              }

              if (cmd_seen('D'))
              {
                setParameter(P_ABL_STATE, 0, 0);
                storeCmd("M117 UBL inactive\n");
              }
            }
            else  // if RRF
            {
              if (cmd_seen('S'))
              {
                uint8_t v = cmd_value();

                if (v == 1 || v == 2)
                  setParameter(P_ABL_STATE, 0, v % 2);  // value will be 1 if v == 1, 0 if v == 2
              }
            }
          }
          break;
        #endif

        case 90:  // G90, set absolute position mode, in Marlin this includes the extruder position unless overridden by M83.
          coorSetRelative(false);
          if (infoMachineSettings.firmwareType == FW_MARLIN)
            eSetRelative(false);
          break;

        case 91:  // G91, set relative position mode, in Marlin this includes the extruder position unless overridden by M82.
          coorSetRelative(true);
          if (infoMachineSettings.firmwareType == FW_MARLIN)
            eSetRelative(true);
          break;

        case 92:  // G92
        {
          bool coorRelative = coorGetRelative();
          bool eRelative = eGetRelative();

          // set to absolute mode
          coorSetRelative(false);
          eSetRelative(false);

          for (AXIS i = X_AXIS; i < TOTAL_AXIS; i++)
          {
            if (cmd_seen(axis_id[i]))
            {
              coordinateSetAxisTarget(i, cmd_float());

              #ifdef FIL_RUNOUT_PIN
                if (i == E_AXIS)  // reset SFS status. Avoid false Filament runout caused by G92 resetting E-axis position
                  FIL_SFS_SetAlive(true);
              #endif
            }
          }

          // restore mode
          coorSetRelative(coorRelative);
          eSetRelative(eRelative);
          break;
        }
      }
      break;  // end parsing G-codes

    case 'T':
      heatSetCurrentTool(cmd_value());
      break;
  }  // end parsing cmd

  if (sendCmd(false, avoid_terminal) == true)  // if command was sent
    infoHost.wait = infoHost.connected;
}  // sendQueueCmd
