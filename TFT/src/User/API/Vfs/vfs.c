#include "vfs.h"
#include "includes.h"

MYFILE infoFile = {"?:", {0}, {0}, 0, 0, 0, 0, TFT_SD, BOARD_SD, {0}, {0}, false};

void setPrintModelIcon(bool exist)
{
  infoFile.modelIcon = exist;
}

bool isPrintModelIcon(void)
{
  return infoFile.modelIcon;
}

bool mountFS(void)
{
  //resetInfoFile();  // needn't this
  switch (infoFile.source)
  {
    case TFT_SD:
      return mountSDCard();

    case TFT_USB_DISK:
      return mountUSBDisk();

    case BOARD_MEDIA:
      if (infoHost.printing)
        return true;  // no mount while printing
      else
        return mountGcodeSDCard();

    default:
      return false;
  }
}

// clear and free memory from file list
void clearInfoFile(void)
{
  uint8_t i = 0;

  for (i = 0; i < infoFile.folderCount; i++)
  {
    free(infoFile.folder[i]);
    infoFile.folder[i] = NULL;

    if (infoFile.longFolder[i] != NULL)  // long folder name is optional so we need to check its presence
      free(infoFile.longFolder[i]);
    infoFile.longFolder[i] = NULL;
  }

  for (i = 0; i < infoFile.fileCount; i++)
  {
    free(infoFile.file[i]);
    infoFile.file[i] = NULL;

    if (infoFile.longFile[i] != NULL)  // long filename is optional so we need to check its presence
      free(infoFile.longFile[i]);
    infoFile.longFile[i] = NULL;
  }

  infoFile.folderCount = 0;
  infoFile.fileCount = 0;
}

TCHAR * getCurFileSource(void)
{
  switch (infoFile.source)
  {
    case TFT_SD:
      return SD_ROOT_DIR;

    case TFT_USB_DISK:
      return USBDISK_ROOT_DIR;

    case BOARD_MEDIA:
    case BOARD_MEDIA_REMOTE:
      return infoMachineSettings.firmwareType == FW_REPRAPFW ? "gcodes" : "bMD:";

    default:
      break;
  }
  return NULL;
}

// reset file list
void resetInfoFile(void)
{
  FS_SOURCE source = infoFile.source;
  ONBOARD_SOURCE onboardSource = infoFile.boardSource;
  clearInfoFile();
  memset(&infoFile, 0, sizeof(infoFile));
  infoFile.source = source;
  infoFile.boardSource =  onboardSource;
  strcpy(infoFile.title, getCurFileSource());
}

// scan files in source
bool scanPrintFiles(void)
{
  clearInfoFile();
  switch (infoFile.source)
  {
    case TFT_SD:
    case TFT_USB_DISK:
      return scanPrintFilesFatFs();

    case BOARD_MEDIA:
      return scanPrintFilesGcodeFs();

    default:
      return false;
  }
}

// check and open folder
bool EnterDir(char * nextdir)
{
  if (strlen(infoFile.title) + strlen(nextdir) + 2 >= MAX_PATH_LEN)
    return 0;
  strcat(infoFile.title, "/");
  strcat(infoFile.title, nextdir);
  return 1;
}

// close folder
void ExitDir(void)
{
  int i = strlen(infoFile.title);
  for (; i > 0 && infoFile.title[i] != '/'; i--)
  {
  }
  infoFile.title[i] = 0;
}

// check if current folder is root
bool IsRootDir(void)
{
  return !strchr(infoFile.title, '/');
}

// check if filename provides a supported filename extension
char * isSupportedFile(char * filename)
{
  char * extPos = strrchr(filename, '.');  // check last "." in the name where extension is supposed to start

  if (extPos != NULL && extPos[1] != 'g' && extPos[1] != 'G')
    extPos = NULL;

  return extPos;
}

char * getFoldername(uint8_t index)
{
  if (infoFile.longFolder[index] != NULL)
    return infoFile.longFolder[index];
  else
    return infoFile.folder[index];
}

char * getFilename(uint8_t index)
{
  if (infoFile.longFile[index] != NULL)
    return infoFile.longFile[index];
  else
    return infoFile.file[index];
}

char * hideExtension(char * filename)
{
  if (infoSettings.filename_extension == 0)  // if filename extension is disabled
  {
    char * extPos = isSupportedFile(filename);

    // if filename provides a supported filename extension then
    // check extra byte for filename extension check. If 0, no filename extension was previously hidden
    if (extPos != NULL && filename[strlen(filename) + 1] == 0)
      filename[extPos - filename] = 0;  // temporary hide filename extension
  }

  return filename;
}

char * restoreExtension(char * filename)
{
  if (infoSettings.filename_extension == 0)  // if filename extension is disabled
  {
    // check extra byte for filename extension check. If 0, no filename extension was previously hidden
    if (filename[strlen(filename) + 1] != 0)
      filename[strlen(filename)] = '.';  // restore filename extension
  }

  return filename;
}

char * hideFilenameExtension(uint8_t index)
{
  char * filename = hideExtension(infoFile.file[index]);

  if (infoFile.longFile[index] != NULL)
    filename = hideExtension(infoFile.longFile[index]);

  return filename;
}

char * restoreFilenameExtension(uint8_t index)
{
  char * filename = restoreExtension(infoFile.file[index]);

  if (infoFile.longFile[index] != NULL)
    filename = restoreExtension(infoFile.longFile[index]);

  return filename;
}

// Volume exist detect
static bool volumeSrcStatus[FF_VOLUMES] = {false, false};

bool volumeExists(uint8_t src)
{
  if (src >= FF_VOLUMES)
    return true;

  return volumeSrcStatus[src];
}

uint8_t (*volumeInserted[FF_VOLUMES])(void) = {SD_CD_Inserted, USBH_USR_Inserted};

void loopVolumeSource(void)
{
  for (uint8_t i = 0; i < FF_VOLUMES; i++)
  {
    if (volumeSrcStatus[i] != (*volumeInserted[i])())
    {
      const int16_t labelSDStates[FF_VOLUMES][2] = {{LABEL_TFTSD_REMOVED, LABEL_TFTSD_INSERTED},
                                                    {LABEL_USB_DISK_REMOVED, LABEL_USB_DISK_INSERTED}};
      volumeSrcStatus[i] = (*volumeInserted[i])();
      volumeReminderMessage(labelSDStates[i][volumeSrcStatus[i]], STATUS_NORMAL);
    }
  }
}
