#include "ProbeHeightControl.h"
#include "includes.h"

#define PROBE_UPDATE_DELAY 200  // 1 seconds is 1000

static uint32_t nextQueryTime = 0;
static bool curSoftwareEndstops = true;

// Enable probe height
// Temporary disable software endstops
void probeHeightEnable(void)
{
  curSoftwareEndstops = infoMachineSettings.softwareEndstops;

  if (curSoftwareEndstops)  // if software endstops is enabled, disable it temporary
  {
    if (infoMachineSettings.firmwareType == FW_REPRAPFW)
      mustStoreCmd("M564 S0 H0\n");
    else
      mustStoreCmd("M211 S0\n");  // disable software endstops to move nozzle minus Zero (Z0) if necessary
  }
}

// Disable probe height
// Restore original software endstops state
void probeHeightDisable(void)
{
  if (curSoftwareEndstops)  // if software endstops was originally enabled, enable it again
  {
    if (infoMachineSettings.firmwareType == FW_REPRAPFW)
      mustStoreCmd("M564 S1 H1\n");
    else
      mustStoreCmd("M211 S1\n");  // enable software endstops
  }
}

// Start probe height
void probeHeightStart(float initialHeight, bool relativeHeight)
{
  if (relativeHeight)
    probeHeightRelative();                                // set relative position mode
  else
    probeHeightAbsolute();                                // set absolute position mode

  mustStoreCmd("G1 Z%.2f F%d\n",
               initialHeight,
               infoSettings.level_feedrate[FEEDRATE_Z]);  // move nozzle to provided absolute Z point and set feedrate
  probeHeightRelative();                                  // set relative position mode
}

// Stop probe height
void probeHeightStop(float raisedHeight)
{
  probeHeightRelative();                                  // set relative position mode
  mustStoreCmd("G1 Z%.2f F%d\n",
               raisedHeight,
               infoSettings.level_feedrate[FEEDRATE_Z]);  // raise Z and set feedrate
  probeHeightAbsolute();                                  // set absolute position mode
}

// Set probe height to relative position mode
void probeHeightRelative(void)
{
  mustStoreCmd("G91\n");  // set relative position mode
}

// Set probe height to absolute position mode
void probeHeightAbsolute(void)
{
  mustStoreCmd("G90\n");  // set absolute position mode
}

// Change probe height
void probeHeightMove(float unit, int8_t direction)
{
  // if invert is true, 'direction' multiplied by -1
  storeCmd("G1 Z%.2f F%d\n", (infoSettings.invert_axis[Z_AXIS] ? -direction : direction) * unit,
           infoSettings.level_feedrate[FEEDRATE_Z]);
}

// Query for new coordinates
void probeHeightQueryCoord(void)
{
  if (OS_GetTimeMs() > nextQueryTime)
  {
    coordinateQuery();
    nextQueryTime = OS_GetTimeMs() + PROBE_UPDATE_DELAY;
  }
}
