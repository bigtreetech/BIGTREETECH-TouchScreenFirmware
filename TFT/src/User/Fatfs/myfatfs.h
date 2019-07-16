#ifndef _MYFATFS_H_
#define _MYFATFS_H_

#include "stdbool.h"

bool mountSDCard(void);
bool mountUDisk(void);
bool scanPrintFilesFatFs(void);
bool Get_NewestGcode(const char* path);

#endif 
