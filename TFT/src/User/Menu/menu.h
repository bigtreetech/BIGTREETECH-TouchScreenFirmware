#ifndef _MENU_H_
#define _MENU_H_

#include "stdint.h"
#include "stdbool.h"
#include "GUI.h"

typedef enum
{
  PAGE_MAIN = 0,
  PAGE_HEAT,
  PAGE_MOVE,
  PAGE_HOME,
  PAGE_PRINT,
  PAGE_EXTRUDE,
  PAGE_FAN,
  PAGE_SETTINGS,
  PAGE_NUM
}PAGE;

#define IDLE_TOUCH	0xFFFF
typedef enum
{
  KEY_ICON_0 = 0,
  KEY_ICON_1,
  KEY_ICON_2,
  KEY_ICON_3,
  KEY_ICON_4,
  KEY_ICON_5,
  KEY_ICON_6,
  KEY_ICON_7,
  KEY_LABEL_0,
  KEY_LABEL_1,
  KEY_LABEL_2,
  KEY_LABEL_3,
  KEY_LABEL_4,
  KEY_LABEL_5,
  KEY_LABEL_6,
  KEY_LABEL_7,
  KEY_IDLE = IDLE_TOUCH,
}KEY_VALUES;

#define ITEM_PER_PAGE  8

/*-------------------------send gcode-------top*/
typedef enum
{
  GKEY_0 = 0,
  GKEY_1,
  GKEY_2,
  GKEY_3,
  GKEY_4,
  GKEY_5,
  GKEY_6,
  GKEY_7,
  GKEY_8,
  GKEY_9,
  GKEY_10,
  GKEY_11,
  GKEY_12,
  GKEY_13,
  GKEY_14,
  GKEY_15,
  GKEY_16,
  GKEY_17,
  GKEY_18,
  GKEY_19,
  GKEY_20,
  GKEY_21,
  GKEY_22,
  GKEY_23,
  GKEY_IDLE = IDLE_TOUCH,
}GKEY_VALUES;
/*-------------------------send gcode-------end*/
/*-------------------------select mode-------top*/
#define SELECTMODE 2
typedef enum
{
  MKEY_0 = 0,
  MKEY_1,
  MKEY_IDLE = IDLE_TOUCH,
}MKEY_VALUES;
/*-------------------------select mode-------end*/

typedef struct
{
  int16_t icon;
  int16_t label;
}ITEM;

typedef struct
{
  int16_t title;
  ITEM    items[ITEM_PER_PAGE];
}MENUITEMS;


typedef enum
{
  STATUS_IDLE = 0,
  STATUS_BUSY,
  STATUS_UNCONNECT,
  STATUS_NORMAL
}SYS_STATUS;

typedef struct
{
  GUI_RECT rect;
  uint32_t time;
  uint8_t  status;
  int16_t inf;
}REMINDER;

extern const GUI_RECT exhibitRect;
#define CENTER_Y  ((exhibitRect.y1 - exhibitRect.y0)/2 + exhibitRect.y0)
#define CENTER_X  ((exhibitRect.x1 - exhibitRect.x0 - BYTE_WIDTH)/2 + exhibitRect.x0)

void reminderMessage(int16_t inf, SYS_STATUS status);
void volumeReminderMessage(int16_t inf, SYS_STATUS status);

void busyIndicator(SYS_STATUS status);

void menuDrawItem (const ITEM * menuItem, uint8_t positon);
void menuDrawTitle(const MENUITEMS * menuItems);
void menuDrawPage (const MENUITEMS * menuItems);

void itemDrawIconPress(uint8_t positon, uint8_t is_press);
KEY_VALUES menuKeyGetValue(void);

//GCODE KEYB fun
GKEY_VALUES GKeyGetValue(void);
void DrawGKEY(void);
bool revinfo(void);
void DrawEGKEY(void);
bool SendGcode(void);
bool BackGKEY(void);
void showb(void);

//select mode fun
extern const GUI_RECT rect_of_mode[SELECTMODE];
extern MKEY_VALUES MKeyGetValue(void);
extern void selectmode(int8_t  nowMode);

void loopProcess (void);

#endif

