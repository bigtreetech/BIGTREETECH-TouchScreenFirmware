#ifndef _MONITORING_H_
#define _MONITORING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for DEBUG_MONITORING

#ifdef DEBUG_MONITORING
  typedef struct
  {
    uint32_t tx_cmd;
    uint32_t tx_cmd_rate;
    uint32_t tx_bytes;
    uint32_t tx_bytes_rate;
    uint32_t rx_ack;
    uint32_t rx_ack_rate;
    uint32_t rx_bytes;
    uint32_t rx_bytes_rate;
    uint32_t scan_rate_counter;
    uint32_t scan_rate_per_second;
  } MONITORING;

  extern MONITORING infoMonitoring;

  #define UPD_TX_KPIS(bytes)            \
    {                                   \
      infoMonitoring.tx_cmd++;          \
      infoMonitoring.tx_bytes += bytes; \
    }

  #define UPD_RX_KPIS(bytes)            \
    {                                   \
      infoMonitoring.rx_ack++;          \
      infoMonitoring.rx_bytes += bytes; \
    }

  #define UPD_SCAN_RATE() infoMonitoring.scan_rate_counter++

  #define AVG_KPIS()                                                          \
    {                                                                         \
      infoMonitoring.tx_cmd_rate = infoMonitoring.tx_cmd;                     \
      infoMonitoring.tx_cmd = 0;                                              \
      infoMonitoring.tx_bytes_rate = infoMonitoring.tx_bytes;                 \
      infoMonitoring.tx_bytes = 0;                                            \
      infoMonitoring.rx_ack_rate = infoMonitoring.rx_ack;                     \
      infoMonitoring.rx_ack = 0;                                              \
      infoMonitoring.rx_bytes_rate = infoMonitoring.rx_bytes;                 \
      infoMonitoring.rx_bytes = 0;                                            \
      infoMonitoring.scan_rate_per_second = infoMonitoring.scan_rate_counter; \
      infoMonitoring.scan_rate_counter = 0;                                   \
    }

  void menuMonitoring(void);
#else
  #define UPD_TX_KPIS(bytes)
  #define UPD_RX_KPIS(bytes)
  #define UPD_SCAN_RATE()
  #define AVG_KPIS()
#endif

#ifdef __cplusplus
}
#endif

#endif
