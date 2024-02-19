#ifndef _RRF_STATUS_CONTROL_H_
#define _RRF_STATUS_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

extern bool starting_print;

void rrfStatusSet(char status);
bool rrfStatusIsBusy(void);
void rrfStatusSetBusy(void);
bool rrfStatusIsMacroBusy(void);
void rrfStatusSetMacroBusy(void);
void rrfStatusQueryFast(void);
void rrfStatusQueryNormal(void);
void rrfStatusQuery(void);

#ifdef __cplusplus
}
#endif

#endif
