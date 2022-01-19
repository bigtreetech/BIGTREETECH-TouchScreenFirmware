//TG MODIFIED BY T.GIOIOSA
#include "includes.h"
#include "More.h"

MENUITEMS moreItems = {  //TG removed const so this menu can be dynamically changed
  // title
  LABEL_MORE,
  // icon                          label
  {
	{ICON_SPINDLE,                 LABEL_SPINDLE},
	{ICON_VACUUM,                  LABEL_VACUUM},
	{ICON_FAN,                     LABEL_FAN},        //TG 2/20/21 was EXTRUDE, removed for CNC
	{ICON_PERCENTAGE,              LABEL_PERCENTAGE},
	{ICON_FEATURE_SETTINGS,        LABEL_FEATURE_SETTINGS},
	{ICON_MACHINE_SETTINGS,        LABEL_MACHINE_SETTINGS},
    #ifdef LOAD_UNLOAD_M701_M702
      {ICON_LOAD,                 LABEL_LOAD_UNLOAD_SHORT}, //TG 8/21/21 NEED TO CHECK THIS ICON!!
    #else
      {ICON_GCODE,                   LABEL_TERMINAL},
    #endif
    {ICON_BACK,                    LABEL_BACK},
  }
};

void isPauseExtrude(void)
{
  if(printPause(true,false))                   //TG 8/21/21 fixed for V27, and removed Extrude module
    infoMenu.menu[infoMenu.cur] = (infoSettings.laser_mode == 1) ? menuLaser : menuSpindle;    //TG 2/8/21 was menuExtrude
}

void isPauseLoadUnload(void)
{
  if (printPause(true, false))
    infoMenu.menu[infoMenu.cur] = menuLoadUnload;
}

void menuMore(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  moreItems.items[0].icon = (infoSettings.laser_mode == 1) ? ICON_LASER : ICON_SPINDLE;				//TG added
  moreItems.items[0].label.index = (infoSettings.laser_mode == 1) ? LABEL_LASER : LABEL_SPINDLE;	//TG added

  menuDrawPage(&moreItems);

  while (infoMenu.menu[infoMenu.cur] == menuMore)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:	//TG modified this
        if (isPrinting() && !isPaused())  // need paused before extrude
        {
          setDialogText(LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, isPauseExtrude, NULL, NULL);
        }
        else
        {
          infoMenu.menu[++infoMenu.cur] = menuSpindle;
        }
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuVacuum;
        break;

      case KEY_ICON_2:
		infoMenu.menu[++infoMenu.cur] = menuFan;
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
          if (isPrinting() && !isPaused())  // need paused before extrude
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
