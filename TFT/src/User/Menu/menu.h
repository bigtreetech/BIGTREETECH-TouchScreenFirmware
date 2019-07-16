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


void loopProcess (void);

#endif

