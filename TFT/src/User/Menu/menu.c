#include "menu.h"
#include "includes.h"

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

  for(int i=0; i < sizeof(gaps)/sizeof(gaps[0]); i++)
  GUI_ClearRect(gaps[i].x0, gaps[i].y0, gaps[i].x1, gaps[i].y1);
}

static const MENUITEMS * curMenuItems = NULL;   //current menu

void menuDrawItem(const ITEM * item, u8 positon)
{
  const GUI_RECT *rect = rect_of_key + positon;    
  if(item->icon != ICON_BACKGROUND)
    ICON_ReadDisplay(rect->x0, rect->y0, item->icon);
  else
    GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);

  rect = rect_of_key + ITEM_PER_PAGE + positon;
  GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);
  if(item->label != LABEL_BACKGROUND)
    GUI_DispStringInPrect(rect,textSelect(item->label),0);
}


static REMINDER reminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_UNCONNECT, LABEL_UNCONNECTED};
static REMINDER volumeReminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_IDLE, LABEL_BACKGROUND};
static REMINDER busySign = {{LCD_WIDTH - 5, 0, LCD_WIDTH, 5}, 0, STATUS_BUSY, LABEL_BUSY};


void reminderMessage(int16_t inf, SYS_STATUS status)
{    
  reminder.inf = inf;
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf), 1);
  GUI_SetColor(FK_COLOR);
  reminder.status = status;
  reminder.time = OS_GetTime();
}

void volumeReminderMessage(int16_t inf, SYS_STATUS status)
{    
  volumeReminder.inf = inf;
  GUI_SetColor(GBLUE);
  GUI_DispStringInPrect(&volumeReminder.rect, textSelect(volumeReminder.inf), 1);
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
      if(OS_GetTime()<volumeReminder.time+200)
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
  u16 start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;
  GUI_ClearRect(10, start_y, LCD_WIDTH-10, start_y+BYTE_HEIGHT);
  
  if(menuItems->title != LABEL_BACKGROUND)
    GUI_DispString(10, start_y, textSelect(menuItems->title),0);

  if(reminder.status == STATUS_IDLE) return;
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf), 1);
  GUI_SetColor(FK_COLOR);
}

//Draw the entire interface
void menuDrawPage(const MENUITEMS * menuItems)
{
  u8 i=0;
  curMenuItems = menuItems;
  TSC_ReDrawIcon = itemDrawIconPress;
  //    GUI_Clear(BLACK);
  menuClearGaps();     //Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
  menuDrawTitle(menuItems);
  for(i=0; i<ITEM_PER_PAGE; i++)
  {
    menuDrawItem(&menuItems->items[i], i);            
  }
}

//When there is a button value, the icon changes color and redraws
void itemDrawIconPress(u8 positon, u8 is_press)
{
  if(curMenuItems == NULL)                                  return;
  if(positon > KEY_ICON_7)                                  return;
  if(curMenuItems->items[positon].icon == ICON_BACKGROUND)  return;

  const GUI_RECT *rect = rect_of_key + positon;   
  if(is_press)   //Turn green when pressed
    ICON_PressedDisplay(rect->x0, rect->y0, curMenuItems->items[positon].icon);
  else           //Redraw normal icon when released
    ICON_ReadDisplay(rect->x0, rect->y0, curMenuItems->items[positon].icon);
}

//Get button value
KEY_VALUES menuKeyGetValue(void)
{    
  return(KEY_VALUES)KEY_GetValue(sizeof(rect_of_key)/sizeof(rect_of_key[0]), rect_of_key);    
}


void loopProcess(void)
{
  getGcodeFromFile();                 //Get Gcode command from the file to be printed
           
  sendQueueCmd();                     //Parse and send Gcode commands in the queue
  
  parseACK();                         //Parse the received slave response information
  
  parseRcvGcode();                    //Parse the received Gcode from other UART, such as: ESP3D, etc...

  loopCheckHeater();			            //Temperature related settings
  
  loopVolumeSource();                 //Check if volume source(SD/U disk) insert

#if defined ONBOARD_SD_SUPPORT && !defined M27_AUTOREPORT
  loopCheckPrinting();                //Check if there is a SD or USB print running.
#endif

  loopReminderClear();	              //If there is a message in the status bar, timed clear
  
  loopVolumeReminderClear();

  loopBusySignClear();                //Busy Indicator clear
  
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
