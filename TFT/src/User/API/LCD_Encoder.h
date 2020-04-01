#ifndef _LCD_ENCODER_H_
#define _LCD_ENCODER_H_

#include "stdbool.h"
#include "stdint.h"

#define ENCODER_PULSES_PER_STEP   4
#define LCD_CHANGE_MODE_INTERVALS 1500 //long pressed 1.5s
#define LCD_BUTTON_INTERVALS      20   //20ms

#define EN_A (1<<0)
#define EN_B (1<<1)
#define EN_C (1<<2)
#define B01  1
#define B10  2

extern int16_t encoderPosition;


void LCD_EncoderInit(void);
bool LCD_ReadBtn(uint16_t intervals);
void LCD_LoopEncoder(void);
void loopCheckMode(void);
uint8_t LCD_GetEncoderButton(void);
bool LCD_CheckEncoderState(void);
void LCD_loopCheckEncoder(void);

#endif
