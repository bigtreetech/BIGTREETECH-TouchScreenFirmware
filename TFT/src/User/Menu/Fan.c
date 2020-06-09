#include "Fan.h"
#include "includes.h"


const char* fanID[] = FAN_DISPLAY_ID;
const char* fanCmd[] = FAN_CMD;
const char* fanSignID[] = FAN_SIGN_ID;

static u8   fanSpeed[MAX_FAN_COUNT] = {0};
static u8   curIndex = 0;
static bool send_waiting[MAX_FAN_COUNT] = {false};

const ITEM itemFan[2] = {
  //icon                label
  {ICON_FAN,            LABEL_FAN},
  {ICON_FAN_HALF_SPEED, LABEL_FAN_HALF_SPEED},
  };

void fanSetSpeed(u8 i, u8 speed)
{
  fanSpeed[i] = speed;
}

u8 fanGetSpeed(u8 i)
{
  return fanSpeed[i];
}

void fanSetSpeedPercent(u8 i, int16_t percent)
{
  percent = limitValue(0, percent, 100);
  fanSpeed[i] = (percent * infoSettings.fan_max[i]) / 100;
}

u8 fanGetSpeedPercent(u8 i)
{
  return ((fanSpeed[i]*100)/255);
}

void fanSetCurIndex(u8 i)
{
  if(i >= infoSettings.fan_count) return;
  curIndex = i;
}

u8 fanGetCurIndex(u8 i)
{
  return curIndex;
}

void fanSetSendWaiting(u8 i, bool isWaiting)
{
  send_waiting[i] = isWaiting;
}

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
    sprintf(fan_s, "%s: % 4d", fanID[curIndex], (int)(fanSpeed[curIndex]));
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

  u8 nowIndex = curIndex;
  u8 nowFanSpeed[infoSettings.fan_count];
  memcpy(nowFanSpeed, fanSpeed, sizeof(fanSpeed));
  KEY_VALUES key_num = KEY_IDLE;

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
    switch(key_num)
    {
      case KEY_ICON_0:
        if (fanSpeed[curIndex] > 0)
        {
          if (infoSettings.fan_percentage ==  1)
          {
            fanSetSpeedPercent(curIndex, (fanGetSpeedPercent(curIndex) - 1));  //subtracting 1 from current speed % reduces speed % by 2.
          }
          else
          {
            fanSpeed[curIndex]--;
          }
        }
        break;

      case KEY_ICON_3:
        if (fanSpeed[curIndex] < infoSettings.fan_max[curIndex])
        {
          if (infoSettings.fan_percentage ==  1)
          {
            fanSetSpeedPercent(curIndex, (fanGetSpeedPercent(curIndex) + 2)); //adding 2 to current speed % increases speed % by 1.
          }
          else
          {
            fanSpeed[curIndex]++;
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
          fanSpeed[curIndex] = (infoSettings.fan_max[curIndex] + 1) / 2;
        }
        break;

      case KEY_ICON_5:
        fanSpeed[curIndex] = infoSettings.fan_max[curIndex];
        break;

      case KEY_ICON_6:
        fanSpeed[curIndex] = 0;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if (fanSpeed[curIndex] < infoSettings.fan_max[curIndex] && encoderPosition > 0)
            {
              if (infoSettings.fan_percentage ==  1)
              {
                fanSetSpeedPercent(curIndex, fanGetSpeedPercent(curIndex) + 2);
              }
              else
              {
                fanSpeed[curIndex]++;
              }
            }

            if (fanSpeed[curIndex] > 0 && encoderPosition < 0) {
              if (infoSettings.fan_percentage ==  1)
              {
                fanSetSpeedPercent(curIndex, fanGetSpeedPercent(curIndex) - 1);
              }
              else
              {
                fanSpeed[curIndex]--;
              }
            }
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if(nowIndex != curIndex)
    {
      nowIndex = curIndex;
      fanSpeedReDraw();
    }

    if(nowFanSpeed[curIndex] != fanSpeed[curIndex])
    {
      nowFanSpeed[curIndex] = fanSpeed[curIndex];
      fanSpeedReDraw();
      // if(send_waiting[curIndex] != true)
      // {
        //send_waiting[curIndex] = true;
      storeCmd("%s S%d\n", fanCmd[curIndex],fanSpeed[curIndex]);
      //}
    }

    loopProcess();
  }
}
