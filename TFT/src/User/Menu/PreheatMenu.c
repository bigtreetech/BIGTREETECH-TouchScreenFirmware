#include "PreheatMenu.h"
#include "includes.h"

STRINGS_STORE * preheatnames;


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

void drawPreheatNames(void)
{
    for (int i = 0;i < PREHEAT_COUNT;i++)
  {
    const GUI_RECT *rect;
    rect = rect_of_key + ITEM_PER_PAGE + i;
    GUI_ClearPrect(rect);

    GUI_DispStringInPrect(rect, (u8*)preheatnames->preheat_name[i]);
  }
}

void menuPreheat(void)
{
  MENUITEMS preheatItems = {
  // title
  LABEL_PREHEAT,
    // icon                       label
    {
      {ICON_PREHEAT_PLA,          LABEL_BACKGROUND},
      {ICON_PREHEAT_PETG,         LABEL_BACKGROUND},
      {ICON_PREHEAT_ABS,          LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_PREHEAT_BOTH,         LABEL_PREHEAT_BOTH},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACK,                 LABEL_BACK},
    }
  };

  if(infoSettings.unified_menu !=1)
    {
      preheatItems.items[6].icon = ICON_HEAT;
      preheatItems.items[6].label.index = LABEL_HEAT;
    }

  preheatnames = (STRINGS_STORE*)malloc(sizeof(STRINGS_STORE));
  uint8_t *data_p = (uint8_t *)preheatnames;
  W25Qxx_ReadBuffer(data_p,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));

  static TOOLPREHEAT nowHeater = BOTH;
  KEY_VALUES  key_num = KEY_IDLE;

  menuDrawPage(&preheatItems);
  drawPreheatNames();
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
            heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
            heatSetTargetTemp(heatGetCurrentToolNozzle(), infoSettings.preheat_temp[key_num]);
            break;
          case BED_PREHEAT:
            heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
            break;
          case NOZZLE0_PREHEAT:
            heatSetTargetTemp(heatGetCurrentToolNozzle(), infoSettings.preheat_temp[key_num]);
            break;
        }
        drawPreheatNames();
        break;

      case KEY_ICON_5:
        nowHeater = (TOOLPREHEAT)((nowHeater+1) % 3);
        preheatItems.items[key_num] = itemToolPreheat[nowHeater];
        menuDrawItem(&preheatItems.items[key_num], key_num);;
        break;

      case KEY_ICON_6:
        if(infoSettings.unified_menu != 1)
         {
           infoMenu.menu[++infoMenu.cur] = menuHeat;
         }
        break;

      case KEY_ICON_7:
        free(preheatnames);
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}
