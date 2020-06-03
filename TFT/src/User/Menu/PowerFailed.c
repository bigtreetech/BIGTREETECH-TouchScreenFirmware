#include "PowerFailed.h"
#include "includes.h"

BREAK_POINT  infoBreakPoint;
char powerFailedFileName[256];

static bool create_ok = false;

void powerFailedSetDriverSource(char *src)
{
  strcpy(powerFailedFileName, src);
  strcat(powerFailedFileName, BREAK_POINT_FILE);
}

void clearPowerFailed(void)
{
  memset(&infoBreakPoint, 0, sizeof(BREAK_POINT));
}

FIL fpPowerFailed;
bool powerFailedCreate(char *path)
{
  UINT br;

  create_ok = false;

  if(infoFile.source == BOARD_SD)  return false; // on board SD not support now

  if(f_open(&fpPowerFailed, powerFailedFileName, FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)  return false;

  f_write(&fpPowerFailed, path, MAX_PATH_LEN, &br);
  f_write(&fpPowerFailed, &infoBreakPoint, sizeof(BREAK_POINT), &br);
  f_sync(&fpPowerFailed);

  create_ok = true;
  return true;
}

void powerFailedCache(u32 offset)
{
  UINT        br;

  if (infoBreakPoint.axis[Z_AXIS] == coordinateGetAxisTarget(Z_AXIS)) return; // Z axis no raise.
  if (create_ok == false )      return;
  if (isPause() == true)        return;
  if (infoCacheCmd.count != 0)  return;

  infoBreakPoint.offset = offset;
  for (AXIS i = X_AXIS; i < TOTAL_AXIS; i++)
  {
    infoBreakPoint.axis[i] = coordinateGetAxisTarget(i);
  }
  infoBreakPoint.feedrate = coordinateGetFeedRate();
  infoBreakPoint.speed = speedGetPercent(0); // Move speed percent
  infoBreakPoint.flow = speedGetPercent(1); // Flow percent

  for(TOOL i = BED; i < HEATER_COUNT; i++)
  {
    infoBreakPoint.target[i] = heatGetTargetTemp(i);
  }
  infoBreakPoint.nozzle = heatGetCurrentToolNozzle();

  for(u8 i = 0; i < infoSettings.fan_count; i++)
  {
   infoBreakPoint.fan[i] = fanGetSpeed(i);
  }
  infoBreakPoint.relative = coorGetRelative();
  infoBreakPoint.relative_e = eGetRelative();

  f_lseek(&fpPowerFailed, MAX_PATH_LEN);
  f_write(&fpPowerFailed, &infoBreakPoint, sizeof(BREAK_POINT), &br);
  f_sync(&fpPowerFailed);
}

void powerFailedClose(void)
{
  if(create_ok==false)   return;

  f_close(&fpPowerFailed);
}

void  powerFailedDelete(void)
{
  if(create_ok==false)   return;

  f_unlink(powerFailedFileName);
  clearPowerFailed();
}

static bool powerFailedExist(void)
{
  FIL  fp;
  UINT br;
  if(f_open(&fp, powerFailedFileName, FA_OPEN_EXISTING|FA_READ) != FR_OK)    return false;
  if(f_read(&fp, infoFile.title,      MAX_PATH_LEN, &br) != FR_OK)           return false;
  if(f_close(&fp) != FR_OK)                                                  return false;

  create_ok = true;
  return true;
}


bool powerFailedlSeek(FIL* fp)
{
  if(f_lseek(fp,infoBreakPoint.offset) != FR_OK)  return false;

  return true;
}


bool powerOffGetData(void)
{
  FIL   fp;
  UINT  br;

  if(f_open(&fp, powerFailedFileName, FA_OPEN_EXISTING|FA_READ) != FR_OK)        return false;
  if(f_lseek(&fp, MAX_PATH_LEN)                                 != FR_OK)        return false;
  if(f_read(&fp, &infoBreakPoint,  sizeof(infoBreakPoint), &br) != FR_OK)        return false;

  for(TOOL i = BED; i < HEATER_COUNT; i++)
  {
    if(infoBreakPoint.target[i] != 0)
      mustStoreCacheCmd("%s S%d\n", heatWaitCmd[i], infoBreakPoint.target[i]);
  }

  for(u8 i=0; i < infoSettings.fan_count; i++)
  {
    if(infoBreakPoint.fan[i] != 0)
      mustStoreCacheCmd("%s S%d\n", fanCmd[i], infoBreakPoint.fan[i]);
  }

  mustStoreCacheCmd("%s\n", tool_change[infoBreakPoint.nozzle - NOZZLE0]);
  if(infoBreakPoint.feedrate != 0)
  {
    int btt_zraise = 0;
    if(infoSettings.btt_ups == 1)
        btt_zraise = infoSettings.powerloss_z_raise;
    mustStoreCacheCmd("G92 Z%.3f\n", infoBreakPoint.axis[Z_AXIS] + btt_zraise);
    mustStoreCacheCmd("G1 Z%.3f\n", infoBreakPoint.axis[Z_AXIS] + infoSettings.powerloss_z_raise);
    if (infoSettings.powerloss_home)
    {
      mustStoreCacheCmd("G28\n");
      mustStoreCacheCmd("G1 Z%.3f\n", infoBreakPoint.axis[Z_AXIS] + infoSettings.powerloss_z_raise);
    }
    else
    {
      mustStoreCacheCmd("G28 R0 XY\n");
    }

    mustStoreCacheCmd("M83\n");
    mustStoreCacheCmd("G1 E30 F300\n");
    mustStoreCacheCmd("G1 E-%.5f F4800\n", infoSettings.pause_retract_len);
    mustStoreCacheCmd("G1 X%.3f Y%.3f Z%.3f F3000\n",
                          infoBreakPoint.axis[X_AXIS],
                          infoBreakPoint.axis[Y_AXIS],
                          infoBreakPoint.axis[Z_AXIS]);
    mustStoreCacheCmd("G1 E%.5f F4800\n", infoSettings.resume_purge_len);
    mustStoreCacheCmd("G92 E%.5f\n",infoBreakPoint.axis[E_AXIS]);
    mustStoreCacheCmd("G1 F%d\n",infoBreakPoint.feedrate);

    if(infoBreakPoint.relative_e == false)
    {
      mustStoreCacheCmd("M82\n");
    }
    if(infoBreakPoint.relative == true)
    {
      mustStoreCacheCmd("G91\n");
    }
  }

  f_close(&fp);
  return true;
}

void menuPowerOff(void)
{
  u16 key_num = IDLE_TOUCH;
  clearPowerFailed();
  GUI_Clear(lcd_colors[infoSettings.bg_color]);
  GUI_DispString((LCD_WIDTH - GUI_StrPixelWidth(textSelect(LABEL_LOADING)))/2, LCD_HEIGHT/2 - BYTE_HEIGHT, textSelect(LABEL_LOADING));

  if(mountFS()==true && powerFailedExist())
  {
    popupDrawPage(bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (u8* )infoFile.title, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

    while(infoMenu.menu[infoMenu.cur]==menuPowerOff)
    {
      key_num = KEY_GetValue(2, doubleBtnRect);
      switch(key_num)
      {
        case KEY_POPUP_CONFIRM:
          powerOffGetData();
          infoMenu.menu[1] = menuPrintFromSource;
          infoMenu.menu[2] = menuBeforePrinting;
          infoMenu.cur=2;
          break;

        case KEY_POPUP_CANCEL:
          powerFailedDelete();
          ExitDir();
          infoMenu.cur--;
          break;
      }

      #ifdef SD_CD_PIN
      if(isVolumeExist(infoFile.source) != true)
      {
        resetInfoFile();
        clearPowerFailed();
        infoMenu.cur--;
      }
      #endif
      loopProcess();
    }
  }
  else
  {
    infoMenu.cur--;
  }
}
