#include "GPIO_Init.h"
#include "includes.h"
#include "LCD_Encoder.h"

#if LCD_ENCODER_SUPPORT

int8_t encoderDirection = 1;
volatile int8_t encoderDiff;  // Updated in update_buttons, added to encoderPosition every LCD update
int16_t encoderPosition = 0;
uint8_t buttons = 0;
uint8_t _encLastBtn = 0;

void HW_EncoderInit(void)
{
  uint16_t encPin[] = {LCD_ENCA_PIN, LCD_ENCB_PIN, LCD_BTN_PIN};

  for (uint8_t i = 0; i < COUNT(encPin); i++)
  {
    GPIO_InitSet(encPin[i], MGPIO_MODE_IPU, 0);
  }
  _encLastBtn = encoder_GetPos();
}

#if ENC_ACTIVE_SIGNAL
  void HW_EncActiveSignalInit(void)
  {
    if (infoSettings.marlin_type != LCD12864)
      return;
    GPIO_InitSet(LCD_ENC_EN_PIN, MGPIO_MODE_OUT_PP, 0);
    setEncActiveSignal(0);
  }

  void setEncActiveSignal(uint8_t status)
  {
    if (infoSettings.marlin_type != LCD12864)
      return;
    GPIO_SetLevel(LCD_ENC_EN_PIN, status);
  }
#endif

bool encoder_ReadStep(uint16_t io_pin)
{
  return !GPIO_GetLevel(io_pin);
}

// read hardware encoder button for select btn press
bool encoder_ReadBtn(uint16_t intervals)
{
  static uint32_t nowTime = 0;

  if (!GPIO_GetLevel(LCD_BTN_PIN))
  {
    if (OS_GetTimeMs() - nowTime > intervals)
    {
      return true;
    }
  }
  else
  {
    nowTime = OS_GetTimeMs();
  }
  return false;
}

//check touch to send select button
bool LCD_BtnTouch(uint16_t intervals)
{
  static uint32_t BtnTime = 0;
  uint16_t tx, ty;
  if (!XPT2046_Read_Pen())
  {
    TS_Get_Coordinates(&tx, &ty);
    if (OS_GetTimeMs() - BtnTime > intervals)
    {
      if (tx > LCD_FREE_WIDTH && ty < LCD_FREE_HEIGHT)
        return true;
    }
  }
  else
  {
    BtnTime = OS_GetTimeMs();
  }
  return false;
}

//return hardware encoder pin states
uint8_t encoder_GetPos(void)
{
  uint8_t newbutton = 0;
  if (encoder_ReadStep(LCD_ENCA_PIN)) newbutton |= EN_A;
  if (encoder_ReadStep(LCD_ENCB_PIN)) newbutton |= EN_B;

  return newbutton;
}

bool encoder_CheckState()
{
 if (encoder_ReadBtn(LCD_BUTTON_INTERVALS) || _encLastBtn != encoder_GetPos())
  {
    _encLastBtn = encoder_GetPos();
    return true;
  } else return false;
}

void loopCheckEncoderSteps(void)
{
  static uint8_t lastEncoderBits = 0;
  buttons = encoder_GetPos();

  #define encrot0 0
  #define encrot1 2
  #define encrot2 3
  #define encrot3 1

  // Manage encoder rotation
  #define ENCODER_SPIN(_E1, _E2) switch (lastEncoderBits) { case _E1: encoderDiff += encoderDirection; break; \
                                                            case _E2: encoderDiff -= encoderDirection; }

  if (buttons != lastEncoderBits)
  {
    switch (buttons)
    {
      case encrot0: ENCODER_SPIN(encrot3, encrot1); break;
      case encrot1: ENCODER_SPIN(encrot0, encrot2); break;
      case encrot2: ENCODER_SPIN(encrot1, encrot3); break;
      case encrot3: ENCODER_SPIN(encrot2, encrot0); break;
    }
    lastEncoderBits = buttons;
  }

  const float abs_diff = ABS(encoderDiff);
  const bool encoderPastThreshold = (abs_diff >= (ENCODER_PULSES_PER_STEP));
  if (encoderPastThreshold)
  {
    encoderPosition += encoderDiff / (ENCODER_PULSES_PER_STEP);
    encoderDiff = 0;
  }
}

#if 1
//Parse the touch to control encoder
uint8_t LCD_ReadTouch(void)
{
  uint16_t ex = 0, ey = 0;
  static uint32_t CTime = 0;
  static uint16_t sy;
  static bool MOVE = false;

  if (!XPT2046_Read_Pen() && CTime < OS_GetTimeMs())
  {
    TS_Get_Coordinates(&ex, &ey);
    if (!MOVE)
      sy = ey;

    if (ex > LCD_FREE_WIDTH)  //stop mode switch if touched in navigation area
      skipMode = true;
    else
      skipMode = false;

    MOVE = true;
    if (ex > LCD_FREE_WIDTH)
    {
      if ((sy > ey) && ey != 0)
      {
        if (sy - ey > LCD_HEIGHT / 9 && sy - ey < LCD_HEIGHT / 7)  //7 - 5
        {
          sy = ey;
          return 2;
        }
        //return 0;
      }
      else
      {
        if (ey - sy > LCD_HEIGHT / 9 && ey - sy < LCD_HEIGHT / 7)
        {
          sy = ey;
          return 3;
        }
        //return 0;
      }
    }
  }
  else
  {
    CTime = OS_GetTimeMs();
    sy = ey = 0;
    MOVE = false;
    skipMode = false; //resume mode change loop
    //return 0;
  }
  return 0;
}
#endif

//Send encoder pulse
void sendEncoder(uint8_t num)
{
  if (num == 1 || num == 2 || num == 3)
  {
    GPIO_InitSet(LCD_BTN_PIN, MGPIO_MODE_OUT_PP, 0);
    GPIO_InitSet(LCD_ENCA_PIN, MGPIO_MODE_OUT_PP, 0);
    GPIO_InitSet(LCD_ENCB_PIN, MGPIO_MODE_OUT_PP, 0);
  }
  switch (num)
  {
    case 0:
      break;

    case 1:
      GPIO_SetLevel(LCD_BTN_PIN, 0);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_BTN_PIN, 1);
      break;

    case 2:
      GPIO_SetLevel(LCD_ENCA_PIN, 1);
      GPIO_SetLevel(LCD_ENCB_PIN, 1);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 0);
      GPIO_SetLevel(LCD_ENCB_PIN, 1);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 0);
      GPIO_SetLevel(LCD_ENCB_PIN, 0);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 1);
      GPIO_SetLevel(LCD_ENCB_PIN, 0);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 1);
      GPIO_SetLevel(LCD_ENCB_PIN, 1);
      break;

    case 3:
      GPIO_SetLevel(LCD_ENCA_PIN, 1);
      GPIO_SetLevel(LCD_ENCB_PIN, 1);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 1);
      GPIO_SetLevel(LCD_ENCB_PIN, 0);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 0);
      GPIO_SetLevel(LCD_ENCB_PIN, 0);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 0);
      GPIO_SetLevel(LCD_ENCB_PIN, 1);
      Delay_us(LCD_ENCODER_DELAY);
      GPIO_SetLevel(LCD_ENCA_PIN, 1);
      GPIO_SetLevel(LCD_ENCB_PIN, 1);
      break;
  }

  HW_EncoderInit();
}

#endif
