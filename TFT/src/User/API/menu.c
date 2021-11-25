#include "menu.h"
#include "includes.h"
#include "ListItem.h"
#include "Notification.h"

// exhibitRect is 2 ICON Space in the Upper Row and 2 Center column.
const GUI_RECT exhibitRect = {
  1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y
};

// MENU FOR A TEMPERATURE DISPLAY SCREEN (TOOL TEMP, PID, ETC...)
const GUI_RECT rect_of_keysIN[TM_RECT_COUNT] = {
  // 8 icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  // 8 labels area
  {0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},

  // title bar area
  {0, 0, LCD_WIDTH, ICON_START_Y},
  // display area
  {1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y}
};

const GUI_RECT rect_of_key[MENU_RECT_COUNT] = {
  // 8 icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  // 8 labels area
  {0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},

  // title bar area
  {0, 0, LCD_WIDTH, ICON_START_Y},
};

const GUI_RECT rect_of_keySS[SS_RECT_COUNT] = {

#ifdef TFT70_V3_0

  // 5 status icons area
  {0*SSICON_WIDTH+0*SS_SPACE_X+START_X,  0*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*SSICON_WIDTH+0*SS_SPACE_X+START_X,  1*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {1*SSICON_WIDTH+1*SS_SPACE_X+START_X,  0*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*SSICON_WIDTH+1*SS_SPACE_X+START_X,  1*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {2*SSICON_WIDTH+2*SS_SPACE_X+START_X,  0*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*SSICON_WIDTH+2*SS_SPACE_X+START_X,  1*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {3*SSICON_WIDTH+3*SS_SPACE_X+START_X,  0*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*SSICON_WIDTH+3*SS_SPACE_X+START_X,  1*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {4*SSICON_WIDTH+4*SS_SPACE_X+START_X,  0*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  5*SSICON_WIDTH+4*SS_SPACE_X+START_X,  1*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  // 2nd icon row area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {0, 0, 0, 0},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  // 8 labels area
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},

  // title bar area
  {0, 0, LCD_WIDTH, ICON_START_Y},

  // infobox area
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y}

#else

  // 8 icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y},
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  // 8 labels area
  {0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {1*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {2*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},
  {3*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*SPACE_X_PER_ICON,  2*ICON_HEIGHT+2*SPACE_Y+ICON_START_Y},

  // title bar area
  {0, 0, LCD_WIDTH, ICON_START_Y},

  // infobox area
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

#endif

};

const GUI_RECT rect_of_keyPS[] = {
  // hotend area
  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0,                  PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*0+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*0},
  // bed area
  {START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1,                  PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*1+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*0},
  // fan area
  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2,                  PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_SM_WIDTH*1, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*0},
  // timer area
  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0,                  PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*0+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*2+PICON_SPACE_Y*1},
  // layer area
  {START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1,                  PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*1+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*2+PICON_SPACE_Y*1},
  // speed area
  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2,                  PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_SM_WIDTH*1, PICON_START_Y+PICON_HEIGHT*2+PICON_SPACE_Y*1},

  // 4 bottom icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  2*ICON_WIDTH+1*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {2*ICON_WIDTH+2*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  // title bar area
  {0, 0, LCD_WIDTH, ICON_START_Y},
};

const GUI_RECT rect_of_keyPS_end[] = {
  // hotend area
  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0,                  PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*0+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*0},
  // bed area
  {START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1,                  PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*1+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*0},
  // fan area
  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2,                  PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_SM_WIDTH*1, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*0},
  // timer area
  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0,                  PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*0+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*2+PICON_SPACE_Y*1},
  // layer area
  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0,                  PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*1+PICON_SM_WIDTH*0, PICON_START_Y+PICON_HEIGHT*2+PICON_SPACE_Y*1},
  // speed area
  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2,                  PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_SM_WIDTH*1, PICON_START_Y+PICON_HEIGHT*2+PICON_SPACE_Y*1},

  // 4 bottom icons area
  {0*ICON_WIDTH+0*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  1*ICON_WIDTH+0*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
  {0,0,0,0},
  {0,0,0,0},
  {3*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  4*ICON_WIDTH+3*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},

  // title bar area
  {0, 0, LCD_WIDTH, ICON_START_Y},

  // infobox
  {1*ICON_WIDTH+1*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y,  3*ICON_WIDTH+2*SPACE_X+START_X,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y},
};

const GUI_RECT rect_of_keyListView[ITEM_PER_PAGE + 1] = {
  // 8 icons area
  {START_X,  0*LISTITEM_HEIGHT+ICON_START_Y,  LISTITEM_WIDTH+START_X,  1*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  1*LISTITEM_HEIGHT+ICON_START_Y,  LISTITEM_WIDTH+START_X,  2*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  2*LISTITEM_HEIGHT+ICON_START_Y,  LISTITEM_WIDTH+START_X,  3*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  3*LISTITEM_HEIGHT+ICON_START_Y,  LISTITEM_WIDTH+START_X,  4*LISTITEM_HEIGHT+ICON_START_Y},
  {START_X,  4*LISTITEM_HEIGHT+ICON_START_Y,  LISTITEM_WIDTH+START_X,  5*LISTITEM_HEIGHT+ICON_START_Y},

  {2*START_X + LISTITEM_WIDTH,  0*LIST_ICON_HEIGHT+0*LISTICON_SPACE_Y+ICON_START_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  1*LIST_ICON_HEIGHT+0*LISTICON_SPACE_Y+ICON_START_Y},
  {2*START_X + LISTITEM_WIDTH,  1*LIST_ICON_HEIGHT+1*LISTICON_SPACE_Y+ICON_START_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  2*LIST_ICON_HEIGHT+1*LISTICON_SPACE_Y+ICON_START_Y},
  {2*START_X + LISTITEM_WIDTH,  2*LIST_ICON_HEIGHT+2*LISTICON_SPACE_Y+ICON_START_Y,  2*START_X + LISTITEM_WIDTH + 1*LIST_ICON_WIDTH,  3*LIST_ICON_HEIGHT+2*LISTICON_SPACE_Y+ICON_START_Y},

  // title bar area
  {0, 0, LCD_WIDTH, ICON_START_Y}
};

// titlebar touch area
const GUI_RECT rect_of_titleBar[] = {
  {0, 0, LCD_WIDTH, ICON_START_Y}
};

// Clean up the gaps outside icons
#ifdef TFT70_V3_0

void menuClearGaps(void)
{
  const GUI_RECT gapsMenu[]={
  {0,                  0,                  LCD_WIDTH,          TITLE_END_Y},
  {0,                  TITLE_END_Y,        LCD_WIDTH,          rect_of_key[0].y0},
  {0,                  rect_of_key[0].y0,  rect_of_key[0].x0,  LCD_HEIGHT},
  {rect_of_key[0].x1,  rect_of_key[0].y0,  rect_of_key[1].x0,  LCD_HEIGHT},
  {rect_of_key[1].x1,  rect_of_key[0].y0,  rect_of_key[2].x0,  LCD_HEIGHT},
  {rect_of_key[2].x1,  rect_of_key[0].y0,  rect_of_key[3].x0,  LCD_HEIGHT},
  {rect_of_key[3].x1,  rect_of_key[0].y0,  LCD_WIDTH,          LCD_HEIGHT}};

  const GUI_RECT gapsSS[]={
  {0,                   0,                    LCD_WIDTH,            TITLE_END_Y},
  {0,                   TITLE_END_Y,          LCD_WIDTH,            rect_of_keySS[0].y0},
  {0,                   rect_of_keySS[0].y0,  rect_of_keySS[0].x0,  LCD_HEIGHT},
  {rect_of_keySS[4].y0, rect_of_keySS[0].y0,  LCD_WIDTH,            LCD_HEIGHT},
  // 1ST ROW
  {rect_of_keySS[0].x1,  rect_of_keySS[0].y0,  rect_of_keySS[1].x0,  rect_of_keySS[0].y1},
  {rect_of_keySS[1].x1,  rect_of_keySS[0].y0,  rect_of_keySS[2].x0,  rect_of_keySS[0].y1},
  {rect_of_keySS[2].x1,  rect_of_keySS[0].y0,  rect_of_keySS[3].x0,  rect_of_keySS[0].y1},
  {rect_of_keySS[3].x1,  rect_of_keySS[0].y0,  rect_of_keySS[4].x0,  rect_of_keySS[0].y1},
  // gantry rect
  {rect_of_keySS[0].x0,  rect_of_keySS[0].y1,  rect_of_keySS[7].x0,  rect_of_keySS[7].y0},
  // 2ND ROW
  {rect_of_keySS[5].x1,  rect_of_keySS[5].y0,  rect_of_keySS[17].x0,  rect_of_keySS[5].y1},
  {rect_of_keySS[17].x1,  rect_of_keySS[5].y0,  rect_of_keySS[7].x0,  rect_of_keySS[5].y1}};

  GUI_SetBkColor(infoSettings.title_bg_color);

  if (MENU_IS(menuStatus) || ((MENU_IS(menuPrinting)) && !isPrinting()))
    GUI_ClearPrect(gapsSS);
  else
    GUI_ClearPrect(gapsMenu);

  GUI_SetBkColor(infoSettings.bg_color);

  if (MENU_IS(menuStatus) || ((MENU_IS(menuPrinting)) && !isPrinting()))
  {
    for (uint8_t i = 1; i < COUNT(gapsSS); i++)
    {
      GUI_ClearPrect(gapsSS + i);
    }
  }
  else
  {
    for (uint8_t i = 1; i < COUNT(gapsMenu); i++)
    {
      GUI_ClearPrect(gapsMenu + i);
    }
  }
}

#else

void menuClearGaps(void)
{
  const GUI_RECT gaps[] = {
    {0,                  0,                  LCD_WIDTH,          TITLE_END_Y},
    {0,                  TITLE_END_Y,        LCD_WIDTH,          rect_of_key[0].y0},
    {0,                  rect_of_key[0].y0,  rect_of_key[0].x0,  LCD_HEIGHT},
    {rect_of_key[0].x1,  rect_of_key[0].y0,  rect_of_key[1].x0,  LCD_HEIGHT},
    {rect_of_key[1].x1,  rect_of_key[0].y0,  rect_of_key[2].x0,  LCD_HEIGHT},
    {rect_of_key[2].x1,  rect_of_key[0].y0,  rect_of_key[3].x0,  LCD_HEIGHT},
    {rect_of_key[3].x1,  rect_of_key[0].y0,  LCD_WIDTH,          LCD_HEIGHT}
  };

  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_ClearPrect(gaps);
  GUI_SetBkColor(infoSettings.bg_color);
  for (uint8_t i = 1; i < COUNT(gaps); i++)
    GUI_ClearPrect(gaps + i);
}

#endif

void GUI_RestoreColorDefault(void)
{
  GUI_SetBkColor(infoSettings.bg_color);
  GUI_SetColor(infoSettings.font_color);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  setFontSize(FONT_SIZE_NORMAL);
}

static const MENUITEMS *curMenuItems = NULL;  // current menu
static const LISTITEMS *curListItems = NULL;  // current listmenu

static const void (* curMenuRedrawHandle)(void) = NULL;  // current custom menu

static MENU_TYPE menuType = MENU_TYPE_ICON;
static LABEL * curTitle = NULL;
static const GUI_RECT *curRect = NULL;  // current menu layout grid
static uint16_t curRectCount = 0;       // current menu layout rect count

static REMINDER reminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_DISCONNECTED, LABEL_UNCONNECTED};
static REMINDER volumeReminder = {{0, 0, LCD_WIDTH, TITLE_END_Y}, 0, STATUS_IDLE, LABEL_BACKGROUND};
static REMINDER busySign = {{LCD_WIDTH - 5, 0, LCD_WIDTH, 5}, 0, STATUS_BUSY, LABEL_BUSY};

MENUITEMS *getCurMenuItems(void)
{
  return (MENUITEMS *)curMenuItems;
}

LISTITEMS *getCurListItems(void)
{
  return (LISTITEMS *)curListItems;
}

// Get the top left point of the corresponding icon position)
GUI_POINT getIconStartPoint(int index)
{
  GUI_POINT p = {curRect[index].x0, curRect[index].y0};
  return p;
}

uint8_t *labelGetAddress(const LABEL *label)
{
  if (label->index == LABEL_BACKGROUND)  // No content in label
    return NULL;
  if (label->index < LABEL_NUM)  // Index of language
    return textSelect(label->index);
  else  // Address of string
    return label->address;
}

void menuDrawItem(const ITEM *item, uint8_t position)
{
  menuDrawIconOnly(item, position);
  menuDrawIconText(item, position);
}

void menuDrawIconOnly(const ITEM *item, uint8_t position)
{
  const GUI_RECT *rect = curRect + position;
  if (item->icon != ICON_BACKGROUND)
    ICON_ReadDisplay(rect->x0, rect->y0, item->icon);
  else
    GUI_ClearPrect(rect);
}

void menuDrawIconText(const ITEM *item, uint8_t position)
{
  uint8_t *content = labelGetAddress(&item->label);
  const GUI_RECT *rect = curRect + ITEM_PER_PAGE + position;
  GUI_ClearPrect(rect);
  if (content)
    GUI_DispStringInPrect(rect, content);
}

void menuDrawListItem(const LISTITEM *item, uint8_t position)
{
  const GUI_RECT *rect = rect_of_keyListView + position;
  if (item->icon == CHARICON_BACKGROUND)
  {
    GUI_ClearPrect(rect);
  }
  else
  {
    ListItem_Display(rect, position, item, false);
  }
}

void menuRefreshListPage(void)
{
  for (uint8_t i = 0; i < ITEM_PER_PAGE; i++)
  {
    RAPID_PRINTING_COMM()  // perform backend printing loop between drawing icons to avoid printer idling
    menuDrawListItem(&curListItems->items[i], i);
  }
}

void setMenuType(MENU_TYPE type)
{
  menuType = type;
}

MENU_TYPE getMenuType(void)
{
  return menuType;
}

void setMenu(MENU_TYPE menu_type, LABEL * title, uint16_t rectCount, const GUI_RECT * menuRect,
             void(*action_redraw)(uint8_t position, uint8_t is_press),
             void (*menu_redraw)(void))
{
  menuType = menu_type;
  curRect = menuRect;
  curRectCount = rectCount;
  curTitle = title;
  TSC_ReDrawIcon = action_redraw;
  curMenuRedrawHandle = menu_redraw;

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif
}

void reminderMessage(int16_t inf, SYS_STATUS status)
{
  if (toastRunning()) return;

  reminder.inf = inf;
  reminder.status = status;
  reminder.time = OS_GetTimeMs() + 2000;  // 2 seconds

  if (menuType != MENU_TYPE_FULLSCREEN)
  {
    GUI_SetColor(infoSettings.reminder_color);
    GUI_SetBkColor(infoSettings.title_bg_color);
    GUI_DispStringInPrect(&reminder.rect, reminder.inf);
    GUI_RestoreColorDefault();
  }
}

void volumeReminderMessage(int16_t inf, SYS_STATUS status)
{
  LCD_WAKE();

  if (toastRunning()) return;

  volumeReminder.inf = inf;
  volumeReminder.status = status;
  volumeReminder.time = OS_GetTimeMs() + 2000;

  if (menuType != MENU_TYPE_FULLSCREEN)
  {
    GUI_SetColor(infoSettings.status_color);
    GUI_SetBkColor(infoSettings.title_bg_color);
    GUI_DispStringInPrect(&volumeReminder.rect, volumeReminder.inf);
    GUI_RestoreColorDefault();
  }
}

void busyIndicator(SYS_STATUS status)
{
  if (status == STATUS_BUSY)
  {
    GUI_SetColor(YELLOW);
    GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0) / 2);
    GUI_SetColor(infoSettings.font_color);
  }
  busySign.status = status;
  busySign.time = OS_GetTimeMs() + 2000;
}

void loopReminderClear(void)
{
  switch (reminder.status)
  {
    case STATUS_IDLE:
      return;

    case STATUS_BUSY:
      if (isFullCmdQueue())
        return;
      break;

    case STATUS_DISCONNECTED:
      if (infoHost.connected == false)
        return;
      break;

    case STATUS_LISTENING:
      if (GET_BIT(infoSettings.general_settings, INDEX_LISTENING_MODE) == 1)
        return;
      break;

    case STATUS_NORMAL:
      if (OS_GetTimeMs() < reminder.time)
        return;
      break;

    default:
      return;
  }

  reminder.status = STATUS_IDLE;  // Clear status message
  menuReDrawCurTitle();
}

void loopVolumeReminderClear(void)
{
  if (volumeReminder.status != STATUS_NORMAL)
  {
    return;
  }
  else if (OS_GetTimeMs() < volumeReminder.time)
  {
    return;
  }

  volumeReminder.status = STATUS_IDLE;  // Clear status message
  menuReDrawCurTitle();
}

void loopBusySignClear(void)
{
  switch (busySign.status)
  {
    case STATUS_IDLE:
      return;

    case STATUS_BUSY:
     if (OS_GetTimeMs() < busySign.time)
        return;
     break;
  }

  busySign.status = STATUS_IDLE;  // clear busy signal status

  if (menuType == MENU_TYPE_FULLSCREEN)
    curMenuRedrawHandle();
  else
  {
    GUI_SetColor(infoSettings.title_bg_color);
    GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
    GUI_SetColor(infoSettings.font_color);
  }
}

void notificationDot(void)
{
  if (hasNotification())
  {
    GUI_SetColor(infoSettings.font_color);
  }
  else
  {
    GUI_SetColor(infoSettings.title_bg_color);
  }
  GUI_FillCircle(3, 3, 3);
  GUI_RestoreColorDefault();
}

void menuDrawTitle(const uint8_t *content)
{
  if (menuType == MENU_TYPE_FULLSCREEN) return;
  if (toastRunning())
  {
    drawToast(true);
    return;
  }
  uint16_t start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;
  uint16_t start_x = 10;
  uint16_t end_x = drawTemperatureStatus();
  GUI_SetBkColor(infoSettings.title_bg_color);
  if (content)
  {
    GUI_DispLenString(10, start_y, content, LCD_WIDTH - 20, true);
    start_x += GUI_StrPixelWidth(content);
    if (start_x > LCD_WIDTH-20) start_x = LCD_WIDTH - 20;
  }
  GUI_ClearRect(start_x, start_y, end_x, start_y+BYTE_HEIGHT);

  notificationDot();
  GUI_SetBkColor(infoSettings.bg_color);
  if (reminder.status == STATUS_IDLE) return;
  GUI_SetColor(infoSettings.reminder_color);
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_DispStringInPrect(&reminder.rect, reminder.inf);
  GUI_RestoreColorDefault();
}

void menuReDrawCurTitle(void)
{
  if (menuType == MENU_TYPE_LISTVIEW)
  {
    if (curListItems == NULL)
      return;
    if (curListItems->title.index < LABEL_BACKGROUND)
      menuDrawTitle(labelGetAddress(&curListItems->title));
  }
  else if (menuType == MENU_TYPE_ICON)
  {
    if (curMenuItems == NULL)
      return;
    menuDrawTitle(labelGetAddress(&curMenuItems->title));
  }
  else if (menuType == MENU_TYPE_FULLSCREEN)
  {
    if (curMenuRedrawHandle != NULL)
      curMenuRedrawHandle();
  }
  else if (menuType == MENU_TYPE_OTHER)
  {
    menuDrawTitle(labelGetAddress(curTitle));
  }
}

// Draw the entire interface
void menuDrawPage(const MENUITEMS *menuItems)
{
  uint8_t i = 0;
  menuType = MENU_TYPE_ICON;
  curMenuItems = menuItems;
  TSC_ReDrawIcon = (MENU_IS(menuPrinting)) ? itemDrawIconPress_PS : itemDrawIconPress;
  curMenuRedrawHandle = NULL;

  curRect = (MENU_IS(menuStatus)) ? rect_of_keySS : rect_of_key;

  menuClearGaps();  // Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
  menuDrawTitle(labelGetAddress(&menuItems->title));

  for (i = 0; i < ITEM_PER_PAGE; i++)
  {
    menuDrawItem(&menuItems->items[i], i);
    RAPID_PRINTING_COMM()  // perform backend printing loop between drawing icons to avoid printer idling
  }

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif
}

// Draw the entire interface
void menuDrawListPage(const LISTITEMS *listItems)
{
  uint8_t i = 0;
  menuType = MENU_TYPE_LISTVIEW;
  curListItems = listItems;
  TSC_ReDrawIcon = itemDrawIconPress;
  curMenuRedrawHandle = NULL;

  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
  GUI_SetBkColor(infoSettings.bg_color);
  GUI_ClearRect(0, TITLE_END_Y, LCD_WIDTH, LCD_HEIGHT);

  //menuClearGaps();  // Use this function instead of GUI_Clear to eliminate the splash screen when clearing the screen.
  menuDrawTitle(labelGetAddress(&listItems->title));

  for (i = 0; i < ITEM_PER_PAGE; i++)
  {
    //const GUI_RECT *rect = rect_of_keyListView + i;
    if (curListItems->items[i].icon != CHARICON_BACKGROUND)
      menuDrawListItem(&curListItems->items[i], i);
    RAPID_PRINTING_COMM()  // perform backend printing loop between drawing icons to avoid printer idling
  }

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif
}

// Show live info text on icons
void showLiveInfo(uint8_t index, const LIVE_INFO * liveicon, const ITEM * item)
{
  if (item != NULL)
    ICON_PrepareRead(curRect[index].x0, curRect[index].y0, item->icon);

  for (uint8_t i = 0; i < LIVEICON_LINES; i++)
  {
    if (liveicon->enabled[i] == true)
    {
      GUI_POINT loc;
      loc.x = liveicon->lines[i].pos.x + curRect[index].x0;

      if (liveicon->lines[i].v_align == BOTTOM)
      {
        loc.y = liveicon->lines[i].pos.y + curRect[index].y0 - BYTE_HEIGHT;
      }
      else if (liveicon->lines[i].v_align == CENTER)
      {
        loc.y = liveicon->lines[i].pos.y + curRect[index].y0 - BYTE_HEIGHT / 2;
      }
      else
      {
        loc.y = liveicon->lines[i].pos.y + curRect[index].y0;
      }

      if (item == NULL)  // if no icon used as background, set own bg color and text mode
      {
        GUI_SetTextMode(liveicon->lines[i].text_mode);
        GUI_SetBkColor(liveicon->lines[i].bk_color);
      }

      GUI_SetColor(liveicon->lines[i].fn_color);
      setFontSize(liveicon->lines[i].font);

      switch (liveicon->lines[i].h_align)
      {
        case LEFT:
          GUI_DispString(loc.x, loc.y, liveicon->lines[i].text);
          break;

        case CENTER:
          GUI_DispStringCenter(loc.x, loc.y, liveicon->lines[i].text);
          break;

        case RIGHT:
          GUI_DispStringRight(loc.x, loc.y, liveicon->lines[i].text);
          break;

        default:
          break;
      }
    }
  }

  GUI_RestoreColorDefault();

  if (item != NULL)
    ICON_PrepareReadEnd();
}  // showLiveInfo

void displayExhibitHeader(const char * titleStr, const char * unitStr)
{
  // draw header title
  if (titleStr != NULL)
  {
    char tempstr[20];
    sprintf(tempstr, "%-8s", titleStr);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tempstr);
  }

  // draw unit string
  if (unitStr != NULL)
  {
    setFontSize(FONT_SIZE_LARGE);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)unitStr);
  }

  setFontSize(FONT_SIZE_NORMAL);
}

void displayExhibitValue(const char * valueStr)
{
  setFontSize(FONT_SIZE_LARGE);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) valueStr);
  setFontSize(FONT_SIZE_NORMAL);
}

// When there is a button value, the icon changes color and redraws
void itemDrawIconPress(uint8_t position, uint8_t is_press)
{
  if (position > KEY_ICON_7) return;

  if (menuType == MENU_TYPE_ICON)
  {
    if (curMenuItems == NULL) return;
    if (curMenuItems->items[position].icon == ICON_BACKGROUND) return;

    const GUI_RECT *rect = curRect + position;

    if (is_press)  // Turn green when pressed
      ICON_PressedDisplay(rect->x0, rect->y0, curMenuItems->items[position].icon);
    else  // Redraw normal icon when released
      ICON_ReadDisplay(rect->x0, rect->y0,curMenuItems->items[position].icon);
  }
  else if (menuType == MENU_TYPE_LISTVIEW)
  { // draw rec over list item if pressed
    if (curListItems == NULL)
      return;

    const GUI_RECT *rect = rect_of_keyListView + position;

    if (curListItems->items[position].icon == CHARICON_BACKGROUND)
    {
      GUI_ClearPrect(rect);
      return;
    }
    if (is_press)
      ListItem_Display(rect,position,&curListItems->items[position], true);
    else
      ListItem_Display(rect,position,&curListItems->items[position], false);
  }
}

// When there is a button value, the icon changes color and redraws
void itemDrawIconPress_PS(uint8_t position, uint8_t is_press)
{
  if (position < PS_KEY_6 || position > PS_KEY_9) return;
  position -= PS_TOUCH_OFFSET;

  const GUI_RECT *rect = curRect + position;

  if (is_press)  // Turn green when pressed
    ICON_PressedDisplay(rect->x0, rect->y0, curMenuItems->items[position].icon);
  else  // Redraw normal icon when released
    ICON_ReadDisplay(rect->x0, rect->y0,curMenuItems->items[position].icon);
}

// Get button value
KEY_VALUES menuKeyGetValue(void)
{
  KEY_VALUES tempkey = KEY_IDLE;

  if (tempkey == KEY_IDLE)
  {
    switch (menuType)
    {
      case MENU_TYPE_ICON:
        {
          if (MENU_IS(menuStatus))
          {
            tempkey = (KEY_VALUES)KEY_GetValue(COUNT(rect_of_keySS), rect_of_keySS);
          }
          else if(MENU_IS(menuPrinting))
          {
            if(isPrinting() || infoHost.printing == true)
              tempkey = (KEY_VALUES)KEY_GetValue(COUNT(rect_of_keyPS), rect_of_keyPS);
            else
              tempkey = (KEY_VALUES)KEY_GetValue(COUNT(rect_of_keyPS_end), rect_of_keyPS_end);

            if (tempkey == (KEY_VALUES)PS_KEY_TITLEBAR)
              tempkey = KEY_TITLEBAR;
          }
          else if ((MENU_IS(menuHeat)) ||
                   (MENU_IS(menuLoadUnload)) ||
                   (MENU_IS(menuPid)) ||
                   (MENU_IS(menuTuneExtruder)) ||
                   (MENU_IS(menuFan)) ||
                   (MENU_IS(menuExtrude)) ||
                   (MENU_IS(menuSpeed)) ||
                   (MENU_IS(menuZOffset)) ||
                   (MENU_IS(menuMBL)))
          {
            tempkey = (KEY_VALUES)KEY_GetValue(COUNT(rect_of_keysIN), rect_of_keysIN);
          }
          else
          {
            tempkey = (KEY_VALUES)KEY_GetValue(COUNT(rect_of_key), rect_of_key);
          }
        }
        break;

      case MENU_TYPE_LISTVIEW:
        tempkey = (KEY_VALUES)KEY_GetValue(COUNT(rect_of_keyListView), rect_of_keyListView);

        if (tempkey == ITEM_PER_PAGE)
          tempkey = KEY_TITLEBAR;
        break;

      case MENU_TYPE_OTHER:
        if ((KEY_VALUES)KEY_GetValue(1, rect_of_titleBar) == 0)
          tempkey = KEY_TITLEBAR;
        else
          tempkey = (KEY_VALUES)KEY_GetValue(curRectCount, curRect);
        break;

      case MENU_TYPE_FULLSCREEN:
      default:
        tempkey = (KEY_VALUES)KEY_GetValue(curRectCount, curRect);
        break;
    }
  }

  if (menuType != MENU_TYPE_FULLSCREEN && tempkey == KEY_TITLEBAR)
  {
    titleBarPress();
    tempkey = KEY_IDLE;
  }

  #if LCD_ENCODER_SUPPORT
    if (tempkey == KEY_IDLE)
      tempkey = LCD_Enc_KeyValue();
  #endif

  return tempkey;
}

// Smart home (long press on back button to go to status screen)
#ifdef SMART_HOME

void loopCheckBackPress(void)
{
  static bool longPress = false;

  #ifdef HAS_EMULATOR
    static bool backHeld = false;
  #endif

  if (!isPress())
  {
    longPress = false;

    #ifdef HAS_EMULATOR
      backHeld = false;
    #else
      Touch_Enc_ReadPen(0);  // reset TSC press timer
    #endif

    return;
  }

  if (isPrinting())  // no jump to main menu while printing
    return;

  if (getMenuType() != MENU_TYPE_ICON)
    return;

  if ((infoMenu.cur == 0) || (MENU_IS(menuMode)))
    return;

  #ifdef HAS_EMULATOR
    if (backHeld == true)  // prevent mode selection or screenshot if Back button is held
    {
      backHeld = Touch_Enc_ReadPen(0);
      return;
    }
  #endif

  if (longPress == false)  // check if longpress already handled
  {
    if (Touch_Enc_ReadPen(LONG_TOUCH))  // check if TSC is pressed and held
    {
      KEY_VALUES tempKey = KEY_IDLE;
      longPress = true;
      touchSound = false;

      if (MENU_IS(menuPrinting))
      {
        tempKey = Key_value(COUNT(rect_of_keySS), rect_of_keySS);
      }
      else
      {
        tempKey = Key_value(COUNT(rect_of_key), rect_of_key);
      }

      touchSound = true;

      if (tempKey != KEY_IDLE)
      {
        if (getCurMenuItems()->items[tempKey].label.index != LABEL_BACK)  // check if Back button is held
        {
          return;
        }
        else
        {
          BUZZER_PLAY(SOUND_OK);

          #ifdef HAS_EMULATOR
            backHeld = true;
          #endif

          infoMenu.menu[1] = infoMenu.menu[infoMenu.cur];  // prepare menu tree for jump to 0
          infoMenu.cur = 1;

          if (infoMenu.menu[1] == menuPrinting)
            clearInfoFile();
        }
      }
    }
  }
}

#endif  // SMART_HOME

// Non-UI background loop tasks
void loopBackEnd(void)
{
  // Get Gcode command from the file to be printed
  loopPrintFromTFT();  // handle a print from TFT, if any
  // Parse and send Gcode commands in the queue
  sendQueueCmd();
  // Parse the received slave response information
  parseACK();

  if (GET_BIT(infoSettings.general_settings, INDEX_FILE_COMMENT_PARSING) == 1)  // if file comment parsing is enabled
  {
    parseComment();  // Parse comment from gCode file
  }

  #ifdef SERIAL_PORT_2
    // Parse the received Gcode from other UART, such as: ESP3D, etc...
    parseRcvGcode();
  #endif

  // Temperature monitor
  loopCheckHeater();
  // Fan speed monitor
  loopFan();
  // Speed & flow monitor
  loopSpeed();

  #ifdef BUZZER_PIN
    // Buzzer handling
    loopBuzzer();
  #endif

  if (infoMachineSettings.onboardSD == ENABLED)
  {
    loopPrintFromOnboardSD();  // handle a print from (remote) onboard SD, if any
  }

  #ifdef U_DISK_SUPPORT
    USBH_Process(&USB_OTG_Core, &USB_Host);
  #endif

  #ifdef FIL_RUNOUT_PIN
    FIL_BE_CheckRunout();
  #endif

  #if LCD_ENCODER_SUPPORT
    #ifdef HAS_EMULATOR
      if (MENU_IS_NOT(menuMarlinMode))
    #endif
    {
      LCD_Enc_CheckSteps();  // check change in encoder steps
    }
  #endif

  #ifdef HAS_EMULATOR
    Mode_CheckSwitching();
  #endif

  #ifdef SCREEN_SHOT_TO_SD
    loopScreenShot();
  #endif

  #ifdef SMART_HOME
    // check if Back is pressed and held
    loopCheckBackPress();
  #endif

  #ifdef LCD_LED_PWM_CHANNEL
    LCD_CheckDimming();
  #endif

  if (infoMachineSettings.caseLightsBrightness == ENABLED)
  {
    loopCaseLight();
  }

  // Query RRF status
  rrfStatusQuery();
}  // loopBackEnd

// UI-related background loop tasks
void loopFrontEnd(void)
{
  // Check if volume source(SD/U disk) insert
  loopVolumeSource();
  // Loop to check and run toast messages
  loopToast();
  // If there is a message in the status bar, timed clear
  loopReminderClear();
  loopVolumeReminderClear();
  // Busy Indicator clear
  loopBusySignClear();
  // Check update temperature status
  loopTemperatureStatus();

  #ifdef FIL_RUNOUT_PIN
    // Loop for filament runout detection
    FIL_FE_CheckRunout();
  #endif

  // Loop for popup menu
  loopPopup();
}

void loopProcess(void)
{
  loopBackEnd();
  loopFrontEnd();
}

void menuDummy(void)
{
  CLOSE_MENU();
}

void loopProcessToCondition(CONDITION_CALLBACK condCallback)
{
  uint8_t curMenu = infoMenu.cur;
  bool invokedUI = false;

  while (condCallback())  // loop until the condition is no more satisfied
  {
    loopProcess();

    if (infoMenu.cur > curMenu)  // if a user interaction is needed (e.g. dialog box UI), handle it
    {
      invokedUI = true;
      (*infoMenu.menu[infoMenu.cur])();
    }
  }

  if (invokedUI)  // if a UI was invoked, load a dummy menu just to force the caller also to refresh its menu
    OPEN_MENU(menuDummy);
}
