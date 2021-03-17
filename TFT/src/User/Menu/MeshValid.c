
#include "MeshValid.h"
#include "includes.h"


const GUI_POINT meshValid_title = {ICON_WIDTH / 2, PREHEAT_TITLE_Y };
const GUI_POINT meshValid_val_tool = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_TOOL_Y};
const GUI_POINT meshValid_val_bed = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_BED_Y};


const ITEM itemToolMeshValid[] = {
  // icon                        label
  {ICON_BABYSTEP,                LABEL_BABYSTEP},
  {ICON_BED,                     LABEL_BED},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
};

// Redraw Preheat icon details
void refreshMeshValidIcon(int8_t preheatnum, int8_t icon_index, const ITEM * menuitem)
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
  lvIcon.lines[0].pos = meshValid_title;
  lvIcon.lines[0].large_font = false;

  //set preheat tool propertites
  lvIcon.lines[1].h_align = RIGHT;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = WHITE;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = meshValid_val_tool;
  lvIcon.lines[1].large_font = false;

  //set preheat bed properties
  lvIcon.lines[2].h_align = RIGHT;
  lvIcon.lines[2].v_align = CENTER;
  lvIcon.lines[2].fn_color = WHITE;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = meshValid_val_bed;
  lvIcon.lines[2].large_font = false;

  lvIcon.lines[0].text = (uint8_t *)preheatnames.preheat_name[preheatnum];

  char temptool[5];
  char tempbed[5];
  sprintf(temptool, "%d", infoSettings.preheat_temp[preheatnum]);
  sprintf(tempbed, "%d", infoSettings.preheat_bed[preheatnum]);
  lvIcon.lines[1].text = (uint8_t *)temptool;
  lvIcon.lines[2].text = (uint8_t *)tempbed;

  showLiveInfo(icon_index, &lvIcon, menuitem);
}

void menuMeshValid(void)
{
  MENUITEMS meshValidItems = {
    // title
    LABEL_MESH_VALID,
    // icon                          label
    {
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_BABYSTEP,                LABEL_BABYSTEP},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  static TOOLPREHEAT nowHeater = BOTH;
  KEY_VALUES  key_num;

  meshValidItems.items[KEY_ICON_6] = itemToolMeshValid[nowHeater];

  menuDrawPage(&meshValidItems);
  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    refreshMeshValidIcon(i, i, &meshValidItems.items[i]);
  }

  while (infoMenu.menu[infoMenu.cur] == menuMeshValid)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
        //MESHVALID PLA
        case KEY_ICON_0:
        storeCmd("G26 H200 B60 R99\n");
        break;

        //MESHVALID PETG
        case KEY_ICON_1:
         storeCmd("G26 H240 B70 R99\n");
        break;

        //MESHVALID ABS
        case KEY_ICON_2:
        storeCmd("G26 H230 B90 R99\n");
        break;

        //MESHVALID WOOD
        case KEY_ICON_3:
        storeCmd("G26 H170 B50 R99\n");
        break;

        //MESHVALID TPU
        case KEY_ICON_4:
        storeCmd("G26 H220 B50 R99\n");
        break;

        //MESHVALID NYLON
        case KEY_ICON_5:
        storeCmd("G26 H250 B90 R99\n");
        break;

        //Menu babystep 
        case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuBabystep;
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
