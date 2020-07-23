#include "UnifiedMove.h"
#include "includes.h"


void menuUnifiedMove(void)
{
  //1 title, ITEM_PER_PAGE items(icon + label)
  MENUITEMS UnifiedMoveItems = {
  // title
  LABEL_UNIFIEDMOVE,
  // icon                       label
   {{ICON_HOME,                 LABEL_HOME},
    {ICON_MOVE,                 LABEL_MOVE},
    {ICON_LEVELING,             LABEL_ABL},
    {ICON_MANUAL_LEVEL,         LABEL_LEVELING},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK}}
  };

  if(infoMachineSettings.autoLevel == 1){
    UnifiedMoveItems.items[2].icon = ICON_LEVELING;
    UnifiedMoveItems.items[2].label.index = LABEL_ABL;
    UnifiedMoveItems.items[3].icon = ICON_MANUAL_LEVEL;
    UnifiedMoveItems.items[3].label.index = LABEL_LEVELING;

    if(infoSettings.autoLevelState == 1) {
      UnifiedMoveItems.items[6].icon = ICON_EEPROM_SAVE;
      UnifiedMoveItems.items[6].label.index = LABEL_ABL_ENABLE;
    }
    else {
      UnifiedMoveItems.items[6].icon = ICON_STOP;
      UnifiedMoveItems.items[6].label.index = LABEL_ABL_DISABLE;
    }
  }
  else{
    UnifiedMoveItems.items[2].icon = ICON_MANUAL_LEVEL;
    UnifiedMoveItems.items[2].label.index = LABEL_LEVELING;
    UnifiedMoveItems.items[3].icon = ICON_BACKGROUND;
    UnifiedMoveItems.items[3].label.index = LABEL_BACKGROUND;
  }

  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&UnifiedMoveItems);
  while(infoMenu.menu[infoMenu.cur] == menuUnifiedMove)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuHome; break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove; break;
      case KEY_ICON_2:
                      if(infoMachineSettings.autoLevel == 1){
                        if (infoSettings.enable_ubl == 1) {
                          infoMenu.menu[++infoMenu.cur] = menuAutoLevelingUBL;
                        }
                        else {
                          infoMenu.menu[++infoMenu.cur] = menuAutoLeveling;
                        }
                      }
                      else{
                        infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
                      }
                      break;
      case KEY_ICON_3:
                      if(infoMachineSettings.autoLevel == 1){
                        infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
                      }
                      break;
      case KEY_ICON_6:
                      if(infoMachineSettings.autoLevel == 1){
                        if(infoSettings.autoLevelState == 1){
                          storeCmd("M420 S0\n");
                        }
                        else {
                          storeCmd("M420 S1\n");
                        }
                      }
                      break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default: break;
    }
    loopProcess();
  }
}
