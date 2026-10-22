#include "Tuning.h"
#include "includes.h"

typedef enum
{
  TUNE_MPC = 0,
  TUNE_PID,
  TUNE_EXTRUDER,
  TUNE_H_OFFSET,
  TUNE_RESONANCE,
  #ifdef DEBUG_MONITORING
    TUNE_MONITORING,
  #endif
  TUNE_COUNT
} TUNE_ITEM;

static const ITEM tuneItems[TUNE_COUNT] = {
  // icon                          label
  {ICON_MPC_PID,                   LABEL_MPC},
  {ICON_MPC_PID,                   LABEL_PID},
  {ICON_TUNE_EXTRUDER,             LABEL_TUNE_EXTRUDER},
  {ICON_PROBE_OFFSET,              LABEL_H_OFFSET},
  {ICON_PARAMETER,                 LABEL_RESONANCE_TEST},
  #ifdef DEBUG_MONITORING
    {ICON_FAST_SPEED,              LABEL_TEST},
  #endif
};

// check if an item must be displayed
static bool isTuneItemEnabled(TUNE_ITEM item)
{
  switch (item)
  {
    case TUNE_MPC:
      return hasMPC();

    case TUNE_PID:  // PID is replaced by MPC for the bed if the bed is not available
      return !hasMPC() || infoSettings.bed_en;

    case TUNE_H_OFFSET:
      return DELTA_PROBE_TYPE == 0;  // not available on Delta printers

    case TUNE_RESONANCE:
      return hasResonanceTest();

    default:
      return true;
  }
}

void menuTuning(void)
{
  MENUITEMS TuningItems = {
    // title
    LABEL_TUNING,
    // icon                          label
    {
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  TUNE_ITEM keyAction[7];
  KEY_VALUES key_num = KEY_IDLE;
  uint8_t slots = 0;

  // fill the enabled items sequentially, so no gap is left between them
  for (uint8_t i = 0; i < TUNE_COUNT && slots < 7; i++)
  {
    if (isTuneItemEnabled(i))
    {
      TuningItems.items[slots] = tuneItems[i];
      keyAction[slots++] = i;
    }
  }

  menuDrawPage(&TuningItems);

  while (MENU_IS(menuTuning))
  {
    key_num = menuKeyGetValue();

    if (key_num == KEY_ICON_7)
    {
      CLOSE_MENU();
    }
    else if (key_num < slots)
    {
      switch (keyAction[key_num])
      {
        case TUNE_MPC:
          OPEN_MENU(menuMPC);
          break;

        case TUNE_PID:
          OPEN_MENU(menuPid);
          break;

        case TUNE_EXTRUDER:
          OPEN_MENU(menuTuneExtruder);
          break;

        case TUNE_H_OFFSET:
          storeCmd("M206\n");

          zOffsetSetMenu(false);  // use Home Offset menu
          OPEN_MENU(menuZOffset);
          break;

        case TUNE_RESONANCE:
          OPEN_MENU(menuResonanceTest);
          break;

        #ifdef DEBUG_MONITORING
          case TUNE_MONITORING:
            monitoringSetMenu(true);  // use Stress Test Monitoring menu
            OPEN_MENU(menuMonitoring);
            break;
        #endif

        default:
          break;
      }
    }

    loopProcess();
  }
}
