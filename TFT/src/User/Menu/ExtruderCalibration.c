#include "ExtruderCalibration.h"
#include "Parametersetting.h"
#include "includes.h"

MENUITEMS extrudeCalibrationItems = {
    //   title
    LABEL_EXTRUDE,
    // icon                       label
    {
        {ICON_LOAD, LABEL_EXTRUDE},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_EDIT, LABEL_EDIT},
        {ICON_SCREEN_INFO, LABEL_SCREEN_INFO},
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

const char *extrudeCalibration_tool_change[] = TOOL_CHANGE;
static float v = 0.0f;
void showextrudeCalibrationCoordinate(void)
{
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y - BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  char buf[36];
  my_sprintf(buf, "E Steps: %.2f ", infoParameters.StepsPerMM[3]);
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8 *)buf);
}
void updateStepsPerMM(void)
{
  if (OS_GetTimeMs() > nextTime)
  {
    nextTime = OS_GetTimeMs() + update_time;
    showextrudeCalibrationCoordinate();
  }
}
void menuExtrudeCalibration(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  float eSaved = 0.0f;
  float eTemp = 0.0f;
  while (infoCmd.count != 0)
  {
    loopProcess();
  }
  extrudeCoordinate = eTemp = eSaved = coordinateGetAxisTarget(E_AXIS);
  menuDrawPage(&extrudeCalibrationItems);
  showextrudeCalibrationCoordinate();

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif
  while (infoMenu.menu[infoMenu.cur] == menuExtrudeCalibration)
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
          storeCmd("M104 %s S%d\n", extrudeCalibration_tool_change[item_extruder_i], infoSettings.filament_load_limit_temperature);
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
            if (item_extruder_i != heatGetCurrentToolNozzle() - NOZZLE0)
              storeCmd("%s\n", extrudeCalibration_tool_change[item_extruder_i]);
            storeCmd("G91\nG0 E%.5f F%d\nG90\n", infoSettings.distance_filament_extrude_calibration, infoSettings.ext_speed[0]);
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuExtrudeCalibration;
            menuDrawPage(&extrudeCalibrationItems);
            break;

          case KEY_POPUP_CANCEL:
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuExtrudeCalibration;
            menuDrawPage(&extrudeCalibrationItems);
            break;
          }
          loopProcess();
        }
      }
      break;

    case KEY_ICON_3:
      v = numPadFloat(infoParameters.StepsPerMM[3], false);
      if (v != infoParameters.StepsPerMM[3]){
        if(v > 0){
        storeCmd("M92 T0 E%.2f\n",v);
        }
      }
      menuDrawPage(&extrudeCalibrationItems);
      break;

    case KEY_ICON_4:
          popupReminder(textSelect(LABEL_INFO), textSelect(LABEL_EXTRUDE_CALIBRATION_INFORMATION));
      break;


    case KEY_ICON_7:
      storeCmd("M500\n");
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
          storeCmd("M104 %s S%d\n", extrudeCalibration_tool_change[item_extruder_i], infoSettings.filament_load_limit_temperature);
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
              storeCmd("%s\n", extrudeCalibration_tool_change[item_extruder_i]);
            storeCmd("G91\nG0 E%.5f F%d\nG90\n", infoSettings.filament_load_speed, infoSettings.filament_load_speed);
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuExtrudeCalibration;
            menuDrawPage(&extrudeCalibrationItems);
            break;

          case KEY_POPUP_CANCEL:
            waitConfirmation = false;
            infoMenu.menu[infoMenu.cur]=menuExtrudeCalibration;
            menuDrawPage(&extrudeCalibrationItems);
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
    updateStepsPerMM();
    loopProcess();
  }

}
