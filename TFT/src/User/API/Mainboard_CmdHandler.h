#ifndef _MAINBOARD_CMD_HANDLER_H_
#define _MAINBOARD_CMD_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "SerialConnection.h"

#define CMD_MAX_SIZE 100  // including ending character '\0'

#define handleCmd(...)               _handleCmd(__VA_ARGS__, PORT_1)
#define _handleCmd(a, b, ...)        handleCmd(a, b)
#define sendEmergencyCmd(...)        _sendEmergencyCmd(__VA_ARGS__, PORT_1)
#define _sendEmergencyCmd(a, b, ...) sendEmergencyCmd(a, b)

typedef char CMD[CMD_MAX_SIZE];

// used by Monitoring menu available in Notification menu only
// if DEBUG_MONITORING is enabled in Configuration.h
uint8_t getQueueCount(void);

bool isPendingCmd(void);
bool isFullCmdQueue(void);
bool isNotEmptyCmdQueue(void);
bool isEnqueued(const CMD cmd);
bool isWritingMode(void);

bool storeCmd(const char * format, ...);
void mustStoreCmd(const char * format, ...);
void mustStoreScript(const char * format, ...);
bool storeCmdFromUART(const CMD cmd, const SERIAL_PORT_INDEX portIndex);
void clearCmdQueue(void);
void handleCmdLineNumberMismatch(const uint32_t lineNumber);
void handleCmd(CMD cmd, const SERIAL_PORT_INDEX portIndex);
void sendEmergencyCmd(const CMD emergencyCmd, const SERIAL_PORT_INDEX portIndex);
void sendQueueCmd(void);

#ifdef __cplusplus
}
#endif

#endif
