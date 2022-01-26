#ifndef _VFS_H_
#define _VFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "ff.h"

#define FOLDER_NUM   255
#define FILE_NUM     255
#define MAX_PATH_LEN 1024

typedef enum
{
  TFT_SD,
  TFT_USB_DISK,
  BOARD_SD,
  BOARD_SD_REMOTE,
  REMOTE_HOST
} FS_SOURCE;

typedef struct
{
  TCHAR title[MAX_PATH_LEN];   // selected file path
  TCHAR * folder[FOLDER_NUM];  // folder list buffer
  TCHAR * file[FILE_NUM];      // file list buffer
  uint16_t folderCount;        // current folder count
  uint16_t fileCount;          // current gcode file count
  uint16_t curPage;            // current display page index (5 files per page)
  uint16_t fileIndex;          // selected file index
  FS_SOURCE source;            // source of the file. TFT SD or ONBOARD SD.
  TCHAR * longFile[FILE_NUM];  // long file name buffer from ONBOARD SD only
  bool modelIcon;              // 1: model preview icon exist, 0: not exist
} MYFILE;

extern MYFILE infoFile;

void setPrintModelIcon(bool exist);
bool isPrintModelIcon(void);

bool mountFS(void);
bool scanPrintFiles(void);

TCHAR * getCurFileSource(void);
void clearInfoFile(void);
void resetInfoFile(void);
bool EnterDir(char * nextdir);
void ExitDir(void);
bool IsRootDir(void);
char * isSupportedFile(char * filename);
char * hideFileExtension(uint8_t index);
char * restoreFileExtension(uint8_t index);

bool volumeExists(uint8_t src);
void loopVolumeSource(void);

#ifdef __cplusplus
}
#endif

#endif
