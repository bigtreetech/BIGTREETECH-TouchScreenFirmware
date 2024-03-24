#ifndef _RM68042_H_
#define _RM68042_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t LCD_DriveIsRM68042(void);
void RM68042_Init_Sequential(void);
void RM68042_SetDirection(uint8_t rotate);
void RM68042_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t RM68042_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
