#include "Heat.h"
#include "includes.h"

//1 title, ITEM_PER_PAGE items (icon + label)
MENUITEMS heatItems = {
// title
LABEL_HEAT,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_BACK,                 LABEL_BACK},}
};


const ITEM itemTool[] = {
// icon                       label
  {ICON_BED,                  LABEL_BED},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
};

#define ITEM_DEGREE_NUM 3
const ITEM itemDegree[ITEM_DEGREE_NUM] = {
// icon                       label
  {ICON_1_DEGREE,             LABEL_1_DEGREE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_10_DEGREE,            LABEL_10_DEGREE},
};
const  u8 item_degree[ITEM_DEGREE_NUM] = {1, 5, 10};
static u8 item_degree_i = 1;

const char* toolID[] = HEAT_SIGN_ID;
const char* const heatDisplayID[] = HEAT_DISPLAY_ID;
const char* heatCmd[] = HEAT_CMD;
const char* heatWaitCmd[] = HEAT_WAIT_CMD;

static HEATER  heater = {{}, NOZZLE0, NOZZLE0};
static HEATER  lastHeater = {{}, NOZZLE0, NOZZLE0};
static u32     update_time = TEMPERATURE_QUERY_SLOW_DURATION;
static bool    update_waiting = false;
static bool    send_waiting[HEATER_NUM];

/*Set target temperature*/
void heatSetTargetTemp(TOOL tool, u16 temp)
{
  heater.T[tool].target = temp;
}
/*Sync target temperature*/
void heatSyncTargetTemp(TOOL tool, u16 temp)
{
  lastHeater.T[tool].target = heater.T[tool].target = temp;
}

/*Get target temperature */
u16 heatGetTargetTemp(TOOL tool)
{
  return heater.T[tool].target;
}

/* Set current temperature */
void heatSetCurrentTemp(TOOL tool, s16 temp)
{
  heater.T[tool].current = limitValue(-99, temp, 999);
}

/* Get current temperature */
s16 heatGetCurrentTemp(TOOL tool)
{
  return heater.T[tool].current;
}

/* Is heating waiting to heat up */
bool heatGetIsWaiting(TOOL tool)
{
  return (heater.T[tool].waiting != WAIT_NONE);
}

/* Check all heater if there is a heater waiting to be waited */
bool heatHasWaiting(void)
{
  TOOL i;
  for(i = BED; i < infoSettings.ext_count; i++)
  {
    if(heater.T[i].waiting != WAIT_NONE)
      return true;
  }
  return false;
}

/* Set heater waiting status */
void heatSetIsWaiting(TOOL tool, HEATER_WAIT isWaiting)
{
  heater.T[tool].waiting = isWaiting;
  if(isWaiting != WAIT_NONE) // wait heating now, query more frequently
  {
    update_time = TEMPERATURE_QUERY_FAST_DURATION;
  }
  else if(heatHasWaiting() == false)
  {
    update_time = TEMPERATURE_QUERY_SLOW_DURATION;
  }
}

void heatClearIsWaiting(void)
{
  for(TOOL i = BED; i < infoSettings.ext_count; i++)
  {
    heater.T[i].waiting = WAIT_NONE;
  }
  update_time = TEMPERATURE_QUERY_SLOW_DURATION;
}

/* Set current heater tool, nozzle or hot bed */
void heatSetCurrentTool(TOOL tool)
{
  if(tool >= infoSettings.ext_count) return;
  heater.tool = tool;
}
/* Get current tool, nozzle or hot bed */
TOOL heatGetCurrentTool(void)
{
  return heater.tool;
}

/* Set current nozzle */
void heatSetCurrentToolNozzle(TOOL tool)
{
  if(tool >= infoSettings.ext_count && tool < NOZZLE0) return;
  heater.nozzle = tool;
  heater.tool = tool;
}
/* Get current nozzle*/
TOOL heatGetCurrentToolNozzle(void)
{
  return heater.nozzle;
}

/* Set temperature update time interval */
void heatSetUpdateTime(u32 time)
{
  update_time=time;
}

/* Set whether we need to query the current temperature */
void heatSetUpdateWaiting(bool isWaiting)
{
  update_waiting = isWaiting;
}

/* Set whether the heating command has been sent */
void heatSetSendWaiting(TOOL tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

/* Get whether has heating command in Queue */
bool heatGetSendWaiting(TOOL tool)
{
  return send_waiting[tool];
}

void showTemperature(void)
{

  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y-BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)heatDisplayID[heater.tool]);
  GUI_DispDec(CENTER_X-BYTE_WIDTH*3, CENTER_Y, heater.T[heater.tool].current, 3, RIGHT);
  GUI_DispString(CENTER_X, CENTER_Y, (u8*)"/");
  GUI_DispDec(CENTER_X+BYTE_WIDTH*1, CENTER_Y, heater.T[heater.tool].target, 3, LEFT);
}

void currentReDraw(void)
{
  GUI_DispDec(CENTER_X-BYTE_WIDTH*3, CENTER_Y, heater.T[heater.tool].current, 3, RIGHT);
}

void targetReDraw(void)
{
  GUI_DispDec(CENTER_X+BYTE_WIDTH*1, CENTER_Y, heater.T[heater.tool].target, 3, LEFT);
}

void menuHeat(void)
{
  KEY_VALUES  key_num = KEY_IDLE;

  lastHeater = heater;
  update_time = TEMPERATURE_QUERY_FAST_DURATION;

  menuDrawPage(&heatItems);
  showTemperature();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if(heater.T[heater.tool].target > 0)
        {
          heater.T[heater.tool].target =
            limitValue( 0,
                        heater.T[heater.tool].target - item_degree[item_degree_i],
                        infoSettings.max_temp[heater.tool]);
        }
        break;

      case KEY_ICON_3:
        if(heater.T[heater.tool].target < infoSettings.max_temp[heater.tool])
        {
          heater.T[heater.tool].target =
            limitValue( 0,
                        heater.T[heater.tool].target + item_degree[item_degree_i],
                        infoSettings.max_temp[heater.tool]);
        }
        break;

      case KEY_ICON_4:
        lastHeater.tool = heater.tool = (TOOL)((heater.tool+1) % (infoSettings.ext_count + 1));
        heatItems.items[key_num] = itemTool[heater.tool];
        menuDrawItem(&heatItems.items[key_num], key_num);
        showTemperature();
        break;

      case KEY_ICON_5:
        item_degree_i = (item_degree_i+1) % ITEM_DEGREE_NUM;
        heatItems.items[key_num] = itemDegree[item_degree_i];
        menuDrawItem(&heatItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        heater.T[heater.tool].target = 0;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if(heater.T[heater.tool].target < infoSettings.max_temp[heater.tool] && encoderPosition > 0)
            {
              heater.T[heater.tool].target =
                limitValue( 0,
                            heater.T[heater.tool].target + item_degree[item_degree_i],
                            infoSettings.max_temp[heater.tool]);
            }
            if(heater.T[heater.tool].target > 0 && encoderPosition < 0)
            {
              heater.T[heater.tool].target =
                limitValue( 0,
                            heater.T[heater.tool].target - item_degree[item_degree_i],
                            infoSettings.max_temp[heater.tool]);
            }
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if(lastHeater.tool != heater.tool)
    {
      lastHeater.tool = heater.tool;
      showTemperature();
    }
    if(lastHeater.T[heater.tool].current != heater.T[heater.tool].current)
    {
      lastHeater.T[heater.tool].current = heater.T[heater.tool].current;
      currentReDraw();
    }
    if(lastHeater.T[heater.tool].target != heater.T[heater.tool].target)
    {
      targetReDraw();
    }

    loopProcess();
  }

  if(heatHasWaiting() == false)
    update_time = TEMPERATURE_QUERY_SLOW_DURATION;
}

u32 nextHeatCheckTime = 0;
void updateNextHeatCheckTime(void)
{
  nextHeatCheckTime = OS_GetTimeMs() + update_time;
}


void loopCheckHeater(void)
{
  do
  {  /* Send M105 query temperature continuously	*/
    if(update_waiting == true)                {updateNextHeatCheckTime();break;}
    if(OS_GetTimeMs() < nextHeatCheckTime)     break;
    if(RequestCommandInfoIsRunning())          break; //to avoid colision in Gcode response processing
    if(storeCmd("M105\n") == false)            break;
    updateNextHeatCheckTime();
    update_waiting = true;
  }while(0);

  /* Query the heater that needs to wait for the temperature to rise, whether it reaches the set temperature */
  for(u8 i=0; i<infoSettings.ext_count+1; i++)
  {
    if (heater.T[i].waiting == WAIT_NONE)                              continue;
    else if (heater.T[i].waiting == WAIT_HEATING) {
      if (heater.T[i].current+2 <= heater.T[i].target)                 continue;
    }
    else if (heater.T[i].waiting == WAIT_COOLING_HEATING) {
      if (inRange(heater.T[i].current, heater.T[i].target, 2) != true) continue;
    }

    heater.T[i].waiting = WAIT_NONE;
    if (heatHasWaiting())                                              continue;

    if(infoMenu.menu[infoMenu.cur] == menuHeat)                        break;
    update_time = TEMPERATURE_QUERY_SLOW_DURATION;
  }

  for(TOOL i = BED; i < (infoSettings.ext_count+1); i++) // If the target temperature changes, send a Gcode to set the motherboard
  {
    if(lastHeater.T[i].target != heater.T[i].target)
    {
      lastHeater.T[i].target = heater.T[i].target;
      if(send_waiting[i] != true)
      {
        send_waiting[i] = true;
        storeCmd("%s ",heatCmd[i]);
      }
    }
  }
}
