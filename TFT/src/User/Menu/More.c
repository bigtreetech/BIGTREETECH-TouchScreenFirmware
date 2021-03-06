#include "includes.h"
#include "More.h"

const MENUITEMS moreItems = {
  // title
  LABEL_MORE,
  // icon                         label
  {{ICON_HEAT,                    LABEL_HEAT},
   {ICON_FAN,                     LABEL_FAN},
   {ICON_EXTRUDE,                 LABEL_EXTRUDE},
   {ICON_PERCENTAGE,              LABEL_PERCENTAGE},
   {ICON_FEATURE_SETTINGS,        LABEL_FEATURE_SETTINGS},
   {ICON_MACHINE_SETTINGS,        LABEL_MACHINE_SETTINGS},
#ifdef LOAD_UNLOAD_M701_M702
   {ICON_EXTRUDE,                 LABEL_LOAD_UNLOAD_SHORT},
#else
   {ICON_GCODE,                   LABEL_TERMINAL},
#endif
   {ICON_BACK,                    LABEL_BACK},}
};

void isPauseExtrude(void)
{
  if(setPrintPause(true,false))
    infoMenu.menu[infoMenu.cur] = menuExtrude;
}

void isPauseLoadUnload(void)
{
  if(setPrintPause(true,false))
    infoMenu.menu[infoMenu.cur] = menuLoadUnload;
}

void menuMore(void)
{
  KEY_VALUES key_num;

  menuDrawPage(&moreItems);

  while(infoMenu.menu[infoMenu.cur] == menuMore)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;

      case KEY_ICON_2:
        if (isPrinting() && !isPause()) // need paused before extrude
        {
          setDialogText(LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, isPauseExtrude, NULL, NULL);
        }
        else
        {
          infoMenu.menu[++infoMenu.cur] = menuExtrude;
        }
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuFeatureSettings;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuMachineSettings;
        break;

      case KEY_ICON_6:
        #ifdef LOAD_UNLOAD_M701_M702
          if (isPrinting() && !isPause()) // need paused before extrude
          {
            setDialogText(LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, isPauseLoadUnload, NULL, NULL);
          }
          else
          {
            infoMenu.menu[++infoMenu.cur] = menuLoadUnload;
          }
        #else
          infoMenu.menu[++infoMenu.cur] = menuTerminal;
        #endif
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
