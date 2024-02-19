#include "Touch_Encoder.h"
#include "includes.h"

bool Touch_Enc_ReadPen(uint16_t duration)
{
  static uint32_t lastTime = 0;

  if (XPT2046_Read_Pen())  // if touch screen not pressed
  {
    lastTime = OS_GetTimeMs();

    return false;
  }

  if (OS_GetTimeMs() - lastTime < duration)  // if touch screen held pressed but provided duration not yet reached
    return false;

  // touch screen held pressed for the provided duration

  lastTime = OS_GetTimeMs();

  return true;
}

#if LCD_ENCODER_SUPPORT

#define LCD_FREE_WIDTH  (LCD_WIDTH - LCD_WIDTH / 5)
#define LCD_FREE_HEIGHT (LCD_HEIGHT / 5)

bool Touch_Enc_ReadBtn(uint16_t duration)
{
  static uint32_t lastTime = 0;

  if (XPT2046_Read_Pen())  // if touch screen not pressed
  {
    lastTime = OS_GetTimeMs();

    return false;
  }

  if (OS_GetTimeMs() - lastTime < duration)  // if touch screen held pressed but provided duration not yet reached
    return false;

  // touch screen held pressed for the provided duration

  uint16_t tx, ty;

  TS_GetCoordinates(&tx, &ty);

  if (tx > LCD_FREE_WIDTH && ty < LCD_FREE_HEIGHT)
    return true;

  return false;
}

uint8_t Touch_Enc_ReadPos(void)
{
  static bool move = false;
  static uint16_t sy = 0;

  if (XPT2046_Read_Pen())  // if touch screen not pressed
  {
    move = false;
    sy = 0;
    modeSwitching = false;  // resume mode switching

    return 0;
  }

  uint16_t ex, ey;

  ex = ey = 0;

  TS_GetCoordinates(&ex, &ey);

  if (!move)
    sy = ey;

  move = true;

  if (ex > LCD_FREE_WIDTH)  // if touched navigation area, stop mode switching
    modeSwitching = true;
  else
    modeSwitching = false;

  if (ex > LCD_FREE_WIDTH)
  {
    if (sy > ey && ey != 0)
    {
      if (sy - ey > LCD_HEIGHT / 9 && sy - ey < LCD_HEIGHT / 7)  // 7 - 5
      {
        sy = ey;

        return 2;
      }
    }
    else
    {
      if (ey - sy > LCD_HEIGHT / 9 && ey - sy < LCD_HEIGHT / 7)
      {
        sy = ey;

        return 3;
      }
    }
  }

  return 0;
}

#endif  // LCD_ENCODER_SUPPORT
