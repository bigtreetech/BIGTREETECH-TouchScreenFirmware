#ifndef _ST7796S_H_
#define _ST7796S_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ST7796S_0_DEGREE_REG_VALUE   0X28
#define ST7796S_180_DEGREE_REG_VALUE 0XE8
#define ST7796S_90_DEGREE_REG_VALUE  0X88
#define ST7796S_270_DEGREE_REG_VALUE 0X48

uint8_t LCD_DriveIsST7796S(void);
void ST7796S_Init_Sequential(void);
void ST7796S_SetDirection(uint8_t rotate);
void ST7796S_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t ST7796S_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
