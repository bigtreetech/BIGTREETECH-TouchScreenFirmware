#include "Extrude.h"
#include "includes.h"

//1��title(����), ITEM_PER_PAGE��item(ͼ��+��ǩ)
MENUITEMS extrudeItems = {
//   title
LABEL_EXTRUDE,
// icon                       label
 {{ICON_UNLOAD,               LABEL_UNLOAD},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_LOAD,                 LABEL_LOAD},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
  {ICON_E_5_MM,               LABEL_5_MM},
  {ICON_BACK,                 LABEL_BACK},}
};

#define ITEM_EXTRUDER_NUM EXTRUDER_NUM
//const ITEM itemExtruder[ITEM_EXTRUDER_NUM] = {
//// icon                       label
//  {ICON_SLOW_SPEED,           LABEL_SLOW_SPEED},
//  {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
//  {ICON_FAST_SPEED,           LABEL_FAST_SPEED},
//};
//const  u32 item_extruder[ITEM_EXTRUDER_NUM] = {EXTRUDE_SLOW_SPEED, EXTRUDE_NORMAL_SPEED, EXTRUDE_FAST_SPEED};
static u8  item_extruder_i = 0;

#define ITEM_SPEED_NUM 3
const ITEM itemSpeed[ITEM_SPEED_NUM] = {
// icon                       label
  {ICON_SLOW_SPEED,           LABEL_SLOW_SPEED},
  {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
  {ICON_FAST_SPEED,           LABEL_FAST_SPEED},
};
const  u32 item_speed[ITEM_SPEED_NUM] = {EXTRUDE_SLOW_SPEED, EXTRUDE_NORMAL_SPEED, EXTRUDE_FAST_SPEED};
static u8  item_speed_i = 1;

#define ITEM_LEN_NUM 3
const ITEM itemLen[ITEM_LEN_NUM] = {
// icon                       label
  {ICON_E_1_MM,               LABEL_1_MM},
  {ICON_E_5_MM,               LABEL_5_MM},
  {ICON_E_10_MM,              LABEL_10_MM},
};
const  u8 item_len[ITEM_LEN_NUM] = {1, 5, 10};
static u8 item_len_i = 1;

static float extrudeCoordinate = 0.0f;

void extrudeCoordinateReDraw(void)
{
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y, exhibitRect.x1, CENTER_Y+BYTE_HEIGHT};
  char buf[36];
  my_sprintf(buf, "%.2f", extrudeCoordinate);
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)buf);
}

const char* tool_change[] =  TOOL_CHANGE;
const char* extruderDisplayID[] = EXTRUDER_ID;

void showExtrudeCoordinate(void)
{
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y-BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)extruderDisplayID[item_extruder_i]);
  extrudeCoordinateReDraw();
}

void menuExtrude(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  float eSaved = 0.0f;
  float eTemp  = 0.0f;
  bool  eRelative = false;
  u32   feedrate = 0;

  while(infoCmd.count != 0) {loopProcess();}
  extrudeCoordinate = eTemp = eSaved = coordinateGetAxisTarget(E_AXIS);
  feedrate = coordinateGetFeedRate();
  eRelative = eGetRelative();

  menuDrawPage(&extrudeItems);
  showExtrudeCoordinate();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif
  if(eRelative) mustStoreCmd("M82\n"); // Set extruder to absolute
  while(infoMenu.menu[infoMenu.cur] == menuExtrude)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        eTemp -= item_len[item_len_i];
        break;

      case KEY_ICON_3:
        eTemp += item_len[item_len_i];
        break;

      case KEY_ICON_4:
        item_extruder_i = (item_extruder_i + 1) % ITEM_EXTRUDER_NUM;
        showExtrudeCoordinate();
        break;

      case KEY_ICON_5:
        item_speed_i = (item_speed_i+1) % ITEM_SPEED_NUM;
        extrudeItems.items[key_num] = itemSpeed[item_speed_i];
        menuDrawItem(&extrudeItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        item_len_i = (item_len_i+1) % ITEM_LEN_NUM;
        extrudeItems.items[key_num] = itemLen[item_len_i];
        menuDrawItem(&extrudeItems.items[key_num], key_num);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            eTemp += item_len[item_len_i]*encoderPosition;
            encoderPosition = 0;
          }
        #endif
        break;
    }
    if(extrudeCoordinate != eTemp)
    {
      extrudeCoordinate = eTemp;
      extrudeCoordinateReDraw();
      if(item_extruder_i != heatGetCurrentToolNozzle() - NOZZLE0)
        storeCmd("%s\n", tool_change[item_extruder_i]);
      storeCmd("G0 E%.5f F%d\n", extrudeCoordinate, item_speed[item_speed_i]);
    }
    loopProcess();
  }
  mustStoreCmd("G92 E%.5f\n",eSaved);
  mustStoreCmd("G0 F%d\n",feedrate);
  if(eRelative) mustStoreCmd("M83\n"); // Set extruder to relative
}
