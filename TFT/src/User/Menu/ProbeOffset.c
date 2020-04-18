#include "ProbeOffset.h"
#include "includes.h"

//1 titl, ITEM_PER_PAGE item
MENUITEMS probeOffsetItems = {
// title
LABEL_Z_OFFSET,
// icon                        label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_NORMAL_SPEED,         LABEL_VALUE_ZERO},
  {ICON_BACK,                 LABEL_BACK},}
};

typedef struct
{
  const ITEM *list;
  const float *ele;
  uint8_t cur;
  uint8_t totaled;
}ELEMENTS;

#define Z_GCODE "G1 Z%.1f\n"


#define ITEM_PROBE_OFFSET_UNIT_NUM 3
const ITEM itemProbeOffsetUnit[ITEM_PROBE_OFFSET_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_probeOffset_unit[ITEM_PROBE_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;

static void initElements(u8 position)
{
  elementsUnit.totaled = ITEM_PROBE_OFFSET_UNIT_NUM;
  elementsUnit.list = itemProbeOffsetUnit;
  elementsUnit.ele  = item_probeOffset_unit;

  for(u8 i=0; i<elementsUnit.totaled; i++)
  {
    if(memcmp(&elementsUnit.list[i], &probeOffsetItems.items[position], sizeof(ITEM)) == 0)
    {
      elementsUnit.cur = i;
      break;
    }
  }
}

AXIS zAxis = Z_AXIS;

void storeMoveCmdZ(AXIS xyz, int8_t direction) {
  const char *xyzMoveCmd[] = {Z_GCODE};
  // if invert is true, 'direction' multiplied by -1
  storeCmd(xyzMoveCmd[xyz], (infoSettings.invert_axis[xyz] ? -direction : direction) * elementsUnit.ele[elementsUnit.cur]);
  // update now axis be selected
  zAxis = xyz;
}


#define PROBE_OFFSET_MAX_VALUE 20.0f
#define PROBE_OFFSET_MIN_VALUE -20.0f

void showProbeOffset(float val, float probval)
{
  GUI_SetColor(REMINDER_FONT_COLOR);
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y - 20, probval ,3, 2, RIGHT);  // show actual Z offset
  GUI_SetColor(FONT_COLOR);
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y + 20, val, 3, 2, RIGHT);
}

void menuProbeOffset(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  //float probe_offset_value;
  float probe_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);
  float now = 0.0f;
  float new_offset = 0.0f;
  const char* softendstop = textSelect(LABEL_SOFT_ENDSTOP_OFF);
  u16 SW_X = (LCD_WIDTH - GUI_StrPixelWidth((u8 *)softendstop))/2;

  initElements(KEY_ICON_5);
  menuDrawPage(&probeOffsetItems);
  GUI_SetColor(REMINDER_FONT_COLOR);
  GUI_DispString(SW_X, CENTER_Y - 50, (u8 *)softendstop);  // show endstop warning

  // Z offset g code sequence start
    storeCmd("G28\n");                          // home printer
    storeCmd(OFFSET_GCODE);                     // move nozzle to Z offset test point set in Configuration.h
    storeCmd("M211 S0\n");                      // disable Software Endstop to move nozzle minus Zero (Z0) if necessary
    storeCmd("M851\n");                         // get actual Z offset in Buffer
    mustStoreCmd("G91\n");                      // set relative position mode 
    mustStoreCmd("G1 F%d\n",SPEED_MOVE_SLOW);   // set Z axis movement rate to slow
  // Z offset g code sequence end

  showProbeOffset(now, probe_offset_value);
  //popupReminder(textSelect(LABEL_WARNING), textSelect(LABEL_SOFTENDSTOP_OFF));  // show warning 
  
  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuProbeOffset)
  {
    //probe_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if(now - elementsUnit.ele[elementsUnit.cur] > PROBE_OFFSET_MIN_VALUE)
        {
          //if(storeCmd("M851 Z%.2f\n",probe_offset_value-elementsUnit.ele[elementsUnit.cur]))
            now -= elementsUnit.ele[elementsUnit.cur];
            storeMoveCmdZ(Z_AXIS, -1);
        }
        break;
      case KEY_ICON_3:
        if(now + elementsUnit.ele[elementsUnit.cur] < PROBE_OFFSET_MAX_VALUE)
        {
          //if(storeCmd("M851 Z%.2f\n",probe_offset_value+elementsUnit.ele[elementsUnit.cur]))
            now += elementsUnit.ele[elementsUnit.cur];
            storeMoveCmdZ(Z_AXIS, 1);
        }
        break;
      case KEY_ICON_4:
        probe_offset_value = now;
        showProbeOffset(now, probe_offset_value);
        storeCmd("M851 Z%.2f\n",probe_offset_value);    // set new Z offset
        mustStoreCmd("G90\n");                          // set absolute position mode
        storeCmd("M211 S1\n");                          // enable Software Endstop
		storeCmd("G28\n");                          	// home printer
        if(infoMachineSettings.EEPROM == 1){
           storeCmd("M500\n");                          // store new Z offset in EEprom
        }
        infoMenu.cur--;
        break;
      case KEY_ICON_5:
        elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
        probeOffsetItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
        menuDrawItem(&probeOffsetItems.items[key_num], key_num);
        break;
      case KEY_ICON_6:
        //if(storeCmd("M851 Z%.2f\n",0))
        if(now != 0)
        {
          storeCmd("G1 Z%.2f\n",(now * -1));          // move nozzle back
          now = 0.0f;                                 // set new Z offset to 0
        }
        break;
      case KEY_ICON_7:
        mustStoreCmd("G90\n");                        // set absolute position mode
        storeCmd("M211 S1\n");                        // enable Software Endstop
        infoMenu.cur--;
        break;
      default :
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            now += elementsUnit.ele[elementsUnit.cur]*encoderPosition;
            storeMoveCmdZ(Z_AXIS, encoderPosition > 0 ? 1 : -1);
            encoderPosition = 0;
          }
        #endif
        break;
      }
    if(now != new_offset)
    {
      new_offset = now;
      showProbeOffset(now, probe_offset_value);
    }
    loopProcess();
  }
}
