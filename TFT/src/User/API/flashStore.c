#include "flashStore.h"
#include "STM32_Flash.h"

#define TSC_SIGN  0x20200512 // DO NOT MODIFY
#define PARA_SIGN 0x20201220 // (YYYYMMDD) If a new setting parameter is added,
                             // modify here and initialize the initial value
                             // in the "infoSettingsReset()" function


extern int32_t TSC_Para[7];
extern SETTINGS infoSettings;

enum{
 PARA_TSC_EXIST = (1 << 0),
 PARA_WAS_RESTORED = (1<< 1),
};
uint8_t paraStatus = 0;

void wordToByte(u32 word, u8 *bytes)
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
void readStoredPara(void)
{
  u8 data[PARA_SIZE];
  u32 index = 0;
  u32 sign = 0;
  STM32_FlashRead(data, PARA_SIZE);

  sign = byteToWord(data + (index += 4), 4);
  if(sign == TSC_SIGN)
  {
    paraStatus |= PARA_TSC_EXIST;    // If the touch screen calibration parameter already exists
    for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
    {
      TSC_Para[i] = byteToWord(data + (index += 4), 4);
    }
    infoSettings.rotate_ui            = byteToWord(data + (index += 4), 4); // rotate_ui and TSC_Para[] are bound together
  }
  else
  {
    infoSettings.rotate_ui            = DISABLED;
  }

  sign = byteToWord(data + (index += 4), 4);
  if(sign != PARA_SIGN) // If the settings parameter is illegal, reset settings parameter
  {
    paraStatus |= PARA_WAS_RESTORED;
    infoSettingsReset();
  }
  else
  {
  infoSettings.baudrate             = byteToWord(data + (index += 4), 4);
  infoSettings.language             = byteToWord(data + (index += 4), 4);
  infoSettings.mode                 = byteToWord(data + (index += 4), 4);
  infoSettings.status_screen        = byteToWord(data + (index += 4), 4);

  infoSettings.bg_color             = byteToWord(data + (index += 4), 4);
  infoSettings.font_color           = byteToWord(data + (index += 4), 4);
  infoSettings.title_bg_color       = byteToWord(data + (index += 4), 4);
  infoSettings.reminder_color       = byteToWord(data + (index += 4), 4);
  infoSettings.sd_reminder_color    = byteToWord(data + (index += 4), 4);
  infoSettings.status_xyz_bg_color  = byteToWord(data + (index += 4), 4);
  infoSettings.list_border_color    = byteToWord(data + (index += 4), 4);
  infoSettings.list_button_color    = byteToWord(data + (index += 4), 4);
  infoSettings.mesh_min_color       = byteToWord(data + (index += 4), 4);
  infoSettings.mesh_max_color       = byteToWord(data + (index += 4), 4);

  infoSettings.touchSound           = byteToWord(data + (index += 4), 4);
  infoSettings.toastSound           = byteToWord(data + (index += 4), 4);
  infoSettings.alertSound           = byteToWord(data + (index += 4), 4);

  infoSettings.terminalACK          = byteToWord(data + (index += 4), 4);
  infoSettings.move_speed           = byteToWord(data + (index += 4), 4);
  infoSettings.knob_led_color       = byteToWord(data + (index += 4), 4);
  infoSettings.knob_led_idle        = byteToWord(data + (index += 4), 4);
  infoSettings.persistent_info      = byteToWord(data + (index += 4), 4);
  infoSettings.file_listmode        = byteToWord(data + (index += 4), 4);
  infoSettings.ack_notification     = byteToWord(data + (index += 4), 4);

  infoSettings.lcd_brightness       = byteToWord(data + (index += 4), 4);
  infoSettings.lcd_idle_brightness  = byteToWord(data + (index += 4), 4);
  infoSettings.lcd_idle_timer       = byteToWord(data + (index += 4), 4);

  infoSettings.print_summary        = byteToWord(data + (index += 4), 4);

  infoSettings.serial_alwaysOn            = byteToWord(data + (index += 4), 4);
  infoSettings.marlin_mode_bg_color       = byteToWord(data + (index += 4), 4);
  infoSettings.marlin_mode_font_color     = byteToWord(data + (index += 4), 4);
  infoSettings.marlin_mode_showtitle      = byteToWord(data + (index += 4), 4);
  infoSettings.marlin_mode_fullscreen     = byteToWord(data + (index += 4), 4);
  infoSettings.marlin_type                = byteToWord(data + (index += 4), 4);

  infoSettings.send_start_gcode     = byteToWord(data + (index += 4), 4);
  infoSettings.send_end_gcode       = byteToWord(data + (index += 4), 4);
  infoSettings.send_cancel_gcode    = byteToWord(data + (index += 4), 4);
  infoSettings.auto_off             = byteToWord(data + (index += 4), 4);
  infoSettings.ps_active_high       = byteToWord(data + (index += 4), 4);
  infoSettings.auto_off_temp        = byteToWord(data + (index += 4), 4);

  infoSettings.runout               = byteToWord(data + (index += 4), 4);
  infoSettings.runout_invert        = byteToWord(data + (index += 4), 4);
  infoSettings.runout_noise_ms      = byteToWord(data + (index += 4), 4);
  infoSettings.runout_distance      = byteToWord(data + (index += 4), 4);

  infoSettings.powerloss_en         = byteToWord(data + (index += 4), 4);
  infoSettings.powerloss_home       = byteToWord(data + (index += 4), 4);
  infoSettings.powerloss_invert     = byteToWord(data + (index += 4), 4);
  infoSettings.powerloss_z_raise    = byteToWord(data + (index += 4), 4);
  infoSettings.btt_ups              = byteToWord(data + (index += 4), 4);

//machine specific settings

  infoSettings.hotend_count         = byteToWord(data + (index += 4), 4);
  infoSettings.bed_en               = byteToWord(data + (index += 4), 4);
  infoSettings.chamber_en           = byteToWord(data + (index += 4), 4);
  infoSettings.ext_count            = byteToWord(data + (index += 4), 4);
  infoSettings.fan_count            = byteToWord(data + (index += 4), 4);
  infoSettings.fan_ctrl_count       = byteToWord(data + (index += 4), 4);
  infoSettings.auto_load_leveling   = byteToWord(data + (index += 4), 4);
  infoSettings.onboardSD            = byteToWord(data + (index += 4), 4);
  infoSettings.m27_refresh_time     = byteToWord(data + (index += 4), 4);
  infoSettings.m27_active           = byteToWord(data + (index += 4), 4);
  infoSettings.longFileName         = byteToWord(data + (index += 4), 4);

  for(int i = 0; i < MAX_HEATER_COUNT; i++)
  {
    infoSettings.max_temp[i]          = byteToWord(data + (index += 4), 4);
  }

  infoSettings.min_ext_temp           = byteToWord(data + (index += 4), 4);

  for(int i = 0; i < MAX_FAN_COUNT ;i++)
  {
    infoSettings.fan_max[i]           = byteToWord(data + (index += 4), 4);
  }

  infoSettings.fan_percentage         = byteToWord(data + (index += 4), 4);

  for(int i = 0; i < AXIS_NUM ;i++) //x, y, z
  {
    infoSettings.invert_axis[i]       = byteToWord(data + (index += 4), 4);
    infoSettings.machine_size_min[i]  = byteToWord(data + (index += 4), 4);
    infoSettings.machine_size_max[i]  = byteToWord(data + (index += 4), 4);
    infoSettings.level_feedrate[i]    = byteToWord(data + (index += 4), 4);
  }

  for(int i = 0; i < SPEED_COUNT ;i++)
  {
    infoSettings.axis_speed[i]        = byteToWord(data + (index += 4), 4);
    infoSettings.ext_speed[i]         = byteToWord(data + (index += 4), 4);
  }

  infoSettings.pause_retract_len      = byteToWord(data + (index += 4), 4);
  infoSettings.resume_purge_len       = byteToWord(data + (index += 4), 4);


  infoSettings.pause_pos[X_AXIS]      = byteToWord(data + (index += 4), 4);  // X
  infoSettings.pause_pos[Y_AXIS]      = byteToWord(data + (index += 4), 4);  // Y
  infoSettings.pause_z_raise          = byteToWord(data + (index += 4), 4);

  for(int i = 0; i < TOTAL_AXIS ;i++)
  {
    infoSettings.pause_feedrate[i]    = byteToWord(data + (index += 4), 4); // X, Y, Z, E
  }

  infoSettings.level_edge             = byteToWord(data + (index += 4), 4);
  infoSettings.level_z_pos            = byteToWord(data + (index += 4), 4);
  infoSettings.level_z_raise          = byteToWord(data + (index += 4), 4);

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    infoSettings.preheat_temp[i]      = byteToWord(data + (index += 4), 4);
    infoSettings.preheat_bed[i]       = byteToWord(data + (index += 4), 4);
  }

  }
}

void storePara(void)
{
  u8 data[PARA_SIZE];
  u32 index = 0;

  wordToByte(TSC_SIGN,                                data + (index += 4));
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    wordToByte(TSC_Para[i],                           data + (index += 4));
  }
  wordToByte(infoSettings.rotate_ui,                  data + (index += 4));

  wordToByte(PARA_SIGN,                               data + (index += 4));
  wordToByte(infoSettings.baudrate,                   data + (index += 4));
  wordToByte(infoSettings.language,                   data + (index += 4));
  wordToByte(infoSettings.mode,                       data + (index += 4));
  wordToByte(infoSettings.status_screen,              data + (index += 4));

  wordToByte(infoSettings.bg_color,                   data + (index += 4));
  wordToByte(infoSettings.font_color,                 data + (index += 4));
  wordToByte(infoSettings.title_bg_color,             data + (index += 4));
  wordToByte(infoSettings.reminder_color,             data + (index += 4));
  wordToByte(infoSettings.sd_reminder_color,          data + (index += 4));
  wordToByte(infoSettings.status_xyz_bg_color,        data + (index += 4));
  wordToByte(infoSettings.list_border_color,          data + (index += 4));
  wordToByte(infoSettings.list_button_color,          data + (index += 4));
  wordToByte(infoSettings.mesh_min_color,             data + (index += 4));
  wordToByte(infoSettings.mesh_max_color,             data + (index += 4));

  wordToByte(infoSettings.touchSound,                 data + (index += 4));
  wordToByte(infoSettings.toastSound,                 data + (index += 4));
  wordToByte(infoSettings.alertSound,                 data + (index += 4));

  wordToByte(infoSettings.terminalACK,                data + (index += 4));
  wordToByte(infoSettings.move_speed,                 data + (index += 4));
  wordToByte(infoSettings.knob_led_color,             data + (index += 4));
  wordToByte(infoSettings.knob_led_idle,              data + (index += 4));
  wordToByte(infoSettings.persistent_info,            data + (index += 4));
  wordToByte(infoSettings.file_listmode,              data + (index += 4));
  wordToByte(infoSettings.ack_notification,           data + (index += 4));

  wordToByte(infoSettings.lcd_brightness,             data + (index += 4));
  wordToByte(infoSettings.lcd_idle_brightness,        data + (index += 4));
  wordToByte(infoSettings.lcd_idle_timer,             data + (index += 4));

  wordToByte(infoSettings.print_summary,              data + (index += 4));

  wordToByte(infoSettings.serial_alwaysOn,            data + (index += 4));
  wordToByte(infoSettings.marlin_mode_bg_color,       data + (index += 4));
  wordToByte(infoSettings.marlin_mode_font_color,     data + (index += 4));
  wordToByte(infoSettings.marlin_mode_showtitle,      data + (index += 4));
  wordToByte(infoSettings.marlin_mode_fullscreen,     data + (index += 4));
  wordToByte(infoSettings.marlin_type,                data + (index += 4));

  wordToByte(infoSettings.send_start_gcode,           data + (index += 4));
  wordToByte(infoSettings.send_end_gcode,             data + (index += 4));
  wordToByte(infoSettings.send_cancel_gcode,          data + (index += 4));
  wordToByte(infoSettings.auto_off,                   data + (index += 4));
  wordToByte(infoSettings.ps_active_high,             data + (index += 4));
  wordToByte(infoSettings.auto_off_temp,              data + (index += 4));

  wordToByte(infoSettings.runout,                     data + (index += 4));
  wordToByte(infoSettings.runout_invert,              data + (index += 4));
  wordToByte(infoSettings.runout_noise_ms,            data + (index += 4));
  wordToByte(infoSettings.runout_distance,            data + (index += 4));

  wordToByte(infoSettings.powerloss_en,               data + (index += 4));
  wordToByte(infoSettings.powerloss_home,             data + (index += 4));
  wordToByte(infoSettings.powerloss_invert,           data + (index += 4));
  wordToByte(infoSettings.powerloss_z_raise,          data + (index += 4));
  wordToByte(infoSettings.btt_ups,                    data + (index += 4));

  //machine specific settings

  wordToByte(infoSettings.hotend_count,               data + (index += 4));
  wordToByte(infoSettings.bed_en,                     data + (index += 4));
  wordToByte(infoSettings.chamber_en,                 data + (index += 4));
  wordToByte(infoSettings.ext_count,                  data + (index += 4));
  wordToByte(infoSettings.fan_count,                  data + (index += 4));
  wordToByte(infoSettings.fan_ctrl_count,             data + (index += 4));
  wordToByte(infoSettings.auto_load_leveling,         data + (index += 4));
  wordToByte(infoSettings.onboardSD,                  data + (index += 4));
  wordToByte(infoSettings.m27_refresh_time,           data + (index += 4));
  wordToByte(infoSettings.m27_active,                 data + (index += 4));
  wordToByte(infoSettings.longFileName,               data + (index += 4));

  for(int i = 0; i < MAX_HEATER_COUNT; i++)
  {
    wordToByte(infoSettings.max_temp[i],              data + (index += 4));
  }

  wordToByte(infoSettings.min_ext_temp,               data + (index += 4));

  for(int i = 0; i < MAX_FAN_COUNT ;i++)
  {
    wordToByte(infoSettings.fan_max[i],               data + (index += 4));
  }

  wordToByte(infoSettings.fan_percentage,             data + (index += 4));

  for(int i = 0; i < AXIS_NUM ;i++) //x, y, z
  {
    wordToByte(infoSettings.invert_axis[i],            data + (index += 4));
    wordToByte(infoSettings.machine_size_min[i],       data + (index += 4));
    wordToByte(infoSettings.machine_size_max[i],       data + (index += 4));
    wordToByte(infoSettings.level_feedrate[i],         data + (index += 4));
  }

  for(int i = 0; i < SPEED_COUNT ;i++)
  {
    wordToByte(infoSettings.axis_speed[i],              data + (index += 4));
    wordToByte(infoSettings.ext_speed[i],               data + (index += 4));
  }

  wordToByte(infoSettings.pause_retract_len,          data + (index += 4));
  wordToByte(infoSettings.resume_purge_len,           data + (index += 4));


  wordToByte(infoSettings.pause_pos[X_AXIS],          data + (index += 4));  // X
  wordToByte(infoSettings.pause_pos[Y_AXIS],          data + (index += 4));  // Y
  wordToByte(infoSettings.pause_z_raise,              data + (index += 4));

  for(int i = 0; i < TOTAL_AXIS ;i++)
  {
    wordToByte(infoSettings.pause_feedrate[i],        data + (index += 4)); // X, Y, Z, E
  }

  wordToByte(infoSettings.level_edge,                 data + (index += 4));
  wordToByte(infoSettings.level_z_pos,                data + (index += 4));
  wordToByte(infoSettings.level_z_raise,              data + (index += 4));

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    wordToByte(infoSettings.preheat_temp[i],          data + (index += 4));
    wordToByte(infoSettings.preheat_bed[i],           data + (index += 4));
  }

  STM32_FlashWrite(data, PARA_SIZE);
}

bool readIsTSCExist(void)
{
  return ((paraStatus & PARA_TSC_EXIST) != 0);
}

bool readIsRestored(void)
{
  return ((paraStatus & PARA_WAS_RESTORED) != 0);
}
