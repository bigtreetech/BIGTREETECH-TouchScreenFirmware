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
  uint32_t runout;
  uint8_t  language;
  uint8_t  mode;
}SETTINGS;


extern SETTINGS infoSettings;

void menuSettings(void);

#endif
