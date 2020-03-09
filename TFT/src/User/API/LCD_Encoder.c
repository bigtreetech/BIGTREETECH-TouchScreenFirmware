#include "LCD_Encoder.h"
#include "GPIO_Init.h"
#include "includes.h"

#if LCD_ENCODER_SUPPORT

int8_t encoderDirection = 1;
volatile int8_t encoderDiff; // Updated in update_buttons, added to encoderPosition every LCD update
int16_t encoderPosition = 0;
uint8_t buttons = 0;

void LCD_EncoderInit(void)
{
  uint16_t encPin[]  = {LCD_ENCA_PIN,  LCD_ENCB_PIN,  LCD_BTN_PIN};

  for(u8 i = 0; i < COUNT(encPin); i++)
  {
    GPIO_InitSet(encPin[i], MGPIO_MODE_IPU, 0);
  }
}

bool LCD_ReadEncA(void)
{
  return !GPIO_GetLevel(LCD_ENCA_PIN);
}

bool LCD_ReadEncB(void)
{
  return !GPIO_GetLevel(LCD_ENCB_PIN);
}

bool LCD_ReadBtn(uint8_t intervals)
{
  static u32 nowTime = 0;

  if(!GPIO_GetLevel(LCD_BTN_PIN))
  {
    if(OS_GetTime() - nowTime > intervals)
    {
      return true;
    }
  }
  else
  {
    nowTime = OS_GetTime();
  }
  return false;
}

void LCD_LoopEncoder(void)
{
  static uint8_t lastEncoderBits = 0;
  uint8_t newbutton = 0;

  if(LCD_ReadEncA()) newbutton |= EN_A;
  if(LCD_ReadEncB()) newbutton |= EN_B;

  buttons = newbutton;

  #define encrot0 0
  #define encrot1 2
  #define encrot2 3
  #define encrot3 1

  // Manage encoder rotation
  #define ENCODER_SPIN(_E1, _E2) switch (lastEncoderBits) { case _E1: encoderDiff += encoderDirection; break; case _E2: encoderDiff -= encoderDirection; }

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

void loopCheckMode(void)
{
//  #ifndef CLEAN_MODE_SWITCHING_SUPPORT
//  IDEALLY I would like to be able to swap even when the TFT is in printing mode 
//  but before I can allow that I need a way to make sure that we swap back into the right mode (and correct screen)
//  and I really want a reliable way to DETECT that the TFT should be in printing mode even when the print was started externally.
    if(isPrinting()) return;
//  #endif
  if(LCD_ReadBtn(LCD_CHANGE_MODE_INTERVALS) || LCD_ReadPen(LCD_CHANGE_MODE_INTERVALS))
  {
    infoMenu.menu[++infoMenu.cur] = menuMode;
  }
}
#endif
