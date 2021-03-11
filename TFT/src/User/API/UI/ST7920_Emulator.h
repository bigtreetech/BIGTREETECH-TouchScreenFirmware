#ifndef _ST7920_SIMULATOR_H_
#define _ST7920_SIMULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../../Configuration.h"

#define ST7920_XSTART    (0x80)
#define ST7920_YSTART    (0x80)


//# ST7920_FULLSCREEN
#define ST7920_GXROWS_FULLSCREEN  128.0f
#define ST7920_GYROWS_FULLSCREEN  64.0f
#define ST7920_GXDOT_FULLSCREEN   (MIN(LCD_WIDTH / ST7920_GXROWS_FULLSCREEN, LCD_HEIGHT / ST7920_GYROWS_FULLSCREEN))
#define ST7920_GYDOT_FULLSCREEN   (ST7920_GXDOT_FULLSCREEN)
#define ST7920_GXSTART_FULLSCREEN ((LCD_WIDTH - ST7920_GXDOT_FULLSCREEN * ST7920_GXROWS_FULLSCREEN) / 2)
#define ST7920_GYSTART_FULLSCREEN ((LCD_HEIGHT - ST7920_GYDOT_FULLSCREEN * ST7920_GYROWS_FULLSCREEN) / 2)

#define ST7920_GXROWS  128
#define ST7920_GYROWS  64
#define ST7920_GXDOT   (MIN(LCD_WIDTH / ST7920_GXROWS, LCD_HEIGHT / ST7920_GYROWS))
#define ST7920_GYDOT   (ST7920_GXDOT)
#define ST7920_GXSTART ((LCD_WIDTH - ST7920_GXDOT * ST7920_GXROWS) / 2)
#define ST7920_GYSTART ((LCD_HEIGHT - ST7920_GYDOT * ST7920_GYROWS) / 2)

typedef void (*FP_CMD)(uint8_t);

typedef enum
{
  ST7920_IDLE = 0,
  ST7920_WCMD = 0xF8,
  ST7920_WDATA = 0xFA,
  ST7920_RCMD = 0xFC,
  ST7920_RDATA = 0xFE,
} ST7920_CTRL_STATUS;

typedef enum
{
  ST7920_DATA_DDRAM = 0,
  ST7920_DATA_CGRAM,
  ST7920_DATA_GDRAM,
} ST7920_DATA_TYPE;

typedef struct
{
  int16_t  x;             // current x pixel, range is 0 - 127
  int16_t  y;             // current y pixel, range is 0 - 63
  // Extended Instruction, The first address set Y, second address set X
  uint8_t  address_is_y;  // record current address is Y or X
} ST7920_PIXEL;

/*** Common Instruction ***/

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t : 1,
              g : 1,   // Graphic display control bit
              re : 1,  // extended instruction set control bit
              :1,
              dl : 1;  // 4/8-bit interface control bit (0-4bit, 1-8bit)
    };
  };
} ST7920_REG_FS;  // Function Set

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
} ST7920_BIREG_EMS;  // Entry Mode Set

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
} ST7920_BIREG_DC;  // Display Control

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
} ST7920_BIREG_CDSC;  // Cursor/Display Shift Control

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
} ST7920_BIREG_CGRAMA;  // Set CGRAM Address

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t ac : 7;  // First line AC range is 00H…0FH
                       // Second line AC range is 10H…1FH
                       // Third line AC range is 20H…2FH
                       // Fourth line AC range is 30H…3FH
    };
  };
} ST7920_BIREG_DDRAMA;  // Set DDRAM Address

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
} ST7920_EIREG_SB;  // Standby

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t sr : 1;  // "1", the Vertical Scroll mode is enabled.
                       // "0", “Set CGRAM Address” instruction (basic instruction) is enabled
    };
  };
} ST7920_EIREG_VSRA;  // Vertical Scroll or RAM Address Select

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
} ST7920_EIREG_REV;  // Reverse

typedef struct
{
  union
  {
    uint8_t reg;
    struct
    {
      uint8_t ac : 6;  // ST7920_EIREG_VSR.sr = 1: AC5~AC0 is vertical scroll displacement address
    };
  };
} ST7920_EIREG_SA;  // Set Scroll Address

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
} ST7920_EIREG_GRAMA;  // Set Graphic RAM Address

typedef struct
{
  // 1 << 0 : Display Clear
  // 1 << 1 : Return Home
  ST7920_BIREG_EMS    ems;     // 1 << 2 : Entry Mode Set
  ST7920_BIREG_DC     dc;      // 1 << 3 : Display Control
  ST7920_BIREG_CDSC   cdsc;    // 1 << 4 : Cursor/Display Shift Control
  // 1 << 5 : Function Set
  ST7920_BIREG_CGRAMA cgrama;  // 1 << 6 : Set CGRAM Address
  ST7920_BIREG_DDRAMA ddrama;  // 1 << 7 : Set DDRAM Address
} ST7920_BI;  // Basic Instruction

typedef struct
{
  ST7920_EIREG_SB    sb;     // 1 << 0 : Standby
  ST7920_EIREG_VSRA  vsra;   // 1 << 1 : Vertical Scroll or RAM Address Select
  ST7920_EIREG_REV   rev;    // 1 << 2 : Reverse
  // 1 << 3 : Reserved
  // 1 << 4 : Reserved
  // 1 << 5 : Function Set
  ST7920_EIREG_SA    sa;     // 1 << 6 : Set Scroll Address
  ST7920_EIREG_GRAMA grama;  // 1 << 7 : Set Graphic RAM Address
} ST7920_EI;  // Extended Instruction

typedef struct
{
  ST7920_REG_FS      fs;       // Function Set
  ST7920_BI          bi;       // Basic Instruction
  ST7920_EI          ei;       // Extended Instruction
  uint8_t            reverse;  // Begin with normal and toggle to reverse
  uint8_t            cgram;
  ST7920_DATA_TYPE   data_type;
  ST7920_CTRL_STATUS ctrl_status;
} ST7920_REG;  // Extended Instruction

void ST7920_ParseRecv(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
