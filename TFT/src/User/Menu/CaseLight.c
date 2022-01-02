#include "CaseLight.h"
#include "includes.h"

const ITEM itemCaseLight[2] = {
  // icon                        label
  {ICON_RGB_OFF,                 LABEL_OFF},
  {ICON_RGB_WHITE,               LABEL_ON},
};

static inline void updateCaseLightIcon(MENUITEMS * curmenu, bool state)
{
  curmenu->items[KEY_ICON_4] = itemCaseLight[state ? 1 : 0];
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
      {ICON_RGB_WHITE,               LABEL_ON},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  // Initiate query. Value will be compared in while loop
  caseLightValueQuery();

  bool currentCaseLightState = caseLightGetState();
  bool previousCaseLightState = currentCaseLightState;
  uint8_t currentCaseLightBrightness = caseLightGetBrightness();
  uint8_t previousCaseLightBrightness = currentCaseLightBrightness;

  updateCaseLightIcon(&caseLightItems, currentCaseLightState);
  menuDrawPage(&caseLightItems);
  caseLightBrightnessReDraw();

  while (MENU_IS(menuCaseLight))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
        caseLightChangeBrightnessPrecent(-10);
        caseLightBrightnessReDraw();
        break;

      case KEY_ICON_3:
        caseLightChangeBrightnessPrecent(10);
        caseLightBrightnessReDraw();
        break;

      case KEY_ICON_4:
        caseLightSetState(!currentCaseLightState);
        caseLightBrightnessReDraw();
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            caseLightChangeBrightnessPrecent(encoderPosition);
            caseLightBrightnessReDraw();
            encoderPosition = 0;
          }
        #endif
        break;
    }

    currentCaseLightState = caseLightGetState();
    if (previousCaseLightState != currentCaseLightState)
    {
      // Dynamically change the light on/off icon based on the current state
      previousCaseLightState = currentCaseLightState;
      updateCaseLightIcon(&caseLightItems, currentCaseLightState);
      menuDrawItem(&caseLightItems.items[KEY_ICON_4], KEY_ICON_4);
      caseLightBrightnessReDraw();
    }

    currentCaseLightBrightness = caseLightGetBrightness();
    if (previousCaseLightBrightness != currentCaseLightBrightness)
    {
      previousCaseLightBrightness = currentCaseLightBrightness;
      caseLightBrightnessReDraw();
    }

    loopProcess();
  }
}
