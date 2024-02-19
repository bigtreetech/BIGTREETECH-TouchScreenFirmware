#ifndef _GCODE_H_
#define _GCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define CMD_MAX_REV   5000
#define MAX_ERROR_NUM 3

typedef void (* FP_STREAM_HANDLER)(const char *);

typedef struct
{
  char * cmd_rev_buf;       // Buffer where store the command response
  const char * startMagic;  // The magic to identify the start
  const char * stopMagic;   // The magic to identify the stop
  const char * errorMagic[MAX_ERROR_NUM];
  // The magic to identify the error response
  // Some gcodes respond to multiple errors, such as M21 - "SD card failed", "No SD card", "volume.init failed"
  uint8_t error_num;       // Number of error magic corresponding to current gcode
  bool inResponse;         // true if between start and stop magic
  bool inWaitResponse;     // true if waiting for start magic
  bool done;               // true if command is executed and response is received
  bool inError;            // true if error response
  bool inJson;             // true if !inResponse and !inWaitResponse and '{' is found
  FP_STREAM_HANDLER stream_handler;
} REQUEST_COMMAND_INFO;

extern REQUEST_COMMAND_INFO requestCommandInfo;

bool requestCommandInfoIsRunning(void);
void clearRequestCommandInfo(void);
void abortRequestCommandInfo(void);

void detectAdvancedOk(void);
bool request_M21(void);
char * request_M20(void);
char * request_M33(const char * filename);
long request_M23_M36(const char * filename);
void request_M24(int pos);
void request_M25(void);
void request_M27(uint8_t seconds);
void request_M125(void);
void request_M0(void);
void request_M98(const char * filename);
void request_M20_rrf(const char * dir, bool with_ts, FP_STREAM_HANDLER handler);

#ifdef __cplusplus
}
#endif

#endif
