#include "gcode.h"
#include "includes.h"

REQUEST_COMMAND_INFO requestCommandInfo = {0};

bool isWaitingResponse(void)
{
  return (!requestCommandInfo.done);
}

bool requestCommandInfoIsRunning(void)
{
  return (requestCommandInfo.inWaitResponse || requestCommandInfo.inResponse);
}

void clearRequestCommandInfo(void)
{
  if (requestCommandInfo.cmd_rev_buf != NULL)
  {
    free(requestCommandInfo.cmd_rev_buf);
    requestCommandInfo.cmd_rev_buf = NULL;
  }
}

static void resetRequestCommandInfo(
  const char *string_start,   // The magic to identify the start
  const char *string_stop,    // The magic to identify the stop
  const char *string_error0,  // The first magic to identify the error response
  const char *string_error1,  // The second error magic
  const char *string_error2   // The third error magic
)
{
  clearRequestCommandInfo();  // release requestCommandInfo.cmd_rev_buf before allocating a new one

  requestCommandInfo.cmd_rev_buf = malloc(CMD_MAX_REV);
  while (!requestCommandInfo.cmd_rev_buf)
    ;  // malloc failed
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

  loopProcessToCondition(&isNotEmptyCmdQueue);  // wait for the communication to be clean before requestCommand

  requestCommandInfo.inWaitResponse = true;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.done = false;
  requestCommandInfo.inError = false;
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
  const char * sdString = (infoMachineSettings.firmwareType == FW_REPRAPFW) ? "card mounted " : "SD card ";
  const char * errString1 = (infoMachineSettings.firmwareType == FW_REPRAPFW) ? "Error" : "No SD card";

  resetRequestCommandInfo(sdString,               // The magic to identify the start
                          "ok",                   // The magic to identify the stop
                          errString1,             // The first magic to identify the error response
                          "SD init fail",         // The second error magic
                          "volume.init failed");  // The third error magic

  mustStoreCmd("M21\n");

  // Wait for response
  loopProcessToCondition(&isWaitingResponse);

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
  loopProcessToCondition(&isWaitingResponse);

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
    mustStoreCmd("M33 /%s\n", filename);  // append '/' to short file path
  else
    mustStoreCmd("M33 %s\n", filename);

  // Wait for response
  loopProcessToCondition(&isWaitingResponse);

  //clearRequestCommandInfo();  //shall be call after copying the buffer ...
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
  loopProcessToCondition(&isWaitingResponse);

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
 * Start or resume print
 **/
void request_M24(int pos)
{
  if (pos == 0)
    mustStoreCmd("M24\n");
  else
    mustStoreCmd("M24 S%d\n", pos);
}

/**
 * Abort print
 **/
void request_M524(void)
{
  mustStoreCmd("M524\n");
}

/**
 * Pause print
 **/
void request_M25(void)
{
  mustStoreCmd("M25 P1\n");
}

/**
 * @brief Print status (start auto report)
 * @param seconds: Pass 0 to disable auto report. Pass delay in seconds
 *                 for auto query if available in marlin.
 *
 * ->  SD printing byte 123/12345
 * ->  Not SD printing
 */
void request_M27(uint8_t seconds)
{
  if (infoMachineSettings.autoReportSDStatus == 1)
    mustStoreCmd("M27 S%d\n", seconds);
}

/**
 * Park Head / Pause Print
 **/
void request_M125(void)
{
  mustStoreCmd("M125 P1\n");
}

/**
 * Stop or Unconditional stop in reprap firmware
 **/
void request_M0(void)
{
  mustStoreCmd("M0\n");
}

void request_M98(char *filename)
{
  char command[CMD_MAX_CHAR];
  snprintf(command, CMD_MAX_CHAR, "M98 P/%s\n", filename);
  rrfStatusSetMacroBusy();
  mustStoreCmd(command);
  // prevent a race condition when rrfStatusQuery returns !busy before executing the macro
  while (isEnqueued(command))
  {
    loopProcess();
  }
  rrfStatusQueryFast();

  // Wait for macro to complete
  loopProcessToCondition(&rrfStatusIsBusy);
  rrfStatusQueryNormal();
}

// nextdir path must start with "macros"
char *request_M20_macros(char *nextdir)
{
  resetRequestCommandInfo("{", "}", "Error:", NULL, NULL);

  mustStoreCmd("M20 S2 P\"/%s\"\n", nextdir);

  // Wait for response
  loopProcessToCondition(&isWaitingResponse);

  //clearRequestCommandInfo();  //shall be call after copying the buffer ...
  return requestCommandInfo.cmd_rev_buf;
}
