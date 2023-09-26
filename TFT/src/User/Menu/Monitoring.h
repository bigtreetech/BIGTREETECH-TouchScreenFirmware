#ifndef _MONITORING_H_
#define _MONITORING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for DEBUG_MONITORING

#ifdef DEBUG_MONITORING
  typedef struct
  {
    uint32_t scan_rate_counter;
    uint32_t scan_rate_per_second;
  } MONITORING;

  extern MONITORING infoMonitoring;

  #define UPD_SCAN_RATE() infoMonitoring.scan_rate_counter++
  #define AVG_SCAN_RATE()                                                     \
    {                                                                         \
      infoMonitoring.scan_rate_per_second = infoMonitoring.scan_rate_counter; \
      infoMonitoring.scan_rate_counter = 0;                                   \
    }

  void menuMonitoring(void);
#else
  #define UPD_SCAN_RATE()
  #define AVG_SCAN_RATE()
#endif

#ifdef __cplusplus
}
#endif

#endif
