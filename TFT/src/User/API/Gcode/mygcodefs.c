#include "mygcodefs.h"
#include "includes.h"

/* 
*/
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

//static uint32_t date=0;
//static FILINFO  finfo;
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

  char *data = request_M20();
  char s[3];
  
  if(strstr(data, "\r\n")) //for smoothieware
    strcpy(s, "\r\n");
  else //for Marlin
    strcpy(s, "\n");
    
  char *line = strtok(data, s);
  for (;line != NULL;line = strtok(NULL, s))
  {
    if ( strcmp(line,"Begin file list") == 0 || strcmp(line,"End file list") == 0 || strcmp(line,"ok") == 0) continue; // Start and Stop tag
    if (strlen(line) < strlen(infoFile.title)-4) continue; // No path line exclude
    if (strlen(infoFile.title) > 4 && strstr(line,infoFile.title+4) == NULL) continue; // No current directory

    char *pline = line + strlen(infoFile.title) - 4;
    if (strlen(infoFile.title) > 4)pline++;

    if (strchr(pline, '/') == NULL)
    {
      // FILE
      if (infoFile.f_num >= FILE_NUM)
        continue; /* Gcode max number is FILE_NUM*/

      char* rest = pline;  
      char* file = strtok_r(rest," ",&rest);

      uint16_t len = strlen(file) + 1;

      infoFile.file[infoFile.f_num] = malloc(len);
      if (infoFile.file[infoFile.f_num] == NULL)
        break;
      strcpy(infoFile.file[infoFile.f_num++], file);

    }
    else
    {
      // DIRECTORY
      if (infoFile.F_num >= FOLDER_NUM)
        continue; /* floder max number is FOLDER_NUM */

      char* rest = pline;  
      char* folder = strtok_r(rest,"/",&rest);

      bool found = false;
      for(int i=0; i < infoFile.F_num; i++)
      {
        if(strcmp(folder, infoFile.folder[i]) == 0)
        {
          found = true;
          break;
        }
      }  

      if(!found)
      {
        uint16_t len = strlen(folder) + 1;
        infoFile.folder[infoFile.F_num] = malloc(len);
        if (infoFile.folder[infoFile.F_num] == NULL)
          break;
        strcpy(infoFile.folder[infoFile.F_num++], folder);

      }
    }
  }
  return true;
}

