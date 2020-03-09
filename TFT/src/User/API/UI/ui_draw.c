#include "ui_draw.h"
#include "includes.h"

#ifdef STM32_HAS_FSMC

void lcd_frame_display(u16 sx,u16 sy,u16 w,u16 h, u32 addr);

#else

void lcd_frame_display(u16 sx,u16 sy,u16 w,u16 h, u32 addr)
{
  u16 x,y;
  u16 color = 0;
  u32 address = addr;

  LCD_SetWindow(sx,sy,sx+w-1,sy+h-1);
  LCD_WR_REG(0x2C);

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((address& 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(address & 0xFF);

  for(y=sy; y<sy+h; y++)
  {
    for(x=sx; x<sx+w; x++)
    {
      color  = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE)<<8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
      LCD_WR_16BITS_DATA(color);
    }
  }
  W25Qxx_SPI_CS_Set(1);
}
#endif

void LOGO_ReadDisplay(void)
{
  lcd_frame_display(0, 0, LCD_WIDTH, LCD_HEIGHT, LOGO_ADDR);
}

void ICON_ReadDisplay(u16 sx,u16 sy, u8 icon)
{
  lcd_frame_display(sx, sy, ICON_WIDTH, ICON_HEIGHT, ICON_ADDR(icon));
}

//directly draw BMP file to lcd (pos - GUI_POINT(top left corner of image location on lcd), bmp: path of bmp file)
bool bmp_DirectDisplay(GUI_POINT pos, char *bmp)
{
  FIL   bmpFile;
  char  magic[2];
  int   w,h,bytePerLine;
  short bpp;
  int   offset;
  u8    lcdcolor[4];
  UINT  mybr;

  GUI_COLOR pix;

  if(f_open(&bmpFile,bmp,FA_OPEN_EXISTING | FA_READ)!=FR_OK)
    return false;

  f_read(&bmpFile, magic, 2 ,&mybr);
  if (memcmp(magic, "BM", 2)){
    f_close(&bmpFile);
    return false;
  }

  f_lseek(&bmpFile, 10);
  f_read(&bmpFile, &offset, sizeof(int),&mybr);

  f_lseek(&bmpFile, 18);
  f_read(&bmpFile, &w, sizeof(int),&mybr);
  f_read(&bmpFile, &h, sizeof(int),&mybr);

  f_lseek(&bmpFile, 28);
  f_read(&bmpFile, &bpp, sizeof(short),&mybr);
  if(bpp<24){
    f_close(&bmpFile);
    return false;
  }
  bpp >>=3;
  bytePerLine=w*bpp;
  if(bytePerLine%4 !=0) //bmp
    bytePerLine=(bytePerLine/4+1)*4;

  for(int j=0; j<h; j++)
  {
    f_lseek(&bmpFile, offset+(h-j-1)*bytePerLine);
    for(int i=0; i<w; i++)
    {
      f_read(&bmpFile,(char *)&lcdcolor,bpp,&mybr);

      pix.RGB.r=lcdcolor[2]>>3;
      pix.RGB.g=lcdcolor[1]>>2;
      pix.RGB.b=lcdcolor[0]>>3;

      GUI_DrawPixel((pos.x + i), (pos.y + j), pix.color);

    }
  }
  f_close(&bmpFile);
  return true;
}

//draw icon with different length and width (sx & sy cordinates for top left of icon, w width, h height, addr flash byte address)
void ICON_CustomReadDisplay(u16 sx,u16 sy,u16 w, u16 h, u32 addr)
{
  lcd_frame_display(sx, sy, w, h, addr);
}

void SMALLICON_ReadDisplay(u16 sx,u16 sy, u8 icon)
{
  lcd_frame_display(sx, sy, SMALLICON_WIDTH, SMALLICON_HEIGHT, SMALL_ICON_ADDR(icon));
}

void ICON_PressedDisplay(u16 sx,u16 sy, u8 icon)
{
  u16 mode=0x0FF0;
  u16 x,y;
  u16 color = 0;
  u32 address = ICON_ADDR(icon);

  LCD_SetWindow(sx, sy, sx+ICON_WIDTH-1, sy+ICON_HEIGHT-1);
  LCD_WR_REG(0x2C);

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((address& 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(address & 0xFF);

  for(y=sy; y<sy+ICON_WIDTH; y++)
  {
    for(x=sx; x<sx+ICON_HEIGHT; x++)
    {
      color  = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE)<<8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
      LCD_WR_16BITS_DATA(color & mode);
    }
  }
  W25Qxx_SPI_CS_Set(1);
}
