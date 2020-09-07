#include "ST7920_Simulator.h"
#include "includes.h"
#include "GUI.h"
#include "../../Configuration.h"

#ifdef ST7920_SPI
// CGRAM buffer
uint8_t ST7920_CGRAM[64][2]; // [64*2] = [4 * 16*2*8], means 4 * [16*16] bitmap font,

ST7920_PIXEL st7920 = {
  .x = 0, // current x pixel, range is 0 - 127
  .y = 0, // current y pixel, range is 0 - 63
  .address_is_y = 1, // Extended Instruction, The first address set Y, second address set X
};

ST7920_REG st7920_reg = {
  .fs = //0x00, // default basic instruction, 8-bit MPU interface, Graphic display OFF
        0x26, // default extended instruction, 4-bit MPU interface, Graphic display On
  .bi = {
    .ems  = 0x02, // Cursor move to right ,DDRAM address counter (AC) plus 1
    .dc   = 0x00, // Display, cursor and blink are ALL OFF
    .cdsc = 0x00, // No cursor or display shift operation
 },
  .ei = {
    .vsra = 0x00, // Allow vertical scroll or set CGRAM address
    .rev  = 0x00, // Begin with normal and toggle to reverse
  },
  .reverse = 0, // Begin with normal and toggle to reverse
  .cgram = 0,
  .data_type = ST7920_DATA_DDRAM,
  .ctrl_status = ST7920_IDLE,
};

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
                      st7920_gx_start_full + st7920_gx_dot_full * (x+1),
                      st7920_gy_start_full + st7920_gy_dot_full * (y+1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  } else {
    GUI_FillRectColor(st7920_gx_start + st7920_gx_dot * x,     st7920_gy_start + st7920_gy_dot * y ,
                      st7920_gx_start + st7920_gx_dot * (x+1), st7920_gy_start + st7920_gy_dot * (y+1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  }
}

void ST7920_DrawCharPixel(int16_t sx, int16_t sy, int16_t x, int16_t y, bool isForeGround)
{
  const float st7920_cx_dot_full = (ST7920_GXDOT_FULLSCREEN * 8.0f) / BYTE_WIDTH;
  const float st7920_cy_dot_full = (ST7920_GYDOT_FULLSCREEN * 16.0f) / BYTE_HEIGHT;
  const uint16_t st7920_cx_dot = (st7920_gx_dot * 8) / BYTE_WIDTH;
  const uint16_t st7920_cy_dot = (st7920_gy_dot * 16) / BYTE_HEIGHT;
  if (infoSettings.marlin_mode_fullscreen)
  {
    GUI_FillRectColor(st7920_gx_start_full + st7920_gx_dot_full * sx + st7920_cx_dot_full * x,
                      st7920_gy_start_full + st7920_gy_dot_full * sy + st7920_cy_dot_full * y,
                      st7920_gx_start_full + st7920_gx_dot_full * sx + st7920_cx_dot_full * (x+1),
                      st7920_gy_start_full + st7920_gy_dot_full * sy + st7920_cy_dot_full * (y+1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  } else {
    GUI_FillRectColor(st7920_gx_start + st7920_gx_dot * sx + st7920_cx_dot * x,
                      st7920_gy_start + st7920_gy_dot * sy + st7920_cy_dot * y ,
                      st7920_gx_start + st7920_gx_dot * sx + st7920_cx_dot * (x+1),
                      st7920_gy_start + st7920_gy_dot * sy + st7920_cy_dot * (y+1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  }
}

// Display graphic
void ST7920_DrawGDRAM(uint8_t data)
{
  if (st7920_reg.fs.g != 1) return; // Graphic display off
  for (uint8_t i = 0; i < 8; i++) {
    if (data & 0x80)
      ST7920_DrawPixel(st7920.x, st7920.y, true);
    else
      ST7920_DrawPixel(st7920.x, st7920.y, false);
    data <<= 1;
    st7920.x++;
  }

  if (st7920.x >= 128) {
    st7920.x = 0;
    st7920.y = (st7920.y + 32);
    if (st7920.y >= 64) st7920.y = (st7920.y + 1) & (64 - 1);
  }
}

// Display CGRAM fonts, only 0x0000, 0x0002, 0x0004, 0x0006
void ST7920_DispCGRAM(uint8_t data)
{
  uint8_t ex = st7920.x + 16, // GRAM, 16*16 bitmap font
          ey = st7920.y + 16,
          yIndex = data * 8;

  for (uint8_t y = st7920.y; y < ey; y++) {
    uint16_t temp = ST7920_CGRAM[yIndex][0] << 8 | ST7920_CGRAM[yIndex][1];
    for (uint8_t x = st7920.x; x < ex; x++) {
      ST7920_DrawPixel(x, y, temp & (1<<15));
      temp <<= 1;
    }
    yIndex++;
  }

  st7920.x += 16;
  if (st7920.x >= 128) {
    st7920.x = 0;
    st7920.y = (st7920.y + 32);
    if (st7920.y == 64) st7920.y = 16;
    else if (st7920.y == 80) st7920.y = 0;
  }
}

// Display HCGROM fonts, 02H~7FH.
void ST7920_DispHCGROM(uint8_t p)
{
  if (p < ' ' || p > '~') return;

  CHAR_INFO info = {.bytes = 0};
  getCharacterInfo(&p, &info);

  uint8_t  i = 0;
  uint8_t  font[BYTE_HEIGHT * BYTE_WIDTH / 8];
  uint32_t temp = 0;

  W25Qxx_ReadBuffer(font, info.bitMapAddr, BYTE_HEIGHT * BYTE_WIDTH / 8);

  for (uint8_t x = 0; x < BYTE_WIDTH; x++) {

    for(uint8_t j=0; j < (BYTE_HEIGHT + 8-1)/8; j++)
    {
      temp <<= 8;
      temp |= font[i++];
    }

    for (uint8_t y = 0; y < BYTE_HEIGHT; y++) {
      ST7920_DrawCharPixel(st7920.x, st7920.y, x, y, temp & (1<<BYTE_HEIGHT));
      temp <<= 1;
    }
  }

  st7920.x += 8;
  if (st7920.x >= 128) {
    st7920.x = 0;
    st7920.y = (st7920.y + 32);
    if (st7920.y == 64) st7920.y = 16;
    else if (st7920.y == 80) st7920.y = 0;
  }
}


/*** Common instruction ***/
// cmd : 1 << 5
void ST7920_CI15_FunctionSet(uint8_t cmd)
{
  st7920_reg.fs.reg = cmd;
}

/*** Basic instruction ***/
// cmd : 1 << 0
void ST7920_BI10_DisplayClear(uint8_t cmd)
{
  // Fill DDRAM with "20H"(space code).
  for (uint8_t y = 0; y < 64; y++) {
    for (uint8_t x = 0; x < 128; x++) {
      ST7920_DrawPixel(x, y, false);
    }
  }
  // Set DDRAM address counter (AC) to"00H"
  st7920.x = st7920.y = 0;
  // Set Entry Mode I/D bit to be "1". Cursor moves right and AC adds 1 after write or read operation
  st7920_reg.bi.ems.id = 1;
}
// cmd : 1 << 1
void ST7920_BI11_ReturnHome(uint8_t cmd)
{
  // Set address counter (AC) to "00H".
  st7920.x = st7920.y = 0;
}
// cmd : 1 << 2
void ST7920_BI12_EntryModeSet(uint8_t cmd)
{
  st7920_reg.bi.ems.reg = cmd;
}
// cmd : 1 << 3
void ST7920_BI13_DisplayControl(uint8_t cmd)
{
  st7920_reg.bi.dc.reg = cmd;
}
// cmd : 1 << 4
void ST7920_BI14_CursorDisplayControl(uint8_t cmd)
{
  st7920_reg.bi.cdsc.reg = cmd;
}
// cmd : 1 << 6
void ST7920_BI16_SetCGRAMAddress(uint8_t cmd)
{
  st7920_reg.bi.cgrama.reg = cmd;
  uint8_t address = st7920_reg.bi.cgrama.ac;
  // Set CGRAM address
  // x is 0-1 = [2*8], y is 0-64 = [4*16], means 4 * [16*16] bitmap font
  st7920.x = 0;
  st7920.y = address;
  st7920_reg.data_type = ST7920_DATA_CGRAM;
}
// the data follow cmd : 1 << 6
void ST7920_BI_SetCGRAMData(uint8_t data)
{
  ST7920_CGRAM[st7920.y][st7920.x++] = data;
  if (st7920.x >= 2) {
    st7920.x = 0;
    st7920.y = (st7920.y + 1) & (64 - 1);
  }
}
// cmd : 1 << 7
void ST7920_BI17_SetDDRAMAddress(uint8_t cmd)
{
  st7920_reg.bi.ddrama.reg = cmd;
  uint8_t address = st7920_reg.bi.ddrama.ac;
  // Set DDRAM address
  // x is 0-127. y is 0-63
  st7920.x = (address & 0x07) * 16;
  st7920.y = ((address >> 4) & 0x01) * 16 + ((address >> 3) & 0x01) * 32;
  st7920_reg.data_type = ST7920_DATA_DDRAM;
}

/*** Extended Instruction ***/
// cmd : 1 << 0
void ST7920_EI10_StandBy(uint8_t cmd)
{
  st7920_reg.ei.sb.reg = cmd;
}
// cmd : 1 << 1
void ST7920_EI11_ScrollOrRAMAddress(uint8_t cmd)
{
  st7920_reg.ei.vsra.reg = cmd;
}
// cmd : 1 << 2
void ST7920_EI12_Reverse(uint8_t cmd)
{
  st7920_reg.ei.rev.reg = cmd;
  st7920_reg.reverse = !st7920_reg.reverse; // Reverse the display by toggling this instruction
}
// cmd : 1 << 6
void ST7920_EI16_SetScrollAddress(uint8_t cmd)
{
  st7920_reg.ei.sa.reg = cmd;
}
// cmd : 1 << 7
void ST7920_EI17_SetGRAMAddress(uint8_t cmd)
{
  st7920_reg.ei.grama.reg = cmd;
  uint8_t address = st7920_reg.ei.grama.ac;
  if (st7920.address_is_y) {
    st7920.y = address;
    st7920.address_is_y = 0;
  } else {
    if (address & 0x08) st7920.y += 32;
    st7920.x = (address & 0x07) * 16;
    st7920.address_is_y = 1;
  }
  st7920_reg.data_type = ST7920_DATA_GDRAM;
}

static const FP_CMD st7920CmdCallBack[8][2] = {
// Basic Instruction                 Extended Instruction
{ ST7920_BI10_DisplayClear,          ST7920_EI10_StandBy},            // cmd 1 << 0
{ ST7920_BI11_ReturnHome,            ST7920_EI11_ScrollOrRAMAddress}, // cmd 1 << 1
{ ST7920_BI12_EntryModeSet,          ST7920_EI12_Reverse},            // cmd 1 << 2
{ ST7920_BI13_DisplayControl,        NULL},                           // cmd 1 << 3
{ ST7920_BI14_CursorDisplayControl,  NULL},                           // cmd 1 << 4
{ ST7920_CI15_FunctionSet,           ST7920_CI15_FunctionSet},        // cmd 1 << 5
{ ST7920_BI16_SetCGRAMAddress,       ST7920_EI16_SetScrollAddress},   // cmd 1 << 6
{ ST7920_BI17_SetDDRAMAddress,       ST7920_EI17_SetGRAMAddress},     // cmd 1 << 7
};

void ST7920_ST7920_ParseWCmd(uint8_t cmd)
{
  for (int8_t i = 7; i >= 0; i--) {
    if (cmd & (1 << i)) {
      if (st7920CmdCallBack[i][st7920_reg.fs.re] != NULL) {
        (*st7920CmdCallBack[i][st7920_reg.fs.re])(cmd);
      }
      break;
    }
  }
}

void ST7920_ST7920_ParseWData(uint8_t data)
{
  switch (st7920_reg.data_type) {
    case ST7920_DATA_DDRAM:
      if (st7920_reg.cgram == 1) {
        st7920_reg.cgram = 0;
        ST7920_DispCGRAM(data);
      } else if (data == 0x00) {
        st7920_reg.cgram = 1;
        return;
      } else {
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
  if(data == ST7920_WCMD || data == ST7920_WDATA || data == ST7920_RCMD || data == ST7920_RDATA)
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
    st7920_reg.ctrl_status = (ST7920_CTRL_STATUS)val;
    rcvIndex = 0;
    if (st7920_reg.ctrl_status == ST7920_WDATA) {
      st7920.address_is_y = 1;
      st7920_reg.cgram = 0;
    }
  }
  else
  {
    val &= 0xF0;           // Every 8 bits instruction/data will be separated into 2 groups, lower 4 bits always 0 in every groups
    if (rcvIndex == 0) {
      rcvData = val;       // Higher 4 bits in first byte
      rcvIndex++;
      return;
    } else {
      rcvData |= val >> 4; // Lower 4 bits in second byte
      rcvIndex = 0;
    }

    switch (st7920_reg.ctrl_status)
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
