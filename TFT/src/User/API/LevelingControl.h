#ifndef _LEVELING_CONTROL_H_
#define _LEVELING_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
  LEVEL_NO_POINT = -1,
  LEVEL_BOTTOM_LEFT = 0,  // bottom left bed corner
  LEVEL_BOTTOM_RIGHT,     // bottom right bed corner
  LEVEL_TOP_RIGHT,        // top right bed corner
  LEVEL_TOP_LEFT,         // top left bed corner
  LEVEL_CENTER,           // center bed point
  LEVELING_POINT_COUNT
} LEVELING_POINT;

void levelingMoveToPoint(LEVELING_POINT point);              // move to point
void levelingProbePoint(LEVELING_POINT point);               // probe point
void levelingSetProbedPoint(int16_t x, int16_t y, float z);  // set probed point and Z offset for point matching XY coords
LEVELING_POINT levelingGetProbedPoint(void);                 // get probed point or LEVEL_NO_POINT in case of no new updates
void levelingResetProbedPoint(void);                         // reset probed point to LEVEL_NO_POINT to check for new updates
float levelingGetProbedZ(void);                              // get probed Z offset

#ifdef __cplusplus
}
#endif

#endif
