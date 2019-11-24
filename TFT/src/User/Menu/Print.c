#include "Print.h"
#include "includes.h"

MENUITEMS printItems = {
//  title
LABEL_BACKGROUND,
// icon                       label
 {{ICONCHAR_BACKGROUND,           LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,           LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,           LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,           LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,           LABEL_BACKGROUND},
  {ICONCHAR_PAGEUP,           LABEL_BACKGROUND},
  {ICONCHAR_PAGEDOWN,         LABEL_BACKGROUND},
  {ICONCHAR_BACK,             LABEL_BACKGROUND},}
};


// File list number per page
#define NUM_PER_PAGE	5

SCROLL   titleScroll;
GUI_RECT titleRect={10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH-10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

SCROLL   gcodeScroll;
  
GUI_RECT gcodeRect[NUM_PER_PAGE]= { 
  {START_X + BYTE_HEIGHT + 4,  0*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  1*LISTITEM_HEIGHT+TITLE_END_Y-1},
  {START_X + BYTE_HEIGHT + 4,  1*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  2*LISTITEM_HEIGHT+TITLE_END_Y-1},
  {START_X + BYTE_HEIGHT + 4,  2*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  3*LISTITEM_HEIGHT+TITLE_END_Y-1},
  {START_X + BYTE_HEIGHT + 4,  3*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  4*LISTITEM_HEIGHT+TITLE_END_Y-1},
  {START_X + BYTE_HEIGHT + 4,  4*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  5*LISTITEM_HEIGHT+TITLE_END_Y-1},
};

void scrollFileNameCreate(u8 i)
{
  
  u8 num=infoFile.cur_page * NUM_PER_PAGE + i;	

  if(infoFile.F_num + infoFile.f_num==0)
  {
    memset(&gcodeScroll,0,sizeof(SCROLL));
    return;
  }
  if(num<infoFile.F_num)
  {
    Scroll_CreatePara(&gcodeScroll, (u8* )infoFile.folder[num],&gcodeRect[i]);
  }
  else if(num<infoFile.F_num+infoFile.f_num)
  {
    Scroll_CreatePara(&gcodeScroll, (u8* )infoFile.file[num-infoFile.F_num],&gcodeRect[i]);
  }
}

void normalNameDisp(const GUI_RECT *rect, u8 *name)
{
  if(name == NULL) return;
  uint16_t w = rect->x1 - rect->x0;
  uint16_t h = rect->y1 - rect->y0;

  GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);
  GUI_SetRange(rect->x0, rect->y0, rect->x1, rect->y1);
  GUI_DispString(rect->x0,rect->y0 + (h - BYTE_HEIGHT)/2, name);
  GUI_CancelRange();
}

void gocdeListDraw(void)
{
  u8 i = 0;
  ITEM curItem = {ICONCHAR_BACKGROUND, LABEL_BACKGROUND};

  //scrollFileNameCreate(0);
  Scroll_CreatePara(&titleScroll, (u8 *)infoFile.title, &titleRect);
  GUI_ClearRect(titleRect.x0, titleRect.y0, titleRect.x1, titleRect.y1);

  for (i = 0; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num) && (i < NUM_PER_PAGE); i++) // folder
  {
    curItem.icon = ICONCHAR_FOLDER;
    //curItem.label = infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE];
    menuDrawListItem(&curItem, i);
    normalNameDisp(&gcodeRect[i], (u8 *)infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
  }
  for (; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.f_num + infoFile.F_num) && (i < NUM_PER_PAGE); i++) // gcode file
  {
    curItem.icon = ICONCHAR_FILE;
    curItem.label = infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE];
    menuDrawListItem(&curItem, i);
    normalNameDisp(&gcodeRect[i], (u8 *)infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num]);
  }
  for (; (i < NUM_PER_PAGE); i++) //background
  {
    curItem.icon = ICONCHAR_BACKGROUND;
    curItem.label = LABEL_BACKGROUND;
    menuDrawListItem(&curItem, i);
  }
}

const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

void menuPrintFromSource(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  u8 update=0;

  GUI_Clear(BK_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING));

  if (mountFS() == true && scanPrintFiles() == true)
  {
    menuDrawPage(&printItems,true);
    gocdeListDraw();		
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(labelVolumeError[infoFile.source]));
    Delay_ms(1000);
    infoMenu.cur--;
  }

  while(infoMenu.menu[infoMenu.cur] == menuPrintFromSource)
  {
    Scroll_DispString(&titleScroll, LEFT);    //
    //Scroll_DispString(&gcodeScroll, LEFT);  //

    key_num = menuKeyGetValue();

    switch(key_num)
    {
      case KEY_ICON_5:			
        if(infoFile.cur_page > 0)
        {
          infoFile.cur_page--;
          update=1;
        }
        break;

      case KEY_ICON_6:	
        if(infoFile.cur_page+1 < (infoFile.F_num+infoFile.f_num+(NUM_PER_PAGE-1))/NUM_PER_PAGE)
        {
          infoFile.cur_page++;
          update=1;
        }	
        break;

      case KEY_ICON_7:
        infoFile.cur_page = 0;
        if(IsRootDir() == true)
        {
          clearInfoFile();
          infoMenu.cur--;
          break;
        }
        else
        {
          ExitDir();				
          scanPrintFiles();			
          update = 1;
        }
        break;

      case KEY_IDLE:
        break;

      default:                   
        if(key_num <= KEY_ICON_4)
        {	
          u16 start = infoFile.cur_page * NUM_PER_PAGE;
          if(key_num + start < infoFile.F_num)						//folder
          {
            if(EnterDir(infoFile.folder[key_num + start])==false)  break;						
            scanPrintFiles();
            update=1;
            infoFile.cur_page=0;
          }
          else if(key_num+start < infoFile.F_num+infoFile.f_num)	//gcode
          {	
            if(infoHost.connected !=true) break;
            if(EnterDir(infoFile.file[key_num + start - infoFile.F_num]) == false) break;	
            
            infoMenu.menu[++infoMenu.cur] = menuBeforePrinting;	
          }				
        }                  
        else if(key_num >=KEY_LABEL_0 && key_num <= KEY_LABEL_4)     
        {                  
          if(key_num - KEY_LABEL_0 + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num + infoFile.f_num)
          {
            normalNameDisp(gcodeScroll.rect, gcodeScroll.text);
            //scrollFileNameCreate(key_num - KEY_LABEL_0);
          }
        }	
        break;
    }
    
    if(update)
    {
      update=0;
      gocdeListDraw();
    }
    
    #ifdef SD_CD_PIN
    if(isVolumeExist(infoFile.source) != true)
    {
      resetInfoFile();
      infoMenu.cur--;
    }
    #endif
    loopProcess();
  }
}

MENUITEMS sourceSelItems = {
//  title
LABEL_PRINT,
// icon                       label
 {{ICON_SD_SOURCE,            LABEL_TFTSD},
  {ICON_BSD_SOURCE,           LABEL_ONBOARDSD},
 #ifdef U_DISK_SUPPROT
  {ICON_U_DISK,               LABEL_U_DISK},
 #else
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
 #endif
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuPrint(void)
{
  KEY_VALUES  key_num = KEY_IDLE;
  
  menuDrawPage(&sourceSelItems,false);
  while(infoMenu.menu[infoMenu.cur] == menuPrint)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoFile.source = TFT_SD;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;
        infoMenu.menu[++infoMenu.cur] = menuPowerOff;
        goto selectEnd;
      
      case KEY_ICON_1:
        infoFile.source = BOARD_SD;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;   //TODO: fix here,  onboard sd card PLR feature
        goto selectEnd;
      
      #ifdef U_DISK_SUPPROT
      case KEY_ICON_2:
        infoFile.source = TFT_UDISK;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;
        infoMenu.menu[++infoMenu.cur] = menuPowerOff;
        goto selectEnd;
      #endif
      
      case KEY_ICON_7:
        infoMenu.cur--;
        return;
      
      default: break;
    }
    loopProcess();
  }
  
selectEnd:  
  resetInfoFile();
  powerFailedSetDriverSource(getCurFileSource());
}
