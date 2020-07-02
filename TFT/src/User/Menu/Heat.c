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

// Show/draw temperature in heat menu
void showTemperature(TOOL tool)
{
  char tempstr[20];

  sprintf(tempstr, "%-15s", heatDisplayID[tool]);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);

  sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(tool), heatGetTargetTemp(tool));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void menuHeat(void)
{
  KEY_VALUES  key_num = KEY_IDLE;

  heatSetUpdateTime(TEMPERATURE_QUERY_FAST_DURATION);

  heatItems.items[KEY_ICON_4] = itemTool[heatGetCurrentTool()];
  menuDrawPage(&heatItems);
  showTemperature(heatGetCurrentTool());

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    key_num = menuKeyGetValue();
    int16_t t_temp = heatGetTargetTemp(heatGetCurrentTool());
    TOOL c_tool = heatGetCurrentTool();
    switch(key_num)
    {
      case KEY_ICON_0:
          heatSetTargetTemp(c_tool, limitValue( 0, t_temp - item_degree[item_degree_i], infoSettings.max_temp[c_tool]));
        break;

      case KEY_ICON_3:
          heatSetTargetTemp(c_tool, limitValue( 0, t_temp + item_degree[item_degree_i], infoSettings.max_temp[c_tool]));
        break;

      case KEY_ICON_4:
        c_tool = (TOOL)(c_tool + 1) % (HEATER_COUNT);
        heatSetCurrentTool(c_tool);
        heatItems.items[key_num] = itemTool[c_tool];
        menuDrawItem(&heatItems.items[key_num], key_num);
        break;

      case KEY_ICON_5:
        item_degree_i = (item_degree_i+1) % ITEM_DEGREE_NUM;
        heatItems.items[key_num] = itemDegree[item_degree_i];
        menuDrawItem(&heatItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        heatSetTargetTemp(c_tool, 0);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if(encoderPosition > 0)
                heatSetTargetTemp(c_tool, limitValue(0, t_temp + item_degree[item_degree_i], infoSettings.max_temp[c_tool]));
            if(encoderPosition < 0)
                heatSetTargetTemp(c_tool, limitValue(0, t_temp - item_degree[item_degree_i], infoSettings.max_temp[c_tool]));
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (heatToolChanged())
      showTemperature(c_tool);

    if (heatCurrentTempChanged(c_tool) || heatTargetTempChanged(c_tool))
      showTemperature(c_tool);

    loopProcess();
  }

  // Set slow update time if not waiting for target temperature
  if(heatHasWaiting() == false)
    heatSetUpdateTime(TEMPERATURE_QUERY_SLOW_DURATION);
}
