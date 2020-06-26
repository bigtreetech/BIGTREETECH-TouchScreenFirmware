#include "FilamentChange.h"
#include "includes.h"

MENUITEMS filamentChangeItems = {
    //   title
    LABEL_EXTRUDE,
    // icon                       label
    {
        {ICON_LOAD, LABEL_LOAD},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_UNLOAD, LABEL_UNLOAD},
        {ICON_NOZZLE, LABEL_NOZZLE},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_BACK, LABEL_BACK},
    }};

// The nozzle number
static u8 item_extruder_i = 0;
// len of extrusion
 static float extrudeCoordinate = 0.0f;

// wait for update the screen
static u32 nextTime = 0;
static u32 update_time = 2000; // 1 seconds is 1000

const char *filamentChange_tool_change[] = TOOL_CHANGE;
const char *filamentChangeDisplayID[] = EXTRUDER_ID;

void showFilamentChangeCoordinate(void)
{
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y - BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  char buf[36];
  my_sprintf(buf, "%s:%d/%d", filamentChangeDisplayID[item_extruder_i], heatGetCurrentTemp(heatGetCurrentToolNozzle()), heatGetTargetTemp(heatGetCurrentToolNozzle()));
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8 *)buf);
}
void updateTemperature(void)
{
  if (OS_GetTimeMs() > nextTime)
  {
    nextTime = OS_GetTimeMs() + update_time;
    showFilamentChangeCoordinate();
  }
}
void menuFilamentChange(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  float eSaved = 0.0f;
  bool  eRelative = false;
  u32   feedrate = 0;

  while (infoCmd.count != 0)
  {
    loopProcess();
  }
  if(isPause()){
  extrudeCoordinate = eSaved = coordinateGetAxisTarget(E_AXIS);
  feedrate = coordinateGetFeedRate();
  eRelative = eGetRelative();
  }
  menuDrawPage(&filamentChangeItems);
  showFilamentChangeCoordinate();

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif
  if(eRelative) mustStoreCmd("M82\n"); // Set extruder to absolute
  while (infoMenu.menu[infoMenu.cur] == menuFilamentChange)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_0:
      if (heatGetCurrentTemp(heatGetCurrentToolNozzle()) < infoSettings.filament_load_limit_temperature - 5)
      {
        popupReminder(textSelect(LABEL_INFO), textSelect(LABEL_FILAMENT_LOAD_TEMPERATURE));
        if (heatGetTargetTemp(heatGetCurrentToolNozzle()) < infoSettings.filament_load_limit_temperature)
        {
          storeCmd("M104 %s S%d\n", filamentChange_tool_change[item_extruder_i], infoSettings.filament_load_limit_temperature);
        }
      }
      else
      {
        u16 key_num = IDLE_TOUCH;
        bool waitConfirmation = true;
        popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_FILAMENT_CHANGE_READY), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));
        while (waitConfirmation)
        {
          key_num = KEY_GetValue(2, doubleBtnRect);
          switch (key_num)
          {
          case KEY_POPUP_CONFIRM:
            if (item_extruder_i != heatGetCurrentToolNozzle() - NOZZLE0){
              storeCmd("%s\n", filamentChange_tool_change[item_extruder_i]);
            }
            if(isPause()){
              extrudeCoordinate = infoSettings.filament_load_length;
              storeCmd("G0 E%.5f F%d\n", extrudeCoordinate, infoSettings.filament_load_speed);
            }else{
              storeCmd("G91\nG0 E%.5f F%d\nG90\n", infoSettings.filament_load_length, infoSettings.filament_load_speed);
            }
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuFilamentChange;
            menuDrawPage(&filamentChangeItems);
            break;

          case KEY_POPUP_CANCEL:
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuFilamentChange;
            menuDrawPage(&filamentChangeItems);
            break;
          }
          loopProcess();
        }
      }
      break;

    case KEY_ICON_3:
      if (heatGetCurrentTemp(heatGetCurrentToolNozzle()) < infoSettings.filament_load_limit_temperature - 5)
      {
        popupReminder(textSelect(LABEL_INFO), textSelect(LABEL_FILAMENT_LOAD_TEMPERATURE));
        if (heatGetTargetTemp(heatGetCurrentToolNozzle()) < infoSettings.filament_load_limit_temperature)
        {
          storeCmd("M104 %s S%d\n", filamentChange_tool_change[item_extruder_i], infoSettings.filament_load_limit_temperature);
        }
      }
      else
      {
        u16 key_num = IDLE_TOUCH;
        bool waitConfirmation = true;
        popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_FILAMENT_CHANGE_READY), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));
        while (waitConfirmation)
        {
          key_num = KEY_GetValue(2, doubleBtnRect);
          switch (key_num)
          {
          case KEY_POPUP_CONFIRM:
            if (item_extruder_i != heatGetCurrentToolNozzle() - NOZZLE0){
              storeCmd("%s\n", filamentChange_tool_change[item_extruder_i]);
            }
            if(isPause()){
              extrudeCoordinate =  -1 * infoSettings.filament_unload_length;
              storeCmd("G0 E%.5f F%d\n",  extrudeCoordinate, infoSettings.filament_unload_speed);
            }
            else{
              storeCmd("G91\nG0 E%.5f F%d\n",infoSettings.filament_unload_retract_length, infoSettings.filament_unload_retract_speed);
              storeCmd("G0 E%.5f F%d\nG90\n",   -1 * infoSettings.filament_unload_length, infoSettings.filament_unload_speed);
            }
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuFilamentChange;
            menuDrawPage(&filamentChangeItems);
            break;

          case KEY_POPUP_CANCEL:
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuFilamentChange;
            menuDrawPage(&filamentChangeItems);
            break;
          }
          loopProcess();
        }
      }
      break;

    case KEY_ICON_4:
      item_extruder_i = (item_extruder_i + 1) % infoSettings.ext_count;
      updateTemperature();
      break;


    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default:
#if LCD_ENCODER_SUPPORT
      if (encoderPosition)
      {
       if (heatGetCurrentTemp(heatGetCurrentToolNozzle()) < infoSettings.filament_load_limit_temperature - 5)
      {
        popupReminder(textSelect(LABEL_INFO), textSelect(LABEL_PID_AUTOTUNE_WARNING));
        if (heatGetTargetTemp(heatGetCurrentToolNozzle()) < infoSettings.filament_load_limit_temperature)
        {
          storeCmd("M104 %s S%d\n", filamentChange_tool_change[item_extruder_i], infoSettings.filament_load_limit_temperature);
        }
      }
      else
      {
        u16 key_num = IDLE_TOUCH;
        bool waitConfirmation = true;
        popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_PID_AUTOTUNE_WARNING), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));
        while (waitConfirmation)
        {
          key_num = KEY_GetValue(2, doubleBtnRect);
          switch (key_num)
          {
          case KEY_POPUP_CONFIRM:
            if (item_extruder_i != heatGetCurrentToolNozzle() - NOZZLE0)
              storeCmd("%s\n", filamentChange_tool_change[item_extruder_i]);
              if(isPause()){
                extrudeCoordinate =  infoSettings.filament_load_length;
                storeCmd("G0 E%.5f F%d\n", extrudeCoordinate, infoSettings.filament_load_speed);
              }else{
                storeCmd("G91\nG0 E%.5f F%d\nG90\n", infoSettings.filament_load_length, infoSettings.filament_load_speed);
              }
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuFilamentChange;
            menuDrawPage(&filamentChangeItems);
            break;

          case KEY_POPUP_CANCEL:
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuFilamentChange;
            menuDrawPage(&filamentChangeItems);
            break;
          }
          loopProcess();
        }
      }
      break;
        encoderPosition = 0;
      }
#endif
      break;
    }
    updateTemperature();
    loopProcess();
  }
  if(isPause()){
  mustStoreCmd("G92 E%.5f\n", eSaved);
  mustStoreCmd("G0 F%d\n", feedrate);
  if(eRelative) mustStoreCmd("M83\n"); // Set extruder to relative
  }
}
