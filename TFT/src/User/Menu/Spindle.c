//TG MODIFIED BY T.GIOIOSA
#include "Spindle.h"
#include "includes.h"
#include "Numpad.h"
#include "Settings.h"

/*//TG 2/4/21
  Handles the Spindle Menu..
  This module is new, created to handle a tool (Spindle) written by T. Gioiosa
  Adapted from Fan.c and Temperature.c
  There should logically be only be 1 spindle on a machine (no special Ctl spindles)
  Although future mods could allow multiple tools (bits)
*/

uint8_t speed_mismatch;
static uint8_t spIndex = 0;          // should always be zero
static uint8_t stepsIndex = 0;       //TG 1/16/20 new where the list starts as default 1=CW
//uint8_t spindleState=0;              // spindle off = 0       

//TG 8/31/21 these arrays usually get filled by spindleBuildList() called from spindleControlInit() to handle multiple instances
//TG 8/31/21 but since there can only be one spindle, we just define a single instance here and no need to call spindleBuildList()
char* spindleID[MAX_SPINDLE_COUNT]      = SPINDLE_SIGN_ID;   //TG 2/5/21 added these
char* spindleDisplayID[MAX_SPINDLE_COUNT] = SPINDLE_DISPLAY_ID;
char* spindleCmd[MAX_SPINDLE_COUNT]       = SPINDLE_CMD;
//const char *const spindleWaitCmd[MAX_SPINDLE_COUNT]   = SPINDLE_WAIT_CMD;

MTYPE marlin_units;    // copy the output power units to simplify access
MTYPE disp_units;      // copy the displayed power units to simplify access

MENUITEMS spindleItems = {
  // title
  LABEL_SPINDLE ,
  // icon                         label
  {{ICON_DEC,                     LABEL_DEC},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_INC,                     LABEL_INC},
   {ICON_SPINDLE,                 LABEL_SPINDLE},     
   {ICON_500_RPM,                 LABEL_500_RPM},   //TODO change icon
   {ICON_SSTOP,                   LABEL_SSTOP},
   {ICON_BACK,                    LABEL_BACK},}
};

const ITEM itemSpindleONOFF[2] = {
// icon                       label
  {ICON_SSTART,              LABEL_SSTART},
  {ICON_SSTOP,               LABEL_SSTOP},
};

void toolSetCurrentIndex(uint8_t index)
{
  spIndex = index;
}

// Show/draw spindle speed/status in upper center status area
void updateSpeedStatusDisplay(u8 index, bool speed_only)
{
  uint8_t* symPtr;
  
  char tempstr[18];
  if(speed_only == false) {
    sprintf(tempstr, "%-15s", spindleDisplayID[index]);                               // display the tool id
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);                    // above the speeds
    setLargeFont(true);
    symPtr = (infoSettings.cutter_disp_unit == MPWM) ? (uint8_t *)"PWM" 
             : (infoSettings.cutter_disp_unit == MPCT) ? (uint8_t *)"%" 
             : (uint8_t *)"RPM";
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1)>>1, exhibitRect.y0, symPtr);
  }
  
  setLargeFont(true);
  GUI_SetColor(speed_mismatch==1 ? RED : infoSettings.font_color);  //TG 8/31/21 if speed mismatch, highlight target/actual in RED
  sprintf(tempstr, "%5d/%-5d", convertSpeed_Marlin_2_LCD(index,spindleGetCurSpeed(index)), 
          convertSpeed_Marlin_2_LCD(index, spindleGetSetSpeed(index))); // show temp target/actual in large font
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  GUI_SetColor(infoSettings.font_color);  //TG 8/31/21 restore normal font color for code that follows                                       
  setLargeFont(false);

  if(speed_only == false) {
    drawSpindleStatusInIcon();    // show CW, CCW, or nothing in spindle icon
    // need a wider rectangle than standard to clean up previous messages since one starts further left and on ends further right
    GUI_RECT widerRect = {exhibitRectLower.x0 - BYTE_WIDTH,exhibitRectLower.y0,exhibitRectLower.x1 + BYTE_WIDTH, exhibitRectLower.y1};
    GUI_ClearPrect(&widerRect);   // clear any previous extra information rectangle with wider rectangle
    GUI_SetColor(spindleState==1 ? RED : DARKGREEN);
    GUI_SetBkColor(infoSettings.title_bg_color);
    sprintf(tempstr, "Spindle %s %-4s", spindleState==0 ? "stopped" : "running",
                     (spindleState==1 ? (infoSettings.spin_dir==0 ? "CCW" : "CW") 
                     : " "));  // display spindle spin direction
    if(spindleState==1)
      GUI_DispString(exhibitRectLower.x0 - BYTE_WIDTH/2, exhibitRectLower.y0, (u8 *)tempstr);         // below the Temp
    else
      GUI_DispLenString(exhibitRectLower.x0 + BYTE_WIDTH, exhibitRectLower.y0, (u8 *)tempstr, BYTE_WIDTH*18,true);         // below the Temp
  }
  GUI_RestoreColorDefault();
}


uint8_t toPercent (uint8_t speed){
  return (speed * 100.0f) / infoSettings.spindle_pwm_max[spIndex] + 0.5f;
}


void menuSpindle(void)   //TODO should rename this and this file from tool to spindle
{
  int16_t lastCurrent = spindleGetCurSpeed(spIndex);
  int16_t lastTarget = spindleGetSetSpeed(spIndex);
    
  marlin_units = infoSettings.cutter_power_unit;    // copy the output power units to simplify access below
  disp_units = infoSettings.cutter_disp_unit;       // copy the displayed power units to simplify access below
  
  //spindleSpeedSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);  //TG 8/31/21 - **This is not needed**

  spindleItems.items[6].icon = spindleState==0 ? ICON_SSTART:ICON_SSTOP;                         //TG set icon per state of spindle on entry
  spindleItems.items[6].label.index = (int32_t)(spindleState==0 ? LABEL_SSTART:LABEL_SSTOP);     //TG set label per state of spindle on entry
  spindleItems.items[4] = itemSpindle[spIndex];    // choose icon for current tool, since there is only 1 spindle all icons are the same
                                                   // in future we can eliminate arrays since only 1 spindle
  
  //TG setup the units steps icon for the current lcd units mode
  //stepsIndex = 0;
  spindleItems.items[5] = ((MTYPE)infoSettings.cutter_disp_unit == MRPM) ? itemRPMSteps[stepsIndex] :
                              ((MTYPE)infoSettings.cutter_disp_unit == MPCT) ? itemPercent[stepsIndex] : itemPWMSteps[stepsIndex]; 
   
  // reset previous set speed to zero only if spindle if OFF, else leave it where it was last time
  //spindleSetSpeed(spIndex, spindleState ? spindleGetSetSpeed (spIndex) : 0);
  menuDrawPage(&spindleItems);
  updateSpeedStatusDisplay(spIndex, false);    //TG displays the tool id, temp, and status of spindle rotation if spIndex=spindle

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuSpindle)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    actCurrent = spindleGetCurSpeed (spIndex);
    actTarget =  spindleGetSetSpeed (spIndex);
    
    switch(key_num)
    {
      case KEY_ICON_0:
      /* 
         subtract current step increment from target speed, will send M3/M4 gcode command when loopBackEnd() calls loopSpindle()
         The setSpindleSpeed variable is always handled as the marlin mode(infoSettings.cutter_power_unit) type.
         The displayed speed is always handled as a lcd mode(infoSettings.cutter_disp_unit) type.
         Conversions are made between marlin units and lcd units as needed (RPM, PCT, or PWM).
      */
        {
          uint16_t newpower;
          uint16_t newstep;
          // subtract converted step value from current speed
          newstep = convertSpeed_LCD_2_Marlin(spIndex, getLCD_Units_StepValue(spIndex, stepsIndex));
          newpower = (spindleGetSetSpeed(spIndex)-newstep > 0) ? spindleGetSetSpeed(spIndex)-newstep : 0; 

          //newpower = spindleGetSetSpeed(spIndex) - convertSpeed_LCD_2_Marlin(spIndex, getLCD_Units_StepValue(spIndex, stepsIndex));
          // update speed with limit-checked sum from above, always in Marlin units
          spindleSetSpeed(spIndex, checkPowerLimits(spIndex, newpower)); 
          updateSpeedStatusDisplay(spIndex, false);
          break;
        }

      case KEY_INFOBOX:   //TG cool, didn't know this existed, touch the infobox area and you can type direct values!
      {
        char titlestr[30];
        switch(infoSettings.cutter_disp_unit)   // choose appropriate title based on LCD display units
          {
            case MRPM:
              sprintf(titlestr, "Min:0 | Max:%d", infoSettings.spindle_rpm_max[spIndex]);
              break;
            case MPCT:
              strcpy(titlestr, "Min:0 | Max:100");
              break;
            case MPWM:
              sprintf(titlestr, "Min:0 | Max:%d",MAX_SPINDLE_PWM_SPEED);  //TG - 10/2/21 modified
              break;
            default:
              break;
          }
        // convert setSpeed to LCD units for working with numPad
        uint16_t SetSpeed_in_LCD_Units = convertSpeed_Marlin_2_LCD(spIndex, spindleGetSetSpeed(spIndex));
        uint16_t newval = numPadInt((u8 *) titlestr, (int32_t)SetSpeed_in_LCD_Units, 0, false);
        // convert value back to Marlin units before setting new target speed
        newval = convertSpeed_LCD_2_Marlin(spIndex,newval);
        newval = checkPowerLimits(spIndex, newval);   // checkPowerLimits bases test using Marlin units
        if(newval != spindleGetSetSpeed(spIndex))
          spindleSetSpeed(spIndex, newval);        
        
        menuDrawPage(&spindleItems);
        updateSpeedStatusDisplay(spIndex, false);
        break;
      }

      case KEY_ICON_3:
      /* 
         add current step increment to target speed, will send M3/M4 gcode command when loopBackEnd() calls loopSpindle()
         The setSpindleSpeed variable is always handled as the marlin mode(infoSettings.cutter_power_unit) type.
         The displayed speed is always handled as a lcd mode(infoSettings.cutter_disp_unit) type.
         Conversions are made between marlin units and lcd units as needed (RPM, PCT, or PWM).
      */
        {
          uint16_t newpower;
          // add current speed and step value converted from LCD units to Marlin units
          newpower = spindleGetSetSpeed(spIndex) + convertSpeed_LCD_2_Marlin(spIndex, getLCD_Units_StepValue(spIndex, stepsIndex));
          // update speed with limit-checked sum from above, always in Marlin units
          spindleSetSpeed(spIndex, checkPowerLimits(spIndex, newpower)); 
          updateSpeedStatusDisplay(spIndex, false);
          break;
        }

      case KEY_ICON_4:    //TODO  MIGHT NEED TO WORK ON THIS -  all references need to change to laser
        if ((infoSettings.spindle_count + infoSettings.spindle_ctrl_count) > 1)
          spIndex = (spIndex + 1) % (infoSettings.spindle_count + infoSettings.spindle_ctrl_count);

        //spindleSetSpeed(spIndex, infoSettings.spindle_pwm_max[spIndex] / 2);  // 50%
        spindleItems.items[key_num] = itemSpindle[spIndex];
        menuDrawItem(&spindleItems.items[key_num], key_num);
        //updateSpeedStatusDisplay(spIndex);
        break;

     case KEY_ICON_5:
        // cycle thru the step multipliers for RPM, PCT, or PWM
        stepsIndex = (stepsIndex + 1) % (((MTYPE)infoSettings.cutter_disp_unit == MRPM) ? ITEM_RPM_NUM :
                     ((MTYPE)infoSettings.cutter_disp_unit == MPCT) ? ITEM_PERCENT_STEPS_NUM : ITEM_PWM_NUM);
                                                                
        spindleItems.items[key_num] = ((MTYPE)infoSettings.cutter_disp_unit == MRPM) ? itemRPMSteps[stepsIndex] :
                                      ((MTYPE)infoSettings.cutter_disp_unit == MPCT) ? itemPercent[stepsIndex] : itemPWMSteps[stepsIndex]; 
        
        menuDrawItem(&spindleItems.items[key_num], key_num);         // redraw the screen with change
        break;

      case KEY_ICON_6:
        spindleState ^= 1;    // toggle value when pressed
        spindleItems.items[key_num] = itemSpindleONOFF[spindleState];  // update icon/label in menu
        menuDrawItem(&spindleItems.items[key_num], key_num);           // redraw the menu with updates
        
        // set speed in marlin units from actTarget var, will update when loopBackEnd() calls loopSpindle()
        spindleState==0 ?spindleSetSpeed(spIndex, 0):spindleSetSpeed(spIndex, actTarget);
        updateSpeedStatusDisplay(spIndex, false);
        break;                                                                            

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (encoderPosition > 0)
              spindleSetSpeed(spIndex, actTarget + RPMSteps[stepsIndex]);
            else // if < 0)
              spindleSetSpeed(spIndex, actTarget - RPMSteps[stepsIndex]);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if ( lastCurrent != actCurrent || lastTarget != actTarget )
    { //TG 8/31/21 added next line to flag when actual speed and target speed are off by >1500    
      if (abs(actCurrent - actTarget) > 1500) {speed_mismatch=1;} else {speed_mismatch=0;}
      updateSpeedStatusDisplay(spIndex, true);
      lastCurrent = actCurrent;
      lastTarget = actTarget;
    }
    
    loopProcess();
  }

}

void drawSpindleStatusInIcon(void)    //TG new adds text to an icon at ss_spin_point
{
  //icons and their values are updated one by one to reduce flicker/clipping
  //char tempstr[45];
  GUI_POINT ss_spin_point = {SSICON_WIDTH - 3*BYTE_WIDTH-BYTE_WIDTH/4, SSICON_NAME_Y0};
  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.lines[0].h_align = LEFT;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = SSICON_NAME_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = ss_spin_point;
  lvIcon.lines[0].large_font = NAME_LARGE_FONT;
  lvIcon.enabled[1] = false;
  lvIcon.enabled[2] = false;
              
  //TOOL / EXT
  lvIcon.lines[0].text = (uint8_t *)(infoSettings.spin_dir==0 ? "CCW" : " CW");
  if(spindleState==0) {lvIcon.lines[0].text = (uint8_t *)"OFF";}
  showLiveInfo(4, &lvIcon, &spindleItems.items[4]);
 
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_SetBkColor(infoSettings.status_xyz_bg_color);
  GUI_RestoreColorDefault();
}

