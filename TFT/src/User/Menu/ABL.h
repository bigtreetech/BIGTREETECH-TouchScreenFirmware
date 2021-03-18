#ifndef _ABL_H_
#define _ABL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// called by parseAck() to notify ABL process status
void ablUpdateStatus(bool succeeded);

// Start ABL process
void ablStart(void);

void menuUBLSave(void);
void menuUBLLoad(void);

#ifdef __cplusplus
}
#endif

#endif
