#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "SerialConnection.h"

typedef enum
{
  SRC_TERMINAL_GCODE = 0,
  SRC_TERMINAL_ACK,
  SRC_TERMINAL_COUNT
} TERMINAL_SRC;

void terminalCache(const char * stream, uint16_t streamLen, SERIAL_PORT_INDEX portIndex, TERMINAL_SRC src);
void menuTerminal(void);

#ifdef __cplusplus
}
#endif

#endif
