#include "gcode.h"
#include "includes.h"
#include "RRFStatusControl.h"

REQUEST_COMMAND_INFO requestCommandInfo = {0};

static void waitForResponse(void)
{
  TASK_LOOP_WHILE(!requestCommandInfo.done);
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

void abortRequestCommandInfo(void)
{
  requestCommandInfo.inWaitResponse = false;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.done = true;
  requestCommandInfo.inError = true;
}

static void resetRequestCommandInfo(
  const char * string_start,   // the magic to identify the start
  const char * string_stop,    // the magic to identify the stop
  const char * string_error0,  // the first magic to identify the error response
  const char * string_error1,  // the second error magic
  const char * string_error2   // the third error magic
)
{
  clearRequestCommandInfo();  // release requestCommandInfo.cmd_rev_buf before allocating a new one

  requestCommandInfo.cmd_rev_buf = malloc(CMD_MAX_REV);

  while (!requestCommandInfo.cmd_rev_buf)  // if malloc failed, block the TFT
  {
  }

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

  TASK_LOOP_WHILE(isNotEmptyCmdQueue());  // wait for the communication to be clean

  requestCommandInfo.stream_handler = NULL;
  requestCommandInfo.inWaitResponse = true;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.done = false;
  requestCommandInfo.inError = false;
}

void detectAdvancedOk(void)
{
  uint8_t advanced_ok = GET_BIT(infoSettings.general_settings, INDEX_ADVANCED_OK);  // backup the configured ADVANCED_OK setting
  uint8_t cmd_index = 0;

  // temporary disable the ADVANCED_OK feature (if enabled) just to allow the TFT to send only one gcode
  // per time and the mainboard to reply with an ADVANCED_OK response with the maximum available buffers
  SET_BIT_OFF(infoSettings.general_settings, INDEX_ADVANCED_OK);

  TASK_LOOP_WHILE(isPendingCmd() && isNotEmptyCmdQueue());  // wait for the communication to be clean

  resetRequestCommandInfo("ok",   // the magic to identify the start
                          "ok",   // the magic to identify the stop
                          NULL,   // the first magic to identify the error response
                          NULL,   // the second error magic
                          NULL);  // the third error magic

  // send any gcode replied by the mainboard with a regular OK response ("ok\n") or an ADVANCED_OK response (e.g. "ok N10 P15 B3\n")
  mustStoreCmd("M220\n");

  waitForResponse();  // wait for response

  while (requestCommandInfo.cmd_rev_buf[cmd_index] != '\0')
  {
    if (requestCommandInfo.cmd_rev_buf[cmd_index++] == 'B')
    {
      if (strtol(&requestCommandInfo.cmd_rev_buf[cmd_index], NULL, 10) != 0)  // if different than 0
      {
        // set infoHost.target_tx_slots and infoSettings.tx_slots to the value detected by TFT
        infoHost.target_tx_slots = infoSettings.tx_slots = strtol(&requestCommandInfo.cmd_rev_buf[cmd_index], NULL, 10);
      }
    }
  }

  clearRequestCommandInfo();

  SET_BIT_VALUE(infoSettings.general_settings, INDEX_ADVANCED_OK, advanced_ok);  // restore the configured ADVANCED_OK setting
}

/**
 * Send M21 command and wait for response
 *
 * SENDING: M21
 * echo:SD card ok
 * echo:No SD card
 */
bool request_M21(void)
{
  resetRequestCommandInfo("SD card ",             // the magic to identify the start
                          "ok",                   // the magic to identify the stop
                          "No SD card",           // the first magic to identify the error response
                          "SD init fail",         // the second error magic
                          "volume.init failed");  // the third error magic

  mustStoreCmd((infoMachineSettings.multiVolume == ENABLED) ? ((infoFile.onboardSource == BOARD_SD) ? "M21 S\n" : "M21 U\n") : "M21\n");

  waitForResponse();  // wait for response

  clearRequestCommandInfo();

  // check reponse
  return !requestCommandInfo.inError;
}

char * request_M20(void)
{
  resetRequestCommandInfo("Begin file list",  // the magic to identify the start
                          "End file list",    // the magic to identify the stop
                          "Error",            // the first magic to identify the error response
                          NULL,               // the second error magic
                          NULL);              // the third error magic

  if (infoMachineSettings.longFilename == ENABLED)  // if long filename is supported
    mustStoreCmd("M20 L\n");  // L option is supported since Marlin 2.0.9
  else
    mustStoreCmd("M20\n");

  waitForResponse();  // wait for response

  //clearRequestCommandInfo();  // shall be call after copying the buffer
  return requestCommandInfo.cmd_rev_buf;
}

/**
 * M33 retrieve long filename from short filename
 *
 * SENDING: M33 miscel~1/armchair/armcha~1.gco
 * /Miscellaneous/Armchair/Armchair.gcode
 */
char * request_M33(const char * filename)
{
  resetRequestCommandInfo("/",                   // the magic to identify the start
                          "ok",                  // the magic to identify the stop
                          "Cannot open subdir",  // the first magic to identify the error response
                          NULL,                  // the second error magic
                          NULL);                 // the third error magic

  if (filename[0] != '/')
    mustStoreCmd("M33 /%s\n", filename);  // append '/' to short file path
  else
    mustStoreCmd("M33 %s\n", filename);

  waitForResponse();  // wait for response

  //clearRequestCommandInfo();  // shall be call after copying the buffer
  return requestCommandInfo.cmd_rev_buf;
}

/**
 * Select the file to print
 *
 * SENDING: M23 YEST~1/TEST2/PI3MK2~5.GCO
 * echo:Now fresh file: YEST~1/TEST2/PI3MK2~5.GCO
 * File opened: PI3MK2~5.GCO Size: 11081207
 * File selected
 *
 * File information in RepRapFirmware
 *
 * SENDING: M36 3DBenchy.gcode
 * echo: {"err":0,"size":2758088,"lastModified":"2020-10-20T17:12:18","height":49.00,"firstLayerHeight":0.20,"layerHeight":0.20,"printTime":6173,"filament":[4065.3],"generatedBy":"SuperSlicer 2.2.53 on 2020-10-20 at 15:12:18 UTC"}
 */
long request_M23_M36(const char * filename)
{
  long size = 0;  // initialize to 0 in case of error
  const char * sizeTag;
  char * strPtr;

  if (infoMachineSettings.firmwareType != FW_REPRAPFW)  // all other firmwares except RepRap firmware
  {
    resetRequestCommandInfo("File opened",    // the magic to identify the start
                            "File selected",  // the magic to identify the stop
                            "open failed",    // the first magic to identify the error response
                            NULL,             // the second error magic
                            NULL);            // the third error magic

    // skip source and first "/" character (e.g. "oMD:/sub_dir/cap2.gcode" -> "sub_dir/cap2.gcode")
    mustStoreCmd("M23 %s\n", filename + strlen(getFS()) + 1);

    sizeTag = "Size:";
  }
  else  // RepRap firmware
  {
    resetRequestCommandInfo("{\"err\"",  // the magic to identify the start
                            "}",         // the magic to identify the stop
                            "Error:",    // the first magic to identify the error response
                            NULL,        // the second error magic
                            NULL);       // the third error magic

    mustStoreCmd("M36 /%s\n", filename);

    sizeTag = "size\":";  // RepRap firmware reports size JSON
  }

  waitForResponse();  // wait for response

  if (requestCommandInfo.inError)
  {
    clearRequestCommandInfo();

    return size;
  }

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
    mustStoreCmd("M23 /%s\n", filename);  // send M23 for RepRap firmware

  // find file size and report it
  strPtr = strstr(requestCommandInfo.cmd_rev_buf, sizeTag);

  if (strPtr != NULL)
    size = strtol(strPtr + strlen(sizeTag), NULL, 10);

  clearRequestCommandInfo();

  return size;
}

/**
 * Start or resume print
 */
void request_M24(int pos)
{
  if (pos == 0)
    mustStoreCmd("M24\n");
  else
    mustStoreCmd("M24 S%d\n", pos);
}

/**
 * Pause print
 */
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
 */
void request_M125(void)
{
  mustStoreCmd("M125 P1\n");
}

/**
 * Stop or Unconditional stop in RepRap firmware
 */
void request_M0(void)
{
  mustStoreCmd("M0\n");
}

void request_M98(const char * filename)
{
  CMD command;

  snprintf(command, CMD_MAX_SIZE, "M98 P/%s\n", filename);
  rrfStatusSetMacroBusy();

  mustStoreCmd(command);

  // prevent a race condition when rrfStatusQuery returns !busy before executing the macro
  TASK_LOOP_WHILE(isEnqueued(command));

  rrfStatusQueryFast();

  // wait for macro to complete
  TASK_LOOP_WHILE(rrfStatusIsBusy());

  rrfStatusQueryNormal();
}

// nextdir path must start with "macros" or "gcodes"
void request_M20_rrf(const char * nextdir, bool with_ts, FP_STREAM_HANDLER handler)
{
  resetRequestCommandInfo("{", "}", "Error:", NULL, NULL);
  requestCommandInfo.stream_handler = handler;

  mustStoreCmd("M20 S%d P\"/%s\"\n", with_ts ? 3 : 2, nextdir);

  waitForResponse();  // wait for response
}
