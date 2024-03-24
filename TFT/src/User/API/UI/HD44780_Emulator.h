#ifndef _HD44780_SIMULATOR_H_
#define _HD44780_SIMULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void HD44780_ParseRecv(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
