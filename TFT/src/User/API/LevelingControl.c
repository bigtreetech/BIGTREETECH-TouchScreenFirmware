#include "LevelingControl.h"
#include "includes.h"

void levelingGetPointCoords(LEVELING_POINT_COORDS coords)
{
  int16_t x_left = infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge;
  int16_t x_right = infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge;
  int16_t y_bottom = infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge;
  int16_t y_top = infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge;

  if (infoSettings.invert_axis[X_AXIS])
  {
    int16_t temp = x_left;  // Swap left and right
    x_left = x_right;
    x_right = temp;
  }

  // The Y-axis of different printer (move hotbed or move nozzle) move in different directions.
  // So Y-axis leveling invert can't follow up invert_axis[Y_AXIS].
  // We separate a single variable to deal with the Y-axis leveling movement direction.
  if (infoSettings.leveling_invert_y_axis)
  {
    int16_t temp = y_bottom;  // Swap lower and upper
    y_bottom = y_top;
    y_top = temp;
  }

  coords[LEVEL_BOTTOM_LEFT][0] = x_left;
  coords[LEVEL_BOTTOM_LEFT][1] = y_bottom;
  coords[LEVEL_BOTTOM_RIGHT][0] = x_right;
  coords[LEVEL_BOTTOM_RIGHT][1] = y_bottom;
  coords[LEVEL_TOP_RIGHT][0] = x_right;
  coords[LEVEL_TOP_RIGHT][1] = y_top;
  coords[LEVEL_TOP_LEFT][0] = x_left;
  coords[LEVEL_TOP_LEFT][1] = y_top;
  coords[LEVEL_CENTER][0] = (x_left + x_right) / 2;
  coords[LEVEL_CENTER][1] = (y_bottom + y_top) / 2;
}

LEVELING_POINT levelingGetPoint(int16_t x, int16_t y)
{
  LEVELING_POINT_COORDS coords;
  uint8_t i;

  levelingGetPointCoords(coords);

  for (i = 0; i < LEVEL_POINT_COUNT; i++)
  {
    if (coords[i][0] == x && coords[i][1] == y)  // if point is found, exit from loop
      break;
  }

  if (i < LEVEL_POINT_COUNT)  // if point is found, return the point
    return i;

  return LEVEL_CENTER;  // if point is not found, return the center point
}

void levelingProbePoint(LEVELING_POINT point)
{
  LEVELING_POINT_COORDS coords;

  levelingGetPointCoords(coords);

  if (infoSettings.touchmi_sensor != 0)
  {
    mustStoreCmd("M401\n");
    mustStoreCmd("G30 E0 X%d Y%d\n", coords[point][0], coords[point][1]);  // move to selected point
    mustStoreCmd("G1 Z10\n");
  }
  else
  {
    mustStoreCmd("G30 E1 X%d Y%d\n", coords[point][0], coords[point][1]);  // move to selected point
  }

  mustStoreCmd(ENABLE_STEPPER_CMD);
  mustStoreCmd(DISABLE_STEPPER_CMD);
}

void levelingMoveToPoint(LEVELING_POINT point)
{
  LEVELING_POINT_COORDS coords;

  levelingGetPointCoords(coords);

  if (coordinateIsKnown() == false)
    storeCmd("G28\n");

  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_raise, infoSettings.level_feedrate[FEEDRATE_Z]);
  storeCmd("G0 X%d Y%d F%d\n", coords[point][0], coords[point][1], infoSettings.level_feedrate[FEEDRATE_XY]);
  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_pos, infoSettings.level_feedrate[FEEDRATE_Z]);
}
