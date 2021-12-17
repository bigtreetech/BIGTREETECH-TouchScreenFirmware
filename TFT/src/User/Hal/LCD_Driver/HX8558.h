#ifndef _HX8558_H_
#define _HX8558_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HX8558_0_DEGREE_REG_VALUE   0xA4
#define HX8558_180_DEGREE_REG_VALUE 0X64
#define HX8558_90_DEGREE_REG_VALUE  0x04
#define HX8558_270_DEGREE_REG_VALUE 0xC4

uint8_t LCD_DriveIsHX8558(void);
void HX8558_Init_Sequential(void);
void HX8558_SetDirection(uint8_t rotate);
void HX8558_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t HX8558_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
