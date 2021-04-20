#include "vfs.h"
#include "includes.h"

MYFILE infoFile = {"?:", {0}, {0}, 0, 0, 0, 0, TFT_SD, {0}};

void setPrintModelIcon(bool exist)
{
  infoFile.model_icon = exist;
}

bool isPrintModelIcon(void)
{
  return infoFile.model_icon;
}

bool mountFS(void)
{
  //  resetInfoFile();  //needn't this
  switch (infoFile.source)
  {
    case TFT_SD:
      return mountSDCard();

    case TFT_UDISK:
      return mountUDisk();

    case BOARD_SD:
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
    infoFile.folder[i] = 0;
  }
  for (i = 0; i < infoFile.fileCount; i++)
  {
    free(infoFile.file[i]);
    infoFile.file[i] = 0;
    free(infoFile.Longfile[i]);
    infoFile.Longfile[i] = 0;
  }
  infoFile.folderCount = 0;
  infoFile.fileCount = 0;
}

TCHAR *getCurFileSource(void)
{
  switch (infoFile.source)
  {
    case TFT_SD:
      return "SD:";

    case TFT_UDISK:
      return "U:";

    case BOARD_SD:
    case BOARD_SD_REMOTE:
      return "bSD:";

    default:
      break;
  }
  return NULL;
}

// reset file list
void resetInfoFile(void)
{
  FS_SOURCE source = infoFile.source;
  clearInfoFile();
  memset(&infoFile, 0, sizeof(infoFile));
  infoFile.source = source;
  strcpy(infoFile.title, getCurFileSource());
}

// scan files in source
bool scanPrintFiles(void)
{
  clearInfoFile();
  switch (infoFile.source)
  {
    case TFT_SD:
    case TFT_UDISK:
      return scanPrintFilesFatFs();

    case BOARD_SD:
      return scanPrintFilesGcodeFs();
    default:
      return false;
  }
}

// check and open folder
bool EnterDir(char *nextdir)
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

// Volume exist detect
static bool volumeSrcStatus[FF_VOLUMES] = {false, false};

bool isVolumeExist(uint8_t src)
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
                                                    {LABEL_U_DISK_REMOVED, LABEL_U_DISK_INSERTED}};
      volumeSrcStatus[i] = (*volumeInserted[i])();
      volumeReminderMessage(labelSDStates[i][volumeSrcStatus[i]], STATUS_NORMAL);
    }
  }
}
