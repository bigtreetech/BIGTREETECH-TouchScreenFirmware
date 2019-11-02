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

#define ITEM_RUNOUT_NUM 3
const ITEM itemRunout[ITEM_RUNOUT_NUM] = {
// icon                       label
  {ICON_RUNOUT,             LABEL_RUNOUT_OFF},
  {ICON_RUNOUT,             LABEL_RUNOUT_ON},
  {ICON_RUNOUT,             LABEL_SMART_RUNOUT_ON},
};
const  u32 item_runout[ITEM_RUNOUT_NUM] = {FILAMENT_RUNOUT_OFF, FILAMENT_RUNOUT_ON, FILAMENT_SMART_RUNOUT_ON};
static u8  item_runout_i = 0;

void menuFeatureSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  #ifdef FIL_RUNOUT_PIN
  for(u8 i=0; i<ITEM_RUNOUT_NUM; i++)
  {
    if(infoSettings.runout == item_runout[i])
    {
      item_runout_i = i;
      featureSettingsItems.items[KEY_ICON_0] = itemRunout[item_runout_i];
    }
  }
  #endif
  menuDrawPage(&featureSettingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuFeatureSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      #ifdef FIL_RUNOUT_PIN
      case KEY_ICON_0:
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
