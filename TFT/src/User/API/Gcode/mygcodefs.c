#include "mygcodefs.h"
#include "includes.h"
#include "RRFM20Parser.hpp"

bool mountGcodeSDCard(void)
{
  // Send M21 ( Init SD )
  // wait result
  // return 0=Error 1=OK
  /*
>>> M21
SENDING:M21
echo:SD card ok
*/
  return request_M21();
}

static inline void rrfScanPrintFilesGcodeFs(void)
{
  // TODO enable the optimization for fetching M20 S2 once the problem with M20 S3 is fixed
  // switch (infoSettings.files_sort_by)
  // {
    // case SORT_NAME_ASCENDING:
    // case SORT_NAME_DESCENDING:
      // parseJobListResponse(request_M20_rrf(infoFile.title));
      // clearRequestCommandInfo();
      // break;
    // case SORT_DATE_NEW_FIRST:
    // case SORT_DATE_OLD_FIRST:
      // properly getting timestamps requires M20 S3, which has a larger payload
      // ~50 files would exceed 5K easily with M20 S3, ~100 files would exceed 5K with M20 S2
  request_M20_rrf_streaming(infoFile.title, parseJobListResponseStreaming);
      // break;
  // }
}

//static uint32_t date = 0;
//static FILINFO finfo;
//static uint16_t len = 0;
/*
SENDING:M20
Begin file list
PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~3.GCO 11081207
/YEST~1/TEST2/PI3MK2~2.GCO 11081207
/YEST~1/TEST2/PI3MK2~4.GCO 11081207
/YEST~1/TEST2/PI3MK2~5.GCO 11081207
/YEST~1/PI3MK2~1.GCO 11081207
/YEST~1/PI3MK2~3.GCO 11081207
/YEST~1/PI3MK2~2.GCO 11081207
End file list
*/
bool scanPrintFilesGcodeFs(void)
{
  clearInfoFile();

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    rrfScanPrintFilesGcodeFs();
    return true;
  }
  char *ret = request_M20();
  char *data = malloc(strlen(ret) + 1);
  strcpy(data, ret);
  clearRequestCommandInfo();

  char * s = strstr(data, "\r\n") ? "\r\n" : "\n";  // smoothieware report with \r\n, marlin reports with \n
  char *line = strtok(data, s);

  for (; line != NULL; line = strtok(NULL, s))
  {
    if (strcmp(line, "Begin file list") == 0 || strcmp(line, "End file list") == 0 || strcmp(line, "ok") == 0) continue;  // Start and Stop tag
    if (strlen(line) < strlen(infoFile.title) - 4) continue;  // No path line exclude
    if (strlen(infoFile.title) > 4 && strstr(line, infoFile.title + 4) == NULL) continue;  // No current directory

    char *pline = line + strlen(infoFile.title) - 4;
    if (strlen(infoFile.title) > 4) pline++;

    if (strchr(pline, '/') == NULL)
    {
      // FILE
      if (infoFile.fileCount >= FILE_NUM)
        continue;  // Gcode max number is FILE_NUM

      if (infoMachineSettings.long_filename_support == ENABLED)
      {
        char *Pstr_tmp = strrchr(line, ' ');
        if (Pstr_tmp != NULL)
          *Pstr_tmp = 0;
        // remove file size from line
        char *longfilename;
        if (strrchr(line, '~') != NULL)  // check if file name is 8.3 format
          longfilename = request_M33(line);
        else
          longfilename = line;

        /*
          When AUTO_REPORT_TEMPERATURES is enabled by M155, The response of M33 may become the following
            SENDING: M33 /1A29A~1.GCO
            T:29.43 /0.00 B:27.95 /0.00 @:0 B@:0
            /1.gcode
            ok
          So the longfilename will be parsed "0.00 @:0 B@:0" instead of "1.gcode" if the truncated character is '\n' not string "\nok"
        */
        Pstr_tmp = strstr(longfilename, "\nok");
        if (Pstr_tmp != NULL)
          *Pstr_tmp = 0;  // remove end of M33 command

        Pstr_tmp = strrchr(longfilename, '/');  // remove folder information
        if (Pstr_tmp == NULL)
          Pstr_tmp = longfilename;
        else
          Pstr_tmp++;

        infoFile.Longfile[infoFile.fileCount] = malloc(strlen(Pstr_tmp) + 1);

        if (infoFile.Longfile[infoFile.fileCount] == NULL)
        {
          clearRequestCommandInfo();
          break;
        }
        strcpy(infoFile.Longfile[infoFile.fileCount], Pstr_tmp);
        clearRequestCommandInfo();  // for M33
      }

      char* rest = pline;
      char* file = strtok_r(rest, " ", &rest);  // remove file size from pline
      infoFile.file[infoFile.fileCount] = malloc(strlen(file) + 1);
      if (infoFile.file[infoFile.fileCount] == NULL) break;
      strcpy(infoFile.file[infoFile.fileCount++], file);
    }
    else
    {
      // DIRECTORY
      if (infoFile.folderCount >= FOLDER_NUM)
        continue;  // floder max number is FOLDER_NUM

      char* rest = pline;
      char* folder = strtok_r(rest, "/", &rest);

      bool found = false;
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
        uint16_t len = strlen(folder) + 1;
        infoFile.folder[infoFile.folderCount] = malloc(len);
        if (infoFile.folder[infoFile.folderCount] == NULL)
          break;
        strcpy(infoFile.folder[infoFile.folderCount++], folder);
      }
    }
  }
  free(data);
  return true;
}
