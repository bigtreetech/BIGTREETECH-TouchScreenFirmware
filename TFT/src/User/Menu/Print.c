#include "Print.h"
#include "includes.h"

  #ifdef MENU_LIST_MODE
    LISTITEMS printItems = {
    // title
    LABEL_BACKGROUND,
    // icon                 ItemType      Item Title        item value text(only for custom value)
    {
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACK,       LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},}
    };

#else

    MENUITEMS printItems = {
    //  title
    LABEL_BACKGROUND,
    // icon                       label
    {

      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_PAGE_UP,              LABEL_PAGE_UP},
      {ICON_PAGE_DOWN,            LABEL_PAGE_DOWN},
      {ICON_BACK,                 LABEL_BACK},

    }
    };
  #endif

// File list number per page
#define NUM_PER_PAGE	5

SCROLL   titleScroll;
GUI_RECT titleRect={10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH-10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

SCROLL   gcodeScroll;
  
#ifndef MENU_LIST_MODE
/*   GUI_RECT gcodeRect[NUM_PER_PAGE] = { 
    {START_X + BYTE_HEIGHT + 4,  0*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  1*LISTITEM_HEIGHT+TITLE_END_Y-1},
    {START_X + BYTE_HEIGHT + 4,  1*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  2*LISTITEM_HEIGHT+TITLE_END_Y-1},
    {START_X + BYTE_HEIGHT + 4,  2*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  3*LISTITEM_HEIGHT+TITLE_END_Y-1},
    {START_X + BYTE_HEIGHT + 4,  3*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  4*LISTITEM_HEIGHT+TITLE_END_Y-1},
    {START_X + BYTE_HEIGHT + 4,  4*LISTITEM_HEIGHT+TITLE_END_Y+1,    LISTITEM_WIDTH+START_X - 2,  5*LISTITEM_HEIGHT+TITLE_END_Y-1},
  };
#else */
  GUI_RECT gcodeRect[NUM_PER_PAGE] = { 
   {BYTE_WIDTH/2+0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,  
    1*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

   {BYTE_WIDTH/2+1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
    2*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

   {BYTE_WIDTH/2+2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
    3*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

   {BYTE_WIDTH/2+3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
    4*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

   {BYTE_WIDTH/2+0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
    1*SPACE_X_PER_ICON-BYTE_WIDTH/2,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
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
#endif

void normalNameDisp(const GUI_RECT *rect, u8 *name)
{
  if(name == NULL) return;

  GUI_ClearPrect(rect);
  GUI_SetRange(rect->x0, rect->y0, rect->x1, rect->y1);
  #ifdef MENU_LIST_MODE
    GUI_DispString(rect->x0, rect->y0 + (rect->y1 - rect->y0 - BYTE_HEIGHT)/2, name);
  #else
    GUI_DispStringInPrect(rect, name);
  #endif
  GUI_CancelRange();
}


#ifdef MENU_LIST_MODE
  void gocdeListDraw(void)
  {
    u8 i = 0;

    //scrollFileNameCreate(0);
    Scroll_CreatePara(&titleScroll, (u8 *)infoFile.title, &titleRect);
    printItems.title.address = (u8 *)infoFile.title;
    GUI_ClearRect(titleRect.x0, titleRect.y0, titleRect.x1, titleRect.y1);

    for (i = 0; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num) && (i < NUM_PER_PAGE); i++) // folder
    {
      printItems.items[i].icon = ICONCHAR_FOLDER;
      dynamic_label[i] = infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE];
      printItems.items[i].titlelabel.index = LABEL_DYNAMIC;
      menuDrawListItem(&printItems.items[i], i);
    }
    for (; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.f_num + infoFile.F_num) && (i < NUM_PER_PAGE); i++) // gcode file
    {
      printItems.items[i].icon = ICONCHAR_FILE;
      dynamic_label[i] = (infoFile.source == BOARD_SD) ? infoFile.Longfile[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num] : infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num];
      printItems.items[i].titlelabel.index = LABEL_DYNAMIC;
      menuDrawListItem(&printItems.items[i], i);
    }
    for (; (i < NUM_PER_PAGE); i++) //background
    {
      printItems.items[i].icon = ICONCHAR_BACKGROUND;
      printItems.items[i].titlelabel.index = LABEL_BACKGROUND;
      menuDrawListItem(&printItems.items[i], i);
    }
      // set page up down button according to page count and current page
      int t_pagenum = (infoFile.F_num+infoFile.f_num+(LISTITEM_PER_PAGE-1))/LISTITEM_PER_PAGE;
      if ((infoFile.F_num+infoFile.f_num) <= LISTITEM_PER_PAGE)
      {
        printItems.items[5].icon = ICONCHAR_BACKGROUND;
        printItems.items[6].icon = ICONCHAR_BACKGROUND;
      }
      else
      {
        if(infoFile.cur_page == 0){
          printItems.items[5].icon = ICONCHAR_BACKGROUND;
          printItems.items[6].icon = ICONCHAR_PAGEDOWN;
        }
        else if(infoFile.cur_page == (t_pagenum-1)){
          printItems.items[5].icon = ICONCHAR_PAGEUP;
          printItems.items[6].icon = ICONCHAR_BACKGROUND;
        }
        else
        {
          printItems.items[5].icon = ICONCHAR_PAGEUP;
          printItems.items[6].icon = ICONCHAR_PAGEDOWN;
        }
      }
      menuDrawListItem(&printItems.items[5],5);
      menuDrawListItem(&printItems.items[6],6);
  }

#else

  void gocdeListDraw(void)
  {
    u8 i=0;
    int gn;
    char *gnew;
    ITEM curItem = {ICON_BACKGROUND, LABEL_BACKGROUND};

    scrollFileNameCreate(0);
    Scroll_CreatePara(&titleScroll, (u8* )infoFile.title,&titleRect);
    printItems.title.address = (u8 *)infoFile.title;
//    menuDrawTitle(&printItems);
    GUI_ClearRect(titleRect.x0, titleRect.y0, titleRect.x1, titleRect.y1);

    for(i=0;(i + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num)
            &&(i < NUM_PER_PAGE)                                  ; i++)                  // folder
    {
      curItem.icon = ICON_FOLDER;
      menuDrawItem(&curItem, i);
      normalNameDisp(&gcodeRect[i], (u8* )infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
    }
    for(   ;(i + infoFile.cur_page * NUM_PER_PAGE < infoFile.f_num + infoFile.F_num)
            &&(i < NUM_PER_PAGE)                                                   ;i++)  // gcode file
    {
      curItem.icon = ICON_FILE;
      // Preview
      gn = strlen(infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num]) - 6; // -6 means ".gcode"
      gnew = malloc(gn + 10);
      if(gnew)
      {
        strcpy(gnew, getCurFileSource());
        strncat(gnew, infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num], gn);
        if(bmpDecode(strcat(gnew, "_"STRINGIFY(ICON_WIDTH)".bmp"), ICON_ADDR(ICON_PREVIEW+i)))
          curItem.icon = ICON_PREVIEW+i;
        free(gnew);
      }
      menuDrawItem(&curItem, i);
      normalNameDisp(&gcodeRect[i], (u8* )infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num]);
    }
    for(; (i<NUM_PER_PAGE); i++)			//background
    {		
      curItem.icon = ICON_BACKGROUND;
      menuDrawItem(&curItem, i);
    }
  }
#endif

const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

void menuPrintFromSource(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  u8 update=0;

  GUI_Clear(BK_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING));

  if (mountFS() == true && scanPrintFiles() == true)
  {
      #ifdef MENU_LIST_MODE
        menuDrawListPage(&printItems);
            gocdeListDraw();	
      #else
        menuDrawPage(&printItems);
      #endif
	
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
    #ifndef MENU_LIST_MODE
      Scroll_DispString(&gcodeScroll, CENTER); //
    #endif

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
            if(EnterDir(infoFile.folder[key_num + start]) == false)  break;						
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
        #ifndef MENU_LIST_MODE                
        else if(key_num >=KEY_LABEL_0 && key_num <= KEY_LABEL_4)     
        {                  
          if(key_num - KEY_LABEL_0 + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num + infoFile.f_num)
          {
            normalNameDisp(gcodeScroll.rect, gcodeScroll.text);
            scrollFileNameCreate(key_num - KEY_LABEL_0);
          }
        }
        #endif
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
 #ifdef ONBOARD_SD_SUPPORT
  {ICON_BSD_SOURCE,           LABEL_ONBOARDSD},
 #endif
 #ifdef U_DISK_SUPPROT
  {ICON_U_DISK,               LABEL_U_DISK},
 #else
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
 #endif
 #ifndef ONBOARD_SD_SUPPORT
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
  
  menuDrawPage(&sourceSelItems);
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
      
      #ifdef ONBOARD_SD_SUPPORT
      case KEY_ICON_1:
        infoFile.source = BOARD_SD;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;   //TODO: fix here,  onboard sd card PLR feature
        goto selectEnd;
      #endif
      
      #ifdef U_DISK_SUPPROT
      #ifdef ONBOARD_SD_SUPPORT
      case KEY_ICON_2:
      #else
      case KEY_ICON_1:
      #endif
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
