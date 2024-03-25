#ifndef _ILI9341_H_
#define _ILI9341_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t LCD_DriveIsILI9341(void);
void ILI9341_Init_Sequential(void);
void ILI9341_SetDirection(uint8_t rotate);
void ILI9341_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t ILI9341_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
