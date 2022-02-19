#include "myfatfs.h"
#include "includes.h"  // for infoSettings etc...
#include "diskio.h"

FATFS fatfs[FF_VOLUMES];  // FATFS work area

/**
 * @brief  Compare file/folder details according to sort settings
 *
 * @param name1 name of first file/folder
 * @param date1 date/time for first file/folder
 * @param name2 name of second file/folder
 * @param date2 date/time for second file/folder
 */
bool compareFile(char * name1, uint32_t date1, char * name2, uint32_t date2)
{
  // sort by date
  if (infoSettings.files_sort_by <= SORT_DATE_OLD_FIRST)
  {
    // file with most recent date displays first in newest first and last in oldest first
    return ((date1 > date2) == infoSettings.files_sort_by % 2);
  }
  // sort by name
  else
  {
    uint16_t maxlen = (strlen(name1) < strlen(name2)) ? strlen(name1) : strlen(name2);

    // compare each character
    for (uint16_t i = 0; i < maxlen; i++)
    {
      // convert all upper case characters to lower case
      char a = (name1[i] > 64 && name1[i] < 91) ? (name1[i] + 32) : name1[i];
      char b = (name2[i] > 64 && name2[i] < 91) ? (name2[i] + 32) : name2[i];

      if (a != b)
        return ((a < b) == infoSettings.files_sort_by % 2);
    }
    // file with longer name displays last in ascending order and first in descending order
    return ((strlen(name1) < strlen(name2)) == infoSettings.files_sort_by % 2);
  }
}

/*
 mount SD Card from Fatfs
 true: mount ok
 false: mount failed
*/
bool mountSDCard(void)
{
  return (f_mount(&fatfs[VOLUMES_SD_CARD], SD_ROOT_DIR, 1) == FR_OK);
}

/*
 mount USB disk from Fatfs
*/
bool mountUSBDisk(void)
{
  return (f_mount(&fatfs[VOLUMES_USB_DISK], USBDISK_ROOT_DIR, 1) == FR_OK);
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
  uint32_t folderDate[FILE_NUM];
  uint32_t fileDate[FILE_NUM];

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
    if ((finfo.fattrib & AM_DIR) == AM_DIR)  // if folder
    {
      if (infoFile.folderCount >= FOLDER_NUM)
        continue;

      infoFile.folder[infoFile.folderCount] = malloc(len);
      if (infoFile.folder[infoFile.folderCount] == NULL)
        break;

      // copy date/time modified
      folderDate[infoFile.folderCount] = ((uint32_t)(finfo.fdate) << 16) | finfo.ftime;
      // copy folder name
      memcpy(infoFile.folder[infoFile.folderCount++], finfo.fname, len);
    }
    else  // if file
    {
      if (infoFile.fileCount >= FILE_NUM)
        continue;

      if (isSupportedFile(finfo.fname) == NULL)  // if filename doesn't provide a supported filename extension
        continue;

      infoFile.file[infoFile.fileCount] = malloc(len + 1);  // plus one extra byte for filename extension check
      if (infoFile.file[infoFile.fileCount] == NULL)
        break;

      // copy date/time modified
      fileDate[infoFile.fileCount] = ((uint32_t)(finfo.fdate) << 16) | finfo.ftime;
      // copy file name and set the flag for filename extension check
      strncpy(infoFile.file[infoFile.fileCount], finfo.fname, len + 1);  // "+ 1": the flag for filename extension check
      infoFile.longFile[infoFile.fileCount] = NULL;                      // long filename is not supported, so always set it to NULL
      infoFile.fileCount++;
    }
  }

  f_closedir(&dir);

  // sort folder list
  for (int i = 1; i < infoFile.folderCount; i++)
  {
    // compare folders with each other
    for (int j = i;
         j > 0 && compareFile(infoFile.folder[j - 1], folderDate[j - 1], infoFile.folder[j], folderDate[j]);
         j--)
    {
      // swap places if not in order
      char * tmp = infoFile.folder[j - 1];
      infoFile.folder[j - 1] = infoFile.folder[j];
      infoFile.folder[j] = tmp;

      int32_t tmpInt = folderDate[j - 1];
      folderDate[j - 1] = folderDate[j];
      folderDate[j] = tmpInt;
    }
  }

  // sort file list
  for (int i = 1; i < infoFile.fileCount; i++)
  {
    // compare files with each other
    for (int j = i;
         j > 0 && compareFile(infoFile.file[j - 1], fileDate[j - 1], infoFile.file[j], fileDate[j]);
         j--)
    {
      // swap places
      char * tmp = infoFile.file[j - 1];
      infoFile.file[j - 1] = infoFile.file[j];
      infoFile.file[j] = tmp;

      int32_t tmpInt = fileDate[j - 1];
      fileDate[j - 1] = fileDate[j];
      fileDate[j] = tmpInt;
    }
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
