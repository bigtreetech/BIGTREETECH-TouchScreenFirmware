#include "Print.h"
#include "includes.h"

MENUITEMS printItems = {
//  title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_PAGE_UP,              LABEL_PAGE_UP},
  {ICON_PAGE_DOWN,            LABEL_PAGE_DOWN},
  {ICON_BACK,                 LABEL_BACK},}
};


// file list number per page

#define NUM_PER_PAGE	5

SCROLL   titleScroll;
GUI_RECT titleRect={10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH-10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

SCROLL   gcodeScroll;
GUI_RECT gcodeRect[NUM_PER_PAGE]= { 
  {2+0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,  
   1*SPACE_X_PER_ICON-2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
  
  {2+1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   2*SPACE_X_PER_ICON-2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
  
  {2+2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   3*SPACE_X_PER_ICON-2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
  
  {2+3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   4*SPACE_X_PER_ICON-2,  1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
  
  {2+0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   1*SPACE_X_PER_ICON-2,  2*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
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

#define NAME_SPACE (SPACE_X_PER_ICON-4)
#define NAME_LEN   (NAME_SPACE/BYTE_WIDTH)
void normalNameDisp(GUI_RECT *rect, u8 *name)
{
  u8  len, x_offset; 

  if(name == NULL) return;

  len = my_strlen((u8 *)name);	
  if(len > NAME_LEN)
  {		
    len = NAME_LEN;
  }
  x_offset=((NAME_SPACE - (len * BYTE_WIDTH)) >> 1);	
  GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);
  GUI_SetRange(rect->x0+x_offset, rect->y0, rect->x0+x_offset+len*BYTE_WIDTH, rect->y0+BYTE_HEIGHT);
  GUI_DispLenString(rect->x0+x_offset, rect->y0, (u8 *)name, 0, len);
  GUI_CancelRange();
}

void gocdeListDraw(void)
{
  u8 i=0;
  ITEM curItem = {ICON_BACKGROUND, LABEL_BACKGROUND};

  scrollFileNameCreate(0);
  Scroll_CreatePara(&titleScroll, (u8* )infoFile.title,&titleRect);
  GUI_ClearRect(titleRect.x0, titleRect.y0, titleRect.x1, titleRect.y1);

  for(i=0;(i + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num)
          &&(i < NUM_PER_PAGE)                                  ; i++)                  //folder
  {
    curItem.icon = ICON_FOLDER;
    menuDrawItem(&curItem, i);
    normalNameDisp(&gcodeRect[i], (u8* )infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
  }
  for(   ;(i + infoFile.cur_page * NUM_PER_PAGE < infoFile.f_num + infoFile.F_num)
          &&(i < NUM_PER_PAGE)                                                   ;i++)  //file
  {	
    curItem.icon = ICON_FILE;
    menuDrawItem(&curItem, i);
    normalNameDisp(&gcodeRect[i], (u8* )infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num]);
  }
  for(; (i<NUM_PER_PAGE); i++)			//background
  {		
    curItem.icon = ICON_BACKGROUND;
    menuDrawItem(&curItem, i);
  }
}

const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};
void menuPrintFromSource(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  u8 update=0;

  GUI_Clear(BK_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING), 1);

  if (mountFS() == true && scanPrintFiles() == true)
  {
    menuDrawPage(&printItems);
    gocdeListDraw();		
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(labelVolumeError[infoFile.source]), 1);
    Delay_ms(1000);
    infoMenu.cur--;
  }

  while(infoMenu.menu[infoMenu.cur] == menuPrintFromSource)
  {
    Scroll_DispString(&titleScroll,1,LEFT);    //
    Scroll_DispString(&gcodeScroll,1,CENTER);  //

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
            scrollFileNameCreate(key_num - KEY_LABEL_0);
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

/*----------------------------------send GCODE------------------------------------------------top*/
void menuSendGcode(void)
{
    SETTINGS now = infoSettings;
    char p[24] = {'\0'};
    char n[24] = {'1','2','3','M','G',' ','4','5','6','X','Y','.','7','8','9','Z','E',' ','.','0','-',' ',' ',' '};
    char e[24] = {'A','B','C','D','F',' ','H','I','K','L','N',' ','O','P','Q','R','S',' ','T','U','V','W','S',' '};
    uint8_t i,k,a;
    uint8_t j=0;
    bool setcol = false;
    bool send = false;
    bool keyturn = false;
    bool back = false;
    TGCODE = 1;
    GKEY_VALUES  key_num = GKEY_IDLE;
    GUI_Clear(FK_COLOR);
    GUI_SetColor(BLUE);
    GUI_FillRect(0,0,LCD_WIDTH,(SPACE_GKY+TITLE_END_Y));
    GUI_SetColor(FK_COLOR);
    GUI_FillRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5);
    GUI_SetColor(YELLOW);
    GUI_FillRect(10,10,GCODE_KEYW+START_GKX,10+BYTE_HEIGHT+5);//Back
    GUI_FillRect(LCD_WIDTH-GCODE_KEYW-START_GKX,10,LCD_WIDTH-10,10+BYTE_HEIGHT+5);//Send
    GUI_SetColor(BK_COLOR);
    GUI_DispStringInRect(LCD_WIDTH-GCODE_KEYW-START_GKX,10,LCD_WIDTH-10,10+BYTE_HEIGHT+5,"Send", 0);
    GUI_DispStringInRect(10,10,GCODE_KEYW+START_GKX,10+BYTE_HEIGHT+5,"Back", 0);
      
    while(infoMenu.menu[infoMenu.cur] == menuSendGcode)
    {
      key_num = GKeyGetValue();
      send = false;
      
      if(keyturn)
      {
      DrawEGKEY();
      for(k=0;k<24;k++)
      {
        if(key_num == GKEY_5) break;
        if(key_num == GKEY_11) break;
        if(key_num == GKEY_17) break;
        if(key_num == (GKEY_VALUES)k)
        {
        p[j] = e[k];
        j++;
        GUI_SetColor(FK_COLOR);
        GUI_FillRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5);
        GUI_SetColor(BK_COLOR);
        break;
        }
      }
      }
      else
      {
      DrawGKEY();
      for(i=0;i<24;i++)
      {
        if(key_num == GKEY_5) break;
        if(key_num == GKEY_11) break;
        if(key_num == GKEY_17) break;
        if(key_num == GKEY_23) break;
        if(key_num == (GKEY_VALUES)i)
        {
        p[j] = n[i];
        j++;
        GUI_SetColor(FK_COLOR);
        GUI_FillRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5);
        GUI_SetColor(BK_COLOR);
        break;
        }
      }        
      }
      
      if(key_num == GKEY_17)//ÇÐ»»
      {
        if(keyturn)
        keyturn = false;
        else
        keyturn = true;
        GUI_SetColor(FK_COLOR);
        GUI_FillRect(0,(SPACE_GKY+TITLE_END_Y),LCD_WIDTH,LCD_HEIGHT);
        GUI_SetColor(BK_COLOR);
      }
      
      if(SendGcode() && p[0]!='\0')//·¢ËÍÃüÁî
      {
        while(!XPT2046_Read_Pen());
        p[j] = '\n';
        while(!storeCmd(p));
        send = true;
        j=0;
      }
      
      if(key_num == GKEY_11)
      {
        p[j] = ' ';
        j++;
        GUI_SetColor(FK_COLOR);
        GUI_FillRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5);
        GUI_SetColor(BK_COLOR);
      }
      
      if(key_num == GKEY_23)//buzzer
      {
          infoSettings.buzzer = !infoSettings.buzzer;
      }
      
      if(key_num == GKEY_5)
      {
        p[j] = '\0';
        j--;
        GUI_SetColor(FK_COLOR);
        GUI_FillRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5);
        GUI_SetColor(BK_COLOR);
      }
      
      if(BackGKEY()||back)//·µ»Ø
      {
        setcol = true;
        infoMenu.cur--;
      }
      if(setcol)
      GUI_SetColor(FK_COLOR);
      else
      GUI_SetColor(BK_COLOR);
      
      sendQueueCmd();
      parseACK();
      
      if(send)
      {
      showb();
      GUI_DispLenString(10,SIMULATOR_YSTART,(uint8_t *)p,0,(LCD_WIDTH-30)/BYTE_WIDTH);
      if(revinfo())
      {
      back=true;
      }
      else
      {
      GUI_SetColor(FK_COLOR);
      GUI_FillRect(0,SPACE_GKY+TITLE_END_Y,LCD_WIDTH,LCD_HEIGHT);
      GUI_SetColor(YELLOW);
      GUI_FillRect(LCD_WIDTH-GCODE_KEYW-START_GKX,10,LCD_WIDTH-10,10+BYTE_HEIGHT+5);
      GUI_SetColor(BK_COLOR);
      GUI_DispStringInRect(LCD_WIDTH-GCODE_KEYW-START_GKX,10,LCD_WIDTH-10,10+BYTE_HEIGHT+5,(uint8_t *)"Send", 0);
      }
      }
      
      if(j<24)
      {
      for(a =j;a<24;a++)
      {
      p[a] = '\0';
      }
      GUI_DispStringInRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5,(uint8_t *)p, 0);
      }
      else
      j=0;
    }
    TGCODE = 0;
    
    while(!XPT2046_Read_Pen());
    
    if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }     
}
/*----------------------------------send GCODE------------------------------------------------end*/

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
