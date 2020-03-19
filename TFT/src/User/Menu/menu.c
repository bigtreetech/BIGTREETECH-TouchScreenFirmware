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

  GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
  GUI_ClearPrect(gaps);
  GUI_SetBkColor(BACKGROUND_COLOR);
  for(uint8_t i = 1; i < COUNT(gaps); i++)
    GUI_ClearPrect(gaps + i);
}

void GUI_RestoreColorDefault(void){
  GUI_SetBkColor(BACKGROUND_COLOR);
  GUI_SetColor(FONT_COLOR);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
}

static const MENUITEMS *curMenuItems = NULL;   //current menu

static const LISTITEMS *curListItems = NULL;   //current listmenu

static bool isListview;

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
        #ifndef CLEAN_MODE_SWITCHING_SUPPORT
          if(isPrinting() == true)
        #endif
        {
          loopBackEnd();	 //perform backend printing loop between drawing icons to avoid printer idling
        }
      #endif      
    }

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
  GUI_SetColor(REMINDER_FONT_COLOR);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf));
  GUI_SetColor(FONT_COLOR);
  reminder.status = status;
  reminder.time = OS_GetTimeMs() + 2000; // 2 seconds
}

void volumeReminderMessage(int16_t inf, SYS_STATUS status)
{
  volumeReminder.inf = inf;
  GUI_SetColor(VOLUME_REMINDER_FONT_COLOR);
  GUI_DispStringInPrect(&volumeReminder.rect, textSelect(volumeReminder.inf));
  GUI_SetColor(FONT_COLOR);
  volumeReminder.status = status;
  volumeReminder.time = OS_GetTimeMs() + 2000;
}

void busyIndicator(SYS_STATUS status)
{
  if(status == STATUS_BUSY)
  {
    GUI_SetColor(YELLOW);
    GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
    GUI_SetColor(FONT_COLOR);
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
  if(curMenuItems == NULL)
    return;
  menuDrawTitle(labelGetAddress(&curMenuItems->title));
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
  if(curMenuItems == NULL)
    return;
  menuDrawTitle(labelGetAddress(&curMenuItems->title));
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
  GUI_SetColor(BACKGROUND_COLOR);
  GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
  GUI_SetColor(FONT_COLOR);
}

void menuDrawTitle(const uint8_t *content) //(const MENUITEMS * menuItems)
{
  uint16_t start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;
  GUI_FillRectColor(10, start_y, LCD_WIDTH-10, start_y+BYTE_HEIGHT, TITLE_BACKGROUND_COLOR);

  if (content)
  {
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_DispLenString(10, start_y, content, LCD_WIDTH-20);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  }

  show_GlobalInfo();
  if(reminder.status == STATUS_IDLE) return;
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf));
  GUI_SetColor(FONT_COLOR);
}

//Draw the entire interface
void menuDrawPage(const MENUITEMS *menuItems)
{
  u8 i = 0;
  isListview = false;
  curMenuItems = menuItems;
  TSC_ReDrawIcon = itemDrawIconPress;

  //GUI_Clear(BLACK);
  menuClearGaps(); //Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
  menuDrawTitle(labelGetAddress(&menuItems->title));
  for (i = 0; i < ITEM_PER_PAGE; i++)
  {
    menuDrawItem(&menuItems->items[i], i);
    #ifdef RAPID_SERIAL_COMM
      #ifndef CLEAN_MODE_SWITCHING_SUPPORT
        if(isPrinting() == true)
      #endif
      {
        loopBackEnd();	 //perform backend printing loop between drawing icons to avoid printer idling
      }
    #endif
  }
}

//Draw the entire interface
void menuDrawListPage(const LISTITEMS *listItems)
{
  u8 i = 0;
  isListview = true;
  curListItems = listItems;
  TSC_ReDrawIcon = itemDrawIconPress;

  GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
  GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
  GUI_SetBkColor(BACKGROUND_COLOR);
  GUI_ClearRect(0, TITLE_END_Y, LCD_WIDTH, LCD_HEIGHT);

  //menuClearGaps(); //Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
  menuDrawTitle(labelGetAddress(&listItems->title));

  for (i = 0; i < ITEM_PER_PAGE; i++)
  {
    //const GUI_RECT *rect = rect_of_keyListView + i;
    if (curListItems->items[i].icon != ICONCHAR_BACKGROUND)
    menuDrawListItem(&curListItems->items[i], i);
    #ifdef RAPID_SERIAL_COMM
      #ifndef CLEAN_MODE_SWITCHING_SUPPORT
        if(isPrinting() == true)
      #endif
        {
          loopBackEnd();	 //perform backend printing loop between drawing icons to avoid printer idling
        }
    #endif
  }
//  show_globalinfo();
}

//When there is a button value, the icon changes color and redraws
void itemDrawIconPress(u8 position, u8 is_press)
{

  if (position > KEY_ICON_7) return;

  if (isListview == false)
  {
    if (curMenuItems == NULL) return;
    if (curMenuItems->items[position].icon == ICON_BACKGROUND) return;

    const GUI_RECT *rect = rect_of_key + position;

    if (is_press) // Turn green when pressed
      ICON_PressedDisplay(rect->x0, rect->y0, curMenuItems->items[position].icon);
    else // Redraw normal icon when released
      ICON_ReadDisplay(rect->x0, rect->y0, curMenuItems->items[position].icon);
  }
  else
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
  if (isListview == false)
  {
    return (KEY_VALUES)KEY_GetValue(sizeof(rect_of_key) / sizeof(rect_of_key[0]), rect_of_key); // for normal menu
  }
  else
  {
    return (KEY_VALUES)KEY_GetValue(sizeof(rect_of_keyListView) / sizeof(rect_of_keyListView[0]), rect_of_keyListView); //for listview
  }
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

  loopCheckHeater();			            //Temperature related settings

#ifdef BUZZER_PIN
  loopBuzzer();
#endif
  
#if defined ONBOARD_SD_SUPPORT && !defined M27_AUTOREPORT
  loopCheckPrinting();                //Check if there is a SD or USB print running.
#endif

#ifdef U_DISK_SUPPROT
  USBH_Process(&USB_OTG_Core, &USB_Host);
#endif

#if LCD_ENCODER_SUPPORT
  loopCheckMode();
#endif

#ifdef FIL_RUNOUT_PIN
  loopFILRunoutDetect();
#endif
}

void loopFrontEnd(void)
{
  loopVolumeSource();                 //Check if volume source(SD/U disk) insert

  loopReminderClear();	              //If there is a message in the status bar, timed clear

  loopVolumeReminderClear();

  loopBusySignClear();                //Busy Indicator clear

  temp_Change();
}

void loopProcess(void)
{
  loopBackEnd();
  loopFrontEnd();
}
