#include "CaseLight.h"
#include "includes.h"

const ITEM itemCaseLight[2] = {
    //icon                label
    {ICON_RGB_OFF,        LABEL_OFF},
    {ICON_RGB_WHITE,      LABEL_ON},
};

void caseLightBrightnessReDraw()
{
  char tempstr[20];

  sprintf(tempstr, "  %d%%  ", caseLightGetBrightnessPercent());

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void menuCaseLight(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS caseLightItems = {
      // title
      LABEL_CASE_LIGHT,
      {
      //  icon                      label
          {ICON_DEC,                LABEL_DEC},
          {ICON_BACKGROUND,         LABEL_BACKGROUND},
          {ICON_BACKGROUND,         LABEL_BACKGROUND},
          {ICON_INC,                LABEL_INC},
          {ICON_RGB_WHITE,          LABEL_ON},
          {ICON_BACKGROUND,         LABEL_BACKGROUND},
          {ICON_BACKGROUND,         LABEL_BACKGROUND},
          {ICON_BACK,               LABEL_BACK},
      }};

  KEY_VALUES key_num = KEY_IDLE;

  // Initiate query. Value will be compared in while loop
  caseLightValueQuery();

  bool currentCaseLightState = caseLightGetState();
  bool previousCaseLightState = currentCaseLightState;
  uint8_t currentCaseLightBrightness = caseLightGetBrightness();
  uint8_t previousCaseLightBrightness = currentCaseLightBrightness;

#define SET_CASE_LIGHT_ICON() caseLightItems.items[KEY_ICON_4] = itemCaseLight[currentCaseLightState ? 1 : 0];

  SET_CASE_LIGHT_ICON();
  menuDrawPage(&caseLightItems);
  caseLightBrightnessReDraw();

  while (infoMenu.menu[infoMenu.cur] == menuCaseLight)
  {
    key_num = menuKeyGetValue();
    currentCaseLightState = caseLightGetState();
    if (previousCaseLightState != currentCaseLightState)
    {
      // Dynamically change the light on/off icon based on the current state
      previousCaseLightState = currentCaseLightState;
      SET_CASE_LIGHT_ICON();
      menuDrawPage(&caseLightItems);
      caseLightBrightnessReDraw();
    }

    currentCaseLightBrightness = caseLightGetBrightness();
    if (previousCaseLightBrightness != currentCaseLightBrightness)
    {
      previousCaseLightBrightness = currentCaseLightBrightness;
      caseLightBrightnessReDraw();
    }

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
      caseLightToggleState();
      menuDrawPage(&caseLightItems);
      caseLightBrightnessReDraw();
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
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

    loopProcess();
  }
}
