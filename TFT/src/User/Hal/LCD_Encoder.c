#include "LCD_Encoder.h"
#include "includes.h"
#include "GPIO_Init.h"

#if ENC_ACTIVE_SIGNAL

void LCD_Enc_InitActiveSignal(bool marlinType_LCD12864)
{
  if (!marlinType_LCD12864)
    return;

  GPIO_InitSet(LCD_ENC_EN_PIN, MGPIO_MODE_OUT_PP, 0);
  LCD_Enc_SetActiveSignal(true, 0);
}

void LCD_Enc_SetActiveSignal(bool marlinType_LCD12864, uint8_t status)
{
  if (!marlinType_LCD12864)
    return;

  GPIO_SetLevel(LCD_ENC_EN_PIN, status);
}

#endif  // ENC_ACTIVE_SIGNAL

#if LCD_ENCODER_SUPPORT

#define EN_A (1<<0)
#define EN_B (1<<1)
#define EN_C (1<<2)
#define B01  1
#define B10  2

static uint8_t encoderLastState = 0;
static uint8_t encoderLastSteps = 0;
volatile int8_t encoderDiff;  // updated in LCD_Enc_CheckSteps(), added to encoderPosition at every LCD update
int8_t encoderDirection = 1;
int16_t encoderPosition = 0;

bool LCD_Enc_ReadStep(uint16_t io_pin)
{
  return !GPIO_GetLevel(io_pin);
}

void LCD_Enc_Init(void)
{
  uint16_t encPin[] = {LCD_ENCA_PIN, LCD_ENCB_PIN, LCD_BTN_PIN};

  for (uint8_t i = 0; i < COUNT(encPin); i++)
  {
    GPIO_InitSet(encPin[i], MGPIO_MODE_IPU, 0);
  }

  encoderLastState = encoderLastSteps = LCD_Enc_ReadPos();
}

bool LCD_Enc_ReadBtn(uint16_t interval)
{
  static uint32_t nowTime = 0;

  if (!GPIO_GetLevel(LCD_BTN_PIN))
  {
    if (OS_GetTimeMs() - nowTime >= interval)
      return true;
  }
  else
  {
    nowTime = OS_GetTimeMs();
  }

  return false;
}

uint8_t LCD_Enc_ReadPos(void)
{
  uint8_t pos = 0;

  if (LCD_Enc_ReadStep(LCD_ENCA_PIN))
    pos |= EN_A;

  if (LCD_Enc_ReadStep(LCD_ENCB_PIN))
    pos |= EN_B;

  return pos;
}

void LCD_Enc_SendPulse(uint8_t num)
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

  LCD_Enc_Init();
}

bool LCD_Enc_CheckState()
{
  if (LCD_Enc_ReadBtn(LCD_ENC_BUTTON_INTERVAL) || LCD_Enc_ReadPos() != encoderLastState)
  {
    encoderLastState = LCD_Enc_ReadPos();

    return true;
  }

  return false;
}

void LCD_Enc_CheckSteps(void)
{
  uint8_t pos = LCD_Enc_ReadPos();

  #define encrot0 0
  #define encrot1 2
  #define encrot2 3
  #define encrot3 1

  // manage encoder rotation
  #define ENCODER_SPIN(_E1, _E2) switch (encoderLastSteps) { case _E1: encoderDiff += encoderDirection; break; \
                                                             case _E2: encoderDiff -= encoderDirection; }

  if (pos != encoderLastSteps)
  {
    switch (pos)
    {
      case encrot0: ENCODER_SPIN(encrot3, encrot1); break;
      case encrot1: ENCODER_SPIN(encrot0, encrot2); break;
      case encrot2: ENCODER_SPIN(encrot1, encrot3); break;
      case encrot3: ENCODER_SPIN(encrot2, encrot0); break;
    }

    encoderLastSteps = pos;
  }

  if (ABS(encoderDiff) >= LCD_ENC_PULSES_PER_STEP)
  {
    encoderPosition += encoderDiff / (LCD_ENC_PULSES_PER_STEP);
    encoderDiff = 0;
  }
}

#endif  // LCD_ENCODER_SUPPORT
