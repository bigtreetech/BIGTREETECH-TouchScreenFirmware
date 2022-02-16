#include "mygcodefs.h"
#include "includes.h"
#include "RRFM20Parser.hpp"

bool mountGcodeSDCard(void)
{
  // Send M21 ( Init SD )
  // wait result
  // return 0=Error 1=OK
  //
  // >>> M21
  // SENDING:M21
  // echo:SD card ok
  //
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    return true;
  }
  return request_M21();
}

static inline void rrfScanPrintFilesGcodeFs(void)
{
  // TODO detect files_sort_by and set with_ts appropriately once M20 S3 works
  request_M20_rrf(infoFile.title, false, parseJobListResponse);
}

//static uint32_t date = 0;
//static FILINFO finfo;
//static uint16_t len = 0;
//
// SENDING:M20
// Begin file list
// PI3MK2~1.GCO 11081207
// /YEST~1/TEST2/PI3MK2~1.GCO 11081207
// /YEST~1/TEST2/PI3MK2~3.GCO 11081207
// /YEST~1/TEST2/PI3MK2~2.GCO 11081207
// /YEST~1/TEST2/PI3MK2~4.GCO 11081207
// /YEST~1/TEST2/PI3MK2~5.GCO 11081207
// /YEST~1/PI3MK2~1.GCO 11081207
// /YEST~1/PI3MK2~3.GCO 11081207
// /YEST~1/PI3MK2~2.GCO 11081207
// End file list
//
bool scanPrintFilesGcodeFs(void)
{
  clearInfoFile();

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    rrfScanPrintFilesGcodeFs();
    return true;
  }

  uint8_t str_len;
  char* ret = request_M20();
  char* data = malloc(strlen(ret) + 1);
  strcpy(data, ret);
  clearRequestCommandInfo();

  char* s = strstr(data, "\r\n") ? "\r\n" : "\n";  // smoothieware report with \r\n, marlin reports with \n
  char* line = strtok(data, s);
  char* Pstr_tmp;

  for (; line != NULL; line = strtok(NULL, s))
  {
    if (strlen(line) == 0 || strcmp(line, "ok") == 0 ||
        strcmp(line, "Begin file list") == 0 || strcmp(line, "End file list") == 0 )  // Start and Stop tag
      continue;

    if (line[0] == '/')  // old Marlin fw provides "/" at the beginning while latest Marlin fw doesn't, so skip it if present
      line++;            // (use a common file path) e.g. "/sub_dir/cap.gcode" -> "sub_dir/cap.gcode"

    str_len = strlen(infoFile.title);
    // "line" never has "/" at the beginning of a path (e.g. "sub_dir/cap.gcode") while "infoFile.title" has it
    // (e.g. "bSD:/sub_dir"), so we skip it during the check of current folder match (index 5 used instead of 4)
    if (str_len > 5)  // we're in a subfolder
    {
      Pstr_tmp = strstr(line, infoFile.title + 5);  // "+ 5" skips the 5 bytes related to prefix "bSD:/" in infoFile.title
      if (Pstr_tmp == NULL)  // if "line" doesn't include current folder
        continue;
      else if (Pstr_tmp[str_len - 5] != '/')  // "- 5" skips the prefix "bSD:/" in infoFile.title
        continue;  // because it's a file, not a folder
    }

    Pstr_tmp = strrchr(line, ' ');  // check and remove file size at the end of line
    if (Pstr_tmp != NULL)
      *Pstr_tmp = '\0';

    // e.g. "sub_dir/cap.gcode" -> "cap.gcode", "sub_dir/sub_dir_2/cap2.gcode" -> "sub_dir_2/cap.gcode"
    char* pline = line + (str_len - 4);  // "- 4" represents the 4 bytes related to prefix "bSD:" in infoFile.title

    if (strchr(pline, '/') == NULL)  // if FILE
    {
      if (infoFile.fileCount >= FILE_NUM)
        continue;  // Gcode max number is FILE_NUM

      if (infoMachineSettings.longFilename == ENABLED)
      { // get long filename
        char *longFilename = request_M33(line);  // retrieve long filename (if exists)

        /*
          When AUTO_REPORT_TEMPERATURES is enabled by M155, The response of M33 may become the following
            SENDING: M33 /1A29A~1.GCO
            T:29.43 /0.00 B:27.95 /0.00 @:0 B@:0
            /1.gcode
            ok
          So the longfilename will be parsed "0.00 @:0 B@:0" instead of "1.gcode" if the truncated character is '\n' not string "\nok"
        */
 
        Pstr_tmp = strstr(longFilename, "\nok");
        if (Pstr_tmp != NULL)
          *Pstr_tmp = '\0';  // remove end of M33 command

        Pstr_tmp = strrchr(longFilename, '/');  // remove folder information
        if (Pstr_tmp != NULL)
          longFilename = Pstr_tmp + 1;  // ditch the trailing "/"

        if (strcmp(longFilename, "???") == 0)  // no long filename exist
          longFilename = pline;

        str_len = strlen(longFilename);
        infoFile.longFile[infoFile.fileCount] = malloc(str_len + 2);  // "+ 2": space for terminating null character and the flag for filename extension check
        if (infoFile.longFile[infoFile.fileCount] == NULL)
        {
          clearRequestCommandInfo();
          break;
        }
        // copy file name and set the flag for filename extension check
        strncpy(infoFile.longFile[infoFile.fileCount], longFilename, str_len + 2);  // "+ 2": space for terminating null character and the flag for filename extension check
        clearRequestCommandInfo();  // finally free the buffer allocated by M33, if any
      }
      else  // if long filename is not supported
      {
        infoFile.longFile[infoFile.fileCount] = NULL;  // long filename is not supported, so always set it to NULL
      }

      // NOTE: no need to filter files based on filename extension because files are already filtered by Marlin,
      // so leave the following block commented
/*      if (IsSupportedFile(file) == NULL)  // if filename doesn't provide a supported filename extension
      {
        if (infoFile.Longfile[infoFile.fileCount] != NULL)
          free(infoFile.Longfile[infoFile.fileCount]);

        continue;
      }*/

      // get short filename

      str_len = strlen(pline);
      infoFile.file[infoFile.fileCount] = malloc(str_len + 2);  // "+ 2": space for terminating null character and the flag for filename extension check
      if (infoFile.file[infoFile.fileCount] == NULL)
      {
        if (infoFile.longFile[infoFile.fileCount] != NULL)
          free(infoFile.longFile[infoFile.fileCount]);

        break;
      }
      // copy file name and set the flag for filename extension check
      strncpy(infoFile.file[infoFile.fileCount], pline, str_len + 2);  // "+ 2": space for terminating null character and the flag for filename extension check
      infoFile.fileCount++;
    }
    else  // if FOLDER
    {
      if (infoFile.folderCount >= FOLDER_NUM)
        continue;  // folder max number is FOLDER_NUM

      bool found = false;
      char* rest = pline;
      char* folder = strtok_r(rest, "/", &rest);
      char* longFolder = NULL;

      str_len = strlen(folder);

      if (infoMachineSettings.longFilename == ENABLED)
      {
        Pstr_tmp = malloc(pline - line + str_len + 1);

        Pstr_tmp[0] = '\0';
        strncat(Pstr_tmp, line, pline - line + str_len);  // construct full path to folder
        longFolder = request_M33(Pstr_tmp);  // retrieve long folder names (if exist)
        free(Pstr_tmp);

        Pstr_tmp = strstr(longFolder, "\nok");
        if (Pstr_tmp != NULL)
          *Pstr_tmp = '\0';  // remove end of M33 command

        Pstr_tmp = strrchr(longFolder, '/');  // retrieve last folder from full path
        if (Pstr_tmp != NULL)
          longFolder = Pstr_tmp + 1;  // ditch the trailing "/"

        if (strcmp(longFolder, "???") == 0)  // no long folder name exist
          longFolder = NULL;
      }

      for (int i = 0; i < infoFile.folderCount; i++)
      {
        if (strcmp(folder, infoFile.folder[i]) == 0)
        {
          found = true;
          break;
        }
      }

      if (!found)
      {
        infoFile.folder[infoFile.folderCount] = malloc(str_len + 1);

        if (infoFile.folder[infoFile.folderCount] == NULL)
          break;
          
        strcpy(infoFile.folder[infoFile.folderCount], folder);

        if (longFolder != NULL)
        {
          infoFile.longFolder[infoFile.folderCount] = malloc(strlen(longFolder) + 1);
          if (infoFile.longFolder[infoFile.folderCount] != NULL)
            strcpy(infoFile.longFolder[infoFile.folderCount], longFolder);
        }
        else
          infoFile.longFolder[infoFile.folderCount] = NULL;

        infoFile.folderCount++;
      }
    }
  }
  free(data);
  return true;
}
