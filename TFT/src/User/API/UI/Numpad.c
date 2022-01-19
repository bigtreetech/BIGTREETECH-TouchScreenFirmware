#include "Numpad.h"
#include "includes.h"
#include "CharIcon.h"

#define SKEYHEIGHT (LCD_HEIGHT - ICON_START_Y) / 4
#define SKEYWIDTH  LCD_WIDTH / 4

#define KEY_COUNT        16
#define FLOAT_BUF_LENGTH  9
#define INT_BUF_LENGTH    6

typedef enum
{
  NUM_KEY_1 = 0,
  NUM_KEY_2,
  NUM_KEY_3,
  NUM_KEY_OK,
  NUM_KEY_4,
  NUM_KEY_5,
  NUM_KEY_6,
  NUM_KEY_DEL,
  NUM_KEY_7,
  NUM_KEY_8,
  NUM_KEY_9,
  NUM_KEY_EXIT,
  NUM_KEY_DEC,
  NUM_KEY_0,
  NUM_KEY_MINUS,
  NUM_KEY_RESET,

  NUM_KEY_IDLE = IDLE_TOUCH,
} NUM_KEY_VALUES;

const GUI_RECT rect_of_numkey[KEY_COUNT] = {
  {0 * SKEYWIDTH, ICON_START_Y + 0 * SKEYHEIGHT, 1 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT},  // 1
  {1 * SKEYWIDTH, ICON_START_Y + 0 * SKEYHEIGHT, 2 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT},  // 2
  {2 * SKEYWIDTH, ICON_START_Y + 0 * SKEYHEIGHT, 3 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT},  // 3
  {3 * SKEYWIDTH, ICON_START_Y + 0 * SKEYHEIGHT, 4 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT},  // OK

  {0 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT, 1 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT},  // 4
  {1 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT, 2 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT},  // 5
  {2 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT, 3 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT},  // 6
  {3 * SKEYWIDTH, ICON_START_Y + 1 * SKEYHEIGHT, 4 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT},  // Del

  {0 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT, 1 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT},  // 7
  {1 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT, 2 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT},  // 8
  {2 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT, 3 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT},  // 9
  {3 * SKEYWIDTH, ICON_START_Y + 2 * SKEYHEIGHT, 4 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT},  // Cancel

  {0 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT, 1 * SKEYWIDTH, ICON_START_Y + 4 * SKEYHEIGHT},  // .
  {1 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT, 2 * SKEYWIDTH, ICON_START_Y + 4 * SKEYHEIGHT},  // 0
  {2 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT, 3 * SKEYWIDTH, ICON_START_Y + 4 * SKEYHEIGHT},  // -
  {3 * SKEYWIDTH, ICON_START_Y + 3 * SKEYHEIGHT, 4 * SKEYWIDTH, ICON_START_Y + 4 * SKEYHEIGHT},  // Undo/Reset
};

const GUI_RECT oldParameterRect = {0,                          0, LCD_WIDTH / 2 - BYTE_WIDTH,  ICON_START_Y + 0 * SKEYHEIGHT};
const GUI_RECT newParameterRect = {LCD_WIDTH / 2 + BYTE_WIDTH, 0,                  LCD_WIDTH,  ICON_START_Y + 0 * SKEYHEIGHT};
const GUI_RECT arrowRect        = {LCD_WIDTH / 2 - BYTE_WIDTH, 0, LCD_WIDTH / 2 + BYTE_WIDTH,  ICON_START_Y + 0 * SKEYHEIGHT};

const char * const numPadKeyChar[KEY_COUNT] = {
  "1", "2", "3", "\u0894",
  "4", "5", "6", "\u0899",
  "7", "8", "9", "\u0895",
  ".", "0", "-", "\u08A5"
};

uint8_t numpadType = 0;  // numpad type identifier

void drawKeypadButton(uint8_t index, uint8_t isPressed)
{
  if (index >= COUNT(rect_of_numkey)) return;

  #ifdef KEYBOARD_MATERIAL_THEME
    uint16_t fontcolor = CTRL_FONT_COLOR;
    uint16_t bgcolor = KEY_BG_COLOR;
    GUI_RECT rectBtn = {rect_of_numkey[index].x0 + 4, rect_of_numkey[index].y0 + 4,
                        rect_of_numkey[index].x1 - 4, rect_of_numkey[index].y1 - 4};

    switch (index)
    {
      case NUM_KEY_OK:
        bgcolor = CTRL_SEND_BG_COLOR;
        break;

      case NUM_KEY_DEL:
        bgcolor = CTRL_DEL_BG_COLOR;
        break;

      case NUM_KEY_EXIT:
        bgcolor = CTRL_BACK_BG_COLOR;
        break;

      case NUM_KEY_RESET:
        bgcolor = CTRL_ABC_BG_COLOR;
        break;

      default:
        fontcolor = KEY_FONT_COLOR;
        break;
    }

    BUTTON btn = {.fontColor  = fontcolor,
                  .backColor  = bgcolor,
                  .context    = (uint8_t *)numPadKeyChar[index],
                  .lineColor  = bgcolor,
                  .lineWidth  = 0,
                  .pBackColor = fontcolor,
                  .pFontColor = bgcolor,
                  .pLineColor = fontcolor,
                  .radius     = 13,
                  .rect       = rectBtn};

    setFontSize(FONT_SIZE_LARGE);
    if (!(index == NUM_KEY_DEC && GET_BIT(numpadType, 0)) && !(index == NUM_KEY_MINUS && !GET_BIT(numpadType, 1)))
      GUI_DrawButton(&btn, isPressed);
    setFontSize(FONT_SIZE_NORMAL);
  #else
    if (!isPressed)
      GUI_SetColor(infoSettings.bg_color);
    GUI_DrawRect(rect_of_numkey[index].x0 + 2, rect_of_numkey[index].y0 + 2, rect_of_numkey[index].x1-2, rect_of_numkey[index].y1 - 2);
    GUI_SetColor(infoSettings.font_color);
  #endif  // KEYBOARD_MATERIAL_THEME
}

void Draw_keyboard(uint8_t * title, bool numberOnly, bool negative)
{
  numpadType = (negative << 1) | (numberOnly << 0);  // numpad type identfier
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  #ifdef KEYBOARD_MATERIAL_THEME
    GUI_SetBkColor(BAR_BG_COLOR);
    GUI_ClearRect(0, 0, LCD_WIDTH, rect_of_numkey[0].y0);

    GUI_SetBkColor(KB_BG_COLOR);
    GUI_ClearRect(0, rect_of_numkey[0].y0, LCD_WIDTH, LCD_HEIGHT);

    GUI_SetColor(BAR_BORDER_COLOR);
    // draw value display border line
    GUI_HLine(rect_of_numkey[0].x0,rect_of_numkey[0].y0,rect_of_numkey[3].x1);

    for (uint8_t i = 0; i < KEY_COUNT; i++)
    {
      drawKeypadButton(i, false);
    }
    GUI_SetColor(BAR_FONT_COLOR);
  #else
    GUI_ClearRect(0, 0, LCD_WIDTH, rect_of_numkey[0].y0);
    GUI_ClearRect(0, rect_of_numkey[0].y0, LCD_WIDTH, LCD_HEIGHT);

    // draw button borders
    GUI_SetColor(infoSettings.list_border_color);
    for (int i = 0; i < 3; i++)
    {
      GUI_VLine(rect_of_numkey[i].x1, rect_of_numkey[i].y0, rect_of_numkey[12+i].y1);
      GUI_HLine(rect_of_numkey[i*4].x0, rect_of_numkey[i*4].y1, rect_of_numkey[3+i*4].x1);
    }

    // draw value display border line
    GUI_SetColor(infoSettings.font_color);
    GUI_HLine(rect_of_numkey[0].x0,rect_of_numkey[0].y0,rect_of_numkey[3].x1);

    setFontSize(FONT_SIZE_LARGE);

    for (uint8_t i = 0; i < KEY_COUNT ;i++)
    {
      if (!(i == NUM_KEY_DEC || i == NUM_KEY_MINUS || (i % 4) == 3))  // || i == NUM_KEY_DEL || i == NUM_KEY_EXIT || i == NUM_KEY_RESET))
        GUI_DispStringInPrect(&rect_of_numkey[i], (uint8_t *)numPadKeyChar[i]);
    }

    if (!numberOnly)
      GUI_DispStringInPrect(&rect_of_numkey[NUM_KEY_DEC],(uint8_t *)numPadKeyChar[NUM_KEY_DEC]);
    if (negative)
      GUI_DispStringInPrect(&rect_of_numkey[NUM_KEY_MINUS],(uint8_t *)numPadKeyChar[NUM_KEY_MINUS]);

    setFontSize(FONT_SIZE_LARGE);

    drawCharIcon(&rect_of_numkey[NUM_KEY_OK], CENTER, CHARICON_OK, false, 0);
    drawCharIcon(&rect_of_numkey[NUM_KEY_DEL], CENTER, CHARICON_POINT_LEFT, false, 0);
    drawCharIcon(&rect_of_numkey[NUM_KEY_EXIT], CENTER, CHARICON_CANCEL, false, 0);
    drawCharIcon(&rect_of_numkey[NUM_KEY_RESET], CENTER, CHARICON_RESET, false, 0);
  #endif  // KEYBOARD_MATERIAL_THEME

  GUI_DispStringInPrect(&arrowRect,(uint8_t *)"\u089A");

  setFontSize(FONT_SIZE_LARGE);
  if ((oldParameterRect.x1 - oldParameterRect.x0) <= GUI_StrPixelWidth_str(title))
    setFontSize(FONT_SIZE_NORMAL);

  GUI_DispStringInPrect(&oldParameterRect, title);
  setFontSize(FONT_SIZE_NORMAL);
}

static inline void drawValue(char * str)
{
  #ifdef KEYBOARD_MATERIAL_THEME
    GUI_SetBkColor(BAR_BG_COLOR);
    GUI_SetColor(BAR_FONT_COLOR);
  #endif
  GUI_ClearPrect(&newParameterRect);

  setFontSize(FONT_SIZE_LARGE);
  GUI_DispStringInPrect(&newParameterRect, (uint8_t *)str);
  setFontSize(FONT_SIZE_NORMAL);
}

// Numpad for decimal numbers
double numPadFloat(uint8_t * title, double old_val, double reset_val, bool negative)
{
  //bool exit = false;
  NUM_KEY_VALUES key_num = NUM_KEY_IDLE;
  touchSound = false;

  uint8_t nowIndex;
  uint8_t lastIndex = 0;
  char ParameterBuf[FLOAT_BUF_LENGTH + 1] = {0};
  uint8_t prec = (old_val == 0) ? 0 : 3;
  bool valueFirstPress = true;

  sprintf(ParameterBuf, "%.*f", prec, old_val);
  nowIndex = strlen(ParameterBuf);

  if (title == NULL)
  {
    char tempstr[FLOAT_BUF_LENGTH + 1];
    sprintf(tempstr, "%.*f", prec, old_val);
    title = (uint8_t *)tempstr;
  }

  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(rect_of_numkey), rect_of_numkey, drawKeypadButton, NULL);
  Draw_keyboard(title, false, negative);

  while (1)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case NUM_KEY_EXIT:
        BUZZER_PLAY(SOUND_CANCEL);
        touchSound = true;
        return old_val;

      case NUM_KEY_DEL:
        if (nowIndex == 1)  // last character deleted
        {
          if (ParameterBuf[0] == '0')
          { // '0' cannot be deleted
            BUZZER_PLAY(SOUND_DENY);
            break;
          }
          else
          {
            ParameterBuf[0] = '0';
            lastIndex = 0;  // this will trigger a value redraw
          }
        }
        else
        {
          ParameterBuf[--nowIndex] = 0;
        }

        BUZZER_PLAY(SOUND_KEYPRESS);
        valueFirstPress = false;
        break;

      case NUM_KEY_RESET:
        sprintf(ParameterBuf, "%.*f", prec, reset_val);
        nowIndex = strlen(ParameterBuf);
        lastIndex = 0;
        valueFirstPress = true;
        BUZZER_PLAY(SOUND_KEYPRESS);
        break;

      case NUM_KEY_1:
      case NUM_KEY_2:
      case NUM_KEY_3:
      case NUM_KEY_4:
      case NUM_KEY_5:
      case NUM_KEY_6:
      case NUM_KEY_7:
      case NUM_KEY_8:
      case NUM_KEY_9:
      case NUM_KEY_0:
        if (valueFirstPress == true)
        {
          valueFirstPress = false;
          nowIndex = lastIndex = 0;
        }
        if (nowIndex < FLOAT_BUF_LENGTH - 1)
        {
          if (ParameterBuf[0] == '0' && nowIndex == 1)  // avoid "0x", change to "x"
          {
            nowIndex = lastIndex = 0;
          }
          else if (ParameterBuf[0] == '-' && ParameterBuf[1] == '0' && nowIndex == 2)  // avoid "-0x", change to "-x"
          {
            nowIndex = lastIndex = 1;
          }

          ParameterBuf[nowIndex++] = numPadKeyChar[key_num][0];
          ParameterBuf[nowIndex] = 0;
          BUZZER_PLAY(SOUND_KEYPRESS);
        }
        else
        {
          BUZZER_PLAY(SOUND_DENY);
        }
        break;

      case NUM_KEY_DEC:
        if (valueFirstPress == true)
        {
          valueFirstPress = false;
          ParameterBuf[0] = '0';
          nowIndex = lastIndex = 1;
        }
        if (!strchr((const char *)ParameterBuf, numPadKeyChar[key_num][0]) && nowIndex < (FLOAT_BUF_LENGTH - 1))
        {
          if (nowIndex == 1 && ParameterBuf[0] == '-')  // check if minus sign and no other number
          {
            ParameterBuf[nowIndex++] = '0';             // add zero between minus and decimal sign
          }
          ParameterBuf[nowIndex++] = '.';
          ParameterBuf[nowIndex] = 0;
          BUZZER_PLAY(SOUND_KEYPRESS);
        }
        else
        {
          BUZZER_PLAY(SOUND_DENY);
        }
        break;

      case NUM_KEY_MINUS:
        if (negative)
        {
          if (valueFirstPress == true)
          {
            valueFirstPress = false;
            ParameterBuf[0] = '0';
            nowIndex = 1;
          }
          if (nowIndex == 1 && ParameterBuf[0] == '0')
          {
            ParameterBuf[0] = '-';
            ParameterBuf[1] = 0;
            lastIndex = 0;  // this will trigger a value redraw
            BUZZER_PLAY(SOUND_KEYPRESS);
          }
          else
          {
            BUZZER_PLAY(SOUND_DENY);
          }
        }
        break;

      case NUM_KEY_OK:
        if (nowIndex == 1 && ParameterBuf[0] == '-')
        {
          BUZZER_PLAY(SOUND_DENY);
          break;
        }
        BUZZER_PLAY(SOUND_OK);
        touchSound = true;
        return strtod(ParameterBuf, NULL);

      default:
        break;
    }

    if (lastIndex != nowIndex)
    {
      lastIndex = nowIndex;
      drawValue(ParameterBuf);
    }
    loopBackEnd();
  }
}

// Numpad for integer numbers
int32_t numPadInt(uint8_t* title, int32_t old_val, int32_t reset_val, bool negative)
{
  NUM_KEY_VALUES key_num = NUM_KEY_IDLE;
  touchSound = false;

  int32_t val = old_val, lastval = 0;
  uint8_t len = 0;
  char ParameterBuf[INT_BUF_LENGTH + 1];
  int8_t neg = (old_val < 0) ? -1 : 1;
  int8_t lastneg = 1;
  bool valueFirstPress = true;

  val = old_val * neg;

  if (title == NULL)
  {
    char tempstr[INT_BUF_LENGTH + 1];
    sprintf(tempstr, "%i", old_val);
    title = (uint8_t *)tempstr;
  }

  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(rect_of_numkey), rect_of_numkey, drawKeypadButton, NULL);
  Draw_keyboard(title, true, negative);

  sprintf(ParameterBuf, "%i", val);
  drawValue(ParameterBuf);
  len = strlen(ParameterBuf);

  while (1)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case NUM_KEY_EXIT:
        BUZZER_PLAY(SOUND_CANCEL);
        touchSound = true;
        return old_val;

      case NUM_KEY_MINUS:
        if (negative)
        {
          neg = neg * -1;
          BUZZER_PLAY(SOUND_KEYPRESS);
        }
        break;

      case NUM_KEY_DEL:
        if (val > 0)
        {
          val /= 10;
          valueFirstPress = false;
          BUZZER_PLAY(SOUND_KEYPRESS);
        }
        else
        {
          BUZZER_PLAY(SOUND_DENY);
        }
        break;

      case NUM_KEY_RESET:
        neg = (reset_val >= 0) ? 1 : -1;
        val = reset_val * neg;
        valueFirstPress = true;
        BUZZER_PLAY(SOUND_KEYPRESS);
        break;

      case NUM_KEY_1:
      case NUM_KEY_2:
      case NUM_KEY_3:
      case NUM_KEY_4:
      case NUM_KEY_5:
      case NUM_KEY_6:
      case NUM_KEY_7:
      case NUM_KEY_8:
      case NUM_KEY_9:
      case NUM_KEY_0:
        if (valueFirstPress == true)
        {
          valueFirstPress = false;
          val = 0;
        }
        len = strlen(ParameterBuf);
        if (len < INT_BUF_LENGTH && !(val == 0 && key_num == NUM_KEY_0))
        {
          int num = (numPadKeyChar[key_num][0] - '0');
          val = (val * 10) + ABS(num);
          BUZZER_PLAY(SOUND_KEYPRESS);
        }
        else
        {
          BUZZER_PLAY(SOUND_DENY);
        }
        break;

      case NUM_KEY_OK:
        BUZZER_PLAY(SOUND_OK);
        setFontSize(FONT_SIZE_NORMAL);
        touchSound = true;
        return (val * neg);

      default:
        break;
    }

    if (lastval != val || lastneg != neg)
    {
      lastval = val;
      lastneg = neg;
      GUI_ClearPrect(&newParameterRect);
      sprintf(ParameterBuf, "%i", val * neg);
      drawValue(ParameterBuf);
    }
    loopBackEnd();
  }
}
