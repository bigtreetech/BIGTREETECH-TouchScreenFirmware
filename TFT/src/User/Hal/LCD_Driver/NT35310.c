#include "includes.h"

#if LCD_DRIVER_HAS(NT35310)

  #include "NT35310.h"
  // NT35310
  uint8_t LCD_DriveIsNT35310(void)
  {
    uint16_t id = 0;
    LCD_WR_REG(0XD4);
    id = LCD_RD_DATA();  // dummy read
    id = LCD_RD_DATA();
    id = LCD_RD_DATA();
    id <<= 8;

    id |= LCD_RD_DATA();
    return (id == 0x5310);
  }

  void NT35310_Init_Sequential(void)
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
    LCD_WR_DATA(0X60);
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

  void NT35310_SetDirection(uint8_t rotate)
  {
    LCD_WR_REG(0X36);
  
    #ifdef PORTRAIT_MODE
      LCD_WR_DATA(rotate ? NT35310_270_DEGREE_REG_VALUE : NT35310_90_DEGREE_REG_VALUE);
    #else
      LCD_WR_DATA(rotate ? NT35310_180_DEGREE_REG_VALUE : NT35310_0_DEGREE_REG_VALUE);
    #endif
  }

  void NT35310_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
  {
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
    LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
    LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
    LCD_WR_REG(0x2C);  // Ready to write memory
  }

  uint32_t NT35310_ReadPixel_24Bit(int16_t x, int16_t y)
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

#endif  // LCD_DRIVER_HAS(NT35310)
