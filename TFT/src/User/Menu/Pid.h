#ifndef _PID_H_
#define _PID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* called by parseAck() to notify PID process status */
void pidUpdateStatus(bool succeeded);

void menuPid(void);

#ifdef __cplusplus
}
#endif

#endif
