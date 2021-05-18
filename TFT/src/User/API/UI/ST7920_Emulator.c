#include "ST7920_Emulator.h"
#include "includes.h"
#include "GUI.h"
#include "../../Configuration.h"

#ifdef ST7920_EMULATOR

const ST7920_POSITION st7920_init_position = {
  .xByte = 0,         // current x byte, range is 0 ~ 15 byte = 0 ~ 127 pixel
  .yPixel = 0,        // current y pixel, range is 0 ~ 63
  .address_is_y = 1,  // Extended Instruction, The first address set Y, second address set X
};

const ST7920_REG st7920_inti_reg = {
  .fs = //0x00,    // default basic instruction, 8-bit MPU interface, Graphic display OFF
        0x26,      // default extended instruction, 4-bit MPU interface, Graphic display On
  .bi = {
    .ems  = 0x02,  // Cursor move to right ,DDRAM address counter (AC) plus 1
    .dc   = 0x00,  // Display, cursor and blink are ALL OFF
    .cdsc = 0x00,  // No cursor or display shift operation
  },
  .ei = {
    .vsra = 0x00,  // Allow vertical scroll or set CGRAM address
    .rev  = 0x00,  // Begin with normal and toggle to reverse
  },
  .reverse = 0,    // Begin with normal and toggle to reverse
  .data_type = ST7920_DATA_DDRAM,
  .ctrl_status = ST7920_IDLE,
};

ST7920 *pSt7920;

const float st7920_gx_start_full = ST7920_GXSTART_FULLSCREEN;
const float st7920_gy_start_full = ST7920_GYSTART_FULLSCREEN;
const float st7920_gx_dot_full = ST7920_GXDOT_FULLSCREEN;
const float st7920_gy_dot_full = ST7920_GYDOT_FULLSCREEN;
const uint16_t st7920_gx_start = ST7920_GXSTART;
const uint16_t st7920_gy_start = ST7920_GYSTART;
const uint16_t st7920_gx_dot = ST7920_GXDOT;
const uint16_t st7920_gy_dot = ST7920_GYDOT;

void ST7920_DrawPixel(int16_t x, int16_t y, bool isForeGround)
{
  if (infoSettings.marlin_mode_fullscreen)
  {
    GUI_FillRectColor(st7920_gx_start_full + st7920_gx_dot_full * x,
                      st7920_gy_start_full + st7920_gy_dot_full * y,
                      st7920_gx_start_full + st7920_gx_dot_full * (x + 1),
                      st7920_gy_start_full + st7920_gy_dot_full * (y + 1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  }
  else
  {
    GUI_FillRectColor(st7920_gx_start + st7920_gx_dot * x,
                      st7920_gy_start + st7920_gy_dot * y,
                      st7920_gx_start + st7920_gx_dot * (x + 1),
                      st7920_gy_start + st7920_gy_dot * (y + 1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  }
}

void ST7920_ClearRAM(void)
{
  // Clear CGRAM buffer
  memset(pSt7920->CGRAM, 0, sizeof(pSt7920->CGRAM));
  // Clear DDRAM buffer, Fill with "20H"(space code).
  memset(pSt7920->DDRAM, ' ', sizeof(pSt7920->DDRAM));
  // Clear GDRAM buffer
  memset(pSt7920->GDRAM, 0, sizeof(pSt7920->GDRAM));
}

// Init St7920 struct and load 8x16 ASCII font from SPI flash
void ST7920_Init(ST7920 *pStruct)
{
  pSt7920 = pStruct;
  // Init pSt7920
  pSt7920->position = st7920_init_position;
  pSt7920->reg = st7920_inti_reg;
  ST7920_ClearRAM();
  // Init 8x16Font
  W25Qxx_ReadBuffer((uint8_t *)pSt7920->_8x16Font, _8X16_FONT_ADDR, sizeof(pSt7920->_8x16Font));
}

// When I/D = "1", cursor moves right, address counter (AC) is increased by 1
void ST7920_CursorIncrease(void)
{
  pSt7920->position.xByte++;
  if (pSt7920->position.xByte >= 16)
  {
    pSt7920->position.xByte = 0;
    pSt7920->position.yPixel = (pSt7920->position.yPixel + 32);
    if (pSt7920->position.yPixel == 64) pSt7920->position.yPixel = 16;
    else if (pSt7920->position.yPixel == 80) pSt7920->position.yPixel = 0;
  }
}

// Draw the byte at the specified position
// The byte comes from the XOR of DDRAM(CGRAM or HCGROM) and GDRAM
void ST7920_DrawByte(uint8_t xByte, uint8_t yPixel)
{
  uint8_t yByte = yPixel / 16;
  uint8_t yOffset = yPixel & 0x0F;
  uint8_t xStartByte = xByte & 0xFE; // CGRAM fonts can only be displayed in the start position of each address

  uint8_t drawByte = 0;

  if (pSt7920->DDRAM[yByte][xStartByte] == 0)  // 0x00 means this is a CGRAM data(0x0000, 0x0002, 0x0004, 0x0006)
  {
    uint8_t np = pSt7920->DDRAM[yByte][xStartByte + 1] & 0xF6;  // bit0 and bit3 are don’t care
    if (xByte == xStartByte)  // CGRAM high 8bits
    {
      drawByte = pSt7920->CGRAM[np * 8 + yOffset][0];
    }
    else  // CGRAM low 8bits
    {
      drawByte = pSt7920->CGRAM[np * 8 + yOffset][1];
    }
  }
  else
  {
    // HCGROM fonts
    uint8_t p = pSt7920->DDRAM[yByte][xByte];
    drawByte = pSt7920->_8x16Font[p - ' '][yOffset];
  }

  drawByte ^= pSt7920->GDRAM[yPixel][xByte];  // XOR GDRAM
  for (uint8_t i = 0; i < 8; i++)
  {
    ST7920_DrawPixel(xByte * 8 + i, yPixel, drawByte & (1 << 7));
    drawByte <<= 1;
  }
}

// Display graphic
void ST7920_DrawGDRAM(uint8_t data)
{
  if (pSt7920->reg.fs.g != 1) return;  // Graphic display off

  pSt7920->GDRAM[pSt7920->position.yPixel][pSt7920->position.xByte] = data;

  ST7920_DrawByte(pSt7920->position.xByte, pSt7920->position.yPixel);

  ST7920_CursorIncrease();
}

// Display DDRAM(CGRAM or HCGROM)
void ST7920_DrawDDRAM(uint8_t data)
{
  uint8_t xStartByte = pSt7920->position.xByte & 0xFE; // CGRAM fonts can only be displayed in the start position of each address
  uint8_t yByte = pSt7920->position.yPixel / 16;
  uint8_t tempDDRAM = pSt7920->DDRAM[yByte][pSt7920->position.xByte];

  pSt7920->DDRAM[yByte][pSt7920->position.xByte] = data;

  uint8_t ey = pSt7920->position.yPixel + 16;

  if (pSt7920->DDRAM[yByte][xStartByte] == 0)  // 0x00 means this is a CGRAM data(0x0000, 0x0002, 0x0004, 0x0006)
  {
    if (xStartByte != pSt7920->position.xByte)
    {
      // Display CGRAM 16*16 bitmap fonts
      for (uint8_t y = pSt7920->position.yPixel; y < ey; y++)
      {
        ST7920_DrawByte(xStartByte, y);
        ST7920_DrawByte(pSt7920->position.xByte, y);
      }
    }
  }
  else
  {
    // Display HCGROM fonts, 02H~7EH.
    if (data >= ' ' && data <= '~')
    {
      for (uint8_t y = pSt7920->position.yPixel; y < ey; y++)
      {
        ST7920_DrawByte(pSt7920->position.xByte, y);
      }
    }

    // Check if CGRAM data(0x0000, 0x0002, 0x0004, 0x0006) has been overwritten
    if ((tempDDRAM == 0) && (xStartByte == pSt7920->position.xByte))
    {
      // Clear second char of DDRAM for CGRAM data
      pSt7920->DDRAM[yByte][pSt7920->position.xByte + 1] = 0x20;

      for (uint8_t y = pSt7920->position.yPixel; y < ey; y++)
      {
        ST7920_DrawByte(pSt7920->position.xByte + 1, y);
      }
    }
  }

  ST7920_CursorIncrease();
}

/*** Common instruction ***/

// cmd : 1 << 5
void ST7920_CI15_FunctionSet(uint8_t cmd)
{
  pSt7920->reg.fs.reg = cmd;
}

/*** Basic instruction ***/

// cmd : 1 << 0
void ST7920_BI10_DisplayClear(uint8_t cmd)
{
  // Set DDRAM address counter (AC) to"00H"
  pSt7920->position.xByte = pSt7920->position.yPixel = 0;
  // Set Entry Mode I/D bit to be "1". Cursor moves right and AC adds 1 after write or read operation
  pSt7920->reg.bi.ems.id = 1;
  // Clear CGRAM/DDRAM/GDRAM
  ST7920_ClearRAM();
}

// cmd : 1 << 1
void ST7920_BI11_ReturnHome(uint8_t cmd)
{
  // Set address counter (AC) to "00H".
  pSt7920->position.xByte = pSt7920->position.yPixel = 0;
}

// cmd : 1 << 2
void ST7920_BI12_EntryModeSet(uint8_t cmd)
{
  pSt7920->reg.bi.ems.reg = cmd;
}

// cmd : 1 << 3
void ST7920_BI13_DisplayControl(uint8_t cmd)
{
  pSt7920->reg.bi.dc.reg = cmd;
}

// cmd : 1 << 4
void ST7920_BI14_CursorDisplayControl(uint8_t cmd)
{
  pSt7920->reg.bi.cdsc.reg = cmd;
}

// cmd : 1 << 6
void ST7920_BI16_SetCGRAMAddress(uint8_t cmd)
{
  pSt7920->reg.bi.cgrama.reg = cmd;
  uint8_t address = pSt7920->reg.bi.cgrama.ac;
  // Set CGRAM address
  // xByte is 0-1 = [2*8], yPixel is 0-64 = [4*16], means 4 * [16*16] bitmap font
  pSt7920->position.xByte = 0;
  pSt7920->position.yPixel = address;
  pSt7920->reg.data_type = ST7920_DATA_CGRAM;
}

// the data follow cmd : 1 << 6
void ST7920_BI_SetCGRAMData(uint8_t data)
{
  pSt7920->CGRAM[pSt7920->position.yPixel][pSt7920->position.xByte++] = data;
  if (pSt7920->position.xByte >= 2)
  {
    pSt7920->position.xByte = 0;
    pSt7920->position.yPixel = (pSt7920->position.yPixel + 1) & (64 - 1);
  }
}

// cmd : 1 << 7
void ST7920_BI17_SetDDRAMAddress(uint8_t cmd)
{
  pSt7920->reg.bi.ddrama.reg = cmd;
  uint8_t address = pSt7920->reg.bi.ddrama.ac;
  // Set DDRAM address
  // xByte is 0~15. y is 0~63
  pSt7920->position.xByte = (address & 0x07) * 2;
  pSt7920->position.yPixel = ((address >> 4) & 0x01) * 16 + ((address >> 3) & 0x01) * 32;
  pSt7920->reg.data_type = ST7920_DATA_DDRAM;
}

/*** Extended Instruction ***/

// cmd : 1 << 0
void ST7920_EI10_StandBy(uint8_t cmd)
{
  pSt7920->reg.ei.sb.reg = cmd;
}

// cmd : 1 << 1
void ST7920_EI11_ScrollOrRAMAddress(uint8_t cmd)
{
  pSt7920->reg.ei.vsra.reg = cmd;
}

// cmd : 1 << 2
void ST7920_EI12_Reverse(uint8_t cmd)
{
  pSt7920->reg.ei.rev.reg = cmd;
  pSt7920->reg.reverse = !pSt7920->reg.reverse;  // Reverse the display by toggling this instruction
}

// cmd : 1 << 6
void ST7920_EI16_SetScrollAddress(uint8_t cmd)
{
  pSt7920->reg.ei.sa.reg = cmd;
}

// cmd : 1 << 7
void ST7920_EI17_SetGDRAMAddress(uint8_t cmd)
{
  pSt7920->reg.ei.gdrama.reg = cmd;
  uint8_t address = pSt7920->reg.ei.gdrama.ac;
  if (pSt7920->position.address_is_y)
  {
    pSt7920->position.yPixel = address;
    pSt7920->position.address_is_y = 0;
  }
  else
  {
    if (address & 0x08) pSt7920->position.yPixel += 32;
    pSt7920->position.xByte = (address & 0x07) * 2;
    pSt7920->position.address_is_y = 1;
  }
  pSt7920->reg.data_type = ST7920_DATA_GDRAM;
}

static const FP_CMD st7920CmdCallBack[8][2] = {
  // Basic Instruction                Extended Instruction
  { ST7920_BI10_DisplayClear,         ST7920_EI10_StandBy},             // cmd 1 << 0
  { ST7920_BI11_ReturnHome,           ST7920_EI11_ScrollOrRAMAddress},  // cmd 1 << 1
  { ST7920_BI12_EntryModeSet,         ST7920_EI12_Reverse},             // cmd 1 << 2
  { ST7920_BI13_DisplayControl,       NULL},                            // cmd 1 << 3
  { ST7920_BI14_CursorDisplayControl, NULL},                            // cmd 1 << 4
  { ST7920_CI15_FunctionSet,          ST7920_CI15_FunctionSet},         // cmd 1 << 5
  { ST7920_BI16_SetCGRAMAddress,      ST7920_EI16_SetScrollAddress},    // cmd 1 << 6
  { ST7920_BI17_SetDDRAMAddress,      ST7920_EI17_SetGDRAMAddress},     // cmd 1 << 7
};

void ST7920_ST7920_ParseWCmd(uint8_t cmd)
{
  for (int8_t i = 7; i >= 0; i--)
  {
    if (cmd & (1 << i))
    {
      if (st7920CmdCallBack[i][pSt7920->reg.fs.re] != NULL)
      {
        (*st7920CmdCallBack[i][pSt7920->reg.fs.re])(cmd);
      }
      break;
    }
  }
}

void ST7920_ST7920_ParseWData(uint8_t data)
{
  switch (pSt7920->reg.data_type)
  {
    case ST7920_DATA_DDRAM:
      ST7920_DrawDDRAM(data);
      break;

    case ST7920_DATA_CGRAM:
      ST7920_BI_SetCGRAMData(data);
      break;

    case ST7920_DATA_GDRAM:
      ST7920_DrawGDRAM(data);
      break;
  }
}

uint8_t ST7920_IsCtrlByte(uint8_t data)
{
  if (data == ST7920_WCMD || data == ST7920_WDATA || data == ST7920_RCMD || data == ST7920_RDATA)
    return true;
  else
    return false;
}

// Parse queue data
void ST7920_ParseRecv(uint8_t val)
{
  static uint8_t rcvData = 0;
  static uint8_t rcvIndex = 0;
  if (ST7920_IsCtrlByte(val))
  {
    pSt7920->reg.ctrl_status = (ST7920_CTRL_STATUS)val;
    rcvIndex = 0;
    if (pSt7920->reg.ctrl_status == ST7920_WDATA)
    {
      pSt7920->position.address_is_y = 1;
    }
  }
  else
  {
    val &= 0xF0;            // Every 8 bits instruction/data will be separated into 2 groups, lower 4 bits always 0 in every groups
    if (rcvIndex == 0)
    {
      rcvData = val;        // Higher 4 bits in first byte
      rcvIndex++;
      return;
    }
    else
    {
      rcvData |= val >> 4;  // Lower 4 bits in second byte
      rcvIndex = 0;
    }

    switch (pSt7920->reg.ctrl_status)
    {
      case ST7920_WCMD:
        ST7920_ST7920_ParseWCmd(rcvData);
        break;

      case ST7920_WDATA:
        ST7920_ST7920_ParseWData(rcvData);
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

#endif
