#include "CaseLight.h"
#include "includes.h"

static uint8_t caseLightPercent = 0;
static bool caseLightState;

static void updateCaseLightIcon(MENUITEMS * curmenu, const bool state)
{
  curmenu->items[KEY_ICON_5].icon = state ? ICON_RGB_WHITE : ICON_RGB_OFF;
  curmenu->items[KEY_ICON_5].label.index = state ? LABEL_ON : LABEL_OFF;
}

static void caseLightPercentReDraw(void)
{
  char tempstr[20];

  sprintf(tempstr, "  %d%%  ", caseLightPercent);
  setFontSize(FONT_SIZE_LARGE);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setFontSize(FONT_SIZE_NORMAL);
}

void caseLightSetPercent(uint8_t brightness)
{
  brightness = NOBEYOND(0, brightness, 255);
  caseLightPercent = (brightness * 100.0f) / 255 + 0.5f;
}

void caseLightSetState(const bool state)
{
  caseLightState = state;
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

  static uint8_t percent_index = 1;

  KEY_VALUES key_num = KEY_IDLE;
  uint8_t requestedCLpercent = caseLightPercent;
  uint8_t requestedCLstate = caseLightState;

  enum
  {
    SEND_NOTHING =     0B0000,
    DO_SEND_PERCENT =  0B0001,
    NOT_SEND_PERCENT = 0B1110,
    DO_SEND_STATE =    0B0010,
    NOT_SEND_STATE =   0B1101,
  } sendingNeeded = SEND_NOTHING;

  caseLightItems.items[KEY_ICON_4] = itemPercent[percent_index];
  updateCaseLightIcon(&caseLightItems, caseLightState);

  menuDrawPage(&caseLightItems);
  caseLightPercentReDraw();

  mustStoreCmd("M355\n");

  while (MENU_IS(menuCaseLight))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      // decrease case light
      case KEY_ICON_0:
      case KEY_DECREASE:
      // increase case light
      case KEY_ICON_3:
      case KEY_INCREASE:
        requestedCLpercent = (key_num == KEY_ICON_3 || key_num == KEY_INCREASE) ?
                             NOBEYOND(0, requestedCLpercent + percentSteps[percent_index], 100) :
                             NOBEYOND(0, requestedCLpercent - percentSteps[percent_index], 100);
        sendingNeeded |= DO_SEND_PERCENT;
        break;

      // change unit
      case KEY_ICON_4:
        percent_index = (percent_index + 1) % ITEM_PERCENT_STEPS_NUM;
        caseLightItems.items[key_num] = itemPercent[percent_index];

        menuDrawItem(&caseLightItems.items[key_num], key_num);
        break;

      // switch on/off case light
      case KEY_ICON_5:
        TOGGLE_BIT(requestedCLstate, 0);
        sendingNeeded |= DO_SEND_STATE;
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (requestedCLpercent != caseLightPercent)
    {
      if (sendingNeeded & DO_SEND_PERCENT)
        caseLightPercent = requestedCLpercent;
      else
        requestedCLpercent = caseLightPercent;

      caseLightPercentReDraw();
    }

    if (requestedCLstate != caseLightState)
    {
      if (sendingNeeded & DO_SEND_STATE)
        caseLightState = requestedCLstate;
      else
        requestedCLstate = caseLightState;

      updateCaseLightIcon(&caseLightItems, caseLightState);

      menuDrawItem(&caseLightItems.items[KEY_ICON_5], KEY_ICON_5);
    }

    if (sendingNeeded & DO_SEND_PERCENT)
    {
      if (storeCmd("M355 P%d\n", requestedCLpercent * 255U / 100))
        sendingNeeded &= NOT_SEND_PERCENT;
    }

    if (sendingNeeded & DO_SEND_STATE)
    {
      if (storeCmd("M355 S%d\n", requestedCLstate))
        sendingNeeded &= NOT_SEND_STATE;
    }

    loopProcess();
  }
}
