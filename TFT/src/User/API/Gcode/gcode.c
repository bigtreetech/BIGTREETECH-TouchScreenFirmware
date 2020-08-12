#include "gcode.h"
#include "includes.h"

REQUEST_COMMAND_INFO requestCommandInfo;
bool WaitingGcodeResponse=0;

static void resetRequestCommandInfo(void)
{
  requestCommandInfo.cmd_rev_buf = malloc(CMD_MAX_REV);
  while(!requestCommandInfo.cmd_rev_buf); // malloc failed
  memset(requestCommandInfo.cmd_rev_buf,0,CMD_MAX_REV);
  requestCommandInfo.inWaitResponse = true;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.done = false;
  requestCommandInfo.inError = false;
}

bool RequestCommandInfoIsRunning(void)
{
   return WaitingGcodeResponse;  //i try to use requestCommandInfo.done but does not work as expected ...
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
  strcpy(requestCommandInfo.command,"M21\n");
  strcpy(requestCommandInfo.startMagic,"SD");
  strcpy(requestCommandInfo.stopMagic,"card ok");
  strcpy(requestCommandInfo.nosdMagic,"No SD card");
  strcpy(requestCommandInfo.errorMagic,"volume.init failed");

  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  // Wait for response
  WaitingGcodeResponse = 1;
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  WaitingGcodeResponse = 0;
  clearRequestCommandInfo();
  // Check reponse
  return !requestCommandInfo.inError;
}

/*
SENDING:M20
Begin file list
PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~3.GCO 11081207
/YEST~1/TEST2/PI3MK2~2.GCO 11081207
/YEST~1/TEST2/PI3MK2~4.GCO 11081207
/YEST~1/TEST2/PI3MK2~5.GCO 11081207
/YEST~1/PI3MK2~1.GCO 11081207
/YEST~1/PI3MK2~3.GCO 11081207
/YEST~1/PI3MK2~2.GCO 11081207
End file list
*/
char *request_M20(void)
{
  strcpy(requestCommandInfo.command,"M20\n");
  strcpy(requestCommandInfo.startMagic,"Begin file list");
  strcpy(requestCommandInfo.stopMagic,"End file list");
  strcpy(requestCommandInfo.errorMagic,"Error");
  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  // Wait for response
  WaitingGcodeResponse = 1;
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  WaitingGcodeResponse = 0;
  //clearRequestCommandInfo(); //shall be call after copying the buffer ...
  return requestCommandInfo.cmd_rev_buf;
}


/*
 * M33 retrieve long filename from short file name
 *   M33 miscel~1/armchair/armcha~1.gco
 * Output:
 *   /Miscellaneous/Armchair/Armchair.gcode
*/
char * request_M33(char *filename)
{
  sprintf(requestCommandInfo.command, "M33 %s\n",filename);
  strcpy(requestCommandInfo.startMagic,"/"); //a character that is in the line to be treated
  strcpy(requestCommandInfo.stopMagic,"ok");
  strcpy(requestCommandInfo.errorMagic,"Cannot open subdir");
  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  // Wait for response
  WaitingGcodeResponse = 1;
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  WaitingGcodeResponse = 0;
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
 **/
long request_M23(char *filename)
{
  sprintf(requestCommandInfo.command, "M23 %s\n",filename);
  strcpy(requestCommandInfo.startMagic, "File opened");
  strcpy(requestCommandInfo.stopMagic,"File selected");
  strcpy(requestCommandInfo.errorMagic,"open failed");
  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  // Wait for response
  WaitingGcodeResponse = 1;
  while (!requestCommandInfo.done)
  {
    loopProcess();
  }
  WaitingGcodeResponse = 0;
  // Find file size and report its.
  char *ptr;
  long size = strtol(strstr(requestCommandInfo.cmd_rev_buf,"Size:")+5, &ptr, 10);
  clearRequestCommandInfo();
  return size;
}

/**
 * Start o resume print
 **/
bool request_M24(int pos)
{
  if(pos == 0){
      mustStoreCmd("M24\n");
  } else {
      char command[100];
      sprintf(command, "M24 S%d\n",pos);
      mustStoreCmd(command);
  }
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
  mustStoreCmd("M25\n");
  return true;
}

/**
 * Print status ( start auto report)
 * ->  SD printing byte 123/12345
 * ->  Not SD printing
 **/
bool request_M27(int seconds)
{
  char command[10];
  sprintf(command, "M27 S%d\n",seconds);
  mustStoreCmd(command);
  return true;
}
