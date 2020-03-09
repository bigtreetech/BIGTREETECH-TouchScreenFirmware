#include "PreheatMenu.h"
#include "includes.h"


MENUITEMS preheatItems = {
// title
LABEL_PREHEAT,
// icon                       label
  {
    {ICON_PREHEAT_PLA,          LABEL_PREHEAT_PLA},
    {ICON_PREHEAT_PETG,         LABEL_PREHEAT_PETG},
    {ICON_PREHEAT_ABS,          LABEL_PREHEAT_ABS},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
//    {ICON_PREHEAT_CUSTOM1,      LABEL_PREHEAT_CUSTOM1},
//    {ICON_PREHEAT_CUSTOM2,      LABEL_PREHEAT_CUSTOM2},
    {ICON_PREHEAT_BOTH,         LABEL_PREHEAT_BOTH},
    #ifdef UNIFIED_MENU
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    #else
    {ICON_HEAT,                 LABEL_HEAT},
    #endif
    {ICON_BACK,                 LABEL_BACK},
  }
};

const ITEM itemToolPreheat[] = {
// icon                       label
  {ICON_PREHEAT_BOTH,         LABEL_PREHEAT_BOTH},
  {ICON_BED,                  LABEL_BED},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
};

const u16   preheat_bed_temp[] = PREHEAT_BED;
const u16   preheat_hotend_temp[] = PREHEAT_HOTEND;


void menuPreheat(void)
{
  static TOOLPREHEAT nowHeater = BOTH;
  KEY_VALUES  key_num = KEY_IDLE;

  menuDrawPage(&preheatItems);

  while(infoMenu.menu[infoMenu.cur] == menuPreheat)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
        switch(nowHeater){
          case BOTH:
            heatSetTargetTemp(BED, preheat_bed_temp[key_num]);
            heatSetTargetTemp(heatGetCurrentToolNozzle(), preheat_hotend_temp[key_num]);
            break;
          case BED_PREHEAT:
            heatSetTargetTemp(BED, preheat_bed_temp[key_num]);
            break;
          case NOZZLE0_PREHEAT:
            heatSetTargetTemp(heatGetCurrentToolNozzle(), preheat_hotend_temp[key_num]);
            break;
        }
        break;

      case KEY_ICON_5:
        nowHeater = (TOOLPREHEAT)((nowHeater+1) % 3);
        preheatItems.items[key_num] = itemToolPreheat[nowHeater];
        menuDrawItem(&preheatItems.items[key_num], key_num);;
        break;

      #ifndef UNIFIED_MENU
      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      #endif
      case KEY_ICON_7:
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}
