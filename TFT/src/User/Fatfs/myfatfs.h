#ifndef _MYFATFS_H_
#define _MYFATFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

enum
{
  SORT_DATE_NEW_FIRST = 0,
  SORT_DATE_OLD_FIRST,
  SORT_NAME_ASCENDING,
  SORT_NAME_DESCENDING,
  SORT_BY_COUNT
};

bool mountSDCard(void);
bool mountUDisk(void);
bool scanPrintFilesFatFs(void);

bool f_remove_full_dir(const char* path);
bool f_dir_exists(const char* path);
bool f_file_exists(const char* path);

#ifdef __cplusplus
}
#endif

#endif
