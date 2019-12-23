#ifndef _GCODE_H_
#define _GCODE_H_
#include "stdbool.h"
#include "interfaceCmd.h"

#define CMD_MAX_REV     5000

typedef struct {
    char command[CMD_MAX_CHAR];     // The command sent to printer
    char startMagic[CMD_MAX_CHAR];  // The magic to identify the start
    char stopMagic[CMD_MAX_CHAR];   // The magic to identify the stop
    char errorMagic[CMD_MAX_CHAR];  // The magic to identify the error response
    bool inResponse;                // true if between start and stop magic
    bool inWaitResponse;            // true if waiting for start magic
    bool done;                      // true if command is executed and response is received
    bool inError;                   // true if error response
    char *cmd_rev_buf;              // buffer where store the command response
} REQUEST_COMMAND_INFO;

extern REQUEST_COMMAND_INFO requestCommandInfo;

void clearRequestCommandInfo(void);
bool RequestCommandInfoIsRunning(void);
bool request_M21(void);
char * request_M20(void);
char * request_M33(char *filename);
bool request_M25(void);
bool request_M27(int seconds);
bool request_M524(void);
bool request_M24(int pos);
long request_M23(char *filename);

#endif
