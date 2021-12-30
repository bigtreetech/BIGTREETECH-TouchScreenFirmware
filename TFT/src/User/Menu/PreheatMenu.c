#include "PreheatMenu.h"
#include "includes.h"

const GUI_POINT preheat_title = {ICON_WIDTH / 2, PREHEAT_TITLE_Y };
const GUI_POINT preheat_val_tool = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_TOOL_Y};
const GUI_POINT preheat_val_bed = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_BED_Y};

const ITEM itemToolPreheat[] = {
  // icon                        label
  {ICON_PREHEAT_BOTH,            LABEL_PREHEAT_BOTH},
  {ICON_BED,                     LABEL_BED},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
};

// Redraw Preheat icon details
void refreshPreheatIcon(PREHEAT_STORE * preheatStore, int8_t index, const ITEM * menuitem)
{
  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = true;

  // set preheat title properties
  lvIcon.lines[0].h_align = CENTER;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = PH_VAL_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = preheat_title;
  lvIcon.lines[0].font = FONT_SIZE_NORMAL;

  // set preheat tool propertites
  lvIcon.lines[1].h_align = RIGHT;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = PH_VAL_COLOR;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = preheat_val_tool;
  lvIcon.lines[1].font = FONT_SIZE_NORMAL;

  // set preheat bed properties
  lvIcon.lines[2].h_align = RIGHT;
  lvIcon.lines[2].v_align = CENTER;
  lvIcon.lines[2].fn_color = PH_VAL_COLOR;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = preheat_val_bed;
  lvIcon.lines[2].font = FONT_SIZE_NORMAL;

  lvIcon.lines[0].text = (uint8_t *)preheatStore->preheat_name[index];

  char temptool[5];
  char tempbed[5];
  sprintf(temptool, "%d", preheatStore->preheat_temp[index]);
  sprintf(tempbed, "%d", preheatStore->preheat_bed[index]);
  lvIcon.lines[1].text = (uint8_t *)temptool;
  lvIcon.lines[2].text = (uint8_t *)tempbed;

  showLiveInfo(index, &lvIcon, menuitem);
}

void menuPreheat(void)
{
  MENUITEMS preheatItems = {
    // title
    LABEL_PREHEAT,
    // icon                          label
    {
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT_BOTH,            LABEL_PREHEAT_BOTH},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  static TOOLPREHEAT nowHeater = BOTH;
  KEY_VALUES key_num = KEY_IDLE;
  PREHEAT_STORE preheatStore;

  preheatItems.items[KEY_ICON_6] = itemToolPreheat[nowHeater];

  W25Qxx_ReadBuffer((uint8_t*)&preheatStore, PREHEAT_STORE_ADDR, sizeof(PREHEAT_STORE));
  menuDrawPage(&preheatItems);

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    refreshPreheatIcon(&preheatStore, i, &preheatItems.items[i]);
  }

  while (MENU_IS(menuPreheat))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
      case KEY_ICON_4:
      case KEY_ICON_5:
        switch (nowHeater)
        {
          case BOTH:
            heatSetTargetTemp(BED, preheatStore.preheat_bed[key_num]);
            heatSetTargetTemp(heatGetCurrentHotend(), preheatStore.preheat_temp[key_num]);
            break;

          case BED_PREHEAT:
            heatSetTargetTemp(BED, preheatStore.preheat_bed[key_num]);
            break;

          case NOZZLE0_PREHEAT:
            heatSetTargetTemp(heatGetCurrentHotend(), preheatStore.preheat_temp[key_num]);
            break;
        }
        refreshPreheatIcon(&preheatStore, key_num, &preheatItems.items[key_num]);
        break;

      case KEY_ICON_6:
        nowHeater = (TOOLPREHEAT)((nowHeater + 1) % 3);
        preheatItems.items[key_num] = itemToolPreheat[nowHeater];
        menuDrawItem(&preheatItems.items[key_num], key_num);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
