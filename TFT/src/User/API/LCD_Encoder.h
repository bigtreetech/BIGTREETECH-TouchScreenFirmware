#ifndef _LCD_ENCODER_H_
#define _LCD_ENCODER_H_

#include "stdbool.h"
#include "stdint.h"
#define ENCODER_PULSES_PER_STEP   4
#define LCD_CHANGE_MODE_INTERVALS 1500 //long pressed 1.5s
#define LCD_BUTTON_INTERVALS      20   //20ms
#define LCD_FREE_WIDTH         (LCD_WIDTH - LCD_WIDTH / 5)
#define LCD_FREE_HEIGHT        (LCD_HEIGHT / 5)
#define EN_A (1<<0)
#define EN_B (1<<1)
#define EN_C (1<<2)
#define B01  1
#define B10  2

extern int16_t encoderPosition;


void HW_EncoderInit(void);
bool encoder_ReadBtn(uint16_t intervals);
bool LCD_ReadPen(uint16_t intervals);
bool LCD_BtnTouch(uint16_t intervals);
uint8_t LCD_ReadTouch(void);
void loopCheckEncoderSteps(void);
uint8_t encoder_GetPos(void);
bool encoder_CheckState(void);
void sendEncoder(uint8_t num);
void loopCheckEncoder(void);

#endif