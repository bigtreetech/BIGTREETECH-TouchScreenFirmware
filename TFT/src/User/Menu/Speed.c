#include "Speed.h"
#include "includes.h"

const ITEM itemPercentType[SPEED_NUM] = {
  // icon                        label
  {ICON_MOVE,                    LABEL_PERCENTAGE_SPEED},
  {ICON_EXTRUDE,                 LABEL_PERCENTAGE_FLOW},
};

const int16_t itemPercentTypeTitle[SPEED_NUM] = {
  LABEL_PERCENTAGE_SPEED,
  LABEL_PERCENTAGE_FLOW
};

static uint8_t item_index = 0;
static uint8_t percentSteps_index = 0;

void setSpeedItemIndex(uint8_t index)
{
  item_index = index;
}

void percentageReDraw(bool skip_header)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skip_header)
  {
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, textSelect(itemPercentTypeTitle[item_index]));
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"%");
  }

  sprintf(tempstr, "  %d/%d  ", speedGetCurPercent(item_index), speedGetSetPercent(item_index));
  GUI_DispStringInPrect(&exhibitRect,(u8*)tempstr);
  setLargeFont(false);
}

void menuSpeed(void)
{
  MENUITEMS percentageItems = {
    // title
    LABEL_PERCENTAGE_SPEED,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_MOVE,                    LABEL_PERCENTAGE_SPEED},
      {ICON_E_5_PERCENT,             LABEL_5_PERCENT},
      {ICON_NORMAL_SPEED,            LABEL_NORMAL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  percentageItems.items[KEY_ICON_5] = itemPercent[percentSteps_index];

  storeCmd("M220\nM221 D%d\n", heatGetCurrentTool());
  KEY_VALUES key_num;
  LASTSPEED lastSpeed;
  speedSetPercent(item_index, speedGetCurPercent(item_index));
  lastSpeed = (LASTSPEED) {speedGetCurPercent(item_index), speedGetSetPercent(item_index)};

  percentageItems.title.index = itemPercentTypeTitle[item_index];
  percentageItems.items[KEY_ICON_4] = itemPercentType[item_index];
  menuDrawPage(&percentageItems);
  percentageReDraw(false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuSpeed)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (speedGetSetPercent(item_index) > SPEED_MIN)
          speedSetPercent(item_index, speedGetSetPercent(item_index) - percentSteps[percentSteps_index]);
        break;

      case KEY_INFOBOX:
        {
          char titlestr[30];
          sprintf(titlestr, "Min:%i | Max:%i", SPEED_MIN, SPEED_MAX);

          uint16_t val = numPadInt((uint8_t *) titlestr, speedGetCurPercent(item_index), 100, false);
          val = NOBEYOND(SPEED_MIN, val, SPEED_MAX);

          if (val != speedGetSetPercent(item_index))
            speedSetPercent(item_index, val);

          menuDrawPage(&percentageItems);
          percentageReDraw(false);
        }
        break;

      case KEY_ICON_3:
        if (speedGetSetPercent(item_index) < SPEED_MAX)
          speedSetPercent(item_index, speedGetSetPercent(item_index) + percentSteps[percentSteps_index]);
        break;

      case KEY_ICON_4:
        item_index = (item_index + 1) % SPEED_NUM;
        percentageItems.items[key_num] = itemPercentType[item_index];
        menuDrawItem(&percentageItems.items[key_num], key_num);
        percentageItems.title.index = itemPercentTypeTitle[item_index];
        menuDrawTitle(textSelect(percentageItems.title.index));
        GUI_ClearPrect(&exhibitRect);
        percentageReDraw(false);
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
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (speedGetSetPercent(item_index) < SPEED_MAX && encoderPosition > 0)
              speedSetPercent(item_index, speedGetSetPercent(item_index) + percentSteps[percentSteps_index]);
            else if (speedGetSetPercent(item_index) > SPEED_MIN && encoderPosition < 0)
              speedSetPercent(item_index, speedGetSetPercent(item_index) - percentSteps[percentSteps_index]);
            encoderPosition = 0;
          }
        #endif
      break;
    }

    if ((lastSpeed.cur != speedGetCurPercent(item_index)) || (lastSpeed.set != speedGetSetPercent(item_index)))
    {
      lastSpeed = (LASTSPEED) {speedGetCurPercent(item_index), speedGetSetPercent(item_index)};
      percentageReDraw(true);
    }

    loopProcess();
  }
}
