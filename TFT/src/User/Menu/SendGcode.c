#include "SendGcode.h"
#include "includes.h"

#define SCROLL_LINE  22
#define SCROLL_PAGE  1
char terminalBuf[TERMINAL_MAX_CHAR];
TERMINAL_PAGE terminal_page = {terminalBuf, 0, 0, 0, 0, 0};
static uint16_t terminalBufTail = 0;
uint8_t buf_full = 0;

typedef enum
{
  SOFT_KEY_123 = 0,
  SOFT_KEY_ABC,
}SOFT_KEY_TYPE;

// 26 buttons
const GUI_RECT rect_of_Gkey[]={
  {0*GKEY_WIDTH, 0*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH, 0*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH, 0*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH, 0*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y},
  {4*GKEY_WIDTH, 0*GKEY_HEIGHT+ICON_START_Y, 5*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y},
  {5*GKEY_WIDTH, 0*GKEY_HEIGHT+ICON_START_Y, 6*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y}, // Del

  {0*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y},
  {4*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y, 5*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y},
  {5*GKEY_WIDTH, 1*GKEY_HEIGHT+ICON_START_Y, 6*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y}, // Space

  {0*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y},
  {4*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y, 5*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y},
  {5*GKEY_WIDTH, 2*GKEY_HEIGHT+ICON_START_Y, 6*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y}, // ABC to 123

  {0*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH, 4*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH, 4*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH, 4*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH, 4*GKEY_HEIGHT+ICON_START_Y},
  {4*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y, 5*GKEY_WIDTH, 4*GKEY_HEIGHT+ICON_START_Y},
  {5*GKEY_WIDTH, 3*GKEY_HEIGHT+ICON_START_Y, 6*GKEY_WIDTH, 4*GKEY_HEIGHT+ICON_START_Y},

  // Back
  {0*GKEY_WIDTH+5, 0*GKEY_HEIGHT+5,  1*GKEY_WIDTH-5, ICON_START_Y-5},
  // Send
  {5*GKEY_WIDTH+5, 0*GKEY_HEIGHT+5,  6*GKEY_WIDTH-5, ICON_START_Y-5},
};

const char * const softKeyValue[][26] = {
  {
    "1", "2", "3", "M", "G", "Del",
    "4", "5", "6", "X", "Y", "Space",
    "7", "8", "9", "Z", "E", "ABC",
    ".", "0", "-", "S", "T", ">",
    "Back", "Send",
  },
  {
    "A", "B", "C", "D", "F", "Del",
    "H", "I", "J", "K", "L", "Space",
    "N", "O", "P", "Q", "R", "123",
    "U", "V", "W", " ", " ", " ",
    "Back", "Send",
  }
};

SOFT_KEY_TYPE softKeyType = SOFT_KEY_123;

void sendGcodeReDrawButton(u8 position, u8 pressed)
{
  if(position > GKEY_SEND) return;
  if(pressed)
  {
    GUI_SetColor(WHITE);
    GUI_SetBkColor(BLACK);
  }
  GUI_ClearPrect(rect_of_Gkey+position);
  GUI_DispStringInPrect(rect_of_Gkey + position, (u8 *)softKeyValue[softKeyType][position]);
  if(pressed)
  {
    GUI_SetColor(BLACK);
    GUI_SetBkColor(WHITE);
  }
}

void menuDrawSendGcode(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(rect_of_Gkey), rect_of_Gkey, sendGcodeReDrawButton);

  GUI_RECT gcodeRect = {rect_of_Gkey[GKEY_BACK].x1+10, rect_of_Gkey[GKEY_BACK].y0, rect_of_Gkey[GKEY_SEND].x0-10, rect_of_Gkey[GKEY_SEND].y1};

  GUI_SetBkColor(BLUE);
  GUI_ClearRect(0, 0, LCD_WIDTH, rect_of_Gkey[0].y0);
  GUI_SetBkColor(WHITE);
  GUI_ClearPrect(&gcodeRect);
  GUI_ClearRect(0, rect_of_Gkey[0].y0, LCD_WIDTH, LCD_HEIGHT);

  GUI_SetColor(YELLOW);
  GUI_FillPrect(&rect_of_Gkey[GKEY_BACK]); // Back
  GUI_FillPrect(&rect_of_Gkey[GKEY_SEND]); // Send
  GUI_SetColor(BLACK);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  for (uint8_t i = 0; i < COUNT(softKeyValue[0]); i++)
  {
//    GUI_DrawPrect(rect_of_Gkey + i);  // Draw keyboard border
    GUI_DispStringInPrect(rect_of_Gkey + i, (u8 *)softKeyValue[softKeyType][i]);
  }
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void menuSendGcode(void)
{
  GUI_RECT gcodeRect = {rect_of_Gkey[GKEY_BACK].x1+10, rect_of_Gkey[GKEY_BACK].y0, rect_of_Gkey[GKEY_SEND].x0-10, rect_of_Gkey[GKEY_SEND].y1};

  char gcodeBuf[CMD_MAX_CHAR] = {0};
  uint8_t nowIndex = 0,
          lastIndex = 0;
  GKEY_VALUES key_num = GKEY_IDLE;
  menuDrawSendGcode();
  while(infoMenu.menu[infoMenu.cur] == menuSendGcode)
  {
    key_num = menuKeyGetValue();

    switch(key_num)
    {
      case GKEY_IDLE:
        break;

      case GKEY_BACK:
        infoMenu.cur--;
        break;

      case GKEY_SEND:
        if (nowIndex)
        {
          gcodeBuf[nowIndex++] = '\n'; // End char '\n' for Gcode
          gcodeBuf[nowIndex] = 0;
          storeCmd(gcodeBuf);
          gcodeBuf[nowIndex = 0] = 0;
        }
        infoMenu.menu[++infoMenu.cur] = menuTerminal;
        break;

      case GKEY_ABC_123:
        softKeyType = (softKeyType == SOFT_KEY_123) ? SOFT_KEY_ABC : SOFT_KEY_123;
        for (uint8_t i = 0; i < GKEY_BACK; i++)
        {
          GUI_DispStringInPrect(rect_of_Gkey + i, (u8 *)softKeyValue[softKeyType][i]);
        }
        break;

      case GKEY_DEL:
        if(nowIndex)
          gcodeBuf[--nowIndex] = 0;
        break;

      case GKEY_SPACE:
        if(nowIndex < CMD_MAX_CHAR - 1)
        {
          gcodeBuf[nowIndex++] = ' ';
          gcodeBuf[nowIndex] = 0;
        }
        break;

      default:
        if(nowIndex < CMD_MAX_CHAR - 1)
        {
          gcodeBuf[nowIndex++] = softKeyValue[softKeyType][key_num][0];
          gcodeBuf[nowIndex] = 0;
        }
        break;
    }
    if(lastIndex != nowIndex)
    {
      lastIndex = nowIndex;
      GUI_ClearPrect(&gcodeRect);
      GUI_DispStringInPrect(&gcodeRect, (u8 *)gcodeBuf);
    }

    loopBackEnd();
  }

  GUI_RestoreColorDefault();
}

void saveGcodeTerminalCache(uint16_t sign_len, char *str)
{
  uint16_t len = 0;
  if((terminalBufTail+sign_len)<TERMINAL_MAX_CHAR)
  {
    memcpy(terminalBuf+terminalBufTail, str, sign_len);
    terminalBufTail += sign_len;
  }
  else
  {
    len = (terminalBufTail+sign_len) - TERMINAL_MAX_CHAR;
    memcpy(terminalBuf+terminalBufTail, str, (sign_len-len));
    terminalBufTail = 0;
    memcpy(terminalBuf+terminalBufTail, str + (sign_len-len), len);
    terminalBufTail += len;
    buf_full = 1;
    terminal_page.pageHead++;
  }
}

void sendGcodeTerminalCache(char *stream, TERMINAL_SRC src)
{
  uint16_t sign_len=0;
  uint16_t stream_len=0;
  const char* const terminalSign[] = {"Send: ", "Rcv: "};
  if (infoMenu.menu[infoMenu.cur] != menuSendGcode && infoMenu.menu[infoMenu.cur] != menuTerminal) return;
  sign_len = strlen(terminalSign[src]);
  saveGcodeTerminalCache(sign_len,(char *)terminalSign[src]);
  stream_len = strlen(stream);
  saveGcodeTerminalCache(stream_len,stream);
}

#define CURSOR_START_X 0
#define CURSOR_END_X   LCD_WIDTH
#define CURSOR_START_Y 0
#define CURSOR_END_Y   (LCD_HEIGHT - TITLE_END_Y)

const GUI_RECT terminalRect[] = {
  {0, CURSOR_END_Y, LCD_WIDTH / 3, LCD_HEIGHT},
  {LCD_WIDTH / 3, CURSOR_END_Y,  2 * LCD_WIDTH / 3, LCD_HEIGHT},
  {2 * LCD_WIDTH / 3, CURSOR_END_Y, LCD_WIDTH, LCD_HEIGHT},
};

const char *terminalString[] = {"<", ">", "Back"};

void terminalReDrawButton(u8 position, u8 pressed)
{
  if(position > COUNT(terminalString)) return;
  if(pressed)
  {
    GUI_SetColor(BLACK);
    GUI_SetBkColor(WHITE);
  }
  else
  {
    GUI_SetColor(WHITE);
    GUI_SetBkColor(BLACK);
  }

  GUI_ClearPrect(terminalRect + position);
  GUI_DispStringInPrect(terminalRect + position, (const u8 *)terminalString[position]);

  GUI_SetColor(BLACK);
  GUI_SetBkColor(GRAY);
}

void menuTerminal(void)
{
  CHAR_INFO info;
  int16_t cursorX = CURSOR_START_X,
          cursorY = CURSOR_START_Y;
  uint16_t lastTerminalIndex = 0;
  uint16_t key_num = IDLE_TOUCH;
  uint8_t pageBufIndex = 0;

  GUI_ClearRect(0, CURSOR_END_Y, LCD_WIDTH, LCD_HEIGHT);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  for (uint8_t i = 0; i < COUNT(terminalString); i++)
  {
    GUI_DispStringInPrect(terminalRect + i, (const u8 *)terminalString[i]);
  }
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

  GUI_SetColor(BLACK);
  GUI_SetBkColor(GRAY);
  GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y);

  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(terminalRect), terminalRect, terminalReDrawButton);

  while(infoMenu.menu[infoMenu.cur] == menuTerminal)
  {
    key_num = KEY_GetValue(3, terminalRect);
    switch (key_num)
    {
      case GKEY_0:              //page up
        if(terminal_page.pageIndex < (terminal_page.pageTail - terminal_page.pageHead))
          terminal_page.pageIndex += SCROLL_PAGE;
        if((terminal_page.pageTail < terminal_page.pageHead) &&
          (terminal_page.pageIndex < (terminal_page.pageTail + MAX_BUFF - terminal_page.pageHead)))
          terminal_page.pageIndex += SCROLL_PAGE;
        break;

      case GKEY_1:             //page down
        if(terminal_page.pageIndex > 0)
          terminal_page.pageIndex -= SCROLL_PAGE;
        break;
      case GKEY_2:             //back
        infoMenu.cur--;
      default:
        break;
    }

    if(terminal_page.oldPageIndex != terminal_page.pageIndex)          //Scroll a certain number of pages from the top of the page buffer
    {
      terminal_page.oldPageIndex = terminal_page.pageIndex;
      cursorX = CURSOR_START_X;
      cursorY = CURSOR_START_Y;
      GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X-1, CURSOR_END_Y);
      if(terminal_page.pageTail >= terminal_page.pageIndex) pageBufIndex =  terminal_page.pageTail - terminal_page.pageIndex;
      else pageBufIndex = terminal_page.pageTail + (MAX_BUFF - terminal_page.pageIndex);
      lastTerminalIndex = terminal_page.ptr[pageBufIndex] - terminalBuf;
    }
    getCharacterInfo((u8 *)&terminalBuf[lastTerminalIndex], &info);
    while ((terminalBuf[lastTerminalIndex]) && (lastTerminalIndex!=terminalBufTail))
    {
      if (cursorX + info.pixelWidth > CURSOR_END_X || (terminalBuf[lastTerminalIndex] == '\n' && cursorX != CURSOR_START_X)) // Next Line
      {
        cursorX = CURSOR_START_X;
        cursorY += info.pixelHeight;
      }
      if(terminalBuf[lastTerminalIndex] != '\n')
      {
        if (cursorY + info.pixelHeight > CURSOR_END_Y) // Save the page pointer and scroll to a new screen
        {
          if(terminal_page.pageIndex != 0) break;

          terminal_page.pageTail++;
          if(terminal_page.pageTail >= MAX_BUFF) terminal_page.pageTail=0;
          terminal_page.ptr[terminal_page.pageTail] = &terminalBuf[lastTerminalIndex];           //Save character buffer index to page buffer

          if((buf_full==1)&&(terminal_page.oldPageHead==terminal_page.pageHead)) terminal_page.pageHead++;  //Update the bottom of the page buffer to limit the page range
          if(terminal_page.pageHead >= MAX_BUFF) terminal_page.pageHead=0;
          terminal_page.oldPageHead = terminal_page.pageHead;

          cursorX = CURSOR_START_X;
          cursorY = CURSOR_START_Y;
          GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X, CURSOR_END_Y);
        }
        GUI_SetColor(BLACK);
        GUI_SetBkColor(GRAY);
        GUI_DispOne(cursorX, cursorY, (u8 *)&terminalBuf[lastTerminalIndex]);
        cursorX += info.pixelWidth;
      }
      lastTerminalIndex += info.bytes;
      if(lastTerminalIndex>=TERMINAL_MAX_CHAR) lastTerminalIndex=0;
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
