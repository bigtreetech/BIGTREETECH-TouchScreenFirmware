#ifndef _PROBE_HEIGHT_CONTROL_H_
#define _PROBE_HEIGHT_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Enable probe height
 * Temporary disable software endstops
 */
void probeHeightEnable(void);

/* Disable probe height
 * Restore original software endstops state
 */
void probeHeightDisable(void);

/* Start probe height */
void probeHeightStart(void);

/* Stop probe height */
void probeHeightStop(void);

/* Change probe height */
void probeHeightMove(float unit, int8_t direction);

/* Query for new coordinates */
void probeHeightQueryCoord(void);

#ifdef __cplusplus
}
#endif

#endif
