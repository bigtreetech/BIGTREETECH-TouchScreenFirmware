#include "menu.h"
#include "includes.h"
#include "list_item.h"

// exhibitRect is 2 ICON Space in the Upper Row and 2 Center Coloum.
const GUI_RECT exhibitRect = {
   1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y
};

const GUI_RECT rect_of_key[ITEM_PER_PAGE*2]={
  //8 icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  //8 labels area
  {0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
};

const GUI_RECT rect_of_keyListView[ITEM_PER_PAGE]={
  //8 icons area
  {START_X,  0*LISTITEM_HEIGHT+ICON_START_Y,    LISTITEM_WIDTH+START_X,  1*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  1*LISTITEM_HEIGHT+ICON_START_Y,    LISTITEM_WIDTH+START_X,  2*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  2*LISTITEM_HEIGHT+ICON_START_Y,    LISTITEM_WIDTH+START_X,  3*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  3*LISTITEM_HEIGHT+ICON_START_Y,    LISTITEM_WIDTH+START_X,  4*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  4*LISTITEM_HEIGHT+ICON_START_Y,    LISTITEM_WIDTH+START_X,  5*LISTITEM_HEIGHT+ICON_START_Y},

  {2*START_X + LISTITEM_WIDTH,  0*LIST_ICON_HEIGHT+0*LISTICON_SPACE_Y+ICON_START_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  1*LIST_ICON_HEIGHT+0*LISTICON_SPACE_Y+ICON_START_Y},
  {2*START_X + LISTITEM_WIDTH,  1*LIST_ICON_HEIGHT+1*LISTICON_SPACE_Y+ICON_START_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  2*LIST_ICON_HEIGHT+1*LISTICON_SPACE_Y+ICON_START_Y},
  {2*START_X + LISTITEM_WIDTH,  2*LIST_ICON_HEIGHT+2*LISTICON_SPACE_Y+ICON_START_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  3*LIST_ICON_HEIGHT+2*LISTICON_SPACE_Y+ICON_START_Y},
};

//Clean up the gaps outside icons
void menuClearGaps(void)
{
  const GUI_RECT gaps[]={
  {0,                               0,               LCD_WIDTH,                       TITLE_END_Y},
  {0,                               TITLE_END_Y,     LCD_WIDTH,                       ICON_START_Y},
  {0,                               ICON_START_Y,    0*ICON_WIDTH+0*SPACE_X+START_X,  LCD_HEIGHT},
  {1*ICON_WIDTH+0*SPACE_X+START_X,  ICON_START_Y,    1*ICON_WIDTH+1*SPACE_X+START_X,  LCD_HEIGHT},
  {2*ICON_WIDTH+1*SPACE_X+START_X,  ICON_START_Y,    2*ICON_WIDTH+2*SPACE_X+START_X,  LCD_HEIGHT},
  {3*ICON_WIDTH+2*SPACE_X+START_X,  ICON_START_Y,    3*ICON_WIDTH+3*SPACE_X+START_X,  LCD_HEIGHT},
  {4*ICON_WIDTH+3*SPACE_X+START_X,  ICON_START_Y,    LCD_WIDTH,                       LCD_HEIGHT}};

  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_ClearPrect(gaps);
  GUI_SetBkColor(infoSettings.bg_color);
  for(uint8_t i = 1; i < COUNT(gaps); i++)
    GUI_ClearPrect(gaps + i);
}

void GUI_RestoreColorDefault(void){
  GUI_SetBkColor(infoSettings.bg_color);
  GUI_SetColor(infoSettings.font_color);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  setLargeFont(false);
}

static const MENUITEMS *curMenuItems = NULL;   //current menu

static const LISTITEMS *curListItems = NULL;   //current listmenu

MENU_TYPE menuType = MENU_TYPE_ICON;

uint8_t *labelGetAddress(const LABEL *label)
{
  if (label->index == LABEL_BACKGROUND) return NULL;  // No content in label
  if (label->index < LABEL_NUM) // Index of language
    return textSelect(label->index);
  else // Address of string
    return label->address;
}

void menuDrawItem(const ITEM *item, uint8_t positon)
{
  uint8_t *content = labelGetAddress(&item->label);
  const GUI_RECT *rect = rect_of_key + positon;
  if(item->icon != ICON_BACKGROUND)
    ICON_ReadDisplay(rect->x0, rect->y0, item->icon);
  else
    GUI_ClearPrect(rect);

  rect = rect_of_key + ITEM_PER_PAGE + positon;
  GUI_ClearPrect(rect);
  if (content)
    GUI_DispStringInPrect(rect, content);
}

void menuDrawIconOnly(const ITEM *item, uint8_t positon)
{
  const GUI_RECT *rect = rect_of_key + positon;
  if(item->icon != ICON_BACKGROUND)
    ICON_ReadDisplay(rect->x0, rect->y0, item->icon);
  else
    GUI_ClearPrect(rect);
}

 void menuDrawListItem(const LISTITEM *item, uint8_t position)
{
   const GUI_RECT *rect = rect_of_keyListView + position;
  if (item->icon == ICONCHAR_BACKGROUND)
  {
    GUI_ClearPrect(rect);
  }
  else
  {
    ListItem_Display(rect, position, item, false);
  }
}
void menuRefreshListPage(void){
 for (uint8_t i = 0; i < ITEM_PER_PAGE; i++)
    {
      menuDrawListItem(&curListItems->items[i], i);
      #ifdef RAPID_SERIAL_COMM
        if(isPrinting() == true && infoSettings.serial_alwaysOn != 1)
        {
          loopBackEnd();   //perform backend printing loop between drawing icons to avoid printer idling
        }
      #endif
    }

}

void setMenuType(MENU_TYPE type)
{
  menuType = type;
}

MENU_TYPE getMenuType(void)
{
  return menuType;
}

static REMINDER reminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_UNCONNECT, LABEL_UNCONNECTED};
static REMINDER volumeReminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_IDLE, LABEL_BACKGROUND};
static REMINDER busySign = {{LCD_WIDTH - 5, 0, LCD_WIDTH, 5}, 0, STATUS_BUSY, LABEL_BUSY};

void reminderSetUnConnected(void)
{
  reminder.status = STATUS_UNCONNECT;
  reminder.inf = LABEL_UNCONNECTED;
}

void reminderMessage(int16_t inf, SYS_STATUS status)
{
  reminder.inf = inf;
  GUI_SetColor(infoSettings.reminder_color);
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf));
  GUI_RestoreColorDefault();
  reminder.status = status;
  reminder.time = OS_GetTimeMs() + 2000; // 2 seconds
}

void volumeReminderMessage(int16_t inf, SYS_STATUS status)
{
  volumeReminder.inf = inf;
  GUI_SetColor(infoSettings.sd_reminder_color);
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_DispStringInPrect(&volumeReminder.rect, textSelect(volumeReminder.inf));
  volumeReminder.status = status;
  volumeReminder.time = OS_GetTimeMs() + 2000;
  GUI_RestoreColorDefault();
}

void busyIndicator(SYS_STATUS status)
{
  if(status == STATUS_BUSY)
  {
    GUI_SetColor(YELLOW);
    GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
    GUI_SetColor(infoSettings.font_color);
  }
  busySign.status = status;
  busySign.time = OS_GetTimeMs() + 2000;
}

void loopReminderClear(void)
{
  switch(reminder.status)
  {
    case STATUS_IDLE:
      return;

    case STATUS_BUSY:
      if(infoCmd.count == CMD_MAX_LIST)
        return;
      break;

    case STATUS_UNCONNECT:
      if(infoHost.connected == false)
        return;
      break;

    case STATUS_NORMAL:
      if(OS_GetTimeMs() < reminder.time)
        return;
      break;
    default:
      return;
  }

  /* Clear warning message */
  reminder.status = STATUS_IDLE;
  menuReDrawCurTitle();
}

void loopVolumeReminderClear(void)
{
  switch(volumeReminder.status)
  {
    case STATUS_NORMAL:
      if(OS_GetTimeMs() < volumeReminder.time)
        return;
      break;
    default:
      return;
  }

  /* Clear warning message */
  volumeReminder.status = STATUS_IDLE;
  menuReDrawCurTitle();
}

void loopBusySignClear(void)
{
  switch(busySign.status)
  {
    case STATUS_IDLE:
      return;

    case STATUS_BUSY:
     if(OS_GetTimeMs() < busySign.time)
        return;
     break;
  }

  /* End Busy display sing */
  busySign.status = STATUS_IDLE;
  GUI_SetColor(infoSettings.title_bg_color);
  GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
  GUI_SetColor(infoSettings.font_color);
}

void menuDrawTitle(const uint8_t *content) //(const MENUITEMS * menuItems)
{
  uint16_t start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;
  uint16_t start_x = 10;
  uint16_t end_x = drawTemperatureStatus();
  GUI_SetBkColor(infoSettings.title_bg_color);
  if (content)
  {
    GUI_DispLenString(10, start_y, content, LCD_WIDTH - 20);
    start_x += GUI_StrPixelWidth(content);
    if (start_x > LCD_WIDTH-20) start_x = LCD_WIDTH - 20;
  }
  GUI_ClearRect(start_x, start_y, end_x, start_y+BYTE_HEIGHT);

  GUI_SetBkColor(infoSettings.bg_color);
  if(reminder.status == STATUS_IDLE) return;
  GUI_SetColor(infoSettings.reminder_color);
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf));
  GUI_RestoreColorDefault();
}

void menuReDrawCurTitle(void)
{
  if (menuType == MENU_TYPE_LISTVIEW)
  {
    if(curListItems == NULL) return;
    menuDrawTitle(labelGetAddress(&curListItems->title));
  }
  else if(menuType == MENU_TYPE_ICON)
  {
    if(curMenuItems == NULL) return;
    menuDrawTitle(labelGetAddress(&curMenuItems->title));
  }
}

//Draw the entire interface
void menuDrawPage(const MENUITEMS *menuItems)
{
  u8 i = 0;
  menuType = MENU_TYPE_ICON;
  curMenuItems = menuItems;
  TSC_ReDrawIcon = itemDrawIconPress;

  //GUI_Clear(BLACK);
  menuClearGaps(); //Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
  menuDrawTitle(labelGetAddress(&menuItems->title));
  for (i = 0; i < ITEM_PER_PAGE; i++)
  {
    menuDrawItem(&menuItems->items[i], i);
    #ifdef RAPID_SERIAL_COMM
      if(isPrinting() == true && infoSettings.serial_alwaysOn != 1)
      {
        loopBackEnd();   //perform backend printing loop between drawing icons to avoid printer idling
      }
    #endif
  }
}

//Draw the entire interface
void menuDrawListPage(const LISTITEMS *listItems)
{
  u8 i = 0;
  menuType = MENU_TYPE_LISTVIEW;
  curListItems = listItems;
  TSC_ReDrawIcon = itemDrawIconPress;

  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
  GUI_SetBkColor(infoSettings.bg_color);
  GUI_ClearRect(0, TITLE_END_Y, LCD_WIDTH, LCD_HEIGHT);

  //menuClearGaps(); //Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
  menuDrawTitle(labelGetAddress(&listItems->title));

  for (i = 0; i < ITEM_PER_PAGE; i++)
  {
    //const GUI_RECT *rect = rect_of_keyListView + i;
    if (curListItems->items[i].icon != ICONCHAR_BACKGROUND)
    menuDrawListItem(&curListItems->items[i], i);
    #ifdef RAPID_SERIAL_COMM
        if(isPrinting() == true && infoSettings.serial_alwaysOn != 1)
        {
          loopBackEnd();   //perform backend printing loop between drawing icons to avoid printer idling
        }
    #endif
  }
}

//Show live info text on icons
void showLiveInfo(uint8_t index, const LIVE_INFO * liveicon, const ITEM * item)
{
  if (item != NULL)
    menuDrawIconOnly(item,index);

  for (uint8_t i = 0; i < LIVEICON_LINES; i++)
  {
    if (liveicon->enabled[i] == true)
    {
      GUI_SetColor(lcd_colors[liveicon->lines[i].fn_color]);
      GUI_SetBkColor(lcd_colors[liveicon->lines[i].bk_color]);
      GUI_SetTextMode(liveicon->lines[i].text_mode);

      GUI_POINT loc;
      loc.x = liveicon->lines[i].pos.x + rect_of_key[index].x0;

      if (liveicon->lines[i].v_align == BOTTOM)
      {
        loc.y = liveicon->lines[i].pos.y + rect_of_key[index].y0 - BYTE_HEIGHT;
      }
      else if (liveicon->lines[i].v_align == CENTER)
      {
        loc.y = liveicon->lines[i].pos.y + rect_of_key[index].y0 - BYTE_HEIGHT / 2;
      }
      else
      {
        loc.y = liveicon->lines[i].pos.y + rect_of_key[index].y0;
      }
      switch (liveicon->lines[i].h_align)
      {
      case LEFT:
        GUI_DispString(loc.x, loc.y, liveicon->lines[i].text);
        break;

      case CENTER:
        GUI_DispStringCenter(loc.x, loc.y, liveicon->lines[i].text);
        break;

      case RIGHT:
        GUI_DispStringRight(loc.x, loc.y, liveicon->lines[i].text);
        break;

      default:
        break;
      }
    }
  }
  GUI_RestoreColorDefault();
} //showLiveInfo

//When there is a button value, the icon changes color and redraws
void itemDrawIconPress(u8 position, u8 is_press)
{

  if (position > KEY_ICON_7) return;

  if (menuType == MENU_TYPE_ICON)
  {
    if (curMenuItems == NULL) return;
    if (curMenuItems->items[position].icon == ICON_BACKGROUND) return;

    const GUI_RECT *rect = rect_of_key + position;

    if (is_press) // Turn green when pressed
      ICON_PressedDisplay(rect->x0, rect->y0, curMenuItems->items[position].icon);
    else // Redraw normal icon when released
      ICON_ReadDisplay(rect->x0, rect->y0, curMenuItems->items[position].icon);
  }
  else if (menuType == MENU_TYPE_LISTVIEW)
  { //draw rec over list item if pressed
    if (curListItems == NULL)
    return;

    const GUI_RECT *rect = rect_of_keyListView + position;

    if (curListItems->items[position].icon == ICONCHAR_BACKGROUND)
    {
    GUI_ClearPrect(rect);
    return;
    }
    if (is_press){
    ListItem_Display(rect,position,&curListItems->items[position], true);
    }
    else{
    ListItem_Display(rect,position,&curListItems->items[position], false);
    }

  }
}

// Get button value
KEY_VALUES menuKeyGetValue(void)
{
  if (menuType == MENU_TYPE_ICON)
  {
    return (KEY_VALUES)KEY_GetValue(sizeof(rect_of_key) / sizeof(rect_of_key[0]), rect_of_key); // for normal menu
  }
  else if (menuType == MENU_TYPE_LISTVIEW)
  {
    return (KEY_VALUES)KEY_GetValue(sizeof(rect_of_keyListView) / sizeof(rect_of_keyListView[0]), rect_of_keyListView); //for listview
  }
  else return KEY_IDLE;
}

//Get the top left point of the corresponding icon position)
GUI_POINT getIconStartPoint(int index){
  GUI_POINT p = {rect_of_key[index].x0,rect_of_key[index].y0};
  return p;
}

void loopBackEnd(void)
{
  getGcodeFromFile();                 //Get Gcode command from the file to be printed

  sendQueueCmd();                     //Parse and send Gcode commands in the queue

  parseACK();                         //Parse the received slave response information

  parseRcvGcode();                    //Parse the received Gcode from other UART, such as: ESP3D, etc...

  loopCheckHeater();                  //Temperature monitor

  loopFan();                          //Fan speed monitor

  loopSpeed();                        //Speed & flow monitor

#ifdef BUZZER_PIN
  loopBuzzer();
#endif

if(infoMachineSettings.onboard_sd_support == ENABLED && infoMachineSettings.autoReportSDStatus == DISABLED)
  {
    loopCheckPrinting(); //Check if there is a SD or USB print running.
  }

#ifdef U_DISK_SUPPORT
  USBH_Process(&USB_OTG_Core, &USB_Host);
#endif

#if LCD_ENCODER_SUPPORT
  loopCheckEncoder();
#ifdef ST7920_SPI
  if(infoMenu.menu[infoMenu.cur] != menuST7920)
#endif
#ifdef LCD2004_simulator
  if(infoMenu.menu[infoMenu.cur] != menuHD44780)
#endif
    {
      loopCheckEncoderSteps(); //check change in encoder steps
    }
#endif

#if defined(ST7920_SPI) || defined(LCD2004_simulator)
  loopCheckMode();
#endif

#ifdef FIL_RUNOUT_PIN
  loopBackEndFILRunoutDetect();
#endif
}

void loopFrontEnd(void)
{
  loopVolumeSource();                 //Check if volume source(SD/U disk) insert

  loopReminderClear();                //If there is a message in the status bar, timed clear

  loopVolumeReminderClear();

  loopBusySignClear();                //Busy Indicator clear

  loopTemperatureStatus();

#ifdef FIL_RUNOUT_PIN
  loopFrontEndFILRunoutDetect();
#endif
}

void loopProcess(void)
{
  loopBackEnd();
  loopFrontEnd();
}
