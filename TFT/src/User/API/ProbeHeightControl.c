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
}

/* Disable probe height
 * Restore original software endstops state
 */
void probeHeightDisable(void)
{
  if (curSoftwareEndstops)                                 // if software endstops was originally enabled, enable it again
    mustStoreCmd("M211 S1\n");                             // enable software endstops
}

/* Start probe height */
void probeHeightStart(void)
{
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("G1 Z0\n");                                 // move nozzle to Z0 absolute point

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
