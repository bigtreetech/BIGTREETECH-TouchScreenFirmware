#ifndef _MYGCODEFS_H_
#define _MYGCODEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool mountGcodeSDCard(void);
bool scanPrintFilesGcodeFs(void);

#ifdef __cplusplus
}
#endif

#endif
