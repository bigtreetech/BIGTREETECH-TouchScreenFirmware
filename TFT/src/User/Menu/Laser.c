#include "Laser.h"
#include "includes.h"


//1 title, ITEM_PER_PAGE items(icon+label)
const MENUITEMS laserItems = {
//   title
LABEL_LASER,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_LASER_2 ,             LABEL_LASER_2},
  {ICON_LASER_100,       	    LABEL_LASER_100},
  {ICON_LASER_OFF,            LABEL_LASER_OFF},
  {ICON_BACK,                 LABEL_BACK},}
};

const char* laserID[] = FAN_ID;
const char* laserCmd[] = FAN_CMD;
const u8    laserMaxPWM[] = FAN_MAX_PWM;

static u8   laserSpeed[FAN_NUM] = {0};
static u8   curIndex = 0;
static bool send_waiting[FAN_NUM] = {false};

void laserSetSpeed(u8 i, u8 speed)
{
  laserSpeed[i] = speed;
}

u8 laserGetSpeed(u8 i)
{
  return laserSpeed[i];
}

void laserSetCurIndex(u8 i)
{
  if(i >= FAN_NUM) return;
  curIndex = i;
}

u8 laserGetCurIndex(u8 i)
{
  return curIndex;
}

void laserSetSendWaiting(u8 i, bool isWaiting)
{
  send_waiting[i] = isWaiting;
}

void showLaserSpeed(void)
{
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y-BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  u8 fs;
  #ifdef SHOW_FAN_PERCENTAGE
    fs = (laserSpeed[curIndex]*100)/255;
  #else
    fs = laserSpeed[curIndex]
  #endif
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)laserID[curIndex]);
  #ifdef SHOW_FAN_PERCENTAGE
    char laser_s[5];
    sprintf(laser_s, "%3d%%", fs);
    GUI_DispString(CENTER_X-BYTE_WIDTH, CENTER_Y, (u8 *)laser_s);
  #else
    GUI_DispDec(CENTER_X-BYTE_WIDTH, CENTER_Y, fs, 3, LEFT);
  #endif
}

void laserSpeedReDraw(void)
{
  #ifdef SHOW_FAN_PERCENTAGE
    char laser_s[5] = "";
    sprintf(laser_s, "%3d%%", (laserSpeed[curIndex]*100)/255);
    GUI_DispString(CENTER_X-BYTE_WIDTH, CENTER_Y, (u8 *)laser_s);
  #else
    GUI_DispDec(CENTER_X-BYTE_WIDTH, CENTER_Y, laserSpeed[curIndex];, 3, LEFT);
  #endif
}

void menuLaser(void)
{
  u8 nowIndex = curIndex;
  u8 nowLaserSpeed[FAN_NUM];
  memcpy(nowLaserSpeed, laserSpeed, sizeof(laserSpeed));
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&laserItems);
  showLaserSpeed();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuLaser)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if (laserSpeed[curIndex] > 0) {
            #ifdef SHOW_FAN_PERCENTAGE
              if ((laserSpeed[curIndex]-2) > 0) {
                laserSpeed[curIndex]-=2; //2.55 is 1 percent, rounding down
              } else {
                laserSpeed[curIndex]=0;
              }
            #else
              laserSpeed[curIndex]--;
            #endif
        }
        break;

      case KEY_ICON_3:
        if (laserSpeed[curIndex] < laserMaxPWM[curIndex]){
            #ifdef SHOW_FAN_PERCENTAGE
              if (laserSpeed[curIndex]+2 <= laserMaxPWM[curIndex]){
                laserSpeed[curIndex]+=2; //2.55 is 1 percent, rounding down
              } else {
                laserSpeed[curIndex]=laserMaxPWM[curIndex];
              }
            #else
              laserSpeed[curIndex]++;
            #endif
        }
        break;

      case KEY_ICON_4:
        /*curIndex = (curIndex + 1) % FAN_NUM;
        showLaserSpeed();*/
        laserSpeed[curIndex] = 6;
        break;

      case KEY_ICON_5:
        laserSpeed[curIndex] = laserMaxPWM[curIndex];
        break;

      case KEY_ICON_6:
        laserSpeed[curIndex] = 0;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if (laserSpeed[curIndex] < laserMaxPWM[curIndex] && encoderPosition > 0){
              #ifdef SHOW_FAN_PERCENTAGE
                if (laserSpeed[curIndex]+2 <= laserMaxPWM[curIndex]){
                  laserSpeed[curIndex]+=2; //2.55 is 1 percent, rounding down
                } else {
                  laserSpeed[curIndex]=laserMaxPWM[curIndex];
                }
              #else
                laserSpeed[curIndex]++;
              #endif
            }

            if (laserSpeed[curIndex] > 0 && encoderPosition < 0) {
              #ifdef SHOW_FAN_PERCENTAGE
                if ((laserSpeed[curIndex]-2) > 0) {
                  laserSpeed[curIndex]-=2; //2.55 is 1 percent, rounding down
                } else {
                  laserSpeed[curIndex]=0;
                }
              #else
                laserSpeed[curIndex]--;
              #endif
            }

            encoderPosition = 0;
          }
          LCD_LoopEncoder();
        #endif
        break;
    }

    if(nowIndex != curIndex)
    {
      nowIndex = curIndex;
      showLaserSpeed();
    }
    if(nowLaserSpeed[curIndex] != laserSpeed[curIndex])
    {
      nowLaserSpeed[curIndex] = laserSpeed[curIndex];
      laserSpeedReDraw();
      if(send_waiting[curIndex] != true)
      {
        send_waiting[curIndex] = true;
        storeCmd("%s ", laserCmd[curIndex]);
      }
    }
    loopProcess();
  }
}
