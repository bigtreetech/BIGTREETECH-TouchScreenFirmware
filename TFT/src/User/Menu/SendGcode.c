#include "SendGcode.h"
#include "includes.h"

#define TERMINAL_MAX_CHAR ((LCD_WIDTH / BYTE_WIDTH) * (LCD_HEIGHT / BYTE_HEIGHT) * 4)
#define MAX_BUFF          20
#define SCROLL_LINE       22
#define SCROLL_PAGE       1

#define BAR_BK_COLOR     0x2174
#define BAR_INNER_COLOR  0xFFF2
#define BAR_BORDER_COLOR 0x4b0d

typedef struct
{
  char *ptr[MAX_BUFF];                 //Pointer into the terminal page buffer, full Screen is one page
  uint8_t pageHead;                    //Buffer top of page
  uint8_t pageTail;                    //Buffer buttom of page
  uint8_t oldPageHead;
  uint8_t pageIndex;                   //page buffer index
  uint8_t oldPageIndex;
} TERMINAL_PAGE;

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
  GKEY_ABC_123,
  GKEY_SPACE,
  GKEY_DEL,
  GKEY_BACK,
  GKEY_SEND,
  GKEY_KEY_NUM,                       // number of keys
  GKEY_IDLE = IDLE_TOUCH,
} GKEY_VALUES;

typedef enum
{
  SOFT_KEY_123 = 0,
  SOFT_KEY_ABC,
} SOFT_KEY_TYPE;

/*about GCODE KEY*/
#define GKEY_WIDTH  (LCD_WIDTH / 6)
#define GKEY_HEIGHT ((LCD_HEIGHT - ICON_START_Y) / 5)

#define GCODE_ROW_NUM 1
#define GCODE_COL_NUM 4
#define GCODE_HEIGHT  (ICON_START_Y / GCODE_ROW_NUM)
#define GCODE_WIDTH   (LCD_WIDTH / GCODE_COL_NUM)
#define GCODE_X0      0
#define GCODE_Y0      0

#define KEY_ROW_NUM 1
#define KEY_COL_NUM 6
#define KEY_HEIGHT  GKEY_HEIGHT
#define KEY_WIDTH   GKEY_WIDTH
#define KEY_X0      0
#define KEY_Y0      ICON_START_Y

#define CTRL_ROW_NUM 1
#define CTRL_COL_NUM 4
#define CTRL_HEIGHT  GKEY_HEIGHT
#define CTRL_WIDTH   (LCD_WIDTH / CTRL_COL_NUM)
#define CTRL_X0      0
#define CTRL_Y0      (LCD_HEIGHT - GKEY_HEIGHT)

const GUI_RECT gcodeValueRect = {
  GCODE_X0 + 0 * GCODE_WIDTH + 4, GCODE_Y0 + 0 * GCODE_HEIGHT + 4, GCODE_X0 + 3 * GCODE_WIDTH - 4, GCODE_Y0 + 1 * GCODE_HEIGHT - 4
};

const GUI_RECT gcodeKeyRect[GKEY_KEY_NUM]={
  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 0 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT},

  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 1 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT},

  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 2 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT},

  {KEY_X0 + 0 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 1 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 2 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 3 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 4 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},
  {KEY_X0 + 5 * KEY_WIDTH, KEY_Y0 + 3 * KEY_HEIGHT, KEY_X0 + 6 * KEY_WIDTH, KEY_Y0 + 4 * KEY_HEIGHT},

  {CTRL_X0 + 0 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 1 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // ABC or 123
  {CTRL_X0 + 1 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 2 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Space
  {CTRL_X0 + 2 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 3 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Del
  {CTRL_X0 + 3 * CTRL_WIDTH, CTRL_Y0 + 0 * CTRL_HEIGHT, CTRL_X0 + 4 * CTRL_WIDTH, CTRL_Y0 + 1 * CTRL_HEIGHT},          // Back

  {GCODE_X0 + 3 * GCODE_WIDTH, GCODE_Y0 + 0 * GCODE_HEIGHT, LCD_WIDTH, GCODE_Y0 + 1 * GCODE_HEIGHT},                   // Send
};

const char * const gcodeKey[][GKEY_KEY_NUM] = {
  {
    "1", "2", "3", "M", "G", "T",
    "4", "5", "6", "X", "Y", "Z",
    "7", "8", "9", "E", "F", "R",
    ".", "0", "-", "~", "S", "V",
    "ABC", "Space", "Del", "Back",
    "Send",
  },
  {
    "A", "B", "C", "D", "H", "I",
    "J", "K", "L", "N", "O", "P",
    "Q", "U", "W", ",", ";", ":",
    "+", "*", "!", "%", "&", "#",
    "123", "Space", "Del", "Back",
    "Send",
  }
};

const GUI_RECT gcodeAreaRect[3] = {
  {GCODE_X0, GCODE_Y0, LCD_WIDTH, ICON_START_Y},                               // gcode area
  {KEY_X0, KEY_Y0, LCD_WIDTH, LCD_HEIGHT - GKEY_HEIGHT},                       // keyboard area
  {CTRL_X0, CTRL_Y0, LCD_WIDTH, LCD_HEIGHT},                                   // control area
};

typedef enum
{
  TERM_PAGE_UP = 0,
  TERM_PAGE_DOWN,
  TERM_BACK,
  TERM_KEY_NUM,                        // number of keys
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
  {CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y},                // cursor area
  {PAGE_X0, PAGE_Y0, LCD_WIDTH, LCD_HEIGHT},                                   // control area
};

char terminalBuf[TERMINAL_MAX_CHAR];
TERMINAL_PAGE terminal_page = {terminalBuf, 0, 0, 0, 0, 0};
static uint16_t terminalBufTail = 0;
uint8_t buf_full = 0;
SOFT_KEY_TYPE gcodeKeyType = SOFT_KEY_123;

void sendGcodeReDrawButton(u8 position, u8 pressed)
{
  if (position > GKEY_SEND)
    return;

  if (pressed)
  {
    if (position < 24)
    {
      GUI_SetColor(infoSettings.font_color);
      GUI_SetBkColor(infoSettings.bg_color);
    }
    else
    {
      GUI_SetColor(infoSettings.bg_color);
      GUI_SetBkColor(infoSettings.font_color);
    }
  }
  else
  {
    if (position < 24)
    {
      GUI_SetColor(infoSettings.bg_color);
      GUI_SetBkColor(infoSettings.font_color);
    }
    else
    {
      GUI_SetColor(infoSettings.font_color);
      GUI_SetBkColor(BAR_BK_COLOR);
    }
  }

  if (position != GKEY_SEND)
    setLargeFont(true);

  GUI_ClearRect(gcodeKeyRect[position].x0 + 2, gcodeKeyRect[position].y0 + 2, gcodeKeyRect[position].x1 - 1, gcodeKeyRect[position].y1 - 1);
  GUI_DispStringInRect(gcodeKeyRect[position].x0 + 2, gcodeKeyRect[position].y0 + 2, gcodeKeyRect[position].x1 - 1, gcodeKeyRect[position].y1 - 1, (u8 *) gcodeKey[gcodeKeyType][position]);

  setLargeFont(false);
}

void sendGcodeDrawGcode(char *gcode)
{
  GUI_SetColor(infoSettings.bg_color);
  GUI_SetBkColor(BAR_INNER_COLOR);

  GUI_ClearRect(gcodeValueRect.x0 + 1, gcodeValueRect.y0 + 1, gcodeValueRect.x1 - 1, gcodeValueRect.y1 - 1);

  if (gcode != NULL)
    GUI_DispStringInRect(gcodeValueRect.x0 + 1, gcodeValueRect.y0 + 1, gcodeValueRect.x1 - 1, gcodeValueRect.y1 - 1, (u8 *) gcode);
}

void sendGcodeDrawKeyboard(void)
{
  GUI_SetColor(BAR_BK_COLOR);

  // draw vertical button borders
  for (int i = 0; i < 5; i++)
  {
    GUI_DrawLine(gcodeKeyRect[i].x1, gcodeKeyRect[i].y0, gcodeKeyRect[18 + i].x1, gcodeKeyRect[18 + i].y1);
  }

  // draw horizontal button borders
  for (int i = 0; i < 3; i++)
  {
    GUI_DrawLine(gcodeKeyRect[i * 6].x0, gcodeKeyRect[i * 6].y1, gcodeKeyRect[5 + i * 6].x1, gcodeKeyRect[5 + i * 6].y1);
  }

  GUI_SetColor(BAR_BORDER_COLOR);

  GUI_DrawLine(gcodeAreaRect[1].x0, gcodeAreaRect[1].y0, gcodeAreaRect[1].x1, gcodeAreaRect[1].y0);
  GUI_DrawLine(gcodeAreaRect[1].x0, gcodeAreaRect[1].y1, gcodeAreaRect[1].x1, gcodeAreaRect[1].y1);

  for (uint8_t i = 0; i < COUNT(gcodeKey[0]); i++)
  {
    sendGcodeReDrawButton(i, false);
  }
}

void sendGcodeDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(gcodeKeyRect), gcodeKeyRect, sendGcodeReDrawButton);

  GUI_SetBkColor(BAR_BK_COLOR);

  GUI_ClearRect(gcodeAreaRect[0].x0, gcodeAreaRect[0].y0, gcodeAreaRect[0].x1, gcodeAreaRect[0].y1);
  GUI_ClearRect(gcodeAreaRect[2].x0, gcodeAreaRect[2].y0, gcodeAreaRect[2].x1, gcodeAreaRect[2].y1);

  GUI_SetBkColor(infoSettings.font_color);

  GUI_ClearRect(gcodeAreaRect[1].x0, gcodeAreaRect[1].y0, gcodeAreaRect[1].x1, gcodeAreaRect[1].y1);

  GUI_SetColor(BAR_BORDER_COLOR);

  GUI_DrawPrect(&gcodeValueRect);                          // draw gcode value area borders

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

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

      case GKEY_ABC_123:
        gcodeKeyType = (gcodeKeyType == SOFT_KEY_123) ? SOFT_KEY_ABC : SOFT_KEY_123;

        sendGcodeDrawKeyboard();
        break;

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
          gcodeBuf[nowIndex++] = gcodeKey[gcodeKeyType][key_num][0];
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

void terminalReDrawButton(u8 position, u8 pressed)
{
  if (position > COUNT(terminalKey))
    return;

  if (pressed)
  {
    GUI_SetColor(infoSettings.bg_color);
    GUI_SetBkColor(infoSettings.font_color);
  }
  else
  {
    GUI_SetColor(infoSettings.font_color);
    GUI_SetBkColor(BAR_BK_COLOR);
  }

  GUI_ClearRect(terminalKeyRect[position].x0 + 2, terminalKeyRect[position].y0 + 2, terminalKeyRect[position].x1 - 1, terminalKeyRect[position].y1 - 1);
  GUI_DispStringInRect(terminalKeyRect[position].x0 + 2, terminalKeyRect[position].y0 + 2, terminalKeyRect[position].x1 - 1, terminalKeyRect[position].y1 - 1, (u8 *) terminalKey[position]);
}

void terminalDrawPage(char *pageNum)
{
  GUI_SetColor(infoSettings.font_color);
  GUI_SetBkColor(BAR_BK_COLOR);

  GUI_ClearRect(terminalPageRect.x0 + 1, terminalPageRect.y0 + 1, terminalPageRect.x1 - 1, terminalPageRect.y1 - 1);

  if (pageNum != NULL)
    GUI_DispStringInRect(terminalPageRect.x0 + 1, terminalPageRect.y0 + 1, terminalPageRect.x1 - 1, terminalPageRect.y1 - 1, (u8 *) pageNum);
}

void terminalDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(terminalKeyRect), terminalKeyRect, terminalReDrawButton);

  GUI_SetBkColor(infoSettings.marlin_mode_bg_color);

  GUI_ClearRect(terminalAreaRect[0].x0, terminalAreaRect[0].y0, terminalAreaRect[0].x1, terminalAreaRect[0].y1);

  GUI_SetBkColor(BAR_BK_COLOR);

  GUI_ClearRect(terminalAreaRect[1].x0, terminalAreaRect[1].y0, terminalAreaRect[1].x1, terminalAreaRect[1].y1);

  GUI_SetColor(BAR_BORDER_COLOR);

  GUI_DrawLine(terminalAreaRect[1].x0, terminalAreaRect[1].y0, terminalAreaRect[1].x1, terminalAreaRect[1].y0);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

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
      case TERM_PAGE_UP:                                   //page up
        if (terminal_page.pageIndex < (terminal_page.pageTail - terminal_page.pageHead))
          terminal_page.pageIndex += SCROLL_PAGE;

        if ((terminal_page.pageTail < terminal_page.pageHead) &&
          (terminal_page.pageIndex < (terminal_page.pageTail + MAX_BUFF - terminal_page.pageHead)))
          terminal_page.pageIndex += SCROLL_PAGE;
        break;

      case TERM_PAGE_DOWN:                                 //page down
        if (terminal_page.pageIndex > 0)
          terminal_page.pageIndex -= SCROLL_PAGE;
        break;

      case TERM_BACK:                                      //back
        infoMenu.cur--;
        break;

      default:
        break;
    }

    if (terminal_page.oldPageIndex != terminal_page.pageIndex)       //Scroll a certain number of pages from the top of the page buffer
    {
      terminal_page.oldPageIndex = terminal_page.pageIndex;

      if (terminal_page.pageTail >= terminal_page.pageIndex)
        pageBufIndex =  terminal_page.pageTail - terminal_page.pageIndex;
      else
        pageBufIndex = terminal_page.pageTail + (MAX_BUFF - terminal_page.pageIndex);

      lastTerminalIndex = terminal_page.ptr[pageBufIndex] - terminalBuf;

      cursorX = CURSOR_START_X;
      cursorY = CURSOR_START_Y;

      GUI_SetBkColor(infoSettings.marlin_mode_bg_color);

      GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y);

      sprintf(pageNum, "%d/%d", ((terminal_page.pageTail - terminal_page.pageHead) - terminal_page.pageIndex) + 1, (terminal_page.pageTail - terminal_page.pageHead) + 1);

      terminalDrawPage(pageNum);
    }

    getCharacterInfo((u8 *) &terminalBuf[lastTerminalIndex], &info);

    while ((terminalBuf[lastTerminalIndex]) && (lastTerminalIndex != terminalBufTail))
    {
      if (cursorX + info.pixelWidth > CURSOR_END_X || (terminalBuf[lastTerminalIndex] == '\n' && cursorX != CURSOR_START_X)) // Next Line
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

          terminal_page.ptr[terminal_page.pageTail] = &terminalBuf[lastTerminalIndex];             //Save character buffer index to page buffer

          if ((buf_full == 1) && (terminal_page.oldPageHead == terminal_page.pageHead))
            terminal_page.pageHead++;                      //Update the bottom of the page buffer to limit the page range

          if (terminal_page.pageHead >= MAX_BUFF)
            terminal_page.pageHead = 0;

          terminal_page.oldPageHead = terminal_page.pageHead;

          cursorX = CURSOR_START_X;
          cursorY = CURSOR_START_Y;

          GUI_SetBkColor(infoSettings.marlin_mode_bg_color);

          GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y);

          sprintf(pageNum, "%d/%d", ((terminal_page.pageTail - terminal_page.pageHead) - terminal_page.pageIndex) + 1, (terminal_page.pageTail - terminal_page.pageHead) + 1);

          terminalDrawPage(pageNum);
        }

        GUI_SetColor(infoSettings.marlin_mode_font_color);
        GUI_SetBkColor(infoSettings.marlin_mode_bg_color);

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
