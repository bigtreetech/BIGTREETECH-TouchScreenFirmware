#include "heat.h"
#include "includes.h"

//1个title(标题), ITEM_PER_PAGE个item(图标+标签) 
MENUITEMS heatItems = {
//  title
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

const u16   heat_max_temp[] = HEAT_MAX_TEMP;
const char* toolID[] = HEAT_SIGN_ID;
const char* const heatDisplayID[] = HEAT_DISPLAY_ID;
const char* heatCmd[] = HEAT_CMD;
const char* heatWaitCmd[] = HEAT_WAIT_CMD;

static HEATER  heater = {{0}, NOZZLE0, NOZZLE0};
static u32     update_time = 300;
static bool    update_waiting = false;
static bool    send_waiting[HEATER_NUM];

/*设置目标温度*/
void heatSetTargetTemp(TOOL tool,u16 temp)
{
  heater.T[tool].target = temp;
}

/*获取目标的温度*/
u16 heatGetTargetTemp(TOOL tool)
{
  return heater.T[tool].target;
}

/* 设置当前的温度 */
void heatSetCurrentTemp(TOOL tool, s16 temp)
{
  heater.T[tool].current = limitValue(-99, temp, 999);
}

/* 获取当前的温度 */
s16 heatGetCurrentTemp(TOOL tool)
{
  return heater.T[tool].current;
}

/* 是否等待加热器升温 */
bool heatGetIsWaiting(TOOL tool)
{
  return heater.T[tool].waiting;
}

/* 查询是否有需要等待的加热器 */
bool heatHasWaiting(void)
{
  TOOL i;
  for(i = BED; i < HEATER_NUM; i++)
  {
    if(heater.T[i].waiting == true)
    return true;
  }
  return false;
}

/* 设置是否等待加热器升温 */
void heatSetIsWaiting(TOOL tool, bool isWaiting)
{
  heater.T[tool].waiting = isWaiting;
  if(isWaiting == true)
  {
    update_time = 100;
  }
  else if(heatHasWaiting() == false)
  {
    update_time = 300;		
  }
}

void heatClearIsWaiting(void)
{
  for(TOOL i = BED; i < HEATER_NUM; i++)
  {
    heater.T[i].waiting = false;
  }
  update_time = 300;
}

/* 设置当前是喷头还是热床 */
void heatSetCurrentTool(TOOL tool)
{
  if(tool >= HEATER_NUM) return;
  heater.tool = tool;
}
/* 获取当前是喷头还是热床 */
TOOL heatGetCurrentTool(void)
{
  return heater.tool;
}

/* 设置当前是哪个喷头*/
void heatSetCurrentToolNozzle(TOOL tool)
{
  if(tool >= HEATER_NUM && tool < NOZZLE0) return;
  heater.nozzle = tool;
  heater.tool = tool;
}
/* 获取当前是那个喷头*/
TOOL heatGetCurrentToolNozzle(void)
{
  return heater.nozzle;
}

/* 设置查询温度的时间间隔 */
void heatSetUpdateTime(u32 time)
{
  update_time=time;
}
/* 设置当前是否需要查询温度 */
void heatSetUpdateWaiting(bool isWaiting)
{
  update_waiting = isWaiting;
}

/* 设置是否已经发送加热命令 */
void heatSetSendWaiting(TOOL tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void showTemperature(void)
{
  
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y-BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)heatDisplayID[heater.tool], 1);
  GUI_DispDec(CENTER_X-BYTE_WIDTH*3, CENTER_Y, heater.T[heater.tool].current, 3, 1, RIGHT);
  GUI_DispChar(CENTER_X, CENTER_Y, '/', 0);
  GUI_DispDec(CENTER_X+BYTE_WIDTH*1, CENTER_Y, heater.T[heater.tool].target,3,1,LEFT);
}

void currentReDraw(void)
{
  GUI_DispDec(CENTER_X-BYTE_WIDTH*3, CENTER_Y, heater.T[heater.tool].current,3,1,RIGHT);
}

void targetReDraw(void)
{
  GUI_DispDec(CENTER_X+BYTE_WIDTH*1, CENTER_Y, heater.T[heater.tool].target,3,1,LEFT);
}

void menuHeat(void)
{
  HEATER      nowHeater = heater;
  KEY_VALUES  key_num = KEY_IDLE;

  update_time=100;

  menuDrawPage(&heatItems);
  showTemperature();

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
                        heat_max_temp[heater.tool]);
        }
        break;
      
      case KEY_ICON_3:
        if(heater.T[heater.tool].target < heat_max_temp[heater.tool])
        {
          heater.T[heater.tool].target = 
            limitValue( 0, 
                        heater.T[heater.tool].target + item_degree[item_degree_i], 
                        heat_max_temp[heater.tool]);
        }
        break;
        
      case KEY_ICON_4:
        nowHeater.tool = heater.tool = (TOOL)((heater.tool+1) % HEATER_NUM);
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
        break;
    }

    if(nowHeater.tool != heater.tool)
    {
      nowHeater.tool = heater.tool;
      showTemperature();
    }
    if(nowHeater.T[heater.tool].current != heater.T[heater.tool].current)
    {      
      nowHeater.T[heater.tool].current = heater.T[heater.tool].current;
      currentReDraw();
    }
    if(nowHeater.T[heater.tool].target != heater.T[heater.tool].target)
    {
      targetReDraw();
    }
    
    for(TOOL i = BED; i < HEATER_NUM; i++)
    {
      if(nowHeater.T[i].target != heater.T[i].target)
      {
        nowHeater.T[i].target = heater.T[i].target;
        if(send_waiting[i] != true)
        {
          send_waiting[i] = true;
          storeCmd("%s ",heatCmd[i]);
        }
      }
    }
    

    loopProcess();
  }
  if(heatHasWaiting()==false)
  update_time=300;
}


void loopCheckHeater(void)
{
  u8 i;
  static u32  nowTime=0;

  do
  {  /* 定时发送M105查询温度	*/
    if(update_waiting == true)                {nowTime=OS_GetTime();break;}
    if(OS_GetTime()<nowTime+update_time)       break;

    if(storeCmd("M105\n")==false)              break;

    nowTime=OS_GetTime();
    update_waiting=true;
  }while(0);

  /* 查询需要等待温度上升的加热器，是否达到设定温度 */
  for(i=0; i<HEATER_NUM; i++)
  {
    if (heater.T[i].waiting == false)                                   continue;
    if (i==BED)
    {
      if (heater.T[BED].current+2 <= heater.T[BED].target)              continue;
    }
    else
    {
      if (inRange(heater.T[i].current, heater.T[i].target, 2) != true)  continue;
    }

    heater.T[i].waiting = false;
    if(heatHasWaiting() == true)                                        continue;

    if(infoMenu.menu[infoMenu.cur] == menuHeat)                         break;
    update_time=300;
  }
}





