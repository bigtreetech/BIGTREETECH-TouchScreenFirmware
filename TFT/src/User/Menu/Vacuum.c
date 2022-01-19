//TG MODIFIED BY T.GIOIOSA
#include "includes.h"

const char *const  vacuumID[MAX_VACUUM_COUNT]         = VACUUM_SIGN_ID;
const char *const  vacuumDisplayID[MAX_VACUUM_COUNT]  = VACUUM_DISPLAY_ID;
const char *const  vacuumCmd[MAX_VACUUM_COUNT]        = VACUUM_CMD;

static uint32_t nextVacuumTime = 0;
#define NEXT_VACUUM_WAIT 1000  // 1 second is 1000

MENUITEMS vacuumItems = {
// title
LABEL_VACUUM,
// icon                       label
 {{ICON_APPLY,                LABEL_ON},
  {ICON_STOP,                 LABEL_OFF},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_AUTO_MODE,            LABEL_AUTO_MODE},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

uint8_t vacuumState = 0;      // start vacuum off, bit0 = on/off ctl, bit1 = auto mode flag on/off
uint8_t lastvacuumState = 0;
void menuVacuum(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  if((vacuumState & 2) == 0){       // reset menu if auto mode is set off from somewhere else
    vacuumItems.items[3].icon = ICON_AUTO_MODE;
    vacuumItems.items[3].label.index = LABEL_AUTO_MODE;
  }
  menuDrawPage(&vacuumItems);
  while(infoMenu.menu[infoMenu.cur] == menuVacuum)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: vacuumState |= 1; break;    //TG 2/18/21 this should send command to turn vacuum ON state bit 0 only
      case KEY_ICON_1: vacuumState &= ~1; break;   //TG 2/18/21 this should send command to turn vacuum OFF state bit 0 only
      case KEY_ICON_3:                             //TG 2/18/21 this should send command to set vacuum AUTO to follow spindle ON/OFF 
        if((vacuumState & 2) == 0)
        {
          vacuumState |= 2; 
          vacuumItems.items[3].icon = ICON_AUTO_ON;
          vacuumItems.items[3].label.index = LABEL_AUTO_ON;
        }
        else
        {
          vacuumState &= ~2;
          vacuumItems.items[3].icon = ICON_AUTO_MODE;
          vacuumItems.items[3].label.index = LABEL_AUTO_MODE;
        }
        menuDrawItem(&vacuumItems.items[key_num], key_num);
        break;        
      // case KEY_ICON_4: storeCmd("G92 X0\n"); break;
      // case KEY_ICON_5: storeCmd("G92 Y0\n"); break;
      // case KEY_ICON_6: storeCmd("G92 Z19.75\n"); break;
      case KEY_ICON_7: infoMenu.cur--;      break;
      default:break;
    }
    loopProcess();
  }
}

uint8_t getVacuumState(void){
  return vacuumState;
}

void loopVacuum(void)    //TG this is called by loopBackEnd() in menu.c to update the spindle speed to the machine
{
  if ((lastvacuumState != vacuumState) && (OS_GetTimeMs() > nextVacuumTime))
  {
    if((vacuumState & 1) == 1)
      vacuum_set(255);
    else
      vacuum_set(0);
    nextVacuumTime = OS_GetTimeMs() + NEXT_VACUUM_WAIT; // avoid rapid fire, clogging the queue
  } // if value changed and time to send
}// loopSpindle

// gets "M42" from vacuumCmd[i], adds I1 (ignore protected pins) and M1 (set pin to output),
// then "S1 or S0" on or off giving "M42 I1 M1 Sn"
void vacuum_set (uint8_t state) {
  uint8_t vac_pin = VACUUM_CTL_PIN;   // this is the Vaccum hardware pin number used in Marlin
  if (storeCmd("%s I1 M1 P%i S%i\n", vacuumCmd[0], vac_pin, state))
    {
      vacuumState = (state > 0) ? (vacuumState | 1) : (vacuumState & ~1); // adjust state
      lastvacuumState = vacuumState;  // update vacuum state here only, not in spindleControl.c 
    }
}