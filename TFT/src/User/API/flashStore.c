#include "flashStore.h"
#include "STM32_Flash.h"

#define PARA_SIZE 256  //bytes
#define TSC_SIGN  0x20190827 // DO NOT MODIFY
#define PARA_SIGN 0x20200313 // If a new setting parameter is added, modify here and initialize the initial value in the "infoSettingsReset()" function

extern u32 TSC_Para[7];        //
extern SETTINGS infoSettings;  //

void wordToByte(u32 word, u8 *bytes)  //
{
  u8 len = 4;
  u8 i = 0;
  for(i = 0; i < len; i++)
  {
    bytes[i] = (word >> 24) & 0xFF;
    word <<= 8;
  }
}

u32 byteToWord(u8 *bytes, u8 len)
{
  u32 word = 0;
  u8 i = 0;
  for(i = 0; i < len; i++)
  {
    word <<= 8;
    word |= bytes[i];
  }
  return word;
}

// Read settings parameter if exist, or reset settings parameter
// return value: whether the touch screen calibration parameter exists
bool readStoredPara(void)
{
  bool paraExist = true;
  u8 data[PARA_SIZE];
  u32 index = 0;
  u32 sign = 0;
  STM32_FlashRead(data, PARA_SIZE);

  sign = byteToWord(data + (index += 4), 4);
  if(sign != TSC_SIGN) paraExist = false;    // If the touch screen calibration parameter does not exist
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    TSC_Para[i] = byteToWord(data + (index += 4), 4);
  }

  sign = byteToWord(data + (index += 4), 4);
  if(sign != PARA_SIGN) // If the settings parameter is illegal, reset settings parameter
  {
    infoSettingsReset();
  }
  else
  {
    infoSettings.baudrate            = byteToWord(data + (index += 4), 4);
    infoSettings.language            = byteToWord(data + (index += 4), 4);
    infoSettings.mode                = byteToWord(data + (index += 4), 4);
    infoSettings.runout              = byteToWord(data + (index += 4), 4);
    infoSettings.rotate_ui           = byteToWord(data + (index += 4), 4);
    infoSettings.bg_color            = byteToWord(data + (index += 4), 4);
    infoSettings.font_color          = byteToWord(data + (index += 4), 4);
    infoSettings.silent              = byteToWord(data + (index += 4), 4);
    infoSettings.auto_off            = byteToWord(data + (index += 4), 4);
    infoSettings.terminalACK         = byteToWord(data + (index += 4), 4);
    infoSettings.invert_axis[X_AXIS] = byteToWord(data + (index += 4), 4);
    infoSettings.invert_axis[Y_AXIS] = byteToWord(data + (index += 4), 4);
    infoSettings.invert_axis[Z_AXIS] = byteToWord(data + (index += 4), 4);
    infoSettings.move_speed          = byteToWord(data + (index += 4), 4);
    infoSettings.send_start_gcode    = byteToWord(data + (index += 4), 4);
    infoSettings.send_end_gcode      = byteToWord(data + (index += 4), 4);
    infoSettings.persistent_info     = byteToWord(data + (index += 4), 4);
    infoSettings.file_listmode       = byteToWord(data + (index += 4), 4);
    infoSettings.knob_led_color      = byteToWord(data + (index += 4), 4);
  }

  return paraExist;
}

void storePara(void)
{
  u8 data[PARA_SIZE];
  u32 index = 0;

  wordToByte(TSC_SIGN, data + (index += 4));
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    wordToByte(TSC_Para[i], data + (index += 4));
  }
  wordToByte(PARA_SIGN, data + (index += 4));
  wordToByte(infoSettings.baudrate,           data + (index += 4));
  wordToByte(infoSettings.language,           data + (index += 4));
  wordToByte(infoSettings.mode,               data + (index += 4));
  wordToByte(infoSettings.runout,             data + (index += 4));
  wordToByte(infoSettings.rotate_ui,          data + (index += 4));
  wordToByte(infoSettings.bg_color,           data + (index += 4));
  wordToByte(infoSettings.font_color,         data + (index += 4));
  wordToByte(infoSettings.silent,             data + (index += 4));
  wordToByte(infoSettings.auto_off,           data + (index += 4));
  wordToByte(infoSettings.terminalACK,        data + (index += 4));
  wordToByte(infoSettings.invert_axis[X_AXIS],data + (index += 4));
  wordToByte(infoSettings.invert_axis[Y_AXIS],data + (index += 4));
  wordToByte(infoSettings.invert_axis[Z_AXIS],data + (index += 4));
  wordToByte(infoSettings.move_speed,         data + (index += 4));
  wordToByte(infoSettings.send_start_gcode,   data + (index += 4));
  wordToByte(infoSettings.send_end_gcode,     data + (index += 4));
  wordToByte(infoSettings.persistent_info,    data + (index += 4));
  wordToByte(infoSettings.file_listmode,      data + (index += 4));
  wordToByte(infoSettings.knob_led_color,     data + (index += 4));

  STM32_FlashWrite(data, PARA_SIZE);
}
