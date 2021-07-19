#include "includes.h"

#if LCD_DRIVER_HAS(RM68042)

  #include "RM68042.h"
  // RM68042
  void RM68042_Init_Sequential(void)
  {
    LCD_WR_REG(0X11);
    Delay_ms(20);
    LCD_WR_REG(0XD0);   // VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
    LCD_WR_DATA(0X07);
    LCD_WR_DATA(0X42);
    LCD_WR_DATA(0X1C);
    LCD_WR_REG(0XD1);   // VCOMH VCOM_AC amplitude setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X19);
    LCD_WR_DATA(0X16);
    LCD_WR_REG(0XD2);   // Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X11);
    LCD_WR_REG(0XE4);
    LCD_WR_DATA(0X00A0);
    LCD_WR_REG(0XF3);
    LCD_WR_DATA(0X0000);
    LCD_WR_DATA(0X002A);
    LCD_WR_REG(0XC0);   // REV SM GS
    LCD_WR_DATA(0X10);
    LCD_WR_DATA(0X3B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);
    LCD_WR_DATA(0X11);
    LCD_WR_REG(0XC5);   // Frame rate setting = 72HZ  when setting 0x03
    LCD_WR_DATA(0X03);
    LCD_WR_REG(0XC8);   // Gamma setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X35);
    LCD_WR_DATA(0X23);
    LCD_WR_DATA(0X07);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X04);
    LCD_WR_DATA(0X45);
    LCD_WR_DATA(0X53);
    LCD_WR_DATA(0X77);
    LCD_WR_DATA(0X70);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X04);
    LCD_WR_REG(0X20);   // Exit invert mode
    LCD_WR_REG(0X36);
    LCD_WR_DATA(0X28);
    LCD_WR_REG(0X3A);
    LCD_WR_DATA(0X55);  // 16λģʽ
    Delay_ms(120);
    LCD_WR_REG(0X29);
  }

  void RM68042_SetDirection(uint8_t rotate)
  {
    LCD_WR_REG(0X36);
    LCD_WR_DATA(rotate ? RM68042_180_DEGREE_REG_VALUE : RM68042_0_DEGREE_REG_VALUE);
  }

  void RM68042_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
  {
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
    LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
    LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
    LCD_WR_REG(0x2C);  // Ready to write memory
  }

  uint32_t RM68042_ReadPixel_24Bit(int16_t x, int16_t y)
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

#endif  // LCD_DRIVER_HAS(RM68042)
