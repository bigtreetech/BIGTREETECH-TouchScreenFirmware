#include "Heat.h"
#include "includes.h"

#define HEAT_TOGGLE_ICON    0
#define DEC_BUTTON          8
#define NUM_BUTTON          9
#define INC_BUTTON          10
#define PREHEAT_TOGGLE_ICON 4

#define ACTION_BAR_LEFT     (ICON_WIDTH+SPACE_X+START_X)
#define ACTION_BAR_WIDTH    (2*ICON_WIDTH + SPACE_X)
#define ACTION_BAR_BOTTOM   (ICON_HEIGHT+ICON_START_Y)
#define ACTION_BAR_TOP      (ACTION_BAR_BOTTOM-SMALLICON_HEIGHT)
#define ACTION_BUTTON_COUNT 3

const GUI_RECT rect_of_HeatMenu[ITEM_PER_PAGE*2]={
  //8 icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {0,0,0,0},
  {0,0,0,0},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  {ACTION_BAR_LEFT,                                      ACTION_BAR_TOP, ACTION_BAR_LEFT+SMALLICON_WIDTH,                       ACTION_BAR_BOTTOM},
  {ACTION_BAR_LEFT+(ACTION_BAR_WIDTH-SMALLICON_WIDTH)/2, ACTION_BAR_TOP, ACTION_BAR_LEFT+(ACTION_BAR_WIDTH+SMALLICON_WIDTH)/2,  ACTION_BAR_BOTTOM},
  {ACTION_BAR_LEFT+ACTION_BAR_WIDTH-SMALLICON_WIDTH,     ACTION_BAR_TOP, ACTION_BAR_LEFT+ACTION_BAR_WIDTH,                      ACTION_BAR_BOTTOM},
};

//1 title, ITEM_PER_PAGE items (icon + label)
MENUITEMS heatItems = {
// title
LABEL_HEAT,
// icon                       label
 {{ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_PREHEAT,              LABEL_PREHEAT},
  {ICON_PREHEAT_BOTH,         LABEL_PREHEAT_BOTH},
  {ICON_EEPROM_SAVE,          LABEL_APPLY},
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

const ITEM preheatItems[PREHEAT_NUM] = {
// icon                       label
  {ICON_PREHEAT_BOTH,         LABEL_PREHEAT_BOTH},
  {ICON_BED,                  LABEL_BED},
  {ICON_NOZZLE,               LABEL_NOZZLE},
};

const char* toolID[] = HEAT_SIGN_ID;
const char* const heatDisplayID[] = HEAT_DISPLAY_ID;
const char* heatCmd[] = HEAT_CMD;
const char* heatWaitCmd[] = HEAT_WAIT_CMD;

const GUI_POINT preheat_title = {ICON_WIDTH/2, PREHEAT_TITLE_Y };
const GUI_POINT preheat_val_tool = {ICON_WIDTH - BYTE_WIDTH/2, PREHEAT_TOOL_Y};
const GUI_POINT preheat_val_bed = {ICON_WIDTH - BYTE_WIDTH/2, PREHEAT_BED_Y};


static HEATER  heater = {{}, NOZZLE0, NOZZLE0};
static HEATER  lastHeater = {{}, NOZZLE0, NOZZLE0};

static u32     update_time = TEMPERATURE_QUERY_SLOW_DURATION;
static bool    update_waiting = false;
static bool    send_waiting[HEATER_NUM];

static u8 preheatTools = 0;
static u8 preheat_selected = 0;
static bool isEditing = false;

/*Set target temperature*/
void heatSetTargetTemp(TOOL tool, u16 temp)
{
  if (temp <= infoSettings.max_temp[tool])
    heater.T[tool].target = temp;
}

/*Sync target temperature*/
void heatSyncTargetTemp(TOOL tool, u16 temp)
{
  lastHeater.T[tool].target = heater.T[tool].target = temp;

  if (TSC_ReDrawIcon == IconRedraw && tool == heater.tool)
      showTemperature(tool);
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
  for(i = BED; i < HEATER_COUNT; i++)
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
  for(TOOL i = BED; i < HEATER_COUNT; i++)
  {
    heater.T[i].waiting = WAIT_NONE;
  }
  update_time = TEMPERATURE_QUERY_SLOW_DURATION;
}

/* Set current heater tool, nozzle or hot bed */
void heatSetCurrentTool(TOOL tool)
{
  if(tool >= HEATER_COUNT) return;
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
  if(tool >= (infoSettings.tool_count + 1) || tool < NOZZLE0) return;
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

// get key press index
KEY_VALUES key_val(void)
{
  return (KEY_VALUES)KEY_GetValue(COUNT(rect_of_HeatMenu), rect_of_HeatMenu);
}

// draw icon press aor action bar and pass
 IconRedraw(u8 position, u8 ispressed)
{
  if (position <= KEY_ICON_7)
    {
      itemDrawIconPress(position, ispressed);
    }
  else if(position <= INC_BUTTON)
  {
    if (!ispressed)
    {
    GUI_SetColor(infoSettings.bg_color);
    }
    GUI_DrawRect(rect_of_HeatMenu[position].x0-2,rect_of_HeatMenu[position].y0-2,rect_of_HeatMenu[position].x1+2,rect_of_HeatMenu[position].y1+2);
    GUI_SetColor(infoSettings.font_color);
  }
}

// Show/draw temperature in heat menu
void showTemperature(TOOL tool)
{
  char tempstr[40];
  char val[15];
  sprintf(val,"%d/%d",heatGetCurrentTemp(tool), heatGetTargetTemp(tool));
  sprintf(tempstr, "%-3s %7s",heatDisplayID[tool], val);
  setLargeFont(true);
  GUI_DispStringInRect(exhibitRect.x0,exhibitRect.y0,exhibitRect.x1,exhibitRect.y0+BYTE_HEIGHT,(u8*)tempstr);
  setLargeFont(false);
}

// Redraw Preheat icon details
void refreshPreheatIcon(int8_t num)
{
  STRINGS_STORE preheatnames;
  W25Qxx_ReadBuffer((uint8_t*)&preheatnames,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = true;

  //set preheat title properties
  lvIcon.lines[0].h_align = CENTER;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = LCD_WHITE;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = preheat_title;

  //set preheat tool propertites
  lvIcon.lines[1].h_align = RIGHT;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = LCD_WHITE;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = preheat_val_tool;

  //set preheat bed properties
  lvIcon.lines[2].h_align = RIGHT;
  lvIcon.lines[2].v_align = CENTER;
  lvIcon.lines[2].fn_color = LCD_WHITE;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = preheat_val_bed;

  lvIcon.lines[0].text = (u8 *)preheatnames.preheat_name[preheat_selected];

  char temptool[5];
  char tempbed[5];
  sprintf(temptool, "%d", infoSettings.preheat_temp[preheat_selected]);
  sprintf(tempbed, "%d", infoSettings.preheat_bed[preheat_selected]);
  lvIcon.lines[1].text = (u8 *)temptool;
  lvIcon.lines[2].text = (u8 *)tempbed;

  showLiveInfo(PREHEAT_TOGGLE_ICON, &lvIcon, &heatItems.items[PREHEAT_TOGGLE_ICON]);
}

void refreshHeatPage(void)
{
  heatItems.items[HEAT_TOGGLE_ICON] = itemTool[heater.tool];
  menuDrawPage(&heatItems);
  SMALLICON_ReadDisplay(rect_of_HeatMenu[DEC_BUTTON].x0, rect_of_HeatMenu[DEC_BUTTON].y0, SMALL_ICON_DEC);
  SMALLICON_ReadDisplay(rect_of_HeatMenu[NUM_BUTTON].x0, rect_of_HeatMenu[NUM_BUTTON].y0, SMALL_ICON_NUMPAD);
  SMALLICON_ReadDisplay(rect_of_HeatMenu[INC_BUTTON].x0, rect_of_HeatMenu[INC_BUTTON].y0, SMALL_ICON_INC);
  showTemperature(heater.tool);
  refreshPreheatIcon(preheat_selected);
  TSC_ReDrawIcon = IconRedraw;
}

void menuHeat(void)
{
  KEY_VALUES  key_num = KEY_IDLE;

  lastHeater = heater;
  update_time = TEMPERATURE_QUERY_FAST_DURATION;
  preheatTools = BOTH;

  refreshHeatPage();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    key_num = key_val();
    switch(key_num)
    {
      case KEY_ICON_0:
        lastHeater.tool = heater.tool = (TOOL)((heater.tool + 1) % (HEATER_COUNT));
        heatItems.items[key_num] = itemTool[heater.tool];
        menuDrawItem(&heatItems.items[key_num], key_num);
        heatSetCurrentToolNozzle(heater.tool);
        showTemperature(heater.tool);
        break;

      case KEY_ICON_3:
        heater.T[heater.tool].target = 0;
        break;

      case KEY_ICON_4:
        preheat_selected = (preheat_selected + 1) % PREHEAT_COUNT;
        refreshPreheatIcon(preheat_selected);
        break;

      case KEY_ICON_5:
        preheatTools = (preheatTools + 1) % PREHEAT_NUM;
        heatItems.items[key_num] = preheatItems[preheatTools];
        menuDrawItem(&heatItems.items[key_num], key_num);
        refreshPreheatIcon(preheat_selected);
        break;

      case KEY_ICON_6:
        switch(preheatTools)
        {
          case BOTH:
            heatSetTargetTemp(BED, infoSettings.preheat_bed[preheat_selected]);
            heatSetTargetTemp(heatGetCurrentToolNozzle(), infoSettings.preheat_temp[preheat_selected]);
            break;
          case BED_PREHEAT:
            heatSetTargetTemp(BED, infoSettings.preheat_bed[preheat_selected]);
            break;
          case NOZZLE_PREHEAT:
            heatSetTargetTemp(heatGetCurrentToolNozzle(), infoSettings.preheat_temp[preheat_selected]);
            break;
        }
        break;

    case DEC_BUTTON:
      heater.T[heater.tool].target =
          limitValue(0, heater.T[heater.tool].target - 1, infoSettings.max_temp[heater.tool]);
      break;

    case NUM_BUTTON:
      isEditing = true;
      ICON_CHAR charicon = (heater.tool == BED) ? ICONCHAR_BED : ICONCHAR_NOZZLE;
      char tempname[20];
      sprintf(tempname, "%s %s", (char *)IconCharSelect(charicon), heatDisplayID[heater.tool]);
      uint16_t v = numPadInt((u8 *)tempname, heater.T[heater.tool].target, false);
      isEditing = false;
      heatSetTargetTemp(heater.tool, v);
      refreshHeatPage();
      break;

    case INC_BUTTON:
      heater.T[heater.tool].target =
          limitValue(0, heater.T[heater.tool].target + 1, infoSettings.max_temp[heater.tool]);
      break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            if (heater.T[heater.tool].target < infoSettings.max_temp[heater.tool] && encoderPosition > 0)
            {
              heater.T[heater.tool].target =
                  limitValue(0, heater.T[heater.tool].target + 1, infoSettings.max_temp[heater.tool]);
            }
            if (heater.T[heater.tool].target > 0 && encoderPosition < 0)
            {
              heater.T[heater.tool].target =
                  limitValue(0, heater.T[heater.tool].target - 1, infoSettings.max_temp[heater.tool]);
            }
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if(lastHeater.T[heater.tool].current != heater.T[heater.tool].current || lastHeater.T[heater.tool].target != heater.T[heater.tool].target)
    {
      lastHeater.T[heater.tool].current = heater.T[heater.tool].current;
      showTemperature(heater.tool);
    }

    loopProcess();
  }

  if(heatHasWaiting() == false)
    update_time = TEMPERATURE_QUERY_SLOW_DURATION;
} //menuHeat

u32 nextHeatCheckTime = 0;
void updateNextHeatCheckTime(void)
{
  nextHeatCheckTime = OS_GetTimeMs() + update_time;
}


void loopCheckHeater(void)
{
  do
  {  /* Send M105 query temperature continuously  */
    if(update_waiting == true)                {updateNextHeatCheckTime();break;}
    if(OS_GetTimeMs() < nextHeatCheckTime)     break;
    if(RequestCommandInfoIsRunning())          break; //to avoid colision in Gcode response processing
    if(storeCmd("M105\n") == false)            break;
    updateNextHeatCheckTime();
    update_waiting = true;
  }while(0);

  /* Query the heater that needs to wait for the temperature to rise, whether it reaches the set temperature */
  for(u8 i=0; i< HEATER_COUNT; i++)
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

  for(TOOL i = BED; i < HEATER_COUNT; i++) // If the target temperature changes, send a Gcode to set the motherboard
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
