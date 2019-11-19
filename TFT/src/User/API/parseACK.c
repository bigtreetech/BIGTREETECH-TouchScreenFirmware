#include "includes.h"
#include "parseACK.h"

char dmaL2Cache[ACK_MAX_SIZE];
static u16 ack_index=0;
static u8 ack_cur_src = SERIAL_PORT;
int MODEselect;
// Ignore reply "echo:" message (don't display in popup menu)
const char *const ignoreEcho[] = {
  "Now fresh file:",
  "Probe Z Offset:",
};

void setCurrentAckSrc(uint8_t src)
{
  ack_cur_src = src;
}

static char ack_seen(const char *str)
{
  u16 i;	
  for(ack_index=0; ack_index<ACK_MAX_SIZE && dmaL2Cache[ack_index]!=0; ack_index++)
  {
    for(i=0; str[i]!=0 && dmaL2Cache[ack_index+i]!=0 && dmaL2Cache[ack_index+i]==str[i]; i++)
    {}
    if(str[i]==0)
    {
      ack_index += i;      
      return true;
    }
  }
  return false;
}
static char ack_cmp(const char *str)
{
  u16 i;
  for(i=0; i<ACK_MAX_SIZE && str[i]!=0 && dmaL2Cache[i]!=0; i++)
  {
    if(str[i] != dmaL2Cache[i])
    return false;
  }
  if(dmaL2Cache[i] != 0) return false;
  return true;
}


static float ack_value()
{
  return (strtod(&dmaL2Cache[ack_index], NULL));
}

// Read the value after the / if exists
static float ack_second_value()
{
  char *secondValue = strchr(&dmaL2Cache[ack_index],'/');
  if(secondValue != NULL)
  {
    return (strtod(secondValue+1, NULL));
  }
  else 
  {
    return -0.5;
  }
}

void ackPopupInfo(const char *info)
{
  if (info == echomagic)
  {
    statusScreen_setMsg((u8 *)info, (u8 *)dmaL2Cache + ack_index);
  }  
  if (infoMenu.menu[infoMenu.cur] == menuTerminal) return;
  if (infoMenu.menu[infoMenu.cur] == menuStatus && info == echomagic) return;
  
  popupReminder((u8* )info, (u8 *)dmaL2Cache + ack_index);
}

void syncL2CacheFromL1(uint8_t port)
{
  uint16_t i = 0;
  for (i = 0; dmaL1Data[port].rIndex != dmaL1Data[port].wIndex; i++)
  {
    dmaL2Cache[i] = dmaL1Data[port].cache[dmaL1Data[port].rIndex];
    dmaL1Data[port].rIndex = (dmaL1Data[port].rIndex + 1) % DMA_TRANS_LEN;
  }
  dmaL2Cache[i] = 0; // End character
}

void parseACK(void)
{ 
  if(infoHost.rx_ok[SERIAL_PORT] != true) return; //not get response data
  
  syncL2CacheFromL1(SERIAL_PORT);
  infoHost.rx_ok[SERIAL_PORT] = false;
  
  if(infoHost.connected == false) //not connected to Marlin
  {
    if((!ack_seen("T:") && !ack_seen("T0:")) || !ack_seen("ok"))  goto parse_end;  //the first response should be such as "T:25/50 ok\n"
    infoHost.connected = true;
  }    

  // GCode command response
  if(requestCommandInfo.inWaitResponse && ack_seen(requestCommandInfo.startMagic))
  {
    requestCommandInfo.inResponse = true;
    requestCommandInfo.inWaitResponse = false;
  }
  if(requestCommandInfo.inResponse)
  {
    if(strlen(requestCommandInfo.cmd_rev_buf)+strlen(dmaL2Cache) < CMD_MAX_REV)
    {
      strcat(requestCommandInfo.cmd_rev_buf, dmaL2Cache);
      
      if(ack_seen(requestCommandInfo.errorMagic ))
      {
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        requestCommandInfo.inError = true;
      }
      else if(ack_seen(requestCommandInfo.stopMagic ))
      {
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
      }
    }
    else 
    {
      requestCommandInfo.done = true;
      requestCommandInfo.inResponse = false;
      ackPopupInfo(errormagic);
    }
    infoHost.wait = false;
    goto parse_end;
  }  
  // end 

  if(ack_cmp("ok\n"))
  {
    infoHost.wait = false;	
  }
  else
  {
    if(ack_seen("ok"))
    {
      infoHost.wait = false;
    }					
    if(ack_seen("T:") || ack_seen("T0:")) 
    {
      heatSetCurrentTemp(heatGetCurrentToolNozzle(), ack_value()+0.5);
      heatSyncTargetTemp(heatGetCurrentToolNozzle(), ack_second_value()+0.5);
      for(TOOL i = BED; i < HEATER_NUM; i++)
      {
        if(ack_seen(toolID[i])) 
        {
          heatSetCurrentTemp(i, ack_value()+0.5);
          heatSyncTargetTemp(i, ack_second_value()+0.5);
        }      
      }
    }
    else if(ack_seen("B:"))		
    {
      heatSetCurrentTemp(BED,ack_value()+0.5);
      heatSyncTargetTemp(BED, ack_second_value()+0.5);
    }
    else if(ack_seen("Mean:"))
    {
      popupReminder((u8* )"Repeatability Test", (u8 *)dmaL2Cache + ack_index-5);
      //popupReminder((u8* )"Standard Deviation", (u8 *)&infoCmd.queue[infoCmd.index_r].gcode[5]);
    }
    else if(ack_seen("Probe Offset"))
    {
      if(ack_seen("Z"))
      {
        setCurrentOffset(ack_value());
      }
    }
    else if(ack_seen("Count E:")) // parse actual position, response of "M114"
    {
      coordinateSetAxisActualSteps(E_AXIS, ack_value());
    }
    else if (ack_seen("X:"))
    {
      storegantry(0, ack_value());
      if (ack_seen("Y:"))
      {
        storegantry(1, ack_value());

        if (ack_seen("Z:"))
        {
          storegantry(2, ack_value());
        }
      }
    }
    else if(ack_seen(echomagic) && ack_seen(busymagic) && ack_seen("processing"))
    {
      busyIndicator(STATUS_BUSY);
    }
#ifdef ONBOARD_SD_SUPPORT     
    else if(ack_seen(bsdnoprintingmagic) && infoMenu.menu[infoMenu.cur] == menuPrinting)
    {
      infoHost.printing = false;
      completePrinting();
    }
    else if(ack_seen(bsdprintingmagic))
    {
      if(infoMenu.menu[infoMenu.cur] != menuPrinting && !infoHost.printing) {
        infoMenu.menu[++infoMenu.cur] = menuPrinting;
        infoHost.printing=true;
      }
      // Parsing printing data
      // Exampre: SD printing byte 123/12345
      char *ptr;
      u32 position = strtol(strstr(dmaL2Cache, "byte ")+5, &ptr, 10); 
      setPrintCur(position);
//      powerFailedCache(position);
    }    
#endif    
    else if(ack_seen(errormagic))
    {
      ackPopupInfo(errormagic);
    }
    else if(ack_seen(echomagic))
    {
      for(u8 i = 0; i < aCount(ignoreEcho); i++)
      {
        if(strstr(dmaL2Cache, ignoreEcho[i])) goto parse_end;
      }
      ackPopupInfo(echomagic);
    }
  }
  
parse_end:    
  if(ack_cur_src != SERIAL_PORT)
  {
    Serial_Puts(ack_cur_src, dmaL2Cache);
  }
  sendGcodeTerminalCache(dmaL2Cache, TERMINAL_ACK);
}

void parseRcvGcode(void)
{
  #ifdef SERIAL_PORT_2
    uint8_t i = 0;
    for(i = 0; i < _USART_CNT; i++)
    {
      if(i != SERIAL_PORT && infoHost.rx_ok[i] == true)
      {
        infoHost.rx_ok[i] = false;
        syncL2CacheFromL1(i);
        storeCmdFromUART(i, dmaL2Cache);
      }
    }
  #endif
}
