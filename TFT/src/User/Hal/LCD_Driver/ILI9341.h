#ifndef _ILI9341_H_
#define _ILI9341_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ILI9341_0_DEGREE_REG_VALUE   0X68
#define ILI9341_180_DEGREE_REG_VALUE 0XA8
#define ILI9341_90_DEGREE_REG_VALUE  0XC8
#define ILI9341_270_DEGREE_REG_VALUE 0X8

uint8_t LCD_DriveIsILI9341(void);
void ILI9341_Init_Sequential(void);
void ILI9341_SetDirection(uint8_t rotate);
void ILI9341_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t ILI9341_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
