#ifndef _LCD_ENCODER_H_
#define _LCD_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"  // for ENC_ACTIVE_SIGNAL, LCD_ENCODER_SUPPORT etc...
#include "menu.h"

#if ENC_ACTIVE_SIGNAL
  void LCD_Enc_InitActiveSignal(bool marlinType_LCD12864);
  void LCD_Enc_SetActiveSignal(bool marlinType_LCD12864, uint8_t status);
#endif

#if LCD_ENCODER_SUPPORT
  extern int16_t encoderPosition;  // make it available for reading the current rotation value

  void LCD_Enc_Init(void);
  bool LCD_Enc_ReadBtn(uint16_t interval);  // return the button press state. Interval is in milli seconds
  uint8_t LCD_Enc_ReadPos(void);            // return the position pins state
  void LCD_Enc_SendPulse(uint8_t num);      // send a pulse to the encoder
  bool LCD_Enc_CheckState(void);
  void LCD_Enc_CheckSteps(void);
  KEY_VALUES LCD_Enc_KeyValue(void);  // return a value from provided variables based on encoder position
#endif

#ifdef __cplusplus
}
#endif

#endif
