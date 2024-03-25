#ifndef _ST7789_H_
#define _ST7789_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t LCD_DriveIsST7789(void);
void ST7789_Init_Sequential(void);
void ST7789_SetDirection(uint8_t rotate);
void ST7789_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t ST7789_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
