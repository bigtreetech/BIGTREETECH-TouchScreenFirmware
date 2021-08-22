#ifndef _LEVELING_H_
#define _LEVELING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
  LEVEL_BOTTOM_LEFT = 0,  // bottom left corner
  LEVEL_BOTTOM_RIGHT,     // bottom right corner
  LEVEL_TOP_RIGHT,        // top right corner
  LEVEL_TOP_LEFT,         // top left corner
  LEVEL_CENTER,           // center point
  LEVEL_POINT_COUNT
} LEVELING_POINT;

typedef int16_t LEVELING_POINTS[LEVEL_POINT_COUNT][2];

void levelingGetPoints(LEVELING_POINTS points);
void levelingMoveToPoint(LEVELING_POINT point);
void menuManualLeveling(void);

#ifdef __cplusplus
}
#endif

#endif
