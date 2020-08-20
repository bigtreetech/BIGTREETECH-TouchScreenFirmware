#include "TuneExtruder.h"

#include "includes.h"

MENUITEMS tuneExtruderItems = {
  // title
  LABEL_TUNE_EXT_TEMP,
  // icon                       label
  { {ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_NOZZLE,               LABEL_NOZZLE},
    {ICON_5_DEGREE,             LABEL_5_DEGREE},
    {ICON_EXTRUDE_100,          LABEL_TUNE_EXT_EXTRUDE_100},
    {ICON_BACK,                 LABEL_BACK},}
};

#define EXTRUDE_DEGREE_NUM 3

const ITEM extruderDegree[EXTRUDE_DEGREE_NUM] = {
// icon                       label
  {ICON_1_DEGREE,             LABEL_1_DEGREE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_10_DEGREE,            LABEL_10_DEGREE},
};

const  u8 extrude_degree[EXTRUDE_DEGREE_NUM] = {1, 5, 10};
static u8 extrude_degree_i = 1;

static uint8_t c_heater = NOZZLE0;

// Show/draw temperature in heat menu
void showExtrudeTemperature(uint8_t index)
{
  char tempstr[20];

  sprintf(tempstr, "%-15s", heatDisplayID[index]);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);

  sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(index), heatGetTargetTemp(index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void turnHeaterOff(void)
{
    heatSetTargetTemp(c_heater, 0);
    infoMenu.cur--;
}

void returnToTuning(void)
{
    infoMenu.cur--;
}

void showNewESteps(const float measured_length, const float old_esteps, float * new_esteps)
{
  char tempstr[20];
  
  //First we calculate the new E-step value:
  *new_esteps = (100 * old_esteps) / (100 - (measured_length - 20));

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, textSelect(LABEL_TUNE_EXT_MEASURED));

  sprintf(tempstr, "%0.1fmm", measured_length);
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);

  sprintf(tempstr, (char*)textSelect(LABEL_TUNE_EXT_OLD_ESTEP), old_esteps);
  GUI_DispString(exhibitRect.x0, BYTE_HEIGHT * 5, (u8 *)tempstr);
  sprintf(tempstr, (char*)textSelect(LABEL_TUNE_EXT_NEW_ESTEP), *new_esteps);
  GUI_DispString(exhibitRect.x0,  BYTE_HEIGHT * 6, (u8 *)tempstr);
}

void extrudeFilament()
{
  storeCmd("G28 X Y R20\n");                   // Home extruder
  mustStoreScript("G90\nG0 F3000 X0 Y0\n");    // present extruder
  mustStoreScript("M83\nG1 F50 E100\nM82\n");  // extrude
  infoMenu.menu[++infoMenu.cur] = menuNewExtruderESteps;
}
// end Esteps part

void menuTuneExtruder(void)
{
  int16_t lastCurrent = heatGetCurrentTemp(c_heater);
  int16_t lastTarget = heatGetTargetTemp(c_heater);


  heatSetUpdateTime(TEMPERATURE_QUERY_FAST_DURATION);

  menuDrawPage(&tuneExtruderItems);
  showExtrudeTemperature(c_heater);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuTuneExtruder)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    int16_t actCurrent = heatGetCurrentTemp(c_heater);
    int16_t actTarget = heatGetTargetTemp(c_heater);
    switch(key_num)
    {
      case KEY_ICON_0:
          heatSetTargetTemp(c_heater, actTarget - extrude_degree[extrude_degree_i]);
        break;

      case KEY_ICON_3:
          heatSetTargetTemp(c_heater, actTarget + extrude_degree[extrude_degree_i]);
        break;

      case KEY_ICON_4:
        do{
          c_heater = (c_heater + 1) % MAX_HOTEND_COUNT;
        } while(!heaterIsValid(c_heater));
        showExtrudeTemperature(c_heater);
        break;

      case KEY_ICON_5:
        extrude_degree_i = (extrude_degree_i+1) % EXTRUDE_DEGREE_NUM;
        tuneExtruderItems.items[key_num] = extruderDegree[extrude_degree_i];
        menuDrawItem(&tuneExtruderItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
      {
        char tmpBuf[120];
        if(heatGetTargetTemp(c_heater) < infoSettings.min_ext_temp)
        {
            sprintf(tmpBuf, (char*)textSelect(LABEL_TUNE_EXT_TEMPLOW), infoSettings.min_ext_temp);
            popupReminder(DIALOG_TYPE_ALERT, textSelect(tuneExtruderItems.title.index), (u8*) tmpBuf);
            break;
        }
        if(heatGetCurrentTemp(c_heater) < heatGetTargetTemp(c_heater) - 1)
        {
            popupReminder(DIALOG_TYPE_ALERT, textSelect(tuneExtruderItems.title.index), textSelect(LABEL_TUNE_EXT_DESIREDVAL));
            break;
        } else {
            sprintf(tmpBuf, (char*)textSelect(LABEL_TUNE_EXT_MARK120MM), textSelect(LABEL_EXTRUDE));
            showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), (u8*) tmpBuf,
              textSelect(LABEL_EXTRUDE), textSelect(LABEL_CANCEL), extrudeFilament, NULL, NULL);
        }
      }break;

      case KEY_ICON_7:
        if(heatGetTargetTemp(c_heater) > 0)
        {
            showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), textSelect(LABEL_TUNE_EXT_HEATOFF),
              textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), turnHeaterOff, returnToTuning, NULL);
        } else {
            infoMenu.cur--;   
        }
        
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if(encoderPosition > 0)
                heatSetTargetTemp(c_heater, actTarget + extrude_degree[extrude_degree_i]);
            if(encoderPosition < 0)
                heatSetTargetTemp(c_heater, actTarget - extrude_degree[extrude_degree_i]);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget) {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      showExtrudeTemperature(c_heater);
    }

    loopProcess();
  }

  // Set slow update time if not waiting for target temperature
  if(heatHasWaiting() == false)
    heatSetUpdateTime(TEMPERATURE_QUERY_SLOW_DURATION);
}

void menuNewExtruderESteps(void)
{
  // Extruder steps are not correct. Ask user for the amount that's extruded
  // Automaticaly calculate new steps/mm when changing the measured distance
  // When pressing save to eeprom the new steps will be saved.
   MENUITEMS newExtruderESteps = {
    // title
    LABEL_TUNE_EXT_ADJ_ESTEPS,
    // icon                       label
    { {ICON_DEC,                  LABEL_DEC},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_INC,                  LABEL_INC},
      {ICON_S_SAVE,               LABEL_SAVE},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACK,                 LABEL_BACK},}
  };

  float measured_length = 20.0f;
  float old_esteps; // get the value of the E-steps
  float new_esteps;
  u32 hidetext_ms = 0;

  mustStoreCmd("M503 S0\n");
  old_esteps = getParameter(P_STEPS_PER_MM, E_AXIS); // get the value of the E-steps

  menuDrawPage(&newExtruderESteps);
  showNewESteps(measured_length, old_esteps, &new_esteps);
  
  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuNewExtruderESteps)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
          measured_length -= 0.1f;
          showNewESteps(measured_length, old_esteps, &new_esteps);
        break;

      case KEY_ICON_3:
          measured_length += 0.1f;
          showNewESteps(measured_length, old_esteps, &new_esteps);
        break;

      case KEY_ICON_4:
      {
        char tmpBuf[120];
        storeCmd("M92 T0 E%0.2f\n", new_esteps);

        sprintf(tmpBuf, (char*)textSelect(LABEL_TUNE_EXT_ESTEPS_SAVED), new_esteps);
        GUI_DispStringInRectEOL(exhibitRect.x0,  BYTE_HEIGHT * 8, exhibitRect.x1 + 20, LCD_HEIGHT, (u8*) tmpBuf);
        BUZZER_PLAY(sound_success);
        hidetext_ms = OS_GetTimeMs() + TUNE_EXT_HIDE_SAVEDMSG;
      }break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if(encoderPosition > 0)
              measured_length += 0.1;

            if(encoderPosition < 0)
              measured_length -= 0.1;

            showNewESteps(measured_length, old_esteps, &new_esteps);
            encoderPosition = 0;
          }
        #endif
        break;
    }
    if(hidetext_ms > 0 && OS_GetTimeMs() > hidetext_ms)
    {
      GUI_ClearRect(exhibitRect.x0, BYTE_HEIGHT * 8, exhibitRect.x1 + 20, BYTE_HEIGHT * (8 + 5));
      hidetext_ms = 0;
    }
    loopProcess();
  }
}
