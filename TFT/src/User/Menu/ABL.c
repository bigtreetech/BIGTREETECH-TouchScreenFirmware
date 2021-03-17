#include "ABL.h"
#include "includes.h"
#include "coordinate.h"
#include "menu.h"
#include "MachineParameters.h"

static uint8_t ublSlot;
static bool ublIsSaving = true;
static bool ublSlotSaved = false;

// called by parseAck() to notify ABL process status
void ablUpdateStatus(bool succeeded)
{
  bool savingEnabled = true;

  init_label(tempTitle);
  tempTitle.index = LABEL_ABL_SETTINGS;
  LABELCHAR(tempMsg, LABEL_BL_COMPLETE);

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
    {
      tempTitle.index = LABEL_ABL_SETTINGS_BBL;
      break;
    }
    case BL_UBL:
    {
      savingEnabled = false;
      tempTitle.index = LABEL_ABL_SETTINGS_UBL;

      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_BL_SMART_FILL));
      break;
    }
    default:
      break;
  }

  if (succeeded)  // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(sound_success);

    if (savingEnabled && infoMachineSettings.EEPROM == 1)
    {
      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));
      setDialogText(tempTitle.index, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, tempTitle.index, (uint8_t *) tempMsg);
    }
  }
  else  // if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, tempTitle.index, LABEL_PROCESS_ABORTED);
  }
}

void ublSaveloadConfirm(void)
{
  if (!ublIsSaving)
  {
    storeCmd("G29 L%d\n", ublSlot);
  }
  else
  {
    ublSlotSaved = storeCmd("G29 S%d\n", ublSlot);
  }
}

void menuUBLSaveLoad(void)
{
  MENUITEMS UBLSaveLoadItems = {
    // title
    LABEL_ABL_SETTINGS_UBL_SAVE,
    // icon                          label
    {
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT0},
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT1},
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT2},
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT3},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (!ublIsSaving)
  {
    UBLSaveLoadItems.title.index = LABEL_ABL_SETTINGS_UBL_LOAD;
    for (int i = 0; i < 4; i++)
    {
      UBLSaveLoadItems.items[i].icon = ICON_EEPROM_RESTORE;
    }
  }

  menuDrawPage(&UBLSaveLoadItems);

  while (infoMenu.menu[infoMenu.cur] == menuUBLSaveLoad)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
        ublSlot = key_num;

        setDialogText(UBLSaveLoadItems.title.index, LABEL_CONFIRMATION, LABEL_CONFIRM, LABEL_CANCEL);
        showDialog(DIALOG_TYPE_QUESTION, ublSaveloadConfirm, NULL, NULL);
        break;

      case KEY_ICON_7:
        if (ublSlotSaved == true && infoMachineSettings.EEPROM == 1)
        {
          ublSlotSaved = false;

          setDialogText(LABEL_ABL_SETTINGS_UBL, LABEL_ABL_SLOT_EEPROM, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        else
        {
          ublSlotSaved = false;
          infoMenu.cur--;
        }
        break;

      default:
        break;
    }

    loopProcess();
  }
}

void menuUBLSave(void)
{
  ublIsSaving = true;
  infoMenu.menu[++infoMenu.cur] = menuUBLSaveLoad;
}

void menuUBLLoad(void)
{
  ublIsSaving = false;
  infoMenu.menu[++infoMenu.cur] = menuUBLSaveLoad;
}

void ScanLevelCorner(u8 pointer)
{
  s16 pointPosition[4][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge}, 
  };
  mustStoreCmd("M401\n");
  mustStoreCmd("G30 E0 X%d Y%d\n", (s16)pointPosition[pointer][0], (s16)pointPosition[pointer][1]);
  mustStoreCmd("G1 Z15\n");
  mustStoreCmd("M17 X Y Z\n");

}

void refreshLevelCornerValue(MENUITEMS levelItems) 
{
  char tempstr[10];
  int valIndex[4] = {4,6,2,0};
  int valPos;
  int valPosSub;
  LIVE_INFO lvIcon;
  lvIcon.lines[0].pos = ss_val_point;
  lvIcon.lines[1].pos = ss_val_point;
  lvIcon.lines[2].pos = ss_val_point;
  lvIcon.lines[3].pos = ss_val_point;

  if ( (int)GetLevelCornerPosition(0) != 0)
  {
    if (((int)GetLevelCornerPosition(0) >= 1) && ((int)GetLevelCornerPosition(0) <= 4) )
    {
      valPos = (int)GetLevelCornerPosition(0);
      valPosSub = (int)GetLevelCornerPosition(0)-1;     
      sprintf(tempstr, "  %1.4f  ", GetLevelCornerPosition(valPos));
      lvIcon.lines[valPosSub].text = (uint8_t*)tempstr;
      showLevelCornerLiveInfo(valIndex[valPosSub], valPosSub, &lvIcon, &levelItems.items[valIndex[valPosSub]]); 
      SetLevelCornerPosition(0, 0);  
    }
  } 
}

void menuLEVELCORNERLoad(void)
{ 
  //char tempMsg[40];
  KEY_VALUES key_num = KEY_IDLE;
  MENUITEMS LEVELCORNERItems = {
    // title
    LABEL_LEVELCORNER,
    // icon                         label
    {{ICON_LEVELCORNER4, LABEL_BACKGROUND},
     {ICON_RESET_VALUE, LABEL_START},
     {ICON_LEVELCORNER3, LABEL_BACKGROUND},
     {ICON_LEVEL_EDGE_DISTANCE, LABEL_DISTANCE},
     {ICON_LEVELCORNER1, LABEL_BACKGROUND},
     {ICON_BACKGROUND, LABEL_BACKGROUND},
     {ICON_LEVELCORNER2, LABEL_BACKGROUND},
     {ICON_BACK, LABEL_BACK}}
  };
  
  menuDrawPage(&LEVELCORNERItems);

  /* Init Probe Offset in parseAck to get probe offset X and Y*/
  mustStoreCmd("M851\n");
  
  /* Init Coordinate */
  if (coordinateIsKnown() == false) 
  {
    mustStoreCmd("G28\n");
  }
  /* Check min edge limit for the probe with probe offset set in parseACK.c */
  uint8_t edge_min = MAX(ABS(getParameter((s16)P_PROBE_OFFSET, X_STEPPER)),ABS((s16)getParameter(P_PROBE_OFFSET, Y_STEPPER))) + 1;
  if (infoSettings.level_edge < edge_min) 
  {
    infoSettings.level_edge = ((LEVELING_EDGE_DISTANCE >= edge_min)?LEVELING_EDGE_DISTANCE:edge_min);
  }
  
  /* Scan all 4 corner */
  ScanLevelCorner(0);
  ScanLevelCorner(1);
  ScanLevelCorner(2);
  ScanLevelCorner(3);

  while (infoMenu.menu[infoMenu.cur] == menuLEVELCORNERLoad)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        ScanLevelCorner(3);
        break;
      case KEY_ICON_1:
        ScanLevelCorner(0);
        ScanLevelCorner(1);
        ScanLevelCorner(2);
        ScanLevelCorner(3);
        break;
      case KEY_ICON_2:
        ScanLevelCorner(2);
        break;
      case KEY_ICON_3:
        {
          char tempstr[30];
          
          sprintf(tempstr, "%Min:%d | Max:%d", edge_min, LEVELING_EDGE_DISTANCE_MAX);

          int val = numPadInt((u8 *)tempstr, infoSettings.level_edge, LEVELING_EDGE_DISTANCE_DEFAULT, false);
          infoSettings.level_edge = NOBEYOND(LEVELING_EDGE_DISTANCE_MIN, val, LEVELING_EDGE_DISTANCE_MAX);
          infoSettings.level_edge = ((val >= edge_min)?val:edge_min);

          menuDrawPage(&LEVELCORNERItems);
        }
        break;
      case KEY_ICON_4:
        ScanLevelCorner(0);
        break;
      case KEY_ICON_6:
        ScanLevelCorner(1);
        break;
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default:
        break;
    }
    refreshLevelCornerValue(LEVELCORNERItems);    
    loopProcess();
  }
}

void menuABL(void)
{
  MENUITEMS autoLevelingItems = {
    // title
    LABEL_ABL_SETTINGS,
    // icon                          label
    {
      {ICON_LEVELING,                LABEL_START},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BLTOUCH,                 LABEL_BLTOUCH},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_HEAT,                    LABEL_PREHEAT},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoSettings.touchmi_sensor != 0)
  {
    autoLevelingItems.items[4].icon = ICON_NOZZLE;
    autoLevelingItems.items[4].label.index = LABEL_TOUCHMI;
  }

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_BBL;
      break;

    case BL_UBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_UBL;
      autoLevelingItems.items[1].icon = ICON_EEPROM_SAVE;
      autoLevelingItems.items[1].label.index = LABEL_SAVE;
      autoLevelingItems.items[2].icon = ICON_EEPROM_RESTORE;
      autoLevelingItems.items[2].label.index = LABEL_LOAD;
      break;

    default:
      break;
  }

  menuDrawPage(&autoLevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuABL)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        storeCmd("G28\n");

        switch (infoMachineSettings.leveling)
        {
          case BL_BBL:  // if Bilinear Bed Leveling
            storeCmd("G29\n");
            break;

          case BL_UBL:  // if Unified Bed Leveling
            storeCmd("G29 P1\n");
            // Run this multiple times since it only fills some missing points, not all.
            storeCmd("G29 P3\n");
            storeCmd("G29 P3\n");
            storeCmd("G29 P3\n");
            break;

          default:  // if any other Auto Bed Leveling
            storeCmd("G29\n");
            break;
        }

        storeCmd("M118 A1 ABL Completed\n");
        break;

      case KEY_ICON_1:
        if (infoMachineSettings.leveling == BL_UBL)
          menuUBLSave();
        break;

      case KEY_ICON_2:
        if (infoMachineSettings.leveling == BL_UBL)
          menuUBLLoad();
        break;

      case KEY_ICON_4:
        if (infoSettings.touchmi_sensor != 0)
          infoMenu.menu[++infoMenu.cur] = menuTouchMi;
        else
          infoMenu.menu[++infoMenu.cur] = menuBLTouch;
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuPreheat;
        break;

      case KEY_ICON_7:
        for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
        {
          if (heatGetTargetTemp(i) > 0)
          {
            setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
            break;
          }
        }
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
