#include "SettingsMenu.h"
#include "includes.h"

static uint8_t firmare_name[64] = "Unknow system"; // Marlin firmware version
uint8_t machine_type[64] = "3D Printer"; // Marlin machine type

void infoSetFirmwareName(uint8_t *name, uint8_t name_len)
{
  if (name_len > sizeof(firmare_name) - 1)
    name_len = sizeof(firmare_name) - 1;
  uint8_t i;
  for (i = 0; i < name_len; i++) {
    firmare_name[i] = name[i];
  }
  firmare_name[i] = 0;
}

void infoSetMachineType(uint8_t *machine, uint8_t type_len)
{
  if (type_len > sizeof(machine_type) - 1)
    type_len = sizeof(machine_type) - 1;
  uint8_t i;
  for (i = 0; i < type_len; i++) {
    machine_type[i] = machine[i];
  }
  machine_type[i] = 0;
  statusScreen_setReady();
}

// Version infomation
void menuInfo(void)
{
  char buf[128];
  const GUI_POINT clocks[] = {
    {0 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
    {1 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
    {2 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
    {0 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},
    {1 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},
    {2 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},};
  const char* hardware = "BIGTREETECH_" HARDWARE_VERSION;
  const char* firmware = HARDWARE_VERSION"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__;

  GUI_Clear(infoSettings.bg_color);
  GUI_SetColor(GRAY);

  my_sprintf(buf, "SYS:%dMhz", mcuClocks.rccClocks.SYSCLK_Frequency / 1000000);
  GUI_DispString(clocks[0].x, clocks[0].y, (uint8_t *)buf);

  my_sprintf(buf, "APB1:%dMhz", mcuClocks.rccClocks.PCLK1_Frequency / 1000000);
  GUI_DispString(clocks[1].x, clocks[1].y, (uint8_t *)buf);

  my_sprintf(buf, "P1Tim:%dMhz", mcuClocks.PCLK1_Timer_Frequency / 1000000);
  GUI_DispString(clocks[2].x, clocks[2].y, (uint8_t *)buf);

  my_sprintf(buf, "AHB:%dMhz", mcuClocks.rccClocks.HCLK_Frequency / 1000000);
  GUI_DispString(clocks[3].x, clocks[3].y, (uint8_t *)buf);

  my_sprintf(buf, "APB2:%dMhz", mcuClocks.rccClocks.PCLK2_Frequency / 1000000);
  GUI_DispString(clocks[4].x, clocks[4].y, (uint8_t *)buf);

  my_sprintf(buf, "P2Tim:%dMhz", mcuClocks.PCLK2_Timer_Frequency / 1000000);
  GUI_DispString(clocks[5].x, clocks[5].y, (uint8_t *)buf);

  GUI_HLine(0, clocks[5].y + BYTE_HEIGHT, LCD_WIDTH);

  //spi flash info
  float usedMB = (float)FLASH_USED/1048576;
  sprintf(buf, "Used %.2f%% (%.2fMB/%uMB)", flashUsedPercentage(), usedMB, (W25Qxx_ReadCapacity()/1048576));

  const uint16_t top_y = (LCD_HEIGHT - (9 * BYTE_HEIGHT)) / 2; // 8 firmware info lines + 1 SPI flash info line
  const uint16_t start_x = sizeof("Firmware:") * BYTE_WIDTH;
  const GUI_RECT version[5] = {
    {start_x, top_y + 0*BYTE_HEIGHT, LCD_WIDTH, top_y + 2*BYTE_HEIGHT},
    {start_x, top_y + 2*BYTE_HEIGHT, LCD_WIDTH, top_y + 4*BYTE_HEIGHT},
    {start_x, top_y + 4*BYTE_HEIGHT, LCD_WIDTH, top_y + 6*BYTE_HEIGHT},
    {start_x, top_y + 6*BYTE_HEIGHT, LCD_WIDTH, top_y + 8*BYTE_HEIGHT},
    {start_x, top_y + 8*BYTE_HEIGHT, LCD_WIDTH, top_y + 9*BYTE_HEIGHT},
    };
  //draw titles
  GUI_DispString(0, version[0].y0, (uint8_t *)"System  :");
  GUI_DispString(0, version[1].y0, (uint8_t *)"Machine :");
  GUI_DispString(0, version[2].y0, (uint8_t *)"Board   :");
  GUI_DispString(0, version[3].y0, (uint8_t *)"Firmware:");
  GUI_DispString(0, version[4].y0, (uint8_t *)"SPIFlash:");

  //draw info
  GUI_SetColor(0xDB40);
  GUI_DispStringInPrectEOL(&version[0], firmare_name);
  GUI_DispStringInPrectEOL(&version[1], machine_type);
  GUI_DispStringInPrectEOL(&version[2], (uint8_t *)hardware);
  GUI_DispStringInPrectEOL(&version[3], (uint8_t *)firmware);
  GUI_DispStringInPrectEOL(&version[4], (uint8_t *)buf);

  GUI_SetColor(GRAY);

  GUI_HLine(0, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH);

  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  while(!isPress()) loopBackEnd();
  while(isPress())  loopBackEnd();

  GUI_RestoreColorDefault();
  infoMenu.cur--;
}

void menuSettings(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  const MENUITEMS settingsItems = {
    // title
    LABEL_SETTINGS,
    // icon                         label
    {{ICON_SCREEN_SETTINGS,         LABEL_SCREEN_SETTINGS},
     {ICON_MACHINE_SETTINGS,        LABEL_MACHINE_SETTINGS},
     {ICON_FEATURE_SETTINGS,        LABEL_FEATURE_SETTINGS},
     {ICON_SCREEN_INFO,             LABEL_SCREEN_INFO},
     {ICON_CONNECTION_SETTINGS,     LABEL_CONNECTION_SETTINGS},
     {ICON_TUNING,                  LABEL_TUNING},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&settingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuScreenSettings;
        break;

      case KEY_ICON_1:
        mustStoreCmd("M503 S0\n");
        infoMenu.menu[++infoMenu.cur] = menuMachineSettings;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuFeatureSettings;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuConnectionSettings;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuTuning;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }
}
