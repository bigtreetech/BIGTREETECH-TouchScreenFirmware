#ifndef _PID_H_
#define _PID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum
{
  PID_IDLE = 0,
  PID_RUNNING,
  PID_TIMEOUT,
  PID_FAILED,
  PID_SUCCESS,
} PID_STATUS;

// called by parseAck() to notify PID process status
void pidUpdateStatus(PID_STATUS status);

void menuPid(void);

#ifdef __cplusplus
}
#endif

#endif
