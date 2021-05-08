#ifndef _VFS_H_
#define _VFS_H_

#include <stdint.h>
#include <stdbool.h>
#include "ff.h"

#define FOLDER_NUM   255
#define FILE_NUM     255
#define MAX_PATH_LEN 1024

typedef enum
{
  TFT_SD,
  TFT_UDISK,
  BOARD_SD,
  BOARD_SD_REMOTE
} FS_SOURCE;

typedef struct
{
  TCHAR title[MAX_PATH_LEN];   // selected file path
  TCHAR * folder[FOLDER_NUM];  // folder list buffer
  TCHAR * file[FILE_NUM];      // file list buffer
  uint16_t folderCount;        // current folder count
  uint16_t fileCount;          // current gcode file count
  uint16_t cur_page;           // current display page index (5 files per page)
  uint16_t fileIndex;          // selected file index
  FS_SOURCE source;            // The source of the file. TFT SD or ONBOARD SD.
  TCHAR * Longfile[FILE_NUM];  // Long file name buffer from ONBOARD SD only
  bool model_icon;             // 1: model preview icon exist, 0: not exist
} MYFILE;

extern MYFILE infoFile;

void setPrintModelIcon(bool exist);
bool isPrintModelIcon(void);

bool mountFS(void);
bool scanPrintFiles(void);

TCHAR* getCurFileSource(void);
void clearInfoFile(void);
void resetInfoFile(void);
bool EnterDir(char *nextdir);
void ExitDir(void);
bool IsRootDir(void);

bool isVolumeExist(uint8_t src);
void loopVolumeSource(void);

#endif
