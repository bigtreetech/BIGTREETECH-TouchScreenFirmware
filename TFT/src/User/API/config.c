#include "config.h"

//#define CONFIG_DEBUG  // To be used only when calling 'getConfigFromFile()' after boot process
#ifdef CONFIG_DEBUG
#define PRINTDEBUG(x) Serial_Puts(SERIAL_PORT, x);
#else
#define PRINTDEBUG(x)
#endif


const GUI_POINT pointConfigTitle     = {2,2};
const GUI_RECT  rectTitleline         = {0,               BYTE_HEIGHT+4,      LCD_WIDTH,                BYTE_HEIGHT+6};
const GUI_RECT  recterror             = {BYTE_WIDTH/2-2,  BYTE_HEIGHT*2+2,    LCD_WIDTH-BYTE_WIDTH/2+2, LCD_HEIGHT-(BYTE_HEIGHT*4)-4};
const GUI_RECT  recterrortxt          = {BYTE_WIDTH/2,    BYTE_HEIGHT*2+4,    LCD_WIDTH-BYTE_WIDTH/2,   LCD_HEIGHT-(BYTE_HEIGHT*4)-6};
const GUI_RECT  rectProgressframe     = {BYTE_WIDTH/2-2, LCD_HEIGHT-(BYTE_HEIGHT*2+BYTE_HEIGHT/2), LCD_WIDTH-BYTE_WIDTH/2+2,LCD_HEIGHT-BYTE_HEIGHT/2};
const GUI_POINT pointProgressText     = {BYTE_WIDTH/2-2, LCD_HEIGHT-(BYTE_HEIGHT*4)};

u16 foundkeys = 0;

CONFIGFILE configFile;
char cur_line[LINE_MAX_CHAR];
int customcode_index = 0;
int customcode_good[CUSTOM_GCODES_COUNT];
bool scheduleRotate = false;

static CUSTOM_GCODES* configCustomGcodes = NULL;
PRINT_GCODES* configPrintGcodes  = NULL;
STRINGS_STORE* configStringsStore = NULL;

const char *const config_keywords[CONFIG_COUNT] = {
  #define X_CONFIG(NAME) CONFIG_##NAME ,
  #include "config.inc"
  #undef  X_CONFIG
};


void getConfigFromFile(void)
{
  #ifdef CONFIG_DEBUG
    Serial_ReSourceInit();
  #endif

  configCustomGcodes = (CUSTOM_GCODES*)malloc(sizeof(CUSTOM_GCODES));
  configPrintGcodes = (PRINT_GCODES*)malloc(sizeof(PRINT_GCODES));
  configStringsStore = (STRINGS_STORE*)malloc(sizeof(STRINGS_STORE));
  customcode_index = 0;
  bool comment_mode = false;
  bool comment_space = true;
  char cur_char;
  char last_char = 0;
  u8 count = 0;
  UINT br = 0;
  if (f_file_exists(CONFIG_FILE_PATH) == false)
    return;

  drawProgressPage();

  if (f_open(&configFile.file, CONFIG_FILE_PATH, FA_OPEN_EXISTING | FA_READ) != FR_OK)
  {
    PRINTDEBUG("parse error\n");
    showError(CSTAT_FILE_NOTOPEN);
    return;
  }
  else
  {

    configFile.size = f_size(&configFile.file);
    if (configFile.size == 0)
    {
      showError(CSTAT_FILE_INVALID);
      f_close(&configFile.file);
      return;
    }
    for (; configFile.cur < configFile.size;)
    {
      if (f_read(&configFile.file, &cur_char, 1, &br) != FR_OK)
      {
        PRINTDEBUG("read error\n");
        return;
      }
      configFile.cur++;

      if (cur_char == '\n')             // start parsing line after new line.
      {
        comment_mode = false;           //for new command
        comment_space = true;
        if (count != 0)
        {
          //cur_line[count++] = '\n';
          cur_line[count++] = '\0';
          cur_line[count] = 0;          //terminate string
          parseConfigLine();
          drawProgress();

          PRINTDEBUG("\n");
          PRINTDEBUG(cur_line);
          count = 0;                    //clear buffer
        }
      }
      else if (count < LINE_MAX_CHAR - 2)
      {
        if (cur_char == '#')
          comment_mode = true;
        else
        {
          if (comment_space && cur_char != ' ')                    //ignore ' ' space bytes
            comment_space = false;
          if (!comment_mode && !comment_space && cur_char != '\r') //normal code
          {
            if (cur_char == 'n' && last_char == '\\')              //replace "\n" with new line char('\n')
            {
              cur_char = '\n';
              count--;
            }
            cur_line[count++] = cur_char;
            last_char = cur_char;

            if (configFile.cur == configFile.size)
            {
              cur_line[count++] = '\0';
              cur_line[count] = 0;        //terminate string
              parseConfigLine();          //start parsing at the end of the file.
            }
          }
        }
      }
    }
    //store custom codes count
    configCustomGcodes->count = customcode_index;

    PRINTDEBUG("\nCustom gcode stored at 1:");
    PRINTDEBUG(configCustomGcodes->gcode[1]);
    if(scheduleRotate)
    {
      LCD_RefreshDirection();
      TSC_Calibration();
    }
    storePara();
    saveConfig();
    free(configCustomGcodes);
    free(configPrintGcodes);
    free(configStringsStore);

    f_close(&configFile.file);
    configFile.cur = 0;
    configFile.size  = 0;
  }
}

static u16 c_index = 0;

//check if the value is within min and max limits
bool inLimit(int val, int min, int max)
{
  if (val < min || val > max)
  {
    showError(CSTAT_INVALID_VALUE);
    return false;
  }
  else
  {
    return true;
  }
}

//check if keyword exits in the buffer line
static char key_seen(const char *keyStr)
{
  u16 i;
  for (c_index = 0; c_index < ACK_MAX_SIZE && cur_line[c_index] != 0; c_index++)
  {
    for (i = 0; keyStr[i] != 0 && cur_line[c_index + i] != 0 && cur_line[c_index + i] == keyStr[i]; i++)
    {
    }
    if (keyStr[i] == 0)
    {
      c_index += i;
      return true;
    }
  }
  return false;
}

// Get the int after config keyword.
static int config_int(void)
{
  return (strtol(&cur_line[c_index], NULL, 10));
}

// Treat all values other than 0 as 1.
int8_t getOnOff(void)
{
  if (config_int()  == 0)
    return 0;
  else
    return 1;
}

// Get the float after config keyword.
static float config_float(void)
{
  return (strtod(&cur_line[c_index], NULL));
}

//check if value is hex format
static bool config_is_hex(void)
{
  return (strstr(&cur_line[c_index], "0x") != NULL);
}

// Get the hex after config keyword.
static uint32_t config_hex(void)
{
  return (strtol(&cur_line[c_index], NULL, 16));
}

// Get the hex after config keyword.
static uint16_t RGB888_to_RGB565(uint32_t rgb888)
{
  uint8_t r = ((rgb888 >> 16) & 0xFF) >> 3; // R5
  uint8_t g = ((rgb888 >> 8) & 0xFF) >> 2;  // G6
  uint8_t b = (rgb888 & 0xFF) >> 3;         // B5
  return ((r << 11) | (g << 5) | b);
}

static void config_set_color(uint16_t *color_src)
{
  if (config_is_hex()) {
    *color_src = RGB888_to_RGB565(config_hex());
  } else if (inLimit(config_int(), 0, LCD_COLOR_COUNT-1)){
    *color_src = lcd_colors[config_int()];
  }
}

//check keywords in the config line in buffer
void parseConfigLine(void)
{
  for (u16 i = 0; i < CONFIG_COUNT; i++){
    if (key_seen(config_keywords[i]))
    {
      PRINTDEBUG("\n");
      PRINTDEBUG((char*)config_keywords[i]);
      parseConfigKey(i);
      foundkeys++;
      return;
    }
  }
  showError(CSTAT_UNKNOWN_KEYWORD);
}

void saveConfig(void)
{
  writeConfig((uint8_t *)configCustomGcodes, sizeof(CUSTOM_GCODES), CUSTOM_GCODE_ADDR, CUSTOM_GCODE_MAX_SIZE);

  writeConfig((uint8_t *)configPrintGcodes, sizeof(PRINT_GCODES), PRINT_GCODES_ADDR, PRINT_GCODES_MAX_SIZE);

  writeConfig((uint8_t *)configStringsStore, sizeof(STRINGS_STORE), STRINGS_STORE_ADDR, STRINGS_STORE_MAX_SIZE);

  #ifdef CONFIG_DEBUG
  CUSTOM_GCODES tempgcode;// = NULL;

  uint8_t * data_r = (u8 *)&tempgcode;

  W25Qxx_ReadBuffer(data_r,CUSTOM_GCODE_ADDR,sizeof(CUSTOM_GCODES));
  PRINTDEBUG("\nread done");

  PRINTDEBUG("\nread from flash:");
  PRINTDEBUG(tempgcode.gcode[1]);
  #endif
}

void writeConfig(uint8_t* dataBytes, uint16_t numBytes, uint32_t addr, uint32_t maxSize){
  //do not proceed if data size is larger than reserved max size.
  if (numBytes > maxSize)
    {
      PRINTDEBUG("\nwrite error\n");
      showError(CSTAT_STORAGE_LOW);
      return;
    }
  int sectorCount =maxSize/W25QXX_SECTOR_SIZE;

  //erase part of flash to be rewritten
  for (int i = 0; i < sectorCount;i++){
    W25Qxx_EraseSector(addr+(i*W25QXX_SECTOR_SIZE));
  }
  Delay_ms(100); //give time for spi flash to settle

  //write data to spi flash
  W25Qxx_WriteBuffer(dataBytes,addr,numBytes);
  Delay_ms(100); //give time for spi flash to settle

}

//Reset & store config settings
void resetConfig(void)
{
  const uint8_t cg_enabled[MAX_GCODE_LENGTH]     = CUSTOM_GCODE_ENABLED;
  const char cg_list[][MAX_GCODE_LENGTH]         = CUSTOM_GCODE_LIST;
  const char cg_names[][MAX_GCODE_LENGTH]        = CUSTOM_GCODE_LABELS;
  const char cg_preheatnames[][MAX_GCODE_LENGTH] = PREHEAT_LABELS;


  CUSTOM_GCODES tempCG;
  STRINGS_STORE tempST;
  PRINT_GCODES tempPC;

  //restore custom gcode presets
  int n = 0;
  for (int i = 0; i < CUSTOM_GCODES_COUNT;i++)
  {
    if(cg_enabled[i] == 1){
    strcpy(tempCG.gcode[n],cg_list[i]);
    strcpy(tempCG.name[n],cg_names[i]);
    n++;
    }
  }
  tempCG.count = n;

  //restore strings store
  strcpy(tempST.marlin_title,ST7920_BANNER_TEXT);

  for (int i = 0; i < PREHEAT_COUNT;i++)
  {
    strcpy(tempST.preheat_name[i],cg_preheatnames[i]);
  }

  //restore print gcodes
  strcpy(tempPC.start_gcode,PRINT_START_GCODE);
  strcpy(tempPC.end_gcode,PRINT_END_GCODE);
  strcpy(tempPC.cancel_gcode,PRINT_CANCEL_GCODE);

  // write restored config
  writeConfig((uint8_t *)&tempCG, sizeof(CUSTOM_GCODES), CUSTOM_GCODE_ADDR, CUSTOM_GCODE_MAX_SIZE);
  writeConfig((uint8_t *)&tempPC, sizeof(PRINT_GCODES), PRINT_GCODES_ADDR, PRINT_GCODES_MAX_SIZE);
  writeConfig((uint8_t *)&tempST, sizeof(STRINGS_STORE), STRINGS_STORE_ADDR, STRINGS_STORE_MAX_SIZE);
}

void drawProgressPage(void)
{
  GUI_Clear(BLACK);
  GUI_DispString(2,2,(u8*)"Updating Configuration...");
  GUI_FillRectColor(rectTitleline.x0,rectTitleline.y0,rectTitleline.x1,rectTitleline.y1,BLUE);
  //GUI_DrawPrect(&recterror);
  GUI_DrawPrect(&rectProgressframe);
}

void drawProgress(void){
  char tempstr[50];
  my_sprintf(tempstr,"Total keywords found: %d",foundkeys);
  GUI_DispString(pointProgressText.x,pointProgressText.y,(u8*)tempstr);
  u16 p = map(configFile.cur,0,configFile.size, rectProgressframe.x0,rectProgressframe.x1);
  GUI_FillRect(rectProgressframe.x0,rectProgressframe.y0,p,rectProgressframe.y1);
}

void showError(CONFIG_STATS stat)
{
  char *ttl;
  char *txt;
  GUI_SetBkColor(BLACK);
  GUI_ClearPrect(&recterrortxt);
  GUI_DrawPrect(&recterror);

  char tempstr[50];

  switch (stat)
  {
  case CSTAT_INVALID_VALUE:
    GUI_SetColor(ORANGE);
    ttl = "Invalid Value(s) in:";
    txt = cur_line;
    break;
  case CSTAT_UNKNOWN_KEYWORD:
    GUI_SetColor(RED);
    ttl = "Invalid Keyword:";
    txt = cur_line;
    break;
  case CSTAT_FILE_NOTOPEN:
    GUI_SetColor(RED);
    ttl = "Error:";
    my_sprintf(tempstr, "Unable to open %s", CONFIG_FILE_PATH);
    txt = tempstr;
    break;
  case CSTAT_STORAGE_LOW:
    GUI_SetColor(RED);
    ttl = "Write Error:";
    my_sprintf(tempstr, "Config size is larger than allocated size", CONFIG_FILE_PATH);
    txt = tempstr;
    break;
  case CSTAT_FILE_INVALID:
  default:
    GUI_SetColor(RED);
    ttl = "Error:";
    txt = "Invalid config File";
    break;
  }
  GUI_DispString(recterrortxt.x0, recterrortxt.y0, (u8*)ttl);
  GUI_DispStringInRect(recterrortxt.x0, recterrortxt.y0 + (BYTE_HEIGHT * 2), recterrortxt.x1, recterrortxt.y1, (u8*)txt);
  GUI_SetColor(WHITE);
  Delay_ms(5000);
}

// parse the keyword values in the buffer
void parseConfigKey(u16 index)
{
  //---------------------------------------------------------general settings

  switch (index)
  {
  case C_INDEX_UNIFIEDMENU:
      infoSettings.unified_menu = getOnOff();
    break;

  case C_INDEX_UART_BAUDRATE:
    if (inLimit(config_int(),0,ITEM_BAUDRATE_NUM-1))
      infoSettings.baudrate = item_baudrate[config_int()];
    break;

  case C_INDEX_LANGUAGE:
    if (inLimit(config_int(), 0, LANGUAGE_NUM-1))
      infoSettings.language = config_int();
    break;

  case C_INDEX_TITLE_BG:
    config_set_color(&infoSettings.title_bg_color);
    break;

  case C_INDEX_MENU_BG_COLOR:
    config_set_color(&infoSettings.bg_color);
    break;

  case C_INDEX_MENU_FONT_COLOR:
    config_set_color(&infoSettings.font_color);
    break;

  case C_INDEX_NOTIFY_COLOR:
    config_set_color(&infoSettings.reminder_color);
    break;

  case C_INDEX_SD_NOTIFY_COLOR:
    config_set_color(&infoSettings.sd_reminder_color);
    break;

  case C_INDEX_SS_XYZ_BG_COLOR:
    config_set_color(&infoSettings.status_xyz_bg_color);
    break;

  case C_INDEX_LIST_BORDER_COLOR:
    config_set_color(&infoSettings.list_border_color);
    break;

  case C_INDEX_LIST_BUTTON_BG_COLOR:
    config_set_color(&infoSettings.list_button_color);
    break;

  case C_INDEX_ROTATE_UI:
      if (infoSettings.rotate_ui != getOnOff())
          scheduleRotate = true;
        infoSettings.rotate_ui = getOnOff();
    break;

  case C_INDEX_TERMINAL_ACK:
      infoSettings.terminalACK = getOnOff();
    break;

  case C_INDEX_INVERT_AXIS:
    if (key_seen("X"))
        infoSettings.invert_axis[X_AXIS] = getOnOff();
    if (key_seen("Y"))
        infoSettings.invert_axis[Y_AXIS] = getOnOff();
    if (key_seen("Z"))
        infoSettings.invert_axis[Z_AXIS] = getOnOff();
    break;

  case C_INDEX_PERSISTENT_TEMP:
      infoSettings.persistent_info = getOnOff();
    break;

  case C_INDEX_LIST_MODE:
      infoSettings.file_listmode = getOnOff();
    break;

  //---------------------------------------------------------Marlin Mode Settings (Only for TFT35_V3.0/TFT24_V1.1/TFT28V3.0)

#ifdef ST7920_SPI

  case C_INDEX_MODE:
    if (inLimit(config_int(), 0, MODE_COUNT-1))
      infoSettings.mode = config_int();
    break;

  case C_INDEX_SERIAL_ON:
      infoSettings.serial_alwaysOn = getOnOff();
    break;

  case C_INDEX_MARLIN_BG_COLOR:
    config_set_color(&infoSettings.marlin_mode_bg_color);
    break;

  case C_INDEX_MARLIN_FONT_COLOR:
    config_set_color(&infoSettings.marlin_mode_font_color);
    break;

  case C_INDEX_MARLIN_SHOW_TITLE:
      infoSettings.marlin_mode_showtitle = getOnOff();
    break;
      
  case C_INDEX_MARLIN_FULLSCREEN:
      infoSettings.marlin_mode_fullscreen = getOnOff();
    break;
      
  case C_INDEX_MARLIN_TITLE:
    {
      char * pchr;
      pchr = strrchr(cur_line,':') + 1;
      int utf8len = getUTF8Length((u8*)pchr);
      int bytelen = strlen(pchr) + 1;
      if (inLimit(utf8len,NAME_MIN_LENGTH,MAX_STRING_LENGTH) && inLimit(bytelen,NAME_MIN_LENGTH,MAX_GCODE_LENGTH))
        strcpy(configStringsStore->marlin_title, pchr);
    }
    break;
      
#endif //ST7920_SPI

  //---------------------------------------------------------Printer / Machine Settings

  case C_INDEX_TOOL_COUNT:
    if (inLimit(config_int(), 1, MAX_TOOL_COUNT))
      infoSettings.tool_count = config_int();
    break;

  case C_INDEX_EXT_COUNT:
    if (inLimit(config_int(), 1, MAX_EXT_COUNT))
      infoSettings.ext_count = config_int();
    break;

  case C_INDEX_FAN_COUNT:
    if (inLimit(config_int(), 1, MAX_FAN_COUNT))
      infoSettings.fan_count = config_int();
    break;

  case C_INDEX_MAX_TEMP:
    if (key_seen("BED:"))
    { if (inLimit(config_int(), MIN_BED_TEMP, MAX_BED_TEMP))
        infoSettings.max_temp[BED] = config_int();
    }
    if (key_seen("T0:"))
    { if (inLimit(config_int(), MIN_TOOL_TEMP, MAX_TOOL_TEMP))
        infoSettings.max_temp[BED + 1] = config_int();
    }
    if (key_seen("T1:"))
    { if (inLimit(config_int(), MIN_TOOL_TEMP, MAX_TOOL_TEMP))
        infoSettings.max_temp[BED + 2] = config_int();
    }
    if (key_seen("T2:"))
    { if (inLimit(config_int(), MIN_TOOL_TEMP, MAX_TOOL_TEMP))
        infoSettings.max_temp[BED + 3] = config_int();
    }
    if (key_seen("T3:"))
    { if (inLimit(config_int(), MIN_TOOL_TEMP, MAX_TOOL_TEMP))
        infoSettings.max_temp[BED + 4] = config_int();
    }
    if (key_seen("T4:"))
    { if (inLimit(config_int(), MIN_TOOL_TEMP, MAX_TOOL_TEMP))
        infoSettings.max_temp[BED + 5] = config_int();
    }
    if (key_seen("T5:"))
    { if (inLimit(config_int(), MIN_TOOL_TEMP, MAX_TOOL_TEMP))
        infoSettings.max_temp[BED + 6] = config_int();
    }
    break;

  case C_INDEX_MIN_TEMP:
    if (inLimit(config_int(), 25, MAX_TOOL_TEMP))
      infoSettings.min_ext_temp = config_int();
    break;

  case C_INDEX_FAN_MAX:
    if (key_seen("F0:"))
    { if (inLimit(config_int(), MIN_FAN_SPEED, MAX_FAN_SPEED))
        infoSettings.fan_max[0] = config_int();
    }
    if (key_seen("F1:"))
    { if (inLimit(config_int(), MIN_FAN_SPEED, MAX_FAN_SPEED))
        infoSettings.fan_max[1] = config_int();
    }
    if (key_seen("F2:"))
    { if (inLimit(config_int(), MIN_FAN_SPEED, MAX_FAN_SPEED))
        infoSettings.fan_max[2] = config_int();
    }
    if (key_seen("F3:"))
    { if (inLimit(config_int(), MIN_FAN_SPEED, MAX_FAN_SPEED))
        infoSettings.fan_max[3] = config_int();
    }
    if (key_seen("F4:"))
    { if (inLimit(config_int(), MIN_FAN_SPEED, MAX_FAN_SPEED))
        infoSettings.fan_max[4] = config_int();
    }
    if (key_seen("F5:"))
    { if (inLimit(config_int(), MIN_FAN_SPEED, MAX_FAN_SPEED))
        infoSettings.fan_max[5] = config_int();
    }
    break;

  case C_INDEX_BUILD_MIN:
    if (key_seen("X"))
    { if (inLimit(config_int(),  MIN_SIZE_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.machine_size_min[X_AXIS] = config_int();
    }
    if (key_seen("Y"))
    { if (inLimit(config_int(),  MIN_SIZE_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.machine_size_min[Y_AXIS] = config_int();
    }
    if (key_seen("Z"))
    { if (inLimit(config_int(),  MIN_SIZE_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.machine_size_min[Z_AXIS] = config_int();
    }
    break;

  case C_INDEX_BUILD_MAX:
    if (key_seen("X"))
    { if (inLimit(config_int(),  MIN_SIZE_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.machine_size_max[X_AXIS] = config_int();
    }
    if (key_seen("Y"))
    { if (inLimit(config_int(),  MIN_SIZE_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.machine_size_max[Y_AXIS] = config_int();
    }
    if (key_seen("Z"))
    { if (inLimit(config_int(),  MIN_SIZE_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.machine_size_max[Z_AXIS] = config_int();
    }
    break;

  case C_INDEX_MOVE_SPEED:
    if (key_seen("S"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.axis_speed[0] = config_int();
    }
    if (key_seen("N"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.axis_speed[1] = config_int();
    }
    if (key_seen("F"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.axis_speed[2] = config_int();
    }
    break;

  case C_INDEX_EXT_SPEED:
    if (key_seen("S"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.ext_speed[0] = config_int();
    }
    if (key_seen("N"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.ext_speed[1] = config_int();
    }
    if (key_seen("F"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.ext_speed[2] = config_int();
    }
    break;

  case C_INDEX_AUTO_LEVEL:
      infoSettings.auto_load_leveling = getOnOff();
    break;

  case C_INDEX_ONBOARD_SD:
    {
      u8 i = config_int();
      if (inLimit(i,0,2))
        infoSettings.onboardSD = i;
      break;
    }

  case C_INDEX_M27_DELAY:
    if (inLimit(config_int(), MIN_DELAY_SEC, MAX_DELAY_SEC))
      infoSettings.m27_refresh_time = config_int();
    break;

  case C_INDEX_M27_KEEP_ON:
      infoSettings.m27_active = getOnOff();
    break;

  case C_INDEX_LONG_FILENAME:
    {
      u8 i = config_int();
      if (inLimit(i,0,2))
        infoSettings.longFileName = i;
      break;
    }

  case C_INDEX_FAN_PERCENT:
      infoSettings.fan_percentage = getOnOff();
    break;

  case C_INDEX_PAUSE_RETRACT:
    if (key_seen("R"))
    { if (inLimit(config_float(), MIN_RETRACT_LIMIT, MAX_RETRACT_LIMIT))
        infoSettings.pause_retract_len = config_float();
    }
    if (key_seen("P"))
    { if (inLimit(config_float(), MIN_RETRACT_LIMIT, MAX_RETRACT_LIMIT))
        infoSettings.resume_purge_len = config_float();
    }
    break;

  case C_INDEX_PAUSE_POS:
    if (key_seen("X"))
    { if (inLimit(config_float(), MIN_POS_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.pause_pos[X_AXIS] = config_float();
    }
    if (key_seen("Y"))
    { if (inLimit(config_float(), MIN_POS_LIMIT, MAX_SIZE_LIMIT))
        infoSettings.pause_pos[Y_AXIS] = config_float();
    }
    break;

  case C_INDEX_PAUSE_Z_RAISE:
    if (inLimit(config_float(), MIN_POS_LIMIT, MAX_SIZE_LIMIT))
      infoSettings.pause_z_raise = config_float();
    break;

  case C_INDEX_PAUSE_FEEDRATE:
    if (key_seen("X"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.pause_feedrate[0] = config_int();
    }
    if (key_seen("Y"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.pause_feedrate[1] = config_int();
    }
    if (key_seen("Z"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.pause_feedrate[2] = config_int();
    }
    if (key_seen("E"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.pause_feedrate[3] = config_int();
    }
    break;

  case C_INDEX_LEVEL_EDGE:
    if (inLimit(config_int(), MIN_POS_LIMIT, MAX_SIZE_LIMIT))
      infoSettings.level_edge = config_int();
    break;

  case C_INDEX_LEVEL_Z_POS:
    if (inLimit(config_float(), MIN_POS_LIMIT, MAX_SIZE_LIMIT))
      infoSettings.level_z_pos = config_float();
    break;

  case C_INDEX_LEVEL_Z_RAISE:
    if (inLimit(config_float(), MIN_POS_LIMIT, MAX_SIZE_LIMIT))
      infoSettings.level_z_raise = config_float();
    break;

  case C_INDEX_LEVEL_FEEDRATE:
    if (key_seen("X"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.level_feedrate[0] = config_int();
    }
    if (key_seen("Y"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.level_feedrate[1] = config_int();
    }
    if (key_seen("Z"))
    { if (inLimit(config_int(), MIN_SPEED_LIMIT, MAX_SPEED_LIMIT))
        infoSettings.level_feedrate[2] = config_int();
    }
    break;

  case C_INDEX_PREHEAT_NAME_1:
  case C_INDEX_PREHEAT_NAME_2:
  case C_INDEX_PREHEAT_NAME_3:
  {
    char pchr[LINE_MAX_CHAR];
    strcpy(pchr, strrchr(cur_line, ':') + 1);
      int utf8len = getUTF8Length((u8*)pchr);
      int bytelen = strlen(pchr) + 1;
      if (inLimit(utf8len,NAME_MIN_LENGTH,MAX_STRING_LENGTH) && inLimit(bytelen,NAME_MIN_LENGTH,MAX_GCODE_LENGTH))
      strcpy(configStringsStore->preheat_name[index - C_INDEX_PREHEAT_NAME_1], pchr);
  }

    break;

  case C_INDEX_PREHEAT_TEMP_1:
  case C_INDEX_PREHEAT_TEMP_2:
  case C_INDEX_PREHEAT_TEMP_3:
    {
        int val_index = index - C_INDEX_PREHEAT_TEMP_1;
      if (key_seen("B"))
      { if (inLimit(config_int(), MIN_BED_TEMP, MAX_BED_TEMP))
          infoSettings.preheat_bed[val_index] = config_int();
      }
      if (key_seen("T"))
      { if (inLimit(config_int(), MIN_TOOL_TEMP, MAX_TOOL_TEMP))
          infoSettings.preheat_temp[val_index] = config_int();
      }
    }
    break;

  //---------------------------------------------------------Power Supply Settings (if connected to TFT Controller):

#ifdef PS_ON_PIN

  case C_INDEX_PS_ON:
    if (config_int() == 1)
      infoSettings.auto_off = 1;
    else if(config_int() == 2)
      infoSettings.auto_off = 2;
    else
      infoSettings.auto_off = 0;
    break;

  case C_INDEX_PS_LOGIC:
      infoSettings.powerloss_invert = getOnOff();
    break;

  case C_INDEX_SHUTDOWN_TEMP:
    if (inLimit(config_int(), MIN_BED_TEMP, MAX_BED_TEMP))
        infoSettings.auto_off_temp = config_int();
    break;

#endif

  //---------------------------------------------------------Power Loss Recovery & BTT UPS Settings (if connected to TFT Controller:

#ifdef BTT_MINI_UPS

  case C_INDEX_POWERLOSS_HOME:
      infoSettings.powerloss_home = getOnOff();
    break;

  case C_INDEX_POWERLOSS_ZRAISE:
    if (inLimit(config_float(),  MIN_SIZE_LIMIT, MAX_SIZE_LIMIT))
          infoSettings.powerloss_z_raise = config_float();
    break;

  case C_INDEX_BTT_MINIUPS:
      infoSettings.btt_ups = getOnOff();
    break;

#endif

  //---------------------------------------------------------Filament Runout Settings (if connected to TFT Controller):

#ifdef FIL_RUNOUT_PIN
  case C_INDEX_RUNOUT:
      infoSettings.runout = getOnOff();
    break;

  case C_INDEX_RUNOUT_LOGIC:
      infoSettings.runout_invert = getOnOff();
    break;

  case C_INDEX_RUNOUT_NOISE:
    if (inLimit(config_int(), MIN_DELAY_MS, MAX_DELAY_MS))
          infoSettings.runout_noise_ms = config_int();
    break;

  case C_INDEX_RUNOUT_DISTANCE:
    if (inLimit(config_int(), MIN_RUNOUT_DISTANCE, MAX_RUNOUT_DISTANCE))
          infoSettings.runout_distance = config_int();
    break;
#endif
  //---------------------------------------------------------other device specific settings
#ifdef BUZZER_PIN
  case C_INDEX_BUZZER_ON:
    if (inLimit(config_int(),0,1))
      {
        if (config_int() == 0)
          infoSettings.silent = 1;
        else
          infoSettings.silent = 0;
      }
    break;
#endif

#ifdef LED_COLOR_PIN
  case C_INDEX_KNOB_COLOR:
    if (inLimit(config_int(), 0, LED_COLOR_NUM-1))
      infoSettings.knob_led_color = config_int();
    break;
#endif

#ifdef LCD_LED_PWM_CHANNEL
  case C_INDEX_BRIGHTNESS:
    if (inLimit(config_int(), 0, ITEM_BRIGHTNESS_NUM-1))
      infoSettings.lcd_brightness = config_int();
    break;
  case C_INDEX_BRIGHTNESS_IDLE:
    if (inLimit(config_int(), 0, ITEM_BRIGHTNESS_NUM-1))
      infoSettings.lcd_idle_brightness = config_int();
    break;
  case C_INDEX_BRIGHTNESS_IDLE_DELAY:
    if (inLimit(config_int(), 0, ITEM_SECONDS_NUM-1))
      infoSettings.lcd_idle_timer = config_int();
    break;
#endif
  //---------------------------------------------------------CustomG-Code Commands upto 15 custom G-code
  case C_INDEX_CUSTOM_LABEL_1:
  case C_INDEX_CUSTOM_LABEL_2:
  case C_INDEX_CUSTOM_LABEL_3:
  case C_INDEX_CUSTOM_LABEL_4:
  case C_INDEX_CUSTOM_LABEL_5:
  case C_INDEX_CUSTOM_LABEL_6:
  case C_INDEX_CUSTOM_LABEL_7:
  case C_INDEX_CUSTOM_LABEL_8:
  case C_INDEX_CUSTOM_LABEL_9:
  case C_INDEX_CUSTOM_LABEL_10:
  case C_INDEX_CUSTOM_LABEL_11:
  case C_INDEX_CUSTOM_LABEL_12:
  case C_INDEX_CUSTOM_LABEL_13:
  case C_INDEX_CUSTOM_LABEL_14:
  case C_INDEX_CUSTOM_LABEL_15:
    {
      char pchr[LINE_MAX_CHAR];
      strcpy(pchr,strrchr(cur_line,':') + 1);
      int utf8len = getUTF8Length((u8*)pchr);
      int bytelen = strlen(pchr) + 1;
      if (inLimit(utf8len,NAME_MIN_LENGTH,MAX_GCODE_NAME_LENGTH) && inLimit(bytelen,NAME_MIN_LENGTH,MAX_GCODE_LENGTH))
      {
        strcpy(configCustomGcodes->name[customcode_index++], pchr);
        customcode_good[index - C_INDEX_CUSTOM_LABEL_1] = 1; //set name was found ok
      }
      else
      {
        customcode_good[index - C_INDEX_CUSTOM_LABEL_1] = 0;//set name was not ok
      }
    break;
    }
  case C_INDEX_CUSTOM_GCODE_1:
  case C_INDEX_CUSTOM_GCODE_2:
  case C_INDEX_CUSTOM_GCODE_3:
  case C_INDEX_CUSTOM_GCODE_4:
  case C_INDEX_CUSTOM_GCODE_5:
  case C_INDEX_CUSTOM_GCODE_6:
  case C_INDEX_CUSTOM_GCODE_7:
  case C_INDEX_CUSTOM_GCODE_8:
  case C_INDEX_CUSTOM_GCODE_9:
  case C_INDEX_CUSTOM_GCODE_10:
  case C_INDEX_CUSTOM_GCODE_11:
  case C_INDEX_CUSTOM_GCODE_12:
  case C_INDEX_CUSTOM_GCODE_13:
  case C_INDEX_CUSTOM_GCODE_14:
  case C_INDEX_CUSTOM_GCODE_15:
    {
      int fileindex = index - C_INDEX_CUSTOM_GCODE_1; //actual gcode index in config file
      char pchr[LINE_MAX_CHAR];
      strcpy(pchr,strrchr(cur_line,':') + 1);
      int len = strlen(pchr) + 1;
      if (inLimit(len,GCODE_MIN_LENGTH,MAX_GCODE_LENGTH) && (customcode_good[fileindex] == 1)) //check if gcode length is ok and the name was ok
        {
          strcpy(configCustomGcodes->gcode[customcode_index-1], pchr);
        }
      else if (customcode_good[fileindex] == 1) //if name was ok but gcode is not ok then reduce count
        {
          customcode_index--;
        }
    break;
    }
  //---------------------------------------------------------Start, End & Cancel G-codes
  case C_INDEX_START_GCODE_ON:
        infoSettings.send_start_gcode = getOnOff();
    break;

  case C_INDEX_END_GOCODE_ON:
        infoSettings.send_end_gcode = getOnOff();
    break;

  case C_INDEX_CANCEL_GOCODE_ON:
        infoSettings.send_cancel_gcode = getOnOff();
    break;

  case C_INDEX_START_GCODE:
    {
      char * pchr;
      pchr = strrchr(cur_line,':') + 1;
      int len = strlen(pchr);
      if (inLimit(len,GCODE_MIN_LENGTH,MAX_GCODE_LENGTH))
      {
        strcpy(configPrintGcodes->start_gcode, pchr);
      #ifdef CONFIG_DEBUG
        GUI_DispStringInRect(recterrortxt.x0, recterrortxt.y0 + (BYTE_HEIGHT * 2), recterrortxt.x1, recterrortxt.y1, (u8*)configPrintGcodes->start_gcode);
        Delay_ms(1000);
        Delay_ms(1000);

      #endif
      }

    }
    break;
  case C_INDEX_END_GCODE:
    {
      char * pchr;
      pchr = strrchr(cur_line,':') + 1;
      int len = strlen(pchr);
      if (inLimit(len,GCODE_MIN_LENGTH,MAX_GCODE_LENGTH))
      {
        strcpy(configPrintGcodes->end_gcode, pchr);
      #ifdef CONFIG_DEBUG
        GUI_DispStringInRect(recterrortxt.x0, recterrortxt.y0 + (BYTE_HEIGHT * 2), recterrortxt.x1, recterrortxt.y1, (u8*)configPrintGcodes->end_gcode);
        Delay_ms(1000);
        Delay_ms(1000);
      #endif
      }

    }
    break;
  case C_INDEX_CANCEL_GCODE:
    {
      char * pchr;
      pchr = strrchr(cur_line,':') + 1;
      int len = strlen(pchr);
      if (inLimit(len,GCODE_MIN_LENGTH,MAX_GCODE_LENGTH))
      {
        strcpy(configPrintGcodes->cancel_gcode, pchr);
      #ifdef CONFIG_DEBUG
        GUI_DispStringInRect(recterrortxt.x0, recterrortxt.y0 + (BYTE_HEIGHT * 2), recterrortxt.x1, recterrortxt.y1, (u8*)configPrintGcodes->cancel_gcode);
        Delay_ms(1000);
        Delay_ms(1000);
      #endif
      }

    }
    break;

  default:
    break;
  }
}
