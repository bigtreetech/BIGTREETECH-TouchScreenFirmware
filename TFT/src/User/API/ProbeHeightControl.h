#ifndef _PROBE_HEIGHT_CONTROL_H_
#define _PROBE_HEIGHT_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Enable probe height
// Temporary disable software endstops and save ABL state
void probeHeightEnable(void);

// Disable probe height
// Restore original software endstops state and ABL state
void probeHeightDisable(void);

// Home, disable ABL and raise nozzle
void probeHeightHome(void);

// Home and disable ABL
void probeHeightHomeAndNoABL(void);

// Home and raise nozzle
void probeHeightHomeAndRaise(void);

// Start probe height
void probeHeightStart(float initialHeight, bool relativeHeight);

// Stop probe height
void probeHeightStop(float raisedHeight);

// Set probe height to relative position mode
void probeHeightRelative(void);

// Set probe height to absolute position mode
void probeHeightAbsolute(void);

// Change probe height
void probeHeightMove(float unit);

// Query for new coordinates
void probeHeightQueryCoord(void);

#ifdef __cplusplus
}
#endif

#endif
