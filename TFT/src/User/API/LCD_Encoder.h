#ifndef _LCD_ENCODER_H_
#define _LCD_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <variants.h>

#if ENC_ACTIVE_SIGNAL
  void LCD_Enc_InitActiveSignal(void);
  void LCD_Enc_SetActiveSignal(uint8_t status);
#endif

#if LCD_ENCODER_SUPPORT
  #define LCD_ENC_PULSES_PER_STEP  4
  #define LCD_ENC_BUTTON_INTERVAL 20  // 20ms

  extern int16_t encoderPosition;

  void LCD_Enc_Init(void);
  bool LCD_Enc_ReadBtn(uint16_t interval);  // return the button press state. Interval is in milli seconds
  uint8_t LCD_Enc_ReadPos(void);            // return the position pins state
  void LCD_Enc_SendPulse(uint8_t num);      // send a pulse to the encoder
  bool LCD_Enc_CheckState(void);
  void LCD_Enc_CheckSteps(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
