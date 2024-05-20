#ifndef _PROBE_HEIGHT_CONTROL_H_
#define _PROBE_HEIGHT_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// enable probe height.
// Temporary disable software endstops and save ABL state
void probeHeightEnable(void);

// disable probe height.
// Restore original software endstops state and ABL state
void probeHeightDisable(void);

void probeHeightHome(void);                                       // home, disable ABL and raise nozzle
void probeHeightHomeAndNoABL(void);                               // home and disable ABL
void probeHeightHomeAndRaise(void);                               // home and raise nozzle
void probeHeightStart(float initialHeight, bool relativeHeight);  // start probe height
void probeHeightStop(float raisedHeight);                         // stop probe height
void probeHeightRelative(void);                                   // set probe height to relative position mode
void probeHeightAbsolute(void);                                   // set probe height to absolute position mode
void probeHeightMove(float unit);                                 // change probe height
void probeHeightQueryCoord(void);                                 // query for new coordinates

#ifdef __cplusplus
}
#endif

#endif
