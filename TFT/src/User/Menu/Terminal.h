#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "SerialConnection.h"

typedef enum
{
  TERMINAL_GCODE = 0,
  TERMINAL_ACK,
  TERMINAL_COUNT
} TERMINAL_SRC;

void menuTerminal(void);
void terminalCache(const char * stream, uint16_t streamLen, SERIAL_PORT_INDEX portIndex, TERMINAL_SRC src);

#ifdef __cplusplus
}
#endif

#endif
