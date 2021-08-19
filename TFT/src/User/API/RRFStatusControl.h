#ifndef _RRF_STATUS_CONTROL_H_
#define _RRF_STATUS_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void rrfStatusQuery(void);
void rrfStatusQueryFast(void);
void rrfStatusQueryNormal(void);
void rrfStatusSet(char status);
void rrfStatusSetBusy(void);
bool rrfStatusIsBusy(void);

#ifdef __cplusplus
}
#endif

#endif