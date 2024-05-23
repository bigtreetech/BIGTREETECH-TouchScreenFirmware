#include "includes.h"

#if LCD_DRIVER_HAS(ILI9488)

#include "ILI9488.h"

#if defined(MKS_TFT35_V1_0)  // Rotate display 180 degrees by default for MKS_TFT35_V1_0
  #define ILI9488_180_DEGREE_REG_VALUE 0X28
  #define ILI9488_0_DEGREE_REG_VALUE   0XE8
  #define ILI9488_90_DEGREE_REG_VALUE  0X88
  #define ILI9488_270_DEGREE_REG_VALUE 0X48
#else
  #define ILI9488_0_DEGREE_REG_VALUE   0X28
  #define ILI9488_180_DEGREE_REG_VALUE 0XE8
  #define ILI9488_90_DEGREE_REG_VALUE  0X88
  #define ILI9488_270_DEGREE_REG_VALUE 0X48
#endif

uint8_t LCD_DriveIsILI9488(void)
{
  uint16_t id = 0;

  LCD_WR_REG(0XD3);
  id = LCD_RD_DATA();  // Dummy read
  id = LCD_RD_DATA();
  id = LCD_RD_DATA();
  id <<= 8;

  id |= LCD_RD_DATA();

  return (id == 0x9488);
}

void ILI9488_Init_Sequential(void)
{
  LCD_WR_REG(0xC0);
  LCD_WR_DATA(0x0c);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xC1);
  LCD_WR_DATA(0x44);
  LCD_WR_REG(0xC5);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x16);
  LCD_WR_DATA(0x80);
  LCD_WR_REG(0x36);
  LCD_WR_DATA(0x28);
  LCD_WR_REG(0x3A);   // Interface Mode Control
  LCD_WR_DATA(0x55);
  LCD_WR_REG(0XB0);   // Interface Mode Control
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB1);   // Frame rate 70HZ
  LCD_WR_DATA(0xB0);
  LCD_WR_REG(0xB4);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xB6);   // RGB/MCU Interface Control
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xE9);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0XF7);
  LCD_WR_DATA(0xA9);
  LCD_WR_DATA(0x51);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x82);
  LCD_WR_REG(0x11);
  Delay_ms(120);
  LCD_WR_REG(0x29);
}

void ILI9488_SetDirection(uint8_t rotate)
{
  LCD_WR_REG(0X36);

  #ifdef PORTRAIT_MODE
    LCD_WR_DATA(rotate ? ILI9488_270_DEGREE_REG_VALUE : ILI9488_90_DEGREE_REG_VALUE);
  #else
    LCD_WR_DATA(rotate ? ILI9488_180_DEGREE_REG_VALUE : ILI9488_0_DEGREE_REG_VALUE);
  #endif
}

void ILI9488_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  LCD_WR_REG(0x2A);
  LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
  LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
  LCD_WR_REG(0x2B);
  LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
  LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
  LCD_WR_REG(0x2C);  // Ready to write memory
}

uint32_t ILI9488_ReadPixel_24Bit(int16_t x, int16_t y)
{
  LCD_SetWindow(x, y, x, y);
  LCD_WR_REG(0X2E);
  Delay_us(1);
  LCD_RD_DATA();  // Dummy read

  uint16_t rg, br;

  rg = LCD_RD_DATA();  // First pixel R:8bit-G:8bit
  br = LCD_RD_DATA();  // First pixel B:8bit - Second pixel R:8bit

  return ((rg) << 8) | ((br & 0xFF00) >> 8);  // RG-B
}

#endif  // LCD_DRIVER_HAS(ILI9488)
