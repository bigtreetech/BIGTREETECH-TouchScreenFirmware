#include "includes.h"

#if LCD_DRIVER_HAS(HX8558)

  #include "HX8558.h"
  // HX8558
  void HX8558_Init_Sequential(void)
  {
    Delay_ms(50);  // delay 50 ms

    LCD_WR_REG(0xFE);
    LCD_WR_REG(0xEF);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(5);
    LCD_WR_REG(0X36);
    LCD_WR_DATA(0x64);
    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x22);
    LCD_WR_REG(0xE3);
    LCD_WR_DATA(1);
    LCD_WR_DATA(4);
    LCD_WR_REG(0xA5);
    LCD_WR_DATA(0x40);
    LCD_WR_DATA(0x40);
    LCD_WR_REG(0xA4);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x44);
    LCD_WR_REG(0xAB);
    LCD_WR_DATA(8);
    LCD_WR_REG(0xAA);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x88);
    LCD_WR_REG(0xAE);
    LCD_WR_DATA(0xB);
    LCD_WR_REG(0xAC);
    LCD_WR_DATA(0);
    LCD_WR_REG(0xAF);
    LCD_WR_DATA(0x77);
    LCD_WR_REG(0xAD);
    LCD_WR_DATA(0x77);

    LCD_WR_REG(0x2A);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0xEF);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA(1);
    LCD_WR_DATA(0x3F);

    LCD_WR_REG(0x2C);

    LCD_WR_REG(0xF0);
    LCD_WR_DATA(2);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA(1);
    LCD_WR_DATA(1);
    LCD_WR_DATA(7);

    LCD_WR_REG(0xF1);
    LCD_WR_DATA(1);
    LCD_WR_DATA(3);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(41);
    LCD_WR_DATA(0x13);

    LCD_WR_REG(0xF2);
    LCD_WR_DATA(8);
    LCD_WR_DATA(6);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(3);
    LCD_WR_DATA(5);
    LCD_WR_DATA(0x34);

    LCD_WR_REG(0xF3);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0xC);
    LCD_WR_DATA(0x5A);
    LCD_WR_DATA(4);
    LCD_WR_DATA(3);
    LCD_WR_DATA(0x69);

    LCD_WR_REG(0xF4);
    LCD_WR_DATA(0xD);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(5);
    LCD_WR_DATA(5);
    LCD_WR_DATA(0);

    LCD_WR_REG(0xF5);
    LCD_WR_DATA(0xD);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x35);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0);
    LCD_WR_REG(0x11);

    Delay_ms(150);

    LCD_WR_REG(0x29);
    LCD_WR_REG(0x2C);
  }

  void HX8558_SetDirection(uint8_t rotate)
  {
    LCD_WR_REG(0X36);
    LCD_WR_DATA(rotate ? HX8558_180_DEGREE_REG_VALUE : HX8558_0_DEGREE_REG_VALUE);
  }

  void HX8558_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
  {
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
    LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
    LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
    LCD_WR_REG(0x2C);  // Ready to write memory
  }

  uint32_t HX8558_ReadPixel_24Bit(int16_t x, int16_t y)
  {
    LCD_SetWindow(x, y, x, y);
    LCD_WR_REG(0X22);
    Delay_us(1);
    LCD_RD_DATA();  // Dummy read

    GUI_COLOR pix;
    pix.color = LCD_RD_DATA();
    return (pix.RGB.r << 19) | (pix.RGB.g << 10) | (pix.RGB.b << 3);
  }

#endif  // LCD_DRIVER_HAS(HX8558)
