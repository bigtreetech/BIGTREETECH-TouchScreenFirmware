#ifndef _SEND_GCODE_H_
#define _SEND_GCODE_H_

#include "menu.h"


typedef enum
{
  TERMINAL_GCODE,
  TERMINAL_ACK,
}TERMINAL_SRC;


void menuSendGcode(void);
void menuTerminal(void);
void sendGcodeTerminalCache(char *stream, TERMINAL_SRC src);

#endif
