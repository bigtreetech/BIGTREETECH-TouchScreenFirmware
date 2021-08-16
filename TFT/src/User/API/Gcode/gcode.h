#ifndef _GCODE_H_
#define _GCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define CMD_MAX_REV   5000
#define MAX_ERROR_NUM 3

typedef struct
{
  char *cmd_rev_buf;       // buffer where store the command response
  const char *startMagic;  // The magic to identify the start
  const char *stopMagic;   // The magic to identify the stop
  const char *errorMagic[MAX_ERROR_NUM];
  // The magic to identify the error response
  // Some Gcode respond to multiple errors, such as:M21 -- "SD card failed", "No SD card", "volume.init failed"
  uint8_t error_num;       // Number of error magic corresponding to current Gcode
  bool inResponse;         // true if between start and stop magic
  bool inWaitResponse;     // true if waiting for start magic
  bool done;               // true if command is executed and response is received
  bool inError;            // true if error response
  bool inJson;             // true if !inResponse and !inWaitResponse and '{' is found
} REQUEST_COMMAND_INFO;

extern REQUEST_COMMAND_INFO requestCommandInfo;

bool isWaitingResponse(void);  // condition callback for loopProcessToCondition()
bool requestCommandInfoIsRunning(void);
void clearRequestCommandInfo(void);

bool request_M21(void);
char *request_M20(void);
char *request_M33(char *filename);
long request_M23_M36(char *filename);
void request_M24(int pos);
void request_M524(void);
void request_M25(void);
void request_M27(uint8_t seconds);
void request_M125(void);
void request_M0(void);
void request_M98(char *filename);
char *request_M20_macros(char *dir);

#ifdef __cplusplus
}
#endif

#endif
