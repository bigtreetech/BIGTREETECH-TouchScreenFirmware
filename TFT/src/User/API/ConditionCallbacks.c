#include "ConditionCallbacks.h"
#include "interfaceCmd.h"
#include "gcode.h"
#include "includes.h"

bool fullQueueConditionCallback(void)
{
  return (infoCmd.count >= CMD_MAX_LIST);
}

bool usedQueueConditionCallback(void)
{
  return (infoCmd.count || infoHost.wait);
}

bool noResponseConditionCallback(void)
{
  return (!requestCommandInfo.done);
}

bool hostPrintingConditionCallback(void)
{
  return (infoHost.printing);
}
