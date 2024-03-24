#include "HD44780_Emulator.h"
#include "includes.h"
#include "../../Configuration.h"
#include "GUI.h"
#include "HD44780.h"

#ifdef LCD2004_EMULATOR

#define XROWS        20
#define YROWS        4
#define FONT_PIXEL   (MIN(LCD_WIDTH / XROWS / BYTE_WIDTH, LCD_HEIGHT / YROWS / BYTE_HEIGHT))
#define BITMAP_PIXEL (MIN(LCD_WIDTH / XROWS / 6, LCD_HEIGHT / YROWS / 9))
#define XSTART       ((LCD_WIDTH - FONT_PIXEL * XROWS * BYTE_WIDTH) / 2)
#define YSTART       ((LCD_HEIGHT - FONT_PIXEL * YROWS * BYTE_HEIGHT) / 2)
#define YOFFSET      (BYTE_HEIGHT * FONT_PIXEL - 9 * BITMAP_PIXEL)

typedef void (* HD44780_CMD)(uint8_t);

typedef enum
{
  HD44780_DATA_DDRAM = 0,
  HD44780_DATA_CGRAM,
} HD44780_DATA_TYPE;

typedef struct
{
  int16_t  x;             // Current x pixel, range is 0 - 127
  int16_t  y;             // Current y pixel, range is 0 - 63
  // Extended Instruction, the first address set Y, second address set X
  uint8_t  address_is_y;  // Record current address is Y or X
} HD44780_PIXEL;

/*** Common Instruction ***/

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t : 2,
              f : 1,   // Sets the character font
              n : 1,   // Sets the number of display lines
              dl : 1;  // 4/8-bit interface control bit (0-4bit, 1-8bit)
    };
  };
} HD44780_REG_FS;  // Function Set

/*** Basic Instruction ***/

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t slr : 1,  // Display Shift Control: (Shift Left/Right)
              id : 1;   // Address Counter Control: (Increase/Decrease)
    };
  };
} HD44780_BIREG_EMS;  // Entry Mode Set

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t blink : 1,    // Character Blink ON/OFF control bit
              cursor : 1,   // Cursor ON/OFF control bit
              display : 1;  // Display ON/OFF control bit
    };
  };
} HD44780_BIREG_DC;  // Display Control

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t : 2,
              rl : 1,  // Cursor moves/shift left/right
              sc : 1;  // Display shift, cursor also follows to shift
    };
  };
} HD44780_BIREG_CDSC;  // Cursor/Display Shift Control

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t ac : 6;  // AC range is 00H…3FH
    };
  };
} HD44780_BIREG_CGRAMA;  // Set CGRAM Address

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t ac : 7;  // First line AC range is  80H … 93h
                       // Second line AC range is C0H … D3H
                       // Third line AC range is  94H … 2FH
                       // Fourth line AC range is D4H … E7H
    };
  };
} HD44780_BIREG_DDRAMA;  // Set DDRAM Address

/*** Extended Instruction ***/

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t sb : 1;
    };
  };
} HD44780_EIREG_SB;  // Standby

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t sr : 1;  // "1", the Vertical Scroll mode is enabled
                       // "0", “Set CGRAM Address” instruction (basic instruction) is enabled
    };
  };
} HD44780_EIREG_VSRA;  // Vertical Scroll or RAM Address Select

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t r0 : 1,  // R1 R0 Description
              r1 : 1;  // 0  0  First line normal or reverse
                       // 0  1  Second line normal or reverse
                       // 1  0  Third line normal or reverse
                       // 1  1  Fourth line normal or reverse
    };
  };
} HD44780_EIREG_REV;  // Reverse

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t ac : 6;  // HD44780_EIREG_VSR.sr = 1: AC5~AC0 is vertical scroll displacement address
    };
  };
} HD44780_EIREG_SA;  // Set Scroll Address

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t ac : 6;
    };
  };
} HD44780_EIREG_GRAMA;  // Set Graphic RAM Address

typedef struct
{
  // 1 << 0 : Display Clear
  // 1 << 1 : Return Home
  HD44780_BIREG_EMS    ems;     // 1 << 2 : Entry Mode Set
  HD44780_BIREG_DC     dc;      // 1 << 3 : Display Control
  HD44780_BIREG_CDSC   cdsc;    // 1 << 4 : Cursor/Display Shift Control
  // 1 << 5 : Function Set
  HD44780_BIREG_CGRAMA cgrama;  // 1 << 6 : Set CGRAM Address
  HD44780_BIREG_DDRAMA ddrama;  // 1 << 7 : Set DDRAM Address
} HD44780_BI;  // Basic Instruction

typedef struct
{
  HD44780_REG_FS    fs;       // Function Set
  HD44780_BI        bi;       // Basic Instruction
  uint8_t           reverse;  // Begin with normal and toggle to reverse
  uint8_t           cgram;
  HD44780_DATA_TYPE data_type;
} HD44780_REG;  // Extended Instruction

uint8_t HD44780_CGRAM[8][8];  // [64 * 2] = [4 * 16 * 2 * 8], means 4 * [16 * 16] bitmap font

HD44780_PIXEL HD44780 = {
  .x = 0,             // Current x pixel, range is 0 - 127
  .y = 0,             // Current y pixel, range is 0 - 63
  .address_is_y = 1,  // Extended instruction, the first address set Y, second address set X
};

HD44780_REG HD44780_reg = {
  .fs = //0x00,    // Default basic instruction, 8-bit MPU interface, graphic display OFF
        0x26,      // Default extended instruction, 4-bit MPU interface, graphic display On
  .bi = {
    .ems  = 0x02,  // Cursor move to right, DDRAM address counter (AC) plus 1
    .dc   = 0x04,  // Display, cursor and blink are ALL OFF
    .cdsc = 0x00,  // No cursor or display shift operation
  },
  .reverse = 0,    // Begin with normal and toggle to reverse
  .cgram = 0,
  .data_type = HD44780_DATA_DDRAM,
};

/*** Basic Instruction ***/

// cmd : 1 << 0
void HD44780_BI10_DisplayClear(uint8_t cmd)
{
  GUI_FillRectColor(XSTART,
                    YSTART,
                    LCD_WIDTH,
                    LCD_HEIGHT,
                    infoSettings.marlin_bg_color);
  HD44780.x = HD44780.y = 0;
  HD44780_reg.bi.ems.id = 1;
}

// cmd : 1 << 1
void HD44780_BI11_ReturnHome(uint8_t cmd)
{
  // Set address counter (AC) to "00H"
  HD44780.x = HD44780.y = 0;
}

// cmd : 1 << 2
void HD44780_BI12_EntryModeSet(uint8_t cmd)
{
  HD44780_reg.bi.ems.reg = cmd;
}

// cmd : 1 << 3
void HD44780_BI13_DisplayControl(uint8_t cmd)
{
  HD44780_reg.bi.dc.reg = cmd;
}

// cmd : 1 << 4
void HD44780_BI14_CursorDisplayControl(uint8_t cmd)
{
  HD44780_reg.bi.cdsc.reg = cmd;
}

// cmd : 1 << 5
void HD44780_CI15_FunctionSet(uint8_t cmd)
{
  HD44780_reg.fs.reg = cmd;
}

// cmd : 1 << 6
void HD44780_BI16_SetCGRAMAddress(uint8_t cmd)
{
  HD44780_reg.bi.cgrama.reg = cmd;

  uint8_t address = HD44780_reg.bi.cgrama.ac;

  // Set CGRAM address
  HD44780.y = (address >> 3) & 0x07;
  HD44780.x = 0;
  HD44780_reg.data_type = HD44780_DATA_CGRAM;
}

// cmd : 1 << 7
void HD44780_BI17_SetDDRAMAddress(uint8_t cmd)
{
  HD44780_reg.bi.ddrama.reg = cmd;

  // Set DDRAM address
  // x is 0-20 . y is 0-4

  if (0x80 <= cmd && cmd <= 0x93)
  { // First line AC range is  80H … 93h
    HD44780.y = 0;
    HD44780.x = cmd - 0x80;
  }

  if (0xC0 <= cmd && cmd <= 0xD3)
  { // Second line AC range is C0H … D3H
    HD44780.y = 1;
    HD44780.x = cmd - 0xC0;
  }

  if (0x94 <= cmd && cmd <= 0xA7)
  { // Third line AC range is  94H … A7H
    HD44780.y = 2;
    HD44780.x = cmd - 0x94;
  }

  if (0xD4 <= cmd && cmd <= 0xE7)
  { // Fourth line AC range is D4H … E7H
    HD44780.y = 3;
    HD44780.x = cmd - 0xD4;
  }

  HD44780_reg.data_type = HD44780_DATA_DDRAM;
}

void HD44780_DrawPixel(int16_t x, int16_t y, bool isForeGround, bool isFont)
{
  if (isFont)
  {
    GUI_FillRectColor(XSTART + FONT_PIXEL * x,
                      YSTART + FONT_PIXEL * y,
                      XSTART + FONT_PIXEL * (x + 1),
                      YSTART + FONT_PIXEL * (y + 1),
                      isForeGround ? infoSettings.marlin_font_color : infoSettings.marlin_bg_color);
  }
  else
  {
    GUI_FillRectColor(XSTART + BITMAP_PIXEL * x,
                      YSTART + YOFFSET + BITMAP_PIXEL * y,
                      XSTART + BITMAP_PIXEL * (x + 1),
                      YSTART + YOFFSET + BITMAP_PIXEL * (y + 1),
                      isForeGround ? infoSettings.marlin_font_color : infoSettings.marlin_bg_color);
  }
}

// Display CGRAM fonts
void HD44780_DispDDRAM(uint8_t data)
{
  uint16_t i  = 0,
           ex = 0,
           ey = 0;

  if (data < 8)
  { // 5 * 8 bitmap
    ex = HD44780.x * 6 + 6;
    ey = HD44780.y * 12 + 8;

    for (uint16_t y = HD44780.y * 12; y < ey; y++)
    {
      uint8_t temp = HD44780_CGRAM[data][i++];

      for (uint16_t x = HD44780.x * 6; x < ex; x++)
      {
        HD44780_DrawPixel(x, y, temp & 0x10, 0);
        temp <<= 1;
      }
    }
  }
  else
  { // font
    if (data < ' ' || data > '~')
      return;

    ex = HD44780.x * BYTE_WIDTH + BYTE_WIDTH - 1;
    ey = HD44780.y * BYTE_HEIGHT + BYTE_HEIGHT - 1;

    CHAR_INFO info = {.bytes = 0};

    getCharacterInfo(&data, &info);

    uint16_t x = 0,
             y = 0,
             j = 0;
    uint16_t bitMapSize = (info.pixelHeight * info.pixelWidth / 8);
    uint8_t  font[bitMapSize];
    uint32_t temp = 0;

    W25Qxx_ReadBuffer(font, info.bitMapAddr, bitMapSize);

    for (x = HD44780.x * BYTE_WIDTH; x < ex; x++)
    {
      for (j = 0; j < (BYTE_HEIGHT + 8 - 1) / 8; j++)
      {
        temp <<= 8;
        temp |= font[i++];
      }

      for (y = HD44780.y * BYTE_HEIGHT; y < ey; y++)
      {
        HD44780_DrawPixel(x, y, temp & (1 << (BYTE_HEIGHT - 1)), 1);
        temp <<= 1;
      }
    }
  }

  HD44780.x ++;
}

void HD44780_SetCGRAMData(uint8_t data)
{
  HD44780_CGRAM[HD44780.y][HD44780.x++] = data;

  if (HD44780.x > 7)
  {
    HD44780.x = 0;
    HD44780.y = (HD44780.y + 1) % (8 - 1);
  }
}

static const HD44780_CMD hd44780CmdCallBack[8] = {
  // Basic Instruction                Extended Instruction
  HD44780_BI10_DisplayClear,          // cmd 1 << 0
  HD44780_BI11_ReturnHome,            // cmd 1 << 1
  HD44780_BI12_EntryModeSet,          // cmd 1 << 2
  HD44780_BI13_DisplayControl,        // cmd 1 << 3
  HD44780_BI14_CursorDisplayControl,  // cmd 1 << 4
  HD44780_CI15_FunctionSet,           // cmd 1 << 5
  HD44780_BI16_SetCGRAMAddress,       // cmd 1 << 6
  HD44780_BI17_SetDDRAMAddress,       // cmd 1 << 7
};

void HD44780_ParseWCmd(uint8_t cmd)
{
  for (int8_t i = 7; i >= 0; i--)
  {
    if (cmd & (1 << i))
    {
      (*hd44780CmdCallBack[i])(cmd);

      break;
    }
  }
}

void HD44780_ParseWData(uint8_t data)
{
  switch (HD44780_reg.data_type)
  {
    case HD44780_DATA_DDRAM:
      HD44780_DispDDRAM(data);
      break;

    case HD44780_DATA_CGRAM:
      HD44780_SetCGRAMData(data);
      break;
  }
}

// Parse queue data
void HD44780_ParseRecv(uint8_t val)
{
  static uint8_t rcvData = 0;
  static uint8_t rcvIndex = 0;
  static uint8_t curIsCmd = 0;

  if (val & 0x80)
    curIsCmd = 1;

  val &= 0x0F;           // Every 8 bits instruction/data will be separated into 2 groups, higher 4 bits always 0 in every groups

  if (rcvIndex == 0)
  {
    rcvData = val << 4;  // Higher 4 bits in first byte
    rcvIndex++;

    return;
  }
  else
  {
    rcvData |= val;      // Lower 4 bits in second byte
    rcvIndex = 0;
  }

  if (curIsCmd)
  {
    HD44780_ParseWCmd(rcvData);
    curIsCmd = 0;
  }
  else
  {
    HD44780_ParseWData(rcvData);
  }
}

#endif
