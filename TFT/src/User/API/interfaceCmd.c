#include "interfaceCmd.h"
#include "includes.h"


GCODE_QUEUE infoCmd;       //
GCODE_QUEUE infoCacheCmd;  // Only when heatHasWaiting() is false the cmd in this cache will move to infoCmd queue.

static u8 cmd_index=0;

static bool ispolling = true;

// Is there a code character in the current gcode command.
static bool cmd_seen(char code)
{
  for(cmd_index = 0; infoCmd.queue[infoCmd.index_r].gcode[cmd_index] != 0 && cmd_index < CMD_MAX_CHAR; cmd_index++)
  {
    if(infoCmd.queue[infoCmd.index_r].gcode[cmd_index] == code)
    {
      cmd_index += 1;
      return true;
    }
  }
  return false;
}

// Get the int after 'code', Call after cmd_seen('code').
static u32 cmd_value(void)
{
  return (strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index], NULL, 10));
}

// Get the float after 'code', Call after cmd_seen('code').
static float cmd_float(void)
{
  return (strtod(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index], NULL));
}

//check if 'string' start with 'search'
bool static startsWith(TCHAR *search, TCHAR *string)
{
  return (strstr(string, search) - string == cmd_index) ? true : false;
}

// Common store cmd
void commonStoreCmd(GCODE_QUEUE *pQueue, const char* format, va_list va)
{
  vsnprintf(pQueue->queue[pQueue->index_w].gcode, CMD_MAX_CHAR, format, va);

  pQueue->queue[pQueue->index_w].src = SERIAL_PORT;
  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;
}

// Store gcode cmd to infoCmd queue, this cmd will be sent by UART in sendQueueCmd(),
// If the infoCmd queue is full, reminde in title bar.
bool storeCmd(const char * format,...)
{
  if (strlen(format) == 0) return false;

  GCODE_QUEUE *pQueue = &infoCmd;

  if (pQueue->count >= CMD_MAX_LIST)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(pQueue, format, va);
  va_end(va);

  return true;
}

// Store gcode cmd to infoCmd queue, this cmd will be sent by UART in sendQueueCmd(),
// If the infoCmd queue is full, reminde in title bar,  waiting for available queue and store the command.
void mustStoreCmd(const char * format,...)
{
  if (strlen(format) == 0) return;

  GCODE_QUEUE *pQueue = &infoCmd;

  if(pQueue->count >= CMD_MAX_LIST) reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while (pQueue->count >= CMD_MAX_LIST)
  {
    loopProcess();
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(pQueue, format, va);
  va_end(va);
}

// Store Script cmd to infoCmd queue
// For example: "M502\nM500\n" will be split into two commands "M502\n", "M500\n"
void mustStoreScript(const char * format,...)
{
  if (strlen(format) == 0) return;

  char script[256];
  va_list va;
  va_start(va, format);
  vsnprintf(script, 256, format, va);
  va_end(va);

  char *p = script;
  uint16_t i = 0;
  char cmd[CMD_MAX_CHAR];
  for (;;) {
    char c = *p++;
    if (!c) return;
    cmd[i++] = c;

    if (c == '\n') {
      cmd[i] = 0;
      mustStoreCmd("%s", cmd);
      i = 0;
    }
  }
}

// Store from UART cmd(such as: ESP3D, OctoPrint, else TouchScreen) to infoCmd queue, this cmd will be sent by UART in sendQueueCmd(),
// If the infoCmd queue is full, reminde in title bar.
bool storeCmdFromUART(uint8_t port, const char * gcode)
{
  if (strlen(gcode) == 0) return false;
  GCODE_QUEUE *pQueue = &infoCmd;

  if (pQueue->count >= CMD_MAX_LIST)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }

  strncpy(pQueue->queue[pQueue->index_w].gcode, gcode, CMD_MAX_CHAR);

  pQueue->queue[pQueue->index_w].src = port;
  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;

  return true;
}

// Store gcode cmd to infoCacheCmd queue, this cmd will be move to infoCmd in getGcodeFromFile() -> moveCacheToCmd(),
// this function is only for restore printing status after power failed.
void mustStoreCacheCmd(const char * format,...)
{
  GCODE_QUEUE *pQueue = &infoCacheCmd;

  if(pQueue->count == CMD_MAX_LIST) reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while (pQueue->count >= CMD_MAX_LIST)
  {
    loopProcess();
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(pQueue, format, va);
  va_end(va);
}

// Move gcode cmd from infoCacheCmd to infoCmd queue.
bool moveCacheToCmd(void)
{
  if(infoCmd.count >= CMD_MAX_LIST) return false;
  if(infoCacheCmd.count == 0) return false;

  storeCmd("%s", infoCacheCmd.queue[infoCacheCmd.index_r].gcode);
  infoCacheCmd.count--;
  infoCacheCmd.index_r = (infoCacheCmd.index_r + 1) % CMD_MAX_LIST;
  return true;
}

// Clear all gcode cmd in infoCmd queue for abort printing.
void clearCmdQueue(void)
{
  infoCmd.count = infoCmd.index_w = infoCmd.index_r =0;
  infoCacheCmd.count = infoCacheCmd.index_w = infoCacheCmd.index_r = 0;
  heatSetUpdateWaiting(false);
}

//remove last line from cmd queue
void purgeLastCmd(void)
{
  infoCmd.count--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_MAX_LIST;
}

// Parse and send gcode cmd in infoCmd.
void sendQueueCmd(void)
{
  if(infoHost.wait == true)    return;
  if(infoCmd.count == 0)       return;

  bool avoid_terminal = false;
  u16  cmd=0;
  cmd_index = 0;
  //check if cmd is from TFT or other host
  bool fromTFT = (infoCmd.queue[infoCmd.index_r].src == SERIAL_PORT);

  if (!ispolling && !fromTFT)
  { //ignore any query from TFT
    purgeLastCmd();
    return;
  }

  // Skip line number from stored gcode for internal parsing purpose
  if (infoCmd.queue[infoCmd.index_r].gcode[0] == 'N')
  {
    cmd_index = strcspn(infoCmd.queue[infoCmd.index_r].gcode, " ") + 1;
  }

  switch(infoCmd.queue[infoCmd.index_r].gcode[cmd_index])
  {
    // parse M-codes
    case 'M':
      cmd = strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 1], NULL, 10);
      switch(cmd)
      {
        case 0:
          if (isPrinting()) {
            setPrintPause(true,true);
          }
          break;
        case 1:
          if (isPrinting()) {
            setPrintPause(true,true);
          }
          break;
        case 18: //M18/M84 disable steppers
        case 84:
          coordinateSetKnown(false);
          break;

#ifdef SERIAL_PORT_2
        case 20: //M20
          if (!fromTFT)
          {
            if (startsWith("M20 SD:", infoCmd.queue[infoCmd.index_r].gcode) ||startsWith("M20 U:", infoCmd.queue[infoCmd.index_r].gcode))   {
            if(startsWith("M20 SD:", infoCmd.queue[infoCmd.index_r].gcode)) infoFile.source = TFT_SD;
            else infoFile.source = TFT_UDISK;
            strncpy(infoFile.title, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], MAX_PATH_LEN);
            // strip out any checksum that might be in the string
            for (int i = 0; i < MAX_PATH_LEN && infoFile.title[i] !=0 ; i++)
              {
                if ((infoFile.title[i] == '*') || (infoFile.title[i] == '\n') ||(infoFile.title[i] == '\r'))
                {
                  infoFile.title[i] = 0;
                  break;
                }
              }
            Serial_Puts(SERIAL_PORT_2, "Begin file list\n");
            if (mountFS() == true && scanPrintFiles() == true){
              for (uint16_t i = 0; i < infoFile.f_num; i++) {
                Serial_Puts(SERIAL_PORT_2,infoFile.file[i]);
                Serial_Puts(SERIAL_PORT_2,"\n");
              }
              for (uint16_t i = 0; i < infoFile.F_num; i++) {
                Serial_Puts(SERIAL_PORT_2,"/");
                Serial_Puts(SERIAL_PORT_2,infoFile.folder[i]);
                Serial_Puts(SERIAL_PORT_2,"/\n");
              }
            }
            Serial_Puts(SERIAL_PORT_2, "End file list\nok\n");
            purgeLastCmd();
            return;
            }
          }
          break;

        case 23: //M23
          if (!fromTFT)
          {
            if (startsWith("M23 SD:", infoCmd.queue[infoCmd.index_r].gcode) || startsWith("M23 U:", infoCmd.queue[infoCmd.index_r].gcode))
            {
              if(startsWith("M23 SD:", infoCmd.queue[infoCmd.index_r].gcode))
                infoFile.source = TFT_SD;
              else
                infoFile.source = TFT_UDISK;
              resetInfoFile();
              strncpy(infoFile.title, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], MAX_PATH_LEN);
              // strip out any checksum that might be in the string
              for (int i = 0; i < MAX_PATH_LEN && infoFile.title[i] !=0 ; i++)
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
              if (mountFS() && (f_open(&tmp, infoFile.title, FA_OPEN_EXISTING | FA_READ) == FR_OK) )
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
              purgeLastCmd();
              return;
            }
          }
          break;

        case 24: //M24
          if (!fromTFT)
          {
            if ((infoFile.source == TFT_UDISK) || (infoFile.source == TFT_SD))
            {
              if (isPause())
              {
                setPrintPause(false, false);
              }
              else {
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                purgeLastCmd();
                infoMenu.cur = 1;
                menuBeforePrinting();
              }
              return;
            }
          }
          break;

        case 25: //M25
          if (!fromTFT)
          {
            if (isPrinting() && !infoHost.printing)
            {
              setPrintPause(true, false);
              Serial_Puts(SERIAL_PORT_2, "ok\n");
              purgeLastCmd();
              return;
            }
          }
          break;

        case 27: //M27
          if (!fromTFT)
          {
            if (isPrinting() && !infoHost.printing)
            {
              if (cmd_seen('C')){
                Serial_Puts(SERIAL_PORT_2, "Current file: ");
                Serial_Puts(SERIAL_PORT_2, infoFile.title);
                Serial_Puts(SERIAL_PORT_2, ".\n");
              }
                char buf[55];
                sprintf(buf, "%s printing byte %d/%d\n",(infoFile.source==TFT_SD)?"TFT SD":"TFT USB", getPrintCur(),getPrintSize());
                Serial_Puts(SERIAL_PORT_2, buf);
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                purgeLastCmd();
                return;
            }
          }
          else
          {
            printSetUpdateWaiting(false);
          }
          break;

        case 28: //M28
          if (!fromTFT)
            ispolling = false;
          break;

        case 29: //M29
          if (!fromTFT)
          {
            storeCmd("M105\nM114\nM220\nM221\n");
            ispolling = true;
          }
            break;

        case 30: //M30
          if (!fromTFT)
          {
            if (startsWith("M30 SD:", infoCmd.queue[infoCmd.index_r].gcode) || startsWith("M30 U:", infoCmd.queue[infoCmd.index_r].gcode))
            {
              if(startsWith("M30 SD:", infoCmd.queue[infoCmd.index_r].gcode)) infoFile.source = TFT_SD;
              else infoFile.source = TFT_UDISK;
              TCHAR filepath[MAX_PATH_LEN];
              strncpy(filepath, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], MAX_PATH_LEN);
              // strip out any checksum that might be in the string
              for (int i = 0; i < MAX_PATH_LEN && filepath[i] !=0 ; i++)
                {
                  if ((filepath[i] == '*') || (filepath[i] == '\n') ||(filepath[i] == '\r'))
                  {
                    filepath[i] = 0;
                    break;
                  }
                }
              if ((mountFS() == true) && (f_unlink (filepath)== FR_OK))
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

              purgeLastCmd();
              return;
            }
          }
          break;

        case 115: //M115 TFT
          if (!fromTFT && startsWith("M115 TFT", infoCmd.queue[infoCmd.index_r].gcode))
            {
              char buf[50];
              Serial_Puts(SERIAL_PORT_2, "FIRMWARE_NAME: " FIRMWARE_NAME " SOURCE_CODE_URL:https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware\n");
              sprintf(buf, "Cap:HOTEND_NUM:%d\n", infoSettings.hotend_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:EXTRUDER_NUM:%d\n", infoSettings.ext_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:FAN_NUM:%d\n", infoSettings.fan_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              Serial_Puts(SERIAL_PORT_2, "ok\n");
              purgeLastCmd();
              return;
            }
          break;

        case 524: //M524
          if (!fromTFT && isPrinting() && !infoHost.printing)
            {
              abortPrinting();
              Serial_Puts(SERIAL_PORT_2, "ok\n");
              purgeLastCmd();
              return;
            }
          break;
#else
        case 27: //M27
            printSetUpdateWaiting(false);
        break;
#endif

        case 80: //M80
          #ifdef PS_ON_PIN
            PS_ON_On();
          #endif
          break;

        case 81: //M81
          #ifdef PS_ON_PIN
            PS_ON_Off();
          #endif
          break;

        case 82: //M82
          eSetRelative(false);
          break;

        case 83: //M83
          eSetRelative(true);
          break;

        case 92: //M92 Steps per unit
          if(cmd_seen('X')) setParameter(P_STEPS_PER_MM, X_AXIS, cmd_float());
          if(cmd_seen('Y')) setParameter(P_STEPS_PER_MM, Y_AXIS, cmd_float());
          if(cmd_seen('Z')) setParameter(P_STEPS_PER_MM, Z_AXIS, cmd_float());
          if(cmd_seen('E')) setParameter(P_STEPS_PER_MM, E_AXIS, cmd_float());
          break;

        case 105: //M105
          if (fromTFT)
          {
            heatSetUpdateWaiting(false);
            avoid_terminal = !infoSettings.terminalACK;
          }
          break;

        case 106: //M106
        {
          uint8_t i = cmd_seen('P') ? cmd_value() : 0;
          if(cmd_seen('S'))
          {
            fanSetSpeed(i, cmd_value());
          }
          else if (!cmd_seen('\n'))
          {
            char buf[12];
            sprintf(buf, "S%u\n", fanGetSpeed(i));
            strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
            fanSetSendWaiting(i, false);
          }
          break;
        }

        case 107: //M107
        {
          uint8_t i = cmd_seen('P') ? cmd_value() : 0;
          fanSetSpeed(i, 0);
          break;
        }

        case 109: //M109
          if (fromTFT)
          {
            infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 3]='4';  // Avoid send M109 to Marlin
            uint8_t i = cmd_seen('T') ? cmd_value() : heatGetCurrentHotend();
            if (cmd_seen('R'))
            {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index-1] = 'S';
              heatSetIsWaiting(i, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(i, WAIT_HEATING);
            }
          }
        // No break here, The data processing of M109 is the same as that of M104 below
        case 104: //M104
          if (fromTFT)
          {
            uint8_t i = cmd_seen('T') ? cmd_value() : heatGetCurrentHotend();
            if(cmd_seen('S'))
            {
              heatSyncTargetTemp(i, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetTargetTemp(i));
              strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
              heatSetSendWaiting(i, false);
            }
          }
          break;

        case 114: //M114
          #ifdef FIL_RUNOUT_PIN
          if (fromTFT)
            positionSetUpdateWaiting(false);
          #endif
          break;

        case 117: //M117
          {
            char message[CMD_MAX_CHAR];
            strncpy(message, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], CMD_MAX_CHAR);
            // strip out any checksum that might be in the string
            for (int i = 0; i < CMD_MAX_CHAR && message[i] !=0 ; i++)
            {
              if (message[i] == '*')
              {
                message[i] = 0;
                break;
              }
            }
            statusScreen_setMsg((u8 *)"M117", (u8 *)&message);
            if (infoMenu.menu[infoMenu.cur] != menuStatus)
            {
              addToast(DIALOG_TYPE_INFO, message);
            }
          }
          break;

        case 190: //M190
          if (fromTFT)
          {
            infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 2]='4';   // Avoid send M190 to Marlin
            if (cmd_seen('R'))
            {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index-1] = 'S';
              heatSetIsWaiting(BED, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(BED, WAIT_HEATING);
            }
          }
        // No break here, The data processing of M190 is the same as that of M140 below
        case 140: //M140
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
              strcat(infoCmd.queue[infoCmd.index_r].gcode, (const char *)buf);
              heatSetSendWaiting(BED, false);
            }
          }
          break;

        case 191: //M191
          if (fromTFT)
          {
            infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 2]='4';   // Avoid send M191 to Marlin
            if (cmd_seen('R'))
            {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index-1] = 'S';
              heatSetIsWaiting(CHAMBER, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(CHAMBER, WAIT_HEATING);
            }
          }
        // No break here, The data processing of M191 is the same as that of M141 below
        case 141: //M141
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
              strcat(infoCmd.queue[infoCmd.index_r].gcode, (const char *)buf);
              heatSetSendWaiting(CHAMBER, false);
            }
          }
          break;
        case 201: //M201 Maximum Acceleration (units/s2)
          if(cmd_seen('X')) setParameter(P_MAX_ACCELERATION, X_AXIS, cmd_float());
          if(cmd_seen('Y')) setParameter(P_MAX_ACCELERATION, Y_AXIS, cmd_float());
          if(cmd_seen('Z')) setParameter(P_MAX_ACCELERATION, Z_AXIS, cmd_float());
          if(cmd_seen('E')) setParameter(P_MAX_ACCELERATION, E_AXIS, cmd_float());
          break;
        case 203: //M203 Maximum feedrates (units/s)
          if(cmd_seen('X')) setParameter(P_MAX_FEED_RATE, X_AXIS, cmd_float());
          if(cmd_seen('Y')) setParameter(P_MAX_FEED_RATE, Y_AXIS, cmd_float());
          if(cmd_seen('Z')) setParameter(P_MAX_FEED_RATE, Z_AXIS, cmd_float());
          if(cmd_seen('E')) setParameter(P_MAX_FEED_RATE, E_AXIS, cmd_float());
          break;
        case 204: //M204 Acceleration (units/s2)
          if(cmd_seen('P')) setParameter(P_ACCELERATION,0,cmd_float());
          if(cmd_seen('R')) setParameter(P_ACCELERATION,1,cmd_float());
          if(cmd_seen('T')) setParameter(P_ACCELERATION,2,cmd_float());
          break;
        case 207: //M207 FW Retract
          if(cmd_seen('S')) setParameter(P_FWRETRACT,0,cmd_float());
          if(cmd_seen('W')) setParameter(P_FWRETRACT,1,cmd_float());
          if(cmd_seen('F')) setParameter(P_FWRETRACT,2,cmd_float());
          if(cmd_seen('Z')) setParameter(P_FWRETRACT,3,cmd_float());
          break;
        case 208: //M208 FW Retract recover
          if(cmd_seen('S')) setParameter(P_FWRECOVER,0,cmd_float());
          if(cmd_seen('W')) setParameter(P_FWRECOVER,1,cmd_float());
          if(cmd_seen('F')) setParameter(P_FWRECOVER,2,cmd_float());
          if(cmd_seen('R')) setParameter(P_FWRECOVER,3,cmd_float());
          break;
        case 220: //M220
          if(cmd_seen('S'))
            speedSetPercent(0,cmd_value());
          break;
        case 221: //M221
          if(cmd_seen('S'))
            speedSetPercent(1,cmd_value());
          break;

        #ifdef BUZZER_PIN
          case 300: //M300
            if (cmd_seen('S'))
            {
              uint16_t hz = cmd_value();
              if (cmd_seen('P'))
              {
                uint16_t ms = cmd_value();
                Buzzer_TurnOn(hz, ms);
                if (!fromTFT && startsWith("M300 TFT", infoCmd.queue[infoCmd.index_r].gcode))
                {
                  purgeLastCmd();
                  return;
                }
              }
            }
            break;
        #endif

        case 420: //M420
          if(cmd_seen('S')) {
            infoSettings.autoLevelState = cmd_value();
            setParameter(P_ABL_STATE,0,cmd_value());
            if(cmd_value()==0) storeCmd("M117 ABL inactive\n");
            else storeCmd("M117 ABL active\n");
          }
          if(cmd_seen('Z')) setParameter(P_ABL_STATE,1,cmd_float());
        break;

        #ifdef NOZZLE_PAUSE_M600_M601
          case 600: //M600/M601 pause print
          case 601:
            if (isPrinting())
            {
              setPrintPause(true, false);
              // prevent sending M600/M601 to marlin
              purgeLastCmd();
              return;
            }
            break;
        #endif
        case 851: //M851 Z probe offset
          if(cmd_seen('X')) setParameter(P_PROBE_OFFSET, X_AXIS, cmd_float());
          if(cmd_seen('Y')) setParameter(P_PROBE_OFFSET, Y_AXIS, cmd_float());
          if(cmd_seen('Z')) setParameter(P_PROBE_OFFSET, Z_AXIS, cmd_float());
          break;
        case 900: //M900 Linear advance
          if (cmd_seen('K')) setParameter(P_LIN_ADV, 0, cmd_float());
          break;
        case 906: //M906 Stepper driver current
          if(cmd_seen('X')) setParameter(P_CURRENT, X_AXIS, cmd_value());
          if(cmd_seen('Y')) setParameter(P_CURRENT, Y_AXIS, cmd_value());
          if(cmd_seen('Z')) setParameter(P_CURRENT, Z_AXIS, cmd_value());
          if(cmd_seen('E')) setParameter(P_CURRENT, E_AXIS, cmd_value());
          if(cmd_seen('I'))
          {
            if(cmd_seen('X')) setDualStepperStatus(X_STEPPER,true);
            if(cmd_seen('Y')) setDualStepperStatus(Y_STEPPER,true);
            if(cmd_seen('Z')) setDualStepperStatus(Z_STEPPER,true);
          }
          if(cmd_seen('T') && cmd_value() == 0)
          {
            if(cmd_seen('E')) setParameter(P_CURRENT,E_STEPPER,cmd_value());
          }
          if(cmd_seen('T') && cmd_value() == 1)
          {
            if(cmd_seen('E')) setParameter(P_CURRENT,E2_STEPPER,cmd_value());
            setDualStepperStatus(E_STEPPER,true);
          }
          break;
        case 914: //parse and store TMC Bump sensitivity values
          if(cmd_seen('X')) setParameter(P_BUMPSENSITIVITY, X_STEPPER, cmd_float());
          if(cmd_seen('Y')) setParameter(P_BUMPSENSITIVITY, Y_STEPPER, cmd_float());
          if(cmd_seen('Z')) setParameter(P_BUMPSENSITIVITY, Z_STEPPER, cmd_float());
          break;
      }
      break; //end parsing M-codes

    case 'G':
      cmd=strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 1],NULL,10);
      switch(cmd)
      {
        case 0: //G0
        case 1: //G1
        {
          AXIS i;
          for(i=X_AXIS;i<TOTAL_AXIS;i++)
          {
            if(cmd_seen(axis_id[i]))
            {
              coordinateSetAxisTarget(i,cmd_float());
            }
          }
          if(cmd_seen('F'))
          {
            coordinateSetFeedRate(cmd_value());
          }
          break;
        }

        case 28: //G28
          coordinateSetKnown(true);
          babyStepReset();
          storeCmd("M503 S0\n");
          break;

        case 29: //G29
          if(ENABLE_UBL_VALUE > 0) {
            if(cmd_seen('A')) {
              infoSettings.autoLevelState = 1;
              setParameter(P_ABL_STATE,0,1);
              storeCmd("M117 UBL active\n");
            }
            if(cmd_seen('D')) {
              infoSettings.autoLevelState = 0;
              setParameter(P_ABL_STATE,0,0);
              storeCmd("M117 UBL inactive\n");
            }
          }
          break;

        case 90: //G90
          coorSetRelative(false);
          break;

        case 91: //G91
          coorSetRelative(true);
          break;

        case 92: //G92
        {
          AXIS i;
          bool coorRelative = coorGetRelative();
          bool eRelative = eGetRelative();
          // Set to absolute mode
          coorSetRelative(false);
          eSetRelative(false);
          for(i=X_AXIS;i<TOTAL_AXIS;i++)
          {
            if(cmd_seen(axis_id[i]))
            {
              coordinateSetAxisTarget(i,cmd_float());
            }
          }
          // Restore mode
          coorSetRelative(coorRelative);
          eSetRelative(eRelative);
          break;
        }
      }
      break; //end parsing G-codes

    case 'T':
      cmd=strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 1], NULL, 10);
      heatSetCurrentTool(cmd);
      break;

  } // end parsing cmd

  setCurrentAckSrc(infoCmd.queue[infoCmd.index_r].src);
  Serial_Puts(SERIAL_PORT, infoCmd.queue[infoCmd.index_r].gcode);
  if (avoid_terminal != true){
    sendGcodeTerminalCache(infoCmd.queue[infoCmd.index_r].gcode, TERMINAL_GCODE);
  }
  purgeLastCmd();

  infoHost.wait = infoHost.connected;
} //sendQueueCmd
