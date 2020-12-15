#include "PreheatMenu.h"
#include "includes.h"

const GUI_POINT preheat_title = {ICON_WIDTH/2, PREHEAT_TITLE_Y };
const GUI_POINT preheat_val_tool = {ICON_WIDTH - BYTE_WIDTH/2, PREHEAT_TOOL_Y};
const GUI_POINT preheat_val_bed = {ICON_WIDTH - BYTE_WIDTH/2, PREHEAT_BED_Y};

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
void refreshPreheatIcon(int8_t preheatnum, int8_t icon_index, const ITEM * menuitem)
{
  STRINGS_STORE preheatnames;
  W25Qxx_ReadBuffer((uint8_t*)&preheatnames,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = true;

  //set preheat title properties
  lvIcon.lines[0].h_align = CENTER;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = WHITE;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = preheat_title;
  lvIcon.lines[0].large_font = false;

  //set preheat tool propertites
  lvIcon.lines[1].h_align = RIGHT;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = WHITE;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = preheat_val_tool;
  lvIcon.lines[1].large_font = false;

  //set preheat bed properties
  lvIcon.lines[2].h_align = RIGHT;
  lvIcon.lines[2].v_align = CENTER;
  lvIcon.lines[2].fn_color = WHITE;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = preheat_val_bed;
  lvIcon.lines[2].large_font = false;

  lvIcon.lines[0].text = (u8 *)preheatnames.preheat_name[preheatnum];

  char temptool[5];
  char tempbed[5];
  sprintf(temptool, "%d", infoSettings.preheat_temp[preheatnum]);
  sprintf(tempbed, "%d", infoSettings.preheat_bed[preheatnum]);
  lvIcon.lines[1].text = (u8 *)temptool;
  lvIcon.lines[2].text = (u8 *)tempbed;

  showLiveInfo(icon_index, &lvIcon, menuitem);
}

void menuPreheat(void)
{
  MENUITEMS preheatItems = {
    // title
    LABEL_PREHEAT,
    // icon                         label
    {{ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT_BOTH,            LABEL_PREHEAT_BOTH},
     {ICON_BACK,                    LABEL_BACK},}
  };

  static TOOLPREHEAT nowHeater = BOTH;
  KEY_VALUES  key_num;

  preheatItems.items[KEY_ICON_6] = itemToolPreheat[nowHeater];

  menuDrawPage(&preheatItems);
  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    refreshPreheatIcon(i, i, &preheatItems.items[i]);
  }

  while(infoMenu.menu[infoMenu.cur] == menuPreheat)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
      case KEY_ICON_4:
      case KEY_ICON_5:
        switch(nowHeater)
        {
          case BOTH:
            heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
            heatSetTargetTemp(heatGetCurrentHotend(), infoSettings.preheat_temp[key_num]);
            break;
          case BED_PREHEAT:
            heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
            break;
          case NOZZLE0_PREHEAT:
            heatSetTargetTemp(heatGetCurrentHotend(), infoSettings.preheat_temp[key_num]);
            break;
        }
        refreshPreheatIcon(key_num, key_num, &preheatItems.items[key_num]);
        break;

      case KEY_ICON_6:
        nowHeater = (TOOLPREHEAT)((nowHeater+1) % 3);
        preheatItems.items[key_num] = itemToolPreheat[nowHeater];
        menuDrawItem(&preheatItems.items[key_num], key_num);
        break;

      case KEY_ICON_7:
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}
