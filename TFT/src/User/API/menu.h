#ifndef _MENU_H_
#define _MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "GUI.h"

#define IDLE_TOUCH 0xFFFF

#define ITEM_PER_PAGE     8
#define PS_TOUCH_OFFSET   2  // printing screen icon index offset for touch input
#define MENU_RECT_COUNT   (ITEM_PER_PAGE * 2 + 1)      // 8 items + title bar
#define SS_RECT_COUNT     (ITEM_PER_PAGE * 2 + 1 + 1)  // 8 items + title bar + infobox
#define TM_RECT_COUNT     (ITEM_PER_PAGE * 2 + 1 + 1)  // 8 items + title bar + tempbox
#define LISTITEM_PER_PAGE 5
#define LIVEICON_LINES    3

#define CENTER_Y         ((exhibitRect.y1 - exhibitRect.y0) / 2 + exhibitRect.y0)
#define CENTER_X         ((exhibitRect.x1 - exhibitRect.x0 - BYTE_WIDTH) / 2 + exhibitRect.x0)
#define LISTITEM_WIDTH   (LCD_WIDTH - (3 * START_X) - LIST_ICON_WIDTH)
#define LISTITEM_HEIGHT  ((LCD_HEIGHT - ICON_START_Y - START_X) / 5)
#define LISTICON_SPACE_Y ((LCD_HEIGHT - ICON_START_Y - START_X - (3 * LIST_ICON_HEIGHT)) / 2)

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
  KEY_TITLEBAR,
  KEY_INFOBOX,
  KEY_INCREASE = IDLE_TOUCH - 5,
  KEY_DECREASE = IDLE_TOUCH - 4,
  KEY_PAGEUP   = IDLE_TOUCH - 3,
  KEY_PAGEDOWN = IDLE_TOUCH - 2,
  KEY_BACK     = IDLE_TOUCH - 1,
  KEY_IDLE     = IDLE_TOUCH,
} KEY_VALUES;

typedef enum
{
  PS_KEY_0 = 0,
  PS_KEY_1,
  PS_KEY_2,
  PS_KEY_3,
  PS_KEY_4,
  PS_KEY_5,
  PS_KEY_6,
  PS_KEY_7,
  PS_KEY_8,
  PS_KEY_9,
  PS_KEY_TITLEBAR,
  PS_KEY_INFOBOX,
} PS_KEY_VALUES;

typedef enum
{
  MENU_TYPE_ICON,
  MENU_TYPE_LISTVIEW,
  MENU_TYPE_DIALOG,
  MENU_TYPE_EDITOR,
  MENU_TYPE_FULLSCREEN,
  MENU_TYPE_OTHER,
} MENU_TYPE;

typedef union
{
  int32_t index;  // language index, address = textSelect(index);
  void *address;
} LABEL;

// always initialize label to default values
#define init_label(X) LABEL X = {.index = LABEL_BACKGROUND}

typedef struct
{
  uint16_t icon;
  LABEL label;
} ITEM;

typedef struct
{
  LABEL title;
  ITEM  items[ITEM_PER_PAGE];
} MENUITEMS;

typedef enum
{
  STATUS_IDLE = 0,
  STATUS_BUSY,
  STATUS_DISCONNECTED,
  STATUS_LISTENING,
  STATUS_NORMAL
} SYS_STATUS;

typedef struct
{
  GUI_RECT rect;
  uint32_t time;
  uint8_t status;
  uint16_t inf;
} REMINDER;

typedef enum
{
  LIST_LABEL = 0,
  LIST_TOGGLE,
  LIST_RADIO,
  LIST_MOREBUTTON,
  LIST_CUSTOMVALUE,
} LISTITEM_TYPE;

typedef struct
{
  uint16_t icon;
  LISTITEM_TYPE itemType;
  LABEL titlelabel;
  LABEL valueLabel;
} LISTITEM;

typedef struct
{
  LABEL title;
  //uint16_t titleIconChar;
  LISTITEM items[ITEM_PER_PAGE];
} LISTITEMS;

typedef struct
{
  uint8_t *     text;
  GUI_POINT     pos;      // relative to icon top left corner
  uint8_t       h_align;  // left, right or center of pos point
  uint8_t       v_align;  // left, right or center of pos point
  uint16_t      fn_color;
  uint16_t      bk_color;
  GUI_TEXT_MODE text_mode;
  uint16_t      font;
} LIVE_DATA;

typedef struct
{
  uint8_t   enabled[LIVEICON_LINES];
  LIVE_DATA lines[LIVEICON_LINES];
} LIVE_INFO;

typedef bool (* CONDITION_CALLBACK)(void);

extern const GUI_RECT exhibitRect;
extern const GUI_RECT rect_of_key[MENU_RECT_COUNT];
extern const GUI_RECT rect_of_keySS[SS_RECT_COUNT];
extern const GUI_RECT rect_of_keyPS[];
extern const GUI_RECT rect_of_keyPS_end[];

extern const GUI_RECT rect_of_titleBar[1];

void setMenuType(MENU_TYPE type);
MENU_TYPE getMenuType(void);

void reminderMessage(int16_t inf, SYS_STATUS status);
void volumeReminderMessage(int16_t inf, SYS_STATUS status);
void notificationDot(void);

void busyIndicator(SYS_STATUS status);

MENUITEMS *getCurMenuItems(void);
LISTITEMS *getCurListItems(void);
GUI_POINT getIconStartPoint(int index);

void GUI_RestoreColorDefault(void);
uint8_t *labelGetAddress(const LABEL * label);
void setMenu(MENU_TYPE menu_type, LABEL * title, uint16_t rectCount, const GUI_RECT * menuRect,
             void(*action_redraw)(uint8_t position, uint8_t is_press),
             void (*menu_redraw)(void));
void menuDrawItem (const ITEM * menuItem, uint8_t position);
void menuDrawIconOnly(const ITEM *item, uint8_t position);
void menuDrawIconText(const ITEM *item, uint8_t position);
void menuDrawListItem(const LISTITEM *item, uint8_t position);
void menuRefreshListPage(void);
void menuDrawTitle(const uint8_t *content);  //(const MENUITEMS * menuItems);
void menuReDrawCurTitle(void);
void menuDrawPage(const MENUITEMS * menuItems);
void menuDrawListPage(const LISTITEMS *listItems);

void showLiveInfo(uint8_t index, const LIVE_INFO * liveicon, const ITEM * item);
void displayExhibitHeader(const char * titleStr, const char * unitStr);
void displayExhibitValue(const char * valueStr);

void itemDrawIconPress(uint8_t position, uint8_t is_press);
void itemDrawIconPress_PS(uint8_t position, uint8_t is_press);
KEY_VALUES menuKeyGetValue(void);

// Smart home
#ifdef SMART_HOME
  #define LONG_TOUCH (MODE_SWITCHING_INTERVAL / 3)  // keep it lower than MODE_SWITCHING_INTERVAL

  void loopCheckBackPress(void);
#endif

void menuDummy(void);
void loopBackEnd(void);
void loopFrontEnd(void);
void loopProcess(void);
void loopProcessToCondition(CONDITION_CALLBACK condCallback);

#ifdef __cplusplus
}
#endif

#endif
