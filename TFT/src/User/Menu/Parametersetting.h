#ifndef _PARAMETERSETTING_H_
#define _PARAMETERSETTING_H_
#include "menu.h"

#define P_height        LCD_HEIGHT/4
#define Key_height      (LCD_HEIGHT-P_height)/4
#define SkeyWIDTH       LCD_WIDTH/4

#define BUTTON_SPACE    BYTE_WIDTH/2
#define BUTTON_WIDTH    (LCD_WIDTH-BUTTON_SPACE*5)/4

#define TITLE_SY        (ICON_START_Y - BYTE_HEIGHT) / 2

#define ICON_NOZZLE_X   2*ICON_WIDTH+ICON_WIDTH/2
#define VALUE_NOZZLE_X  ICON_NOZZLE_X+BYTE_WIDTH

#define ICON_BED_X      3*ICON_WIDTH+2*BYTE_WIDTH+ICON_WIDTH/2
#define VALUE_BED_X     ICON_BED_X+BYTE_WIDTH

#define SET_BACKGROUND_COLOR    GRAY

#define KEY_NUM 14
#define PARANMETER_NUM 12
#define BUFLONG 6
#define CDM_NUM 2
#define VALUE_NUM 8

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
  NUM_KEY_0 = 0,
  NUM_KEY_1,
  NUM_KEY_2,
  NUM_KEY_DEL,
  NUM_KEY_4,
  NUM_KEY_5,
  NUM_KEY_6,
  NUM_KEY_7,
  NUM_KEY_8,
  NUM_KEY_9,
  NUM_KEY_10,
  NUM_KEY_11,
  NUM_KEY_12,
  NUM_KEY_13,

  NUM_KEY_IDLE = IDLE_TOUCH,
}NUM_KEY_VALUES;

void parametersetting(void);
void show_GlobalInfo(void);
void temp_Change(void);
extern bool getsetparameter;
extern int cmd_getparameter_num;
extern float Get_parameter_value[VALUE_NUM];
#endif
