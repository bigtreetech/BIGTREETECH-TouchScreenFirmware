#ifndef _VFS_H_
#define _VFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "ff.h"

#define FOLDER_NUM    255
#define FILE_NUM      255
#define MAX_PATH_LEN 1024

#define SD_ROOT_DIR      "SD:"
#define USBDISK_ROOT_DIR "U:"

typedef enum
{
  TFT_SD,
  TFT_USB_DISK,
  BOARD_MEDIA,
  BOARD_MEDIA_REMOTE,
  REMOTE_HOST
} FS_SOURCE;

typedef enum
{
  BOARD_SD,
  BOARD_USB
} ONBOARD_SOURCE;

typedef struct
{
  TCHAR title[MAX_PATH_LEN];       // selected file path
  TCHAR * folder[FOLDER_NUM];      // folder list buffer
  TCHAR * file[FILE_NUM];          // file list buffer
  uint16_t folderCount;            // current folder count
  uint16_t fileCount;              // current gcode file count
  uint16_t curPage;                // current display page index (5 files per page)
  uint16_t fileIndex;              // selected file index
  FS_SOURCE source;                // source of the file. TFT media or onboard media.
  ONBOARD_SOURCE boardSource;      // SD or USB for onboard media only
  TCHAR * longFile[FILE_NUM];      // long file name buffer from onboard media only
  TCHAR * longFolder[FOLDER_NUM];  // long folder name buffer from onboard media only
  bool modelIcon;                  // 1: model preview icon exist, 0: not exist
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
char * getFoldername(uint8_t index);
char * getFilename(uint8_t index);
char * hideFilenameExtension(uint8_t index);
char * restoreFilenameExtension(uint8_t index);

bool volumeExists(uint8_t src);
void loopVolumeSource(void);

#ifdef __cplusplus
}
#endif

#endif
