#include "CaseLight.h"
#include "includes.h"

#define CASE_LIGHT_UPDATE_TIME 1000  // 1 seconds is 1000

// Icons list for Case Light
const ITEM itemCaseLight[2] = {
  // icon                        label
  {ICON_RGB_OFF,                 LABEL_OFF},
  {ICON_RGB_WHITE,               LABEL_ON},
};

static uint8_t curUnit_index = 1;

static inline void updateCaseLightIcon(MENUITEMS * curmenu, bool state)
{
  curmenu->items[KEY_ICON_5] = itemCaseLight[state ? 1 : 0];
}

void caseLightBrightnessReDraw()
{
  char tempstr[20];

  sprintf(tempstr, "  %d%%  ", caseLightGetBrightnessPercent());
  setFontSize(FONT_SIZE_LARGE);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setFontSize(FONT_SIZE_NORMAL);
}

void menuCaseLight(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS caseLightItems = {
    // title
    LABEL_CASE_LIGHT,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_INC,                     LABEL_INC},
      {ICON_E_5_PERCENT,             LABEL_5_PERCENT},
      {ICON_RGB_WHITE,               LABEL_ON},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  bool currentCaseLightState = caseLightGetState();
  bool previousCaseLightState = currentCaseLightState;
  uint8_t currentCaseLightBrightness = caseLightGetBrightness();
  uint8_t previousCaseLightBrightness = currentCaseLightBrightness;
  bool sendingNeeded = false;

  storeCmd("M355\n");  // retrieve the case light's state and brightness. Value will be compared in while loop

  caseLightItems.items[KEY_ICON_4] = itemPercent[curUnit_index];
  updateCaseLightIcon(&caseLightItems, currentCaseLightState);

  menuDrawPage(&caseLightItems);
  caseLightBrightnessReDraw();

  while (MENU_IS(menuCaseLight))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      // decrease case light
      case KEY_ICON_0:
      case KEY_DECREASE:
        caseLightSetBrightnessPercent(- percentSteps[curUnit_index]);
        break;

      // increase case light
      case KEY_ICON_3:
      case KEY_INCREASE:
        caseLightSetBrightnessPercent(+ percentSteps[curUnit_index]);
        break;

      // change unit
      case KEY_ICON_4:
        curUnit_index = (curUnit_index + 1) % ITEM_PERCENT_STEPS_NUM;

        caseLightItems.items[key_num] = itemPercent[curUnit_index];
        menuDrawItem(&caseLightItems.items[key_num], key_num);
        break;

      // switch on/off case light
      case KEY_ICON_5:
        caseLightSetState(!currentCaseLightState);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    currentCaseLightState = caseLightGetState();
    if (previousCaseLightState != currentCaseLightState)  // dynamically change the light on/off icon based on the current state
    {
      previousCaseLightState = currentCaseLightState;

      updateCaseLightIcon(&caseLightItems, currentCaseLightState);
      menuDrawItem(&caseLightItems.items[KEY_ICON_5], KEY_ICON_5);

      sendingNeeded = true;
    }

    currentCaseLightBrightness = caseLightGetBrightness();
    if (previousCaseLightBrightness != currentCaseLightBrightness)
    {
      previousCaseLightBrightness = currentCaseLightBrightness;

      caseLightBrightnessReDraw();

      sendingNeeded = true;
    }

    if (sendingNeeded && nextScreenUpdate(CASE_LIGHT_UPDATE_TIME))
    {
      storeCmd("M355 S%d P%d\n", currentCaseLightState ? 1 : 0, currentCaseLightBrightness);

      sendingNeeded = false;
    }

    loopProcess();
  }
}
