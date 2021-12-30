#include "Fan.h"
#include "includes.h"

const ITEM itemFan[2] = {
  // icon                        label
  {ICON_FAN,                     LABEL_FAN},
  {ICON_FAN_HALF_SPEED,          LABEL_HALF},
};

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
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_FAN ,                    LABEL_FAN},
      {ICON_FAN_FULL_SPEED,          LABEL_FULL},
      {ICON_STOP,                    LABEL_STOP},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  MENUITEMS laserItems = {
  // title
  LABEL_LASER,
  // icon                       label
   {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_LASER_2 ,             LABEL_LASER_2},
    {ICON_LASER_100,            LABEL_LASER_100},
    {ICON_LASER_OFF,            LABEL_LASER_OFF},
    {ICON_BACK,                 LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  LASTFAN lastFan;

  fanSetSpeed(fan_index, fanGetCurSpeed(fan_index));
  lastFan = (LASTFAN) {fanGetCurSpeed(fan_index), fanGetSetSpeed(fan_index)};

  if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
    fanItems.items[KEY_ICON_4] = itemFan[0];
  else
    fanItems.items[KEY_ICON_4] = itemFan[1];

<<<<<<< HEAD
  if (infoSettings.laser_mode != 1)
  {
    menuDrawPage(&fanItems);
  }
  else
  {
    menuDrawPage(&laserItems);
  }
  fanSpeedReDraw(false);
=======
  menuDrawPage(&fanItems);
  fanReDraw(fan_index, false);
>>>>>>> btt_main/master

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

        fanReDraw(fan_index, false);
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
<<<<<<< HEAD
        if (infoSettings.laser_mode == 1)
        {
          fanSetSpeedPercent(curIndex, 2);
        }
        else
        {
          if (infoSettings.fan_count > 1)
          {
            curIndex = (curIndex + 1) % infoSettings.fan_count;
            fanSpeedReDraw(false);
          }
          else
          {
            fanSetSpeed(curIndex, (infoSettings.fan_max[curIndex] + 1) / 2);
          }
=======
        if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
        {
          do
          {
            fan_index = (fan_index + 1) % MAX_FAN_COUNT;
          } while (!fanIsValid(fan_index));

          fanReDraw(fan_index, false);
        }
        else
        {
          fanSetSpeed(fan_index, infoSettings.fan_max[fan_index] / 2);  // 50%
          fanReDraw(fan_index, true);
>>>>>>> btt_main/master
        }
        break;

      case KEY_ICON_5:
        fanSetSpeed(fan_index, infoSettings.fan_max[fan_index]);
        break;

      case KEY_ICON_6:
        fanSetSpeed(fan_index, 0);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if ((lastFan.cur != fanGetCurSpeed(fan_index)) || (lastFan.set != fanGetSetSpeed(fan_index)))
    {
      lastFan = (LASTFAN) {fanGetCurSpeed(fan_index), fanGetSetSpeed(fan_index)};

      fanReDraw(fan_index, true);
    }

    loopProcess();
  }
}
