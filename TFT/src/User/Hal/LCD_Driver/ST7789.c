#include "includes.h"

#if LCD_DRIVER_HAS(ST7789)

  #include "ST7789.h"
  // ST7789
  void ST7789_Init_Sequential(void)
  {
    LCD_WR_REG(0x11);
    Delay_ms(120);  // Delay 120ms
    //------------------------------display and color format setting------------------------------//
    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x68);
    LCD_WR_REG(0x3a);
    LCD_WR_DATA(0x05);
    //--------------------------------ST7789V Frame rate setting----------------------------------//
    LCD_WR_REG(0xb2);
    LCD_WR_DATA(0x0c);
    LCD_WR_DATA(0x0c);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x33);
    LCD_WR_REG(0xb7);
    LCD_WR_DATA(0x35);
    //---------------------------------ST7789V Power setting--------------------------------------//
    LCD_WR_REG(0xbb);
    LCD_WR_DATA(0x28);
    LCD_WR_REG(0xc0);
    LCD_WR_DATA(0x2c);
    LCD_WR_REG(0xc2);
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xc3);
    LCD_WR_DATA(0x0b);
    LCD_WR_REG(0xc4);
    LCD_WR_DATA(0x20);
    LCD_WR_REG(0xc6);
    LCD_WR_DATA(0x0f);
    LCD_WR_REG(0xd0);
    LCD_WR_DATA(0xa4);
    LCD_WR_DATA(0xa1);
    //--------------------------------ST7789V gamma setting---------------------------------------//
    LCD_WR_REG(0xe0);
    LCD_WR_DATA(0xd0);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0f);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x2a);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x3a);
    LCD_WR_DATA(0x0b);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x20);
    LCD_WR_REG(0xe1);
    LCD_WR_DATA(0xd0);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0a);
    LCD_WR_DATA(0x0b);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x4a);
    LCD_WR_DATA(0x2b);
    LCD_WR_DATA(0x1b);
    LCD_WR_DATA(0x1c);
    LCD_WR_DATA(0x22);
    LCD_WR_DATA(0x1f);
    LCD_WR_REG(0x29);
  }

  void ST7789_SetDirection(uint8_t rotate)
  {
    LCD_WR_REG(0X36);
    LCD_WR_DATA(rotate ? ST7789_180_DEGREE_REG_VALUE : ST7789_0_DEGREE_REG_VALUE);
  }

  void ST7789_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
  {
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
    LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
    LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
    LCD_WR_REG(0x2C);  // Ready to write memory
  }

  uint32_t ST7789_ReadPixel_24Bit(int16_t x, int16_t y)
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

#endif  // LCD_DRIVER_HAS(ST7789)
