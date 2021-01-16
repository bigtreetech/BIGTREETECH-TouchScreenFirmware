#ifndef _NUMPAD_H_
#define _NUMPAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "list_item.h"

#define SKEYHEIGHT      (LCD_HEIGHT-ICON_START_Y)/4
#define SKEYWIDTH       LCD_WIDTH/4

#define KEY_NUM 16
#define FLOAT_BUFLONG 9
#define INT_BUFLONG 6

typedef enum
{
  SKEY_0 = 0,
  SKEY_1,
  SKEY_2,
  SKEY_3,
  SKEY_4,
  SKEY_5,
  SKEY_6,
  SKEY_7,
  SKEY_8,
  SKEY_9,
  SKEY_10,
  SKEY_11,
  SKEY_IDLE = IDLE_TOUCH,
}SKEY_VALUES;

typedef enum
{
  NUM_KEY_1 = 0,
  NUM_KEY_2,
  NUM_KEY_3,
  NUM_KEY_OK,
  NUM_KEY_4,
  NUM_KEY_5,
  NUM_KEY_6,
  NUM_KEY_DEL,
  NUM_KEY_7,
  NUM_KEY_8,
  NUM_KEY_9,
  NUM_KEY_EXIT,
  NUM_KEY_DEC,
  NUM_KEY_0,
  NUM_KEY_MINUS,
  NUM_KEY_RESET,

  NUM_KEY_IDLE = IDLE_TOUCH,
}NUM_KEY_VALUES;


float numPadFloat(u8* title, float old_val, float reset_val, bool negative);

int32_t numPadInt(u8* title, int32_t old_val, int32_t reset_val, bool negative);

#ifdef __cplusplus
}
#endif

#endif
