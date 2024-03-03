#ifndef _Z_OFFSET_H_
#define _Z_OFFSET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// called by menuTuning()
void zOffsetSetMenu(bool probeOffset);

void menuZOffset(void);

#ifdef __cplusplus
}
#endif

#endif
