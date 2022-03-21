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
#define MAX_PATH_LEN 420

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
  FS_SOURCE source;                // selected file source. TFT media or onboard media
  ONBOARD_SOURCE boardSource;      // SD or USB for onboard media only
  TCHAR title[MAX_PATH_LEN];       // selected file path
  TCHAR * longFolder[FOLDER_NUM];  // long folder name buffer from onboard media only
  TCHAR * folder[FOLDER_NUM];      // folder list buffer
  TCHAR * longFile[FILE_NUM];      // long file name buffer from onboard media only
  TCHAR * file[FILE_NUM];          // file list buffer
  uint16_t folderCount;            // current folder count
  uint16_t fileCount;              // current gcode file count
  uint16_t fileIndex;              // selected file index
  uint16_t curPage;                // current display page index (5 files per page)
  bool modelIcon;                  // 1: model preview icon exist, 0: not exist
} MYFILE;

extern MYFILE infoFile;

void setPrintModelIcon(bool exist);
bool isPrintModelIcon(void);

TCHAR * getCurFileSource(void);                 // get FS's ID of current source
bool mountFS(void);                             // mount FS of current source
bool scanPrintFiles(void);                      // scan files in current source and create a file list
void clearInfoFile(void);                       // clear and free memory for file list

void resetInfoFile(void);                       // clear file list and path
char * getPathTail(void);                       // skip path information, if any
bool EnterDir(const char * nextdir);            // check and open folder
void ExitDir(void);                             // close folder
bool IsRootDir(void);                           // check if current folder is root
char * isSupportedFile(const char * filename);  // check if filename provides a supported filename extension

// called in Print.c
char * getFoldername(uint8_t index);
char * getFilename(uint8_t index);
char * hideFilenameExtension(uint8_t index);
char * restoreFilenameExtension(uint8_t index);

// called in PrintingMenu.c
char * getPrintFilename(void);                // get print filename according to print originator (remote or local to TFT)
bool getPrintTitle(char * buf, uint8_t len);  // get print title according to print originator (remote or local to TFT)

// called in menu.c
bool volumeExists(uint8_t src);
void loopVolumeSource(void);

#ifdef __cplusplus
}
#endif

#endif
