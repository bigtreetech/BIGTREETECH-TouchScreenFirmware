#include "menu.h"
#include "includes.h"
#include "list_item.h"

// exhibitRect is 2 ICON Space in the Upper Row and 2 Center Coloum. 
const GUI_RECT exhibitRect = {
   1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y
};

const GUI_RECT rect_of_key[ITEM_PER_PAGE*2]={
  //8 icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y},
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},

  //8 labels area
  {0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y,  4*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+TITLE_END_Y},
  {1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+TITLE_END_Y},
  {2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+TITLE_END_Y},
  {3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  4*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+TITLE_END_Y},
};

const GUI_RECT rect_of_keyStatus[ITEM_PER_PAGE*2]={
  //8 icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y,    1*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y,    2*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y,    3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y,    4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y},
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y},

  //8 labels area
  {0,  0,   0, 0 },
  {0,  0,   0, 0 },
  {0,  0,   0, 0 },
  {0,  0,   0, 0 },
  {0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,                      1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+SPACE_Y+SPACE_Y+TITLE_END_Y},
  {1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,                      2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+SPACE_Y+SPACE_Y+TITLE_END_Y},
  {2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,                      3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+SPACE_Y+SPACE_Y+TITLE_END_Y},
  {3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y,                      4*SPACE_X_PER_ICON,  2*ICON_HEIGHT+SPACE_Y+SPACE_Y+TITLE_END_Y},
};

const GUI_RECT rect_of_keyListView[ITEM_PER_PAGE]={
  //8 icons area
  {START_X,  0*LISTITEM_HEIGHT+TITLE_END_Y,    LISTITEM_WIDTH+START_X,  1*LISTITEM_HEIGHT+TITLE_END_Y},
  {START_X,  1*LISTITEM_HEIGHT+TITLE_END_Y,    LISTITEM_WIDTH+START_X,  2*LISTITEM_HEIGHT+TITLE_END_Y},
  {START_X,  2*LISTITEM_HEIGHT+TITLE_END_Y,    LISTITEM_WIDTH+START_X,  3*LISTITEM_HEIGHT+TITLE_END_Y},
  {START_X,  3*LISTITEM_HEIGHT+TITLE_END_Y,    LISTITEM_WIDTH+START_X,  4*LISTITEM_HEIGHT+TITLE_END_Y},
  {START_X,  4*LISTITEM_HEIGHT+TITLE_END_Y,    LISTITEM_WIDTH+START_X,  5*LISTITEM_HEIGHT+TITLE_END_Y},

  {2*START_X + LISTITEM_WIDTH,  0*LIST_ICON_HEIGHT+0*LISTICON_SPACE_Y+TITLE_END_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  1*LIST_ICON_HEIGHT+0*LISTICON_SPACE_Y+TITLE_END_Y},
  {2*START_X + LISTITEM_WIDTH,  1*LIST_ICON_HEIGHT+1*LISTICON_SPACE_Y+TITLE_END_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  2*LIST_ICON_HEIGHT+1*LISTICON_SPACE_Y+TITLE_END_Y},
  {2*START_X + LISTITEM_WIDTH,  2*LIST_ICON_HEIGHT+2*LISTICON_SPACE_Y+TITLE_END_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  3*LIST_ICON_HEIGHT+2*LISTICON_SPACE_Y+TITLE_END_Y},
};

const GUI_RECT rect_of_mode[SELECTMODE]={
  //2 select icon
  {1*SPACE_SELEX+0*selecticonw,SPACE_SELEY,1*SPACE_SELEX+1*selecticonw,SPACE_SELEY+selecticonw},
  {3*SPACE_SELEX+1*selecticonw,SPACE_SELEY,3*SPACE_SELEX+2*selecticonw,SPACE_SELEY+selecticonw},
};

//Clean up the gaps outside icons
void menuClearGaps(void)
{
  const GUI_RECT gaps[]={
  {0,                               0,        LCD_WIDTH,                       TITLE_END_Y},
  {0,                               SPACE_Y,  0*ICON_WIDTH+0*SPACE_X+START_X,  LCD_HEIGHT},
  {1*ICON_WIDTH+0*SPACE_X+START_X,  SPACE_Y,  1*ICON_WIDTH+1*SPACE_X+START_X,  LCD_HEIGHT},
  {2*ICON_WIDTH+1*SPACE_X+START_X,  SPACE_Y,  2*ICON_WIDTH+2*SPACE_X+START_X,  LCD_HEIGHT},
  {3*ICON_WIDTH+2*SPACE_X+START_X,  SPACE_Y,  3*ICON_WIDTH+3*SPACE_X+START_X,  LCD_HEIGHT},
  {4*ICON_WIDTH+3*SPACE_X+START_X,  SPACE_Y,  LCD_WIDTH,                       LCD_HEIGHT}};

  const GUI_RECT ssGap[2] ={
{0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y,  4*ICON_WIDTH+3*SPACE_X+START_X, 1*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y+STATUS_GANTRY_YOFFSET },
{0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y-STATUS_GANTRY_YOFFSET,  4*ICON_WIDTH+3*SPACE_X+START_X, 1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y}
} ;

  for(int i=0; i < sizeof(gaps)/sizeof(gaps[0]); i++)
  GUI_ClearRect(gaps[i].x0, gaps[i].y0, gaps[i].x1, gaps[i].y1);
   if (infoMenu.menu[infoMenu.cur] == menuStatus){
    GUI_ClearPrect(&ssGap[0]);
    GUI_ClearPrect(&ssGap[1]);
  }
}

static const MENUITEMS * curMenuItems = NULL;   //current menu
static bool isListview;

uint8_t *labelGetAddress(const LABEL *label)
{
  if (label->index == LABEL_BACKGROUND) return NULL;  // No content in label
  if (label->index < LABEL_NUM) // Index of language
    return textSelect(label->index);
  else // Address of string
    return label->address;
}

void menuDrawItem(const ITEM * item, u8 positon)
{
  uint8_t *content = labelGetAddress(&item->label);
  const GUI_RECT *rect = rect_of_key + positon;
  if (infoMenu.menu[infoMenu.cur] == menuStatus){
     rect = rect_of_keyStatus + positon;
  }
     
  if(item->icon != ICON_BACKGROUND)
    ICON_ReadDisplay(rect->x0, rect->y0, item->icon);
  else
    GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);

  if (infoMenu.menu[infoMenu.cur] == menuStatus){
     rect = rect_of_keyStatus + ITEM_PER_PAGE + positon;
  }
  else
  {
    rect = rect_of_key + ITEM_PER_PAGE + positon;
  }
  
  GUI_ClearPrect(rect);
  if (content)  
    GUI_DispStringInPrect(rect, content);
}

void menuDrawListItem(const ITEM *item, uint8_t positon)
{
  const GUI_RECT *rect = rect_of_keyListView + positon;
  if (item->icon == ICONCHAR_BACKGROUND && item->label.index == LABEL_BACKGROUND)
  {
    GUI_ClearPrect(rect);
  }
  else
  {
    if (positon > 4)
    {
      GUI_SetBkColor(LISTBTN_BKCOLOR);
      ListItem_Display(rect, item->icon, MIDDLE, item->label.address, false);
      GUI_SetBkColor(BK_COLOR);
    }
    else
    {

      ListItem_Display(rect, item->icon, LEFT_CENTER, item->label.address, false);
    }
  }
}

const GUI_RECT reminderRect = {0, 0, LCD_WIDTH, TITLE_END_Y};
const GUI_RECT reminderRectSS ={0, 0, LCD_WIDTH, SS_TITLE_END_Y};

static REMINDER reminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_UNCONNECT, LABEL_UNCONNECTED};
static REMINDER volumeReminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_IDLE, LABEL_BACKGROUND};
static REMINDER busySign = {{LCD_WIDTH - 5, 0, LCD_WIDTH, 5}, 0, STATUS_BUSY, LABEL_BUSY};


void reminderMessage(int16_t inf, SYS_STATUS status)
{
  if (infoMenu.menu[infoMenu.cur] == menuStatus){
    reminder.rect = reminderRectSS;
  }
  else{
    reminder.rect = reminderRect;
  }
  reminder.inf = inf;
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf));
  GUI_SetColor(FK_COLOR);
  reminder.status = status;
  reminder.time = OS_GetTime();
}

void volumeReminderMessage(int16_t inf, SYS_STATUS status)
{
  if (infoMenu.menu[infoMenu.cur] == menuStatus){
    volumeReminder.rect = reminderRectSS;
  }
  else{
    volumeReminder.rect = reminderRect;
  }   
  volumeReminder.inf = inf;
  GUI_SetColor(GBLUE);
  GUI_DispStringInPrect(&volumeReminder.rect, textSelect(volumeReminder.inf));
  GUI_SetColor(FK_COLOR);
  volumeReminder.status = status;
  volumeReminder.time = OS_GetTime();
}

void busyIndicator(SYS_STATUS status)
{
  if(status == STATUS_BUSY)
  {
    GUI_SetColor(YELLOW);
    GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
    GUI_SetColor(FK_COLOR);
  }
  busySign.status = status;
  busySign.time = OS_GetTime();
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
      if(OS_GetTime()<reminder.time+200)
        return;
      break;
    default:
      return;
  }

  /* Clear warning message */		
  reminder.status = STATUS_IDLE;
  if(curMenuItems == NULL)
    return;
  menuDrawTitle(curMenuItems);
}
void loopVolumeReminderClear(void)
{	
  switch(volumeReminder.status)
  {
    case STATUS_NORMAL:
      if(OS_GetTime()<volumeReminder.time + 200)
        return;
      break;
    default:
      return;
  }

  /* Clear warning message */		
  volumeReminder.status = STATUS_IDLE;
  if(curMenuItems == NULL)
    return;
  menuDrawTitle(curMenuItems);
}

void loopBusySignClear(void)
{	
  switch(busySign.status)
  {
    case STATUS_IDLE:
      return;
    
    case STATUS_BUSY:
     if(OS_GetTime()<busySign.time+200)
        return;
     break;
  }

  /* End Busy display sing */		
  busySign.status = STATUS_IDLE;
  GUI_SetColor(BK_COLOR);
  GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
  GUI_SetColor(FK_COLOR);
}


void menuDrawTitle(const MENUITEMS * menuItems)
{
  const uint8_t *content = labelGetAddress(&menuItems->title);
  uint16_t start_y;
  if (infoMenu.menu[infoMenu.cur] == menuStatus)
  {
    start_y = (SS_TITLE_END_Y - BYTE_HEIGHT) / 2;
  }
  else
  {
    start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;
  }

  GUI_ClearRect(10, start_y, LCD_WIDTH-10, start_y+BYTE_HEIGHT);
  
  if(content)
  {
    GUI_DispLenString(10, start_y, content, LCD_WIDTH-20);
  }

  if(reminder.status == STATUS_IDLE) return;
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf));
  GUI_SetColor(FK_COLOR);
}

//Draw the entire interface
void menuDrawPage(const MENUITEMS *menuItems, bool listview)
{
  u8 i = 0;
    isListview = listview;
  curMenuItems = menuItems;
  TSC_ReDrawIcon = itemDrawIconPress;

  if (listview == false)
  {
    //GUI_Clear(BLACK);
    menuClearGaps(); //Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
    menuDrawTitle(menuItems);
    for (i = 0; i < ITEM_PER_PAGE; i++)
    {
      menuDrawItem(&menuItems->items[i], i);
    }
  }
  else
  {
    GUI_Clear(BLACK);
    menuDrawTitle(menuItems);
    for (i = 0; i < ITEM_PER_PAGE; i++)
    {
      menuDrawListItem(&menuItems->items[i], i);
    }
  }
}

//When there is a button value, the icon changes color and redraws
void itemDrawIconPress(u8 positon, u8 is_press)
{
  if (curMenuItems == NULL)
    return;
  if (positon > KEY_ICON_7)
    return;

  if (isListview == false)
  {
    if (curMenuItems->items[positon].icon == ICON_BACKGROUND)
      return;

    const GUI_RECT *rect = rect_of_key + positon;

    if (infoMenu.menu[infoMenu.cur] == menuStatus)
    {
      rect = rect_of_keyStatus + positon;
    }

    if (is_press) //Turn green when pressed
      ICON_PressedDisplay(rect->x0, rect->y0, curMenuItems->items[positon].icon);
    else //Redraw normal icon when released
      ICON_ReadDisplay(rect->x0, rect->y0, curMenuItems->items[positon].icon);
  }
  else
  { //draw rec over list item if pressed
    if (curMenuItems->items[positon].icon == ICONCHAR_BACKGROUND && curMenuItems->items[positon].label.index == LABEL_BACKGROUND)
      return;

    const GUI_RECT *rect = rect_of_keyListView + positon;
/*     ICON_POS ps = LEFT_CENTER;
        if (positon > 4){ ps = MIDDLE;}
        if (is_press) //draw rec over list item if pressed
            ListItem_Display(rect, curMenuItems->items[positon].icon, ps, (u8 *)curMenuItems->items[positon].label, true);
        else //Redraw normal item when released
            ListItem_Display(rect, curMenuItems->items[positon].icon, ps, (u8 *)curMenuItems->items[positon].label, false);
     */
    if (positon > 4)
    {
      GUI_SetBkColor(LISTBTN_BKCOLOR);
      if (is_press) //draw rec over list item if pressed
      {
      ListItem_Display(rect, curMenuItems->items[positon].icon, MIDDLE, curMenuItems->items[positon].label.address, true);
      }
      else{
      ListItem_Display(rect, curMenuItems->items[positon].icon, MIDDLE, curMenuItems->items[positon].label.address, false);
      }
      GUI_SetBkColor(BK_COLOR);
    }
    else
    {
        if (is_press) //draw rec over list item if pressed
            ListItem_Display(rect, curMenuItems->items[positon].icon, LEFT_CENTER, curMenuItems->items[positon].label.address, true);
        else //Redraw normal item when released
            ListItem_Display(rect, curMenuItems->items[positon].icon, LEFT_CENTER, curMenuItems->items[positon].label.address, false);
    }
  
  
  
  }
}

// Get button value
KEY_VALUES menuKeyGetValue(void)
{
  if (infoMenu.menu[infoMenu.cur] == menuStatus) // for status screen
  {
    return (KEY_VALUES)KEY_GetValue(sizeof(rect_of_keyStatus) / sizeof(rect_of_keyStatus[0]), rect_of_keyStatus);
  }
  else
  {
    if (isListview == false)
    {
      return (KEY_VALUES)KEY_GetValue(sizeof(rect_of_key) / sizeof(rect_of_key[0]), rect_of_key);   // for normal menu
    }
    else
    {
      return (KEY_VALUES)KEY_GetValue(sizeof(rect_of_keyListView) / sizeof(rect_of_keyListView[0]), rect_of_keyListView); //for listview
    }
  }
}

/*------------------------about select mode FUN --------------top-------------*/
MKEY_VALUES MKeyGetValue(void)
{    
  return (MKEY_VALUES)KEY_GetValue(sizeof(rect_of_mode)/sizeof(rect_of_mode[0]), rect_of_mode);    
}

void selectmode(int8_t  nowMode)
{
  if(nowMode==SERIAL_TSC)
  {
    GUI_SetColor(ST7920_FNCOLOR);
    GUI_DispStringInRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Touch Mode");
    GUI_SetColor(FK_COLOR);
    GUI_DispStringInRect(0,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,text_startx,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Marlin Mode");
  }
  else
  {
    GUI_SetColor(ST7920_FNCOLOR);
    GUI_DispStringInRect(0,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,text_startx,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Marlin Mode");
    GUI_SetColor(FK_COLOR);
    GUI_DispStringInRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Touch Mode");
  }
}
/*------------------------about select mode FUN --------------end-------------*/
void loopBackEnd(void)
{
  getGcodeFromFile();                 //Get Gcode command from the file to be printed
           
  sendQueueCmd();                     //Parse and send Gcode commands in the queue
  
  parseACK();                         //Parse the received slave response information
  
  parseRcvGcode();                    //Parse the received Gcode from other UART, such as: ESP3D, etc...

  loopCheckHeater();			            //Temperature related settings
  

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
}

void loopProcess(void)
{
  loopBackEnd();
  loopFrontEnd();
}
