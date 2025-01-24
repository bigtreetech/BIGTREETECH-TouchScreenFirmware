#include "Fan.h"
#include "includes.h"

typedef struct
{
  uint8_t cur;
  uint8_t set;
} LASTFAN;

static uint8_t fan_index = 0;

void menuFan(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS fanItems = {
    // title
    LABEL_FAN,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_INC,                     LABEL_INC},
      {ICON_FAN ,                    LABEL_FAN},
      {ICON_FAN_FULL_SPEED,          LABEL_FULL},
      {ICON_STOP,                    LABEL_STOP},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  LASTFAN lastFan;

  fanSetSpeed(fan_index, fanGetCurSpeed(fan_index));
  lastFan = (LASTFAN) {fanGetCurSpeed(fan_index), fanGetSetSpeed(fan_index)};

  if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
  {
    fanItems.items[KEY_ICON_4].icon = ICON_FAN;
    fanItems.items[KEY_ICON_4].label.index = LABEL_FAN;
  }
  else
  {
    fanItems.items[KEY_ICON_4].icon = ICON_FAN_HALF_SPEED;
    fanItems.items[KEY_ICON_4].label.index = LABEL_HALF;
  }

  menuDrawPage(&fanItems);
  fanReDraw(fan_index, true);

  while (MENU_IS(menuFan))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        if (fanGetSetSpeed(fan_index) > 0)
        {
          if (infoSettings.fan_percentage == 1)
            fanSetPercent(fan_index, fanGetSetPercent(fan_index) - 1);
          else
            fanSetSpeed(fan_index, fanGetSetSpeed(fan_index) - 1);
        }
        break;

      case KEY_INFOBOX:
      {
        int16_t val;

        if (infoSettings.fan_percentage == 1)
        {
          val = editIntValue(0, 100, 0, fanGetSetPercent(fan_index));

          if (val != fanGetSetPercent(fan_index))
            fanSetPercent(fan_index, val);
        }
        else
        {
          val = editIntValue(0, infoSettings.fan_max[fan_index], 0, fanGetCurSpeed(fan_index));

          if (val != fanGetCurSpeed(fan_index))
            fanSetSpeed(fan_index, val);
        }

        lastFan.set = val;  // avoid unnecessary redraw of values

        fanReDraw(fan_index, true);
        break;
      }

      case KEY_ICON_3:
      case KEY_INCREASE:
        if (fanGetSetSpeed(fan_index) < infoSettings.fan_max[fan_index])
        {
          if (infoSettings.fan_percentage == 1)
            fanSetPercent(fan_index, fanGetSetPercent(fan_index) + 1);
          else
            fanSetSpeed(fan_index, fanGetSetSpeed(fan_index) + 1);
        }
        break;

      case KEY_ICON_4:
        if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
        {
          do
          {
            fan_index = (fan_index + 1) % MAX_FAN_COUNT;  // switch between fans
          } while (!fanIsValid(fan_index));

          fanSetSpeed(fan_index, fanGetCurSpeed(fan_index));  // set initial desired speed to actual speed
          lastFan = (LASTFAN) {fanGetCurSpeed(fan_index), fanGetSetSpeed(fan_index)};  // avoid unnecessary redraw of values

          fanReDraw(fan_index, true);
        }
        else
        {
          fanSetSpeed(fan_index, infoSettings.fan_max[fan_index] / 2);  // fan at half speed
        }
        break;

      case KEY_ICON_5:
        fanSetSpeed(fan_index, infoSettings.fan_max[fan_index]);  // fan at maximum speed
        break;

      case KEY_ICON_6:
        fanSetSpeed(fan_index, 0);  // stop fan
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if ((lastFan.cur != fanGetCurSpeed(fan_index)) || (lastFan.set != fanGetSetSpeed(fan_index)))
    { // refresh displayed values if actual or desired speed has changed
      lastFan = (LASTFAN) {fanGetCurSpeed(fan_index), fanGetSetSpeed(fan_index)};

      fanReDraw(fan_index, false);
    }

    loopProcess();
  }
}
