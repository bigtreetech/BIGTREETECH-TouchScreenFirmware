#include "interfaceCmd.h"
#include "includes.h"


QUEUE infoCmd;       //
QUEUE infoCacheCmd;  // Only when heatHasWaiting() is false the cmd in this cache will move to infoCmd queue.

static u8 cmd_index=0;

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

// Store gcode cmd to infoCmd queue, this cmd will be sent by UART in sendQueueCmd(),
// If the infoCmd queue is full, reminde in title bar.
bool storeCmd(const char * format,...)
{
  QUEUE *pQueue = &infoCmd;

  if (pQueue->count >= CMD_MAX_LIST)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }

  my_va_list ap;
  my_va_start(ap,format);
  my_vsprintf(pQueue->queue[pQueue->index_w].gcode, format, ap);
  my_va_end(ap);
  pQueue->queue[pQueue->index_w].src = SERIAL_PORT;

  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;

  return true;
}

// Store gcode cmd to infoCmd queue, this cmd will be sent by UART in sendQueueCmd(),
// If the infoCmd queue is full, reminde in title bar,  waiting for available queue and store the command.
void mustStoreCmd(const char * format,...)
{
  QUEUE *pQueue = &infoCmd;

  if(pQueue->count >= CMD_MAX_LIST) reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while (pQueue->count >= CMD_MAX_LIST)
  {
    loopProcess();
  }

  my_va_list ap;
  my_va_start(ap,format);
  my_vsprintf(pQueue->queue[pQueue->index_w].gcode, format, ap);
  my_va_end(ap);
  pQueue->queue[pQueue->index_w].src = SERIAL_PORT;

  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;
}

// Store from UART cmd(such as: ESP3D, OctoPrint, else TouchScreen) to infoCmd queue, this cmd will be sent by UART in sendQueueCmd(),
// If the infoCmd queue is full, reminde in title bar.
bool storeCmdFromUART(uint8_t port, const char * gcode)
{
  QUEUE *pQueue = &infoCmd;

  if (pQueue->count >= CMD_MAX_LIST)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }

  strcpy(pQueue->queue[pQueue->index_w].gcode, gcode);

  pQueue->queue[pQueue->index_w].src = port;
  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;

  return true;
}

// Store gcode cmd to infoCacheCmd queue, this cmd will be move to infoCmd in getGcodeFromFile() -> moveCacheToCmd(),
// this function is only for restore printing status after power failed.
void mustStoreCacheCmd(const char * format,...)
{
  QUEUE *pQueue = &infoCacheCmd;

  if(pQueue->count == CMD_MAX_LIST) reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while (pQueue->count >= CMD_MAX_LIST)
  {
    loopProcess();
  }

  my_va_list ap;
  my_va_start(ap,format);
  my_vsprintf(pQueue->queue[pQueue->index_w].gcode, format, ap);
  my_va_end(ap);

  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;
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

// Parse and send gcode cmd in infoCmd.
void sendQueueCmd(void)
{
  if(infoHost.wait == true)    return;
  if(infoCmd.count == 0)       return;

  bool avoid_terminal = false;
  u16  cmd=0;

  if (infoCmd.queue[infoCmd.index_r].src != SERIAL_PORT)
  {
    // this command did not originate with the TFT
    // look for certain commands even when they are couched behind N commands (line numbers)
    if (cmd_seen('M'))
    {
      cmd = cmd_value();
      switch(cmd)
      {

        case 92: //M92 Steps per unit
          if(cmd_seen('X')) setParameter(P_STEPS_PER_MM, X_AXIS, cmd_float());
          if(cmd_seen('Y')) setParameter(P_STEPS_PER_MM, Y_AXIS, cmd_float());
          if(cmd_seen('Z')) setParameter(P_STEPS_PER_MM, Z_AXIS, cmd_float());
          if(cmd_seen('E')) setParameter(P_STEPS_PER_MM, E_AXIS, cmd_float());
          break;

        case 117:
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
            popupReminder((u8 *)"M117", (u8 *)&message);
          }
          break;
        }
        case 106: //M106
        {
          u8 i = 0;
          if(cmd_seen('P')) i = cmd_value();
          if(cmd_seen('S'))
          {
            fanSetSpeed(i, cmd_value());
          }
          break;
        }
        case 107: //M107
        {
          u8 i = 0;
          if(cmd_seen('P')) i = cmd_value();
          fanSetSpeed(i, 0);
          break;
        }
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
        case 220: //M220
          if(cmd_seen('S'))
          {
            speedSetPercent(0,cmd_value());
          }
          break;
        case 221: //M221
          if(cmd_seen('S'))
          {
            speedSetPercent(1,cmd_value());
          }
          break;
        case 851: //M203 Maximum feedrates (units/s)
          if(cmd_seen('X')) setParameter(P_PROBE_OFFSET, X_AXIS, cmd_float());
          if(cmd_seen('Y')) setParameter(P_PROBE_OFFSET, Y_AXIS, cmd_float());
          if(cmd_seen('Z')) setParameter(P_PROBE_OFFSET, Z_AXIS, cmd_float());
          break;
        case 906: //M906 Stepper driver current
          if(cmd_seen('X')) setParameter(P_CURRENT, X_AXIS, cmd_value());
          if(cmd_seen('Y')) setParameter(P_CURRENT, Y_AXIS, cmd_value());
          if(cmd_seen('Z')) setParameter(P_CURRENT, Z_AXIS, cmd_value());
          if(cmd_seen('E')) setParameter(P_CURRENT, E_AXIS, cmd_value());
          if(cmd_seen('I'))
          {
            if(cmd_seen('X')) dualstepper[X_STEPPER] = true;
            if(cmd_seen('Y')) dualstepper[Y_STEPPER] = true;
            if(cmd_seen('Z')) dualstepper[Z_STEPPER] = true;
          }
          if(cmd_seen('T') && cmd_value() == 0)
          {
            if(cmd_seen('E')) setParameter(P_CURRENT,E_STEPPER,cmd_value());
          }
          if(cmd_seen('T') && cmd_value() == 1)
          {
            if(cmd_seen('E')) setParameter(P_CURRENT,E2_STEPPER,cmd_value());
            dualstepper[E_STEPPER] = true;
          }
          break;
          case 914: //parse and store TMC Bump sensitivity values
            if(cmd_seen('X')) setParameter(P_BUMPSENSITIVITY, X_STEPPER, cmd_float());
            if(cmd_seen('Y')) setParameter(P_BUMPSENSITIVITY, Y_STEPPER, cmd_float());
            if(cmd_seen('Z')) setParameter(P_BUMPSENSITIVITY, Z_STEPPER, cmd_float());
            break;
      }
    } //parse M code end
  }
  else
  {
    // this command originated with the TFT
    switch(infoCmd.queue[infoCmd.index_r].gcode[0])
    {
      case 'M':
        cmd=strtol(&infoCmd.queue[infoCmd.index_r].gcode[1],NULL,10);
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

          case 27: //M27
            printSetUpdateWaiting(false);
          break;

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

          case 109: //M109
          {
            TOOL i = heatGetCurrentToolNozzle();
            if(cmd_seen('T')) i = (TOOL)(cmd_value() + NOZZLE0);
            infoCmd.queue[infoCmd.index_r].gcode[3]='4';
            if (cmd_seen('R')) {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index-1] = 'S';
              heatSetIsWaiting(i, WAIT_COOLING_HEATING);
            } else {
              heatSetIsWaiting(i, WAIT_HEATING);
            }
          }
          case 104: //M104
          {
            TOOL i = heatGetCurrentToolNozzle();
            if(cmd_seen('T')) i = (TOOL)(cmd_value() + NOZZLE0);
            if(cmd_seen('S'))
            {
              heatSyncTargetTemp(i, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%d\n", heatGetTargetTemp(i));
              strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
              heatSetSendWaiting(i, false);
            }
            break;
          }

          case 105: //M105
            heatSetUpdateWaiting(false);
            avoid_terminal = infoSettings.terminalACK;

            break;

          case 106: //M106
          {
            u8 i = 0;
            if(cmd_seen('P')) i = cmd_value();
            if(cmd_seen('S'))
            {
              fanSetSpeed(i, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%d\n", fanGetSpeed(i));
              strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
              fanSetSendWaiting(i, false);
            }
            break;
          }

          case 107: //M107
          {
            u8 i = 0;
            if(cmd_seen('P')) i = cmd_value();
            fanSetSpeed(i, 0);
            break;
          }

          case 114: //M114
            #ifdef FIL_RUNOUT_PIN
              positionSetUpdateWaiting(false);
            #endif
            break;

          case 117: //M117
            statusScreen_setMsg((u8 *)"M117", (u8 *)&infoCmd.queue[infoCmd.index_r].gcode[5]);
            if (infoMenu.menu[infoMenu.cur] != menuStatus)
            {
              popupReminder((u8 *)"M117", (u8 *)&infoCmd.queue[infoCmd.index_r].gcode[5]);
            }
            break;

          case 190: //M190
            infoCmd.queue[infoCmd.index_r].gcode[2]='4';
            if (cmd_seen('R')) {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index-1] = 'S';
              heatSetIsWaiting(BED, WAIT_COOLING_HEATING);
            } else {
              heatSetIsWaiting(BED, WAIT_HEATING);
            }
          case 140: //M140
            if(cmd_seen('S'))
            {
              heatSyncTargetTemp(BED,cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%d\n", heatGetTargetTemp(BED));
              strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
              heatSetSendWaiting(BED, false);
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
          case 220: //M220
            if(cmd_seen('S'))
            {
              speedSetPercent(0,cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%d\n", speedGetPercent(0));
              strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
              speedSetSendWaiting(0, false);
            }
            break;
          case 851: //M851 Z probe offset
            if(cmd_seen('X')) setParameter(P_PROBE_OFFSET, X_AXIS, cmd_float());
            if(cmd_seen('Y')) setParameter(P_PROBE_OFFSET, Y_AXIS, cmd_float());
            if(cmd_seen('Z')) setParameter(P_PROBE_OFFSET, Z_AXIS, cmd_float());
            break;
          case 221: //M221
            if(cmd_seen('S'))
            {
              speedSetPercent(1,cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%d\n", speedGetPercent(1));
              strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
              speedSetSendWaiting(1, false);
            }
            break;

          #ifdef BUZZER_PIN
            case 300: //M300
              if (cmd_seen('S')) {
                uint16_t hz = cmd_value();
                if (cmd_seen('P')) {
                  uint16_t ms = cmd_value();
                  Buzzer_TurnOn(hz, ms);
                }
              }
              break;
          #endif

          case 906: //M906 Stepper driver current
            if(cmd_seen('X')) setParameter(P_CURRENT, X_AXIS, cmd_value());
            if(cmd_seen('Y')) setParameter(P_CURRENT, Y_AXIS, cmd_value());
            if(cmd_seen('Z')) setParameter(P_CURRENT, Z_AXIS, cmd_value());
            if(cmd_seen('E')) setParameter(P_CURRENT, E_AXIS, cmd_value());
            if(cmd_seen('I'))
            {
              if(cmd_seen('X')) dualstepper[X_STEPPER] = true;
              if(cmd_seen('Y')) dualstepper[Y_STEPPER] = true;
              if(cmd_seen('Z')) dualstepper[Z_STEPPER] = true;
            }
            if(cmd_seen('T') && cmd_value() == 0)
            {
              if(cmd_seen('E')) setParameter(P_CURRENT,E_STEPPER,cmd_value());
            }
            if(cmd_seen('T') && cmd_value() == 1)
            {
              if(cmd_seen('E')) setParameter(P_CURRENT,E2_STEPPER,cmd_value());
              dualstepper[E_STEPPER] = true;
            }
          case 914: //parse and store TMC Bump sensitivity values
            if(cmd_seen('X')) setParameter(P_BUMPSENSITIVITY, X_STEPPER, cmd_float());
            if(cmd_seen('Y')) setParameter(P_BUMPSENSITIVITY, Y_STEPPER, cmd_float());
            if(cmd_seen('Z')) setParameter(P_BUMPSENSITIVITY, Z_STEPPER, cmd_float());
            break;
        }
        break; //end parsing M-codes

      case 'G':
        cmd=strtol(&infoCmd.queue[infoCmd.index_r].gcode[1],NULL,10);
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
        break; //end parsing M-codes

      case 'T':
        cmd=strtol(&infoCmd.queue[infoCmd.index_r].gcode[1], NULL, 10);
        heatSetCurrentToolNozzle((TOOL)(cmd + NOZZLE0));
        break;
    }

  }

  setCurrentAckSrc(infoCmd.queue[infoCmd.index_r].src);
  Serial_Puts(SERIAL_PORT, infoCmd.queue[infoCmd.index_r].gcode); //
  if (avoid_terminal != true){
    sendGcodeTerminalCache(infoCmd.queue[infoCmd.index_r].gcode, TERMINAL_GCODE);
  }
  infoCmd.count--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_MAX_LIST;

  infoHost.wait = infoHost.connected;          //
}
