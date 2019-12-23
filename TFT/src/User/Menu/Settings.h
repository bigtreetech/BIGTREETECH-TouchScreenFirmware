#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "stdint.h"

typedef enum
{
  SERIAL_TSC = 0,
  LCD12864,
  LCD2004  
}LCD_MODE;

typedef struct
{
  uint32_t baudrate;
  uint8_t  language;
  uint8_t  mode;
  uint8_t  runout;
  uint8_t  rotate_ui;
  uint16_t bg_color;
  uint16_t font_color;
  uint8_t  silent;
  uint8_t  auto_off;
  uint8_t  terminalACK;
  uint8_t  invert_yaxis;
  uint8_t  move_speed;
}SETTINGS;


extern SETTINGS infoSettings;

void infoSettingsReset(void);
void menuSettings(void);

#endif
