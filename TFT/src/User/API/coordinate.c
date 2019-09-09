#include "coordinate.h"
#include "string.h"

const char axis_id[TOTAL_AXIS]={'X','Y','Z','E'};

COORDINATE coordinate={{0.0f,0.0f,0.0f,0.0f},3000};

static bool relative_mode = false;
static bool relative_e = false;

bool coorGetRelative(void)
{
  return relative_mode;
}

void coorSetRelative(bool mode)
{
  relative_mode = mode;
}

bool eGetRelative(void)
{
  return relative_e;
}

void eSetRelative(bool mode)
{
  relative_e = mode;
}

void coordinateSetAxis(AXIS axis,float position)
{
  bool r = (axis == E_AXIS) 
          ? relative_e || relative_mode
          : relative_mode;

  if(r==false)
  {
    coordinate.axis[axis] = position;
  }
  else
  {
    coordinate.axis[axis] += position;
  }
}

void coordinateSetFeedRate(u32 feedrate)
{
  coordinate.feedrate=feedrate;
}

float coordinateGetAxis(AXIS axis)
{
  return coordinate.axis[axis];
}

u32 coordinateGetFeedRate(void)
{
  return coordinate.feedrate;
}

void coordinateGetAll(COORDINATE *tmp)
{
  memcpy(tmp, &coordinate, sizeof(coordinate));
}

