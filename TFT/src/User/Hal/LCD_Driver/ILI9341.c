#include "includes.h"

#if LCD_DRIVER_HAS(ILI9341)

#include "ILI9341.h"

#define ILI9341_0_DEGREE_REG_VALUE   0X68
#define ILI9341_180_DEGREE_REG_VALUE 0XA8
#define ILI9341_90_DEGREE_REG_VALUE  0XC8
#define ILI9341_270_DEGREE_REG_VALUE 0X08

void ILI9341_Init_Sequential(void)
{
  Delay_ms(50);  // Delay 50ms

  LCD_WR_REG(0xCF);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC1);
  LCD_WR_DATA(0X30);

  LCD_WR_REG(0xED);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0X12);
  LCD_WR_DATA(0X81);

  LCD_WR_REG(0xE8);   /// @diff :Driver timing control A
  LCD_WR_DATA(0x8A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x78);

  LCD_WR_REG(0xEA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0xCB);
  LCD_WR_DATA(0x39);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x02);

  LCD_WR_REG(0xF7);
  LCD_WR_DATA(0x20);

  LCD_WR_REG(0xC0);   /// @diff Power control
  LCD_WR_DATA(0x25);  // VRH[5:0]

  LCD_WR_REG(0xC1);   /// @diff control
  LCD_WR_DATA(0x12);  // SAP[2:0];BT[3:0]

  LCD_WR_REG(0xC5);   /// @diff VCM control
  LCD_WR_DATA(0x33);
  LCD_WR_DATA(0x3C);

  LCD_WR_REG(0xC7);   /// @diff VCM control2
  LCD_WR_DATA(0x9A);

  LCD_WR_REG(0xB1);   /// @diff Frame Rate Control
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x15);

  LCD_WR_REG(0x3A);
  LCD_WR_DATA(0x55);

  LCD_WR_REG(0x36);   // Memory Access Control
  LCD_WR_DATA(0x68);

  LCD_WR_REG(0xB6);   // Display Function Control
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0xA2);

  LCD_WR_REG(0xF2);   // 3Gamma Function Disable
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0x26);   // Gamma curve selected
  LCD_WR_DATA(0x01);

  LCD_WR_REG(0xE0);   /// @diff Set Gamma
  LCD_WR_DATA(0x1F);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x47);
  LCD_WR_DATA(0xC8);
  LCD_WR_DATA(0x37);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x14);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0XE1);   /// @diff Set Gamma
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x25);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x38);
  LCD_WR_DATA(0x48);
  LCD_WR_DATA(0x48);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x35);
  LCD_WR_DATA(0x37);
  LCD_WR_DATA(0x1F);

  LCD_WR_REG(0x2A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x3f);

  LCD_WR_REG(0x2B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xef);

  LCD_WR_REG(0x11);   // Exit Sleep
  Delay_ms(120);
  LCD_WR_REG(0x29);   // Display on
}

void ILI9341_SetDirection(uint8_t rotate)
{
  LCD_WR_REG(0X36);

  #ifdef PORTRAIT_MODE
    LCD_WR_DATA(rotate ? ILI9341_270_DEGREE_REG_VALUE : ILI9341_90_DEGREE_REG_VALUE);
  #else
    LCD_WR_DATA(rotate ? ILI9341_180_DEGREE_REG_VALUE : ILI9341_0_DEGREE_REG_VALUE);
  #endif
}

void ILI9341_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  LCD_WR_REG(0x2A);
  LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
  LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
  LCD_WR_REG(0x2B);
  LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
  LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
  LCD_WR_REG(0x2C);  // Ready to write memory
}

uint32_t ILI9341_ReadPixel_24Bit(int16_t x, int16_t y)
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

#endif  // LCD_DRIVER_HAS(ILI9341)
