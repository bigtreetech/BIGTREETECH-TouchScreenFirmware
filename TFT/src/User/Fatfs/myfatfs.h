#ifndef _MYFATFS_H_
#define _MYFATFS_H_

#include "stdbool.h"
bool mountSDCard(void);
bool mountUDisk(void);
bool scanPrintFilesFatFs(void);

bool f_remove_full_dir(const char* path);
bool f_dir_exists(const char* path);
bool f_file_exists(const char* path);
  
#endif
