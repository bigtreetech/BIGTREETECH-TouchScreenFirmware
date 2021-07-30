#ifndef _TOUCH_ENCODER_H_
#define _TOUCH_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <variants.h>

bool Touch_Enc_ReadPen(uint16_t interval);  // return the button press state resetting internal timer when reached. Interval is in milli seconds

#if LCD_ENCODER_SUPPORT
  bool Touch_Enc_ReadBtn(uint16_t interval);  // return the button press state to send to encoder. Interval is in milli seconds
  uint8_t Touch_Enc_ReadPos(void);            // return the position to send to encoder
#endif

#ifdef __cplusplus
}
#endif

#endif
