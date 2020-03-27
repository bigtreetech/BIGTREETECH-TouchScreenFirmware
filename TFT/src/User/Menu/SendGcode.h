#ifndef _SEND_GCODE_H_
#define _SEND_GCODE_H_

#include "menu.h"

typedef enum
{
  GKEY_0 = 0,
  GKEY_1,
  GKEY_2,
  GKEY_3,
  GKEY_4,
  GKEY_DEL,
  GKEY_5,
  GKEY_6,
  GKEY_7,
  GKEY_8,
  GKEY_9,
  GKEY_SPACE,
  GKEY_10,
  GKEY_11,
  GKEY_12,
  GKEY_13,
  GKEY_14,
  GKEY_ABC_123,
  GKEY_15,
  GKEY_16,
  GKEY_17,
  GKEY_18,
  GKEY_19,
  GKEY_20,
  GKEY_BACK,
  GKEY_SEND,
  GKEY_IDLE = IDLE_TOUCH,
}GKEY_VALUES;

typedef enum
{
  TERMINAL_GCODE,
  TERMINAL_ACK,
}TERMINAL_SRC;


void menuSendGcode(void);
void menuTerminal(void);
void sendGcodeTerminalCache(char *stream, TERMINAL_SRC src);

#endif
