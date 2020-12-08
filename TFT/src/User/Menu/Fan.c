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

  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "  %d%%  ", fanGetDesiredPercent(curIndex));
  else
    sprintf(tempstr, "  %d  ", (int)fanGetDesiredSpeed(curIndex));

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
    // icon                      label
    {{ICON_DEC,                  LABEL_DEC},
     {ICON_BACKGROUND,           LABEL_BACKGROUND},
     {ICON_BACKGROUND,           LABEL_BACKGROUND},
     {ICON_INC,                  LABEL_INC},
     {ICON_FAN ,                 LABEL_FAN},
     {ICON_FAN_FULL_SPEED,       LABEL_FAN_FULL_SPEED},
     {ICON_STOP,                 LABEL_STOP},
     {ICON_BACK,                 LABEL_BACK},}
  };

  uint8_t lastFan = fanGetCurSpeed(curIndex);

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
    uint8_t actFan = fanGetDesiredSpeed(curIndex);
    uint8_t actFanPercent = fanGetDesiredPercent(curIndex);
    switch (key_num)
    {
      case KEY_ICON_0:
        if (actFan > 0)
        {
          if (infoSettings.fan_percentage == 1)
            fanSetDesiredPercent(curIndex, --actFanPercent);
          else
            fanSetDesiredSpeed(curIndex, --actFan);
        }
        break;

      case KEY_INFOBOX:
        {
          uint8_t val = actFanPercent;
          char titlestr[30];

          sprintf(titlestr, "Min:0 | Max:100");
          val = numPadInt((u8 *) titlestr, actFanPercent, 0, false);
          val = NOBEYOND(0, val, 100);
          if (val != actFanPercent)
            fanSetDesiredPercent(curIndex, val);

          menuDrawPage(&fanItems);
          fanSpeedReDraw(false);
        }
        break;

      case KEY_ICON_3:
        if (actFan < infoSettings.fan_max[curIndex])
        {
          if (infoSettings.fan_percentage == 1)
            fanSetDesiredPercent(curIndex, ++actFanPercent);
          else
            fanSetDesiredSpeed(curIndex, ++actFan);
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
          actFan = 50;
          fanSetDesiredPercent(curIndex, actFan);
        }
        break;

      case KEY_ICON_5:
        fanSetDesiredSpeed(curIndex, infoSettings.fan_max[curIndex]);
        break;

      case KEY_ICON_6:
        fanSetDesiredSpeed(curIndex, 0);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (actFan < infoSettings.fan_max[curIndex] && encoderPosition > 0)
            {
              if (infoSettings.fan_percentage == 1)
                fanSetDesiredPercent(curIndex, ++actFanPercent);
              else
                fanSetDesiredSpeed(curIndex, ++actFan);
            }

            if (actFan > 0 && encoderPosition < 0)
            {
              if (infoSettings.fan_percentage == 1)
                fanSetDesiredPercent(curIndex, --actFanPercent);
              else
                fanSetDesiredSpeed(curIndex, --actFan);
            }
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (lastFan != fanGetDesiredSpeed(curIndex))
    {
      lastFan = fanGetDesiredSpeed(curIndex);
      fanSpeedReDraw(true);
    }

    loopProcess();
  }
}
