#include "boot.h"
#include "includes.h"

const GUI_RECT iconUpdateRect = {(LCD_WIDTH - ICON_WIDTH)/2,              (LCD_HEIGHT - ICON_HEIGHT)/2, 
                                 (LCD_WIDTH - ICON_WIDTH)/2 + ICON_WIDTH, (LCD_HEIGHT - ICON_HEIGHT)/2 + ICON_HEIGHT};
const GUI_RECT labelUpdateRect = {0,        (LCD_HEIGHT - ICON_HEIGHT)/2 + ICON_HEIGHT, 
                                 LCD_WIDTH, (LCD_HEIGHT - ICON_HEIGHT)/2 + ICON_HEIGHT + BYTE_HEIGHT};

const char iconBmpName[][32]={
"Heat", "Move", "Home", "Print", "Extrude", "Fan", "Settings", "Leveling", "Inc", "Dec",
"Nozzle", "Hotbed", "Temp_1", "Temp_5", "Temp_10", "Stop", "Back", "Inc_X", "Inc_Y", "Inc_Z",
"Mmm_01", "Mmm_1", "Mmm_10", "Dec_X", "Dec_Y","Dec_Z", "Home_X", "Home_Y", "Home_Z", "Folder",
"File", "Page_up", "Page_down", "Pause", "Resume", "Load", "Unload", "Slow", "Normal", "Fast",
"Emm_1", "Emm_5", "Emm_10", "Full", "Half", "Rotate", "Language", "TP_Adjust", "More", "About",
"BackGroundColor", "FontColor", "Disconnect", "BaudRate", "Percentage", "BabyStep", "Mmm_001", "OnBoardSD", "OnTFTSD", "U_Disk",
"Runout", "Point_1", "Point_2", "Point_3", "Point_4", "Marlin", "BigTreeTech", "Gcode", "BLTouch", "BLTouchDeploy",
"BLTouchStow", "BLTouchTest", "BLTouchRepeat", "TSCSettings", "MachineSettings", "FeatureSettings", "ProbeOffset", "EEPROMSave", "SilentOn", "ShutDown",
"RGB_Settings", "RGB_Red", "RGB_Green", "RGB_Blue", "RGB_White", "RGB_Off", "Preheat_Both", "Preheat_PLA", "Preheat_PETG", "Preheat_ABS",
"PowerSupply", "Custom", "Custom0", "Custom1", "Custom2", "Custom3", "Custom4", "Custom5", "Custom6", "Home_Move", "Heat_Fan",
"ManualLevel", "CoolDown", "SilentOff","StatusNozzle","StatusBed","StatusFan","MainMenu","StatusSpeed","StatusFlow",
"parametersetting", "global_nozzle", "global_bed", "ledcolor",
}; 

u8 scanUpdateFile(void)
{
  DIR dir;
  u8 rst = 0;
  
  if (f_opendir(&dir, BMP_ROOT_DIR) == FR_OK)
  {
    rst |= BMP;  
    f_closedir(&dir);
  }
  if (f_opendir(&dir, FONT_ROOT_DIR) == FR_OK)
  {
    rst |= FONT;  
    f_closedir(&dir);
  }
  return rst;
}

bool bmpDecode(char *bmp, u32 addr)  
{  
  FIL   bmpFile;
  char  magic[2];  
  int   w,h,bytePerLine;  
  short bpp; 
  int   offset;   
  u8    buf[256];
  u8    lcdcolor[4];
  u16   bnum=0;
  UINT  mybr;

  GUI_COLOR pix;

  if(f_open(&bmpFile,bmp,FA_OPEN_EXISTING | FA_READ)!=FR_OK)
    return false;

  f_read(&bmpFile, magic, 2 ,&mybr);  
  if (memcmp(magic, "BM", 2))  
    return false;
  
  f_lseek(&bmpFile, 10);  
  f_read(&bmpFile, &offset, sizeof(int),&mybr);  

  f_lseek(&bmpFile, 18);  
  f_read(&bmpFile, &w, sizeof(int),&mybr);  
  f_read(&bmpFile, &h, sizeof(int),&mybr);  

  f_lseek(&bmpFile, 28);  
  f_read(&bmpFile, &bpp, sizeof(short),&mybr);  
  if(bpp<24)
    return false;
  bpp >>=3; 
  bytePerLine=w*bpp;     
  if(bytePerLine%4 !=0) //bmp
    bytePerLine=(bytePerLine/4+1)*4;  
  
  for(bnum=0; bnum<(w*h*2+W25QXX_SECTOR_SIZE-1)/W25QXX_SECTOR_SIZE; bnum++)
  {
    W25Qxx_EraseSector(addr+bnum*W25QXX_SECTOR_SIZE);
  }
  bnum=0;
    
  for(int j=0; j<h; j++)
  {  
    f_lseek(&bmpFile, offset+(h-j-1)*bytePerLine);
    for(int i=0; i<w; i++)
    {
      f_read(&bmpFile,(char *)&lcdcolor,bpp,&mybr);

      pix.RGB.r=lcdcolor[2]>>3;
      pix.RGB.g=lcdcolor[1]>>2;        
      pix.RGB.b=lcdcolor[0]>>3;

//      GUI_DrawPixel(i,j,pix.color);
      
      buf[bnum++]=(u8)(pix.color>>8);
      buf[bnum++]=(u8)(pix.color&0xFF);
      
      if(bnum == 256)
      {
        W25Qxx_WritePage(buf,addr,256);
        addr+=256;
        bnum=0;
      }
    }
  }    

  W25Qxx_WritePage(buf,addr,bnum);
  addr+=bnum;
  f_close(&bmpFile);

  return true;  
}  

void updateIcon(void)
{
  char nowBmp[64];  
  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispString(100, 5, (u8*)"Icon Updating...!");

  if(bmpDecode(BMP_ROOT_DIR"/Logo.bmp", LOGO_ADDR))
  {
    LOGO_ReadDisplay();
  }

  GUI_Clear(BACKGROUND_COLOR);
  for(int i=0; i<COUNT(iconBmpName); i++)
  {
    my_sprintf(nowBmp, BMP_ROOT_DIR"/%s.bmp", iconBmpName[i]);
    if(bmpDecode(nowBmp, ICON_ADDR(i)))
    {
      GUI_ClearRect(labelUpdateRect.x0, labelUpdateRect.y0, labelUpdateRect.x1, labelUpdateRect.y1);
      GUI_DispStringInPrect(&labelUpdateRect, (u8 *)nowBmp);
      ICON_ReadDisplay(iconUpdateRect.x0, iconUpdateRect.y0, i);
    }
  }
    if(bmpDecode(BMP_ROOT_DIR"/InfoBox.bmp", INFOBOX_ADDR))
  {
    ICON_CustomReadDisplay(iconUpdateRect.x0, iconUpdateRect.y0, INFOBOX_WIDTH, INFOBOX_HEIGHT,INFOBOX_ADDR);
  }
}

void updateFont(char *font, u32 addr)
{
  u8   progress = 0;
  UINT rnum = 0;
  u32  offset = 0;
  char buffer[128];
  FIL  myfp;
  u8*  tempbuf = NULL;
  
  if (f_open(&myfp, font, FA_OPEN_EXISTING|FA_READ) != FR_OK)  return;

  tempbuf = malloc(W25QXX_SECTOR_SIZE);
  if (tempbuf == NULL)  return;
  GUI_Clear(BACKGROUND_COLOR);
  my_sprintf((void *)buffer,"%s Size: %dKB",font, (u32)f_size(&myfp)>>10);
  GUI_DispString(0, 100, (u8*)buffer);
  GUI_DispString(0, 140, (u8*)"Updating:   %");
  
  while(!f_eof(&myfp))
  {
    if (f_read(&myfp, tempbuf, W25QXX_SECTOR_SIZE, &rnum) != FR_OK) break;
    
    W25Qxx_EraseSector(addr + offset);
    W25Qxx_WriteBuffer(tempbuf, addr + offset, W25QXX_SECTOR_SIZE);
    offset += rnum;
    if(progress != offset * 100 / f_size(&myfp))
    {
      progress = offset * 100 / f_size(&myfp);
      GUI_DispDec(0 + BYTE_WIDTH*9, 140, progress, 3, RIGHT);
    }
    if(rnum !=W25QXX_SECTOR_SIZE)break;
  }
  
  f_close(&myfp);
  free(tempbuf);
}

void scanResetDir(void)
{
  FIL resetfile;
  if (f_open(&resetfile, TFT_RESET_FILE, FA_OPEN_EXISTING | FA_READ) == FR_OK)
  {
    f_close(&resetfile);
    f_rename(TFT_RESET_FILE, TFT_RESET_FILE ".DONE");
    infoSettingsReset();
    TSC_Calibration();
    storePara();
  }
}

void scanUpdates(void)
{
  volatile u8 result = 0;   //must volatileï¼
  if(mountSDCard())
  {
    result = scanUpdateFile();
    if (result & FONT)
    {
      updateFont(FONT_ROOT_DIR"/byte_ascii.fon", BYTE_ASCII_ADDR);
      updateFont(FONT_ROOT_DIR"/word_unicode.fon", WORD_UNICODE);
    }
    if (result & BMP) //bmp
    {
      updateIcon();
    }
    if (result) f_rename(ROOT_DIR, ROOT_DIR".CUR");
    scanResetDir();
  }
}
