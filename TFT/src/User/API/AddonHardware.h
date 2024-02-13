#ifndef _SMART_FEATURES_H_
#define _SMART_FEATURES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"  // for PS_ON_PIN, FIL_RUNOUT_PIN etc.

// power supply
#ifdef PS_ON_PIN
  void PS_ON_Init(void);  // power supply control pins initialization
  void PS_ON_On(void);    // power supply control turn on, M80
  void PS_ON_Off(void);   // power supply control turn off, M81
#endif

// filament runout detection
#ifdef FIL_RUNOUT_PIN
  void FIL_Runout_Init(void);
  void FIL_PosE_ClearSendingWaiting(void);  // clear sending waiting for position query
  void FIL_SFS_SetAlive(bool alive);
  void FIL_BE_CheckRunout(void);            // called in loopBackEnd()
  void FIL_FE_CheckRunout(void);            // called in loopFrontEnd()
#endif

#ifdef __cplusplus
}
#endif

#endif
