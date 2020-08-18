#include "Fan.h"
#include "includes.h"

static uint8_t   curIndex = 0;

const ITEM itemFan[2] = {
  //icon                label
  {ICON_FAN,            LABEL_FAN},
  {ICON_FAN_HALF_SPEED, LABEL_FAN_HALF_SPEED},
  };

void fanSpeedReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%-15s", fanID[curIndex]);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  }

  if(infoSettings.fan_percentage == 1)
    sprintf(tempstr, "  %d%%  ", fanGetSpeedPercent(curIndex));
  else
    sprintf(tempstr, "  %d  ", (int)fanGetSpeed(curIndex));

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
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

  uint8_t lastFan = fanGetSpeed(curIndex);

  if (infoSettings.fan_count > 1)
    fanItems.items[KEY_ICON_4] = itemFan[0];
  else
    fanItems.items[KEY_ICON_4] = itemFan[1];

  menuDrawPage(&fanItems);
  fanSpeedReDraw(false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuFan)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    uint8_t actFan = fanGetSpeed(curIndex);
    uint8_t actFanPercent = fanGetSpeedPercent(curIndex);
    switch (key_num)
    {
    case KEY_ICON_0:
      if (actFan > 0)
      {
        if (infoSettings.fan_percentage == 1)
        {
          fanSetSpeedPercent(curIndex, --actFanPercent);
        }
        else
        {
          fanSetSpeed(curIndex, --actFan);
        }
      }
      break;

    case KEY_ICON_3:
      if (actFan < infoSettings.fan_max[curIndex])
      {
        if (infoSettings.fan_percentage == 1)
          fanSetSpeedPercent(curIndex, ++actFanPercent);
        else
          fanSetSpeed(curIndex, ++actFan);
      }
      break;

    case KEY_ICON_4:
      if (infoSettings.fan_count > 1)
      {
        curIndex = (curIndex + 1) % infoSettings.fan_count;
        fanSpeedReDraw(false);
      }
      else
      {
        actFan = 50;
        fanSetSpeedPercent(curIndex, actFan);
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
          if (actFan < infoSettings.fan_max[curIndex] && encoderPosition > 0)
          {
            if (infoSettings.fan_percentage ==  1)
            {
              fanSetSpeedPercent(curIndex, ++actFanPercent);
            }
            else
            {
              fanSetSpeed(curIndex, ++actFan);
            }
          }

          if (actFan > 0 && encoderPosition < 0) {
            if (infoSettings.fan_percentage == 1)
            {
              fanSetSpeedPercent(curIndex, --actFanPercent);
            }
            else
            {
              fanSetSpeed(curIndex, --actFan);
            }
          }
          encoderPosition = 0;
        }
      #endif
      break;
    }

    if (lastFan != fanGetSpeed(curIndex))
    {
      lastFan = fanGetSpeed(curIndex);
      fanSpeedReDraw(true);
    }

    loopProcess();
  }
}
