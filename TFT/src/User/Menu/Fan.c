#include "Fan.h"
#include "includes.h"


//1 title, ITEM_PER_PAGE items(icon+label)
const MENUITEMS fanItems = {
//   title
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

const char* fanID[] = FAN_ID;
const char* fanCmd[] = FAN_CMD;

static u8   fanSpeed[MAX_FAN_COUNT] = {0};
static u8   curIndex = 0;
static bool send_waiting[MAX_FAN_COUNT] = {false};

void fanSetSpeed(u8 i, u8 speed)
{
  fanSpeed[i] = speed;
}

u8 fanGetSpeed(u8 i)
{
  return fanSpeed[i];
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

void showFanSpeed(void)
{
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y-BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  u8 fs;
  if(infoSettings.fan_percentage == 1)
    fs = (fanSpeed[curIndex]*100)/255;
  else
    fs = fanSpeed[curIndex];

  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)fanID[curIndex]);
  if(infoSettings.fan_percentage == 1)
    {
      char fan_s[5];
      sprintf(fan_s, "%3u%%", fs);
      GUI_DispString(CENTER_X-BYTE_WIDTH, CENTER_Y, (u8 *)fan_s);
    }
  else
    {GUI_DispDec(CENTER_X-BYTE_WIDTH, CENTER_Y, fs, 3, LEFT);
    }
}

void fanSpeedReDraw(void)
{
 if(infoSettings.fan_percentage == 1)
    {
      char fan_s[5] = "";
      sprintf(fan_s, "%3u%%", (fanSpeed[curIndex]*100)/255);
      GUI_DispString(CENTER_X-BYTE_WIDTH, CENTER_Y, (u8 *)fan_s);
    }
  else
    GUI_DispDec(CENTER_X-BYTE_WIDTH, CENTER_Y, fanSpeed[curIndex], 3, LEFT);
}

void menuFan(void)
{
  u8 nowIndex = curIndex;
  u8 nowFanSpeed[infoSettings.fan_count];
  memcpy(nowFanSpeed, fanSpeed, sizeof(fanSpeed));
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&fanItems);
  showFanSpeed();

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
            if ((fanSpeed[curIndex] - 2) > 0)
              fanSpeed[curIndex] -= 2; //2.55 is 1 percent, rounding down
            else
              fanSpeed[curIndex] = 0;
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
            if (fanSpeed[curIndex] + 2 <= infoSettings.fan_max[curIndex])
              fanSpeed[curIndex] += 2; //2.55 is 1 percent, rounding down
            else
              fanSpeed[curIndex] = infoSettings.fan_max[curIndex];
          }
          else
          {
            fanSpeed[curIndex]++;
          }
        }
        break;

      case KEY_ICON_4:
        curIndex = (curIndex + 1) % infoSettings.fan_count;
        showFanSpeed();
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
                if (fanSpeed[curIndex] + 2 <= infoSettings.fan_max[curIndex])
                  fanSpeed[curIndex] += 2; //2.55 is 1 percent, rounding down
                else
                  fanSpeed[curIndex] = infoSettings.fan_max[curIndex];
              }
              else
              {
                fanSpeed[curIndex]++;
              }
            }

            if (fanSpeed[curIndex] > 0 && encoderPosition < 0) {
              if (infoSettings.fan_percentage ==  1)
              {
                if ((fanSpeed[curIndex] - 2) > 0)
                  fanSpeed[curIndex] -= 2; //2.55 is 1 percent, rounding down
                else
                  fanSpeed[curIndex] = 0;
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
      showFanSpeed();
    }
    if(nowFanSpeed[curIndex] != fanSpeed[curIndex])
    {
      nowFanSpeed[curIndex] = fanSpeed[curIndex];
      fanSpeedReDraw();
      if(send_waiting[curIndex] != true)
      {
        send_waiting[curIndex] = true;
        storeCmd("%s ", fanCmd[curIndex]);
      }
    }
    loopProcess();
  }
}
