#include "Speed.h"
#include "includes.h"

typedef struct
{
  uint8_t cur;
  uint8_t set;
} LASTSPEED;

static const ITEM itemPercentType[SPEED_NUM] = {
  // icon                        label
  {ICON_MOVE,                    LABEL_PERCENTAGE_SPEED},
  {ICON_EXTRUDE,                 LABEL_PERCENTAGE_FLOW},
};

static const int16_t itemPercentTypeTitle[SPEED_NUM] = {
  LABEL_PERCENTAGE_SPEED,
  LABEL_PERCENTAGE_FLOW
};

static uint8_t item_index = 0;
static uint8_t percentSteps_index = 1;

#ifdef TFT70_V3_0

void setSpeedItemIndex(uint8_t index)
{
  item_index = index;
}

#endif

void menuSpeed(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS percentageItems = {
    // title
    LABEL_PERCENTAGE_SPEED,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_INC,                     LABEL_INC},
      {ICON_MOVE,                    LABEL_PERCENTAGE_SPEED},
      {ICON_E_5_PERCENT,             LABEL_5_PERCENT},
      {ICON_NORMAL_SPEED,            LABEL_NORMAL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  LASTSPEED lastSpeed;

  speedQuery();

  speedSetPercent(item_index, speedGetCurPercent(item_index));
  lastSpeed = (LASTSPEED) {speedGetCurPercent(item_index), speedGetSetPercent(item_index)};

  percentageItems.title.index = itemPercentTypeTitle[item_index];
  percentageItems.items[KEY_ICON_4] = itemPercentType[item_index];
  percentageItems.items[KEY_ICON_5] = itemPercent[percentSteps_index];

  menuDrawPage(&percentageItems);
  percentageReDraw(item_index, true);

  while (MENU_IS(menuSpeed))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        if (speedGetSetPercent(item_index) > SPEED_MIN)
          speedSetPercent(item_index, speedGetSetPercent(item_index) - percentSteps[percentSteps_index]);
        break;

      case KEY_INFOBOX:
      {
        uint16_t val = editIntValue(SPEED_MIN, SPEED_MAX, 100, speedGetCurPercent(item_index));

        if (val != speedGetSetPercent(item_index))
          speedSetPercent(item_index, val);

        percentageReDraw(item_index, true);
        break;
      }

      case KEY_ICON_3:
      case KEY_INCREASE:
        if (speedGetSetPercent(item_index) < SPEED_MAX)
          speedSetPercent(item_index, speedGetSetPercent(item_index) + percentSteps[percentSteps_index]);
        break;

      case KEY_ICON_4:
        if (infoSettings.ext_count > 0)
          item_index = (item_index + 1) % SPEED_NUM;

        percentageItems.title.index = itemPercentTypeTitle[item_index];
        percentageItems.items[key_num] = itemPercentType[item_index];

        menuDrawTitle();
        menuDrawItem(&percentageItems.items[key_num], key_num);
        percentageReDraw(item_index, true);
        break;

      case KEY_ICON_5:
        percentSteps_index = (percentSteps_index + 1) % ITEM_PERCENT_STEPS_NUM;
        percentageItems.items[key_num] = itemPercent[percentSteps_index];

        menuDrawItem(&percentageItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        speedSetPercent(item_index, 100);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if ((lastSpeed.cur != speedGetCurPercent(item_index)) || (lastSpeed.set != speedGetSetPercent(item_index)))
    {
      lastSpeed = (LASTSPEED) {speedGetCurPercent(item_index), speedGetSetPercent(item_index)};

      percentageReDraw(item_index, false);
    }

    loopProcess();
  }
}
