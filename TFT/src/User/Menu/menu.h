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

/*-------------------------select mode-------top*/
#define SELECTMODE 2
typedef enum
{
  MKEY_0 = 0,
  MKEY_1,
  MKEY_IDLE = IDLE_TOUCH,
}MKEY_VALUES;
/*-------------------------select mode-------end*/

typedef union
{
  uint32_t index;    // language index, address = textSelect(index);
  uint8_t *address;
}LABEL;

typedef struct
{
  uint16_t icon;
  LABEL label;
}ITEM;

typedef struct
{
  LABEL title;
  ITEM  items[ITEM_PER_PAGE];
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
#define LISTITEM_WIDTH (LCD_WIDTH-(3*START_X)-LIST_ICON_WIDTH)
#define LISTITEM_HEIGHT ((LCD_HEIGHT-TITLE_END_Y-START_X)/5)
#define LISTICON_SPACE_Y ((LCD_HEIGHT-TITLE_END_Y-START_X-(3*LIST_ICON_HEIGHT))/ 2)

void reminderMessage(int16_t inf, SYS_STATUS status);
void volumeReminderMessage(int16_t inf, SYS_STATUS status);

void busyIndicator(SYS_STATUS status);

void menuDrawItem (const ITEM * menuItem, uint8_t positon);
void menuDrawListItem(const ITEM *item, uint8_t positon);
void menuDrawTitle(const MENUITEMS * menuItems);
void menuDrawPage (const MENUITEMS * menuItems, bool listview);

void itemDrawIconPress(uint8_t positon, uint8_t is_press);
KEY_VALUES menuKeyGetValue(void);

//select mode fun
extern const GUI_RECT rect_of_mode[SELECTMODE];
extern MKEY_VALUES MKeyGetValue(void);
extern void selectmode(int8_t  nowMode);

void loopBackEnd(void);
void loopFrontEnd(void);
void loopProcess (void);

#endif

