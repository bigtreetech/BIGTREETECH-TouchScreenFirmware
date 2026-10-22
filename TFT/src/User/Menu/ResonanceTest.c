#include "ResonanceTest.h"
#include "includes.h"

// Marlin M495 limits
#define RT_MIN_FREQ     5.0f
#define RT_MAX_FREQ     200.0f
#define RT_MIN_OCTAVE   20.0f
#define RT_MAX_OCTAVE   60.0f
#define RT_MIN_AMP_CORR 1
#define RT_MAX_AMP_CORR 8
#define RT_MAX_TIMELINE 100.0f
#define RT_MAX_ACCEL_Z  15.0f

typedef enum
{
  RT_AXIS = 0,
  RT_START_FREQ,
  RT_END_FREQ,
  RT_ACCEL_PER_HZ,
  RT_OCTAVE,
  RT_AMP_CORR,
  RT_TIMELINE,
  RT_START,
  RT_ABORT,
  RT_ITEM_COUNT
} RT_ITEM;

static const char * const axisName[] = {"X", "Y", "Z"};

static const char * const itemTitle[RT_ITEM_COUNT] = {
  "Axis",
  "Start freq. (Hz)",
  "End freq. (Hz)",
  "Accel/Hz",
  "Octave time (s)",
  "Amplitude corr.",
  "Resonance freq. @ time",
  NULL,
  NULL,
};

// values not set by the user (0) are not sent, so Marlin defaults / last used values are kept
static uint8_t rtAxis = 0;
static float rtStartFreq = 0;
static float rtEndFreq = 0;
static float rtAccelPerHz = 0;
static float rtOctave = 0;
static uint8_t rtAmpCorr = 0;

bool hasResonanceTest(void)
{
  return infoMachineSettings.firmwareType == FW_MARLIN &&
         (getEnabledElementCount(P_FT_MOTION) > 0 || getEnabledElementCount(P_INPUT_SHAPING) > 0);
}

static void startResonanceTest(void)
{
  char cmd[80];
  int n = snprintf(cmd, sizeof(cmd), "M495 %s", axisName[rtAxis]);

  if (rtAccelPerHz > 0) n += snprintf(cmd + n, sizeof(cmd) - n, " A%.1f", (rtAxis == 2) ? MIN(rtAccelPerHz, RT_MAX_ACCEL_Z) : rtAccelPerHz);
  if (rtStartFreq > 0)  n += snprintf(cmd + n, sizeof(cmd) - n, " F%.1f", rtStartFreq);
  if (rtEndFreq > 0)    n += snprintf(cmd + n, sizeof(cmd) - n, " T%.1f", rtEndFreq);
  if (rtOctave > 0)     n += snprintf(cmd + n, sizeof(cmd) - n, " O%.1f", rtOctave);
  if (rtAmpCorr > 0)    n += snprintf(cmd + n, sizeof(cmd) - n, " C%u", rtAmpCorr);

  snprintf(cmd + n, sizeof(cmd) - n, " S\n");
  mustStoreCmd("%s", cmd);
}

static void loadItems(LISTITEM * item, uint16_t index, uint8_t itemPos)
{
  if (index >= RT_ITEM_COUNT)
  {
    item->icon = CHARICON_NULL;
    return;
  }

  switch (index)
  {
    case RT_START:
      item->icon = CHARICON_PLAY;
      item->itemType = LIST_LABEL;
      item->titlelabel.index = LABEL_START;
      item->valueLabel.index = LABEL_NULL;
      return;

    case RT_ABORT:
      item->icon = CHARICON_STOP;
      item->itemType = LIST_LABEL;
      item->titlelabel.index = LABEL_STOP;
      item->valueLabel.index = LABEL_NULL;
      return;
  }

  item->icon = CHARICON_EDIT;
  item->itemType = LIST_CUSTOMVALUE;
  item->titlelabel.index = LABEL_DYNAMIC;
  item->titlelabel.address = (char *) itemTitle[index];

  switch (index)
  {
    case RT_AXIS:
      item->valueLabel.index = LABEL_DYNAMIC;
      setDynamicTextValue(itemPos, axisName[rtAxis]);
      return;

    case RT_START_FREQ:
      setDynamicValue(itemPos, rtStartFreq);
      break;

    case RT_END_FREQ:
      setDynamicValue(itemPos, rtEndFreq);
      break;

    case RT_ACCEL_PER_HZ:
      setDynamicValue(itemPos, rtAccelPerHz);
      break;

    case RT_OCTAVE:
      setDynamicValue(itemPos, rtOctave);
      break;

    case RT_AMP_CORR:
      setDynamicValue(itemPos, rtAmpCorr);
      break;

    default:  // RT_TIMELINE (one shot action, no value to display)
      setDynamicValue(itemPos, 0);
      break;
  }

  item->valueLabel.index = LABEL_CUSTOM_VALUE;
}

static float clampFloat(float val, float min, float max)
{
  return (val < min) ? min : ((val > max) ? max : val);
}

// edit a value with the numpad. If cancelled (value unchanged), the value is left untouched (0 = not set by the user)
static void editFloat(float * val, float defaultVal, float min, float max)
{
  float newVal = numPadFloat(NULL, *val, defaultVal, false);

  if (newVal != *val)
    *val = clampFloat(newVal, min, max);
}

void menuResonanceTest(void)
{
  LABEL title = {LABEL_RESONANCE_TEST};
  uint16_t curIndex = KEY_IDLE;

  listViewCreate(title, NULL, RT_ITEM_COUNT, NULL, false, NULL, loadItems);

  while (MENU_IS(menuResonanceTest))
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case KEY_BACK:
        CLOSE_MENU();
        break;

      case RT_AXIS:
        rtAxis = (rtAxis + 1) % COUNT(axisName);
        listViewRefreshItem(curIndex);
        break;

      case RT_START_FREQ:
        editFloat(&rtStartFreq, RT_MIN_FREQ, RT_MIN_FREQ, RT_MAX_FREQ - 1.0f);
        if (rtEndFreq > 0 && rtEndFreq <= rtStartFreq) rtEndFreq = 0;  // End frequency must be greater than Start frequency
        listViewRefreshMenu();
        break;

      case RT_END_FREQ:
        editFloat(&rtEndFreq, 100.0f, RT_MIN_FREQ + 1.0f, RT_MAX_FREQ);
        if (rtEndFreq > 0 && rtEndFreq <= rtStartFreq) rtEndFreq = rtStartFreq + 1.0f;  // still within RT_MAX_FREQ
        listViewRefreshMenu();
        break;

      case RT_ACCEL_PER_HZ:
        editFloat(&rtAccelPerHz, 60.0f, 1.0f, 1000.0f);  // Z axis limit is applied when the test is started
        listViewRefreshItem(curIndex);
        break;

      case RT_OCTAVE:
        editFloat(&rtOctave, RT_MIN_OCTAVE, RT_MIN_OCTAVE, RT_MAX_OCTAVE);
        listViewRefreshItem(curIndex);
        break;

      case RT_AMP_CORR:
      {
        int32_t newVal = numPadInt(NULL, rtAmpCorr, 5, false);

        if (newVal != rtAmpCorr)
          rtAmpCorr = NOBEYOND(RT_MIN_AMP_CORR, newVal, RT_MAX_AMP_CORR);

        listViewRefreshItem(curIndex);
        break;
      }

      case RT_TIMELINE:  // ask Marlin to convert a timeline position of the test to the resonance frequency
      {
        float timeline = numPadFloat(NULL, 0, 0, false);

        if (timeline > 0)
          mustStoreCmd("M495 G%.1f\n", clampFloat(timeline, 0, RT_MAX_TIMELINE));

        listViewRefreshMenu();
        break;
      }

      case RT_START:
        startResonanceTest();
        break;

      case RT_ABORT:
        sendEmergencyCmd("M496\n");
        break;

      default:
        break;
    }

    loopProcess();
  }
}
