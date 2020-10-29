#ifndef _SEND_GCODE_H_
#define _SEND_GCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "menu.h"

#define TERMINAL_MAX_CHAR ((LCD_WIDTH / BYTE_WIDTH) * (LCD_HEIGHT / BYTE_HEIGHT) * 4)
#define MAX_BUFF  20

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

typedef struct
{
  char *ptr[MAX_BUFF];        //Pointer into the terminal page buffer , full Screen is one page
  uint8_t pageHead;           //Buffer top of page
  uint8_t pageTail;           //Buffer buttom of page
  uint8_t oldPageHead;
  uint8_t pageIndex;          //page buffer index
  uint8_t oldPageIndex;
}TERMINAL_PAGE;

void menuSendGcode(void);
void menuTerminal(void);
void sendGcodeTerminalCache(char *stream, TERMINAL_SRC src);

#ifdef __cplusplus
}
#endif

#endif
