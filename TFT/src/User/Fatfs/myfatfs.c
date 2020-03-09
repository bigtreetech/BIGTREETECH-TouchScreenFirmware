#include "myfatfs.h"
#include "includes.h"
#include "diskio.h"

FATFS fatfs[FF_VOLUMES]; /* FATFS work area*/

/*
 mount SD Card from Fatfs
 true: mount ok
 false: mount failed
*/
 bool mountSDCard(void)
{
  return (f_mount(&fatfs[VOLUMES_SD_CARD], "SD:", 1) == FR_OK);
}

/*
 mount U disk from Fatfs
*/
 bool mountUDisk(void)
{
  return (f_mount(&fatfs[VOLUMES_U_DISK], "U:", 1)== FR_OK);
}

/*
 scanf gcode file in current path
 true: scanf ok
 false: opendir failed
*/
bool scanPrintFilesFatFs(void)
{
  FILINFO  finfo;
  uint16_t len = 0;
  DIR      dir;
  uint8_t  i=0;

  clearInfoFile();

  if (f_opendir(&dir, infoFile.title) != FR_OK) return false;

  for (;;)
  {
    if (f_readdir(&dir,&finfo) !=FR_OK || finfo.fname[0] == 0)  break;
    if ((finfo.fattrib&AM_HID) != 0)  continue;
    if (infoFile.f_num >= FILE_NUM && infoFile.F_num >= FOLDER_NUM)  break;

    len = strlen(finfo.fname) + 1;
    if ((finfo.fattrib&AM_DIR) == AM_DIR)
    {
      if (infoFile.F_num >= FOLDER_NUM)  continue;

      infoFile.folder[infoFile.F_num] = malloc(len);
      if (infoFile.folder[infoFile.F_num] == NULL)  break;
      memcpy(infoFile.folder[infoFile.F_num++], finfo.fname, len);
    }
    else
    {
      if (infoFile.f_num >= FILE_NUM)  continue;

      if (strstr(finfo.fname, ".gcode") == NULL)  continue;

      infoFile.file[infoFile.f_num] = malloc(len);
      if (infoFile.file[infoFile.f_num] == NULL)  break;
      memcpy(infoFile.file[infoFile.f_num++], finfo.fname, len);
    }
  }

  f_closedir(&dir);

  for(i=0; i < infoFile.F_num/2; i++)
  {
    char *temp = infoFile.folder[i];
    infoFile.folder[i] = infoFile.folder[infoFile.F_num - i - 1];
    infoFile.folder[infoFile.F_num - i - 1] = temp;
  }
  for(i=0; i < infoFile.f_num/2; i++)
  {
    char *temp = infoFile.file[i];
    infoFile.file[i] = infoFile.file[infoFile.f_num - i - 1];
    infoFile.file[infoFile.f_num - i - 1] = temp;
  }
  return true;
}





/*
* /
void GUI_DispDate(uint16_t date, uint16_t time)
{
  char buf[100];
  static uint8_t i=0;
  sprintf(buf,"%d/%d/%d--%d:%d:%d",1980+(date>>9),(date>>5)&0xF,date&0x1F,time>>11,(time>>5)&0x3F,time&0x1F);
  GUI_DispString(0,i,(uint8_t* )buf,0);
  i+=16;
}
*/


/*
*/
bool Get_NewestGcode(const TCHAR* path)
{
  uint32_t date=0;
  FILINFO  finfo;
  uint16_t len = 0;
  DIR      dirs;
  char	   status = 0;

  if (f_opendir(&dirs, path) != FR_OK) return false;

  len=strlen(path);
  while (f_readdir(&dirs, &finfo) == FR_OK)
  {
    if(finfo.fname[0]==0)                           break;
    if((finfo.fattrib&AM_HID) != 0)                 continue;

    if((finfo.fattrib&AM_DIR) == AM_DIR)
    {
      char  *nextdirpath = malloc(len+strlen(finfo.fname)+2);
      if(nextdirpath==NULL)                         break;

      strcpy(nextdirpath, path);
      strcat(nextdirpath,"/");
      strcat(nextdirpath, finfo.fname);

      status|=Get_NewestGcode(nextdirpath);
      free(nextdirpath);
      nextdirpath=NULL;
    }
    else
    {
      if(strstr(finfo.fname,".gcode")==NULL)        continue;
      if(((finfo.fdate <<16)|finfo.ftime) < date)   continue;

      date=(finfo.fdate <<16)|finfo.ftime;
      resetInfoFile();

      if(len+strlen(finfo.fname)+2>MAX_PATH_LEN)    break;

      strcpy(infoFile.title,path);
      strcat(infoFile.title,"/");
      strcat(infoFile.title,finfo.fname);
      status = 1;
    }
  }
  return status;
}
