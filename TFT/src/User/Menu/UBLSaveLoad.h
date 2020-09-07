#ifndef _UBL_SAVE_LOAD_H_
#define _UBL_SAVE_LOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void menuUBLSaveLoad(void);
void ublSaveLoad(bool isSaving);
void saveloadConfirm(void);

#ifdef __cplusplus
}
#endif

#endif
