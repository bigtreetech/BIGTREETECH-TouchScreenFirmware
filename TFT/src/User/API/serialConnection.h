#ifndef _SERIAL_CONNECTION_H_
#define _SERIAL_CONNECTION_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BAUDRATE_COUNT 9

extern const uint32_t baudrateList[BAUDRATE_COUNT];

void Serial_ReSourceDeInit(void);
void Serial_ReSourceInit(void);

#ifdef __cplusplus
}
#endif

#endif
