#include "Speed.h"
#include "includes.h"


const ITEM itemPercentType[SPEED_NUM] = {
// icon                       label
  {ICON_MOVE,                 LABEL_PERCENTAGE_SPEED},
  {ICON_EXTRUDE,              LABEL_PERCENTAGE_FLOW},
};

static int16_t itemPercentTypeTitle[SPEED_NUM] = {
  LABEL_PERCENTAGE_SPEED,     LABEL_PERCENTAGE_FLOW
};

static u8 itemPercentType_index = 0;
static u8 percentSteps_index = 0;

void setSpeedItemIndex(uint8_t index)
{
  itemPercentType_index = index;
}

void percentageReDraw(char * title)
{
  char tempstr[20];

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)title);

  sprintf(tempstr, "  %d%%  ", speedGetPercent(itemPercentType_index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect,(u8*)tempstr);
  setLargeFont(false);
}

void menuSpeed(void)
{
  MENUITEMS percentageItems = {
    // title
    LABEL_PERCENTAGE_SPEED,
    // icon                      label
    {{ICON_DEC,                  LABEL_DEC},
     {ICON_BACKGROUND,           LABEL_BACKGROUND},
     {ICON_BACKGROUND,           LABEL_BACKGROUND},
     {ICON_INC,                  LABEL_INC},
     {ICON_MOVE,                 LABEL_PERCENTAGE_SPEED},
     {ICON_E_5_PERCENT,          LABEL_5_PERCENT},
     {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
     {ICON_BACK,                 LABEL_BACK},}
  };

  percentageItems.items[KEY_ICON_5] = itemPercent[percentSteps_index];

  storeCmd("M220\nM221\n");
  KEY_VALUES key_num;

  u16 now = speedGetPercent(itemPercentType_index);

  for (u8 i = 0; i < SPEED_NUM; i++)

  percentageItems.title.index = itemPercentTypeTitle[itemPercentType_index];
  percentageItems.items[KEY_ICON_4] = itemPercentType[itemPercentType_index];
  menuDrawPage(&percentageItems);
  percentageReDraw((char*)textSelect(percentageItems.title.index));

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuSpeed)
  {
    key_num = menuKeyGetValue();
    now = speedGetPercent(itemPercentType_index);
    switch(key_num)
    {
      case KEY_ICON_0:
        if(now > SPEED_MIN)
          speedSetPercent(itemPercentType_index, now - percentSteps[percentSteps_index]);
        break;

      case KEY_INFOBOX:
        {
          u16 val = now;
          char titlestr[30];

          sprintf(titlestr, "Min:%i | Max:%i", SPEED_MIN, SPEED_MAX);
          val = numPadInt((u8 *) titlestr, val, 100, false);
          val = NOBEYOND(SPEED_MIN, val, SPEED_MAX);
          if (val != now)
            speedSetPercent(itemPercentType_index, val);

          menuDrawPage(&percentageItems);
          percentageReDraw((char *) textSelect(percentageItems.title.index));
        }
        break;

      case KEY_ICON_3:
        if(now < SPEED_MAX)
          speedSetPercent(itemPercentType_index, now + percentSteps[percentSteps_index]);
        break;

      case KEY_ICON_4:
        itemPercentType_index = (itemPercentType_index+1) % SPEED_NUM;
        percentageItems.items[key_num] = itemPercentType[itemPercentType_index];
        menuDrawItem(&percentageItems.items[key_num], key_num);
        percentageItems.title.index = itemPercentTypeTitle[itemPercentType_index];
        menuDrawTitle(textSelect(percentageItems.title.index));
        GUI_ClearPrect(&exhibitRect);
        percentageReDraw((char*)textSelect(percentageItems.title.index));
        break;

      case KEY_ICON_5:
        percentSteps_index = (percentSteps_index+1) % ITEM_PERCENT_STEPS_NUM;
        percentageItems.items[key_num] = itemPercent[percentSteps_index];
        menuDrawItem(&percentageItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        speedSetPercent(itemPercentType_index, 100);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (now < SPEED_MAX && encoderPosition > 0)
            {
              speedSetPercent(itemPercentType_index, now + percentSteps[percentSteps_index]);
            }
            else if (now > SPEED_MIN && encoderPosition < 0)
            {
              speedSetPercent(itemPercentType_index, now - percentSteps[percentSteps_index]);
            }
            encoderPosition = 0;
          }
        #endif
      break;
    }

    if (SpeedChanged(itemPercentType_index))
      percentageReDraw((char *)textSelect(percentageItems.title.index));

    loopProcess();
  }
}
