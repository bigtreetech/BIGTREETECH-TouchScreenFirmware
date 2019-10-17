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

const GUI_RECT rect_of_Gkey[24]={
  //24 GKEY
  {0*GCODE_KEYW+0*SPACE_GKX+START_GKX, 0*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y, 1*GCODE_KEYW+0*SPACE_GKX+START_GKX, 1*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y},
  {1*GCODE_KEYW+1*SPACE_GKX+START_GKX, 0*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y, 2*GCODE_KEYW+1*SPACE_GKX+START_GKX, 1*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y},
  {2*GCODE_KEYW+2*SPACE_GKX+START_GKX, 0*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y, 3*GCODE_KEYW+2*SPACE_GKX+START_GKX, 1*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y},
  {3*GCODE_KEYW+3*SPACE_GKX+START_GKX, 0*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y, 4*GCODE_KEYW+3*SPACE_GKX+START_GKX, 1*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y},
  {4*GCODE_KEYW+4*SPACE_GKX+START_GKX, 0*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y, 5*GCODE_KEYW+4*SPACE_GKX+START_GKX, 1*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y},
  {5*GCODE_KEYW+5*SPACE_GKX+START_GKX, 0*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y, 6*GCODE_KEYW+5*SPACE_GKX+START_GKX, 1*GCODE_KEYH+0*SPACE_GKY+TITLE_END_Y},
  {0*GCODE_KEYW+0*SPACE_GKX+START_GKX, 1*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y, 1*GCODE_KEYW+0*SPACE_GKX+START_GKX, 2*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y},
  {1*GCODE_KEYW+1*SPACE_GKX+START_GKX, 1*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y, 2*GCODE_KEYW+1*SPACE_GKX+START_GKX, 2*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y},
  {2*GCODE_KEYW+2*SPACE_GKX+START_GKX, 1*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y, 3*GCODE_KEYW+2*SPACE_GKX+START_GKX, 2*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y},
  {3*GCODE_KEYW+3*SPACE_GKX+START_GKX, 1*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y, 4*GCODE_KEYW+3*SPACE_GKX+START_GKX, 2*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y},
  {4*GCODE_KEYW+4*SPACE_GKX+START_GKX, 1*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y, 5*GCODE_KEYW+4*SPACE_GKX+START_GKX, 2*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y},
  {5*GCODE_KEYW+5*SPACE_GKX+START_GKX, 1*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y, 6*GCODE_KEYW+5*SPACE_GKX+START_GKX, 2*GCODE_KEYH+1*SPACE_GKY+TITLE_END_Y},
  {0*GCODE_KEYW+0*SPACE_GKX+START_GKX, 2*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y, 1*GCODE_KEYW+0*SPACE_GKX+START_GKX, 3*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y},
  {1*GCODE_KEYW+1*SPACE_GKX+START_GKX, 2*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y, 2*GCODE_KEYW+1*SPACE_GKX+START_GKX, 3*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y},
  {2*GCODE_KEYW+2*SPACE_GKX+START_GKX, 2*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y, 3*GCODE_KEYW+2*SPACE_GKX+START_GKX, 3*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y},
  {3*GCODE_KEYW+3*SPACE_GKX+START_GKX, 2*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y, 4*GCODE_KEYW+3*SPACE_GKX+START_GKX, 3*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y},
  {4*GCODE_KEYW+4*SPACE_GKX+START_GKX, 2*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y, 5*GCODE_KEYW+4*SPACE_GKX+START_GKX, 3*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y},
  {5*GCODE_KEYW+5*SPACE_GKX+START_GKX, 2*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y, 6*GCODE_KEYW+5*SPACE_GKX+START_GKX, 3*GCODE_KEYH+2*SPACE_GKY+TITLE_END_Y},
  {0*GCODE_KEYW+0*SPACE_GKX+START_GKX, 3*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y, 1*GCODE_KEYW+0*SPACE_GKX+START_GKX, 4*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y},
  {1*GCODE_KEYW+1*SPACE_GKX+START_GKX, 3*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y, 2*GCODE_KEYW+1*SPACE_GKX+START_GKX, 4*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y},
  {2*GCODE_KEYW+2*SPACE_GKX+START_GKX, 3*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y, 3*GCODE_KEYW+2*SPACE_GKX+START_GKX, 4*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y},
  {3*GCODE_KEYW+3*SPACE_GKX+START_GKX, 3*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y, 4*GCODE_KEYW+3*SPACE_GKX+START_GKX, 4*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y},
  {4*GCODE_KEYW+4*SPACE_GKX+START_GKX, 3*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y, 5*GCODE_KEYW+4*SPACE_GKX+START_GKX, 4*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y},
  {5*GCODE_KEYW+5*SPACE_GKX+START_GKX, 3*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y, 6*GCODE_KEYW+5*SPACE_GKX+START_GKX, 4*GCODE_KEYH+3*SPACE_GKY+TITLE_END_Y},
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

/*------------------------about GCODE KEY FUN ---------------top-------------*/
GKEY_VALUES GKeyGetValue(void)
{    
  return (GKEY_VALUES)KEY_GetValue(sizeof(rect_of_Gkey)/sizeof(rect_of_Gkey[0]), rect_of_Gkey);    
}

void DrawGKEY(void)
{
  uint8_t i;
  char *key[24]={"1","2","3","M","G","¡û","4","5","6","X","Y","Spac","7","8","9","Z","E","A-Z",".","0","-"," "," ","Buzz"};
  for(i=0;i<24;i++)
  {
    GUI_DispStringInRect(rect_of_Gkey[i].x0, rect_of_Gkey[i].y0, rect_of_Gkey[i].x1, rect_of_Gkey[i].y1,(uint8_t *)key[i], 0);
  }
  
}

void DrawEGKEY(void)
{
  uint8_t i;
  char *key[24]={"A","B","C","D","F","¡û","H","I","K","L","N","Spac","O","P","Q","R","S","A-Z","T","U","V","W"," "," "};
  for(i=0;i<24;i++)
  {
    GUI_DispStringInRect(rect_of_Gkey[i].x0, rect_of_Gkey[i].y0, rect_of_Gkey[i].x1, rect_of_Gkey[i].y1,(uint8_t *)key[i], 0);
  }

}

bool revinfo(void)
{
  u16 x=0, y=0;
  int i=0;
  bool keyback = true;
  bool only = false;
  u8 bufpage = 1;
  
  char showbuf1[(LCD_WIDTH/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT];
  char showbuf2[(LCD_WIDTH/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT];
  char showbuf3[(LCD_WIDTH/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT];
 
  while(i<((LCD_WIDTH-2*BYTE_WIDTH)/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT)
  {
    showbuf1[i] = ack_rev_buf[i];
    i++;  
  }
  GUI_SetColor(GRAY);
  GUI_FillRect(0,SPACE_GKY+TITLE_END_Y,LCD_WIDTH,LCD_HEIGHT);
  GUI_SetColor(BK_COLOR);
  GUI_DispStringInRect(0, SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT, LCD_WIDTH,LCD_HEIGHT-BYTE_HEIGHT,(uint8_t *)showbuf1,0);
    
  uint16_t stringlen = my_strlen((u8*)showbuf1)*BYTE_WIDTH;
  uint16_t width = LCD_WIDTH;
  uint16_t height = (LCD_HEIGHT-BYTE_HEIGHT) - (SPACE_GKY+TITLE_END_Y);
  uint8_t  nline = (stringlen+width-1)/width ;
   
  if(nline >= height/BYTE_HEIGHT)
  {
  only = true;
  i = 0; 
  while(i<((LCD_WIDTH-2*BYTE_WIDTH)/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT)
  {
    showbuf2[i] = ack_rev_buf[((LCD_WIDTH-2*BYTE_WIDTH)/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT+i];
    i++;  
  }
  
  GUI_DispStringInRect(LCD_WIDTH/2,LCD_HEIGHT-BYTE_HEIGHT,LCD_WIDTH,LCD_HEIGHT,(uint8_t *)"¡·",0);

  i = 0;
  while(i<((LCD_WIDTH-2*BYTE_WIDTH)/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT)
  {
    showbuf3[i] = ack_rev_buf[2*((LCD_WIDTH-2*BYTE_WIDTH)/BYTE_WIDTH)*(LCD_HEIGHT-(SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT))/BYTE_HEIGHT+i];
    i++;
  }
  }    

  
  while(keyback)
  {
    while(XPT2046_Read_Pen());
    TS_Get_Coordinates(&x,&y);
    if(x>LCD_WIDTH-(GCODE_KEYW+START_GKX) && x<LCD_WIDTH-10 && y>10 && y<10+BYTE_HEIGHT+5)
    {
    #ifdef BUZZER_PIN
    if(infoSettings.buzzer == 1)
    openBuzzer(3, 11);
    #endif
    keyback = false;
    }
    
    if(x>0 && x<LCD_WIDTH/2 && y>LCD_HEIGHT-BYTE_HEIGHT && y<LCD_HEIGHT && bufpage==2 && only)
    {
    while(XPT2046_Read_Pen());
    GUI_SetColor(GRAY);
    GUI_FillRect(0,SPACE_GKY+TITLE_END_Y,LCD_WIDTH,LCD_HEIGHT);
    GUI_SetColor(BK_COLOR);
    GUI_DispStringInRect(0,SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT,LCD_WIDTH,LCD_HEIGHT-BYTE_HEIGHT,(uint8_t *)showbuf1,0);
    GUI_DispStringInRect(LCD_WIDTH/2,LCD_HEIGHT-BYTE_HEIGHT,LCD_WIDTH,LCD_HEIGHT,(uint8_t *)"¡·",0);
    bufpage = 1;
    }
    
    if(  (x>LCD_WIDTH/2 && x<LCD_WIDTH && y>LCD_HEIGHT-BYTE_HEIGHT && y<LCD_HEIGHT && bufpage==1  && only)
      || (x>0 && x<LCD_WIDTH/2 && y>LCD_HEIGHT-BYTE_HEIGHT && y<LCD_HEIGHT && bufpage==3 && only)
      )
    {
    while(XPT2046_Read_Pen());
    GUI_SetColor(GRAY);
    GUI_FillRect(0,SPACE_GKY+TITLE_END_Y,LCD_WIDTH,LCD_HEIGHT);
    GUI_SetColor(BK_COLOR);
    GUI_DispStringInRect(0,SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT,LCD_WIDTH,LCD_HEIGHT-BYTE_HEIGHT,(uint8_t *)showbuf2,0);
    GUI_DispStringInRect(0,LCD_HEIGHT-BYTE_HEIGHT,LCD_WIDTH/2,LCD_HEIGHT,(uint8_t *)"¡¶",0);
    GUI_DispStringInRect(LCD_WIDTH/2,LCD_HEIGHT-BYTE_HEIGHT,LCD_WIDTH,LCD_HEIGHT,(uint8_t *)"¡·",0);
    bufpage = 2;
    }
    
    if(x>LCD_WIDTH/2 && x<LCD_WIDTH && y>LCD_HEIGHT-BYTE_HEIGHT && y<LCD_HEIGHT && bufpage==2 && only)
    {
    while(XPT2046_Read_Pen());
    GUI_SetColor(GRAY);
    GUI_FillRect(0,SPACE_GKY+TITLE_END_Y,LCD_WIDTH,LCD_HEIGHT);
    GUI_SetColor(BK_COLOR);
    GUI_DispStringInRect(0,SPACE_GKY+TITLE_END_Y+BYTE_HEIGHT,LCD_WIDTH,LCD_HEIGHT-BYTE_HEIGHT,(uint8_t *)showbuf3,0);
    GUI_DispStringInRect(0,LCD_HEIGHT-BYTE_HEIGHT,LCD_WIDTH/2,LCD_HEIGHT,(uint8_t *)"¡¶",0);
    //GUI_DispStringInRect(10, 10, LCD_WIDTH-10,LCD_HEIGHT-10,"showbuf3",0);
    bufpage = 3;
    }
    
    if(BackGKEY())
      return true;
  }
  while(XPT2046_Read_Pen());
  return false;
}

bool SendGcode(void)
{
  u16 x=0, y=0;
  TS_Get_Coordinates(&x,&y);
  if(x>LCD_WIDTH-GCODE_KEYW+START_GKX && x<LCD_WIDTH-10 && y>10 && y<10+BYTE_HEIGHT+5)
  {
  #ifdef BUZZER_PIN
  if(infoSettings.buzzer == 1)
	openBuzzer(3, 11);
  #endif
  return true;
  }
  else
  return false;
}

bool BackGKEY(void)
{
  u16 x=0, y=0;
  
  TS_Get_Coordinates(&x,&y);
  if(x>10 && x<GCODE_KEYW+START_GKX && y>10 && y<10+BYTE_HEIGHT+5)
  {
  #ifdef BUZZER_PIN
  if(infoSettings.buzzer == 1)
	openBuzzer(3, 11);
  #endif
  return true;
  }
  else
  return false;
}

void showb(void)
{
  GUI_SetColor(FK_COLOR);
  GUI_FillRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5);
  GUI_SetColor(GRAY);
  GUI_FillRect(0,SPACE_GKY+TITLE_END_Y,LCD_WIDTH,LCD_HEIGHT);
  GUI_SetColor(YELLOW);
  GUI_FillRect(LCD_WIDTH-(GCODE_KEYW+START_GKX),10,LCD_WIDTH-10,10+BYTE_HEIGHT+5);
  GUI_SetColor(BK_COLOR);
  GUI_DispStringInRect(LCD_WIDTH-(GCODE_KEYW+START_GKX),10,LCD_WIDTH-10,10+BYTE_HEIGHT+5,(uint8_t *)"GKEY", 0);
}
/*------------------------about GCODE KEY FUN ---------------end-------------*/

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
  GUI_DispStringInRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Touch Mode", 0);
  GUI_SetColor(FK_COLOR);
  GUI_DispStringInRect(0,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,text_startx,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Marlin's Mode", 0);
  }
  else
  {
  GUI_SetColor(ST7920_FNCOLOR);
  GUI_DispStringInRect(0,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,text_startx,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Marlin's Mode", 0);
  GUI_SetColor(FK_COLOR);
  GUI_DispStringInRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Touch Mode", 0);
  }
}
/*------------------------about select mode FUN --------------end-------------*/

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
