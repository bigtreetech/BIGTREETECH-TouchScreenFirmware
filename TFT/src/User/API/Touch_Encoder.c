#include "Touch_Encoder.h"
#include "includes.h"

bool Touch_Enc_ReadPen(uint16_t interval)
{
  static uint32_t nowTime = 0;

  if (!XPT2046_Read_Pen())
  {
    if (OS_GetTimeMs() - nowTime >= interval)
    {
      nowTime = OS_GetTimeMs();

      return true;
    }
  }
  else
  {
    nowTime = OS_GetTimeMs();
  }

  return false;
}

#if LCD_ENCODER_SUPPORT

#define LCD_FREE_WIDTH  (LCD_WIDTH - LCD_WIDTH / 5)
#define LCD_FREE_HEIGHT (LCD_HEIGHT / 5)

bool Touch_Enc_ReadBtn(uint16_t interval)
{
  static uint32_t nowTime = 0;
  uint16_t tx, ty;

  if (!XPT2046_Read_Pen())
  {
    TS_Get_Coordinates(&tx, &ty);

    if (OS_GetTimeMs() - nowTime >= interval)
    {
      if (tx > LCD_FREE_WIDTH && ty < LCD_FREE_HEIGHT)
        return true;
    }
  }
  else
  {
    nowTime = OS_GetTimeMs();
  }

  return false;
}

uint8_t Touch_Enc_ReadPos(void)
{
  static bool move = false;
  static uint16_t sy;
  uint16_t ex, ey;

  ex = ey = 0;

  if (!XPT2046_Read_Pen())
  {
    TS_Get_Coordinates(&ex, &ey);

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
  }
  else
  {
    move = false;
    sy = ey = 0;
    modeSwitching = false;  // resume mode switching
  }

  return 0;
}

#endif  // LCD_ENCODER_SUPPORT
