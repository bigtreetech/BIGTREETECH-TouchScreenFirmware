#include "LEDColor.h"
#include "includes.h"

// value ranges
#define LED_UPDATE_TIME 1000  // 1 seconds is 1000
#define LED_MIN_VALUE   0
#define LED_MAX_VALUE   255

// data structures
#define LED_VECT_SIZE 6

// LED color components vector:
// R: red
// G: green
// B: blue
// W: white;     NEOPIXEL or RGB(W)
// P: intensity; NEOPIXEL
// I: index;     NEOPIXEL
typedef uint8_t LED_VECT[LED_VECT_SIZE];

// key button enumeration
typedef enum
{
  LED_KEY_PREV = 0,
  LED_KEY_NEXT,
  // after NEXT, all the other keys
  LED_KEY_RESET,
  LED_KEY_CANCEL,
  LED_KEY_OK,
  // no need to declare key numbers if no special task is performed by the key
  LED_KEY_IDLE = IDLE_TOUCH,
} LED_KEY_VALUES;

// colors
#define KB_FONT_COLOR     infoSettings.font_color
#undef KB_BG_COLOR
#define KB_BG_COLOR       infoSettings.bg_color
#define KB_BORDER_COLOR   infoSettings.list_border_color
#define KB_BORDER_COLOR_2 0x4b0d

#define CTRL_FONT_COLOR   WHITE
#define CTRL_BG_COLOR     0x2174
#define CTRL_BORDER_COLOR 0x4b0d

// layout sizes
#define KB_ROW_NUM   4
#define KB_COL_NUM   4
#define CTRL_COL_NUM 3
#define KEY_NUM      ((KB_COL_NUM * KB_ROW_NUM - 1) + (CTRL_COL_NUM - 1))  // keyboard keys + control bar keys
#define ROW_NUM      (KB_ROW_NUM + 1)                                      // keyboard rows + control bar
#define ROW_HEIGHT   (LCD_HEIGHT / ROW_NUM)                                // button height
#define PAGE_ITEMS   3
#define PAGE_NUM     (LED_VECT_SIZE / PAGE_ITEMS)

// keyboard key sizes
#define KB_WIDTH  (LCD_WIDTH / KB_COL_NUM)
#define KB_HEIGHT ROW_HEIGHT

// control bar keys sizes
#define CTRL_WIDTH  (LCD_WIDTH / CTRL_COL_NUM)
#define CTRL_HEIGHT ROW_HEIGHT

// key button rectangles
const GUI_RECT ledKeyRect[KEY_NUM] = {
#ifdef KEYBOARD_ON_LEFT
  // control bar
  {0 * CTRL_WIDTH, 4 * CTRL_HEIGHT, 1 * CTRL_WIDTH, 5 * CTRL_HEIGHT},  // PREV
  {1 * CTRL_WIDTH, 4 * CTRL_HEIGHT, 2 * CTRL_WIDTH, 5 * CTRL_HEIGHT},  // NEXT

  // menu buttons
  {2 * KB_WIDTH, 0 * KB_HEIGHT, 3 * KB_WIDTH, 1 * KB_HEIGHT},  // RESET
  {1 * KB_WIDTH, 0 * KB_HEIGHT, 2 * KB_WIDTH, 1 * KB_HEIGHT},  // CANCEL
  {0 * KB_WIDTH, 0 * KB_HEIGHT, 1 * KB_WIDTH, 1 * KB_HEIGHT},  // OK

  // R component
  {3 * KB_WIDTH, 1 * KB_HEIGHT, 4 * KB_WIDTH, 2 * KB_HEIGHT},  // COLOR
  {2 * KB_WIDTH, 1 * KB_HEIGHT, 3 * KB_WIDTH, 2 * KB_HEIGHT},  // EDIT
  {0 * KB_WIDTH, 1 * KB_HEIGHT, 1 * KB_WIDTH, 2 * KB_HEIGHT},  // DEC
  {1 * KB_WIDTH, 1 * KB_HEIGHT, 2 * KB_WIDTH, 2 * KB_HEIGHT},  // INC

  // G component
  {3 * KB_WIDTH, 2 * KB_HEIGHT, 4 * KB_WIDTH, 3 * KB_HEIGHT},  // COLOR
  {2 * KB_WIDTH, 2 * KB_HEIGHT, 3 * KB_WIDTH, 3 * KB_HEIGHT},  // EDIT
  {0 * KB_WIDTH, 2 * KB_HEIGHT, 1 * KB_WIDTH, 3 * KB_HEIGHT},  // DEC
  {1 * KB_WIDTH, 2 * KB_HEIGHT, 2 * KB_WIDTH, 3 * KB_HEIGHT},  // INC

  // B component
  {3 * KB_WIDTH, 3 * KB_HEIGHT, 4 * KB_WIDTH, 4 * KB_HEIGHT},  // COLOR
  {2 * KB_WIDTH, 3 * KB_HEIGHT, 3 * KB_WIDTH, 4 * KB_HEIGHT},  // EDIT
  {0 * KB_WIDTH, 3 * KB_HEIGHT, 1 * KB_WIDTH, 4 * KB_HEIGHT},  // DEC
  {1 * KB_WIDTH, 3 * KB_HEIGHT, 2 * KB_WIDTH, 4 * KB_HEIGHT},  // INC
#else
  // control bar
  {1 * CTRL_WIDTH, 4 * CTRL_HEIGHT, 2 * CTRL_WIDTH, 5 * CTRL_HEIGHT},  // PREV
  {2 * CTRL_WIDTH, 4 * CTRL_HEIGHT, 3 * CTRL_WIDTH, 5 * CTRL_HEIGHT},  // NEXT

  // menu buttons
  {1 * KB_WIDTH, 0 * KB_HEIGHT, 2 * KB_WIDTH, 1 * KB_HEIGHT},  // RESET
  {2 * KB_WIDTH, 0 * KB_HEIGHT, 3 * KB_WIDTH, 1 * KB_HEIGHT},  // CANCEL
  {3 * KB_WIDTH, 0 * KB_HEIGHT, 4 * KB_WIDTH, 1 * KB_HEIGHT},  // OK

  // R component
  {0 * KB_WIDTH, 1 * KB_HEIGHT, 1 * KB_WIDTH, 2 * KB_HEIGHT},  // COLOR
  {1 * KB_WIDTH, 1 * KB_HEIGHT, 2 * KB_WIDTH, 2 * KB_HEIGHT},  // EDIT
  {2 * KB_WIDTH, 1 * KB_HEIGHT, 3 * KB_WIDTH, 2 * KB_HEIGHT},  // DEC
  {3 * KB_WIDTH, 1 * KB_HEIGHT, 4 * KB_WIDTH, 2 * KB_HEIGHT},  // INC

  // G component
  {0 * KB_WIDTH, 2 * KB_HEIGHT, 1 * KB_WIDTH, 3 * KB_HEIGHT},  // COLOR
  {1 * KB_WIDTH, 2 * KB_HEIGHT, 2 * KB_WIDTH, 3 * KB_HEIGHT},  // EDIT
  {2 * KB_WIDTH, 2 * KB_HEIGHT, 3 * KB_WIDTH, 3 * KB_HEIGHT},  // DEC
  {3 * KB_WIDTH, 2 * KB_HEIGHT, 4 * KB_WIDTH, 3 * KB_HEIGHT},  // INC

  // B component
  {0 * KB_WIDTH, 3 * KB_HEIGHT, 1 * KB_WIDTH, 4 * KB_HEIGHT},  // COLOR
  {1 * KB_WIDTH, 3 * KB_HEIGHT, 2 * KB_WIDTH, 4 * KB_HEIGHT},  // EDIT
  {2 * KB_WIDTH, 3 * KB_HEIGHT, 3 * KB_WIDTH, 4 * KB_HEIGHT},  // DEC
  {3 * KB_WIDTH, 3 * KB_HEIGHT, 4 * KB_WIDTH, 4 * KB_HEIGHT},  // INC
#endif
};

#ifdef KEYBOARD_ON_LEFT
const GUI_RECT ledColorRect = {
  3 * KB_WIDTH, 0 * KB_HEIGHT, 4 * KB_WIDTH, 1 * KB_HEIGHT};

const GUI_RECT ledPageRect = {
  2 * CTRL_WIDTH, 4 * CTRL_HEIGHT, 3 * CTRL_WIDTH, 5 * KB_HEIGHT};
#else
const GUI_RECT ledColorRect = {
  0 * KB_WIDTH, 0 * KB_HEIGHT, 1 * KB_WIDTH, 1 * KB_HEIGHT};

const GUI_RECT ledPageRect = {
  0 * CTRL_WIDTH, 4 * CTRL_HEIGHT, 1 * CTRL_WIDTH, 5 * KB_HEIGHT};
#endif

// area rectangles
const GUI_RECT ledAreaRect[2] = {
  {0, 0,               LCD_WIDTH, LCD_HEIGHT - CTRL_HEIGHT},  // keyboard area
  {0, 4 * CTRL_HEIGHT, LCD_WIDTH, LCD_HEIGHT}                 // control bar area
};

const char *const ledKeyString[2] = {
  "<",  // PREV
  ">",  // NEXT
};

const char * const ledString[LED_VECT_SIZE] = {"R", "G", "B", "W", "P", "I"};

const LED_VECT ledRed =   {0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF};
const LED_VECT ledGreen = {0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF};
const LED_VECT ledBlue =  {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
const LED_VECT ledWhite = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const LED_VECT ledOff =   {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

LED_VECT ledValue = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};

uint8_t ledPage = 0;
uint8_t ledIndex = 0;

void ledSendValue(const LED_VECT * led)
{
  storeCmd("M150 R%d U%d B%d W%d P%d I%d\n", (*led)[0], (*led)[1], (*led)[2], (*led)[3], (*led)[4], (*led)[5]);
}

void ledGetValue(LED_VECT * led)
{
  for (int i = 0; i < LED_VECT_SIZE; i++)
    (*led)[i] = ledValue[i];
}

void ledSetValue(const LED_VECT * led, bool skipNeopixel)
{
  int size = skipNeopixel ? LED_VECT_SIZE - 2 : LED_VECT_SIZE;

  for (int i = 0; i < size; i++)
    ledValue[i] = (*led)[i];
}

uint8_t ledGetComponentIndex(uint8_t index)
{
  return ledPage * PAGE_ITEMS + index;
}

uint8_t ledGetComponentValue(uint8_t index)
{
  return ledValue[ledGetComponentIndex(index)];
}

static inline uint8_t ledEditComponentValue(uint8_t index)
{
  uint8_t realIndex = ledGetComponentIndex(index);

  return ledValue[realIndex] = editIntValue(LED_MIN_VALUE, LED_MAX_VALUE, ledValue[realIndex], ledValue[realIndex]);
}

uint8_t ledUpdateComponentValue(uint8_t index, int8_t unit, int8_t direction)
{
  uint8_t realIndex = ledGetComponentIndex(index);

  return ledValue[realIndex] = NOBEYOND(LED_MIN_VALUE, ledValue[realIndex] + (int16_t) (direction * unit), LED_MAX_VALUE);
}

uint8_t ledGetControlIndex(uint8_t keyNum)
{
  return (keyNum - (LED_KEY_OK + 1)) / 4;
}

uint8_t ledGetControlSubIndex(uint8_t keyNum)
{
  return (keyNum - (LED_KEY_OK + 1)) % 4;
}

uint16_t ledGetRGBColor(LED_VECT * led)
{
  uint8_t r = (*led)[0] >> 3;
  uint8_t g = (*led)[1] >> 2;
  uint8_t b = (*led)[2] >> 3;
  return ((r & 0x001F) << 11) | ((g & 0x003F) << 5) | ((b & 0x001F));  // RGB color in RGB 565 16 bit format
}

uint16_t ledGetComponentRGBColor(uint8_t component, uint8_t index)
{
  LED_VECT led = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // component RGB color in RGB 565 16 bit format

  led[index] = component;

  return ledGetRGBColor(&led);
}

static inline void ledDrawPageNumber(void)
{
  char tempstr[10];

  sprintf(tempstr, "%d/%d", ledPage + 1, PAGE_NUM);

  drawStandardValue(&ledPageRect, VALUE_STRING, &tempstr, true, CTRL_FONT_COLOR, CTRL_BG_COLOR, 1, true);
}

void ledDrawControl(uint8_t index, bool isFocused, bool drawFocus, bool drawAll)
{
  GUI_RECT rect, rect2;
  uint16_t bgColorFocus;
  uint8_t realIndex = ledGetComponentIndex(index);
  uint8_t i = (LED_KEY_OK + 1) + index * 4;
  uint8_t j = i + 1;

  rect = rect2 = ledKeyRect[i];

  #ifdef KEYBOARD_ON_LEFT
    rect.x0 = rect2.x1 = (rect.x0 + rect.x1) / 2;
  #else
    rect.x1 = rect2.x0 = (rect.x0 + rect.x1) / 2;
  #endif

  bgColorFocus = isFocused ? KB_BORDER_COLOR : KB_BG_COLOR;

  // draw focus border
  if (drawFocus)
    drawBorder(&ledKeyRect[i], bgColorFocus, 2);

  // draw LED component string
  drawStandardValue(&rect, VALUE_STRING, ledString[realIndex], true, KB_FONT_COLOR, bgColorFocus, 3, drawFocus);

  // draw LED component RGB color
  drawStandardValue(&rect2, VALUE_NONE, NULL, true, KB_FONT_COLOR, ledGetComponentRGBColor(ledValue[realIndex], realIndex), 3, true);

  // draw LED component value
  drawStandardValue(&ledKeyRect[j], VALUE_BYTE, &ledValue[realIndex], true, KB_FONT_COLOR, KB_BORDER_COLOR, 4, true);

  if (drawAll)
  {
    // draw LED component value area borders (outer)
    drawBorder(&ledKeyRect[j], KB_BORDER_COLOR, 2);

    // draw LED component value area borders (inner)
    drawBorder(&ledKeyRect[j], KB_BORDER_COLOR_2, 3);

    // draw buttons
    drawStandardValue(&ledKeyRect[j + 1], VALUE_STRING, ledKeyString[LED_KEY_PREV], true, KB_FONT_COLOR, KB_BG_COLOR, 3, true);
    drawStandardValue(&ledKeyRect[j + 2], VALUE_STRING, ledKeyString[LED_KEY_NEXT], true, KB_FONT_COLOR, KB_BG_COLOR, 3, true);
  }
}

void ledDrawButton(uint8_t index, uint8_t isPressed)
{
  if (index >= KEY_NUM)
    return;

  if (isPressed)
  {
    if (index <= LED_KEY_NEXT)
      drawStandardValue(&ledKeyRect[index], VALUE_STRING, ledKeyString[index], true, CTRL_BG_COLOR, CTRL_FONT_COLOR, 1, true);
    else
      drawBorder(&ledKeyRect[index], KB_FONT_COLOR, 1);
  }
  else
  {
    if (index <= LED_KEY_NEXT)
      drawStandardValue(&ledKeyRect[index], VALUE_STRING, ledKeyString[index], true, CTRL_FONT_COLOR, CTRL_BG_COLOR, 1, true);
    else
      drawBorder(&ledKeyRect[index], KB_BG_COLOR, 1);
  }
}

void ledDrawKeyboard(void)
{
  // draw horizontal button borders
  GUI_SetColor(KB_BORDER_COLOR);

  for (int i = 1; i < (KB_ROW_NUM); i++)
  {
    GUI_HLine(0, (i * KB_HEIGHT), LCD_WIDTH);
  }

  // draw LED component RGB colors, values and buttons
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  for (uint8_t i = 0; i < KEY_NUM; i++)
  {
    if (i <= LED_KEY_NEXT)  // if control bar key
    {
      ledDrawButton(i, false);
    }
    else if (i > LED_KEY_OK)  // if not unicode string
    {
      if (ledGetControlSubIndex(i) == 0)
        ledDrawControl(ledGetControlIndex(i), false, true, true);
    }
  }

  // draw unicode string
  DrawCharIcon(&ledKeyRect[LED_KEY_RESET], MIDDLE, ICONCHAR_RESET, false, 0);
  DrawCharIcon(&ledKeyRect[LED_KEY_CANCEL], MIDDLE, ICONCHAR_CANCEL, false, 0);
  DrawCharIcon(&ledKeyRect[LED_KEY_OK], MIDDLE, ICONCHAR_OK, false, 0);

  // draw focus for current LED component
  ledDrawControl(ledIndex, true, true, false);

  // draw RGB color
  drawStandardValue(&ledColorRect, VALUE_NONE, NULL, true, KB_FONT_COLOR, ledGetRGBColor(&ledValue), 3, true);

  // draw page number
  ledDrawPageNumber();

  // restore default
  GUI_RestoreColorDefault();
}

void ledDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(ledKeyRect), ledKeyRect, ledDrawButton, &ledDrawMenu);

  // clear keyboard area
  GUI_SetBkColor(KB_BG_COLOR);
  GUI_ClearPrect(&ledAreaRect[0]);

  // clear control bar area
  GUI_SetBkColor(CTRL_BG_COLOR);
  GUI_ClearPrect(&ledAreaRect[1]);

  // draw control bar area shadow border
  GUI_SetColor(CTRL_BORDER_COLOR);
  GUI_HLine(ledAreaRect[1].x0, ledAreaRect[1].y0, ledAreaRect[1].x1);  // first row of control bar used for shadow border

  // draw RGB color border
  drawBorder(&ledColorRect, KB_BORDER_COLOR, 2);

  // draw keyboard
  ledDrawKeyboard();
}

void menuLEDColorCustom(void)
{
  LED_KEY_VALUES key_num = LED_KEY_IDLE;
  LED_VECT origLedValue;
  uint8_t newIndex;
  uint16_t newValue, curValue;
  bool updateForced, sendingNeeded;

  ledGetValue(&origLedValue);  // get initial LED color components vector
  newIndex = ledIndex;
  newValue = curValue = ledGetComponentValue(newIndex);
  updateForced = sendingNeeded = false;

  ledDrawMenu();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuLEDColorCustom)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // previous page
      case LED_KEY_PREV:
        if (ledPage > 0)
        {
          ledPage--;

          updateForced = true;
        }
        break;

      // next page
      case LED_KEY_NEXT:
        if (ledPage < (PAGE_NUM - 1))
        {
          ledPage++;

          updateForced = true;
        }
        break;

      // apply new LED color and exit
      case LED_KEY_OK:
        ledGetValue(&origLedValue);
        // no break here

      // restore original LED color and exit
      case LED_KEY_CANCEL:
        infoMenu.cur--;
        // no break here

      // restore original LED color
      case LED_KEY_RESET:
        ledSetValue(&origLedValue, false);

        updateForced = true;
        break;

      // use rotary encoder to update LED component value
      case LED_KEY_IDLE:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            curValue = ledUpdateComponentValue(ledIndex, 1, (encoderPosition > 0) ? 1 : -1);

            encoderPosition = 0;
          }
        #endif
        break;

      default:
        ledIndex = ledGetControlIndex(key_num);  // get control index

        switch (ledGetControlSubIndex(key_num))  // get control sub index
        {
          case 1:
          {
            curValue = ledEditComponentValue(ledIndex);

            sendingNeeded = true;

            ledDrawMenu();
            break;
          }

          // decrease LED component value
          case 2:
            curValue = ledUpdateComponentValue(ledIndex, 1, -1);
            break;

          // increase LED component value
          case 3:
            curValue = ledUpdateComponentValue(ledIndex, 1, 1);
            break;

          default:
            break;
        }
        break;
    }

    if (updateForced)
      ledDrawKeyboard();  // draw all

    if (newIndex != ledIndex)
    {
      ledDrawControl(newIndex, false, true, false);  // drop focus and draw LED component value
      ledDrawControl(ledIndex, true, true, false);   // get focus and draw LED component value

      newIndex = ledIndex;
      newValue = curValue = ledGetComponentValue(newIndex);  // always load the value for the new index
    }

    if (newValue != curValue)
    {
      ledDrawControl(ledIndex, newIndex == ledIndex, false, false);  // draw changed LED component value
      drawStandardValue(&ledColorRect, VALUE_NONE, NULL, true, KB_FONT_COLOR, ledGetRGBColor(&ledValue), 3, true);  // draw RGB color

      newValue = curValue;
      sendingNeeded = true;
    }

    if ((sendingNeeded && nextScreenUpdate(LED_UPDATE_TIME)) || updateForced)
    {
      ledSendValue(&ledValue);

      updateForced = sendingNeeded = false;
    }

    loopProcess();
  }

  // restore default
  GUI_RestoreColorDefault();
}

const MENUITEMS LEDColorItems = {
  // title
  LABEL_RGB_SETTINGS,
  // icon                          label
  {
    {ICON_RGB_RED,                 LABEL_RED},
    {ICON_RGB_GREEN,               LABEL_GREEN},
    {ICON_RGB_BLUE,                LABEL_BLUE},
    {ICON_RGB_WHITE,               LABEL_WHITE},
    {ICON_CUSTOM,                  LABEL_CUSTOM},
    {ICON_RGB_WHITE,               LABEL_ON},
    {ICON_RGB_OFF,                 LABEL_OFF},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuLEDColor(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&LEDColorItems);

  while (infoMenu.menu[infoMenu.cur] == menuLEDColor)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // red
      case KEY_ICON_0:
        ledSetValue(&ledRed, true);
        break;

      // green
      case KEY_ICON_1:
        ledSetValue(&ledGreen, true);
        break;

      // blue
      case KEY_ICON_2:
        ledSetValue(&ledBlue, true);
        break;

      // white
      case KEY_ICON_3:
        ledSetValue(&ledWhite, true);
        break;

      // custom LED color
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuLEDColorCustom;
        break;

      // turn off
      case KEY_ICON_6:
        ledSendValue(&ledOff);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    if (key_num <= KEY_ICON_5)  // change LED color
      ledSendValue(&ledValue);

    loopProcess();
  }
}
