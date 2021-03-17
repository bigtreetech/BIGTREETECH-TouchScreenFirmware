#include "Fan.h"
#include "includes.h"

static uint8_t curIndex = 0;

const ITEM itemFan[2] = {
  // icon                        label
  {ICON_FAN,                     LABEL_FAN},
  {ICON_FAN_HALF_SPEED,          LABEL_HALF},
};

void fanSpeedReDraw(bool skip_header)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skip_header)
  {
    sprintf(tempstr, "%-15s", fanID[curIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tempstr);
    setLargeFont(true);

    if (infoSettings.fan_percentage == 1)
    {
      GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"%");
    }
    else
    {
      GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"PWM");
    }
  }

  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "  %d/%d  ", fanGetCurPercent(curIndex), fanGetSetPercent(curIndex));
  else
    sprintf(tempstr, "  %d/%d  ", (int)fanGetCurSpeed(curIndex), (int)fanGetSetSpeed(curIndex));

  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}

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

  LASTFAN lastFan;
  fanSetSpeed(curIndex, fanGetCurSpeed(curIndex));
  lastFan = (LASTFAN) {fanGetCurSpeed(curIndex), fanGetSetSpeed(curIndex)};

  if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    fanItems.items[KEY_ICON_4] = itemFan[0];
  else
    fanItems.items[KEY_ICON_4] = itemFan[1];

  menuDrawPage(&fanItems);
  fanSpeedReDraw(false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuFan)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (fanGetSetSpeed(curIndex) > 0)
        {
          if (infoSettings.fan_percentage == 1)
            fanSetPercent(curIndex, fanGetSetPercent(curIndex) - 1);
          else
            fanSetSpeed(curIndex, fanGetSetSpeed(curIndex) - 1);
        }
        break;

      case KEY_INFOBOX:
      {
        char titlestr[30];
        if (infoSettings.fan_percentage == 1)
        {
          strcpy(titlestr, "Min:0 | Max:100");
          uint8_t val = numPadInt((uint8_t *) titlestr, fanGetSetPercent(curIndex), 0, false);
          val = NOBEYOND(0, val, 100);

          if (val != fanGetSetPercent(curIndex))
            fanSetPercent(curIndex, val);
        }
        else
        {
          sprintf(titlestr, "Min:0 | Max:%d", infoSettings.fan_max[curIndex]);
          uint8_t val = numPadInt((uint8_t *) titlestr, fanGetCurSpeed(curIndex), 0, false);
          val = NOBEYOND(0, val,  infoSettings.fan_max[curIndex]);

          if (val != fanGetCurSpeed(curIndex))
            fanSetSpeed(curIndex, val);
        }

        menuDrawPage(&fanItems);
        fanSpeedReDraw(false);
        break;
      }

      case KEY_ICON_3:
        if (fanGetSetSpeed(curIndex) < infoSettings.fan_max[curIndex])
        {
          if (infoSettings.fan_percentage == 1)
            fanSetPercent(curIndex, fanGetSetPercent(curIndex) + 1);
          else
            fanSetSpeed(curIndex, fanGetSetSpeed(curIndex) + 1);
        }
        break;

      case KEY_ICON_4:
        if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
        {
          curIndex = (curIndex + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
          fanSpeedReDraw(false);
        }
        else
        {
          fanSetSpeed(curIndex, infoSettings.fan_max[curIndex] / 2);  // 50%
          fanSpeedReDraw(true);
        }
        break;

      case KEY_ICON_5:
        fanSetSpeed(curIndex, infoSettings.fan_max[curIndex]);
        break;

      case KEY_ICON_6:
        fanSetSpeed(curIndex, 0);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (fanGetSetSpeed(curIndex) < infoSettings.fan_max[curIndex] && encoderPosition > 0)
            {
              if (infoSettings.fan_percentage == 1)
                fanSetPercent(curIndex, fanGetSetPercent(curIndex) + 1);
              else
                fanSetSpeed(curIndex, fanGetSetSpeed(curIndex) + 1);
            }

            if (fanGetSetSpeed(curIndex) > 0 && encoderPosition < 0)
            {
              if (infoSettings.fan_percentage == 1)
                fanSetPercent(curIndex, fanGetSetPercent(curIndex) - 1);
              else
                fanSetSpeed(curIndex, fanGetSetSpeed(curIndex) - 1);
            }
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if ((lastFan.cur != fanGetCurSpeed(curIndex)) || (lastFan.set != fanGetSetSpeed(curIndex)))
    {
      lastFan = (LASTFAN) {fanGetCurSpeed(curIndex), fanGetSetSpeed(curIndex)};
      fanSpeedReDraw(true);
    }

    loopProcess();
  }
}
