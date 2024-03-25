#include "PowerFailed.h"
#include "includes.h"

#define BREAK_POINT_FILE "Printing.sys"

typedef struct
{
  float    axis[TOTAL_AXIS];
  uint32_t feedrate;
  uint16_t speed;
  uint16_t flow;
  uint16_t target[MAX_HEATER_COUNT];
  uint16_t fan[MAX_FAN_COUNT];
  uint8_t  tool;
  uint32_t offset;
  bool     relative;
  bool     relative_e;
  bool     pause;
} BREAK_POINT;

static BREAK_POINT infoBreakPoint;
static char powerFailedFileName[sizeof(BREAK_POINT_FILE) + 20 + 1];  // extra 20 chars for concatenation with string returned by getFS()
static FIL fpPowerFailed;

static bool restore = false;     // print restore flag disabled by default
static bool restore_ok = false;  // print restore initialization flag disabled by default
static bool load_ok = false;     // PLR file loading flag disabled by default
static bool create_ok = false;   // PLR file creation flag disabled by default

void powerFailedSetRestore(bool allowed)
{
  restore = allowed;
}

bool powerFailedGetRestore(void)
{
  return restore;
}

static void powerFailedSetDriverSource(void)
{
  sprintf(powerFailedFileName, "%s%s", getFS(), BREAK_POINT_FILE);
}

bool powerFailedLoad(FIL * print_fp)
{
  // set status flag first
  load_ok = false;
  restore_ok = false;

  // if print restore flag is disabled, nothing to do
  if (!restore) return false;

  // disable print restore flag (one shot flag) for the next print.
  // The flag must always be explicitly re-enabled (e.g by powerFailedSetRestore function)
  restore = false;

  // try to load PLR info from file in order to restore the print from the failed point

  FIL fp;
  UINT br;
  uint8_t model_icon;

  powerFailedSetDriverSource();

  if (f_open(&fp, powerFailedFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    return false;

  if (f_lseek(&fp, MAX_PATH_LEN) == FR_OK)
  {
    if (f_read(&fp, &model_icon, 1, &br) == FR_OK)
    {
      if (f_read(&fp, &infoBreakPoint, sizeof(infoBreakPoint), &br) == FR_OK)
      {
        // set offset on print file once infoBreakPoint was successfully read
        if (f_lseek(print_fp, infoBreakPoint.offset) == FR_OK)
        {
          load_ok = true;
          restore_ok = true;
        }
      }
    }
  }

  f_close(&fp);  // always close the file once it was successfully open before

  if (load_ok)  // if the file was successfully open, set icon model
    setPrintModelIcon(model_icon);

  return load_ok;
}

bool powerFailedInitRestore(void)
{
  // if print restore initialization flag is disabled, nothing to do
  if (!restore_ok) return false;

  // disable print restore initialization flag (one shot flag) for the next print
  restore_ok = false;

  for (uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    if (infoBreakPoint.fan[i] != 0)
      mustStoreCmd(fanCmd[i], infoBreakPoint.fan[i]);
  }

  mustStoreCmd("%s\n", toolChange[infoBreakPoint.tool]);

  for (uint8_t i = MAX_HEATER_COUNT - 1; i >= MAX_HOTEND_COUNT; i--)  // bed & chamber
  {
    if (infoBreakPoint.target[i] != 0)
      mustStoreCmd("%s S%d\n", heatWaitCmd[i], infoBreakPoint.target[i]);
  }

  for (int8_t i = infoSettings.hotend_count - 1; i >= 0; i--)  // tool nozzle
  {
    if (infoBreakPoint.target[i] != 0)
      mustStoreCmd("%s S%d\n", heatWaitCmd[i], infoBreakPoint.target[i]);
  }

  if (infoBreakPoint.feedrate != 0)
  {
    uint16_t z_raised = 0;

    if (infoSettings.btt_ups == 1)
      z_raised += infoSettings.plr_z_raise;

    if (infoBreakPoint.pause)
      z_raised += infoSettings.pause_z_raise;

    mustStoreCmd("G92 Z%.3f\n", infoBreakPoint.axis[Z_AXIS] + z_raised);
    mustStoreCmd("G1 Z%.3f\n", infoBreakPoint.axis[Z_AXIS] + infoSettings.plr_z_raise);

    if (infoSettings.plr_home)
    {
      mustStoreCmd("G28\n");
      mustStoreCmd("G1 Z%.3f\n", infoBreakPoint.axis[Z_AXIS] + infoSettings.plr_z_raise);
    }
    else
    {
      mustStoreCmd("G28 R0 XY\n");
    }

    mustStoreCmd("M83\n");
    mustStoreCmd("G1 E30 F300\n");
    mustStoreCmd("G1 E-%.5f F4800\n", infoSettings.pause_retract_len);
    mustStoreCmd("G1 X%.3f Y%.3f Z%.3f F3000\n", infoBreakPoint.axis[X_AXIS], infoBreakPoint.axis[Y_AXIS], infoBreakPoint.axis[Z_AXIS]);
    mustStoreCmd("G1 E%.5f F4800\n", infoSettings.resume_purge_len);
    mustStoreCmd("G92 E%.5f\nG1 F%d\n", infoBreakPoint.axis[E_AXIS], infoBreakPoint.feedrate);
    mustStoreCmd(infoBreakPoint.relative ? "G91\n" : "G90\n");

    if (infoBreakPoint.relative_e == false)
      mustStoreCmd("M82\n");
  }

  return true;
}

bool powerFailedExist(void)
{
  bool access_ok = false;
  FIL fp;
  UINT br;

  powerFailedSetDriverSource();

  if (f_open(&fp, powerFailedFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    return false;

  if (f_read(&fp, infoFile.path, MAX_PATH_LEN, &br) == FR_OK)
    access_ok = true;

  f_close(&fp);  // always close the file once it was successfully open before

  return access_ok;
}

void powerFailedCreate(char * path)
{
  powerFailedDelete();  // close and delete PLR file, if any, first

  if (!infoSettings.plr) return;                    // if PLR is disabled
  if (infoFile.source >= FS_ONBOARD_MEDIA) return;  // onboard media not supported now

  UINT br;

  powerFailedSetDriverSource();

  if (f_open(&fpPowerFailed, powerFailedFileName, FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
    return;

  if (f_write(&fpPowerFailed, path, MAX_PATH_LEN, &br) == FR_OK)
  {
    uint8_t model_icon = isPrintModelIcon();

    if (f_write(&fpPowerFailed, &model_icon, 1, &br) == FR_OK)
    {
      // if PLR file not loaded, initilaize data. Otherwise use loaded data
      // so also powerFailedInitRestore() function can be used
      if (!load_ok)
        memset(&infoBreakPoint, 0, sizeof(BREAK_POINT));

      if (f_write(&fpPowerFailed, &infoBreakPoint, sizeof(infoBreakPoint), &br) == FR_OK)
      {
        if (f_sync(&fpPowerFailed) == FR_OK)
          create_ok = true;
      }
    }
  }

  if (!create_ok)  // if PLR file was not properly opened and written, close it
    f_close(&fpPowerFailed);

  return;
}

void powerFailedCache(uint32_t offset)
{
  if (!create_ok) return;  // if PLR file not created

  if (infoBreakPoint.axis[Z_AXIS] == coordinateGetAxisTarget(Z_AXIS)) return;  // if Z axis not changed
  if (isNotEmptyCmdQueue()) return;

  if (!isPaused())  // if not paused, update printing progress status
  {
    infoBreakPoint.offset = offset;

    for (AXIS i = X_AXIS; i < TOTAL_AXIS; i++)
    {
      infoBreakPoint.axis[i] = coordinateGetAxisTarget(i);
    }

    infoBreakPoint.feedrate = coordinateGetFeedRate();
    infoBreakPoint.speed = speedGetCurPercent(0);  // speed percent
    infoBreakPoint.flow = speedGetCurPercent(1);   // flow percent

    for (uint8_t i = 0; i < infoSettings.hotend_count; i++)  // tool nozzle
    {
      infoBreakPoint.target[i] = heatGetTargetTemp(i);
    }

    for (uint8_t i = MAX_HOTEND_COUNT; i < MAX_HEATER_COUNT; i++)  // bed & chamber
    {
      infoBreakPoint.target[i] = heatGetTargetTemp(i);
    }

    infoBreakPoint.tool = heatGetToolIndex();

    for (uint8_t i = 0; i < infoSettings.fan_count; i++)
    {
      infoBreakPoint.fan[i] = fanGetCurSpeed(i);
    }

    infoBreakPoint.relative = coorGetRelative();
    infoBreakPoint.relative_e = eGetRelative();
  }
  else if (infoBreakPoint.pause)  // if paused and the pause state has been saved, nothing to do
  {
    return;
  }

  infoBreakPoint.pause = isPaused();

  UINT br;

  f_lseek(&fpPowerFailed, MAX_PATH_LEN + 1);  // infoFile.path + infoPrinting.model_icon
  f_write(&fpPowerFailed, &infoBreakPoint, sizeof(infoBreakPoint), &br);
  f_sync(&fpPowerFailed);
}

void powerFailedDelete(void)
{
  if (create_ok)  // if PLR file is currently open and used, close it first
  {
    f_close(&fpPowerFailed);

    create_ok = false;
  }

  powerFailedSetDriverSource();

  f_unlink(powerFailedFileName);  // delete the file
}
