#ifndef _SERIAL_CONNECTION_H_
#define _SERIAL_CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for SERIAL_PORT_2 etc...
#include <stdbool.h>
#include <stdint.h>

#ifdef SERIAL_PORT_2
  typedef enum
  {
    PORT_2 = 0,
    #ifdef SERIAL_PORT_3
      PORT_3,
    #endif
    #ifdef SERIAL_PORT_4
      PORT_4,
    #endif
    PORT_COUNT
  } MS_PORT;  // Multi-Serial ports

  typedef struct
  {
    uint8_t port;        // physical port (e.g. _USART1) related to logical port (e.g. SERIAL_PORT_2)
    uint16_t cacheSize;  // queue size for sending/receiving data to/from the port
    bool activePort;     // set to "true" if the port is connected to an active device (a device that already sent data to the TFT)
  } MS_PORT_INFO;        // Multi-Serial port info

  extern MS_PORT_INFO multiSerialPort[PORT_COUNT];
#endif

#define BAUDRATE_COUNT 9

extern const uint32_t baudrateList[BAUDRATE_COUNT];
extern const char * const baudrateNames[BAUDRATE_COUNT];

void Serial_ReSourceInit(void);
void Serial_ReSourceDeInit(void);

#ifdef __cplusplus
}
#endif

#endif
