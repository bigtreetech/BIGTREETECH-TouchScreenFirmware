#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* called by parseAck() to notify MBL process status */
void mblUpdateStatus(bool succeeded);

void menuMBL(void);

#ifdef __cplusplus
}
#endif
