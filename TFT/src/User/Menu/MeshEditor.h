#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* called by parseAck() */
bool meshIsWaitingFirstData(void);
bool meshIsWaitingData(void);
void meshUpdateData(char *dataRow);

void menuMeshEditor(void);

#ifdef __cplusplus
}
#endif
