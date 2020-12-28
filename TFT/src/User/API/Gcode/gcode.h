#ifndef _GCODE_H_
#define _GCODE_H_
#include "stdbool.h"
#include "interfaceCmd.h"

#define CMD_MAX_REV     5000

#define MAX_ERROR_NUM   3
typedef struct {
  char *cmd_rev_buf;     // buffer where store the command response
  const char *startMagic; // The magic to identify the start
  const char *stopMagic;  // The magic to identify the stop
  const char *errorMagic[MAX_ERROR_NUM];
                         // The magic to identify the error response
                         // Some Gcode respond to multiple errors, such as:M21 -- "SD card failed", "No SD card", "volume.init failed"
  uint8_t error_num;     // Number of error magic corresponding to current Gcode
  bool inResponse;       // true if between start and stop magic
  bool inWaitResponse;   // true if waiting for start magic
  bool done;             // true if command is executed and response is received
  bool inError;          // true if error response
} REQUEST_COMMAND_INFO;

extern REQUEST_COMMAND_INFO requestCommandInfo;

void clearRequestCommandInfo(void);
bool requestCommandInfoIsRunning(void);
bool request_M21(void);
char * request_M20(void);
char * request_M33(char *filename);
bool request_M25(void);
bool request_M27(int seconds);
bool request_M524(void);
bool request_M24(int pos);
long request_M23(char *filename);

#endif
