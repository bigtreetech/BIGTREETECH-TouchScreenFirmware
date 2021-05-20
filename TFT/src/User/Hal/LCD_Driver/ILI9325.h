#ifndef _ILI9325_H_
#define _ILI9325_H_

#ifdef __cplusplus
extern "C" {
#endif

uint8_t LCD_DriveIsILI9325(void);
void ILI9325_Init_Sequential(void);
void ILI9325_SetDirection(uint8_t rotate);
void ILI9325_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t ILI9325_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
