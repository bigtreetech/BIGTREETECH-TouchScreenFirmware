#ifndef _VFS_H_
#define _VFS_H_

#include "stdint.h"
#include "stdbool.h"
#include "ff.h"

#define FOLDER_NUM   255
#define FILE_NUM     255
#define MAX_PATH_LEN 1024

typedef enum {
    TFT_SD,
    TFT_UDISK,
    BOARD_SD
}FS_SOURCE;

typedef struct 
{
	TCHAR title[MAX_PATH_LEN]; // max path depth
	TCHAR *folder[FOLDER_NUM]; // folder name buffer
	TCHAR *file[FILE_NUM]; // file name buffer
	uint16_t F_num; // current folder number
	uint16_t f_num; // current gcode file number
	uint16_t cur_page; // current display page index (5 files per page)
	FS_SOURCE source; //The source of the file. TFT SD or ONBOARD SD.
	TCHAR *Longfile[FILE_NUM]; // Long file name buffer from ONBOARD SD only
}MYFILE;

extern MYFILE  infoFile;

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
