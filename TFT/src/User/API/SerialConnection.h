#ifndef _SERIAL_CONNECTION_H_
#define _SERIAL_CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"  // for SERIAL_PORT_2 etc.
#include "uart.h"      // for _UART_CNT etc.

#define BAUDRATE_COUNT 10

typedef enum
{
  ALL_PORTS = -1,
  PORT_1 = 0,
  #ifdef SERIAL_PORT_2
    PORT_2,
  #endif
  #ifdef SERIAL_PORT_3
    PORT_3,
  #endif
  #ifdef SERIAL_PORT_4
    PORT_4,
  #endif
  SERIAL_PORT_COUNT
} SERIAL_PORT_INDEX;  // serial port index for all enabled serial ports (This is not actual physical port number)

typedef struct
{
  uint8_t port;             // physical port (e.g. _USART1) related to serial port (e.g. 0 for SERIAL_PORT, 1 for SERIAL_PORT_2 etc...)
  uint16_t cacheSize;       // queue size for sending/receiving data to/from the port
  const char * const id;    // serial port ID (e.g. "" for SERIAL_PORT, "2" for SERIAL_PORT_2 etc...)
  const char * const desc;  // serial port description (e.g. "1 - Printer" for SERIAL_PORT, "2 - WIFI" for SERIAL_PORT_2 etc...)
} SERIAL_PORT_INFO;         // serial port info

extern const SERIAL_PORT_INFO serialPort[SERIAL_PORT_COUNT];  // serial port (index 0 for SERIAL_PORT, 1 for SERIAL_PORT_2 etc...)
extern const uint32_t baudrateValues[BAUDRATE_COUNT];         // baudrate values
extern const char * const baudrateNames[BAUDRATE_COUNT];      // baudrate names

void Serial_Init(int8_t port);    // -1 to apply to all serial ports (primary and supplementary)
void Serial_DeInit(int8_t port);  // -1 to apply to all serial ports (primary and supplementary)

#ifdef __cplusplus
}
#endif

#endif
