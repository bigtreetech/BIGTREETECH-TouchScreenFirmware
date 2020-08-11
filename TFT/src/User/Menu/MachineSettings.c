#include "MachineSettings.h"

uint8_t enabled_gcodes[CUSTOM_GCODES_COUNT];
uint8_t gcode_num;
uint8_t gc_page_count;
uint8_t gc_cur_page = 0;

CUSTOM_GCODES * customcodes = NULL;

LISTITEMS customItems = {
// title
LABEL_CUSTOM,
// icon                 ItemType      Item Title        item value text(only for custom value)
{
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_PAGEUP,     LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_PAGEDOWN,   LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACK,       LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},}
};

//
//load values on page change and reload
//
void loaditemsCustomGcode()
{
  for (uint32_t i = 0; i < LISTITEM_PER_PAGE; i++)
  {
    uint32_t item_index = gc_cur_page*LISTITEM_PER_PAGE + i;
    if (item_index < gcode_num){
      customItems.items[i].icon = ICONCHAR_CODE;
      customItems.items[i].titlelabel.index = LABEL_DYNAMIC;
      setDynamicLabel(i,customcodes->name[item_index]);
    }
    else{
      customItems.items[i].icon = ICONCHAR_BACKGROUND;
      customItems.items[i].titlelabel.index = LABEL_BACKGROUND;
    }
  }
  // set page up down button according to page count and current page
  if (gcode_num <= LISTITEM_PER_PAGE)
  {
    customItems.items[5].icon = ICONCHAR_BACKGROUND;
    customItems.items[6].icon = ICONCHAR_BACKGROUND;
  }
  else
  {
    if(gc_cur_page == 0){
      customItems.items[5].icon = ICONCHAR_BACKGROUND;
      customItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if(gc_cur_page == (gc_page_count-1)){
      customItems.items[5].icon = ICONCHAR_PAGEUP;
      customItems.items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      customItems.items[5].icon = ICONCHAR_PAGEUP;
      customItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
}

void menuCustom(void)
{
  //load custom codes
  CUSTOM_GCODES tempcodes;
  customcodes = &tempcodes;
  W25Qxx_ReadBuffer((u8*)&tempcodes,CUSTOM_GCODE_ADDR,sizeof(CUSTOM_GCODES));
  gcode_num = customcodes->count;

  gc_page_count = (gcode_num+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE;

  KEY_VALUES key_num = KEY_IDLE;

  loaditemsCustomGcode();
  menuDrawListPage(&customItems);

  while(infoMenu.menu[infoMenu.cur] == menuCustom)
  {
    key_num = menuKeyGetValue();

    if (key_num < LISTITEM_PER_PAGE)
    {
      uint32_t item_index = gc_cur_page * LISTITEM_PER_PAGE + key_num;
      if (item_index < gcode_num)
      {
        mustStoreScript(customcodes->gcode[item_index]);
      }
    }
    switch(key_num)
    {
    case KEY_ICON_5:
      if (gc_page_count > 1)
      {
        if (gc_cur_page > 0)
        {
          gc_cur_page--;
          loaditemsCustomGcode();
          menuRefreshListPage();
        }
      }
      break;
    case KEY_ICON_6:
      if (gc_page_count > 1)
      {
        if (gc_cur_page < gc_page_count - 1)
        {
          gc_cur_page++;
          loaditemsCustomGcode();
          menuRefreshListPage();
        }
      }
      break;
    case KEY_ICON_7:
      gc_cur_page = 0;
      infoMenu.cur--;
      break;
    default:
      break;
    }
    loopProcess();
  }
}

void menuRGBSettings(void)
{
  MENUITEMS RGBItems = {
  // title
  LABEL_RGB_SETTINGS,
  // icon                       label
  {{ICON_RGB_RED,              LABEL_RED},
    {ICON_RGB_GREEN,            LABEL_GREEN},
    {ICON_RGB_BLUE,             LABEL_BLUE},
    {ICON_RGB_WHITE,            LABEL_WHITE},
    {ICON_RGB_OFF,              LABEL_OFF},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&RGBItems);

  while(infoMenu.menu[infoMenu.cur] == menuRGBSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:  //Red
        storeCmd("M150 R255 U0 B0 P255\n");
        break;

      case KEY_ICON_1:  //Green
        storeCmd("M150 R0 U255 B0 P255\n");
        break;

      case KEY_ICON_2:  //Blue
        storeCmd("M150 R0 U0 B255 P255\n");
        break;

      case KEY_ICON_3:  //White
        storeCmd("M150 R255 U255 B255 P255\n");
        break;

      case KEY_ICON_4:  //Turn Off
        storeCmd("M150 R0 U0 B0 P0\n");
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

#define ITEM_MACHINE_SETTINGS_SUBMENU_NUM 3

ITEM itemMachineSettingsSubmenu[ITEM_MACHINE_SETTINGS_SUBMENU_NUM] = {
  // icon                         label
  {ICON_EEPROM_SAVE,              LABEL_SAVE},
  {ICON_EEPROM_RESTORE,           LABEL_RESTORE},
  {ICON_EEPROM_RESET,             LABEL_RESET},
};

#if QUICK_EEPROM_BUTTON == 1
static uint8_t curSubmenu = 0;
#endif

void menuMachineSettings(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS machineSettingsItems = {
    // title
    LABEL_MACHINE_SETTINGS,
    // icon                         label
    {{ICON_PARAMETER,               LABEL_PARAMETER_SETTING},
     {ICON_GCODE,                   LABEL_TERMINAL},
     {ICON_CUSTOM,                  LABEL_CUSTOM},
     {ICON_RGB_SETTINGS,            LABEL_RGB_SETTINGS},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

#if QUICK_EEPROM_BUTTON == 1
  if (infoMachineSettings.EEPROM == 1)
  {
    machineSettingsItems.items[KEY_ICON_5].icon = ICON_PAGE_DOWN;
    machineSettingsItems.items[KEY_ICON_5].label.index = LABEL_NEXT;

    machineSettingsItems.items[KEY_ICON_6] = itemMachineSettingsSubmenu[curSubmenu];
  }
#endif

  menuDrawPage(&machineSettingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuMachineSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuParameterSettings;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuCustom;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuRGBSettings;
        break;

#if QUICK_EEPROM_BUTTON == 1
      // change submenu
      case KEY_ICON_5:
        if (infoMachineSettings.EEPROM == 1)
        {
          curSubmenu = (curSubmenu + 1) % ITEM_MACHINE_SETTINGS_SUBMENU_NUM;

          machineSettingsItems.items[KEY_ICON_6] = itemMachineSettingsSubmenu[curSubmenu];

          menuDrawItem(&machineSettingsItems.items[KEY_ICON_6], KEY_ICON_6);
        }
        break;

      // handle submenu
      case KEY_ICON_6:
        if (infoMachineSettings.EEPROM == 1)
        {
          switch (curSubmenu)
          {
            // save to EEPROM
            case 0:
              showDialog(DIALOG_TYPE_QUESTION, textSelect(machineSettingsItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
              break;

            // restore from EEPROM
            case 1:
              showDialog(DIALOG_TYPE_QUESTION, textSelect(machineSettingsItems.title.index), textSelect(LABEL_EEPROM_RESTORE_INFO),
                textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), restoreEepromSettings, NULL, NULL);
              break;

            // reset EEPROM
            case 2:
              showDialog(DIALOG_TYPE_QUESTION, textSelect(machineSettingsItems.title.index), textSelect(LABEL_EEPROM_RESET_INFO),
                textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), resetEepromSettings, NULL, NULL);
              break;

            default:
              break;
          }
        }
        break;
#endif

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }
}
