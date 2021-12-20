#ifndef _SSD1963_H_
#define _SSD1963_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SSD1963_0_DEGREE_REG_VALUE   0x00
#define SSD1963_180_DEGREE_REG_VALUE 0x03
#define SSD1963_90_DEGREE_REG_VALUE  0x02  // TO BE VALIDATED
#define SSD1963_270_DEGREE_REG_VALUE 0x05  // TO BE VALIDATED

uint8_t LCD_DriveIsSSD1963(void);
void SSD1963_Init_Sequential(void);
void SSD1963_SetDirection(uint8_t rotate);
void SSD1963_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
uint32_t SSD1963_ReadPixel_24Bit(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
