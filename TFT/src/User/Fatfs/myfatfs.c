#include "myfatfs.h"
#include "includes.h"
#include "diskio.h"

FATFS fatfs[FF_VOLUMES];  // FATFS work area

/*
 mount SD Card from Fatfs
 true: mount ok
 false: mount failed
*/
bool mountSDCard(void)
{
  return (f_mount(&fatfs[VOLUMES_SD_CARD], "SD:", 1) == FR_OK);
}

/*
 mount U disk from Fatfs
*/
bool mountUDisk(void)
{
  return (f_mount(&fatfs[VOLUMES_U_DISK], "U:", 1) == FR_OK);
}

/*
 scanf gcode file in current path
 true: scanf ok
 false: opendir failed
*/
bool scanPrintFilesFatFs(void)
{
  FILINFO finfo;
  uint16_t len = 0;
  DIR dir;
  uint8_t i = 0;

  clearInfoFile();

  if (f_opendir(&dir, infoFile.title) != FR_OK)
    return false;

  for (;;)
  {
    if (f_readdir(&dir, &finfo) != FR_OK || finfo.fname[0] == 0)
      break;
    if ((finfo.fattrib & AM_HID) != 0)
      continue;
    if (infoFile.fileCount >= FILE_NUM && infoFile.folderCount >= FOLDER_NUM)
      break;

    len = strlen(finfo.fname) + 1;
    if ((finfo.fattrib & AM_DIR) == AM_DIR)
    {
      if (infoFile.folderCount >= FOLDER_NUM)
        continue;

      infoFile.folder[infoFile.folderCount] = malloc(len);
      if (infoFile.folder[infoFile.folderCount] == NULL)
        break;
      memcpy(infoFile.folder[infoFile.folderCount++], finfo.fname, len);
    }
    else
    {
      if (infoFile.fileCount >= FILE_NUM)
        continue;

      if (strstr(finfo.fname, ".g") == NULL)  // support "*.g","*.gco" and "*.gcode"
        continue;

      infoFile.file[infoFile.fileCount] = malloc(len);
      if (infoFile.file[infoFile.fileCount] == NULL)
        break;
      memcpy(infoFile.file[infoFile.fileCount++], finfo.fname, len);
    }
  }

  f_closedir(&dir);

  for (i = 0; i < infoFile.folderCount / 2; i++)
  {
    char *temp = infoFile.folder[i];
    infoFile.folder[i] = infoFile.folder[infoFile.folderCount - i - 1];
    infoFile.folder[infoFile.folderCount - i - 1] = temp;
  }
  for (i = 0; i < infoFile.fileCount / 2; i++)
  {
    char *temp = infoFile.file[i];
    infoFile.file[i] = infoFile.file[infoFile.fileCount - i - 1];
    infoFile.file[infoFile.fileCount - i - 1] = temp;
  }
  return true;
}

/*
void GUI_DispDate(uint16_t date, uint16_t time)
{
  char buf[100];
  static uint8_t i=0;
  sprintf(buf,"%d/%d/%d--%d:%d:%d",1980+(date>>9),(date>>5)&0xF,date&0x1F,time>>11,(time>>5)&0x3F,time&0x1F);
  GUI_DispString(0,i,(uint8_t* )buf,0);
  i+=16;
}
*/

bool Get_NewestGcode(const TCHAR* path)
{
  uint32_t date = 0;
  FILINFO finfo;
  uint16_t len = 0;
  DIR dirs;
  char status = 0;

  if (f_opendir(&dirs, path) != FR_OK)
    return false;

  len = strlen(path);
  while (f_readdir(&dirs, &finfo) == FR_OK)
  {
    if (finfo.fname[0] == 0)
      break;
    if ((finfo.fattrib & AM_HID) != 0)
      continue;

    if ((finfo.fattrib & AM_DIR) == AM_DIR)
    {
      char *nextdirpath = malloc(len + strlen(finfo.fname) + 2);
      if (nextdirpath == NULL)
        break;

      strcpy(nextdirpath, path);
      strcat(nextdirpath, "/");
      strcat(nextdirpath, finfo.fname);

      status |= Get_NewestGcode(nextdirpath);
      free(nextdirpath);
      nextdirpath = NULL;
    }
    else
    {
      if (strstr(finfo.fname, ".gcode") == NULL)
        continue;
      if (((finfo.fdate << 16) | finfo.ftime) < date)
        continue;

      date = (finfo.fdate << 16) | finfo.ftime;
      resetInfoFile();

      if (len + strlen(finfo.fname) + 2 > MAX_PATH_LEN)
        break;

      strcpy(infoFile.title, path);
      strcat(infoFile.title, "/");
      strcat(infoFile.title, finfo.fname);
      status = 1;
    }
  }
  return status;
}

bool f_file_exists(const TCHAR* path)
{
  FIL tmp;

  if (f_open(&tmp, path, FA_OPEN_EXISTING) == FR_OK)
  {
    f_close(&tmp);
    return true;
  }
  return false;
}

bool f_dir_exists(const TCHAR* path)
{
  DIR tmp;

  if (f_opendir(&tmp, path) == FR_OK)
  {
    f_closedir(&tmp);
    return true;
  }
  return false;
}

FRESULT f_remove_node (
  TCHAR* path,   // Path name buffer with the sub-directory to delete
  UINT sz_buff,  // Size of path name buffer (items)
  FILINFO* fno   // Name read buffer
)
{
  UINT i, j;
  FRESULT fr;
  DIR dir;

  fr = f_opendir(&dir, path);  // Open the directory
  if (fr != FR_OK) return fr;

  for (i = 0; path[i]; i++);  // Get current path length
  path[i++] = '/';

  for (;;)
  {
    fr = f_readdir(&dir, fno);  // Get a directory item
    if (fr != FR_OK || !fno->fname[0])  // End of directory?
      break;
    j = 0;
    do
    { // Make a path name
      if (i + j >= sz_buff)
      { // Buffer over flow?
        fr = FR_DENIED;
        goto end_delete;  // Fails with 100 when buffer overflow
      }
      path[i + j] = fno->fname[j];
    } while (fno->fname[j++]);
    if (fno->fattrib & AM_DIR)
    { // Item is a directory
      fr = f_remove_node(path, sz_buff, fno);
    }
    else
    { // Item is a file
      fr = f_unlink(path);
    }
    if (fr != FR_OK)
      break;
  }

end_delete:
  path[--i] = 0;  // Restore the path name
  f_closedir(&dir);

  if (fr == FR_OK) fr = f_unlink(path);  // Delete the empty directory
  return fr;
}

bool f_remove_full_dir(const TCHAR* path)
{
  #define BUFFER_SIZE 256

  char dirBuffer[BUFFER_SIZE];
  FILINFO tmpInfo;

  strncpy(dirBuffer, path, BUFFER_SIZE);
  if (f_remove_node(dirBuffer, BUFFER_SIZE, &tmpInfo) == FR_OK)
  {
    return true;
  }
  return false;
}
