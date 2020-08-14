#include "TuneExtruder.h"

#include "includes.h"

#define TUNE_EXTRUDER_NUM 6

const ITEM tuneExtruderTool[] = {
// icon                       label
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
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

// Esteps part
static u8 measured_length = 20;
static u8 old_steps_mm = 0;

void showNewESteps(void)
{
  char tempstr[20];

  sprintf(tempstr, "%-15s", measured_length);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);

  sprintf(tempstr, "%dmm", measured_length);
  setLargeFont(true);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  setLargeFont(false);
}

void updateExtruderESteps(void)
{
  infoMenu.menu[++infoMenu.cur] = menuNewExtruderESteps;
}

// end Esteps part


void turnHeaterOff(void)
{
    heatSetTargetTemp(c_heater, 0);
    infoMenu.cur--;
}

void returnToTuning(void)
{
    infoMenu.cur--;
}



void menuTuneExtruder(void)
{
  MENUITEMS tuneExtruderItems = {
    // title
    LABEL_HEAT,
    // icon                       label
    { {ICON_DEC,                  LABEL_DEC},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_INC,                  LABEL_INC},
      {ICON_NOZZLE,               LABEL_NOZZLE},
      {ICON_5_DEGREE,             LABEL_5_DEGREE},
      {ICON_EXTRUDE_100,          LABEL_EXTRUDE_100},
      {ICON_BACK,                 LABEL_BACK},}
  };

  int16_t lastCurrent = heatGetCurrentTemp(c_heater);
  int16_t lastTarget = heatGetTargetTemp(c_heater);


  heatSetUpdateTime(TEMPERATURE_QUERY_FAST_DURATION);

  tuneExtruderItems.items[KEY_ICON_4] = tuneExtruderTool[c_heater];
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
        tuneExtruderItems.items[key_num] = tuneExtruderTool[c_heater];
        menuDrawItem(&tuneExtruderItems.items[key_num], key_num);
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
            //sprintf(tmpBuf, "%s\n %s", textSelect(LABEL_TIMEOUT_REACHED), textSelect(LABEL_PROCESS_ABORTED));
            sprintf(tmpBuf, "Desired temperature too low!\nMinimum temperature: %d C", infoSettings.min_ext_temp);
            popupNotification(DIALOG_TYPE_ALERT, textSelect(tuneExtruderItems.title.index), (u8*) tmpBuf); //textSelect(LABEL_INVALID_VALUE));
            break;
        }
        if(heatGetCurrentTemp(c_heater) < heatGetTargetTemp(c_heater) - 1)
        {
            popupNotification(DIALOG_TYPE_ALERT, textSelect(tuneExtruderItems.title.index), "Temperature is not yet at the desired value"); //textSelect(LABEL_INVALID_VALUE));
            break;
        } else {
            sprintf(tmpBuf, "Mark 120 mm on your fillament\nPress '%s' when ready", LABEL_CONFIRM);
            showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), tmpBuf,
              textSelect(LABEL_CONFIRM), "", NULL, NULL, NULL);
            sprintf(tmpBuf, "When extruding is done, press '%s'", LABEL_CONFIRM);
            showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), tmpBuf,
              textSelect(LABEL_CONFIRM), "", NULL, NULL, NULL);
            //Turn of the heater. It's not needed anymore
            heatSetTargetTemp(c_heater, 0);
            showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), "Measure the length to the marked point.\n Is it 20mm?",
              textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), NULL, updateExtruderESteps, NULL);
        }
      }break;

      case KEY_ICON_7:
        if(heatGetTargetTemp(c_heater) > 0)
        {
            showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), "Turn heater off?",
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
    "What is the distance left?", //LABEL_HEAT,
    // icon                       label
    { {ICON_DEC,                  LABEL_DEC},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_INC,                  LABEL_INC},
      {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE_INFO},
      {ICON_BACKGROUND,           LABEL_5_DEGREE},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACK,                 LABEL_BACK},}
  };
  menuDrawPage(&newExtruderESteps);
  showNewESteps();
}