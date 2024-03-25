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
  request_M20_rrf(infoFile.path, false, parseJobListResponse);
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
static void addName(bool isFile, char * longPath, const char * shortPath, const char * relativePath)
{
  char * longName = NULL;  // initialize to NULL in case long filename is not supported or no long name exists

  //
  // get long name, if any
  //

  if (infoMachineSettings.longFilename == ENABLED)  // if long filename is supported
  {
    char * strPtr;
    char * name;

    // if filename (not folder name) and long path is already available (e.g. "M20 L" supported by Marlin)
    //
    // NOTE: Folder names are currently not properly supported by Marlin, so we use M33 for them
    //
    if (isFile && longPath != NULL)
      name = longPath;
    else
      name = request_M33(shortPath);  // retrieve long name, if any

    strPtr = strstr(name, "\nok");  // remove end of M33 command, if any
    if (strPtr != NULL)
      *strPtr = '\0';

    strPtr = strrchr(name, '/');  // remove path information, if any
    if (strPtr != NULL)
      name = strPtr + 1;  // ditch trailing "/"

    if (strcmp(name, "???") != 0)  // if long name exists
      longName = name;
  }

  //
  // add short name and long name, if any
  //

  addFile(isFile, relativePath, longName);

  clearRequestCommandInfo();  // finally, free the buffer allocated by M33 (including "name"), if any
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
 *
 * SENDING: M20 L
 * Begin file list
 * GCODE/BLTOUC~1.GCO 257355 /BLTouch Calibration.gcode
 * GCODE/BLTOUC~2.GCO 975584 /BLTouch Mount Adjustable.gcode
 * GCODE/TEST/BTT_SM~1.GCO 2028666 //BTT_SmartFilamentSensor_Backplate_Angled.gcode
 * GCODE/TEST/BLTOUC~1.GCO 941290 //BLTouch Mount Slider.gcode
 * GCODE/TESTSM~1.GCO 4525869 /TestsM420.gcode
 * XYZCAL~1.GCO 289600 xyzCalibration_cube.gcode
 * CAP~1.GCO 1800716 Cap.gcode
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

  char * s = strstr(data, "\r\n") ? "\r\n" : "\n";  // Smoothieware reports with "\r\n", Marlin reports with "\n"
  char * line = strtok(data, s);
  char * longPath;
  char * relativePath;
  char * strPtr;
  uint8_t strLen;
  uint8_t sourceLenExtra = strlen(getFS()) + 1;  // "+ 1" for "/" character (e.g. "oMD:/sub_dir" -> "oMD:/")

  for (; line != NULL; line = strtok(NULL, s))
  {
    if (strlen(line) == 0 || strcmp(line, "ok") == 0 ||
        strcmp(line, "Begin file list") == 0 || strcmp(line, "End file list") == 0 )  // start and stop tag
      continue;

    longPath = NULL;  // initialize to NULL in case long path is not available (e.g. "M20 L" not supported by Marlin)

    strPtr = strchr(line, ' ');  // get short path removing the file size, if any
    if (strPtr != NULL)
    {
      *strPtr = '\0';

      strPtr = strchr(strPtr + 1, ' ');  // get long path jumping after the file size, if any (e.g. "M20 L" supported by Marlin)
      if (strPtr != NULL)
        longPath = strPtr + 1;
    }

    // old Marlin fw provides "/" at the beginning while latest Marlin fw doesn't, so skip it if present (use a common file path)
    // (e.g. "/sub_dir/cap.gcode" -> "sub_dir/cap.gcode")
    //
    if (line[0] == '/')
      line++;

    relativePath = line;  // initialize relative path to "line"

    // "line" never has "/" at the beginning of a path (e.g. "sub_dir/cap.gcode") while "infoFile.path" has it
    // (e.g. "oMD:/sub_dir"), so we skip it during the check of current folder match
    //
    strLen = strlen(infoFile.path);
    if (strLen > sourceLenExtra)                              // we're in a sub folder (e.g. "infoFile.path" = "oMD:/sub_dir")
    {
      strPtr = strstr(line, infoFile.path + sourceLenExtra);  // (e.g. "infoFile.path" = "oMD:/sub_dir" -> "sub_dir")

      if (strPtr == NULL || strPtr != line)             // if "line" doesn't include current folder or doesn't fully match from beginning
        continue;                                       // (e.g. "line" = "before_sub_dir/cap.gcode" -> "sub_dir/cap.gcode")
      else if (strPtr[strLen - sourceLenExtra] != '/')  // if "line" is a file or doesn't fully match to end
        continue;                                       // (e.g. "line" = "sub_dir_after/cap.gcode" -> "_after/cap.gcode")

      // update relative path skipping current folder and next "/" character
      // (e.g. "relativePath" = "sub_dir/cap.gcode" -> "cap.gcode")
      relativePath += (strLen - sourceLenExtra) + 1;
    }

    // examples:
    //
    // "infoFile.path" = "oMD:"
    // "line" = "arm.gcode"
    // "line" = "sub_dir/cap.gcode"
    //
    // "relativePath" = "arm.gcode"
    // "relativePath" = "sub_dir/cap.gcode"
    //
    // examples:
    //
    // "infoFile.path" = "oMD:/sub_dir"
    // "line" = "sub_dir/cap.gcode"
    // "line" = "sub_dir/sub_dir_2/cap2.gcode"
    // "line" = "sub_dir/sub_dir_2/sub_dir_3/cap3.gcode"
    //
    // "relativePath" = "cap.gcode"
    // "relativePath" = "sub_dir_2/cap2.gcode"
    // "relativePath" = "sub_dir_2/sub_dir_3/cap3.gcode"

    if (strchr(relativePath, '/') == NULL)  // if FILE
    {
      // examples:
      //
      // "infoFile.path" = "oMD:"
      // "relativePath" = "arm.gcode"
      //
      // examples:
      //
      // "infoFile.path" = "oMD:/sub_dir"
      // "relativePath" = "cap.gcode"

      if (infoFile.fileCount >= FILE_NUM)  // gcode file max number is FILE_NUM
        continue;

      addName(true, longPath, line, relativePath);
    }
    else  // if FOLDER
    {
      // examples:
      //
      // "infoFile.path" = "oMD:"
      // "relativePath" = "sub_dir/cap.gcode"
      //
      // examples:
      //
      // "infoFile.path" = "oMD:/sub_dir"
      // "relativePath" = "sub_dir_2/cap2.gcode"
      // "relativePath" = "sub_dir_2/sub_dir_3/cap3.gcode"

      if (infoFile.folderCount >= FOLDER_NUM)  // folder max number is FOLDER_NUM
        continue;

      // "sub_dir/cap.gcode" -> "sub_dir"
      //
      // "sub_dir_2/cap2.gcode" -> "sub_dir_2"
      // "sub_dir_2/sub_dir_3/cap3.gcode" -> "sub_dir_2"
      //
      strPtr = strchr(relativePath, '/');  // remove file and sub folders path (retrieve only root folder), if any
      if (strPtr != NULL)
        *strPtr = '\0';

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
        addName(false, longPath, line, relativePath);
    }
  }

  free(data);
  return true;
}
