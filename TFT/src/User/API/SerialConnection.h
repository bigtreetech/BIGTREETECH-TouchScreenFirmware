#ifndef _SERIAL_CONNECTION_H_
#define _SERIAL_CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define BAUDRATE_COUNT 9

extern const uint32_t baudrateList[BAUDRATE_COUNT];
extern const char * const baudrateNames[BAUDRATE_COUNT];

void Serial_ReSourceInit(void);
void Serial_ReSourceDeInit(void);

#ifdef __cplusplus
}
#endif

#endif
