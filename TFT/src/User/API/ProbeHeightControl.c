#include "ProbeHeightControl.h"
#include "includes.h"

static bool curSoftwareEndstops = true;
static u32 nextTime = 0;
static u32 updateTime = 200;                               // 1 seconds is 1000

/* Enable probe height
 * Temporary disable software endstops
 */
void probeHeightEnable(void)
{
  curSoftwareEndstops = infoMachineSettings.softwareEndstops;

  if (curSoftwareEndstops)                                 // if software endstops is enabled, disable it temporary
    mustStoreCmd("M211 S0\n");                             // disable software endstops to move nozzle minus Zero (Z0) if necessary
#ifdef  RepRapFirmware
  if(infoMachineSettings.isMarlinFirmware == 0)
  {
    mustStoreCmd("M564 S0 H0\n");
  }
#endif
}

/* Disable probe height
 * Restore original software endstops state
 */
void probeHeightDisable(void)
{
  if (curSoftwareEndstops)                                 // if software endstops was originally enabled, enable it again
    mustStoreCmd("M211 S1\n");                             // enable software endstops
  #ifdef RepRapFirmware
  if(infoMachineSettings.isMarlinFirmware == 0)
  {
    mustStoreCmd("M564 S1 H1\n");
  }
  #endif
}

/* Start probe height */
void probeHeightStart(float initialHeight)
{
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("G1 Z%.2f\n", initialHeight);               // move nozzle to provided absolute Z point

  mustStoreCmd("G91\n");                                   // set relative position mode
}

/* Stop probe height */
void probeHeightStop(void)
{
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 Z%.2f F%d\n",
    infoSettings.pause_z_raise,
    infoSettings.axis_speed[infoSettings.move_speed]);     // raise Z and set feedrate

  mustStoreCmd("G90\n");                                   // set absolute position mode
}

/* Change probe height */
void probeHeightMove(float unit, int8_t direction)
{
  // if invert is true, 'direction' multiplied by -1
  storeCmd("G1 Z%.2f F%d\n", (infoSettings.invert_axis[Z_AXIS] ? -direction : direction) * unit, infoSettings.axis_speed[infoSettings.move_speed]);
}

/* Query for new coordinates */
void probeHeightQueryCoord(void)
{
  if (OS_GetTimeMs() > nextTime)
  {
    coordinateQuery();
    nextTime = OS_GetTimeMs() + updateTime;
  }
}
