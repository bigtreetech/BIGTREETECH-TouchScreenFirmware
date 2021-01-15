#include "ProbeHeightControl.h"
#include "includes.h"

static uint32_t nextQueryTime = 0;
#define PROBE_UPDATE_DELAY 200  // 1 seconds is 1000

static bool curSoftwareEndstops = true;

/* Enable probe height
 * Temporary disable software endstops
 */
void probeHeightEnable(void)
{
  curSoftwareEndstops = infoMachineSettings.softwareEndstops;

  if (curSoftwareEndstops)  // if software endstops is enabled, disable it temporary
  {
    if (infoMachineSettings.firmwareType == FW_REPRAPFW)
      mustStoreCmd("M564 S0 H0\n");
    else
      mustStoreCmd("M211 S0\n");
  }
}

/* Disable probe height
 * Restore original software endstops state
 */
void probeHeightDisable(void)
{
  if (curSoftwareEndstops)  // if software endstops was originally enabled, enable it again
  {
    if (infoMachineSettings.firmwareType == FW_REPRAPFW)
      mustStoreCmd("M564 S1 H1\n");
    else
      mustStoreCmd("M211 S1\n");
  }
}

/* Start probe height */
void probeHeightStart(float initialHeight)
{
  mustStoreCmd("G90\n");                      // set absolute position mode
  mustStoreCmd("G1 Z%.2f\n", initialHeight);  // move nozzle to provided absolute Z point
  mustStoreCmd("G91\n");                      // set relative position mode
}

/* Stop probe height */
void probeHeightStop(void)
{
  mustStoreCmd("G91\n");  // set relative position mode
  mustStoreCmd("G1 Z%.2f F%d\n", infoSettings.level_z_raise, infoSettings.level_feedrate[FEEDRATE_Z]);  // raise Z and set feedrate
  mustStoreCmd("G90\n");  // set absolute position mode
}

/* Change probe height */
void probeHeightMove(float unit, int8_t direction)
{
  // if invert is true, 'direction' multiplied by -1
  storeCmd("G1 Z%.2f F%d\n", (infoSettings.invert_axis[Z_AXIS] ? -direction : direction) * unit,
           infoSettings.level_feedrate[FEEDRATE_Z]);
}

/* Query for new coordinates */
void probeHeightQueryCoord(void)
{
  if (OS_GetTimeMs() > nextQueryTime)
  {
    coordinateQuery();
    nextQueryTime = OS_GetTimeMs() + PROBE_UPDATE_DELAY;
  }
}
