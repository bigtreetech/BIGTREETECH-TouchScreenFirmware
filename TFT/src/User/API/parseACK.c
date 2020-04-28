#include "includes.h"
#include "parseACK.h"

char dmaL2Cache[ACK_MAX_SIZE];
static u16 ack_index=0;
static u8 ack_cur_src = SERIAL_PORT;
int MODEselect;
// Ignore reply "echo:" message (don't display in popup menu)
const char *const ignoreEcho[] = {
  "busy: processing",
  "Now fresh file:",
  "Probe Z Offset:",
  "Flow:",
  "echo:;",
  "echo:  G",
  "echo:  M",
};

bool portSeen[_UART_CNT] = {false, false, false, false, false, false};

void setCurrentAckSrc(uint8_t src)
{
  ack_cur_src = src;
  portSeen[src] = true;
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
  if(infoMenu.menu[infoMenu.cur] == menuParameterSettings) return;

  if (info == echomagic)
  {
    statusScreen_setMsg((u8 *)info, (u8 *)dmaL2Cache + ack_index);
  }
  if (infoMenu.menu[infoMenu.cur] == menuTerminal) return;
  if (infoMenu.menu[infoMenu.cur] == menuStatus && info == echomagic) return;

  popupReminder((u8* )info, (u8 *)dmaL2Cache + ack_index);
}

bool dmaL1NotEmpty(uint8_t port)
{
  return dmaL1Data[port].rIndex != dmaL1Data[port].wIndex;
}

void syncL2CacheFromL1(uint8_t port)
{
  uint16_t i = 0;
  for (i = 0; dmaL1NotEmpty(port) && dmaL2Cache[i-1] != '\n'; i++)
  {
    dmaL2Cache[i] = dmaL1Data[port].cache[dmaL1Data[port].rIndex];
    dmaL1Data[port].rIndex = (dmaL1Data[port].rIndex + 1) % DMA_TRANS_LEN;
  }
  dmaL2Cache[i] = 0; // End character
}

void parseACK(void)
{
  bool avoid_terminal = false;
  if(infoHost.rx_ok[SERIAL_PORT] != true) return; //not get response data

  while(dmaL1NotEmpty(SERIAL_PORT))
  {
    syncL2CacheFromL1(SERIAL_PORT);
    infoHost.rx_ok[SERIAL_PORT] = false;

    if(infoHost.connected == false) //not connected to Marlin
    {
      if(!ack_seen("T:") && !ack_seen("T0:"))  goto parse_end;  //the first response should be such as "T:25/50\n"
        updateNextHeatCheckTime();
        infoHost.connected = true;
        storeCmd("M115\n");
        storeCmd("M503 S0\n");
        storeCmd("M92\n"); // Steps/mm of extruder is an important parameter for Smart filament runout
                           // Avoid can't getting this parameter due to disabled M503 in Marlin
    }

    // Gcode command response
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
      // parse temperature
      if(ack_seen("T:") || ack_seen("T0:"))
      {
        TOOL i = heatGetCurrentToolNozzle();
        heatSetCurrentTemp(i, ack_value()+0.5);
        if(!heatGetSendWaiting(i)){
          heatSyncTargetTemp(i, ack_second_value()+0.5);
        }
        for(TOOL i = BED; i < HEATER_NUM; i++)
        {
          if(ack_seen(toolID[i]))
          {
            heatSetCurrentTemp(i, ack_value()+0.5);
            if(!heatGetSendWaiting(i)) {
              heatSyncTargetTemp(i, ack_second_value()+0.5);
            }
          }
        }
        avoid_terminal = infoSettings.terminalACK;
        updateNextHeatCheckTime();
      }
      else if(ack_seen("X:") && ack_index == 2)
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
      else if(ack_seen("Count E:")) // Parse actual extruder position, response of "M114 E\n", required "M114_DETAIL" in Marlin
      {
        coordinateSetAxisActualSteps(E_AXIS, ack_value());
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
        // Example: SD printing byte 123/12345
        char *ptr;
        u32 position = strtol(strstr(dmaL2Cache, "byte ")+5, &ptr, 10);
        setPrintCur(position);
  //      powerFailedCache(position);
      }
  #endif
    //parse and store stepper steps/mm values
      else if(ack_seen("M92 X"))
      {
                          setParameter(P_STEPS_PER_MM, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_STEPS_PER_MM, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_STEPS_PER_MM, Z_STEPPER, ack_value());
        if(ack_seen("E")) setParameter(P_STEPS_PER_MM, E_STEPPER, ack_value());
      }
    //parse and store Max Feed Rate values
     else if(ack_seen("M203 X")){
                          setParameter(P_MAX_FEED_RATE, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_MAX_FEED_RATE, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_MAX_FEED_RATE, Z_STEPPER, ack_value());
        if(ack_seen("E")) setParameter(P_MAX_FEED_RATE, E_STEPPER, ack_value());
      }
    //parse and store Max Acceleration values
      else if(ack_seen("M201 X")){
                          setParameter(P_MAX_ACCELERATION, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_MAX_ACCELERATION, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_MAX_ACCELERATION, Z_STEPPER, ack_value());
        if(ack_seen("E")) setParameter(P_MAX_ACCELERATION, E_STEPPER, ack_value());

      }
    //parse and store Acceleration values
      else if(ack_seen("M204 P")){
                          setParameter(P_ACCELERATION, X_STEPPER, ack_value());
        if(ack_seen("R")) setParameter(P_ACCELERATION, Y_STEPPER, ack_value());
        if(ack_seen("T")) setParameter(P_ACCELERATION, Z_STEPPER, ack_value());
      }
    //parse and store Probe Offset values
      else if(ack_seen("M851 X")){
                          setParameter(P_PROBE_OFFSET, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_PROBE_OFFSET, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_PROBE_OFFSET, Z_STEPPER, ack_value());
      }
    //parse and store TMC Bump sensitivity values
      else if(ack_seen("M914 X")){
                          setParameter(P_BUMPSENSITIVITY, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_BUMPSENSITIVITY, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_BUMPSENSITIVITY, Z_STEPPER, ack_value());
      }
    //parse and store stepper driver current values
      else if(ack_seen("M906 X")){
                          setParameter(P_CURRENT, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_CURRENT, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_CURRENT, Z_STEPPER, ack_value());
      }
      else if(ack_seen("M906 I1")){
        if(ack_seen("X")) dualstepper[X_STEPPER] = true;
        if(ack_seen("Y")) dualstepper[Y_STEPPER] = true;
        if(ack_seen("Z")) dualstepper[Z_STEPPER] = true;
      }
      else if(ack_seen("M906 T0 E")){
        setParameter(P_CURRENT, E_STEPPER, ack_value());
      }
      else if(ack_seen("M906 T1 E")){
        setParameter(P_CURRENT, E2_STEPPER, ack_value());
        dualstepper[E_STEPPER] = true;
      }
    // Parse M115 capability report
      else if(ack_seen("Cap:EEPROM:"))
      {
        infoMachineSettings.EEPROM = ack_value();
      }
      else if(ack_seen("Cap:AUTOREPORT_TEMP:"))
      {
        infoMachineSettings.autoReportTemp = ack_value();
      }
      else if(ack_seen("Cap:AUTOLEVEL:"))
      {
        infoMachineSettings.autoLevel = ack_value();
      }
      else if(ack_seen("Cap:Z_PROBE:"))
      {
        infoMachineSettings.zProbe = ack_value();
      }
      else if(ack_seen("Cap:LEVELING_DATA:"))
      {
        infoMachineSettings.levelingData = ack_value();
      }
      else if(ack_seen("Cap:SOFTWARE_POWER:"))
      {
        infoMachineSettings.softwarePower = ack_value();
      }
      else if(ack_seen("Cap:TOGGLE_LIGHTS:"))
      {
        infoMachineSettings.toggleLights = ack_value();
      }
      else if(ack_seen("Cap:CASE_LIGHT_BRIGHTNESS:"))
      {
        infoMachineSettings.caseLightsBrightness = ack_value();
      }
      else if(ack_seen("Cap:EMERGENCY_PARSER:"))
      {
        infoMachineSettings.emergencyParser = ack_value();
      }
      else if(ack_seen("Cap:AUTOREPORT_SD_STATUS:"))
      {
        infoMachineSettings.autoReportSDStatus = ack_value();
      }
      else if(ack_seen("Cap:CHAMBER_TEMPERATURE:"))
      {
        setupMachine();
      }
    //parse Repeatability Test
      else if(ack_seen("Mean:"))
      {
        popupReminder((u8* )"Repeatability Test", (u8 *)dmaL2Cache + ack_index-5);
      }
      else if(ack_seen("Probe Offset"))
      {
        if(ack_seen("Z"))
        {
          setParameter(P_PROBE_OFFSET,Z_STEPPER, ack_value());
        }
      }
      else if (ack_seen(" F0:"))
      {
        fanSetSpeed(0, ack_value());
      }
    // parse and store feed rate percentage
      else if(ack_seen("FR:"))
      {
        speedSetPercent(0,ack_value());
      }
    // parse and store flow rate percentage
      else if(ack_seen("Flow: "))
      {
        speedSetPercent(1,ack_value());
      }
      else if(ack_seen("paused for user"))
      {
        popupPauseForUser();
      }
    //Parse error messages & Echo messages
      else if(ack_seen(errormagic))
      {
        BUZZER_PLAY(sound_error);

        ackPopupInfo(errormagic);
      }
      else if(ack_seen(echomagic))
      {
        for(u8 i = 0; i < COUNT(ignoreEcho); i++)
        {
          if(strstr(dmaL2Cache, ignoreEcho[i]))
          {
            busyIndicator(STATUS_BUSY);
            goto parse_end;
          }
        }
        BUZZER_PLAY(sound_notify);
        ackPopupInfo(echomagic);
      }
    }

  parse_end:
    if(ack_cur_src != SERIAL_PORT)
    {
      Serial_Puts(ack_cur_src, dmaL2Cache);
    }
    else if (!ack_seen("ok"))
    {
      // make sure we pass on spontaneous messages to all connected ports (since these can come unrequested)
      for (int port = 0; port < _UART_CNT; port++)
      {
        if (port != SERIAL_PORT && portSeen[port])
        {
          // pass on this one to anyone else who might be listening
          Serial_Puts(port, dmaL2Cache);
        }
      }
    }

    if (avoid_terminal != true){
      sendGcodeTerminalCache(dmaL2Cache, TERMINAL_ACK);
    }
  }
}

void parseRcvGcode(void)
{
  #ifdef SERIAL_PORT_2
    uint8_t i = 0;
    for(i = 0; i < _UART_CNT; i++)
    {
      if(i != SERIAL_PORT && infoHost.rx_ok[i] == true)
      {
        infoHost.rx_ok[i] = false;
        while(dmaL1NotEmpty(i))
        {
          syncL2CacheFromL1(i);
          storeCmdFromUART(i, dmaL2Cache);
        }
      }
    }
  #endif
}
