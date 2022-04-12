#include "coordinate.h"
#include "includes.h"

const char axis_id[TOTAL_AXIS] = {'X', 'Y', 'Z', 'E'};

static COORDINATE targetPosition = {{0.0f, 0.0f, 0.0f, 0.0f}, 3000};
static COORDINATE curPosition = {{0.0f, 0.0f, 0.0f, 0.0f}, 3000};
E_AXIS_BACKUP eAxisBackup = {0, 0, false, false};

/**
 * Obtained from "M114 E" instead of "M114", Because the coordinates of "M114" are not real-time coordinates.
 * It may be replaced by "M114 R".
 */
static float extruderPostion = 0.0f;

static bool relative_mode = false;
static bool relative_e = false;
// false means current position is unknown
// false after M18/M84 disable stepper or power up, true after G28
static bool position_known = false;
static bool coordinateQueryWait = false;
static uint8_t curQuerySeconds = 0;

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

float coordinateGetAxisTarget(AXIS axis)
{
  return targetPosition.axis[axis];
}

void coordinateSetAxisTarget(AXIS axis, float position)
{
  bool r = (axis == E_AXIS) ? (relative_e || relative_mode) : relative_mode;

  if (r == false)
  {
    targetPosition.axis[axis] = position;
  }
  else
  {
    targetPosition.axis[axis] += position;
  }
}

uint32_t coordinateGetFeedRate(void)
{
  return targetPosition.feedrate;
}

void coordinateSetFeedRate(uint32_t feedrate)
{
  targetPosition.feedrate = feedrate;
}

void coordinateGetAll(COORDINATE *tmp)
{
  memcpy(tmp, &targetPosition, sizeof(targetPosition));
}

float coordinateGetExtruderActual(void)
{
  return extruderPostion;
}

void coordinateSetExtruderActualSteps(float steps)
{
  curPosition.axis[E_AXIS] = extruderPostion = steps / getParameter(P_STEPS_PER_MM, E_AXIS);
}

float coordinateGetAxisActual(AXIS axis)
{
  return curPosition.axis[axis];
}

void coordinateSetAxisActual(AXIS axis, float position)
{
  curPosition.axis[axis] = position;
}

void coordinateGetAllActual(COORDINATE *tmp)
{
  memcpy(tmp, &curPosition, sizeof(curPosition));
}

void coordinateQuerySetWait(bool wait)
{
  coordinateQueryWait = wait;
}

/**
 * @brief query gantry position.
 * @param seconds: Pass 0 to query manually or disable auto report. Pass delay in seconds
 *                 for auto query if available in marlin.
 */
void coordinateQuery(uint8_t seconds)
{
  if (infoHost.connected == true && infoHost.wait == false && !coordinateQueryWait)
  {
    if (infoMachineSettings.autoReportPos == 1)  // if auto report is enabled
    {
      if (seconds == 0)  // if manual querying is requested (if query interval is 0)
        coordinateQueryWait = storeCmd("M114\n");

      if (seconds != curQuerySeconds)  // if query interval is changed
      {
        if (storeCmd("M154 S%d\n", seconds))  // turn on or off (if query interval is 0) auto report
          curQuerySeconds = seconds;          // if gcode will be sent, avoid to enable auto report again on next
      }                                       // function call if already enabled for that query interval
    }
    else  // if auto report is disabled
    {
      coordinateQueryWait = storeCmd("M114\n");
    }
  }
}

void coordinateQueryTurnOff(void)
{
  coordinateQueryWait = false;

  if (infoMachineSettings.autoReportPos == 1)  // if auto report is enabled, turn it off
  {
    storeCmd("M154 S0\n");
    curQuerySeconds = 0;
  }
}

float coordinateGetAxis(AXIS axis)
{
  if (infoFile.source >= FS_ONBOARD_MEDIA)
    return coordinateGetAxisActual(axis);
  else
    return coordinateGetAxisTarget(axis);
}
