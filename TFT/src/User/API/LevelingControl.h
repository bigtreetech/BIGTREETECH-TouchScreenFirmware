#ifndef _LEVELING_CONTROL_H_
#define _LEVELING_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ENABLE_STEPPER_CMD  "M17 X Y Z\n"
#define DISABLE_STEPPER_CMD "M18 S0 X Y Z\n"

typedef enum
{
  LEVEL_NO_POINT = -1,
  LEVEL_BOTTOM_LEFT,    // bottom left bed corner
  LEVEL_BOTTOM_RIGHT,   // bottom right bed corner
  LEVEL_TOP_RIGHT,      // top right bed corner
  LEVEL_TOP_LEFT,       // top left bed corner
  LEVEL_CENTER,         // center bed point
  LEVELING_POINT_COUNT
} LEVELING_POINT;

typedef int16_t LEVELING_POINT_COORDS[LEVELING_POINT_COUNT][2];  // [][0] X coord, [][1] Y coord

extern LEVELING_POINT probedPoint;  // last probed point or LEVEL_NO_POINT in case of no new updates
extern float probedZ;               // last Z offset measured by probe

void levelingGetPointCoords(LEVELING_POINT_COORDS coords);   // get all point coords
LEVELING_POINT levelingGetPoint(int16_t x, int16_t y);       // get point matching XY coords or LEVEL_CENTER in case of no match
void levelingMoveToPoint(LEVELING_POINT point);              // move to point
void levelingProbePoint(LEVELING_POINT point);               // probe point
void levelingSetProbedPoint(int16_t x, int16_t y, float z);  // set probed point and Z offset for point matching XY coords
inline LEVELING_POINT levelingGetProbedPoint(void)
  {return probedPoint;};                                     // get probed point or LEVEL_NO_POINT in case of no new updates
inline void levelingResetProbedPoint(void)
  {probedPoint = LEVEL_NO_POINT;};                           // reset probed point to LEVEL_NO_POINT to check for new updates
inline float levelingGetProbedZ(void)
  {return probedZ;};                                         // get probed Z offset

#ifdef __cplusplus
}
#endif

#endif
