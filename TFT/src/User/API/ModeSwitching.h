#ifndef _MODE_SWITCHING_H_
#define _MODE_SWITCHING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define MODE_SWITCHING_INTERVAL 1500  // long pressed 1.5s

void Mode_Switch(void);
void Mode_EnableSwitching(bool isEnabled);
void Mode_CheckSwitching(void);

#ifdef __cplusplus
}
#endif

#endif
