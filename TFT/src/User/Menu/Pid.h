#ifndef _PID_H_
#define _PID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void pidUpdateStatus(bool succeeded);

void menuPid(void);

#ifdef __cplusplus
}
#endif

#endif
