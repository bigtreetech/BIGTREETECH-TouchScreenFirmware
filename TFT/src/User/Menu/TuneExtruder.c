#include "TuneExtruder.h"

#include "includes.h"

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

void turnHeaterOff(void)
{
    heatSetTargetTemp(c_heater, 0);
    infoMenu.cur--;
}

void returnToTuning(void)
{
    infoMenu.cur--;
}

// Esteps part
static float measured_length = 20.0;
static float old_esteps = 98.2;
static float new_esteps = 98.2;

void showNewESteps(void)
{
  //First we calculate the new E-step value:
  new_esteps = 100 * old_esteps / measured_length;

  char tempstr[20];

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, "Measured length:");

  sprintf(tempstr, "%0.1fmm", measured_length);
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);

  sprintf(tempstr, "Old esteps: %0.1f", old_esteps);
  GUI_DispString(exhibitRect.x0, BYTE_HEIGHT * 5, (u8 *)tempstr);
  sprintf(tempstr, "New esteps: %0.1f", new_esteps);
  GUI_DispString(exhibitRect.x0,  BYTE_HEIGHT * 6, (u8 *)tempstr);
}


void updateExtruderESteps(void)
{
  heatSetTargetTemp(c_heater, 0);
  infoMenu.menu[++infoMenu.cur] = menuNewExtruderESteps;
}

void extrudeFilament()
{
  storeCmd("G90\nG0 F3000 X0 Y0\n");            // present extruder
  storeCmd("M83\nG1 F50\nG1 E100\nM82\n");      // extrude
  GUI_DispString(exhibitRect.x0, BYTE_HEIGHT * 6, "Extruding 100mm...");
  Delay_ms(1000);
  char tmpBuf[120];
  sprintf(tmpBuf, "Extruding done?\nMeasure the length.\n"
                  "Exactly 20mm? Press '%s'\n"
                  "Otherwise press '%s'", textSelect(LABEL_READY),textSelect(LABEL_TUNING));
  showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), (u8*) tmpBuf,
    textSelect(LABEL_READY), textSelect(LABEL_TUNING), turnHeaterOff, updateExtruderESteps, NULL); //LABEL_CONTINUE should be no.
}

// end Esteps part


void menuTuneExtruder(void)
{
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
            popupReminder(DIALOG_TYPE_ALERT, textSelect(tuneExtruderItems.title.index), (u8*) tmpBuf); //textSelect(LABEL_INVALID_VALUE));
            break;
        }
        if(heatGetCurrentTemp(c_heater) < heatGetTargetTemp(c_heater) - 1)
        {
            popupReminder(DIALOG_TYPE_ALERT, textSelect(tuneExtruderItems.title.index), "Temperature is not yet at the desired value"); //textSelect(LABEL_INVALID_VALUE));
            break;
        } else {
            sprintf(tmpBuf, "Mark 120 mm on your fillament\nPress '%s' when ready", textSelect(LABEL_EXTRUDE));
            showDialog(DIALOG_TYPE_QUESTION, textSelect(tuneExtruderItems.title.index), (u8*) tmpBuf,
              textSelect(LABEL_EXTRUDE), textSelect(LABEL_CANCEL), extrudeFilament, NULL, NULL);
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
    "Adjust E-Steps", //LABEL_HEAT,
    // icon                       label
    { {ICON_DEC,                  LABEL_DEC},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_INC,                  LABEL_INC},
      {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE_INFO},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACK,                 LABEL_BACK},}
  };
  menuDrawPage(&newExtruderESteps);
  showNewESteps();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuNewExtruderESteps)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
          measured_length -= 0.1;
          showNewESteps();
        break;

      case KEY_ICON_3:
          measured_length += 0.1;
          showNewESteps();
        break;

      case KEY_ICON_7:
        //Ask for save when not saved
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

            showNewESteps();
            encoderPosition = 0;
          }
        #endif
        break;
    }
    loopProcess();
  }
}