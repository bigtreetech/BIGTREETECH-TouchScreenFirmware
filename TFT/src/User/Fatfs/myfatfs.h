#ifndef _MYFATFS_H_
#define _MYFATFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool mountSDCard(void);
bool mountUSBDisk(void);
bool scanPrintFilesFatFs(void);

bool f_file_exists(const char * path);
bool f_dir_exists(const char * path);
bool f_remove_full_dir(const char * path);

#ifdef __cplusplus
}
#endif

#endif
