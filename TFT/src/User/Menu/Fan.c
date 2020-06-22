#include "Fan.h"
#include "includes.h"

static u8   curIndex = 0;

const ITEM itemFan[2] = {
  //icon                label
  {ICON_FAN,            LABEL_FAN},
  {ICON_FAN_HALF_SPEED, LABEL_FAN_HALF_SPEED},
  };

void fanSpeedReDraw(void)
{
  setLargeFont(true);
  char fan_s[15];
  if(infoSettings.fan_percentage == 1)
  {
    sprintf(fan_s, "%s: % 4d%%", fanID[curIndex], fanGetSpeedPercent(curIndex));
  }
  else
  {
    sprintf(fan_s, "%s: % 4d", fanID[curIndex], (int)fanGetSpeed(curIndex));
  }
  GUI_DispStringInPrect(&exhibitRect, (u8*)fan_s);
  setLargeFont(false);
}

void menuFan(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS fanItems = {
  // title
  LABEL_FAN,
  // icon                       label
   {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_FAN ,                 LABEL_FAN},
    {ICON_FAN_FULL_SPEED,       LABEL_FAN_FULL_SPEED},
    {ICON_STOP,                 LABEL_STOP},
    {ICON_BACK,                 LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  u8 curspeed;

  if (infoSettings.fan_count > 1)
    fanItems.items[KEY_ICON_4] = itemFan[0];
  else
    fanItems.items[KEY_ICON_4] = itemFan[1];

  menuDrawPage(&fanItems);
  fanSpeedReDraw();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuFan)
  {
    key_num = menuKeyGetValue();
    curspeed = fanGetSpeed(curIndex);
    switch(key_num)
    {
      case KEY_ICON_0:
        if (curspeed > 0)
        {
          if (infoSettings.fan_percentage == 1)
          {
            if (fanGetSpeedPercent(curIndex) % 20 != 0)
            {
              fanSetSpeedPercent(curIndex, (fanGetSpeedPercent(curIndex))); //rounding current speed % reduces speed % by 1.
            }
            else
            {
              fanSetSpeedPercent(curIndex, (fanGetSpeedPercent(curIndex) - 1)); //except for multiples of 20 that needs subtracting 1.
            }
          }
          else
          {
            fanSetSpeed(curIndex, --curspeed);
          }
        }
        break;

      case KEY_ICON_3:
        if (curspeed < infoSettings.fan_max[curIndex])
        {
          if (infoSettings.fan_percentage ==  1)
          {
            fanSetSpeedPercent(curIndex, (fanGetSpeedPercent(curIndex) + 2)); //adding 2 to current speed % increases speed % by 1.
          }
          else
          {
            fanSetSpeed(curIndex, --curspeed);
          }
        }
        break;

      case KEY_ICON_4:
        if (infoSettings.fan_count > 1)
        {
          curIndex = (curIndex + 1) % infoSettings.fan_count;
          fanSpeedReDraw();
        }
        else
        {
          fanSetSpeed(curIndex, (infoSettings.fan_max[curIndex] + 1) / 2);
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
          if(encoderPosition)
          {
            if (curspeed < infoSettings.fan_max[curIndex] && encoderPosition > 0)
            {
              if (infoSettings.fan_percentage ==  1)
              {
                fanSetSpeedPercent(curIndex, fanGetSpeedPercent(curIndex) + 2);
              }
              else
              {
                fanSetSpeed(curIndex, ++curspeed);
              }
            }

            if (fanSpeed[curIndex] > 0 && encoderPosition < 0) {
              if (infoSettings.fan_percentage == 1)
              {
                if (fanGetSpeedPercent(curIndex) % 20 != 0)
                {
                  fanSetSpeedPercent(curIndex, (fanGetSpeedPercent(curIndex))); //rounding current speed % reduces speed % by 1.
                }
                else
                {
                  fanSetSpeedPercent(curIndex, (fanGetSpeedPercent(curIndex) - 1)); //except for multiples of 20 that needs subtracting 1.
                }
              }
              else
              {
                fanSetSpeed(curIndex, --curspeed);
              }
            }
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (fanSpeedChanged(curIndex))
      fanSpeedReDraw();

    loopProcess();
  }
}
