#include "gcode.h"
#include "includes.h"

REQUEST_COMMAND_INFO requestCommandInfo;
u32 timeout = 0;

void requestInitTimeOut(void)
{
  timeout = OS_GetTime();
}

bool requestHasTimeOut(void)
{
  return ((OS_GetTime() - timeout) > 300);  //3s
}

static void resetRequestCommandInfo(void) 
{
  memset(requestCommandInfo.cmd_rev_buf,0,CMD_MAX_REV);
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
    echo:SD init fail

*/
bool request_M21(void)
{
  strcpy(requestCommandInfo.command,"M21\n");
  strcpy(requestCommandInfo.startMagic,"SD card ok");
  strcpy(requestCommandInfo.stopMagic,"\n");
  strcpy(requestCommandInfo.errorMagic,"Error");

  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  requestInitTimeOut();
  // Wait for response
  while (!requestCommandInfo.done && !requestHasTimeOut())
  {
    loopProcess();
  }
  // Check reponse
  return !requestHasTimeOut();
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
  requestInitTimeOut();
  // Wait for response
  while (!requestCommandInfo.done && !requestHasTimeOut())
  {
    loopProcess();
  }
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
  requestInitTimeOut();
  // Wait for response
  while (!requestCommandInfo.done && !requestHasTimeOut())
  {
    loopProcess();
  }

  // Find file size and report its.
  char *ptr;
  return strtol(strstr(requestCommandInfo.cmd_rev_buf,"Size:")+5, &ptr, 10);
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
