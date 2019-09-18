#include "boot.h"
#include "includes.h"

const GUI_RECT iconUpdateRect = {(LCD_WIDTH - ICON_WIDTH)/2,              (LCD_HEIGHT - ICON_HEIGHT)/2, 
                                 (LCD_WIDTH - ICON_WIDTH)/2 + ICON_WIDTH, (LCD_HEIGHT - ICON_HEIGHT)/2 + ICON_HEIGHT};
const GUI_RECT labelUpdateRect = {0,        (LCD_HEIGHT - ICON_HEIGHT)/2 + ICON_HEIGHT, 
                                 LCD_WIDTH, (LCD_HEIGHT - ICON_HEIGHT)/2 + ICON_HEIGHT + BYTE_HEIGHT};

const char iconBmpName[][20]={
"Heat", "Move", "Home", "Print", "Extrude", "Fan", "Settings", "Leveling",
"Inc",  "Dec", "Nozzle", "Hotbed", "Temp_1", "Temp_5", "Temp_10", "Stop", "Back",
"Inc_X", "Inc_Y", "Inc_Z", "Mmm_01", "Mmm_1", "Mmm_10", "Dec_X", "Dec_Y","Dec_Z",
"Home_X", "Home_Y", "Home_Z",
"Folder", "File", "Page_up", "Page_down",
"Pause", "Resume",
"Load", "Unload", "Slow", "Normal", "Fast", "Emm_1", "Emm_5", "Emm_10",
"Full", "Half",
"Rotate", "Language", "TP_Adjust", "About", "Disconnect", "BaudRate",
"Percentage", "BabyStep", "Mmm_001", "OnBoardSD", "OnTFTSD", "U_Disk", "Runout",
"Point_1", "Point_2", "Point_3", "Point_4",
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
  
  for(bnum=0;bnum<(w*h*2+4095)/4096;bnum++)
  {
    W25Qxx_EraseSector(addr+bnum*4096);
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
      
      if(bnum==256)
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
  char nowBmp[30];  
  GUI_Clear(BLACK);
  GUI_DispString(100, 5, (u8*)"Icon Updating...!",0);

  if(bmpDecode(BMP_ROOT_DIR"/Logo.bmp", LOGO_ADDR))
  {
    LOGO_ReadDisplay();
  }
  GUI_Clear(BLACK);
  for(int i=0; i<aCount(iconBmpName); i++)
  {
    my_sprintf(nowBmp, BMP_ROOT_DIR"/%s.bmp", iconBmpName[i]);
    if(bmpDecode(nowBmp, ICON_ADDR(i)))
    {
      GUI_ClearRect(labelUpdateRect.x0, labelUpdateRect.y0, labelUpdateRect.x1, labelUpdateRect.y1);
      GUI_DispStringInPrect(&labelUpdateRect, (u8 *)nowBmp, 0);
      ICON_ReadDisplay(iconUpdateRect.x0, iconUpdateRect.y0, i);
    }
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

  tempbuf = malloc(4096);
  if (tempbuf == NULL)  return;  
  GUI_Clear(BLACK);
  my_sprintf((void *)buffer,"%s Size: %dKB",font, (u32)f_size(&myfp)>>10);
  GUI_DispString(40, 100, (u8*)buffer,0);
  GUI_DispString(40, 140, (u8*)"Updating:   %",0);
  
  while(!f_eof(&myfp))
  {
    if (f_read(&myfp, tempbuf, 4096, &rnum) != FR_OK) break;
    
    W25Qxx_EraseSector(addr+offset);
    W25Qxx_WriteBuffer(tempbuf,addr+offset,4096);
    offset+=rnum;
    if(progress != offset*100/f_size(&myfp))
    {
      progress = offset*100/f_size(&myfp);
      GUI_DispDec(40 + BYTE_WIDTH*9, 140, progress, 3, 1, RIGHT);
    }
    if(rnum!=4096)break;
  }
  
  f_close(&myfp);
  free(tempbuf);
}


void scanUpdates(void)
{
  volatile u8 result = 0;   //must volatile！
  if(mountSDCard())
  {
    result = scanUpdateFile();
    if (result & FONT)
    {
      updateFont(FONT_ROOT_DIR"/byte.fon", BYTE_ADDR);
      updateFont(FONT_ROOT_DIR"/word.fon", WORD_ADDR);
      updateFont(FONT_ROOT_DIR"/uni2oem.bin", UNI2OEM_ADDR);
      updateFont(FONT_ROOT_DIR"/oem2uni.bin", OEM2UNI_ADDR);
    }
    if (result & BMP) //bmp
    {
      updateIcon();
    }
  }
}
