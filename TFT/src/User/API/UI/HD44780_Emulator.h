#ifndef _HD44780_SIMULATOR_H_
#define _HD44780_SIMULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../../Configuration.h"

#define XROWS 20
#define YROWS 4
#define FONT_PIXEL  (MIN(LCD_WIDTH/XROWS/BYTE_WIDTH,LCD_HEIGHT/YROWS/BYTE_HEIGHT)) 
#define BITMAP_PIXEL (MIN(LCD_WIDTH/XROWS/6,LCD_HEIGHT/YROWS/9))
#define XSTART ((LCD_WIDTH - FONT_PIXEL*XROWS*BYTE_WIDTH) / 2)  
#define YSTART ((LCD_HEIGHT - FONT_PIXEL*YROWS*BYTE_HEIGHT) / 2)
#define YOFFSET  (BYTE_HEIGHT*FONT_PIXEL-9*BITMAP_PIXEL)

typedef void (*HD44780_CMD)(uint8_t);

typedef enum
{
  HD44780_DATA_DDRAM = 0,
  HD44780_DATA_CGRAM,
} HD44780_DATA_TYPE;

typedef struct
{
  int16_t  x; // current x pixel, range is 0 - 127
  int16_t  y; // current y pixel, range is 0 - 63
  // Extended Instruction, The first address set Y, second address set X
  uint8_t  address_is_y; // record current address is Y or X
}HD44780_PIXEL;

/*** Common Instruction ***/
typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t : 2,
              f : 1,  // Sets the character font.
              n : 1,  // Sets the number of display lines.
              dl : 1; // 4/8-bit interface control bit (0-4bit, 1-8bit)
    };
  };
}HD44780_REG_FS; // Function Set


/*** Basic Instruction ***/
typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t slr : 1, // Display Shift Control: (Shift Left/Right)
              id : 1;  // Address Counter Control: (Increase/Decrease)
    };
  };
}HD44780_BIREG_EMS; // Entry Mode Set

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t blink : 1,   // Character Blink ON/OFF control bit
              cursor : 1,  // Cursor ON/OFF control bit
              display : 1; // Display ON/OFF control bit
    };
  };
}HD44780_BIREG_DC; // Display Control

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t : 2,
              rl : 1, // Cursor moves/shift left/right
              sc : 1; // Display shift, cursor also follows to shift
    };
  };
}HD44780_BIREG_CDSC; // Cursor/Display Shift Control

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t ac : 6; // AC range is 00H…3FH
    };
  };
}HD44780_BIREG_CGRAMA; // Set CGRAM Address

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t ac : 7; // First line AC range is  80H … 93h
                      // Second line AC range is C0H … D3H
                      // Third line AC range is  94H … 2FH
                      // Fourth line AC range is D4H … E7H
    };
  };
}HD44780_BIREG_DDRAMA; // Set DDRAM Address


/*** Extended Instruction ***/
typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t sb : 1;
    };
  };
}HD44780_EIREG_SB; // Standby

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t sr : 1; // "1", the Vertical Scroll mode is enabled.
                      // "0", “Set CGRAM Address” instruction (basic instruction) is enabled
    };
  };
}HD44780_EIREG_VSRA; // Vertical Scroll or RAM Address Select

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t r0 : 1, // R1 R0 Description
              r1 : 1; // 0  0  First line normal or reverse
                      // 0  1  Second line normal or reverse
                      // 1  0  Third line normal or reverse
                      // 1  1  Fourth line normal or reverse
    };
  };
}HD44780_EIREG_REV; // Reverse

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t ac : 6; // HD44780_EIREG_VSR.sr = 1: AC5~AC0 is vertical scroll displacement address
    };
  };
}HD44780_EIREG_SA; // Set Scroll Address

typedef struct {
  union {
    uint8_t reg;
    struct {
      uint8_t ac : 6; //
    };
  };
}HD44780_EIREG_GRAMA; // Set Graphic RAM Address

typedef struct {
  // 1 << 0 : Display Clear
  // 1 << 1 : Return Home
  HD44780_BIREG_EMS    ems;    // 1 << 2 : Entry Mode Set
  HD44780_BIREG_DC     dc;     // 1 << 3 : Display Control
  HD44780_BIREG_CDSC   cdsc;   // 1 << 4 : Cursor/Display Shift Control
  // 1 << 5 : Function Set
  HD44780_BIREG_CGRAMA cgrama; // 1 << 6 : Set CGRAM Address
  HD44780_BIREG_DDRAMA ddrama; // 1 << 7 : Set DDRAM Address
}HD44780_BI; // Basic Instruction

typedef struct {
  HD44780_REG_FS      fs; // Function Set
  HD44780_BI          bi; // Basic Instruction
  uint8_t             reverse; // Begin with normal and toggle to reverse
  uint8_t             cgram;
  HD44780_DATA_TYPE   data_type;
}HD44780_REG; // Extended Instruction

void HD44780_ParseRecv(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
