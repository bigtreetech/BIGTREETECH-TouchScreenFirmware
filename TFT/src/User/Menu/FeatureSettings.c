#include "FeatureSettings.h"
#include "includes.h"


MENUITEMS featureSettingsItems = {
// title
LABEL_FEATURE_SETTINGS,
// icon                       label
 {
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

#define ITEM_PS_ON_NUM 2
const ITEM itemPowerOff[ITEM_PS_ON_NUM] = {
// icon                       label
  {ICON_POWER_SUPPLY,         LABEL_MANUAL_SHUT_DOWN},
  {ICON_POWER_SUPPLY,         LABEL_AUTO_SHUT_DOWN},
};
const  u8 item_power_off[ITEM_PS_ON_NUM] = {0, 1};
static u8  item_power_off_i = 0;
  
#ifdef FIL_RUNOUT_PIN
  #define ITEM_RUNOUT_NUM 3
  const ITEM itemRunout[ITEM_RUNOUT_NUM] = {
  // icon                       label
    {ICON_RUNOUT,               LABEL_RUNOUT_OFF},
    {ICON_RUNOUT,               LABEL_RUNOUT_ON},
    {ICON_RUNOUT,               LABEL_SMART_RUNOUT_ON},
  };
  const  u8 item_runout[ITEM_RUNOUT_NUM] = {FILAMENT_RUNOUT_OFF, FILAMENT_RUNOUT_ON, FILAMENT_SMART_RUNOUT_ON};
  static u8  item_runout_i = 0;
#endif

void menuFeatureSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for(u8 i=0; i<ITEM_PS_ON_NUM; i++)
  {
    if(infoSettings.auto_off == item_power_off[i])
    {
      item_power_off_i = i;
      featureSettingsItems.items[KEY_ICON_0] = itemPowerOff[item_power_off_i];
    }
  }
  
  #ifdef FIL_RUNOUT_PIN
  for(u8 i=0; i<ITEM_RUNOUT_NUM; i++)
  {
    if(infoSettings.runout == item_runout[i])
    {
      item_runout_i = i;
      featureSettingsItems.items[KEY_ICON_1] = itemRunout[item_runout_i];
    }
  }
  #endif  
  
  menuDrawPage(&featureSettingsItems,false);

  while(infoMenu.menu[infoMenu.cur] == menuFeatureSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      #ifdef FIL_RUNOUT_PIN
      case KEY_ICON_0:
        item_power_off_i = (item_power_off_i + 1) % ITEM_PS_ON_NUM;                
        featureSettingsItems.items[key_num] = itemPowerOff[item_power_off_i];
        menuDrawItem(&featureSettingsItems.items[key_num], key_num);
        infoSettings.auto_off = item_power_off[item_power_off_i];
        break;
      #endif
      
      #ifdef FIL_RUNOUT_PIN
      case KEY_ICON_1:
        item_runout_i = (item_runout_i + 1) % ITEM_RUNOUT_NUM;                
        featureSettingsItems.items[key_num] = itemRunout[item_runout_i];
        menuDrawItem(&featureSettingsItems.items[key_num], key_num);
        infoSettings.runout = item_runout[item_runout_i];
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

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
