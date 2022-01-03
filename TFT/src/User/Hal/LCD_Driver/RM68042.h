#ifndef _RM68042_H_
#define _RM68042_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RM68042_0_DEGREE_REG_VALUE   0X28
#define RM68042_180_DEGREE_REG_VALUE 0X2B
#define RM68042_90_DEGREE_REG_VALUE  0x09
#define RM68042_270_DEGREE_REG_VALUE 0x0A

uint8_t LCD_DriveIsRM68042(void);
void RM68042_Init_Sequential(void);
void RM68042_SetDirection(uint8_t rotate);
void RM68042_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t RM68042_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
