#include "Terminal.h"
#include "includes.h"

#define TERMINAL_MAX_CHAR (NOBEYOND(600, RAM_SIZE * 45, 4800))
#define MAX_PAGE_COUNT    20

typedef struct
{
  char *   ptr[MAX_PAGE_COUNT];  // Pointer array to terminal pages within terminal buffer
  uint8_t  pageHead;             // index of first page
  uint8_t  pageTail;             // index of last page
  uint8_t  oldPageHead;          // old index of starting page
  uint8_t  pageIndex;            // current page index
  uint8_t  oldPageIndex;         // old page index
  uint16_t terminalBufTail;      // last copy index for copying data in buffer
  uint16_t buffSize;             // maximum buffer/cache size
  uint8_t  maxPageCount;         // maximum page count allowed
  uint8_t  bufferFull;
  uint8_t  lastSrc;
} TERMINAL_DATA;

typedef enum
{
  GKEY_SEND = 0,
  GKEY_ABC_123,
  GKEY_SPACE,
  GKEY_DEL,
  GKEY_BACK,
  // no need to declare key numbers if no special task is performed by the key
  GKEY_IDLE = IDLE_TOUCH,
} GKEY_VALUES;

typedef enum
{
  TERM_PAGE_UP = 0,
  TERM_PAGE_DOWN,
  TERM_TOGGLE_ACK,
  TERM_BACK,
  TERM_KEY_COUNT,  // number of keys
  TERM_IDLE = IDLE_TOUCH,
} TERMINAL_KEY_VALUES;

#ifndef KEYBOARD_MATERIAL_THEME
  #if KEYBOARD_COLOR_LAYOUT == 0
    #define KEY_FONT_COLOR     infoSettings.font_color
    #define KEY_BG_COLOR       infoSettings.bg_color
    #define KEY_BORDER_COLOR   infoSettings.list_border_color
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #elif KEYBOARD_COLOR_LAYOUT == 1
    #define KEY_FONT_COLOR     infoSettings.font_color
    #define KEY_BG_COLOR       infoSettings.list_border_color
    #define KEY_BORDER_COLOR   infoSettings.bg_color
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #else
    #define KEY_FONT_COLOR     BLACK
    #define KEY_BG_COLOR       WHITE
    #define KEY_BORDER_COLOR   0x2174
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #endif

  #define KB_BG_COLOR          BLACK
  #define BAR_FONT_COLOR       WHITE
  #define BAR_BG_COLOR         0x2174
  #define BAR_BORDER_COLOR     0x4b0d
  #define TEXTBOX_FONT_COLOR   BLACK
  #define TEXTBOX_BG_COLOR     0xFFF2
#endif

// keyboard layouts
#define LAYOUT_1_COL_COUNT 6
#define LAYOUT_1_ROW_COUNT 4

#define LAYOUT_2_COL_COUNT 7
#define LAYOUT_2_ROW_COUNT 4

#define LAYOUT_3_COL_COUNT 10
#define LAYOUT_3_ROW_COUNT  6

#if LCD_WIDTH < 480  // number of columns and rows is based on LCD display resolution
  #define KB_TYPE_STANDARD
  #define KB_COL_COUNT LAYOUT_1_COL_COUNT
  #define KB_ROW_COUNT LAYOUT_1_ROW_COUNT
#elif LCD_WIDTH < 800
  #define KB_TYPE_EXTENDED
  #define KB_COL_COUNT LAYOUT_2_COL_COUNT
  #define KB_ROW_COUNT LAYOUT_2_ROW_COUNT
#else
  #define KB_TYPE_QWERTY
  #define KB_COL_COUNT LAYOUT_3_COL_COUNT
  #define KB_ROW_COUNT LAYOUT_3_ROW_COUNT
#endif

#define LAYOUT_QWERTY 0
#define LAYOUT_QWERTZ 1
#define LAYOUT_AZERTY 2

#define GRID_ROW_COUNT (1 + KB_ROW_COUNT + 1)         // text box + keyboard rows + control bar
#define ROW_HEIGHT     (LCD_HEIGHT / GRID_ROW_COUNT)  // button height
#define CTRL_COL_COUNT 4                              // control button count for keyboard view

// keyboard key sizes
#define KEY_WIDTH  (LCD_WIDTH / KB_COL_COUNT + (0.5 * (LCD_WIDTH % KB_COL_COUNT > 0)))
#define KEY_HEIGHT ROW_HEIGHT
#define KEY_COUNT  (1 + (KB_COL_COUNT * KB_ROW_COUNT) + (CTRL_COL_COUNT))  // send + all keys + control bar keys

// control bar sizes
#define CTRL_WIDTH           (LCD_WIDTH / CTRL_COL_COUNT)        // control bar button width in keyboard view
#define TERMINAL_CTRL_WIDTH  (LCD_WIDTH / (TERM_KEY_COUNT + 1))  // control bar button width in terminal view + page textbox
#define CTRL_HEIGHT          ROW_HEIGHT

// value textbox inset padding
#define TEXTBOX_INSET 4

#define COMMAND_START_ROW 0                              // row number for textbox and send button
#define KB_START_ROW      1                              // row number for keyboard
#define CRTL_START_ROW    (KB_START_ROW + KB_ROW_COUNT)  // row number for control bar

// for text in terminal
#define CURSOR_H_OFFSET 2
#define CURSOR_END_Y    ((KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT)

// gcode command draw area inside textbox
const GUI_RECT textBoxRect = {             0 + TEXTBOX_INSET, (COMMAND_START_ROW + 0) * CTRL_HEIGHT + TEXTBOX_INSET,
                              3 * CTRL_WIDTH - TEXTBOX_INSET, (COMMAND_START_ROW + 1) * CTRL_HEIGHT - TEXTBOX_INSET};

// keyboard rectangles
const GUI_RECT editorKeyRect[KEY_COUNT] = {
  // row textbox
  {3 * CTRL_WIDTH, COMMAND_START_ROW * CTRL_HEIGHT, 4 * CTRL_WIDTH, (COMMAND_START_ROW + 1) * CTRL_HEIGHT},  // Send (top row)

  // row control bar
  {0 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 1 * CTRL_WIDTH, LCD_HEIGHT},  // ABC/123
  {1 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 2 * CTRL_WIDTH, LCD_HEIGHT},  // Space
  {2 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 3 * CTRL_WIDTH, LCD_HEIGHT},  // Del
  {3 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 4 * CTRL_WIDTH, LCD_HEIGHT},  // Back

  // row 1
  {0 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {1 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {2 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {3 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {4 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {5 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  #endif

    // row 2
    {0 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
  #endif

    // row 3
    {0 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
  #endif

    // row 4
    {0 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
  #endif

  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    // row 5
    {0 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {6 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {7 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},

    // row 6
    {0 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {6 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {7 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
  #endif
};

// area rectangles
const GUI_RECT editorAreaRect[3] = {
  {0, COMMAND_START_ROW * CTRL_HEIGHT, LCD_WIDTH, ROW_HEIGHT},                // text box + send area
  {0, ROW_HEIGHT,                      LCD_WIDTH, LCD_HEIGHT - CTRL_HEIGHT},  // keyboard area
  {0, CRTL_START_ROW * CTRL_HEIGHT,    LCD_WIDTH, LCD_HEIGHT}                 // control bar area
};

const GUI_RECT terminalKeyRect[TERM_KEY_COUNT] = {
  {1 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 2 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},  // page down
  {2 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 3 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},  // page up
  {3 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 4 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},  // ACK
  {4 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 5 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},  // Back
};

const GUI_RECT terminalPageRect = {
  0 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 1 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT};  // page number

const GUI_RECT terminalAreaRect[2] = {
  {0,            0, LCD_WIDTH, CURSOR_END_Y},  // terminal area
  {0, CURSOR_END_Y, LCD_WIDTH,   LCD_HEIGHT},  // control area
};

// keyboard keys for first layout
const char * const gcodeKey123[KEY_COUNT] = {
  "Send", "ABC", "Space", "Del", "Back",
  #if KB_COL_COUNT == LAYOUT_1_COL_COUNT
    "1", "2", "3", "M", "G", "T",
    "4", "5", "6", "X", "Y", "Z",
    "7", "8", "9", "E", "F", "R",
    ".", "0", "-", "/", "S", "V",
  #elif KB_COL_COUNT == LAYOUT_2_COL_COUNT
    "1", "2", "3", "M", "G", "T", "V",
    "4", "5", "6", "X", "Y", "Z", "S",
    "7", "8", "9", "E", "F", "R", "Q",
    ".", "0", "-", "/", "I", "J", "P",
  #else
    "A", "B", "C", "D", "E", "F", "G", "1", "2", "3",
    "H", "I", "J", "K", "L", "M", "N", "4", "5", "6",
    "O", "P", "Q", "R", "S", "T", "U", "7", "8", "9",
    "V", "W", "X", "Y", "Z", "(", ")", ".", "0", "-",
    "!", "@", "#", "%", "&", ",", ";", "*", "/", "+",
    "~", "`", "$","\\","\"", "'", ":", "_", "=", "?",
  #endif
};

// keyboard keys for second layout
const char * const gcodeKeyABC[KEY_COUNT] = {
  "Send", "123", "Space", "Del", "Back",
  #if KB_COL_COUNT == LAYOUT_1_COL_COUNT
    "A", "B", "C", "D", "H", "I",
    "J", "K", "L", "N", "O", "P",
    ",", ";", ":", "Q", "U", "W",
    "+", "*", "?", "!", "#", "&",
  #elif KB_COL_COUNT == LAYOUT_2_COL_COUNT
    "A", "B", "C", "D", "H", "K", "L",
    ",", ";", ":", "N", "O", "U", "W",
    "+", "*", "?", "!", "#", "&", "$",
    "/", "=", "(", ")", "@", "_", "%",
  #else
    #if TERMINAL_KEYBOARD_LAYOUT == LAYOUT_QWERTY
      "!", "@", "#", "%", "&", "*", "(", ")", "-", "+",
      "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
      "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
      "A", "S", "D", "F", "G", "H", "J", "K", "L", ";",
      "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/",
      "~", "`", "$","\\","\"", "'", ":", "_", "=", "?",
    #elif TERMINAL_KEYBOARD_LAYOUT == LAYOUT_QWERTZ
      "!", "\"", "$", "%", "&", "/", "(", ")", "=", "?",
      "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
      "Q", "W", "E", "R", "T", "Z", "U", "I", "O", "P",
      "A", "S", "D", "F", "G", "H", "J", "K", "L", "@",
      "Y", "X", "C", "V", "B", "N", "M", ",", ".", "-",
      "|", ";", ":", "_", "#", "~", "+", "*", "'", "\\",
    #elif TERMINAL_KEYBOARD_LAYOUT == LAYOUT_AZERTY
      "#", "@", "~", "&", "(", ")", "_", "'", "\"", "%",
      "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
      "A", "Z", "E", "R", "T", "Y", "U", "I", "O", "P",
      "Q", "S", "D", "F", "G", "H", "J", "K", "L", "M",
      "W", "X", "C", "V", "B", "N", ".", ",", ":", ";",
      "-", "+", "*", "\\", "|", "/", "?","!", "$", "=",
    #endif
  #endif
};

const uint16_t fontSrcColor[3][3] = {
  // Gcode                  ACK                    Background
  {COLORSCHEME1_TERM_GCODE, COLORSCHEME1_TERM_ACK, COLORSCHEME1_TERM_BACK},  // Material Dark
  {COLORSCHEME2_TERM_GCODE, COLORSCHEME2_TERM_ACK, COLORSCHEME2_TERM_BACK},  // Material Light
  {COLORSCHEME3_TERM_GCODE, COLORSCHEME3_TERM_ACK, COLORSCHEME3_TERM_BACK},  // High Contrast
};

char * terminalBuf;
TERMINAL_DATA * terminalData;
uint8_t curView = 0;

bool numpad =
  #if defined(KB_TYPE_QWERTY)
    false;  // show qwerty as default for larger
  #else
    true;
  #endif

static inline void keyboardDrawButton(uint8_t index, uint8_t isPressed)
{
  if (index >= COUNT(editorKeyRect))
    return;

  // Setup colors and button info
  #ifdef KEYBOARD_MATERIAL_THEME
    uint16_t fontcolor = KEY_FONT_COLOR;
    uint16_t bgcolor = KEY_BG_COLOR;
    GUI_RECT rectBtn = {editorKeyRect[index].x0 + 3, editorKeyRect[index].y0 + 3,
                        editorKeyRect[index].x1 - 3, editorKeyRect[index].y1 - 3};

    switch (index)
    {
      case GKEY_SEND:
        fontcolor = CTRL_FONT_COLOR;
        bgcolor = MAT_GREEN;
        break;

      case GKEY_BACK:
        fontcolor = CTRL_FONT_COLOR;
        bgcolor = MAT_RED;
        break;

      case GKEY_ABC_123:
        fontcolor = CTRL_FONT_COLOR;
        bgcolor = MAT_SLATE;
        break;

      default:
        break;
    }

    BUTTON btn = {.fontColor  = fontcolor,
                  .backColor  = bgcolor,
                  .context    = (uint8_t *)((numpad) ? gcodeKey123[index] : gcodeKeyABC[index]),
                  .lineColor  = bgcolor,
                  .lineWidth  = 0,
                  .pBackColor = fontcolor,
                  .pFontColor = bgcolor,
                  .pLineColor = fontcolor,
                  .radius     = BTN_ROUND_CORNER,
                  .rect       = rectBtn};

    if (index != GKEY_SEND)
      setFontSize(FONT_SIZE_LARGE);

    // draw button
    GUI_DrawButton(&btn, isPressed);
    setFontSize(FONT_SIZE_NORMAL);
  #else  // KEYBOARD_MATERIAL_THEME
    uint16_t color;
    uint16_t bgColor;

    if (isPressed)
    {
      if (index > GKEY_BACK)
      {
        color = KEY_BG_COLOR;
        bgColor = KEY_FONT_COLOR;
      }
      else
      {
        color = BAR_BG_COLOR;
        bgColor = BAR_FONT_COLOR;
      }
    }
    else
    {
      if (index > GKEY_BACK)
      {
        color = KEY_FONT_COLOR;
        bgColor = KEY_BG_COLOR;
      }
      else
      {
        color = BAR_FONT_COLOR;
        bgColor = BAR_BG_COLOR;
      }
    }

    drawStandardValue(&editorKeyRect[index], VALUE_STRING, (numpad) ? gcodeKey123[index] : gcodeKeyABC[index],
                      (index != GKEY_SEND) ? FONT_SIZE_LARGE : FONT_SIZE_NORMAL, color, bgColor, 1, true);
  #endif  // KEYBOARD_MATERIAL_THEME
}

static inline void drawGcodeText(char * gcode)
{
  drawStandardValue(&textBoxRect, VALUE_STRING, gcode, FONT_SIZE_NORMAL, TEXTBOX_FONT_COLOR, TEXTBOX_BG_COLOR, 1, true);
}

static inline void drawKeyboard(void)
{
  #ifndef KEYBOARD_MATERIAL_THEME
    GUI_SetColor(KEY_BORDER_COLOR);

    // draw vertical button borders
    for (int i = 0; i < (KB_COL_COUNT - 1); i++)
    {
      GUI_VLine(editorKeyRect[i + GKEY_BACK + 1].x1, editorAreaRect[1].y0, editorAreaRect[1].y1);
    }

    // draw horizontal button borders
    for (int i = 0; i < (KB_ROW_COUNT - 1); i++)
    {
      GUI_HLine(editorAreaRect[1].x0, editorKeyRect[(i * KB_COL_COUNT) + GKEY_BACK + 1].y1, editorAreaRect[1].x1);
    }
  #endif

  for (uint8_t i = 0; i < COUNT(gcodeKeyABC); i++)
  {
    keyboardDrawButton(i, false);
  }
}

static inline void keyboardDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(editorKeyRect), editorKeyRect, keyboardDrawButton, NULL);

  // clear keyboard area
  GUI_SetBkColor(KB_BG_COLOR);
  GUI_ClearPrect(&editorAreaRect[1]);

  // clear bar area
  GUI_SetBkColor(BAR_BG_COLOR);
  GUI_ClearPrect(&editorAreaRect[0]);
  GUI_ClearPrect(&editorAreaRect[2]);

  #ifndef KEYBOARD_MATERIAL_THEME
    GUI_SetColor(BAR_BORDER_COLOR);

    // draw text box area shadow border
    GUI_DrawPrect(&textBoxRect);

    // draw bar area shadow border
    GUI_HLine(editorAreaRect[0].x0, editorAreaRect[0].y1 - 1, editorAreaRect[0].x1);  // last row of bar 1 area used for shadow border
    GUI_HLine(editorAreaRect[2].x0, editorAreaRect[2].y0, editorAreaRect[2].x1);      // first row of bar 2 area used for shadow border
  #endif

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  // draw keyboard and gcode value
  drawKeyboard();
  drawGcodeText(NULL);
}

static inline void menuKeyboardView(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  CMD gcodeBuf = {0};
  uint8_t nowIndex = 0;
  uint8_t lastIndex = 0;

  keyboardDrawMenu();

  while (curView == 1)
  {
    if (MENU_IS_NOT(menuTerminal))
      break;

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case GKEY_IDLE:
        break;

      case GKEY_BACK:
        CLOSE_MENU();
        break;

      case GKEY_SEND:
        if (nowIndex)
        {
          gcodeBuf[nowIndex++] = '\n';  // End char '\n' for Gcode
          gcodeBuf[nowIndex] = 0;
          storeCmd(gcodeBuf);
          gcodeBuf[nowIndex = 0] = 0;
        }

        curView = 2;
        break;

      case GKEY_ABC_123:
        numpad = !numpad;
        drawKeyboard();
        break;

      case GKEY_DEL:
        if (nowIndex)
          gcodeBuf[--nowIndex] = 0;
        break;

      case GKEY_SPACE:
        if (nowIndex < CMD_MAX_SIZE - 1 && nowIndex > 0)
        {
          gcodeBuf[nowIndex++] = ' ';
          gcodeBuf[nowIndex] = 0;
        }
        break;

      default:
        if (nowIndex < CMD_MAX_SIZE - 1)
        {
          gcodeBuf[nowIndex++] = (numpad) ? gcodeKey123[key_num][0] : gcodeKeyABC[key_num][0];
          gcodeBuf[nowIndex] = 0;
        }
        break;
    }

    if (lastIndex != nowIndex)
    {
      lastIndex = nowIndex;
      drawGcodeText(gcodeBuf);
    }

    loopBackEnd();
  }

  // restore default
  GUI_RestoreColorDefault();
}

static inline void saveGcodeTerminalCache(const char * str, uint16_t strLen)
{
  uint16_t len = 0;

  if ((terminalData->terminalBufTail + strLen) < terminalData->buffSize)
  {
    memcpy(terminalBuf + terminalData->terminalBufTail, str, strLen);
    terminalData->terminalBufTail += strLen;
  }
  else
  {
    len = (terminalData->terminalBufTail + strLen) - terminalData->buffSize;
    memcpy(terminalBuf + terminalData->terminalBufTail, str, (strLen - len));
    terminalData->terminalBufTail = 0;
    memcpy(terminalBuf + terminalData->terminalBufTail, str + (strLen - len), len);
    terminalData->terminalBufTail += len;
    terminalData->bufferFull = 1;
    terminalData->pageHead++;
  }
}

void terminalCache(const char * stream, uint16_t streamLen, SERIAL_PORT_INDEX portIndex, TERMINAL_SRC src)
{
  if (MENU_IS_NOT(menuTerminal))
    return;

  char * srcID[TERMINAL_COUNT] = {"\5", "\6"};

  // copy string source identifier
  if (terminalData->lastSrc != src)
  {
    saveGcodeTerminalCache(srcID[src], 1);
    terminalData->lastSrc = src;
  }

  if (src == TERMINAL_GCODE)
  {
    if (serialPort[portIndex].id[0] != 0)  // if not empty string
      saveGcodeTerminalCache(serialPort[portIndex].id, 1);  // serial port ID (e.g. "2" for SERIAL_PORT_2)

    saveGcodeTerminalCache(">>", 2);
  }

  saveGcodeTerminalCache(stream, streamLen);
}

// reverse lookup for source identifier
TERMINAL_SRC getLastSrc(char * ptr)
{
  TERMINAL_SRC lastSrc = TERMINAL_GCODE;
  char * endPtr = (ptr + 1);

  // Set end of search pointer
  if (endPtr > (terminalBuf + terminalData->buffSize))
    endPtr = terminalBuf;

  while (ptr != endPtr)
  {
    // check each byte for any source identifier
    if (ptr[0] == 0x5 || ptr[0] == 0x6)
    {
      lastSrc = ptr[0] - 0x5;
      break;
    }

    ptr--;

    // loop to end if reached starting point of buffer
    if (ptr < terminalBuf)
      ptr = terminalBuf + terminalData->buffSize;
  }

  return lastSrc;
}

static inline void terminalDrawButton(uint8_t index, uint8_t isPressed)
{
  if (index >= TERM_KEY_COUNT)
    return;

  const char * terminalKey[] = {"<", ">", (char *)textSelect(itemToggle[infoSettings.terminal_ack].index), "Back"};

  #ifdef KEYBOARD_MATERIAL_THEME
    uint16_t fontcolor = KEY_FONT_COLOR;
    uint16_t bgcolor = KEY_BG_COLOR;
    GUI_RECT rectBtn = {terminalKeyRect[index].x0 + 3, terminalKeyRect[index].y0 + 3,
                        terminalKeyRect[index].x1 - 3, terminalKeyRect[index].y1 - 3};

    if (index == TERM_BACK)
    {
      fontcolor = CTRL_FONT_COLOR;
      bgcolor = MAT_RED;
    }

    BUTTON btn = {.fontColor  = fontcolor,
                  .backColor  = bgcolor,
                  .context    = (uint8_t *)terminalKey[index],
                  .lineColor  = bgcolor,
                  .lineWidth  = 0,
                  .pBackColor = fontcolor,
                  .pFontColor = bgcolor,
                  .pLineColor = fontcolor,
                  .radius     = BTN_ROUND_CORNER,
                  .rect       = rectBtn};

    setFontSize(FONT_SIZE_LARGE);
    GUI_DrawButton(&btn, isPressed);
    setFontSize(FONT_SIZE_NORMAL);
  #else
    uint16_t color;
    uint16_t bgColor;

    if (isPressed)
    {
      color = BAR_BG_COLOR;
      bgColor = BAR_FONT_COLOR;
    }
    else
    {
      color = BAR_FONT_COLOR;
      bgColor = BAR_BG_COLOR;
    }

    drawStandardValue(&terminalKeyRect[index], VALUE_STRING, terminalKey[index], FONT_SIZE_LARGE, color, bgColor, 1, true);
  #endif  // KEYBOARD_MATERIAL_THEME
}

static inline void terminalDrawPageNumber(void)
{
  char tempstr[10];

  sprintf(tempstr, "%d/%d", abs(((terminalData->pageTail - terminalData->pageHead) - terminalData->pageIndex) + 1),
          abs((terminalData->pageTail - terminalData->pageHead) + 1));

  drawStandardValue(&terminalPageRect, VALUE_STRING, &tempstr, FONT_SIZE_LARGE, BAR_FONT_COLOR, BAR_BG_COLOR, 1, true);
}

static inline void terminalDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(terminalKeyRect), terminalKeyRect, terminalDrawButton, NULL);

  // clear terminal area
  GUI_SetBkColor(fontSrcColor[infoSettings.terminal_color_scheme][2]);
  GUI_ClearPrect(&terminalAreaRect[0]);

  // clear bar area
  GUI_SetBkColor(BAR_BG_COLOR);
  GUI_ClearPrect(&terminalAreaRect[1]);

  // draw bar area shadow border
  GUI_SetColor(BAR_BORDER_COLOR);
  GUI_HLine(terminalAreaRect[1].x0, terminalAreaRect[1].y0, terminalAreaRect[1].x1);  // first row of bar area used for shadow border

  // draw keyboard
  for (uint8_t i = 0; i < COUNT(terminalKeyRect); i++)
  {
    terminalDrawButton(i, false);
  }

  terminalDrawPageNumber();
}

void menuTerminalWindow(void)
{
  #define CURSOR_START_X (terminalAreaRect[0].x0 + CURSOR_H_OFFSET)

  KEY_VALUES key_num = KEY_IDLE;
  CHAR_INFO info;
  TERMINAL_SRC pageHeadSrc = TERMINAL_GCODE;
  uint16_t lastTerminalIndex = 0;
  uint8_t pageBufIndex = 0;
  int16_t cursorX = CURSOR_START_X;
  int16_t cursorY = terminalAreaRect[0].y0;

  terminalDrawMenu();

  while (curView == 2)
  {
    if (MENU_IS_NOT(menuTerminal))
      break;

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case TERM_PAGE_UP:  // page up
        if (terminalData->pageIndex < (terminalData->pageTail - terminalData->pageHead))
        {
          terminalData->pageIndex++;
        }
        if ((terminalData->pageTail < terminalData->pageHead)
            && (terminalData->pageIndex < (terminalData->pageTail + terminalData->maxPageCount - terminalData->pageHead)))
        {
          terminalData->pageIndex++;
        }
        break;

      case TERM_PAGE_DOWN:  // page down
        if (terminalData->pageIndex > 0)
          terminalData->pageIndex--;
        break;

      case TERM_TOGGLE_ACK:  // toggle ack in terminal
        infoSettings.terminal_ack = (infoSettings.terminal_ack + 1) % ITEM_TOGGLE_NUM;
        terminalDrawButton(TERM_TOGGLE_ACK, false);
        break;

      case TERM_BACK:  // back
        curView = 1;
        break;

      default:
        break;
    }

    // Scroll a certain number of pages from the top of the page buffer
    if (terminalData->oldPageIndex != terminalData->pageIndex)
    {
      terminalData->oldPageIndex = terminalData->pageIndex;

      if (terminalData->pageTail >= terminalData->pageIndex)
        pageBufIndex = terminalData->pageTail - terminalData->pageIndex;
      else
        pageBufIndex = terminalData->pageTail + (terminalData->maxPageCount - terminalData->pageIndex);

      lastTerminalIndex = terminalData->ptr[pageBufIndex] - terminalBuf;
      cursorX = CURSOR_START_X;
      cursorY = terminalAreaRect[0].y0;

      GUI_SetBkColor(fontSrcColor[infoSettings.terminal_color_scheme][2]);
      GUI_ClearPrect(&terminalAreaRect[0]);

      terminalDrawPageNumber();
      pageHeadSrc = getLastSrc(terminalData->ptr[pageBufIndex]);
    }

    while (terminalBuf + lastTerminalIndex && (lastTerminalIndex != terminalData->terminalBufTail))
    {
      getCharacterInfo((uint8_t *)(terminalBuf + lastTerminalIndex), &info);

      // read source identifier
      if (info.codePoint == 0x5 || info.codePoint == 0x6)
        pageHeadSrc = info.codePoint - 0x5;

      // Next Line
      if (cursorX + info.pixelWidth > terminalAreaRect[0].x1 ||
          (terminalBuf[lastTerminalIndex] == '\n' && cursorX != CURSOR_START_X))
      {
        cursorX = CURSOR_START_X;
        cursorY += info.pixelHeight;
      }

      if (terminalBuf[lastTerminalIndex] != '\n')
      {
        if (cursorY + info.pixelHeight > terminalAreaRect[0].y1)  // Save the page pointer and scroll to a new screen
        {
          if (terminalData->pageIndex != 0)
            break;

          terminalData->pageTail = (terminalData->pageTail + 1) % terminalData->maxPageCount;
          // Save character buffer index to page buffer
          terminalData->ptr[terminalData->pageTail] = terminalBuf + lastTerminalIndex;

          // Update the bottom of the page buffer to limit the page range
          if ((terminalData->bufferFull == 1) && (terminalData->oldPageHead == terminalData->pageHead))
            terminalData->pageHead++;

          if (terminalData->pageHead >= terminalData->maxPageCount)
            terminalData->pageHead = 0;

          terminalData->oldPageHead = terminalData->pageHead;
          cursorX = CURSOR_START_X;
          cursorY = terminalAreaRect[0].y0;

          GUI_SetBkColor(fontSrcColor[infoSettings.terminal_color_scheme][2]);
          GUI_ClearPrect(&terminalAreaRect[0]);

          terminalDrawPageNumber();
        }

        GUI_SetColor(fontSrcColor[infoSettings.terminal_color_scheme][pageHeadSrc]);
        GUI_SetBkColor(fontSrcColor[infoSettings.terminal_color_scheme][2]);

        GUI_DispOne(cursorX, cursorY, &info);
        cursorX += info.pixelWidth;
      }

      lastTerminalIndex += info.bytes;
      if (lastTerminalIndex >= terminalData->buffSize)
        lastTerminalIndex = 0;
    }

    loopBackEnd();
  }

  terminalData->bufferFull = 0;
  terminalData->terminalBufTail = 0;
  terminalBuf[0] = 0;
  terminalData->pageTail = 0;
  terminalData->pageHead = 0;
  terminalData->pageIndex = 0;
  terminalData->oldPageIndex = 0;
  terminalData->lastSrc = (TERMINAL_ACK + 1);

  // restore default
  GUI_RestoreColorDefault();
}

void menuTerminal(void)
{
  TERMINAL_DATA termPage = {{terminalBuf}, 0, 0, 0, 0, 0, 0, TERMINAL_MAX_CHAR, MAX_PAGE_COUNT, 0, (TERMINAL_ACK + 1)};

  if (isPrinting() || infoHost.printing)  // display only 1 page if printing
  {
    termPage.buffSize = (LCD_WIDTH / BYTE_WIDTH * LCD_HEIGHT / BYTE_HEIGHT);
    termPage.maxPageCount = 1;
  }

  char pageBuff[termPage.buffSize];

  terminalBuf = pageBuff;
  terminalData = &termPage;
  curView = 1;

  for (uint8_t i = 0; i < termPage.maxPageCount; i++)
  {
    termPage.ptr[i] = terminalBuf;
  }

  while (MENU_IS(menuTerminal))
  {
    if (curView == 1)
      menuKeyboardView();
    else if (curView == 2)
      menuTerminalWindow();
  }
}
