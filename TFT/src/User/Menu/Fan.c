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

  fanSetTargetSpeed(fan_index, fanGetCurrentSpeed(fan_index));
  lastFan = (LASTFAN) {fanGetCurrentSpeed(fan_index), fanGetTargetSpeed(fan_index)};

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
        if (fanGetTargetSpeed(fan_index) > 0)
        {
          if (infoSettings.fan_percentage == 1)
            fanSetTargetPercent(fan_index, fanGetTargetPercent(fan_index) - 1);
          else
            fanSetTargetSpeed(fan_index, fanGetTargetSpeed(fan_index) - 1);
        }
        break;

      case KEY_INFOBOX:
      {
        int16_t val;

        if (infoSettings.fan_percentage == 1)
        {
          val = editIntValue(0, 100, 0, fanGetTargetPercent(fan_index));

          if (val != fanGetTargetPercent(fan_index))
            fanSetTargetPercent(fan_index, val);
        }
        else
        {
          val = editIntValue(0, infoSettings.fan_max[fan_index], 0, fanGetCurrentSpeed(fan_index));

          if (val != fanGetCurrentSpeed(fan_index))
            fanSetTargetSpeed(fan_index, val);
        }

        lastFan.set = val;  // avoid unnecessary redraw of values

        fanReDraw(fan_index, true);
        break;
      }

      case KEY_ICON_3:
      case KEY_INCREASE:
        if (fanGetTargetSpeed(fan_index) < infoSettings.fan_max[fan_index])
        {
          if (infoSettings.fan_percentage == 1)
            fanSetTargetPercent(fan_index, fanGetTargetPercent(fan_index) + 1);
          else
            fanSetTargetSpeed(fan_index, fanGetTargetSpeed(fan_index) + 1);
        }
        break;

      case KEY_ICON_4:
        if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
        {
          do
          {
            fan_index = (fan_index + 1) % MAX_FAN_COUNT;  // switch between fans
          } while (!fanIsValid(fan_index));

          fanSetTargetSpeed(fan_index, fanGetCurrentSpeed(fan_index));  // set initial desired speed to actual speed
          lastFan = (LASTFAN) {fanGetCurrentSpeed(fan_index), fanGetTargetSpeed(fan_index)};  // avoid unnecessary redraw of values

          fanReDraw(fan_index, true);
        }
        else
        {
          fanSetTargetSpeed(fan_index, infoSettings.fan_max[fan_index] / 2);  // fan at half speed
        }
        break;

      case KEY_ICON_5:
        fanSetTargetSpeed(fan_index, infoSettings.fan_max[fan_index]);  // fan at maximum speed
        break;

      case KEY_ICON_6:
        fanSetTargetSpeed(fan_index, 0);  // stop fan
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if ((lastFan.cur != fanGetCurrentSpeed(fan_index)) || (lastFan.set != fanGetTargetSpeed(fan_index)))
    { // refresh displayed values if actual or desired speed has changed
      lastFan = (LASTFAN) {fanGetCurrentSpeed(fan_index), fanGetTargetSpeed(fan_index)};

      fanReDraw(fan_index, false);
    }

    loopProcess();
  }
}
