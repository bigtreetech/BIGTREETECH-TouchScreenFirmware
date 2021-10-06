#include "interfaceCmd.h"
#include "includes.h"

#define CMD_QUEUE_SIZE 20

typedef struct
{
  CMD gcode;
  SERIAL_PORT_INDEX port_index;  // 0: for SERIAL_PORT, 1: for SERIAL_PORT_2 etc...
} GCODE_INFO;

typedef struct
{
  GCODE_INFO queue[CMD_QUEUE_SIZE];
  uint8_t index_r;  // Ring buffer read position
  uint8_t index_w;  // Ring buffer write position
  uint8_t count;    // Count of commands in the queue
} GCODE_QUEUE;

GCODE_QUEUE infoCmd;
GCODE_QUEUE infoCacheCmd;  // Only when heatHasWaiting() is false the cmd in this cache will move to infoCmd queue.
char * cmd_ptr;
uint8_t cmd_len;
uint8_t cmd_index;
SERIAL_PORT_INDEX cmd_port_index;  // index of serial port originating the gcode
bool isPolling = true;

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
  if (strlen(format) == 0) return false;

  if (infoCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
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
// If the infoCmd queue is full, a reminder message is displayed and it will wait the queue
// is available to store the command.
void mustStoreCmd(const char * format, ...)
{
  if (strlen(format) == 0) return;

  if (infoCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    loopProcessToCondition(&isFullCmdQueue);  // wait for a free slot in the queue in case the queue is currently full
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(&infoCmd, format, va);
  va_end(va);
}

// Store Script cmd to infoCmd queue.
// For example: "M502\nM500\n" will be split into two commands "M502\n", "M500\n"
void mustStoreScript(const char * format, ...)
{
  if (strlen(format) == 0) return;

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
  if (strlen(cmd) == 0) return false;

  if (infoCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
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
  if (infoCmd.count >= CMD_QUEUE_SIZE)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    loopProcessToCondition(&isFullCmdQueue);  // wait for a free slot in the queue in case the queue is currently full
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(&infoCmd, format, va);
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

  return (cmd_port_index == PORT_1);  // if gcode is originated by TFT (SERIAL_PORT), return true
}

void updateCmd(const char * buf)
{
  strcat(cmd_ptr, buf);       // append buf to gcode
  cmd_len = strlen(cmd_ptr);  // new length of gcode
}

// Send gcode cmd to printer and remove leading gcode cmd from infoCmd queue.
bool sendCmd(bool purge, bool avoidTerminal)
{
  char * purgeStr = "[Purged] ";

  if (GET_BIT(infoSettings.general_settings, LISTENING_MODE) == 1 &&  // if TFT is in listening mode and FW type was already detected,
      infoMachineSettings.firmwareType != FW_NOT_DETECTED)            // purge the command
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
    Serial_Puts(SERIAL_PORT, cmd_ptr);
    setCurrentAckSrc(cmd_port_index);
  }

  if (!avoidTerminal)
  {
    if (purge)
      terminalCache(purgeStr, strlen(purgeStr), cmd_port_index, TERMINAL_GCODE);
    terminalCache(cmd_ptr, cmd_len, cmd_port_index, TERMINAL_GCODE);
  }

  infoCmd.count--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_QUEUE_SIZE;

  return !purge;  // return true if command was sent. Otherwise, return false
}

// Check if 'cmd' starts with 'key'.
static bool cmd_start_with(const CMD cmd, const char * key)
{
  return (strstr(cmd, key) - cmd == cmd_index) ? true : false;
}

// Check the presence of the specified 'code' character in the current gcode command.
static bool cmd_seen(char code)
{
  for (cmd_index = 0; cmd_index < cmd_len; cmd_index++)
  {
    if (cmd_ptr[cmd_index] == code)
    {
      cmd_index += 1;
      return true;
    }
  }
  return false;
}

// Get the int after 'code'. Call after cmd_seen('code').
static int32_t cmd_value(void)
{
  return (strtol(&cmd_ptr[cmd_index], NULL, 10));
}

// Get the float after 'code'. Call after cmd_seen('code').
static float cmd_float(void)
{
  return (strtod(&cmd_ptr[cmd_index], NULL));
}

// Parse and send gcode cmd in infoCmd queue.
void sendQueueCmd(void)
{
  if (infoHost.wait == true) return;
  if (infoCmd.count == 0) return;

  bool avoid_terminal = false;
  uint16_t cmd = 0;
  cmd_index = 0;
  // check if cmd is from TFT or other host
  bool fromTFT = getCmd();  // retrieve leading gcode in the queue

  if (!isPolling && fromTFT)
  { // ignore any query from TFT
    sendCmd(true, avoid_terminal);
    return;
  }

  // Skip line number from stored gcode for internal parsing purpose
  if (cmd_ptr[0] == 'N')
  {
    cmd_index = strcspn(cmd_ptr, " ") + 1;
  }

  switch (cmd_ptr[cmd_index])
  {
    // parse M-codes
    case 'M':
      cmd = strtol(&cmd_ptr[cmd_index + 1], NULL, 10);
      switch (cmd)
      {
        case 0:
        case 1:
          if (isPrinting() && infoMachineSettings.firmwareType != FW_REPRAPFW)  // Abort printing by "M0" in RepRapFirmware
          {
            // pause if printing from TFT and purge M0/M1 command.
            if (infoFile.source < BOARD_SD )
            {
              sendCmd(true, avoid_terminal);
              printPause(true, PAUSE_M0);
              return;
            }
          }
          break;

        case 18:  // M18/M84 disable steppers
        case 84:
          // Do not mark coordinate as unknown in case of a M18/M84 S<timeout> command that
          // doesn't disable the motors right away but will set their idling timeout.
          if (!(cmd_seen('S') && !cmd_seen('Y') && !cmd_seen('Z') && !cmd_seen('E')))
          {
            // This is something else than an "M18/M84 S<timeout>", this will disable at least one stepper, set coordinate as unknown
            coordinateSetKnown(false);
          }
          break;

        #ifdef SERIAL_PORT_2
          case 20:  // M20
            if (!fromTFT)
            {
              if (cmd_start_with(cmd_ptr, "M20 SD:") ||
                  cmd_start_with(cmd_ptr, "M20 U:"))
              {
                if (cmd_start_with(cmd_ptr, "M20 SD:"))
                  infoFile.source = TFT_SD;
                else
                  infoFile.source = TFT_UDISK;

                strncpy(infoFile.title, &cmd_ptr[cmd_index + 4], MAX_PATH_LEN);
                // strip out any checksum that might be in the string
                for (int i = 0; i < MAX_PATH_LEN && infoFile.title[i] != 0; i++)
                {
                  if ((infoFile.title[i] == '*') || (infoFile.title[i] == '\n') || (infoFile.title[i] == '\r'))
                  {
                    infoFile.title[i] = 0;
                    break;
                  }
                }
                Serial_Puts(SERIAL_PORT_2, "Begin file list\n");
                if (mountFS() == true && scanPrintFiles() == true)
                {
                  for (uint16_t i = 0; i < infoFile.fileCount; i++)
                  {
                    Serial_Puts(SERIAL_PORT_2, infoFile.file[i]);
                    Serial_Puts(SERIAL_PORT_2, "\n");
                  }
                  for (uint16_t i = 0; i < infoFile.folderCount; i++)
                  {
                    Serial_Puts(SERIAL_PORT_2, "/");
                    Serial_Puts(SERIAL_PORT_2, infoFile.folder[i]);
                    Serial_Puts(SERIAL_PORT_2, "/\n");
                  }
                }
                Serial_Puts(SERIAL_PORT_2, "End file list\nok\n");
                sendCmd(true, avoid_terminal);
                return;
              }
            }
            break;

          case 23:  // M23
            if (!fromTFT)
            {
              if (cmd_start_with(cmd_ptr, "M23 SD:") ||
                  cmd_start_with(cmd_ptr, "M23 U:"))
              {
                if (cmd_start_with(cmd_ptr, "M23 SD:"))
                  infoFile.source = TFT_SD;
                else
                  infoFile.source = TFT_UDISK;

                resetInfoFile();
                strncpy(infoFile.title, &cmd_ptr[cmd_index + 4], MAX_PATH_LEN);
                // strip out any checksum that might be in the string
                for (int i = 0; i < MAX_PATH_LEN && infoFile.title[i] != 0 ; i++)
                {
                  if ((infoFile.title[i] == '*') || (infoFile.title[i] == '\n') ||(infoFile.title[i] == '\r'))
                  {
                    infoFile.title[i] = 0;
                    break;
                  }
                }

                Serial_Puts(SERIAL_PORT_2, "echo:Now fresh file: ");
                Serial_Puts(SERIAL_PORT_2, infoFile.title);
                Serial_Puts(SERIAL_PORT_2, "\n");

                FIL tmp;
                if (mountFS() && (f_open(&tmp, infoFile.title, FA_OPEN_EXISTING | FA_READ) == FR_OK))
                {
                  char buf[10];
                  sprintf(buf, "%d", f_size(&tmp));
                  Serial_Puts(SERIAL_PORT_2, "File opened: ");
                  Serial_Puts(SERIAL_PORT_2, infoFile.title);
                  Serial_Puts(SERIAL_PORT_2, " Size: ");
                  Serial_Puts(SERIAL_PORT_2, buf);
                  Serial_Puts(SERIAL_PORT_2, "\nFile selected\n");
                  f_close(&tmp);
                }
                else
                {
                  Serial_Puts(SERIAL_PORT_2, "open failed, File: ");
                  Serial_Puts(SERIAL_PORT_2, infoFile.title);
                  Serial_Puts(SERIAL_PORT_2, "\n");
                }
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                sendCmd(true, avoid_terminal);
                return;
              }
            }
            break;

          case 24:  // M24
            if (!fromTFT)
            {
              if ((infoFile.source == TFT_UDISK) || (infoFile.source == TFT_SD))  // if a file was selected from TFT with M23
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printPause()
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                sendCmd(true, avoid_terminal);

                if (!isPrinting())  // if not printing, start a new print
                {
                  infoMenu.cur = 1;
                  menuBeforePrinting();
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
              if (isPrinting() && infoFile.source < BOARD_SD)  // if printing from TFT
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printPause()
                Serial_Puts(SERIAL_PORT_2, "ok\n");
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
              if (isPrinting() && infoFile.source < BOARD_SD)  // if printing from TFT
              {
                if (cmd_seen('C'))
                {
                  Serial_Puts(SERIAL_PORT_2, "Current file: ");
                  Serial_Puts(SERIAL_PORT_2, infoFile.title);
                  Serial_Puts(SERIAL_PORT_2, ".\n");
                }
                char buf[55];
                sprintf(buf, "%s printing byte %d/%d\n", (infoFile.source == TFT_SD) ? "TFT SD" : "TFT USB", getPrintCur(), getPrintSize());
                Serial_Puts(SERIAL_PORT_2, buf);
                Serial_Puts(SERIAL_PORT_2, "ok\n");
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
              isPolling = false;
            break;

          case 29:  // M29
            if (!fromTFT)
            {
              // force send M29 directly and purge to avoid any loopback
              sendCmd(false, avoid_terminal);

              mustStoreScript("M105\nM114\nM220\n");
              storeCmd("M221 D%d\n", heatGetCurrentTool());
              isPolling = true;
              return;
            }
            break;

          case 30:  // M30
            if (!fromTFT)
            {
              if (cmd_start_with(cmd_ptr, "M30 SD:") ||
                  cmd_start_with(cmd_ptr, "M30 U:"))
              {
                if (cmd_start_with(cmd_ptr, "M30 SD:"))
                  infoFile.source = TFT_SD;
                else
                  infoFile.source = TFT_UDISK;
                TCHAR filepath[MAX_PATH_LEN];
                strncpy(filepath, &cmd_ptr[cmd_index + 4], MAX_PATH_LEN);
                // strip out any checksum that might be in the string
                for (int i = 0; i < MAX_PATH_LEN && filepath[i] != 0 ; i++)
                {
                  if ((filepath[i] == '*') || (filepath[i] == '\n') || (filepath[i] == '\r'))
                  {
                    filepath[i] = 0;
                    break;
                  }
                }
                if ((mountFS() == true) && (f_unlink (filepath) == FR_OK))
                {
                  Serial_Puts(SERIAL_PORT_2, "File deleted: ");
                  Serial_Puts(SERIAL_PORT_2, filepath);
                  Serial_Puts(SERIAL_PORT_2, ".\nok\n");
                }
                else
                {
                  Serial_Puts(SERIAL_PORT_2, "Deletion failed, File: ");
                  Serial_Puts(SERIAL_PORT_2, filepath);
                  Serial_Puts(SERIAL_PORT_2, ".\nok\n");
                }
                sendCmd(true, avoid_terminal);
                return;
              }
            }
            break;

          case 98:  // RRF macro execution, do not wait for it to complete
            sendCmd(false, avoid_terminal);
            return;

          case 115:  // M115 TFT
            if (!fromTFT && cmd_start_with(cmd_ptr, "M115 TFT"))
            {
              char buf[50];
              Serial_Puts(SERIAL_PORT_2,
                          "FIRMWARE_NAME: " FIRMWARE_NAME
                          " SOURCE_CODE_URL:https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware\n");
              sprintf(buf, "Cap:HOTEND_NUM:%d\n", infoSettings.hotend_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:EXTRUDER_NUM:%d\n", infoSettings.ext_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:FAN_NUM:%d\n", infoSettings.fan_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:FAN_CTRL_NUM:%d\n", infoSettings.ctrl_fan_en ? MAX_CRTL_FAN_COUNT : 0);
              Serial_Puts(SERIAL_PORT_2, buf);
              Serial_Puts(SERIAL_PORT_2, "ok\n");
              sendCmd(true, avoid_terminal);
              return;
            }
            break;

          case 125:  // M125
            if (!fromTFT)
            {
              if (isPrinting() && infoFile.source < BOARD_SD)  // if printing from TFT
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printPause()
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                sendCmd(true, avoid_terminal);
                printPause(true, PAUSE_NORMAL);
                return;
              }
            }
            break;

          case 524:  // M524
            if (!fromTFT)
            {
              if (isPrinting() && infoFile.source < BOARD_SD)  // if printing from TFT
              {
                // firstly purge the gcode to avoid a possible reprocessing or infinite nested loop in
                // case the function loopProcess() is invoked by the following function printAbort()
                Serial_Puts(SERIAL_PORT_2, "ok\n");
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
          {
            setPrintProgressPercentage(cmd_value());
          }

          if (cmd_seen('R'))
          {
            setPrintRemainingTime((cmd_value() * 60));
            setM73_presence(true);  // disable parsing remaning time from gCode comments
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

        case 92:  // M92 Steps per unit
        {
          if (cmd_seen('X')) setParameter(P_STEPS_PER_MM, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Z, cmd_float());

          uint8_t i = (cmd_seen('T')) ? cmd_value() : 0;
          if (cmd_seen('E')) setParameter(P_STEPS_PER_MM, AXIS_INDEX_E0 + i, cmd_float());
          break;
        }

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
            {
              heatSyncUpdateSeconds(cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetUpdateSeconds());
              updateCmd(buf);
            }
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

        case 109: // M109
          if (fromTFT)
          {
            if (GET_BIT(infoSettings.general_settings, EMULATED_M109_M190) == 0)  // if emulated M109 / M190 is disabled
              break;

            cmd_ptr[cmd_index + 3] = '4';  // Avoid send M109 to Marlin
            uint8_t i = cmd_seen('T') ? cmd_value() : heatGetCurrentHotend();
            if (cmd_seen('R'))
            {
              cmd_ptr[cmd_index - 1] = 'S';
              heatSetIsWaiting(i, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(i, WAIT_HEATING);
            }
          }
        // No break here, The data processing of M109 is the same as that of M104 below
        case 104: // M104
          if (fromTFT)
          {
            uint8_t i = cmd_seen('T') ? cmd_value() : heatGetCurrentHotend();
            if (cmd_seen('S'))
            {
              heatSyncTargetTemp(i, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetTargetTemp(i));
              updateCmd(buf);
              heatSetSendWaiting(i, false);
            }
          }
          break;

        case 114:  // M114
          #ifdef FIL_RUNOUT_PIN
            if (fromTFT)
              FIL_PosE_SetUpdateWaiting(false);
          #endif
          break;

        case 117:  // M117
          if (cmd_start_with(&cmd_ptr[cmd_index + 5], "Time Left"))
          {
            parsePrintRemainingTime(&cmd_ptr[cmd_index + 14]);
          }
          else
          {
            CMD message;

            strncpy(message, &cmd_ptr[cmd_index + 4], CMD_MAX_SIZE);
            // strip out any checksum that might be in the string
            for (int i = 0; i < CMD_MAX_SIZE && message[i] != 0; i++)
            {
              if (message[i] == '*')
              {
                message[i] = 0;
                break;
              }
            }

            statusScreen_setMsg((uint8_t *)"M117", (uint8_t *)&message);

            if (MENU_IS_NOT(menuStatus))
            {
              addToast(DIALOG_TYPE_INFO, message);
            }
          }
          break;

        case 190:  // M190
          if (fromTFT)
          {
            if (GET_BIT(infoSettings.general_settings, EMULATED_M109_M190) == 0)  // if emulated M109 / M190 is disabled
              break;

            cmd_ptr[cmd_index + 2] = '4';  // Avoid send M190 to Marlin
            if (cmd_seen('R'))
            {
              cmd_ptr[cmd_index - 1] = 'S';
              heatSetIsWaiting(BED, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(BED, WAIT_HEATING);
            }
          }
        // No break here, The data processing of M190 is the same as that of M140 below
        case 140:  // M140
          if (fromTFT)
          {
            if (cmd_seen('S'))
            {
              heatSyncTargetTemp(BED, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetTargetTemp(BED));
              updateCmd(buf);
              heatSetSendWaiting(BED, false);
            }
          }
          break;

        case 191:  // M191
          if (fromTFT)
          {
            cmd_ptr[cmd_index + 2] = '4';  // Avoid send M191 to Marlin
            if (cmd_seen('R'))
            {
              cmd_ptr[cmd_index - 1] = 'S';
              heatSetIsWaiting(CHAMBER, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(CHAMBER, WAIT_HEATING);
            }
          }
        // No break here, The data processing of M191 is the same as that of M141 below
        case 141:  // M141
          if (fromTFT)
          {
            if (cmd_seen('S'))
            {
              heatSyncTargetTemp(CHAMBER, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetTargetTemp(CHAMBER));
              updateCmd(buf);
              heatSetSendWaiting(CHAMBER, false);
            }
          }
          break;

        case 200:  // M200 Set Filament Diameter
        {
          if (cmd_seen('S')) setParameter(P_FILAMENT_DIAMETER, 0, cmd_float());

          uint8_t i = (cmd_seen('T')) ? cmd_value() : 0;
          if (cmd_seen('D')) setParameter(P_FILAMENT_DIAMETER, 1 + i, cmd_float());
          if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
          {
            if (getParameter(P_FILAMENT_DIAMETER, 1) > 0.01F)  // common extruder param
              setParameter(P_FILAMENT_DIAMETER, 0, 1);  // filament_diameter>0.01 to enable  volumetric extrusion
            else
              setParameter(P_FILAMENT_DIAMETER, 0, 0);  // filament_diameter<=0.01 to disable volumetric extrusion
          }
          break;
        }

        case 201:  // M201 Maximum Acceleration (units/s2)
        {
          if (cmd_seen('X')) setParameter(P_MAX_ACCELERATION, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_MAX_ACCELERATION, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_MAX_ACCELERATION, AXIS_INDEX_Z, cmd_float());

          uint8_t i = (cmd_seen('T')) ? cmd_value() : 0;
          if (cmd_seen('E')) setParameter(P_MAX_ACCELERATION, AXIS_INDEX_E0 + i, cmd_float());
          break;
        }

        case 203:  // M203 Maximum feedrates (units/s)
        {
          if (cmd_seen('X')) setParameter(P_MAX_FEED_RATE, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_MAX_FEED_RATE, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_MAX_FEED_RATE, AXIS_INDEX_Z, cmd_float());

          uint8_t i = (cmd_seen('T')) ? cmd_value() : 0;
          if (cmd_seen('E')) setParameter(P_MAX_FEED_RATE, AXIS_INDEX_E0 + i, cmd_float());
          break;
        }

        case 204:  // M204 Acceleration (units/s2)
          if (cmd_seen('P')) setParameter(P_ACCELERATION, 0, cmd_float());
          if (cmd_seen('R')) setParameter(P_ACCELERATION, 1, cmd_float());
          if (cmd_seen('T')) setParameter(P_ACCELERATION, 2, cmd_float());
          break;

        case 205:  // M205 - Set Advanced Settings
          if (cmd_seen('X')) setParameter(P_JERK, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_JERK, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_JERK, AXIS_INDEX_Z, cmd_float());
          if (cmd_seen('E')) setParameter(P_JERK, AXIS_INDEX_E0, cmd_float());
          if (cmd_seen('J')) setParameter(P_JUNCTION_DEVIATION, 0, cmd_float());
          break;

        case 206:  // M206 Home offset
          if (cmd_seen('X')) setParameter(P_HOME_OFFSET, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_HOME_OFFSET, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_HOME_OFFSET, AXIS_INDEX_Z, cmd_float());
          break;

        case 207:  // M207 FW Retract
          if (cmd_seen('S')) setParameter(P_FWRETRACT, 0, cmd_float());
          if (cmd_seen('W')) setParameter(P_FWRETRACT, 1, cmd_float());
          if (cmd_seen('F')) setParameter(P_FWRETRACT, 2, cmd_float());
          if (cmd_seen('Z')) setParameter(P_FWRETRACT, 3, cmd_float());
          break;

        case 208:  // M208 FW Retract recover
          if (cmd_seen('S')) setParameter(P_FWRECOVER, 0, cmd_float());
          if (cmd_seen('W')) setParameter(P_FWRECOVER, 1, cmd_float());
          if (cmd_seen('F')) setParameter(P_FWRECOVER, 2, cmd_float());
          if (cmd_seen('R')) setParameter(P_FWRECOVER, 3, cmd_float());
          break;

        case 209:  // M209 Auto Retract recover
          if (cmd_seen('S')) setParameter(P_AUTO_RETRACT, 0, cmd_float());
          break;

        case 218:  // M218 Hotend Offset
          if (cmd_seen('X')) setParameter(P_HOTEND_OFFSET, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_HOTEND_OFFSET, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_HOTEND_OFFSET, AXIS_INDEX_Z, cmd_float());
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
                if (!fromTFT && cmd_start_with(cmd_ptr, "M300 TFT"))
                {
                  sendCmd(true, avoid_terminal);
                  return;
                }
              }
            }
            break;
        #endif

        case 355:  // M355
        {
          if (cmd_seen('S'))
          {
            caseLightSetState(cmd_value() > 0);
          }
          if (cmd_seen('P'))
          {
            caseLightSetBrightness(cmd_value());
          }
          caseLightApplied(true);
          break;
        }

        case 292:
        case 408:
          // RRF does not send 'ok' while executing M98
          if (rrfStatusIsMacroBusy())
          {
            sendCmd(false, avoid_terminal);
            return;
          }
          break;

        case 420:  // M420
          // ABL state will be set through parsACK.c after receiving confirmation message from the printer
          // to prevent wrong state in case of error.
          if (cmd_seen('Z')) setParameter(P_ABL_STATE, 1, cmd_float());
          break;

        case 569:  // M569 TMC StealthChop
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
          if (isPrinting())
          {
            // purge and pause only if emulated M600 is enabled.
            // if emulated M600 is disabled then let the printer pause the print to avoid premature pause
            if (GET_BIT(infoSettings.general_settings, EMULATED_M600) == 1)
            {
              sendCmd(true, avoid_terminal);
              printPause(true, PAUSE_NORMAL);
              return;
            }
          }
          break;

        #ifdef NOZZLE_PAUSE_M601
          case 601:  // M601 print pause (PrusaSlicer)
            if (isPrinting())
            {
              // purge and pause only if emulated M600 is enabled.
              // if emulated M600 is disabled then let the printer pause the print to avoid premature pause
              if (GET_BIT(infoSettings.general_settings, EMULATED_M600) == 1)
              {
                sendCmd(true, avoid_terminal);
                printPause(true, PAUSE_NORMAL);
                return;
              }
            }
            break;
        #endif

        case 665:  // Delta Configuration / Delta Tower Angle
        {
          if (cmd_seen('H')) setParameter(P_DELTA_CONFIGURATION, 0, cmd_float());
          if (cmd_seen('S')) setParameter(P_DELTA_CONFIGURATION, 1, cmd_float());
          if (cmd_seen('R')) setParameter(P_DELTA_CONFIGURATION, 2, cmd_float());
          if (cmd_seen('L')) setParameter(P_DELTA_CONFIGURATION, 3, cmd_float());
          if (cmd_seen('X')) setParameter(P_DELTA_TOWER_ANGLE, AXIS_INDEX_X, cmd_float());
          if (cmd_seen('Y')) setParameter(P_DELTA_TOWER_ANGLE, AXIS_INDEX_Y, cmd_float());
          if (cmd_seen('Z')) setParameter(P_DELTA_TOWER_ANGLE, AXIS_INDEX_Z, cmd_float());
          break;
        }

        case 666:  // Delta Endstop Adjustments
        {
          if (cmd_seen('X')) setParameter(P_DELTA_ENDSTOP, 0, cmd_float());
          if (cmd_seen('Y')) setParameter(P_DELTA_ENDSTOP, 1, cmd_float());
          if (cmd_seen('Z')) setParameter(P_DELTA_ENDSTOP, 2, cmd_float());
          break;
        }

        case 710:  // M710 Controller Fan
        {
          if (cmd_seen('S')) fanSetCurSpeed(MAX_COOLING_FAN_COUNT, cmd_value());
          if (cmd_seen('I')) fanSetCurSpeed(MAX_COOLING_FAN_COUNT + 1, cmd_value());
          break;
        }

        case 851:  // M851 Z probe offset
        {
          if (cmd_seen('X')) setParameter(P_PROBE_OFFSET, X_AXIS, cmd_float());
          if (cmd_seen('Y')) setParameter(P_PROBE_OFFSET, Y_AXIS, cmd_float());
          if (cmd_seen('Z')) setParameter(P_PROBE_OFFSET, Z_AXIS, cmd_float());
          break;
        }

        case 900:  // M900 Linear advance
        {
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('K')) setParameter(P_LIN_ADV, i, cmd_float());
          break;
        }

        case 906:  // M906 Stepper driver current
        {
          uint8_t i = (cmd_seen('I')) ? cmd_value() : 0;
          if (cmd_seen('X')) setParameter(P_CURRENT, STEPPER_INDEX_X + i, cmd_value());
          if (cmd_seen('Y')) setParameter(P_CURRENT, STEPPER_INDEX_Y + i, cmd_value());
          if (cmd_seen('Z')) setParameter(P_CURRENT, STEPPER_INDEX_Z + i, cmd_value());

          i = (cmd_seen('T')) ? cmd_value() : 0;
          if (cmd_seen('E')) setParameter(P_CURRENT, STEPPER_INDEX_E0 + i, cmd_value());
          break;
        }

        case 913:  // M913 Hybrid Threshold Speed
        {
          uint8_t i = (cmd_seen('I')) ? cmd_value() : 0;
          if (cmd_seen('X')) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_X + i, cmd_value());
          if (cmd_seen('Y')) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_Y + i, cmd_value());
          if (cmd_seen('Z')) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_Z + i, cmd_value());

          i = (cmd_seen('T')) ? cmd_value() : 0;
          if (cmd_seen('E')) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_E0 + i, cmd_value());
          break;
        }

        case 914:  // parse and store TMC Bump sensitivity values
        {
          uint8_t i = (cmd_seen('I')) ? cmd_value() : 0;
          if (cmd_seen('X')) setParameter(P_BUMPSENSITIVITY, STEPPER_INDEX_X + i, cmd_value());
          if (cmd_seen('Y')) setParameter(P_BUMPSENSITIVITY, STEPPER_INDEX_Y + i, cmd_value());
          if (cmd_seen('Z')) setParameter(P_BUMPSENSITIVITY, STEPPER_INDEX_Z + i, cmd_value());
          break;
        }
      }
      break;  // end parsing M-codes

    case 'G':
      cmd = strtol(&cmd_ptr[cmd_index + 1], NULL, 10);
      switch (cmd)
      {
        case 0:  // G0
        case 1:  // G1
        case 2:  // G2
        case 3:  // G3
        {
          AXIS i;
          for (i = X_AXIS; i < TOTAL_AXIS; i++)
          {
            if (cmd_seen(axis_id[i]))
            {
              coordinateSetAxisTarget(i, cmd_float());
            }
          }
          if (cmd_seen('F'))
          {
            coordinateSetFeedRate(cmd_value());
          }
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
                if (v == 1)
                {
                  setParameter(P_ABL_STATE, 0, 1);
                }
                else if (v == 2)
                {
                  setParameter(P_ABL_STATE, 0, 0);
                }
              }
            }
          }
          break;
        #endif

        case 90:  // G90
          coorSetRelative(false);
          break;

        case 91:  // G91
          coorSetRelative(true);
          break;

        case 92:  // G92
        {
          bool coorRelative = coorGetRelative();
          bool eRelative = eGetRelative();
          // Set to absolute mode
          coorSetRelative(false);
          eSetRelative(false);
          for (AXIS i = X_AXIS; i < TOTAL_AXIS; i++)
          {
            if (cmd_seen(axis_id[i]))
            {
              coordinateSetAxisTarget(i, cmd_float());
              #ifdef FIL_RUNOUT_PIN
                if (i == E_AXIS)
                {
                  // Reset SFS status, Avoid false Filament runout caused by G92 resetting E-axis position
                  FIL_SFS_SetAlive(true);
                }
              #endif
            }
          }
          // Restore mode
          coorSetRelative(coorRelative);
          eSetRelative(eRelative);
          break;
        }
      }
      break;  // end parsing G-codes

    case 'T':
      cmd = strtol(&cmd_ptr[cmd_index + 1], NULL, 10);
      heatSetCurrentTool(cmd);
      break;
  }  // end parsing cmd

  if (sendCmd(false, avoid_terminal) == true)  // if command was sent
    infoHost.wait = infoHost.connected;
}  // sendQueueCmd
