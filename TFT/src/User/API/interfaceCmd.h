#ifndef _INTERFACE_CMD_H_
#define _INTERFACE_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "SerialConnection.h"

#define CMD_MAX_SIZE 100

typedef char CMD[CMD_MAX_SIZE];

bool isFullCmdQueue(void);      // also usable as condition callback for loopProcessToCondition()
bool isNotEmptyCmdQueue(void);  // also usable as condition callback for loopProcessToCondition()
bool isEnqueued(const CMD cmd);
bool isWritingMode(void);

bool storeCmd(const char * format, ...);
void mustStoreCmd(const char * format, ...);
void mustStoreScript(const char * format, ...);
bool storeCmdFromUART(SERIAL_PORT_INDEX portIndex, const CMD cmd);
void mustStoreCacheCmd(const char * format, ...);
bool moveCacheToCmd(void);
void clearCmdQueue(void);
void sendQueueCmd(void);

#ifdef __cplusplus
}
#endif

#endif
