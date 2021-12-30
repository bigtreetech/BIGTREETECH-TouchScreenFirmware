#include "includes.h"

#if LCD_DRIVER_HAS(ILI9325)

  #include "ILI9325.h"
  // ILI9325
  void ILI9325_Init_Sequential(void)
  {
    uint16_t R01h, R03h, R60h;
    R01h = (1 << 8) | (0 << 10);                        // SS = 1, SM = 0,  from S720 to S1 (see also  GS bit (R60h))
    R03h = (1 << 12) | (1 << 5) | (1 << 4) | (1 << 3);  // TRI=0, DFM=0, BGR=1, ORG=0, I/D[1:0]=11, AM=1
    R60h = (0 << 15) | (0x27 << 8);                     // Gate Scan Control (R60h) GS=0(G1) NL[5:0]=0x27 (320 lines)

    LCD_WR_REG(0x0001); // Driver Output Control Register (R01h)
    LCD_WR_DATA(R01h);
    LCD_WR_REG(0x0003); // Entry Mode (R03h)
    LCD_WR_DATA(R03h);
    LCD_WR_REG(0x0060); // Driver Output Control (R60h)
    LCD_WR_DATA(R60h);
  }

  void ILI9325_SetDirection(uint8_t dir)
  {
    uint16_t R01h, R03h, R60h;
    R01h = (dir ? 0 : 1 << 8) | (0 << 10);     // SS=horizontal flip, SM=0
    R03h = (1 << 12) | (1 << 5) | (1 << 4) | (1 << 3);            // TRI=0, DFM=0, BGR=1, ORG=0, I/D[1:0]=11, AM=1
    R60h = (dir ? 1 : 0 << 15) | (0x27 << 8);  // GS=vertical flip, NL[5:0]=0x27 (320 lines)

    LCD_WR_REG(0x0001); // Driver Output Control Register (R01h)
    LCD_WR_DATA(R01h);
    LCD_WR_REG(0x0003); // Entry Mode (R03h)
    LCD_WR_DATA(R03h);
    LCD_WR_REG(0x0060); // Driver Output Control (R60h)
    LCD_WR_DATA(R60h);
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
