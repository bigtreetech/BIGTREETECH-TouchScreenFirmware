#include "ST7920_Emulator.h"
#include "includes.h"
#include "GUI.h"
#include "../../Configuration.h"

#ifdef ST7920_EMULATOR

const ST7920_PIXEL st7920_init_pixel = {
  .x = 0,             // current x pixel, range is 0 - 127
  .y = 0,             // current y pixel, range is 0 - 63
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
  .cgram = 0,
  .data_type = ST7920_DATA_DDRAM,
  .ctrl_status = ST7920_IDLE,
};

ST7920 *pSt7920;

void ST7920_ClearRAM(void)
{
  // Clear CGRAM buffer
  memset(pSt7920->CGRAM, 0, sizeof(pSt7920->CGRAM));
  // Clear DDRAM buffer, Fill with "20H"(space code).
  memset(pSt7920->DDRAM, ' ', sizeof(pSt7920->DDRAM));
  // Clear GDRAM buffer
  memset(pSt7920->GDRAM, 0, sizeof(pSt7920->GDRAM));
}

void ST7920_Init(ST7920 *pStruct)
{
  pSt7920 = pStruct;
  // Init pSt7920
  pSt7920->pixel = st7920_init_pixel;
  pSt7920->reg = st7920_inti_reg;
  ST7920_ClearRAM();
  // Init 8x16Font
  W25Qxx_ReadBuffer((uint8_t *)pSt7920->_8x16Font, _8X16_FONT_ADDR, sizeof(pSt7920->_8x16Font));
}

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

uint8_t ST7920_GetMixedByte(uint8_t x, uint8_t y)
{
  uint8_t byte_x = x / 8,
          byte_y = y / 16,
          y_offset = y - byte_y * 16;

  uint8_t p = pSt7920->DDRAM[byte_y][byte_x];

  uint8_t data = 0;

  for (uint8_t i = 0; i < (byte_x + 1) && (i < 15); )  // Traversing this line to detect whether the current byte is CGRAM data
  {
    uint8_t p = pSt7920->DDRAM[byte_y][i],
            np = pSt7920->DDRAM[byte_y][i + 1] & 0xF6;  // bit0 and bit3 are don’t care
    if (p == 0 && (np & 0xF0) == 0)
    {
      if (i == byte_x)  // CGRAM high 8bits
      {
        data = pSt7920->CGRAM[np * 8 + y_offset][0];
        return data;
      }
      else if ((i + 1) == byte_x)  // CGRAM low 8bits
      {
        data = pSt7920->CGRAM[np * 8 + y_offset][1];
        return data;
      }
      i += 2;  // Skip this CGRAM data
    }
    else
    {
      i++;  // Keep traversing
    }
  }

  // HCGROM fonts
  data = pSt7920->_8x16Font[p - ' '][y_offset];

  return data;
}

// Display graphic
void ST7920_DrawGDRAM(uint8_t data)
{
  if (pSt7920->reg.fs.g != 1) return;  // Graphic display off

  pSt7920->GDRAM[pSt7920->pixel.y][pSt7920->pixel.x / 8] = data;
  data ^= ST7920_GetMixedByte(pSt7920->pixel.x, pSt7920->pixel.y);
  for (uint8_t i = 0; i < 8; i++)
  {
    ST7920_DrawPixel(pSt7920->pixel.x, pSt7920->pixel.y, data & (1 << 7));
    data <<= 1;
    pSt7920->pixel.x++;
  }

  if (pSt7920->pixel.x >= 128)
  {
    pSt7920->pixel.x = 0;
    pSt7920->pixel.y = (pSt7920->pixel.y + 32);
    if (pSt7920->pixel.y >= 64) pSt7920->pixel.y = (pSt7920->pixel.y + 1) & (64 - 1);
  }
}

// Display CGRAM fonts, only 0x0000, 0x0002, 0x0004, 0x0006
void ST7920_DispCGRAM(uint8_t data)
{
  uint8_t ex = pSt7920->pixel.x + 16,  // GRAM, 16*16 bitmap font
          ey = pSt7920->pixel.y + 16,
          yIndex = data * 8;

  pSt7920->DDRAM[pSt7920->pixel.y / 16][pSt7920->pixel.x / 8] = 0x00;
  pSt7920->DDRAM[pSt7920->pixel.y / 16][pSt7920->pixel.x / 8 + 1] = data;

  for (uint8_t y = pSt7920->pixel.y; y < ey; y++)
  {
    uint16_t temp = pSt7920->CGRAM[yIndex][0] << 8 | pSt7920->CGRAM[yIndex][1];
    temp ^= ((pSt7920->GDRAM[y][pSt7920->pixel.x / 8] << 8 ) | pSt7920->GDRAM[y][pSt7920->pixel.x / 8 + 1]);
    for (uint8_t x = pSt7920->pixel.x; x < ex; x++)
    {
      ST7920_DrawPixel(x, y, temp & (1 << 15));
      temp <<= 1;
    }
    yIndex++;
  }

  pSt7920->pixel.x += 16;
  if (pSt7920->pixel.x >= 128)
  {
    pSt7920->pixel.x = 0;
    pSt7920->pixel.y = (pSt7920->pixel.y + 32);
    if (pSt7920->pixel.y == 64) pSt7920->pixel.y = 16;
    else if (pSt7920->pixel.y == 80) pSt7920->pixel.y = 0;
  }
}

// Display HCGROM fonts, 02H~7EH.
void ST7920_DispHCGROM(uint8_t p)
{
  if (p < ' ' || p > '~') return;

  pSt7920->DDRAM[pSt7920->pixel.y / 16][pSt7920->pixel.x / 8] = p;

  uint8_t ex = pSt7920->pixel.x + 8,
          ey = pSt7920->pixel.y + 16;

  for (uint8_t y = pSt7920->pixel.y; y < ey; y++)
  {
    uint32_t temp = pSt7920->_8x16Font[p - ' '][y - pSt7920->pixel.y];
    temp ^= pSt7920->GDRAM[y][pSt7920->pixel.x / 8];
    for (uint8_t x = pSt7920->pixel.x; x < ex; x++)
    {
      ST7920_DrawPixel(x, y, temp & (1 << 7));
      temp <<= 1;
    }
  }

  pSt7920->pixel.x += 8;
  if (pSt7920->pixel.x >= 128)
  {
    pSt7920->pixel.x = 0;
    pSt7920->pixel.y = (pSt7920->pixel.y + 32);
    if (pSt7920->pixel.y == 64) pSt7920->pixel.y = 16;
    else if (pSt7920->pixel.y == 80) pSt7920->pixel.y = 0;
  }
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
  pSt7920->pixel.x = pSt7920->pixel.y = 0;
  // Set Entry Mode I/D bit to be "1". Cursor moves right and AC adds 1 after write or read operation
  pSt7920->reg.bi.ems.id = 1;
  // Clear CGRAM/DDRAM/GDRAM
  ST7920_ClearRAM();
}

// cmd : 1 << 1
void ST7920_BI11_ReturnHome(uint8_t cmd)
{
  // Set address counter (AC) to "00H".
  pSt7920->pixel.x = pSt7920->pixel.y = 0;
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
  // x is 0-1 = [2*8], y is 0-64 = [4*16], means 4 * [16*16] bitmap font
  pSt7920->pixel.x = 0;
  pSt7920->pixel.y = address;
  pSt7920->reg.data_type = ST7920_DATA_CGRAM;
}

// the data follow cmd : 1 << 6
void ST7920_BI_SetCGRAMData(uint8_t data)
{
  pSt7920->CGRAM[pSt7920->pixel.y][pSt7920->pixel.x++] = data;
  if (pSt7920->pixel.x >= 2)
  {
    pSt7920->pixel.x = 0;
    pSt7920->pixel.y = (pSt7920->pixel.y + 1) & (64 - 1);
  }
}

// cmd : 1 << 7
void ST7920_BI17_SetDDRAMAddress(uint8_t cmd)
{
  pSt7920->reg.bi.ddrama.reg = cmd;
  uint8_t address = pSt7920->reg.bi.ddrama.ac;
  // Set DDRAM address
  // x is 0-127. y is 0-63
  pSt7920->pixel.x = (address & 0x07) * 16;
  pSt7920->pixel.y = ((address >> 4) & 0x01) * 16 + ((address >> 3) & 0x01) * 32;
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
void ST7920_EI17_SetGRAMAddress(uint8_t cmd)
{
  pSt7920->reg.ei.grama.reg = cmd;
  uint8_t address = pSt7920->reg.ei.grama.ac;
  if (pSt7920->pixel.address_is_y)
  {
    pSt7920->pixel.y = address;
    pSt7920->pixel.address_is_y = 0;
  }
  else
  {
    if (address & 0x08) pSt7920->pixel.y += 32;
    pSt7920->pixel.x = (address & 0x07) * 16;
    pSt7920->pixel.address_is_y = 1;
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
  { ST7920_BI17_SetDDRAMAddress,      ST7920_EI17_SetGRAMAddress},      // cmd 1 << 7
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
      if (pSt7920->reg.cgram == 1)
      {
        pSt7920->reg.cgram = 0;
        ST7920_DispCGRAM(data);
      }
      else if (data == 0x00)
      {
        pSt7920->reg.cgram = 1;
        return;
      }
      else
      {
        ST7920_DispHCGROM(data);
      }
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
      pSt7920->pixel.address_is_y = 1;
      pSt7920->reg.cgram = 0;
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
