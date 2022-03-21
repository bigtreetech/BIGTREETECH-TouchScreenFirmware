#include "vfs.h"
#include "includes.h"

MYFILE infoFile = {TFT_SD, BOARD_SD, "?:", {0}, {0}, {0}, {0}, 0, 0, 0, 0, false};

void setPrintModelIcon(bool exist)
{
  infoFile.modelIcon = exist;
}

bool isPrintModelIcon(void)
{
  return infoFile.modelIcon;
}

// mount FS of current source
bool mountFS(void)
{
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

// clear and free memory for file list
void clearInfoFile(void)
{
  uint8_t i = 0;

  for (i = 0; i < infoFile.folderCount; i++)
  {
    free(infoFile.folder[i]);
    infoFile.folder[i] = NULL;

    if (infoFile.longFolder[i] != NULL)  // long folder name is optional so we need to check its presence
    {
      free(infoFile.longFolder[i]);
      infoFile.longFolder[i] = NULL;
    }
  }

  for (i = 0; i < infoFile.fileCount; i++)
  {
    free(infoFile.file[i]);
    infoFile.file[i] = NULL;

    if (infoFile.longFile[i] != NULL)  // long filename is optional so we need to check its presence
    {
      free(infoFile.longFile[i]);
      infoFile.longFile[i] = NULL;
    }
  }

  infoFile.folderCount = 0;
  infoFile.fileCount = 0;
}


// skip path information, if any
char * getPathTail(void)
{
  // examples:
  //
  // "SD:/test/cap2.gcode" -> "cap2.gcode"
  // "SD:cap.gcode" -> "cap.gcode"
  // "Remote printing..." -> ""

  char * strPtr = strrchr(infoFile.title, '/');  // remove path information, if any

  if (strPtr == NULL)  // if "/" not found, it can be a filename on the root folder
  {
    strPtr = strchr(infoFile.title, ':');  // remove source FS information, if any

    if (strPtr == NULL)  // if ":" not found, a remote host is handling a print
      return (infoFile.title + strlen(infoFile.title));  // return an empty string
  }

  return ++strPtr;  // return path after "/" or ":"
}

// check and open folder
bool EnterDir(const char * nextdir)
{
  if (strlen(infoFile.title) + strlen(nextdir) + 2 > MAX_PATH_LEN)  // "+ 2": space for "/" and terminating null character
    return false;

  strcat(infoFile.title, "/");
  strcat(infoFile.title, nextdir);

  return true;
}

// close folder
void ExitDir(void)
{
  int i = strlen(infoFile.title);

  for (; i > 0 && infoFile.title[i] != '/'; i--)
  {
  }

  infoFile.title[i] = '\0';
}

// check if current folder is root
bool IsRootDir(void)
{
  return !strchr(infoFile.title, '/');
}

// check if filename provides a supported filename extension
char * isSupportedFile(const char * filename)
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

// get print filename according to print originator (remote or local to TFT)
char * getPrintFilename(void)
{
  // if restoring a print after a power failure or printing from remote host, remote onboard media or remote TFT media (with M23 - M24),
  // no filename is available in infoFile. Only infoFile.source and infoFile.title have been set
  //
  if (infoFile.fileCount == 0)  // if no filename is available in infoFile
    return getPathTail();       // skip path information

  return getFilename(infoFile.fileIndex);
}

// get print title according to print originator (remote or local to TFT)
bool getPrintTitle(char * buf, uint8_t len)
{
  // example: "SD:/test/cap2.gcode" -> "SD:cap2.gcode"

  char * strPtr = getPrintFilename();

  // "+ 2": space for terminating null character and the flag for filename extension check
  if (strlen(getCurFileSource()) + strlen(strPtr) + 2 > len)
  {
    *buf = '\0';  // set buffer to empty string

    return false;
  }

  strncpy(buf, getCurFileSource(), len);  // set source and set the flag for filename extension check
  strcat(buf, strPtr);                    // append filename
  hideExtension(buf);                     // hide filename extension if filename extension feature is disabled

  return true;
}

// volume exist detect
static bool volumeSrcStatus[FF_VOLUMES] = {false, false};

bool volumeExists(uint8_t src)
{
  if (src >= FF_VOLUMES)
    return true;

  return volumeSrcStatus[src];
}

uint8_t (* volumeInserted[FF_VOLUMES])(void) = {SD_CD_Inserted, USBH_USR_Inserted};

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
