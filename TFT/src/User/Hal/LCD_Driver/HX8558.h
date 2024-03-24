#ifndef _HX8558_H_
#define _HX8558_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t LCD_DriveIsHX8558(void);
void HX8558_Init_Sequential(void);
void HX8558_SetDirection(uint8_t rotate);
void HX8558_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t HX8558_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
