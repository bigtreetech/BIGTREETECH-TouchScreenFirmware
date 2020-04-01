#include "ST7920_Simulator.h"
#include "includes.h"
#include "GUI.h"
#include "../../Configuration.h"

#ifdef ST7920_SPI

ST7920_PIXEL       st7920 = {ST7920_XSTART, ST7920_YSTART, 0};
ST7920_CTRL_STATUS status = ST7920_IDLE;

void ST7920_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
//  GUI_DrawPixel(x, y, color);
  if(infoSettings.marlin_mode_fullscreen)
  {
    GUI_FillRectColor(SIMULATOR_XSTART_FULLSCREEN + PIXEL_XSIZE_FULLSCREEN*x,
                      SIMULATOR_YSTART_FULLSCREEN + PIXEL_YSIZE_FULLSCREEN*y,
                      SIMULATOR_XSTART_FULLSCREEN + PIXEL_XSIZE_FULLSCREEN*(x+1),
                      SIMULATOR_YSTART_FULLSCREEN + PIXEL_YSIZE_FULLSCREEN*(y+1),
                      color);

  } else {
    GUI_FillRectColor(SIMULATOR_XSTART + PIXEL_XSIZE*x,     SIMULATOR_YSTART + PIXEL_YSIZE*y ,
                      SIMULATOR_XSTART + PIXEL_XSIZE*(x+1), SIMULATOR_YSTART + PIXEL_YSIZE*(y+1),
                      color);
  }
}

int16_t ST7920_MapCoordinateX(void)
{
  return (st7920.x & 0x07) * 16 + st7920.x_record;
}

int16_t ST7920_MapCoordinateY(void)
{
  return ((st7920.x & 0x08)>>3)*32 + (st7920.y - ST7920_YSTART);
}

void ST7920_SetCursor(int16_t x, int16_t y)
{
  st7920.x = x;
  st7920.y = y;
  st7920.x_record = 0;
}


void ST7920_DrawByte(u8 data)
{
  uint8_t i = 0;
  int16_t x = ST7920_MapCoordinateX(),
          y = ST7920_MapCoordinateY();

  for(; i<8; i++)
  {
    if(data & 0x80)
      ST7920_DrawPixel(x, y, infoSettings.font_color);
    else
      ST7920_DrawPixel(x, y, infoSettings.bg_color);
    data <<= 1;
    x++;
  }
  st7920.x_record += 8;
  if(st7920.x_record == 16)
  {
    st7920.x_record = 0;
    st7920.x = (st7920.x + 1) & 0x8F;
  }
}

u8 ST7920_IsCtrlByte(u8 data)
{
  if(data == ST7920_WCMD || data == ST7920_WDATA || data == ST7920_RCMD || data == ST7920_RDATA)
    return true;
  else
    return false;
}

u8 rcvData[64];
u8 rcvIndex = 0;

void ST7920_ParseRecv(u8 val)
{
  if (ST7920_IsCtrlByte(val))
  {
    status = (ST7920_CTRL_STATUS)val;
    rcvIndex = 0;
  }
  else
  {
    rcvData[rcvIndex++] = val;
    if(rcvIndex == 1) return;  //high 4 bits in first byte and
    rcvIndex = 0;              //low 4 bits in second byte is valid

    switch (status)
    {
      case ST7920_WCMD:
        ST7920_ST7920_ParseWCmd(rcvData[0] | (rcvData[1]>>4));
        break;

      case ST7920_WDATA:
        ST7920_DrawByte(rcvData[0] | (rcvData[1]>>4));
        break;

      case ST7920_RCMD:
        break;

      case ST7920_RDATA:
        break;

      default:
        break;
    }
  }
}

void ST7920_WriteXY(u8 xy)
{
  static uint8_t i = 0;
  if(i == 0)        //y first
    st7920.y = xy;
  else
    st7920.x = xy;
  i = (i + 1) % 2;
}

void ST7920_ST7920_ParseWCmd(u8 cmd)
{
  if(cmd & 0x80)
  {
    ST7920_WriteXY(cmd);
  }
}


void menuST7920(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_SetColor(infoSettings.font_color);
  GUI_SetBkColor(infoSettings.bg_color);

  #if defined(ST7920_BANNER_TEXT)
    GUI_DispStringInRect(0, 0, LCD_WIDTH, SIMULATOR_YSTART, (u8*)ST7920_BANNER_TEXT);
  #endif

  SPI_Slave();
  SPI_Slave_CS_Config();

  while(infoMenu.menu[infoMenu.cur] == menuST7920)
  {
    while(SPISlave.rIndex != SPISlave.wIndex)
    {
      ST7920_ParseRecv(SPISlave.data[SPISlave.rIndex]);

      SPISlave.rIndex = (SPISlave.rIndex + 1) % SPI_SLAVE_MAX;
    }

    Touch_Sw(LCD_ReadTouch());

    if(LCD_BtnTouch(LCD_BUTTON_INTERVALS))
			Touch_Sw(1);

    #if LCD_ENCODER_SUPPORT
      loopCheckMode();
      LCD_loopCheckEncoder();
    #endif
    #ifdef CLEAN_MODE_SWITCHING_SUPPORT
      loopBackEnd();
    #endif
  }
  SPI_SlaveDeInit();
}
#endif
