#ifndef _MESH_EDITOR_H_
#define _MESH_EDITOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// called by parseAck()
bool meshIsWaitingData(void);
void meshUpdateData(char *dataRow);

void menuMeshEditor(void);

#ifdef __cplusplus
}
#endif

#endif
