#include "coordinate.h"
#include "string.h"
#include "includes.h"

const char axis_id[TOTAL_AXIS] = {'X', 'Y', 'Z', 'E'};

static COORDINATE targetPosition = {{0.0f, 0.0f, 0.0f, 0.0f}, 3000};

static COORDINATE curPosition = {{0.0f, 0.0f, 0.0f, 0.0f}, 3000};

//
static bool relative_mode = false;
static bool relative_e = false;
// false means current position is unknown
// false after M18/M84 disable stepper or power up, true after G28
static bool position_known = false;
static bool queryWait = false;

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

bool coordinateIsKnown(void)
{
  return position_known;
}

void coordinateSetKnown(bool known)
{
  position_known = known;
}

void coordinateSetAxisTarget(AXIS axis,float position)
{
  bool r = (axis == E_AXIS)
          ? relative_e || relative_mode
          : relative_mode;

  if(r==false)
  {
    targetPosition.axis[axis] = position;
  }
  else
  {
    targetPosition.axis[axis] += position;
  }
}

void coordinateSetFeedRate(u32 feedrate)
{
  targetPosition.feedrate=feedrate;
}

float coordinateGetAxisTarget(AXIS axis)
{
  return targetPosition.axis[axis];
}

u32 coordinateGetFeedRate(void)
{
  return targetPosition.feedrate;
}

void coordinateGetAll(COORDINATE *tmp)
{
  memcpy(tmp, &targetPosition, sizeof(targetPosition));
}

void coordinateSetAxisActualSteps(AXIS axis, int steps)
{
  curPosition.axis[axis] = steps / getParameter(P_STEPS_PER_MM, E_AXIS);
}

void coordinateSetAxisActual(AXIS axis, float position)
{
  curPosition.axis[axis] = position;
}

float coordinateGetAxisActual(AXIS axis)
{
  return curPosition.axis[axis];
}

void coordinateQuerySetWait(bool wait)
{
  queryWait = wait;
}

void coordinateQuery(void)
{
  if (infoHost.connected == true && infoHost.wait == false)
  {
    if (!queryWait)
    {
      storeCmd("M114\n");
      queryWait = true;
    }
  }
}