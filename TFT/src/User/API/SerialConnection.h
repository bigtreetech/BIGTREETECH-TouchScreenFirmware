#ifndef _SERIAL_CONNECTION_H_
#define _SERIAL_CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for SERIAL_PORT_2 etc...
#include "uart.h"      // for _UART_CNT etc...
#include <stdbool.h>
#include <stdint.h>

#define BAUDRATE_COUNT 9

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
  } EXTRA_SERIAL_PORT;  // extra serial ports

  typedef struct
  {
    uint8_t port;        // physical port (e.g. _USART1) related to logical port (e.g. 2 for SERIAL_PORT_2)
    uint16_t cacheSize;  // queue size for sending/receiving data to/from the port
    bool activePort;     // set to "true" if the port is connected to an active device (a device that already sent data to the TFT)
  } SERIAL_PORT_INFO;    // serial port info

  extern SERIAL_PORT_INFO extraSerialPort[PORT_COUNT];
#endif

extern char serialPortId[_UART_CNT][2];                     // logical port ID (e.g. 2 for SERIAL_PORT_2)
extern const uint32_t baudrateValues[BAUDRATE_COUNT];       // baudrate values
extern const char * const baudrateNames[BAUDRATE_COUNT];    // baudrate names

void Serial_ReSourceInit(void);
void Serial_ReSourceDeInit(void);

#ifdef __cplusplus
}
#endif

#endif
