#include "gcode.h"
#include "includes.h"

REQUEST_COMMAND_INFO requestCommandInfo = {0};

static void resetRequestCommandInfo(
  const char *string_start,   // The magic to identify the start
  const char *string_stop,    // The magic to identify the stop
  const char *string_error0,  // The first magic to identify the error response
  const char *string_error1,  // The second error magic
  const char *string_error2   // The third error magic
)
{
  requestCommandInfo.cmd_rev_buf = malloc(CMD_MAX_REV);
  while (!requestCommandInfo.cmd_rev_buf)
    ; // malloc failed
  memset(requestCommandInfo.cmd_rev_buf, 0, CMD_MAX_REV);
  requestCommandInfo.startMagic = string_start;
  requestCommandInfo.stopMagic = string_stop;
  requestCommandInfo.errorMagic[0] = string_error0;
  requestCommandInfo.errorMagic[1] = string_error1;
  requestCommandInfo.errorMagic[2] = string_error2;
  if (string_error0)
    requestCommandInfo.error_num = 1;
  if (string_error1)
    requestCommandInfo.error_num = 2;
  if (string_error2)
    requestCommandInfo.error_num = 3;

  while (infoCmd.count || infoHost.wait)
  {
    loopProcess();  // Wait for the communication to be clean before requestCommand
  }

  requestCommandInfo.inWaitResponse = true;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.done = false;
  requestCommandInfo.inError = false;
}

bool requestCommandInfoIsRunning(void)
{
  return (requestCommandInfo.inWaitResponse || requestCommandInfo.inResponse);
}

void clearRequestCommandInfo(void)
{
  free(requestCommandInfo.cmd_rev_buf);
}

/*
    Send M21 command and wait for response

    >>> M21
    SENDING:M21
    echo:SD card ok
    echo:No SD card

*/
bool request_M21(void)
{
  const char * sdString = (infoMachineSettings.firmwareType == FW_REPRAPFW) ? "SDHC card " : "SD card ";

  resetRequestCommandInfo(sdString,               // The magic to identify the start
                          "ok",                   // The magic to identify the stop
                          "No SD card",           // The first magic to identify the error response
                          "SD init fail",         // The second error magic
                          "volume.init failed");  // The third error magic

  mustStoreCmd("M21\n");

  // Wait for response
  while (!requestCommandInfo.done) { loopProcess(); }
  clearRequestCommandInfo();
  // Check reponse
  return !requestCommandInfo.inError;
}

char *request_M20(void)
{
  resetRequestCommandInfo("Begin file list",  // The magic to identify the start
                          "End file list",    // The magic to identify the stop
                          "Error",            // The first magic to identify the error response
                          NULL,               // The second error magic
                          NULL);              // The third error magic

  mustStoreCmd("M20\n");

  // Wait for response
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  //clearRequestCommandInfo();  //shall be call after copying the buffer ...
  return requestCommandInfo.cmd_rev_buf;
}

/*
 * M33 retrieve long filename from short file name
 *   M33 miscel~1/armchair/armcha~1.gco
 * Output:
 *   /Miscellaneous/Armchair/Armchair.gcode
*/
char *request_M33(char *filename)
{
  resetRequestCommandInfo("/",                   // The magic to identify the start
                          "ok",                  // The magic to identify the stop
                          "Cannot open subdir",  // The first magic to identify the error response
                          NULL,                  // The second error magic
                          NULL);                 // The third error magic

  if (filename[0] != '/')
    mustStoreCmd("M33 /%s\n", filename); // append '/' to short file path
  else
    mustStoreCmd("M33 %s\n", filename);

  // Wait for response
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  //clearRequestCommandInfo(); //shall be call after copying the buffer ...
  return requestCommandInfo.cmd_rev_buf;
}

/**
 * Select the file to print
 *
 * >>> m23 YEST~1/TEST2/PI3MK2~5.GCO
 * SENDING:M23 YEST~1/TEST2/PI3MK2~5.GCO
 * echo:Now fresh file: YEST~1/TEST2/PI3MK2~5.GCO
 * File opened: PI3MK2~5.GCO Size: 11081207
 * File selected
 *
 * file information in RepRapFirmware
 * SENDING:M36 3DBenchy.gcode
 * echo: {"err":0,"size":2758088,"lastModified":"2020-10-20T17:12:18","height":49.00,"firstLayerHeight":0.20,"layerHeight":0.20,"printTime":6173,"filament":[4065.3],"generatedBy":"SuperSlicer 2.2.53 on 2020-10-20 at 15:12:18 UTC"}
 **/
long request_M23_M36(char *filename)
{
  uint8_t offset = 5;
  const char *sizeTag;
  if (infoMachineSettings.firmwareType != FW_REPRAPFW)  // all other firmwares except reprap firmware
  {
    resetRequestCommandInfo("File opened",    // The magic to identify the start
                            "File selected",  // The magic to identify the stop
                            "open failed",    // The first magic to identify the error response
                            NULL,             // The second error magic
                            NULL);            // The third error magic

    mustStoreCmd("M23 %s\n", filename);
    sizeTag = "Size:";
  }
  else // reprap firmware
  {
    resetRequestCommandInfo("{\"err\"",  // The magic to identify the start
                            "}",         // The magic to identify the stop
                            "Error:",    // The first magic to identify the error response
                            NULL,        // The second error magic
                            NULL);       // The third error magic

    mustStoreCmd("M36 %s\n", filename);
    offset = 6;
    sizeTag = "size\":";  // reprap firmware reports size JSON
  }

  // Wait for response
  while (!requestCommandInfo.done) loopProcess();
  if (requestCommandInfo.inError)
  {
    clearRequestCommandInfo();
    return 0;
  }
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
    mustStoreCmd("M23 %s\n", filename);  //send M23 for reprap firmware
  // Find file size and report its.
  char *ptr;
  long size = strtol(strstr(requestCommandInfo.cmd_rev_buf, sizeTag) + offset, &ptr, 10);
  clearRequestCommandInfo();
  return size;
}

/**
 * Start o resume print
 **/
bool request_M24(int pos)
{
  if (pos == 0)
    mustStoreCmd("M24\n");
  else
    mustStoreCmd("M24 S%d\n", pos);
  return true;
}

/**
 * Abort print
 **/
bool request_M524(void)
{
  mustStoreCmd("M524\n");
  return true;
}

/**
 * Pause print
 **/
bool request_M25(void)
{
  mustStoreCmd("M25 P1\n");
  return true;
}

/**
 * Print status ( start auto report)
 * ->  SD printing byte 123/12345
 * ->  Not SD printing
 **/
bool request_M27(int seconds)
{
  mustStoreCmd("M27 S%d\n", seconds);
  return true;
}

/**
 * Park Head / Pause Print
 **/
bool request_M125(void)
{
  mustStoreCmd("M125 P1\n");
  return true;
}

/**
 * Stop or Unconditional stop in reprap firmware
 **/
bool request_M0(void)
{
  mustStoreCmd("M0 \n");
  return true;
}

void send_and_wait_M20(const char* command)
{
  uint32_t timeout = ((uint32_t)0x000FFFFF);
  uint32_t waitloops = ((uint32_t)0x00000006);

  resetRequestCommandInfo("{", "}", "Error:", "Error:", "Error:");
  mustStoreCmd(command);
  while ((strstr(requestCommandInfo.cmd_rev_buf, "dir") == NULL) && (waitloops > 0x00)) //(!find_part("dir"))
  {
    waitloops--;
    timeout = ((uint32_t)0x0000FFFF);
    while ((!requestCommandInfo.done) && (timeout > 0x00))
    {
      loopBackEnd();
            timeout--;
    }
    if (timeout <= 0x00)
    {
      uint16_t wIndex = (dmaL1Data[SERIAL_PORT].wIndex == 0) ? ACK_MAX_SIZE : dmaL1Data[SERIAL_PORT].wIndex;
      if (dmaL1Data[SERIAL_PORT].cache[wIndex - 1] == '}') // \n fehlt
      {
        BUZZER_PLAY(sound_notify); // for DEBUG
        dmaL1Data[SERIAL_PORT].cache[wIndex] = '\n';
        dmaL1Data[SERIAL_PORT].cache[wIndex + 1] = 0;
        dmaL1Data[SERIAL_PORT].wIndex++;
        infoHost.rx_ok[SERIAL_PORT] = true;
      }
    }
    if (dmaL1NotEmpty(SERIAL_PORT) && !infoHost.rx_ok[SERIAL_PORT])
    {
      infoHost.rx_ok[SERIAL_PORT] = true;
    }
    if (strstr(requestCommandInfo.cmd_rev_buf, "dir") == NULL)
    {
      clearRequestCommandInfo();
      resetRequestCommandInfo("{", "}", "Error:", "Error:", "Error:");
      mustStoreCmd("\n");
    }
  }
  return; //  requestCommandInfo.cmd_rev_buf;
}

char *request_M20_macros(char *nextdir)
{
  // set pause Flag
  //infoHost.pauseGantry = true;
  // waitPortReady();
  clearRequestCommandInfo();
  char command[256];
  if ((nextdir == NULL) || strchr(nextdir, '/') == NULL)
  {
    strncpy(command, "M20 S2 P\"/macros\"\n", 256);
  }
  else
  {
    snprintf(command, 256, "M20 S2 P\"/macros/\"%s\n\n", nextdir);
  }
  // Send GCode and wait for responce
  send_and_wait_M20(command);
  // reset pause Flag
  //infoHost.pauseGantry = false;
  GUI_Clear(BACKGROUND_COLOR);
  return requestCommandInfo.cmd_rev_buf;
}

bool request_M98(char *filename)
{
  char command[256];
  snprintf(command, 256, "M98 P/macros/%s\n", filename);
  resetRequestCommandInfo("", "ok", "Warning:", "Warning:", "Warning:");
  mustStoreCmd(command);
  // Wait for response
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  return true;
}