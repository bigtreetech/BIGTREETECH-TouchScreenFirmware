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
static inline bool compareFile(char * name1, uint32_t date1, char * name2, uint32_t date2)
{
  if (infoSettings.files_sort_by <= SORT_DATE_OLD_FIRST)  // sort by date
  {
    // file with most recent date displays first in newest first and last in oldest first
    return ((date1 > date2) == GET_BIT(infoSettings.files_sort_by, 0));
  }
  else  // sort by name
  {
    uint16_t maxlen = (strlen(name1) < strlen(name2)) ? strlen(name1) : strlen(name2);

    // compare each character
    for (uint16_t i = 0; i < maxlen; i++)
    {
      // convert all upper case characters to lower case
      char a = (name1[i] > 64 && name1[i] < 91) ? (name1[i] + 32) : name1[i];
      char b = (name2[i] > 64 && name2[i] < 91) ? (name2[i] + 32) : name2[i];

      if (a != b)
        return ((a < b) == GET_BIT(infoSettings.files_sort_by, 0));
    }

    // file with longer name displays last in ascending order and first in descending order
    return ((strlen(name1) < strlen(name2)) == GET_BIT(infoSettings.files_sort_by, 0));
  }
}

/**
 * sort file list
 */
void sortFile(uint16_t fileCount, TCHAR * fileName[], uint32_t fileDate[])
{
  for (int i = 1; i < fileCount; i++)
  {
    // compare file date with each other
    for (int j = i;
         j > 0 && compareFile(fileName[j - 1], fileDate[j - 1], fileName[j], fileDate[j]);
         j--)
    {
      // swap places if not in order
      char * tmp = fileName[j - 1];
      int32_t tmpInt = fileDate[j - 1];

      fileName[j - 1] = fileName[j];
      fileName[j] = tmp;
      fileDate[j - 1] = fileDate[j];
      fileDate[j] = tmpInt;
    }
  }
}

/**
 * mount SD card from Fatfs
 * true: mount ok
 * false: mount failed
 */
bool mountSDCard(void)
{
  return (f_mount(&fatfs[VOLUMES_SD_CARD], SD_ROOT_DIR, 1) == FR_OK);
}

/**
 * mount USB disk from Fatfs
 * true: mount ok
 * false: mount failed
 */
bool mountUSBDisk(void)
{
  return (f_mount(&fatfs[VOLUMES_USB_DISK], USB_ROOT_DIR, 1) == FR_OK);
}

/**
 * scanf gcode file in current path
 * true: scanf ok
 * false: opendir failed
 */
bool scanPrintFilesFatFs(void)
{
  DIR dir;
  FILINFO finfo;
  uint32_t folderDate[FOLDER_NUM];
  uint32_t fileDate[FILE_NUM];

  clearInfoFile();

  if (f_opendir(&dir, infoFile.path) != FR_OK)
    return false;

  for (;;)
  {
    if (f_readdir(&dir, &finfo) != FR_OK || finfo.fname[0] == 0 ||
        (infoFile.fileCount >= FILE_NUM && infoFile.folderCount >= FOLDER_NUM))
      break;

    if ((finfo.fattrib & AM_HID) != 0)
      continue;

    if ((finfo.fattrib & AM_DIR) == AM_DIR)  // if folder
    {
      if (addFile(false, finfo.fname, NULL) == true)  // if folder successfully added to folder list
        folderDate[infoFile.folderCount - 1] = ((uint32_t)(finfo.fdate) << 16) | finfo.ftime;  // copy date/time modified
    }
    else  // if file
    {
      if (addFile(true, finfo.fname, NULL) == true)  // if file successfully added to file list
        fileDate[infoFile.fileCount - 1] = ((uint32_t)(finfo.fdate) << 16) | finfo.ftime;  // copy date/time modified
    }
  }

  f_closedir(&dir);

  sortFile(infoFile.folderCount, infoFile.folder, folderDate);  // sort folder list
  sortFile(infoFile.fileCount, infoFile.file, fileDate);        // sort file list

  return true;
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

static FRESULT f_remove_node(
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

  strncpy_no_pad(dirBuffer, path, BUFFER_SIZE);
  if (f_remove_node(dirBuffer, BUFFER_SIZE, &tmpInfo) == FR_OK)
  {
    return true;
  }
  return false;
}
