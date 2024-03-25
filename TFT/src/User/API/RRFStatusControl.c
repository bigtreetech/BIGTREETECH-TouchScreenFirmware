#include "RRFStatusControl.h"
#include "includes.h"

#define RRF_NORMAL_STATUS_QUERY_MS 1000
#define RRF_FAST_STATUS_QUERY_MS   500

// available status: status:
// I=idle, P=printing from SD card, S=stopped (i.e. needs a reset), C=running config file (i.e starting up),
// A=paused, D=pausing, R=resuming from a pause, B=busy (e.g. running a macro), F=performing firmware update
static char rrf_status = 'I';
static bool was_printing = false;

static uint16_t rrf_query_interval = RRF_NORMAL_STATUS_QUERY_MS;
static bool macro_busy = false;

bool starting_print = false;

void rrfStatusSet(char status)
{
  if (rrf_status != status)
  {
    switch (status)
    {
      case 'R':
      case 'P':
        switch (rrf_status)
        {
          case 'D':
          case 'A':
            setHostDialog(false);
            setPrintResume(HOST_STATUS_RESUMING);
            break;

          case 'I':
            // rrfParseAck will take care of going to the print screen
            mustStoreCmd("M409 K\"job.file.fileName\"\n");
            starting_print = true;
            break;
        }
        break;

      case 'I':
        switch (rrf_status)
        {
          case 'P':
          case 'R':
          case 'A':
          case 'D':
            setPrintAbort();  // done is the same as abort
            break;

          case 'B':
            if (was_printing)
              setPrintAbort();
            break;
        }
        break;

      case 'D':
      case 'A':
        if (rrf_status == 'P')
          setPrintPause(HOST_STATUS_PAUSING, PAUSE_EXTERNAL);
        break;

      case 'B':
        switch (rrf_status)
        {
          case 'P':
          case 'R':
          case 'A':
          case 'D':
            was_printing = true;
            break;

          default:
            was_printing = false;
            break;
        }
        break;
    }
  }

  rrf_status = status;

  if (status != 'B')
    macro_busy = false;
}

inline bool rrfStatusIsBusy(void)
{
  return rrf_status == 'B';
}

inline void rrfStatusSetBusy(void)
{
  rrf_status = 'B';
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
  if (infoHost.connected)
  {
    static uint32_t rrf_next_query_time = 0;

    if (OS_GetTimeMs() < rrf_next_query_time)
      return;

    rrf_next_query_time = OS_GetTimeMs() + rrf_query_interval;

    // don't send status queries while in the terminal menu to avoid flooding the console
    if (MENU_IS(menuTerminal))
      return;

    storeCmd("M408 S0\n");
  }
}
