#include "UnifiedHeat.h"
#include "includes.h"

//1 title, ITEM_PER_PAGE items(icon + label)
const MENUITEMS UnifiedHeatItems = {
// title
LABEL_UNIFIEDHEAT,
// icon                       label
 {{ICON_HEAT,                 LABEL_PREHEAT},
  {ICON_HEAT,                 LABEL_HEAT},
  {ICON_FAN,                  LABEL_FAN},
  {ICON_COOLDOWN,             LABEL_COOLDOWN},
  {ICON_PID,           LABEL_PID_AUTOTUNE_PLA},
  {ICON_PID,           LABEL_PID_AUTOTUNE_ABS},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuUnifiedHeat(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&UnifiedHeatItems);
  while(infoMenu.menu[infoMenu.cur] == menuUnifiedHeat)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuPreheat;   break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuHeat;    break;
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuFan;    break;
      case KEY_ICON_3:
        for(TOOL i = BED; i < HEATER_COUNT; i++)
        {
          heatSetTargetTemp(i, 0);
        }
        break;
      case KEY_ICON_4:
        pidAutotunePLA();
        break;
      case KEY_ICON_5:
        pidAutotuneABS();
        break;

      case KEY_ICON_7: infoMenu.cur--;      break;
      default:break;
    }
    loopProcess();
  }
}


void pidAutotunePLA(void)
{
  u16 key_num = IDLE_TOUCH;

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_PID_AUTOTUNE_WARNING), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

  while(infoMenu.menu[infoMenu.cur] == menuUnifiedHeat)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        storeCmd("M106 P0 S255\n");
        mustStoreCmd("M42 P6 S255\nM303 E0 S%d C8 U1\nM500\nM42 P4 S255\nM303 E-1 S%d C8 U1\nM500\nM42 P4 S0\nM42 P5 S0\nM42 P6 S0\nM107 P0\n", infoSettings.preheat_temp[0], infoSettings.preheat_bed[0] );
        infoMenu.cur--;
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;
    }
    loopProcess();
  }
}

void pidAutotuneABS(void)
{
  u16 key_num = IDLE_TOUCH;

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_PID_AUTOTUNE_WARNING), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

  while(infoMenu.menu[infoMenu.cur] == menuUnifiedHeat)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        mustStoreCmd("M42 P6 S255\nM303 E0 S%d C8 U1\nM500\nM42 P4 S255\nM303 E-1 S%d C8 U1\nM500\nM42 P4 S0\nM42 P5 S0\nM42 P6 S0\nM107 P0\n", infoSettings.preheat_temp[2], infoSettings.preheat_bed[2]);
        infoMenu.cur--;
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;
    }
    loopProcess();
  }

}