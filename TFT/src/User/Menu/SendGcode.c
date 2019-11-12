#include "SendGcode.h"
#include "includes.h"

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
    GUI_DispStringInRect(LCD_WIDTH-GCODE_KEYW-START_GKX,10,LCD_WIDTH-10,10+BYTE_HEIGHT+5, (u8 *)"Send", 0);
    GUI_DispStringInRect(10,10,GCODE_KEYW+START_GKX,10+BYTE_HEIGHT+5, (u8 *)"Back", 0);
      
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
      
      if(key_num == GKEY_17)//Ȑۻ
      {
        if(keyturn)
        keyturn = false;
        else
        keyturn = true;
        GUI_SetColor(FK_COLOR);
        GUI_FillRect(0,(SPACE_GKY+TITLE_END_Y),LCD_WIDTH,LCD_HEIGHT);
        GUI_SetColor(BK_COLOR);
      }
      
      if(SendGcode() && p[0]!='\0')//ע̍ļ®
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
          infoSettings.silent = !infoSettings.silent;
      }
      
      if(key_num == GKEY_5)
      {
        p[j] = '\0';
        j--;
        GUI_SetColor(FK_COLOR);
        GUI_FillRect(GCODE_KEYW+START_GKX+10,10,LCD_WIDTH-(GCODE_KEYW+START_GKX+10),10+BYTE_HEIGHT+5);
        GUI_SetColor(BK_COLOR);
      }
      
      if(BackGKEY()||back)//׵ܘ
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
      GUI_DispLenString(10, SIMULATOR_YSTART, (uint8_t *)p, 0, LCD_WIDTH-30);
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
