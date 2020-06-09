#include "Speed.h"
#include "includes.h"


#define ITEM_PERCENTAGE_NUM 2
const ITEM itemPercentage[ITEM_PERCENTAGE_NUM] = {
// icon                       label
  {ICON_MOVE,                 LABEL_PERCENTAGE_SPEED},
  {ICON_EXTRUDE,              LABEL_PERCENTAGE_FLOW},
};
static int16_t itemPercentageTitle[ITEM_PERCENTAGE_NUM] = {
  LABEL_PERCENTAGE_SPEED,     LABEL_PERCENTAGE_FLOW
};
                                          //Speed  Flow
static u16 percentage[ITEM_PERCENTAGE_NUM]= {100,   100};
static u8 item_percentage_i = 0;

#define ITEM_PERCENT_UNIT_NUM 3
const ITEM itemPercentUnit[ITEM_PERCENT_UNIT_NUM] = {
// icon                       label
  {ICON_E_1_PERCENT,          LABEL_1_PERCENT},
  {ICON_E_5_PERCENT,          LABEL_5_PERCENT},
  {ICON_E_10_PERCENT,         LABEL_10_PERCENT},
};
const  u8 item_percent_unit[ITEM_PERCENT_UNIT_NUM] = {1, 5, 10};
static u8 item_percent_unit_i = 0;

static bool send_waiting[ITEM_PERCENTAGE_NUM];

void speedSetSendWaiting(u8 tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void speedSetPercent(u8 tool, u16 per)
{
  percentage[tool]=limitValue(10, per, 999);
}

u16 speedGetPercent(u8 tool)
{
  return percentage[tool];
}

void percentageReDraw(char * title)
{
  GUI_DispString(exhibitRect.x0, exhibitRect.y0,(u8*)title);
  char tempstr[10];
  setLargeFont(true);
  sprintf(tempstr,"% 4d%%", percentage[item_percentage_i]);
  GUI_DispStringInPrect(&exhibitRect,(u8*)tempstr);
  setLargeFont(false);
}

void menuSpeed(void)
{
  MENUITEMS percentageItems = {
  //   title
  LABEL_PERCENTAGE_SPEED,
  // icon                       label
  {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_MOVE,                 LABEL_PERCENTAGE_SPEED},
    {ICON_E_5_PERCENT,          LABEL_5_PERCENT},
    {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
    {ICON_BACK,                 LABEL_BACK},}
  };

  percentageItems.items[KEY_ICON_5] = itemPercentUnit[item_percent_unit_i];

  storeCmd("M220\nM221\n");
  KEY_VALUES  key_num=KEY_IDLE;
  u16         now[ITEM_PERCENTAGE_NUM];

  for(u8 i=0; i<ITEM_PERCENTAGE_NUM; i++)
  now[i] = percentage[i];

  menuDrawPage(&percentageItems);
  percentageReDraw((char*)textSelect(percentageItems.title.index));

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuSpeed)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if(percentage[item_percentage_i] > 10)
        {
          percentage[item_percentage_i] =
            limitValue( 10,
                        percentage[item_percentage_i] - item_percent_unit[item_percent_unit_i],
                        999);
        }
        break;

      case KEY_ICON_3:
        if(percentage[item_percentage_i] < 999)
        {
          percentage[item_percentage_i] =
            limitValue( 10,
                        percentage[item_percentage_i] + item_percent_unit[item_percent_unit_i],
                        999);
        }
        break;

      case KEY_ICON_4:
        item_percentage_i = (item_percentage_i+1) % ITEM_PERCENTAGE_NUM;
        percentageItems.items[key_num] = itemPercentage[item_percentage_i];
        menuDrawItem(&percentageItems.items[key_num], key_num);
        percentageItems.title.index = itemPercentageTitle[item_percentage_i];
        menuDrawTitle(textSelect(percentageItems.title.index));
        GUI_ClearPrect(&exhibitRect);
        percentageReDraw((char*)textSelect(percentageItems.title.index));
        break;

      case KEY_ICON_5:
        item_percent_unit_i = (item_percent_unit_i+1) % ITEM_PERCENT_UNIT_NUM;
        percentageItems.items[key_num] = itemPercentUnit[item_percent_unit_i];
        menuDrawItem(&percentageItems.items[key_num], key_num);
        break;
      case KEY_ICON_6:
        percentage[item_percentage_i] = 100;
        break;
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default:
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if(percentage[item_percentage_i] < 999 && encoderPosition > 0)
            {
              percentage[item_percentage_i] =
                limitValue( 10,
                            percentage[item_percentage_i] + item_percent_unit[item_percent_unit_i],
                            999);
            }
            if(percentage[item_percentage_i] > 10 && encoderPosition < 0)
            {
              percentage[item_percentage_i] =
                limitValue( 10,
                            percentage[item_percentage_i] - item_percent_unit[item_percent_unit_i],
                            999);
            }
            encoderPosition = 0;
          }
        #endif
      break;
    }

    char *speedCmd[ITEM_PERCENTAGE_NUM] = {"M220","M221"};
    if(now[item_percentage_i] != percentage[item_percentage_i])
    {
      now[item_percentage_i] = percentage[item_percentage_i];
      percentageReDraw((char*)textSelect(percentageItems.title.index));
      //if(send_waiting[item_percentage_i] != true)
      //{
        //send_waiting[item_percentage_i] = true;
      storeCmd("%s S%d\n",speedCmd[item_percentage_i], percentage[item_percentage_i]);
      //}
    }
    loopProcess();
  }
}
