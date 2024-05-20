#include "Touch_Screen.h"
#include "includes.h"
#include "GPIO_Init.h"

#if defined(MKS_TFT35_V1_0)
  // touch screen X, Y channels are swapped for MKS_TFT35_V1_0.
  // Defined in MKS TFT35 Touch Screen source code: user\others\mks_touch_screen.c CHX=0x90, CHY=0xD0
  #define CMD_RDX 0x90
  #define CMD_RDY 0xD0
#else
  #define CMD_RDX 0xD0
  #define CMD_RDY 0x90
#endif

#define XL1 LCD_X[0]
#define XL2 LCD_X[1]
#define XL3 LCD_X[2]

#define YL1 LCD_Y[0]
#define YL2 LCD_Y[1]
#define YL3 LCD_Y[2]

#define X1  TP_X[0]
#define X2  TP_X[1]
#define X3  TP_X[2]

#define Y1  TP_Y[0]
#define Y2  TP_Y[1]
#define Y3  TP_Y[2]

#define A   TS_CalPara[0]
#define B   TS_CalPara[1]
#define C   TS_CalPara[2]
#define D   TS_CalPara[3]
#define E   TS_CalPara[4]
#define F   TS_CalPara[5]
#define K   TS_CalPara[6]

#define TS_DEBOUNCE_MS 20  // 20 ms
#define TS_ERR_RANGE   10

static volatile uint8_t touchCountdown = TS_DEBOUNCE_MS;

int32_t TS_CalPara[7];
bool TS_Sound = true;
void (* TS_ReDrawIcon)(uint8_t position, uint8_t isPressed) = NULL;

void TS_GetCoordinates(uint16_t * x, uint16_t * y)
{
  uint16_t tp_x = XPT2046_Repeated_Compare_AD(CMD_RDX);
  uint16_t tp_y = XPT2046_Repeated_Compare_AD(CMD_RDY);

  *x = (A * tp_x + B * tp_y + C) / K;
  *y = (D * tp_x + E * tp_y + F) / K;
}

void TS_CheckPress(void)
{
  if (XPT2046_Read_Pen() == LOW)
  {
    if (touchCountdown > 0)
      touchCountdown--;
  }
  else
  {
    touchCountdown = TS_DEBOUNCE_MS;
  }
}

bool TS_IsPressed(void)
{
  return (touchCountdown == 0);
}

uint16_t TS_KeyValue(uint8_t totalRect, const GUI_RECT * menuRect)
{
  uint8_t i = 0;
  uint16_t x, y;

  TS_GetCoordinates(&x, &y);

  for (i = 0; i < totalRect; i++)
  {
    if ((x > menuRect[i].x0) && (x < menuRect[i].x1) && (y > menuRect[i].y0) && (y < menuRect[i].y1))
    {
      #ifdef BUZZER_PIN
        if (TS_Sound == true)
          BUZZER_PLAY(SOUND_KEYPRESS);
      #endif

      return i;
    }
  }

  return IDLE_TOUCH;
}

uint16_t KEY_GetValue(uint8_t totalRect, const GUI_RECT * menuRect)
{
  static uint16_t key_num = IDLE_TOUCH;
  static bool firstPress = true;

  uint16_t key_return = IDLE_TOUCH;

  if (TS_IsPressed())
  {
    if (firstPress)
    {
      key_num = TS_KeyValue(totalRect, menuRect);
      firstPress = false;

      if (TS_ReDrawIcon)
        TS_ReDrawIcon(key_num, 1);
    }
  }
  else
  {
    if (firstPress == false)
    {
      if (TS_ReDrawIcon)
        TS_ReDrawIcon(key_num, 0);

      key_return = key_num;
      key_num = IDLE_TOUCH;
      firstPress = true;

      #ifdef LCD_LED_PWM_CHANNEL
        // if LCD is blocked (on idle), skip the first touch, preventing to trigger any undesired action,
        // and wait the LCD brightness is restored first
        if (LCD_IsBlocked())
          key_return = IDLE_TOUCH;
      #endif
    }
  }

  return key_return;
}

static inline uint8_t TS_CalibrationEnsure(uint16_t x, uint16_t y)
{
  uint32_t i;
  uint16_t tp_x, tp_y;
  int lcd_x, lcd_y;

  GUI_SetColor(BLACK);
  GUI_FillCircle(x, y, 5);

  for (i = 0; i < 10; i++)
  {
    GUI_DrawPoint(x + i, y);
    GUI_DrawPoint(x - i, y);
    GUI_DrawPoint(x, y + i);
    GUI_DrawPoint(x, y - i);
  }

  while (!TS_IsPressed());

  tp_x = XPT2046_Repeated_Compare_AD(CMD_RDX);
  tp_y = XPT2046_Repeated_Compare_AD(CMD_RDY);

  lcd_x = (A * tp_x + B * tp_y + C) / K;
  lcd_y = (D * tp_x + E * tp_y + F) / K;

  if (lcd_x < x + TS_ERR_RANGE && lcd_x > x - TS_ERR_RANGE && lcd_y > y - TS_ERR_RANGE && lcd_y < y + TS_ERR_RANGE)
  {
    GUI_DispStringCenter(LCD_WIDTH / 2, LCD_HEIGHT - 40, (int32_t)LABEL_ADJUST_OK);
    Delay_ms(1000);
  }
  else
  {
    while (TS_IsPressed());

    GUI_SetColor(RED);
    GUI_DispStringCenter(LCD_WIDTH / 2, LCD_HEIGHT - 40, (int32_t)LABEL_ADJUST_FAILED);
    GUI_DispDec(0, 0, lcd_x, 3, 0);
    GUI_DispDec(0, 20, lcd_y, 3, 0);
    Delay_ms(1000);

    return 0;
  }

  return 1;
}

void TS_Calibrate(void)
{
  uint32_t LCD_X[3] = {40, LCD_WIDTH - 40, LCD_WIDTH - 40};
  uint32_t LCD_Y[3] = {40, 40, LCD_HEIGHT - 40};
  uint16_t TP_X[3], TP_Y[3];
  uint32_t tp_num = 0;
  int i;

  do
  {
    GUI_Clear(WHITE);
    GUI_SetColor(BLACK);
    GUI_SetBkColor(WHITE);
    GUI_DispStringCenter(LCD_WIDTH / 2, 5, (int32_t)LABEL_ADJUST_TITLE);
    GUI_DispStringCenter(LCD_WIDTH / 2, 25, (int32_t)LABEL_ADJUST_INFO);
    GUI_SetColor(RED);

    for (tp_num = 0; tp_num < 3; tp_num++)
    {
      GUI_FillCircle(LCD_X[tp_num], LCD_Y[tp_num], 3);

      for (i = 0; i < 10; i++)
      {
        GUI_DrawPoint(LCD_X[tp_num] + i, LCD_Y[tp_num]);
        GUI_DrawPoint(LCD_X[tp_num] - i, LCD_Y[tp_num]);
        GUI_DrawPoint(LCD_X[tp_num], LCD_Y[tp_num] + i);
        GUI_DrawPoint(LCD_X[tp_num], LCD_Y[tp_num] - i);
      }

      while (TS_IsPressed() == false);

      TP_X[tp_num] = XPT2046_Repeated_Compare_AD(CMD_RDX);
      TP_Y[tp_num] = XPT2046_Repeated_Compare_AD(CMD_RDY);

      while (TS_IsPressed() != false);
    }

    K = (X1 - X3) * (Y2 - Y3) - (X2 - X3) * (Y1 - Y3);
    A = ((XL1 - XL3) * (Y2 - Y3) - (XL2 - XL3) * (Y1 - Y3));
    B = (( X1 - X3 ) * ( XL2 - XL3) - (XL1 - XL3) * ( X2 - X3));
    C = (Y1 * (X3 * XL2 - X2 * XL3) + Y2 * (X1 * XL3 - X3 * XL1) + Y3 * (X2 * XL1 - X1 * XL2));
    D = ((YL1 - YL3) * (Y2 - Y3) - (YL2 - YL3) * (Y1 - Y3));
    E = ((X1 - X3) * (YL2 - YL3) - (YL1 - YL3) * (X2 - X3));
    F = (Y1 * (X3 * YL2 - X2 * YL3) + Y2 * (X1 * YL3 - X3 * YL1) + Y3 * (X2 * YL1 - X1 * YL2));
  } while (TS_CalibrationEnsure(LCD_WIDTH / 2, LCD_HEIGHT / 2) == 0);

  GUI_RestoreColorDefault();
}
