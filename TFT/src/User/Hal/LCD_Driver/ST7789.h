#ifndef _ST7789_H_
#define _ST7789_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ST7789_0_DEGREE_REG_VALUE   0X60
#define ST7789_180_DEGREE_REG_VALUE 0XA0
#define ST7789_90_DEGREE_REG_VALUE  0XC0
#define ST7789_270_DEGREE_REG_VALUE 0X00

uint8_t LCD_DriveIsST7789(void);
void ST7789_Init_Sequential(void);
void ST7789_SetDirection(uint8_t rotate);
void ST7789_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t ST7789_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
