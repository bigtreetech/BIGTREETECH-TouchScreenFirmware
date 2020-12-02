#include "SendGcode.h"
#include "includes.h"

#if ((defined(MKS_28_V1_0)) || (defined(MKS_32_V1_4) && !defined(MKS_32_V1_4_NOBL)))
  #define TERMINAL_MAX_CHAR  4800 // for 64k RAM
#else
  #define TERMINAL_MAX_CHAR  3600 // for 48k RAM
#endif
#define MAX_BUFF          20
#define SCROLL_LINE       22
#define SCROLL_PAGE       1

typedef struct
{
  char *ptr[MAX_BUFF];                                     // Pointer into the terminal page buffer, full Screen is one page
  uint8_t pageHead;                                        // Buffer top of page
  uint8_t pageTail;                                        // Buffer buttom of page
  uint8_t oldPageHead;
  uint8_t pageIndex;                                       // page buffer index
  uint8_t oldPageIndex;
} TERMINAL_PAGE;

// colors
#if TERMINAL_KEYBOARD_COLOR_LAYOUT == 0
  #define KEY_FONT_COLOR     infoSettings.font_color
  #define KEY_BG_COLOR       infoSettings.bg_color
  #define KEY_BORDER_COLOR   infoSettings.list_border_color
  #define KEY_BORDER_COLOR_2 DARKGRAY
#elif TERMINAL_KEYBOARD_COLOR_LAYOUT == 1
  #define KEY_FONT_COLOR     infoSettings.font_color
  #define KEY_BG_COLOR       infoSettings.list_button_color
  #define KEY_BORDER_COLOR   infoSettings.bg_color
  #define KEY_BORDER_COLOR_2 DARKGRAY
#else
  #define KEY_FONT_COLOR     BLACK
  #define KEY_BG_COLOR       WHITE
  #define KEY_BORDER_COLOR   0x2174
  #define KEY_BORDER_COLOR_2 DARKGRAY
#endif

#define BAR_FONT_COLOR       WHITE
#define BAR_BG_COLOR         0x2174
#define BAR_BORDER_COLOR     0x4b0d
#define BAR_VALUE_FONT_COLOR BLACK
#define BAR_VALUE_BG_COLOR   0xFFF2

#define TERM_FONT_COLOR infoSettings.marlin_mode_font_color
#define TERM_BG_COLOR   infoSettings.marlin_mode_bg_color

// keyboard layouts
#define LAYOUT_1_COL_NUM 6
#define LAYOUT_1_ROW_NUM 6

#define LAYOUT_2_COL_NUM 7
#define LAYOUT_2_ROW_NUM 6

#define LAYOUT_3_COL_NUM 10
#define LAYOUT_3_ROW_NUM 8

#if LCD_WIDTH < 480                                        // number of columns and rows is based on LCD display resolution
  #define GKEY_COL_NUM LAYOUT_1_COL_NUM
  #define GKEY_ROW_NUM LAYOUT_1_ROW_NUM
#elif LCD_WIDTH < 800
  #define GKEY_COL_NUM LAYOUT_2_COL_NUM
  #define GKEY_ROW_NUM LAYOUT_2_ROW_NUM
#else
  #define GKEY_COL_NUM LAYOUT_3_COL_NUM
  #define GKEY_ROW_NUM LAYOUT_3_ROW_NUM
#endif

#define GKEY_WIDTH  (LCD_WIDTH / GKEY_COL_NUM)
#define GKEY_HEIGHT (LCD_HEIGHT / GKEY_ROW_NUM)

typedef enum
{
  GKEY_0 = 0,
  GKEY_1,
  GKEY_2,
  GKEY_3,
  GKEY_4,
  GKEY_5,
  GKEY_6,
  GKEY_7,
  GKEY_8,
  GKEY_9,
  GKEY_10,
  GKEY_11,
  GKEY_12,
  GKEY_13,
  GKEY_14,
  GKEY_15,
  GKEY_16,
  GKEY_17,
  GKEY_18,
  GKEY_19,
  GKEY_20,
  GKEY_21,
  GKEY_22,
  GKEY_23,
#if GKEY_COL_NUM > LAYOUT_1_COL_NUM
  GKEY_24,
  GKEY_25,
  GKEY_26,
  GKEY_27,
#endif
#if GKEY_COL_NUM > LAYOUT_2_COL_NUM
  GKEY_28,
  GKEY_29,
  GKEY_30,
  GKEY_31,
  GKEY_32,
  GKEY_33,
  GKEY_34,
  GKEY_35,
  GKEY_36,
  GKEY_37,
  GKEY_38,
  GKEY_39,
  GKEY_40,
  GKEY_41,
  GKEY_42,
  GKEY_43,
  GKEY_44,
  GKEY_45,
  GKEY_46,
  GKEY_47,
  GKEY_48,
  GKEY_49,
  GKEY_50,
  GKEY_51,
  GKEY_52,
  GKEY_53,
  GKEY_54,
  GKEY_55,
  GKEY_56,
  GKEY_57,
  GKEY_58,
  GKEY_59,
#endif

#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
  GKEY_ABC_123,
#endif

  GKEY_SPACE,
  GKEY_DEL,
  GKEY_BACK,
  GKEY_SEND,
  GKEY_KEY_NUM,                                            // number of keys
  GKEY_IDLE = IDLE_TOUCH,
} GKEY_VALUES;

// gcode bar sizes
#define GCODE_ROW_NUM 1
#define GCODE_COL_NUM 4
#define GCODE_HEIGHT  (GKEY_HEIGHT / GCODE_ROW_NUM)
#define GCODE_WIDTH   (LCD_WIDTH / GCODE_COL_NUM)
#define GCODE_X0      0
#define GCODE_Y0      0

// keyboard sizes
#define KEY_ROW_NUM GKEY_ROW_NUM - 2
#define KEY_COL_NUM GKEY_COL_NUM
#define KEY_HEIGHT  GKEY_HEIGHT
#define KEY_WIDTH   GKEY_WIDTH
#define KEY_X0      0
#define KEY_Y0      GKEY_HEIGHT

// control bar sizes
#define CTRL_ROW_NUM 1

#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
  typedef enum
  {
    SOFT_KEY_123 = 0,
    SOFT_KEY_ABC,
  } SOFT_KEY_TYPE;

  SOFT_KEY_TYPE gcodeKeyType = SOFT_KEY_123;

  #define CTRL_COL_NUM 4
#else
  #define CTRL_COL_NUM 3
#endif

#define CTRL_HEIGHT  GKEY_HEIGHT
#define CTRL_WIDTH   (LCD_WIDTH / CTRL_COL_NUM)
#define CTRL_X0      0
#define CTRL_Y0      (LCD_HEIGHT - GKEY_HEIGHT)

// gcode value rectangles
const GUI_RECT gcodeValueRect = {
  GCODE_X0 + 0 * GCODE_WIDTH + 4, GCODE_Y0 + 0 * GCODE_HEIGHT + 4, GCODE_X0 + 3 * GCODE_WIDTH - 4, GCODE_Y0 + 1 * GCODE_HEIGHT - 4
};

// keyboard rectangles
const GUI_RECT gcodeKeyRect[GKEY_KEY_NUM]={
  // row 1
  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
#if GKEY_COL_NUM > LAYOUT_1_COL_NUM
  {KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
#endif
#if GKEY_COL_NUM > LAYOUT_2_COL_NUM
  {KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 10 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
#endif

  // row 2
  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
#if GKEY_COL_NUM > LAYOUT_1_COL_NUM
  {KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
#endif
#if GKEY_COL_NUM > LAYOUT_2_COL_NUM
  {KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 10 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
#endif

  // row 3
  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
#if GKEY_COL_NUM > LAYOUT_1_COL_NUM
  {KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
#endif
#if GKEY_COL_NUM > LAYOUT_2_COL_NUM
  {KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 10 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
#endif

  // row 4
  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
#if GKEY_COL_NUM > LAYOUT_1_COL_NUM
  {KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
#endif
#if GKEY_COL_NUM > LAYOUT_2_COL_NUM
  {KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 10 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
#endif

#if GKEY_COL_NUM > LAYOUT_2_COL_NUM
  // row 5
  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},
  {KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT, KEY_X0 + 10 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT},

  // row 6
  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 7 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 8 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
  {KEY_X0 + 9 * KEY_WIDTH, KEY_Y0 + 5 * KEY_HEIGHT, KEY_X0 + 10 * KEY_WIDTH, KEY_Y0 + 6 * KEY_HEIGHT},
#endif

  // row control bar
#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
  {CTRL_X0 + 0 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 1 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // ABC or 123
  {CTRL_X0 + 1 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 2 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Space
  {CTRL_X0 + 2 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 3 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Del
  {CTRL_X0 + 3 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 4 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Back
#else
  {CTRL_X0 + 0 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 1 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Space
  {CTRL_X0 + 1 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 2 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Del
  {CTRL_X0 + 2 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 3 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Back
#endif

  // row gcode bar
  {GCODE_X0 + 3 * GCODE_WIDTH, GCODE_Y0 + 0 * GCODE_HEIGHT, LCD_WIDTH, GCODE_Y0 + 1 * GCODE_HEIGHT},                   // Send
};

// keyboard keys
const char * const gcodeKey[][GKEY_KEY_NUM] = {
  {
#if GKEY_COL_NUM == LAYOUT_1_COL_NUM
    "1", "2", "3", "M", "G", "T",
    "4", "5", "6", "X", "Y", "Z",
    "7", "8", "9", "E", "F", "R",
    ".", "0", "-", "~", "S", "V",
#elif GKEY_COL_NUM == LAYOUT_2_COL_NUM
    "1", "2", "3", "M", "G", "T", "V",
    "4", "5", "6", "X", "Y", "Z", "S",
    "7", "8", "9", "E", "F", "R", "Q",
    ".", "0", "-", "~", "I", "J", "P",
#else
  #if TERMINAL_KEYBOARD_QWERTY_LAYOUT == 1
    "\\","|", "!", "\"","$", "%", "&", "/", "=", "?",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", "'",
    "Z", "X", "C", "V", "B", "N", "M", ",", ".", ";",
    ":", "_", "@", "#", "~", "-", "+", "*", "(", ")", 
  #else
    "1", "2", "3", "A", "B", "C", "D", "E", "F", "G",
    "4", "5", "6", "H", "I", "J", "K", "L", "M", "N",
    "7", "8", "9", "O", "P", "Q", "R", "S", "T", "U",
    ".", "0", "-", "~", "V", "W", "X", "Y", "Z", "'",
    "+", "*", "?", "!", "#", "&", "$", ",", ";", ":",
    "/", "=", "(", ")", "@", "_", "%", "|", "\"","\\",
  #endif
#endif

#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
    "ABC",
#endif

    "Space", "Del", "Back",
    "Send",
  },
  {
#if GKEY_COL_NUM == LAYOUT_1_COL_NUM
    "A", "B", "C", "D", "H", "I",
    "J", "K", "L", "N", "O", "P",
    ",", ";", ":", "Q", "U", "W",
    "+", "*", "?", "!", "#", "&",
#elif GKEY_COL_NUM == LAYOUT_2_COL_NUM
    "A", "B", "C", "D", "H", "K", "L",
    ",", ";", ":", "N", "O", "U", "W",
    "+", "*", "?", "!", "#", "&", "$",
    "/", "=", "(", ")", "@", "_", "%",
#endif

#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
    "123",
#endif

    "Space", "Del", "Back",
    "Send",
  }
};

// area rectangles
const GUI_RECT gcodeAreaRect[3] = {
  {GCODE_X0, GCODE_Y0, LCD_WIDTH, GKEY_HEIGHT},            // gcode area
  {KEY_X0, KEY_Y0, LCD_WIDTH, LCD_HEIGHT - GKEY_HEIGHT},   // keyboard area
  {CTRL_X0, CTRL_Y0, LCD_WIDTH, LCD_HEIGHT},               // control area
};

typedef enum
{
  TERM_PAGE_UP = 0,
  TERM_PAGE_DOWN,
  TERM_BACK,
  TERM_KEY_NUM,                                            // number of keys
  TERM_IDLE = IDLE_TOUCH,
} TERMINAL_KEY_VALUES;

#define CURSOR_START_X 0
#define CURSOR_END_X   LCD_WIDTH
#define CURSOR_START_Y 0
#define CURSOR_END_Y   CTRL_Y0

#define PAGE_ROW_NUM 1
#define PAGE_COL_NUM 4
#define PAGE_HEIGHT  GKEY_HEIGHT
#define PAGE_WIDTH   (LCD_WIDTH / PAGE_COL_NUM)
#define PAGE_X0      0
#define PAGE_Y0      CURSOR_END_Y

const GUI_RECT terminalPageRect = {
  PAGE_X0 + 0 * PAGE_WIDTH, PAGE_Y0 + 0 * PAGE_HEIGHT, PAGE_X0 + 1 * PAGE_WIDTH, PAGE_Y0 + 1 * PAGE_HEIGHT
};

const GUI_RECT terminalKeyRect[TERM_KEY_NUM] = {
  {PAGE_X0 + 1 * PAGE_WIDTH, PAGE_Y0 + 0 * PAGE_HEIGHT, PAGE_X0 + 2 * PAGE_WIDTH, PAGE_Y0 + 1 * PAGE_HEIGHT},
  {PAGE_X0 + 2 * PAGE_WIDTH, PAGE_Y0 + 0 * PAGE_HEIGHT, PAGE_X0 + 3 * PAGE_WIDTH, PAGE_Y0 + 1 * PAGE_HEIGHT},
  {PAGE_X0 + 3 * PAGE_WIDTH, PAGE_Y0 + 0 * PAGE_HEIGHT, PAGE_X0 + 4 * PAGE_WIDTH, PAGE_Y0 + 1 * PAGE_HEIGHT},
};

const char * const terminalKey[TERM_KEY_NUM] = {
  "<",
  ">",
  "Back"
};

const GUI_RECT terminalAreaRect[2] = {
  {CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y},                // terminal area
  {PAGE_X0, PAGE_Y0, LCD_WIDTH, LCD_HEIGHT},                                   // control area
};

char terminalBuf[TERMINAL_MAX_CHAR];
TERMINAL_PAGE terminal_page = {terminalBuf, 0, 0, 0, 0, 0};
static uint16_t terminalBufTail = 0;
uint8_t buf_full = 0;

void sendGcodeReDrawButton(u8 index, u8 isPressed)
{
  if (index >= GKEY_KEY_NUM)
    return;

  if (isPressed)
  {
#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
    if (index < GKEY_ABC_123)
#else
    if (index < GKEY_SPACE)
#endif
    {
      GUI_SetColor(KEY_BG_COLOR);
      GUI_SetBkColor(KEY_FONT_COLOR);
    }
    else
    {
      GUI_SetColor(BAR_BG_COLOR);
      GUI_SetBkColor(BAR_FONT_COLOR);
    }
  }
  else
  {
#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
    if (index < GKEY_ABC_123)
#else
    if (index < GKEY_SPACE)
#endif
    {
      GUI_SetColor(KEY_FONT_COLOR);
      GUI_SetBkColor(KEY_BG_COLOR);
    }
    else
    {
      GUI_SetColor(BAR_FONT_COLOR);
      GUI_SetBkColor(BAR_BG_COLOR);
    }
  }

  if (index != GKEY_SEND)
    setLargeFont(true);

  GUI_ClearRect(gcodeKeyRect[index].x0 + 2, gcodeKeyRect[index].y0 + 2, gcodeKeyRect[index].x1 - 1, gcodeKeyRect[index].y1 - 1);

#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
  GUI_DispStringInRect(gcodeKeyRect[index].x0 + 2, gcodeKeyRect[index].y0 + 2, gcodeKeyRect[index].x1 - 1, gcodeKeyRect[index].y1 - 1, (u8 *) gcodeKey[gcodeKeyType][index]);
#else
  GUI_DispStringInRect(gcodeKeyRect[index].x0 + 2, gcodeKeyRect[index].y0 + 2, gcodeKeyRect[index].x1 - 1, gcodeKeyRect[index].y1 - 1, (u8 *) gcodeKey[0][index]);
#endif

  setLargeFont(false);
}

void sendGcodeDrawGcode(char *gcode)
{
  GUI_SetColor(BAR_VALUE_FONT_COLOR);
  GUI_SetBkColor(BAR_VALUE_BG_COLOR);

  GUI_ClearRect(gcodeValueRect.x0 + 1, gcodeValueRect.y0 + 1, gcodeValueRect.x1 - 1, gcodeValueRect.y1 - 1);

  if (gcode != NULL)
    GUI_DispStringInRect(gcodeValueRect.x0 + 1, gcodeValueRect.y0 + 1, gcodeValueRect.x1 - 1, gcodeValueRect.y1 - 1, (u8 *) gcode);
}

void sendGcodeDrawKeyboard(void)
{
  GUI_SetColor(KEY_BORDER_COLOR);

  // draw vertical button borders
  for (int i = 0; i < (KEY_COL_NUM - 1); i++)
  {
    GUI_DrawLine(gcodeKeyRect[i].x1, gcodeKeyRect[i].y0, gcodeKeyRect[i + ((KEY_ROW_NUM - 1) * KEY_COL_NUM)].x1, gcodeKeyRect[i + ((KEY_ROW_NUM - 1) * KEY_COL_NUM)].y1 - 1);
  }

  // draw horizontal button borders
  for (int i = 0; i < (KEY_ROW_NUM - 1); i++)
  {
    GUI_DrawLine(gcodeKeyRect[i * KEY_COL_NUM].x0, gcodeKeyRect[i * KEY_COL_NUM].y1, gcodeKeyRect[(i * KEY_COL_NUM) + (KEY_COL_NUM - 1)].x1 - 1, gcodeKeyRect[(i * KEY_COL_NUM) + (KEY_COL_NUM - 1)].y1);
  }

  GUI_SetColor(KEY_BORDER_COLOR_2);

  // draw shadow border
  GUI_DrawLine(gcodeAreaRect[1].x0, gcodeAreaRect[1].y0, gcodeAreaRect[1].x1, gcodeAreaRect[1].y0);
  GUI_DrawLine(gcodeAreaRect[1].x0, gcodeAreaRect[1].y1 - 1, gcodeAreaRect[1].x1, gcodeAreaRect[1].y1 - 1);

  for (uint8_t i = 0; i < COUNT(gcodeKey[0]); i++)
  {
    sendGcodeReDrawButton(i, false);
  }
}

void sendGcodeDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(gcodeKeyRect), gcodeKeyRect, sendGcodeReDrawButton);

  GUI_SetBkColor(BAR_BG_COLOR);

  // clear bar area
  GUI_ClearRect(gcodeAreaRect[0].x0, gcodeAreaRect[0].y0, gcodeAreaRect[0].x1, gcodeAreaRect[0].y1);
  GUI_ClearRect(gcodeAreaRect[2].x0, gcodeAreaRect[2].y0, gcodeAreaRect[2].x1, gcodeAreaRect[2].y1);

  GUI_SetBkColor(KEY_BG_COLOR);

  // clear keyboard area
  GUI_ClearRect(gcodeAreaRect[1].x0, gcodeAreaRect[1].y0, gcodeAreaRect[1].x1, gcodeAreaRect[1].y1);

  GUI_SetColor(BAR_BORDER_COLOR);

  // draw gcode value area border
  GUI_DrawPrect(&gcodeValueRect);

  // draw bar area shadow border
  GUI_DrawLine(gcodeAreaRect[0].x0, gcodeAreaRect[0].y1 - 1, gcodeAreaRect[0].x1, gcodeAreaRect[0].y1 - 1);
  GUI_DrawLine(gcodeAreaRect[2].x0, gcodeAreaRect[2].y0, gcodeAreaRect[2].x1, gcodeAreaRect[2].y0);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  // draw keyboard and gcode value
  sendGcodeDrawKeyboard();
  sendGcodeDrawGcode(NULL);
}

void menuSendGcode(void)
{
  GKEY_VALUES key_num = GKEY_IDLE;
  char gcodeBuf[CMD_MAX_CHAR] = {0};
  uint8_t nowIndex = 0,
          lastIndex = 0;

  sendGcodeDrawMenu();

  while (infoMenu.menu[infoMenu.cur] == menuSendGcode)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case GKEY_IDLE:
        break;

      case GKEY_BACK:
        infoMenu.cur--;
        break;

      case GKEY_SEND:
        if (nowIndex)
        {
          gcodeBuf[nowIndex++] = '\n';                     // End char '\n' for Gcode
          gcodeBuf[nowIndex] = 0;
          storeCmd(gcodeBuf);
          gcodeBuf[nowIndex = 0] = 0;
        }

        infoMenu.menu[++infoMenu.cur] = menuTerminal;
        break;

#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
      case GKEY_ABC_123:
        gcodeKeyType = (gcodeKeyType == SOFT_KEY_123) ? SOFT_KEY_ABC : SOFT_KEY_123;

        sendGcodeDrawKeyboard();
        break;
#endif

      case GKEY_DEL:
        if (nowIndex)
          gcodeBuf[--nowIndex] = 0;
        break;

      case GKEY_SPACE:
        if (nowIndex < CMD_MAX_CHAR - 1)
        {
          gcodeBuf[nowIndex++] = ' ';
          gcodeBuf[nowIndex] = 0;
        }
        break;

      default:
        if (nowIndex < CMD_MAX_CHAR - 1)
        {
#if GKEY_COL_NUM < LAYOUT_3_COL_NUM
          gcodeBuf[nowIndex++] = gcodeKey[gcodeKeyType][key_num][0];
#else
          gcodeBuf[nowIndex++] = gcodeKey[0][key_num][0];
#endif
          gcodeBuf[nowIndex] = 0;
        }
        break;
    }

    if (lastIndex != nowIndex)
    {
      lastIndex = nowIndex;

      sendGcodeDrawGcode(gcodeBuf);
    }

    loopBackEnd();
  }

  GUI_RestoreColorDefault();
}

void saveGcodeTerminalCache(uint16_t sign_len, char *str)
{
  uint16_t len = 0;

  if ((terminalBufTail + sign_len) < TERMINAL_MAX_CHAR)
  {
    memcpy(terminalBuf + terminalBufTail, str, sign_len);
    terminalBufTail += sign_len;
  }
  else
  {
    len = (terminalBufTail + sign_len) - TERMINAL_MAX_CHAR;
    memcpy(terminalBuf + terminalBufTail, str, (sign_len - len));
    terminalBufTail = 0;
    memcpy(terminalBuf + terminalBufTail, str + (sign_len - len), len);
    terminalBufTail += len;
    buf_full = 1;
    terminal_page.pageHead++;
  }
}

void sendGcodeTerminalCache(char *stream, TERMINAL_SRC src)
{
  uint16_t sign_len = 0;
  uint16_t stream_len = 0;
  const char* const terminalSign[] = {"Send: ", "Rcv: "};

  if (infoMenu.menu[infoMenu.cur] != menuSendGcode && infoMenu.menu[infoMenu.cur] != menuTerminal)
    return;

  sign_len = strlen(terminalSign[src]);
  saveGcodeTerminalCache(sign_len, (char *) terminalSign[src]);
  stream_len = strlen(stream);
  saveGcodeTerminalCache(stream_len, stream);
}

void terminalReDrawButton(u8 index, u8 isPressed)
{
  if (index >= TERM_KEY_NUM)
    return;

  if (isPressed)
  {
    GUI_SetColor(BAR_BG_COLOR);
    GUI_SetBkColor(BAR_FONT_COLOR);
  }
  else
  {
    GUI_SetColor(BAR_FONT_COLOR);
    GUI_SetBkColor(BAR_BG_COLOR);
  }

  setLargeFont(true);

  GUI_ClearRect(terminalKeyRect[index].x0 + 2, terminalKeyRect[index].y0 + 2, terminalKeyRect[index].x1 - 1, terminalKeyRect[index].y1 - 1);
  GUI_DispStringInRect(terminalKeyRect[index].x0 + 2, terminalKeyRect[index].y0 + 2, terminalKeyRect[index].x1 - 1, terminalKeyRect[index].y1 - 1, (u8 *) terminalKey[index]);

  setLargeFont(false);
}

void terminalDrawPage(char *pageNum)
{
  GUI_SetColor(BAR_FONT_COLOR);
  GUI_SetBkColor(BAR_BG_COLOR);

  setLargeFont(true);

  GUI_ClearRect(terminalPageRect.x0 + 1, terminalPageRect.y0 + 1, terminalPageRect.x1 - 1, terminalPageRect.y1 - 1);

  if (pageNum != NULL)
    GUI_DispStringInRect(terminalPageRect.x0 + 1, terminalPageRect.y0 + 1, terminalPageRect.x1 - 1, terminalPageRect.y1 - 1, (u8 *) pageNum);

  setLargeFont(false);
}

void terminalDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(terminalKeyRect), terminalKeyRect, terminalReDrawButton);

  GUI_SetBkColor(TERM_BG_COLOR);

  // clear terminal area
  GUI_ClearRect(terminalAreaRect[0].x0, terminalAreaRect[0].y0, terminalAreaRect[0].x1, terminalAreaRect[0].y1);

  GUI_SetBkColor(BAR_BG_COLOR);

  // clear bar area
  GUI_ClearRect(terminalAreaRect[1].x0, terminalAreaRect[1].y0, terminalAreaRect[1].x1, terminalAreaRect[1].y1);

  GUI_SetColor(BAR_BORDER_COLOR);

  // draw bar area shadow border
  GUI_DrawLine(terminalAreaRect[1].x0, terminalAreaRect[1].y0, terminalAreaRect[1].x1, terminalAreaRect[1].y0);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  // draw keyboard
  for (uint8_t i = 0; i < COUNT(terminalKey); i++)
  {
    terminalReDrawButton(i, false);
  }
}

void menuTerminal(void)
{
  uint16_t key_num = IDLE_TOUCH;
  CHAR_INFO info;
  uint16_t lastTerminalIndex = 0;
  uint8_t pageBufIndex = 0;
  int16_t cursorX = CURSOR_START_X,
          cursorY = CURSOR_START_Y;
  char pageNum[10];

  terminalDrawMenu();

  while (infoMenu.menu[infoMenu.cur] == menuTerminal)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case TERM_PAGE_UP:                                   // page up
        if (terminal_page.pageIndex < (terminal_page.pageTail - terminal_page.pageHead))
          terminal_page.pageIndex += SCROLL_PAGE;

        if ((terminal_page.pageTail < terminal_page.pageHead) &&
          (terminal_page.pageIndex < (terminal_page.pageTail + MAX_BUFF - terminal_page.pageHead)))
          terminal_page.pageIndex += SCROLL_PAGE;
        break;

      case TERM_PAGE_DOWN:                                 // page down
        if (terminal_page.pageIndex > 0)
          terminal_page.pageIndex -= SCROLL_PAGE;
        break;

      case TERM_BACK:                                      // back
        infoMenu.cur--;
        break;

      default:
        break;
    }

    if (terminal_page.oldPageIndex != terminal_page.pageIndex)       // Scroll a certain number of pages from the top of the page buffer
    {
      terminal_page.oldPageIndex = terminal_page.pageIndex;

      if (terminal_page.pageTail >= terminal_page.pageIndex)
        pageBufIndex =  terminal_page.pageTail - terminal_page.pageIndex;
      else
        pageBufIndex = terminal_page.pageTail + (MAX_BUFF - terminal_page.pageIndex);

      lastTerminalIndex = terminal_page.ptr[pageBufIndex] - terminalBuf;

      cursorX = CURSOR_START_X;
      cursorY = CURSOR_START_Y;

      GUI_SetBkColor(TERM_BG_COLOR);

      GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y);

      sprintf(pageNum, "%d/%d", abs(((terminal_page.pageTail - terminal_page.pageHead) - terminal_page.pageIndex) + 1), abs((terminal_page.pageTail - terminal_page.pageHead) + 1));

      terminalDrawPage(pageNum);
    }

    getCharacterInfo((u8 *) &terminalBuf[lastTerminalIndex], &info);

    while ((terminalBuf[lastTerminalIndex]) && (lastTerminalIndex != terminalBufTail))
    {
      if (cursorX + info.pixelWidth > CURSOR_END_X || (terminalBuf[lastTerminalIndex] == '\n' && cursorX != CURSOR_START_X))     // Next Line
      {
        cursorX = CURSOR_START_X;
        cursorY += info.pixelHeight;
      }

      if (terminalBuf[lastTerminalIndex] != '\n')
      {
        if (cursorY + info.pixelHeight > CURSOR_END_Y)     // Save the page pointer and scroll to a new screen
        {
          if (terminal_page.pageIndex != 0)
            break;

          terminal_page.pageTail++;

          if (terminal_page.pageTail >= MAX_BUFF)
            terminal_page.pageTail = 0;

          terminal_page.ptr[terminal_page.pageTail] = &terminalBuf[lastTerminalIndex];             // Save character buffer index to page buffer

          if ((buf_full == 1) && (terminal_page.oldPageHead == terminal_page.pageHead))
            terminal_page.pageHead++;                      // Update the bottom of the page buffer to limit the page range

          if (terminal_page.pageHead >= MAX_BUFF)
            terminal_page.pageHead = 0;

          terminal_page.oldPageHead = terminal_page.pageHead;

          cursorX = CURSOR_START_X;
          cursorY = CURSOR_START_Y;

          GUI_SetBkColor(TERM_BG_COLOR);

          GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y);

          sprintf(pageNum, "%d/%d", abs(((terminal_page.pageTail - terminal_page.pageHead) - terminal_page.pageIndex) + 1), abs((terminal_page.pageTail - terminal_page.pageHead) + 1));

          terminalDrawPage(pageNum);
        }

        GUI_SetColor(TERM_FONT_COLOR);
        GUI_SetBkColor(TERM_BG_COLOR);

        GUI_DispOne(cursorX, cursorY, (u8 *) &terminalBuf[lastTerminalIndex]);

        cursorX += info.pixelWidth;
      }

      lastTerminalIndex += info.bytes;

      if (lastTerminalIndex >= TERMINAL_MAX_CHAR)
        lastTerminalIndex = 0;
    }

    loopBackEnd();
  }

  buf_full = 0;
  terminalBufTail = 0;
  terminalBuf[0] = 0;
  terminal_page.pageTail = 0;
  terminal_page.pageHead = 0;
  terminal_page.pageIndex = 0;
  terminal_page.oldPageIndex = 0;

  GUI_RestoreColorDefault();
}
