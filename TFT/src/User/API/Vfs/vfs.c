#include "vfs.h"
#include "includes.h"

MYFILE infoFile = {FS_TFT_SD, BOARD_SD, "?:", {0}, {0}, {0}, {0}, 0, 0, 0, 0, false};

void setPrintModelIcon(bool exist)
{
  infoFile.modelIcon = exist;
}

bool isPrintModelIcon(void)
{
  return infoFile.modelIcon;
}

// get FS's ID of current source
TCHAR * getFS(void)
{
  switch (infoFile.source)
  {
    case FS_TFT_SD:
      return SD_ROOT_DIR;

    case FS_TFT_USB:
      return USB_ROOT_DIR;

    case FS_ONBOARD_MEDIA:
    case FS_ONBOARD_MEDIA_REMOTE:
      return infoMachineSettings.firmwareType == FW_REPRAPFW ? "gcodes" : "oMD:";

    case FS_REMOTE_HOST:
      return "Remote printing...";

    default:
      return "";
  }
}

// mount FS of current source
bool mountFS(void)
{
  switch (infoFile.source)
  {
    case FS_TFT_SD:
      return mountSDCard();

    case FS_TFT_USB:
      return mountUSBDisk();

    case FS_ONBOARD_MEDIA:
      if (isPrintingFromOnboard())
        return true;  // no mount while printing
      else
        return mountGcodeSDCard();

    default:
      return false;
  }
}

// scan files in current source and create a file list
bool scanPrintFiles(void)
{
  switch (infoFile.source)
  {
    case FS_TFT_SD:
    case FS_TFT_USB:
      return scanPrintFilesFatFs();

    case FS_ONBOARD_MEDIA:
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

// clear file list and path
void resetInfoFile(void)
{
  FILE_SOURCE source = infoFile.source;
  ONBOARD_SOURCE onboardSource = infoFile.onboardSource;

  clearInfoFile();
  memset(&infoFile, 0, sizeof(infoFile));

  infoFile.source = source;
  infoFile.onboardSource = onboardSource;
  strcpy(infoFile.path, getFS());
}

// skip path information, if any
char * getPathTail(void)
{
  // examples:
  //
  // "SD:/test/cap2.gcode" -> "cap2.gcode"
  // "SD:cap.gcode" -> "cap.gcode"
  // "Remote printing..." -> ""

  char * strPtr = strrchr(infoFile.path, '/');  // remove path information, if any

  if (strPtr == NULL)  // if "/" not found, it can be a filename on the root folder
  {
    strPtr = strchr(infoFile.path, ':');  // remove source FS information, if any

    if (strPtr == NULL)  // if ":" not found, a remote host is handling a print
      return (infoFile.path + strlen(infoFile.path));  // return an empty string
  }

  return ++strPtr;  // return path after "/" or ":"
}

// check and open folder
bool enterFolder(const char * path)
{
  if (strlen(infoFile.path) + strlen(path) + 2 > MAX_PATH_LEN)  // "+ 2": space for "/" and terminating null character
    return false;

  strcat(infoFile.path, "/");
  strcat(infoFile.path, path);

  return true;
}

// close folder
void exitFolder(void)
{
  int i = strlen(infoFile.path);

  for (; i > 0 && infoFile.path[i] != '/'; i--)
  {
  }

  infoFile.path[i] = '\0';
}

// check if current folder is root
bool isRootFolder(void)
{
  return !strchr(infoFile.path, '/');
}

// check if filename provides a supported filename extension
static char * isSupportedFile(const char * filename)
{
  char * extPos = strrchr(filename, '.');  // check last "." in the name where extension is supposed to start

  return (extPos != NULL && (extPos[1] == 'g' || extPos[1] == 'G')) ? extPos : NULL;
}

// add a file name or folder name to file list
bool addFile(bool isFile, const char * shortName, const char * longName)
{
  if (isFile == true)  // if file
  {
    // if file list is full or filename doesn't provide a supported filename extension
    if (infoFile.fileCount >= FILE_NUM || isSupportedFile(shortName) == NULL)
      return false;
  }
  else  // if folder
  {
    if (infoFile.folderCount >= FOLDER_NUM)  // if folder list is full
      return false;
  }

  // "+ 2": space for terminating null character and the flag for filename extension check
  // "+ 1": space for terminating null character
  uint8_t extraLen = isFile ? 2 : 1;
  char * sName;
  char * lName = NULL;  // initialize to NULL in case long filename is not provided

  //
  // get short name
  //

  uint8_t sNameLen = strlen(shortName) + extraLen;

  sName = malloc(sNameLen);

  if (sName == NULL)  // in case of error, exit
    return false;

  strxcpy(sName, shortName, sNameLen);  // copy to "sName" and set to NULL the flag for filename extension check, if any

  //
  // get long name, if any
  //

  if (longName != NULL)  // if long filename is provided
  {
    uint8_t lNameLen = strlen(longName) + extraLen;

    lName = malloc(lNameLen);

    if (lName == NULL)  // in case of error, free the buffer allocated for "sName" and exit
    {
      if (sName != NULL)
        free(sName);

      return false;
    }

    strxcpy(lName, longName, lNameLen);  // copy to "lName" and set to NULL the flag for filename extension check, if any
  }

  //
  // copy to destination
  //

  if (isFile)
  {
    infoFile.file[infoFile.fileCount] = sName;
    infoFile.longFile[infoFile.fileCount] = lName;
    infoFile.fileCount++;
  }
  else
  {
    infoFile.folder[infoFile.folderCount] = sName;
    infoFile.longFolder[infoFile.folderCount] = lName;
    infoFile.folderCount++;
  }

  return true;
}

// return the long folder name if exists, otherwise the short one
char * getFoldername(uint8_t index)
{
  if (infoFile.longFolder[index] != NULL)
    return infoFile.longFolder[index];
  else
    return infoFile.folder[index];
}

// return the long file name if exists, otherwise the short one
char * getFilename(uint16_t index)
{
  if (infoFile.longFile[index] != NULL)
    return infoFile.longFile[index];
  else
    return infoFile.file[index];
}

char * hideExtension(char * filename)
{
  if (infoSettings.filename_extension == 0 && strchr(filename, '\0')[1] == '\0')  // if showing the filename extension is disabled and extension not already hidden
  {
    char * extPos = isSupportedFile(filename);

    // if filename provides a supported filename extension then
    if (extPos != NULL)
      *extPos = '\0';  // temporary hide filename extension
  }

  return filename;
}

char * restoreExtension(char * filename)
{
  if (infoSettings.filename_extension == 0)  // if showing the filename extension is disabled
  {
    char * extPos = strchr(filename, '\0');

    // check extra byte for filename extension check. If 0, no filename extension was previously hidden
    if (extPos[1] != '\0')
      *extPos = '.';  // restore filename extension
  }

  return filename;
}

// get print filename according to print originator (remote or local to TFT)
char * getPrintFilename(void)
{
  // if restoring a print after a power failure or printing from remote host, remote onboard media or remote TFT media (with M23 - M24),
  // no filename is available in infoFile. Only infoFile.source and infoFile.path have been set
  //
  if (infoFile.fileCount == 0)  // if no filename is available in infoFile
    return getPathTail();       // skip path information

  return getFilename(infoFile.fileIndex);
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
      const int16_t labelSDStates[FF_VOLUMES][2] = {{LABEL_TFT_SD_REMOVED, LABEL_TFT_SD_INSERTED},
                                                    {LABEL_TFT_USB_REMOVED, LABEL_TFT_USB_INSERTED}};

      volumeSrcStatus[i] = (*volumeInserted[i])();
      setReminderMsg(labelSDStates[i][volumeSrcStatus[i]], SYS_STATUS_VOL_CHANGE);
    }
  }
}
