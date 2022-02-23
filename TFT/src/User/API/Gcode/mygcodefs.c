#include "mygcodefs.h"
#include "includes.h"
#include "RRFM20Parser.hpp"

/**
 * Send M21 (Init SD) and wait for the result (false = Error, true = OK)
 *
 * SENDING: M21
 * echo:SD card ok
 * echo:No SD card
 */
bool mountGcodeSDCard(void)
{
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
    return true;

  return request_M21();
}

static inline void rrfScanPrintFilesGcodeFs(void)
{
  // TODO detect files_sort_by and set with_ts appropriately once M20 S3 works
  request_M20_rrf(infoFile.title, false, parseJobListResponse);
}

/**
 * When AUTO_REPORT_TEMPERATURES is enabled by M155, The response of M33 may become the following:
 *
 * SENDING: M33 /1A29A~1.GCO
 * T:29.43 /0.00 B:27.95 /0.00 @:0 B@:0
 * /1.gcode
 * ok
 *
 * So the long name will be parsed "0.00 @:0 B@:0" instead of "1.gcode" if the truncated character is "\n" not string "\nok"
 */
void getName(bool filename, const char * absolutePath, const char * relativePath)
{
  // "+ 2": space for terminating null character and the flag for filename extension check
  // "+ 1": space for terminating null character
  uint8_t nameLenExtra = filename ? 2 : 1;
  uint8_t nameLen;
  char * name;
  char * namePtr;
  char * longName = NULL;  // initialize to NULL in case long filename is not supported or no long name exists
  char * shortName = NULL;

  //
  // get long name, if any
  //

  if (infoMachineSettings.longFilename == ENABLED)  // if long filename is supported
  {
    name = request_M33(absolutePath);  // retrieve long name, if any

    namePtr = strstr(name, "\nok");  // remove end of M33 command, if any
    if (namePtr != NULL)
      *namePtr = '\0';

    namePtr = strrchr(name, '/');  // remove path information, if any
    if (namePtr != NULL)
      name = namePtr + 1;  // ditch trailing "/"

    if (strcmp(name, "???") != 0)  // if long name exists
    {
      nameLen = strlen(name) + nameLenExtra;
      longName = malloc(nameLen);
      if (longName == NULL)  // in case of error, free the buffer allocated by M33 (including "name") and exit
      {
        clearRequestCommandInfo();
        return;
      }

      strncpy(longName, name, nameLen);  // set "longName" and set the flag for filename extension check, if any
    }

    clearRequestCommandInfo();  // finally, free the buffer allocated by M33 (including "name")
  }

  //
  // get short name
  //

  nameLen = strlen(relativePath) + nameLenExtra;
  shortName = malloc(nameLen);
  if (shortName == NULL)  // in case of error, free the buffer allocated for "longName", if any, and exit
  {
    if (longName != NULL)
      free(longName);

    return;
  }

  strncpy(shortName, relativePath, nameLen);  // set "shortName" and set the flag for filename extension check, if any

  //
  // copy to destination
  //

  if (filename)
  {
    infoFile.longFile[infoFile.fileCount] = longName;
    infoFile.file[infoFile.fileCount] = shortName;
    infoFile.fileCount++;
  }
  else
  {
    infoFile.longFolder[infoFile.folderCount] = longName;
    infoFile.folder[infoFile.folderCount] = shortName;
    infoFile.folderCount++;
  }
}

/**
 * SENDING: M20
 * Begin file list
 * PI3MK2~1.GCO 11081207
 * /YEST~1/TEST2/PI3MK2~1.GCO 11081207
 * /YEST~1/TEST2/PI3MK2~3.GCO 11081207
 * /YEST~1/TEST2/PI3MK2~2.GCO 11081207
 * /YEST~1/TEST2/PI3MK2~4.GCO 11081207
 * /YEST~1/TEST2/PI3MK2~5.GCO 11081207
 * /YEST~1/PI3MK2~1.GCO 11081207
 * /YEST~1/PI3MK2~3.GCO 11081207
 * /YEST~1/PI3MK2~2.GCO 11081207
 * End file list
 */
bool scanPrintFilesGcodeFs(void)
{
  clearInfoFile();

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    rrfScanPrintFilesGcodeFs();
    return true;
  }

  char * ret = request_M20();             // retrieve file list
  char * data = malloc(strlen(ret) + 1);
  strcpy(data, ret);                      // copy file list in "data"
  clearRequestCommandInfo();              // free the buffer allocated by M20 (including "ret")

  char * s = strstr(data, "\r\n") ? "\r\n" : "\n";  // smoothieware reports with "\r\n", marlin reports with "\n"
  char * line = strtok(data, s);
  char * linePtr;
  uint8_t strLen;

  for (; line != NULL; line = strtok(NULL, s))
  {
    if (strlen(line) == 0 || strcmp(line, "ok") == 0 ||
        strcmp(line, "Begin file list") == 0 || strcmp(line, "End file list") == 0 )  // start and stop tag
      continue;

    linePtr = strrchr(line, ' ');  // remove file size at the end of line, if any
    if (linePtr != NULL)
      *linePtr = '\0';

    // old Marlin fw provides "/" at the beginning while latest Marlin fw doesn't, so skip it if present (use a common file path)
    // (e.g. "/sub_dir/cap.gcode" -> "sub_dir/cap.gcode")
    //
    if (line[0] == '/')
      line++;

    // "line" never has "/" at the beginning of a path (e.g. "sub_dir/cap.gcode") while "infoFile.title" has it
    // (e.g. "bSD:/sub_dir"), so we skip it during the check of current folder match (index 5 used instead of 4)
    //
    strLen = strlen(infoFile.title);
    if (strLen > 5)                                // we're in a subfolder
    {
      linePtr = strstr(line, infoFile.title + 5);  // "+ 5" skips the 5 bytes related to prefix "bSD:/" in "infoFile.title"
      if (linePtr == NULL)                         // if "line" doesn't include current folder
        continue;
      else if (linePtr[strLen - 5] != '/')         // "- 5" skips the prefix "bSD:/" in "infoFile.title"
        continue;                                  // because it's a file, not a folder
    }

    // examples:
    //
    // "infoFile.title" = "bSD:/sub_dir"
    // "line" = "sub_dir/cap.gcode"
    // "line" = "sub_dir/sub_dir_2/cap2.gcode"
    // "line" = "sub_dir/sub_dir_2/sub_dir_3/cap3.gcode"
    //
    // "relativePath" = "cap.gcode"
    // "relativePath" = "sub_dir_2/cap2.gcode"
    // "relativePath" = "sub_dir_2/sub_dir_3/cap3.gcode"
    //
    char * relativePath = line + (strLen - 4);  // "- 4" represents the 4 bytes related to prefix "bSD:" in "infoFile.title"

    if (strchr(relativePath, '/') == NULL)  // if FILE
    {
      // examples:
      //
      // "infoFile.title" = "bSD:/sub_dir"
      // "relativePath" = "cap.gcode"

      if (infoFile.fileCount >= FILE_NUM)  // Gcode max number is FILE_NUM
        continue;

      getName(true, line, relativePath);
    }
    else  // if FOLDER
    {
      // examples:
      //
      // "infoFile.title" = "bSD:/sub_dir"
      // "relativePath" = "sub_dir_2/cap2.gcode"
      // "relativePath" = "sub_dir_2/sub_dir_3/cap3.gcode"

      if (infoFile.folderCount >= FOLDER_NUM)  // folder max number is FOLDER_NUM
        continue;

      // "sub_dir_2/cap2.gcode" -> "sub_dir_2"
      // "sub_dir_2/sub_dir_3/cap3.gcode" -> "sub_dir_2"
      //
      linePtr = strstr(relativePath, "/");  // remove file and sub folders path (retrieve only root folder), if any
      if (linePtr != NULL)
        *linePtr = '\0';

      bool found = false;

      for (int i = 0; i < infoFile.folderCount; i++)
      {
        if (strcmp(relativePath, infoFile.folder[i]) == 0)
        {
          found = true;
          break;
        }
      }

      if (!found)
        getName(false, line, relativePath);
    }
  }

  free(data);
  return true;
}
