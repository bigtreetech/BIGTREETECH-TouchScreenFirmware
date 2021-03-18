#include "LevelCorner.h"
#include "includes.h"

const MENUITEMS levelcornerItems = {
    // title
    LABEL_LEVELCORNER,
    // icon                         label
    {{ICON_LEVELCORNER4, LABEL_BACKGROUND},
     {ICON_RESET_VALUE, LABEL_START},
     {ICON_LEVELCORNER3, LABEL_BACKGROUND},
     {ICON_LEVEL_EDGE_DISTANCE, LABEL_DISTANCE},
     {ICON_LEVELCORNER1, LABEL_BACKGROUND},
     {ICON_BACKGROUND, LABEL_BACKGROUND},
     {ICON_LEVELCORNER2, LABEL_BACKGROUND},
     {ICON_BACK, LABEL_BACK}}
  };

void ScanLevelCorner(u8 pointer)
{
  s16 pointPosition[4][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge}, 
  };
  mustStoreCmd("M401\n");
  mustStoreCmd("G30 E0 X%d Y%d\n", (s16)pointPosition[pointer][0], (s16)pointPosition[pointer][1]);
  mustStoreCmd("G1 Z15\n");
  mustStoreCmd("M17 X Y Z\n");

}

void refreshLevelCornerValue(MENUITEMS levelItems) 
{
  char tempstr[10];
  int valIndex[4] = {4,6,2,0};
  int valPos;
  int valPosSub;
  LIVE_INFO lvIcon;
  lvIcon.lines[0].pos = ss_val_point;
  lvIcon.lines[1].pos = ss_val_point;
  lvIcon.lines[2].pos = ss_val_point;
  lvIcon.lines[3].pos = ss_val_point;

  if ( (int)GetLevelCornerPosition(0) != 0)
  {
    if (((int)GetLevelCornerPosition(0) >= 1) && ((int)GetLevelCornerPosition(0) <= 4) )
    {
      valPos = (int)GetLevelCornerPosition(0);
      valPosSub = (int)GetLevelCornerPosition(0)-1;     
      sprintf(tempstr, "  %1.4f  ", GetLevelCornerPosition(valPos));
      lvIcon.lines[valPosSub].text = (uint8_t*)tempstr;
      showLevelCornerLiveInfo(valIndex[valPosSub], valPosSub, &lvIcon, &levelItems.items[valIndex[valPosSub]]); 
      SetLevelCornerPosition(0, 0);  
    }
  } 
}

void menuLevelCorner(void)
{ 
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&levelcornerItems);

  /* Init Probe Offset in parseAck to get probe offset X and Y*/
  mustStoreCmd("M851\n");
  
  /* Init Coordinate */
  if (coordinateIsKnown() == false) 
  {
    mustStoreCmd("G28\n");
  }
  /* Check min edge limit for the probe with probe offset set in parseACK.c */
  uint8_t edge_min = MAX(ABS(getParameter((s16)P_PROBE_OFFSET, X_STEPPER)),ABS((s16)getParameter(P_PROBE_OFFSET, Y_STEPPER))) + 1;
  if (infoSettings.level_edge < edge_min) 
  {
    infoSettings.level_edge = ((LEVELING_EDGE_DISTANCE >= edge_min)?LEVELING_EDGE_DISTANCE:edge_min);
  }
  
  /* Scan all 4 corner */
  ScanLevelCorner(0);
  ScanLevelCorner(1);
  ScanLevelCorner(2);
  ScanLevelCorner(3);

  while (infoMenu.menu[infoMenu.cur] == menuLevelCorner)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        ScanLevelCorner(3);
        break;
      case KEY_ICON_1:
        ScanLevelCorner(0);
        ScanLevelCorner(1);
        ScanLevelCorner(2);
        ScanLevelCorner(3);
        break;
      case KEY_ICON_2:
        ScanLevelCorner(2);
        break;
      case KEY_ICON_3:
        {
          char tempstr[30];         
          sprintf(tempstr, "%Min:%d | Max:%d", edge_min, LEVELING_EDGE_DISTANCE_MAX);
          int val = numPadInt((u8 *)tempstr, infoSettings.level_edge, LEVELING_EDGE_DISTANCE_DEFAULT, false);
          infoSettings.level_edge = NOBEYOND(LEVELING_EDGE_DISTANCE_MIN, val, LEVELING_EDGE_DISTANCE_MAX);
          infoSettings.level_edge = ((val >= edge_min)?val:edge_min);
          menuDrawPage(&levelcornerItems);
        }
        break;
      case KEY_ICON_4:
        ScanLevelCorner(0);
        break;
      case KEY_ICON_6:
        ScanLevelCorner(1);
        break;
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default:
        break;
    }
    refreshLevelCornerValue(levelcornerItems);    
    loopProcess();
  }
}