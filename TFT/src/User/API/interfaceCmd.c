#include "interfaceCmd.h"
#include "includes.h"


QUEUE infoCmd;       //
QUEUE infoCacheCmd;  //only heatHasWaiting() is false 
                     //the cmd in this cache will move to infoCmd 

static u8 cmd_index=0;

//
static bool cmd_seen(char code)
{  
  for(cmd_index = 0; infoCmd.queue[infoCmd.index_r].gcode[cmd_index] != 0 && cmd_index < CMD_MAX_CHAR; cmd_index++)
  {
    if(infoCmd.queue[infoCmd.index_r].gcode[cmd_index] == code)
    {
      cmd_index+=1;
      return true;
    }
  }
  return false;
}

static u32 cmd_value(void)
{
  return (strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index],NULL,10));
}

static float cmd_float(void)
{
  return (strtod(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index],NULL));
}

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

bool moveCacheToCmd(void)
{
  if(infoCmd.count >= CMD_MAX_LIST) return false;
  if(infoCacheCmd.count == 0) return false;
  
  storeCmd("%s", infoCacheCmd.queue[infoCacheCmd.index_r].gcode);
  infoCacheCmd.count--;
  infoCacheCmd.index_r = (infoCacheCmd.index_r + 1) % CMD_MAX_LIST;
  return true;
}

void clearCmdQueue(void)
{
  infoCmd.count = infoCmd.index_w = infoCmd.index_r =0;  
  infoCacheCmd.count = infoCacheCmd.index_w = infoCacheCmd.index_r =0;
  heatSetUpdateWaiting(false);
}

void sendQueueCmd(void)
{
  if(infoHost.wait == true)    return;  
  if(infoCmd.count == 0)       return;
  
  u16  cmd=0;
  switch(infoCmd.queue[infoCmd.index_r].gcode[0])
  {
    case 'M':
      cmd=strtol(&infoCmd.queue[infoCmd.index_r].gcode[1],NULL,10);
      switch(cmd)
      {
        case 18: //M18/M84 disable steppers
        case 84:
          coordinateSetClear(false);
          break;
        
        case 27: //M27
          printSetUpdateWaiting(false);
        break;
        
        #ifdef PS_ON_PIN
        case 80: //M80
          PS_ON_On();
          break;
        
        case 81: //M81
          PS_ON_Off();
          break;
        #endif
        
        case 82: //M82
          eSetRelative(false);
        break;

        case 83: //M83
          eSetRelative(true);
        break;

        case 109: //M109
        {
          TOOL i = heatGetCurrentToolNozzle();
          if(cmd_seen('T')) i = (TOOL)(cmd_value() + NOZZLE0);
          infoCmd.queue[infoCmd.index_r].gcode[3]='4';
          heatSetIsWaiting(i,true);
        }
        case 104: //M104
        {
          TOOL i = heatGetCurrentToolNozzle();
          if(cmd_seen('T')) i = (TOOL)(cmd_value() + NOZZLE0);
          if(cmd_seen('S'))
          {	
            heatSetTargetTemp(i, cmd_value()); 
          }
          else
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
          break;

        case 106: //M106
        {
          u8 i = 0;
          if(cmd_seen('P')) i = cmd_value();
          if(cmd_seen('S'))
          {
            fanSetSpeed(i, cmd_value()); 
          }
          else
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
          break;

        case 117: //M117
          popupReminder((u8* )"M117", (u8 *)&infoCmd.queue[infoCmd.index_r].gcode[5]);
          break;

        case 190: //M190
          infoCmd.queue[infoCmd.index_r].gcode[2]='4';
          heatSetIsWaiting(BED,true);											
        case 140: //M140
          if(cmd_seen('S'))
          {
            heatSetTargetTemp(BED,cmd_value()); 
          }
          else
          {
            char buf[12];
            sprintf(buf, "S%d\n", heatGetTargetTemp(BED));
            strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
            heatSetSendWaiting(BED, false);
          }
          break;
          
        case 220: //M220
          if(cmd_seen('S'))
          {
            speedSetPercent(0,cmd_value()); 
          }
          else
          {
            char buf[12];
            sprintf(buf, "S%d\n", speedGetPercent(0));
            strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
            speedSetSendWaiting(0, false);
          }
          break;
        case 221: //M221
          if(cmd_seen('S'))
          {
            speedSetPercent(1,cmd_value()); 
          }
          else
          {
            char buf[12];
            sprintf(buf, "S%d\n", speedGetPercent(1));
            strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
            speedSetSendWaiting(1, false);
          }
          break;
      }
      break;

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
              coordinateSetAxis(i,cmd_float());
            }
          }
          if(cmd_seen('F'))			
          {
            coordinateSetFeedRate(cmd_value());
          }
          break;
        }
        
        case 28: //G28
          coordinateSetClear(true);
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
          for(i=X_AXIS;i<TOTAL_AXIS;i++)
          {						
            if(cmd_seen(axis_id[i]))			
            {                       
              coordinateSetAxis(i,cmd_float());                 
            }
          }
          break;
        }
      }
      break;

    case 'T':
      cmd=strtol(&infoCmd.queue[infoCmd.index_r].gcode[1], NULL, 10);
      heatSetCurrentToolNozzle((TOOL)(cmd + NOZZLE0));
      break;
  }
  
  setCurrentAckSrc(infoCmd.queue[infoCmd.index_r].src);
  Serial_Puts(SERIAL_PORT, infoCmd.queue[infoCmd.index_r].gcode); //
  infoCmd.count--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_MAX_LIST;
  
  infoHost.wait = infoHost.connected;          //

  powerFailedEnable(true);
}
