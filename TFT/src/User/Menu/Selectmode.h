#ifndef _SELECTMODE_H_
#define _SELECTMODE_H_

#include "GUI.h"
#include "stdint.h"
#include "stdbool.h"

#define IDLE_TOUCH	0xFFFF
#define SELECTMODE 2
typedef enum
{
  MKEY_0 = 0,
  MKEY_1,
  MKEY_IDLE = IDLE_TOUCH,
}MKEY_VALUES;

bool LCD_ReadPen(uint16_t intervals);
uint8_t LCD_ReadTouch(void);
void Touch_Sw(uint8_t num);
bool LCD_BtnTouch(uint16_t intervals);

//select mode fun
void show_selectICON(void);
MKEY_VALUES MKeyGetValue(void);
void selectmode(int8_t  nowMode);

#endif
