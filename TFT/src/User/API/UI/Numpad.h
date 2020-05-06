#ifndef _NUMPAD_H_
#define _NUMPAD_H_

#include "includes.h"

#define SKEYHEIGHT      (LCD_HEIGHT-ICON_START_Y)/4
#define SKEYWIDTH       LCD_WIDTH/4

#define KEY_NUM 16
#define BUFLONG 6

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
  NUM_KEY_15,

  NUM_KEY_IDLE = IDLE_TOUCH,
}NUM_KEY_VALUES;


float numPadFloat(float old_val, bool negative_val);

u32 numPadInt(u32 old_val);

#endif