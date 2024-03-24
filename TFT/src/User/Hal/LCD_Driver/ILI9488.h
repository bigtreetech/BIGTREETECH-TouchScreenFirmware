#ifndef _ILI9488_H_
#define _ILI9488_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t LCD_DriveIsILI9488(void);
void ILI9488_Init_Sequential(void);
void ILI9488_SetDirection(uint8_t rotate);
void ILI9488_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t ILI9488_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
