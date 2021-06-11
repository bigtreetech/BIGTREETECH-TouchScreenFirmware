#include "PowerFailed.h"
#include "includes.h"

BREAK_POINT  infoBreakPoint;
char powerFailedFileName[256];
FIL fpPowerFailed;

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

bool powerFailedCreate(char *path)
{
  UINT br;

  create_ok = false;
  if (!infoSettings.powerloss_en) return false;   // disable plr
  if (infoFile.source >= BOARD_SD) return false;  // on board SD not support now

  if (f_open(&fpPowerFailed, powerFailedFileName, FA_OPEN_ALWAYS | FA_WRITE) != FR_OK) return false;

  f_write(&fpPowerFailed, path, MAX_PATH_LEN, &br);
  uint8_t model_icon = isPrintModelIcon();
  f_write(&fpPowerFailed, &model_icon, 1, &br);
  f_write(&fpPowerFailed, &infoBreakPoint, sizeof(BREAK_POINT), &br);
  f_sync(&fpPowerFailed);

  create_ok = true;
  return true;
}

void powerFailedCache(uint32_t offset)
{
  UINT br;

  if (infoBreakPoint.axis[Z_AXIS] == coordinateGetAxisTarget(Z_AXIS)) return;  // Z axis no changed.
  if (create_ok == false) return;
  if (infoCacheCmd.count != 0) return;
  if (!isPaused())
  { // not paused, update printing progress status.
    infoBreakPoint.offset = offset;
    for (AXIS i = X_AXIS; i < TOTAL_AXIS; i++)
    {
      infoBreakPoint.axis[i] = coordinateGetAxisTarget(i);
    }
    infoBreakPoint.feedrate = coordinateGetFeedRate();
    infoBreakPoint.speed = speedGetCurPercent(0);  // Move speed percent
    infoBreakPoint.flow = speedGetCurPercent(1);  // Flow percent

    for (uint8_t i = 0; i < infoSettings.hotend_count; i++)  // Tool nozzle
    {
      infoBreakPoint.target[i] = heatGetTargetTemp(i);
    }
    for (uint8_t i = MAX_HOTEND_COUNT; i < MAX_HEATER_COUNT; i++)  // Bed & Chamber
    {
      infoBreakPoint.target[i] = heatGetTargetTemp(i);
    }
    infoBreakPoint.tool = heatGetCurrentTool();

    for (uint8_t i = 0; i < infoSettings.fan_count; i++)
    {
      infoBreakPoint.fan[i] = fanGetCurSpeed(i);
    }
    infoBreakPoint.relative = coorGetRelative();
    infoBreakPoint.relative_e = eGetRelative();
  }
  else if (infoBreakPoint.pause) return;  // paused and the pause state has been saved

  infoBreakPoint.pause = isPaused();

  f_lseek(&fpPowerFailed, MAX_PATH_LEN + 1);  // infoFile.title + infoPrinting.model_icon
  f_write(&fpPowerFailed, &infoBreakPoint, sizeof(BREAK_POINT), &br);
  f_sync(&fpPowerFailed);
}

void powerFailedClose(void)
{
  if (create_ok == false) return;

  f_close(&fpPowerFailed);
}

void powerFailedDelete(void)
{
  if (create_ok == false) return;

  f_unlink(powerFailedFileName);
  clearPowerFailed();
}

static bool powerFailedExist(void)
{
  FIL fp;
  UINT br;

  if (f_open(&fp, powerFailedFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK) return false;
  if (f_read(&fp, infoFile.title, MAX_PATH_LEN, &br)               != FR_OK) return false;
  if (f_close(&fp)                                                 != FR_OK) return false;

  create_ok = true;
  return true;
}

bool powerFailedlSeek(FIL* fp)
{
  if (f_lseek(fp,infoBreakPoint.offset) != FR_OK) return false;

  return true;
}

bool powerOffGetData(void)
{
  FIL     fp;
  UINT    br;
  uint8_t model_icon;

  if (f_open(&fp, powerFailedFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK) return false;
  if (f_lseek(&fp, MAX_PATH_LEN)                                   != FR_OK) return false;
  if (f_read(&fp, &model_icon, 1, &br)                             != FR_OK) return false;
  if (f_read(&fp, &infoBreakPoint,  sizeof(infoBreakPoint), &br)   != FR_OK) return false;

  setPrintModelIcon(model_icon);

  for (uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    if (infoBreakPoint.fan[i] != 0)
    {
      mustStoreCacheCmd(fanCmd[i], infoBreakPoint.fan[i]);
    }
  }

  mustStoreCacheCmd("%s\n", tool_change[infoBreakPoint.tool]);

  for (uint8_t i = MAX_HEATER_COUNT - 1; i >= MAX_HOTEND_COUNT; i--)  // Bed & Chamber infoCacheCmd.queue[0 - 1]
  {
    if (infoBreakPoint.target[i] != 0)
      mustStoreCacheCmd("%s S%d\n", heatWaitCmd[i], infoBreakPoint.target[i]);
  }
  for (int8_t i = infoSettings.hotend_count - 1; i >= 0; i--)  // Tool nozzle infoCacheCmd.queue[2 - 7]
  {
    if (infoBreakPoint.target[i] != 0)
      mustStoreCacheCmd("%s S%d\n", heatWaitCmd[i], infoBreakPoint.target[i]);
  }

  if (infoBreakPoint.feedrate != 0)
  {
    uint16_t z_raised = 0;

    if (infoSettings.btt_ups == 1)
      z_raised += infoSettings.powerloss_z_raise;

    if (infoBreakPoint.pause)
      z_raised += infoSettings.pause_z_raise;

    mustStoreCacheCmd("G92 Z%.3f\n", infoBreakPoint.axis[Z_AXIS] + z_raised);  // infoCacheCmd.queue[8 - 11]
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

    mustStoreCacheCmd("M83\n");  // infoCacheCmd.queue[12 - 17]
    mustStoreCacheCmd("G1 E30 F300\n");
    mustStoreCacheCmd("G1 E-%.5f F4800\n", infoSettings.pause_retract_len);
    mustStoreCacheCmd("G1 X%.3f Y%.3f Z%.3f F3000\n",
                      infoBreakPoint.axis[X_AXIS],
                      infoBreakPoint.axis[Y_AXIS],
                      infoBreakPoint.axis[Z_AXIS]);
    mustStoreCacheCmd("G1 E%.5f F4800\n", infoSettings.resume_purge_len);
    mustStoreCacheCmd("G92 E%.5f\nG1 F%d\n", infoBreakPoint.axis[E_AXIS], infoBreakPoint.feedrate);

    if (infoBreakPoint.relative_e == false)  // infoCacheCmd.queue[18]
    {
      mustStoreCacheCmd("M82\n");
    }

    // infoCacheCmd.queue[19]  max length = 0 - 19.
    if (infoBreakPoint.relative == true)
    {
      mustStoreCacheCmd("G91\n");
    }
    // End of cache
    // The length of the gcode stored in the cache should be less than max length = 20
    // Otherwise, if there are M109/M190/M191 blocking the queue, it will cause "Busy processing, please wait..." and bring bad user experience
  }

  f_close(&fp);
  return true;
}

void menuPowerOff(void)
{
  uint16_t key_num = IDLE_TOUCH;

  clearPowerFailed();
  GUI_Clear(infoSettings.bg_color);

  GUI_DispString((LCD_WIDTH - GUI_StrPixelWidth(LABEL_LOADING)) / 2, LCD_HEIGHT / 2 - BYTE_HEIGHT, LABEL_LOADING);

  if (mountFS() == true && powerFailedExist())
  {
    char okTxt[MAX_LANG_LABEL_LENGTH];
    char cancelTxt[MAX_LANG_LABEL_LENGTH];
    loadLabelText((uint8_t*)okTxt, LABEL_CONFIRM);
    loadLabelText((uint8_t*)cancelTxt, LABEL_CANCEL);

    popupDrawPage(DIALOG_TYPE_QUESTION, bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t* )infoFile.title,
                  (uint8_t*)okTxt, (uint8_t*)cancelTxt);

    while (infoMenu.menu[infoMenu.cur] == menuPowerOff)
    {
      key_num = KEY_GetValue(2, doubleBtnRect);
      switch (key_num)
      {
        case KEY_POPUP_CONFIRM:
          powerOffGetData();
          infoMenu.menu[1] = menuPrintFromSource;
          infoMenu.menu[2] = menuBeforePrinting;
          infoMenu.cur = 2;
          break;

        case KEY_POPUP_CANCEL:
          powerFailedDelete();
          ExitDir();
          infoMenu.cur--;
          break;
      }

      #ifdef SD_CD_PIN
        if (isVolumeExist(infoFile.source) != true)
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
