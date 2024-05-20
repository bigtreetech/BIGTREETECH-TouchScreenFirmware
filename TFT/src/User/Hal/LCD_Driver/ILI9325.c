#include "includes.h"

#if LCD_DRIVER_HAS(ILI9325)

#include "ILI9325.h"

void ILI9325_Init_Sequential(void)
{
  #if defined(MKS_TFT32L_V3_0)                                // For this, SS=0 and GS=0 for default screen rotation=0
    LCD_WR_REG(0x0001);                                       // Driver Output Control Register (R01h)
    LCD_WR_DATA((0 << 8) | (0 << 10));                        // SS=0 (horizontal flip), SM = 0, from S720 to S1 (see also  GS bit (R60h))
    LCD_WR_REG(0x0003);                                       // Entry Mode (R03h)
    LCD_WR_DATA((1 << 12) | (1 << 5) | (1 << 4) | (1 << 3));  // TRI=0, DFM=0, BGR=1, ORG=0, I/D[1:0]=11, AM=1
    LCD_WR_REG(0x0060);                                       // Driver Output Control (R60h)
    LCD_WR_DATA((0 << 15) | (0x27 << 8));                     // GS=0 (G1) NL[5:0]=0x27 (320 lines)
  #else
    LCD_WR_REG(0x0001);                                       // Driver Output Control Register (R01h)
    LCD_WR_DATA((1 << 8) | (0 << 10));                        // SS=1 (horizontal flip), SM = 0, from S720 to S1 (see also  GS bit (R60h))
    LCD_WR_REG(0x0003);                                       // Entry Mode (R03h)
    LCD_WR_DATA((1 << 12) | (1 << 5) | (1 << 4) | (1 << 3));  // TRI=0, DFM=0, BGR=1, ORG=0, I/D[1:0]=11, AM=1
    LCD_WR_REG(0x0060);                                       // Driver Output Control (R60h)
    LCD_WR_DATA((0 << 15) | (0x27 << 8));                     // GS=0 (G1) NL[5:0]=0x27 (320 lines)
  #endif
}

void ILI9325_SetDirection(uint8_t dir)
{
  #if defined(MKS_TFT32L_V3_0)                                // For this, SS=GS for screen rotation
    LCD_WR_REG(0x0001);                                       // Driver Output Control Register (R01h)
    LCD_WR_DATA((!dir << 8) | (0 << 10));                     // SS (horizontal flip), SM = 0, from S720 to S1 (see also  GS bit (R60h))
    LCD_WR_REG(0x0003);                                       // Entry Mode (R03h)
    LCD_WR_DATA((1 << 12) | (1 << 5) | (1 << 4) | (1 << 3));  // TRI=0, DFM=0, BGR=1, ORG=0, I/D[1:0]=11, AM=1
    LCD_WR_REG(0x0060);                                       // Driver Output Control (R60h)
    LCD_WR_DATA((!dir << 15) | (0x27 << 8));                  // GS (G1) NL[5:0]=0x27 (320 lines)
  #else
    LCD_WR_REG(0x0001);                                       // Driver Output Control Register (R01h)
    LCD_WR_DATA((dir ? 0 : 1 << 8) | (0 << 10));              // SS (horizontal flip), SM = 0, from S720 to S1 (see also  GS bit (R60h))
    LCD_WR_REG(0x0003);                                       // Entry Mode (R03h)
    LCD_WR_DATA((1 << 12) | (1 << 5) | (1 << 4) | (1 << 3));  // TRI=0, DFM=0, BGR=1, ORG=0, I/D[1:0]=11, AM=1
    LCD_WR_REG(0x0060);                                       // Driver Output Control (R60h)
    LCD_WR_DATA((dir ? 1 : 0 << 15) | (0x27 << 8));           // GS (G1) NL[5:0]=0x27 (320 lines)
  #endif
}

void ILI9325_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  LCD_WR_REG(0x50);
  LCD_WR_DATA(sy);
  LCD_WR_REG(0x52);
  LCD_WR_DATA(sx);
  LCD_WR_REG(0x51);
  LCD_WR_DATA(ey);
  LCD_WR_REG(0x53);
  LCD_WR_DATA(ex);
  LCD_WR_REG(0x20);
  LCD_WR_DATA(sy);
  LCD_WR_REG(0x21);
  LCD_WR_DATA(sx);
  LCD_WR_REG(0x22);  // Ready to write memory
}

#endif  // LCD_DRIVER_HAS(ILI9325)
