#ifndef _SMART_FEATURES_H_
#define _SMART_FEATURES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"

// Power Supply
#ifdef PS_ON_PIN
  void PS_ON_Init(void);
  void PS_ON_On(void);
  void PS_ON_Off(void);
#endif

// Filament runout detection
#ifdef FIL_RUNOUT_PIN
  #define POS_E_UPDATE_TIME   2000
  #define ALARM_REMINDER_TIME 10000

  void FIL_Runout_Init(void);
  void FIL_PosE_SetUpdateWaiting(bool waiting);
  void FIL_SFS_SetAlive(bool alive);
  void FIL_BE_CheckRunout(void);
  void FIL_FE_CheckRunout(void);
#endif

// Smart home
#ifdef SMART_HOME
  #define LONG_TOUCH (MODE_SWITCHING_INTERVAL / 3)  // keep it lower than MODE_SWITCHING_INTERVAL

  void loopCheckBack(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
