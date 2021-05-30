#ifndef _LOADUNLOAD_H_
#define _LOADUNLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

void menuLoadUnload(void);

typedef enum
{
    NONE = 0,
    LOAD_REQUESTED,
    UNLOAD_REQUESTED,
    LOAD_STARTED,
    UNLOAD_STARTED,
} CMD_TYPE;

#ifdef __cplusplus
}
#endif

#endif
