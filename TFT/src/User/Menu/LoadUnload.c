#include "LoadUnload.h"
#include "includes.h"

static u8  item_extruder_i = 0;

void extruderIdReDraw(void)
{
  char tempstr[20];

  sprintf(tempstr, "%3s", extruderDisplay[item_extruder_i]);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

const char* extruderDisplay[] = EXTRUDER_ID;

void menuLoadUnload(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  
  MENUITEMS loadUnloadItems = {
  // title
  LABEL_EXTRUDE,
  // icon                       label
  {{ICON_UNLOAD,               LABEL_UNLOAD},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_LOAD,                 LABEL_LOAD},
    {ICON_NOZZLE,               LABEL_NOZZLE},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},}
  };

  while(infoCmd.count != 0) {loopProcess();}

  menuDrawPage(&loadUnloadItems);
  extruderIdReDraw();

  while(infoMenu.menu[infoMenu.cur] == menuLoadUnload)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        mustStoreCmd("M702 T%d\n",item_extruder_i);
		infoMenu.cur = 0;
        break;

      case KEY_ICON_3:
        mustStoreCmd("M701 T%d\n",item_extruder_i);
		infoMenu.cur = 0;
        break;

      case KEY_ICON_4:
        item_extruder_i = (item_extruder_i + 1) % infoSettings.ext_count;
        extruderIdReDraw();
        break;

      case KEY_ICON_7:
        infoMenu.cur = 0;
        break;

      default:
        break;
    }
    loopProcess();
  }
}
