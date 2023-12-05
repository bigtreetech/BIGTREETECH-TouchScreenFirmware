#include "LevelingControl.h"
#include "includes.h"

typedef struct XY_coord
{
  int16_t x_coord;
  int16_t y_coord;
} COORD, LEVELING_POINT_COORDS[LEVELING_POINT_COUNT];

LEVELING_POINT probedPoint = LEVEL_NO_POINT;  // last probed point or LEVEL_NO_POINT in case of no new updates
float probedZ = 0.0f;                         // last Z offset measured by probe

int16_t setCoordValue(AXIS axis, ALIGN_POSITION align)
{
  return ((align == LEFT || align == BOTTOM) ? infoSettings.machine_size_min[axis] + infoSettings.level_edge
                                             : infoSettings.machine_size_max[axis] - infoSettings.level_edge) - infoParameters.HomeOffset[axis];
}

void levelingGetPointCoords(LEVELING_POINT_COORDS coords)
{
  int16_t x_left = setCoordValue(X_AXIS, LEFT);
  int16_t x_right = setCoordValue(X_AXIS, RIGHT);
  int16_t y_bottom = setCoordValue(Y_AXIS, BOTTOM);
  int16_t y_top = setCoordValue(Y_AXIS, TOP);

  if (GET_BIT(infoSettings.inverted_axis, X_AXIS))
  { // swap left and right
    int16_t temp = x_left;
    x_left = x_right;
    x_right = temp;
  }

  // leveling Y axis (E_AXIS -> index for param "inverted_axis LY<x>" in "config.ini")
  if (GET_BIT(infoSettings.inverted_axis, E_AXIS))
  { // swap bottom and top
    int16_t temp = y_bottom;
    y_bottom = y_top;
    y_top = temp;
  }

  coords[LEVEL_BOTTOM_LEFT] = (COORD){x_left, y_bottom};
  coords[LEVEL_BOTTOM_RIGHT] = (COORD){x_right, y_bottom};
  coords[LEVEL_TOP_RIGHT] = (COORD){x_right, y_top};
  coords[LEVEL_TOP_LEFT] = (COORD){x_left, y_top};
  coords[LEVEL_CENTER] = (COORD){(x_left + x_right) / 2, (y_bottom + y_top) / 2};
}

LEVELING_POINT levelingGetPoint(int16_t x, int16_t y)
{
  LEVELING_POINT_COORDS coords;
  uint8_t i;

  levelingGetPointCoords(coords);

  for (i = 0; i < LEVELING_POINT_COUNT; i++)
  {
    if (coords[i].x_coord == x && coords[i].y_coord == y)  // if point is found, exit from loop
      break;
  }

  if (i < LEVELING_POINT_COUNT)  // if point is found, return the point
    return i;

  return LEVEL_CENTER;  // if point is not found, return the center point
}

void levelingMoveToPoint(LEVELING_POINT point)
{
  LEVELING_POINT_COORDS coords;

  levelingGetPointCoords(coords);

  if (coordinateIsKnown() == false)
    storeCmd("G28\n");

  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_raise, infoSettings.level_feedrate[FEEDRATE_Z]);
  storeCmd("G0 X%d Y%d F%d\n", coords[point].x_coord, coords[point].y_coord, infoSettings.level_feedrate[FEEDRATE_XY]);
  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_pos, infoSettings.level_feedrate[FEEDRATE_Z]);
}

void levelingProbePoint(LEVELING_POINT point)
{
  LEVELING_POINT_COORDS coords;

  levelingGetPointCoords(coords);

  if (coordinateIsKnown() == false)
    probeHeightHomeAndRaise();  // home and raise nozzle

  if (infoSettings.touchmi_sensor != 0)
  {
    mustStoreCmd("M401\n");
    mustStoreCmd("G30 E0 X%d Y%d\n", coords[point].x_coord, coords[point].y_coord);  // move to selected point
    mustStoreCmd("G1 Z10\n");
  }
  else
  {
    mustStoreCmd("G30 E1 X%d Y%d\n", coords[point].x_coord, coords[point].y_coord);  // move to selected point
  }

  probedPoint = LEVEL_NO_POINT;  // reset probedPoint before waiting for new probed Z
}

void levelingSetProbedPoint(int16_t x, int16_t y, float z)
{
  probedPoint = levelingGetPoint(x, y);
  probedZ = z;
}

LEVELING_POINT levelingGetProbedPoint(void)
{
  return probedPoint;
};

void levelingResetProbedPoint(void)
{
  probedPoint = LEVEL_NO_POINT;
};

float levelingGetProbedZ(void)
{
  return probedZ;
};
