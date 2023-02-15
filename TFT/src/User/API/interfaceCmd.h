#ifndef _INTERFACE_CMD_H_
#define _INTERFACE_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "SerialConnection.h"

#define CMD_MAX_SIZE 100  // including ending character '\0'

#define sendEmergencyCmd(...) _sendEmergencyCmd(__VA_ARGS__, PORT_1)
#define _sendEmergencyCmd(a, b, ...) sendEmergencyCmd(a, b)
#define storeEmergencyCmd(...) _storeEmergencyCmd(__VA_ARGS__, PORT_1)
#define _storeEmergencyCmd(a, b, ...) storeEmergencyCmd(a, b)
#define handleEmergencyCmd(...) _handleEmergencyCmd(__VA_ARGS__, PORT_1)
#define _handleEmergencyCmd(a, b, ...) handleEmergencyCmd(a, b)
#define handleCmd(...) _handleCmd(__VA_ARGS__, PORT_1)
#define _handleCmd(a, b, ...) handleCmd(a, b)

typedef char CMD[CMD_MAX_SIZE];

bool isFullCmdQueue(void);       // also usable as condition callback for loopProcessToCondition()
bool isNotEmptyCmdQueue(void);   // also usable as condition callback for loopProcessToCondition()
bool isEnqueued(const CMD cmd);
bool isWritingMode(void);

void storeEmergencyCmd(const CMD emergencyCmd, const SERIAL_PORT_INDEX portIndex);
void sendEmergencyCmd(const CMD emergencyCmd, const SERIAL_PORT_INDEX portIndex);
void handleEmergencyCmd(const CMD emergencyCmd, const SERIAL_PORT_INDEX portIndex);
bool storeCmd(const char * format, ...);
void mustStoreCmd(const char * format, ...);
void mustStoreScript(const char * format, ...);
bool storeCmdFromUART(const CMD cmd, const SERIAL_PORT_INDEX portIndex);
void mustStoreCacheCmd(const char * format, ...);
bool moveCacheToCmd(void);
void clearCmdQueue(void);
void handleCmd(CMD cmd, const SERIAL_PORT_INDEX portIndex);
void sendQueueCmd(void);

#ifdef __cplusplus
}
#endif

#endif
