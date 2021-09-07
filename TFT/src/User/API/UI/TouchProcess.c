#include "TouchProcess.h"
#include "includes.h"
#include "GPIO_Init.h"

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

#define A TSC_Para[0]
#define B TSC_Para[1]
#define C TSC_Para[2]
#define D TSC_Para[3]
#define E TSC_Para[4]
#define F TSC_Para[5]
#define K TSC_Para[6]

int32_t TSC_Para[7];
static volatile bool touchScreenIsPress = false;
bool touchSound = true;

void TS_Get_Coordinates(uint16_t *x, uint16_t *y)
{
  uint16_t tp_x = XPT2046_Repeated_Compare_AD(CMD_RDX);
  uint16_t tp_y = XPT2046_Repeated_Compare_AD(CMD_RDY);

  *x = (A * tp_x + B * tp_y + C) / K;
  *y = (D * tp_x + E * tp_y + F) / K;
}

#define TS_ERR_RANGE 10
uint8_t calibrationEnsure(uint16_t x, uint16_t y)
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
  while (!isPress());
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
    while (isPress());
    GUI_SetColor(RED);
    GUI_DispStringCenter(LCD_WIDTH / 2, LCD_HEIGHT - 40, (int32_t)LABEL_ADJUST_FAILED);
    GUI_DispDec(0, 0, lcd_x, 3, 0);
    GUI_DispDec(0, 20, lcd_y, 3, 0);
    Delay_ms(1000);
    return 0;
  }
  return 1;
}

void TSC_Calibration(void)
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
      while (isPress() == false);
      TP_X[tp_num] = XPT2046_Repeated_Compare_AD(CMD_RDX);
      TP_Y[tp_num] = XPT2046_Repeated_Compare_AD(CMD_RDY);
      while (isPress() != false);
    }
    K = (X1 - X3) * (Y2 - Y3) - (X2 - X3) * (Y1 - Y3);
    A = ((XL1 - XL3) * (Y2 - Y3) - (XL2 - XL3) * (Y1 - Y3));
    B = (( X1 - X3 ) * ( XL2 - XL3) - (XL1 - XL3) * ( X2 - X3));
    C = (Y1 * (X3 * XL2 - X2 * XL3) + Y2 * (X1 * XL3 - X3 * XL1) + Y3 * (X2 * XL1 - X1 * XL2));
    D = ((YL1 - YL3) * (Y2 - Y3) - (YL2 - YL3) * (Y1 - Y3));
    E = ((X1 - X3) * (YL2 - YL3) - (YL1 - YL3) * (X2 - X3));
    F = (Y1 * (X3 * YL2 - X2 * YL3) + Y2 * (X1 * YL3 - X3 * YL1) + Y3 * (X2 * YL1 - X1 * YL2));
  } while (calibrationEnsure(LCD_WIDTH / 2, LCD_HEIGHT / 2) == 0);

  GUI_RestoreColorDefault();
}

uint16_t Key_value(uint8_t total_rect, const GUI_RECT *menuRect)
{
  uint8_t i = 0;
  uint16_t x, y;

  TS_Get_Coordinates(&x, &y);
  for (i = 0; i < total_rect; i++)
  {
    if ((x > menuRect[i].x0) && (x < menuRect[i].x1) && (y > menuRect[i].y0) && (y < menuRect[i].y1))
    {
      #ifdef BUZZER_PIN
        if (touchSound == true) BUZZER_PLAY(SOUND_KEYPRESS);
      #endif
      return i;
    }
  }
  return IDLE_TOUCH;
}

void loopTouchScreen(void)  // Handle in interrupt
{
  static uint8_t touch;
  if (!XPT2046_Read_Pen())
  {
    if (touch >= 20)  // 20ms
    {
      touchScreenIsPress = true;
    }
    else
    {
      touch++;
    }
  }
  else
  {
    touchScreenIsPress = false;
    touch = 0;
  }
}

uint8_t isPress(void)
{
  return touchScreenIsPress;
}

void (*TSC_ReDrawIcon)(uint8_t position, uint8_t is_press) = NULL;

uint16_t KEY_GetValue(uint8_t total_rect, const GUI_RECT* menuRect)
{
  static uint16_t key_num = IDLE_TOUCH;
  static bool firstPress = true;

  uint16_t key_return = IDLE_TOUCH;

  if (touchScreenIsPress)
  {
    if (firstPress)
    {
      key_num = Key_value(total_rect, menuRect);
      firstPress = false;
      if (TSC_ReDrawIcon)
        TSC_ReDrawIcon(key_num, 1);
    }
  }
  else
  {
    if (firstPress == false)
    {
      if (TSC_ReDrawIcon)
        TSC_ReDrawIcon(key_num, 0);
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

typedef enum
{
  NO_CLICK,
  FIRST_CLICK,
  FIRST_RELEASE,
  SECOND_CLICK,
  LONG_PRESS,
} KEY_STATUS;

#define KEY_DOUOBLE_SPACE        15   //锟洁长时锟斤拷锟节碉拷锟斤拷锟斤拷锟斤拷卸锟轿?双锟斤拷
#define KEY_LONG_PRESS_START     200  //锟斤拷锟斤拷锟斤拷锟斤拷每锟绞硷拷卸锟轿? 锟斤拷锟斤拷 锟斤拷值

#define KEY_LONG_PRESS_SPACE_MAX 10   //锟斤拷锟斤拷时 锟筋长锟斤拷梅锟斤拷锟揭伙拷渭锟街?
#define KEY_LONG_PRESS_SPACE_MIN 2    //����ʱ ��̶�÷���һ�μ�ֵ

//uint16_t KEY_GetValue(uint8_t total_rect, const GUI_RECT *menuRect)
//{
//  uint16_t key_return = NO_TOUCH;

//  static uint16_t first_key = NO_TOUCH;
//  static uint32_t first_time = 0;
//  static uint8_t long_press_space = KEY_LONG_PRESS_SPACE_MAX;

//  static KEY_STATUS nowStatus = NO_CLICK;    //������ǰ��״̬

//  if (touchScreenIsPress)
//  {
//    switch (nowStatus)
//    {
//      case NO_CLICK:
//        nowStatus = FIRST_CLICK;
//        first_key = Key_value(total_rect, menuRect);
//        first_time = OS_GetTime();
//        break;

//      case FIRST_CLICK:
//        if (OS_GetTime() - first_time > KEY_LONG_PRESS_START)
//        {
//          nowStatus = LONG_PRESS;
//          first_key |= KEY_LONG_CLICK;
//        }
//        break;

//      case FIRST_RELEASE:
//        if (first_key == Key_value(total_rect, menuRect))
//        {
//          nowStatus = SECOND_CLICK;
//          first_key |= KEY_DOUBLE_CLICK;
//        }
//        else
//        {
//          nowStatus = NO_CLICK;
//        }
//        break;

//      case SECOND_CLICK:
//        if (OS_GetTime() - first_time > KEY_LONG_PRESS_START)
//        {
//          nowStatus = LONG_PRESS;
//          first_key |= KEY_LONG_CLICK;
//        }
//        break;

//      case LONG_PRESS:
//        if (OS_GetTime() - first_time > long_press_space)
//        {
//          if (long_press_space > KEY_LONG_PRESS_SPACE_MIN)
//            long_press_space--;
//          first_time = OS_GetTime();
//          key_return = first_key;
//        }
//        break;

//      default:
//        break;
//    }
//  }
//  else
//  {
//    switch (nowStatus)
//    {
//      case FIRST_CLICK:
//        nowStatus = FIRST_RELEASE;
//        break;

//      case FIRST_RELEASE:
//        if (OS_GetTime() - first_time > KEY_DOUOBLE_SPACE)
//        {
//        nowStatus = NO_CLICK;
//        key_return = first_key;
//        }
//        break;

//      case SECOND_CLICK:
//        nowStatus = NO_CLICK;
//        key_return = first_key;
//        break;

//      case LONG_PRESS:
//        nowStatus = NO_CLICK;
//        key_return = first_key | KEY_LONG_RELEASE;
//        break;

//      default:
//        break;
//    }
//  }
//  return key_return;
//}

uint16_t KNOB_GetRV(GUI_RECT *knob)
{
  uint16_t key_return = IDLE_TOUCH;
  uint16_t x = 0, y = 0;

  static uint16_t oldx = 0, oldy = 0;
  static uint32_t mytime;

  if (touchScreenIsPress && OS_GetTimeMs() > mytime)
  {
    mytime = OS_GetTimeMs() + 10;
    TS_Get_Coordinates(&x, &y);
    if (x > knob->x0 && x < knob->x1 && y > knob->y0 && y < knob->y1)
    {
      if (x > oldx + 5)
      {
        if (oldy > (knob->y0 + knob->y1) / 2)
        {
          key_return = KNOB_DEC;
        }
        else
        {
          key_return = KNOB_INC;
        }
      }
      else if (x < oldx - 5)
      {
        if (oldy > (knob->y0 + knob->y1) / 2)
        {
          key_return = KNOB_INC;
        }
        else
        {
          key_return = KNOB_DEC;
        }
      }
      if (y > oldy + 5)
      {
        if (x > (knob->x0 + knob->x1) / 2)
        {
          key_return = KNOB_INC;
        }
        else
        {
          key_return = KNOB_DEC;
        }
      }
      else if (y < oldy - 5)
      {
        if (x > (knob->x0 + knob->x1) / 2)
        {
          key_return = KNOB_DEC;
        }
        else
        {
          key_return = KNOB_INC;
        }
      }
    }
  }
  if (key_return != IDLE_TOUCH)
  {
    oldx = x;
    oldy = y;
  }
  return key_return;
}
