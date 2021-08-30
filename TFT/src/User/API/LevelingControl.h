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
  LEVEL_BOTTOM_LEFT = 0,  // bottom left bed corner
  LEVEL_BOTTOM_RIGHT,     // bottom right bed corner
  LEVEL_TOP_RIGHT,        // top right bed corner
  LEVEL_TOP_LEFT,         // top left bed corner
  LEVEL_CENTER,           // center bed point
  LEVEL_POINT_COUNT
} LEVELING_POINT;

typedef int16_t LEVELING_POINT_COORDS[LEVEL_POINT_COUNT][2];  // [][0] X coord, [][1] Y coord

void levelingGetPointCoords(LEVELING_POINT_COORDS coords);
LEVELING_POINT levelingGetPoint(int16_t x, int16_t y);  // return the point matching XY coords or LEVEL_CENTER in case of no match
void levelingProbePoint(LEVELING_POINT point);
void levelingMoveToPoint(LEVELING_POINT point);

#ifdef __cplusplus
}
#endif

#endif
