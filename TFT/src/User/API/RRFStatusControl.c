#include "RRFStatusControl.h"
#include "includes.h"

#define RRF_NORMAL_STATUS_QUERY_MS 1000
#define RRF_FAST_STATUS_QUERY_MS 500

// available status: status:
// I=idle, P=printing from SD card, S=stopped (i.e. needs a reset), C=running config file (i.e starting up),
// A=paused, D=pausing, R=resuming from a pause, B=busy (e.g. running a macro), F=performing firmware update
static char rrf_status = 'I';

static uint16_t rrf_query_interval = RRF_NORMAL_STATUS_QUERY_MS;
static bool macro_busy = false;

void rrfStatusSet(char status)
{
  rrf_status = status;
  if (status != 'B')
  {
    macro_busy = false;
  }
}

inline void rrfStatusSetBusy(void)
{
  rrf_status = 'B';
}

inline bool rrfStatusIsBusy(void)
{
  return rrf_status == 'B';
}

inline bool rrfStatusIsMacroBusy(void)
{
  return macro_busy;
}

void rrfStatusSetMacroBusy(void)
{
  macro_busy = true;
  rrfStatusSetBusy();
}

inline void rrfStatusQueryFast(void)
{
  rrf_query_interval = RRF_FAST_STATUS_QUERY_MS;
}

inline void rrfStatusQueryNormal(void)
{
  rrf_query_interval = RRF_NORMAL_STATUS_QUERY_MS;
}

void rrfStatusQuery(void)
{
  if (!infoHost.connected)
    return;

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    static uint32_t rrf_next_query_time = 0;

    // don't send status queries while in the terminal menu to avoid flooding the console
    if (OS_GetTimeMs() > rrf_next_query_time && infoMenu.menu[infoMenu.cur] != menuTerminal)
    {
      rrf_next_query_time = OS_GetTimeMs() + rrf_query_interval;
      storeCmd("M408 S0\n");
    }
  }
}