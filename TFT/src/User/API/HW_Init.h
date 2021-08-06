#ifndef _HW_INIT_H_
#define _HW_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void HW_Init(void);              // init generic hardware just to allow system boot
void HW_InitMode(uint8_t mode);  // init hardware for the selected UI mode

#ifdef __cplusplus
}
#endif

#endif
