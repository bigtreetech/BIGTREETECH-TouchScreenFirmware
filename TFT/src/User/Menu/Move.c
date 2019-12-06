#include "Move.h"
#include "includes.h"


//const GUI_RECT RecXYZ = {START_X + 1*ICON_WIDTH,        STATUS_GANTRY_YOFFSET,
//                         4*ICON_WIDTH+3*SPACE_X+START_X,TITLE_END_Y-STATUS_GANTRY_YOFFSET};

#ifdef INVERT_YAXIS
#define LABEL_YAXIS_UP LABEL_Y_DEC
#define LABEL_YAXIS_DOWN LABEL_Y_INC
#define YGCODE_UP "G1 Y-%.1f\n"
#define YGCODE_DOWN "G1 Y%.1f\n"
#else
#define LABEL_YAXIS_UP LABEL_Y_INC
#define LABEL_YAXIS_DOWN LABEL_Y_DEC
#define YGCODE_UP "G1 Y%.1f\n"
#define YGCODE_DOWN "G1 Y-%.1f\n"
#endif
//1 title, ITEM_PER_PAGE item
MENUITEMS moveItems = {
//  title
LABEL_MOVE,
// icon                       label
 { 
  #ifdef ALTERNATIVE_MOVE_MENU 
  {ICON_Z_DEC,                LABEL_Z_DEC},
  {ICON_Y_INC,                LABEL_YAXIS_UP},
  {ICON_Z_INC,                LABEL_Z_INC},
  {ICON_1_MM,                 LABEL_1_MM},
  {ICON_X_DEC,                LABEL_X_DEC},
  {ICON_Y_DEC,                LABEL_YAXIS_DOWN},
  {ICON_X_INC,                LABEL_X_INC},
  {ICON_BACK,                 LABEL_BACK},

  #else
  
  {ICON_X_INC,                LABEL_X_INC},
  {ICON_Y_INC,                LABEL_YAXIS_UP},
  {ICON_Z_INC,                LABEL_Z_INC},
  {ICON_1_MM,                 LABEL_1_MM},
  {ICON_X_DEC,                LABEL_X_DEC},
  {ICON_Y_DEC,                LABEL_YAXIS_DOWN},
  {ICON_Z_DEC,                LABEL_Z_DEC},
  {ICON_BACK,                 LABEL_BACK},
  #endif
 }
};


#define ITEM_MOVE_LEN_NUM 3
const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM] = {
// icon                       label
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
  {ICON_10_MM,                LABEL_10_MM},
};
const  float item_move_len[ITEM_MOVE_LEN_NUM] = {0.1f, 1, 10};
static u8    item_move_len_i = 1;

static u32 nowTime = 0;
static u32 update_time = 50; // 1 seconds is 100

void menuMove(void)
{
  KEY_VALUES  key_num = KEY_IDLE;

  menuDrawPage(&moveItems,false);
  mustStoreCmd("G91\n");
  mustStoreCmd("G1 F%d\n",DEFAULT_SPEED_MOVE);
  mustStoreCmd("M114\n");
  drawXYZ();

  while(infoMenu.menu[infoMenu.cur] == menuMove)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      #ifdef ALTERNATIVE_MOVE_MENU
      case KEY_ICON_0: storeCmd("G1 Z-%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_1: storeCmd(YGCODE_UP,   item_move_len[item_move_len_i]);    break;
      case KEY_ICON_2: storeCmd("G1 Z%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_3: 
        item_move_len_i = (item_move_len_i+1)%ITEM_MOVE_LEN_NUM;            
        moveItems.items[key_num] = itemMoveLen[item_move_len_i];
        menuDrawItem(&moveItems.items[key_num], key_num);                        break;
      case KEY_ICON_4: storeCmd("G1 X-%.1f\n", item_move_len[item_move_len_i]);  break;
      case KEY_ICON_5: storeCmd(YGCODE_DOWN, item_move_len[item_move_len_i]);    break;
      case KEY_ICON_6: storeCmd("G1 X%.1f\n",  item_move_len[item_move_len_i]);  break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default:break; 
      
    #else
 
      case KEY_ICON_0: storeCmd("G1 X%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_1: storeCmd(YGCODE_UP,   item_move_len[item_move_len_i]);     break;
      case KEY_ICON_2: storeCmd("G1 Z%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_3: 
        item_move_len_i = (item_move_len_i+1)%ITEM_MOVE_LEN_NUM;            
        moveItems.items[key_num] = itemMoveLen[item_move_len_i];
        menuDrawItem(&moveItems.items[key_num], key_num);                         break;
      case KEY_ICON_4: storeCmd("G1 X-%.1f\n", item_move_len[item_move_len_i]);   break;
      case KEY_ICON_5: storeCmd(YGCODE_DOWN,   item_move_len[item_move_len_i]);   break;
      case KEY_ICON_6: storeCmd("G1 Z-%.1f\n",   item_move_len[item_move_len_i]); break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default:break;  
      #endif
            
    }
    loopProcess();
    update_gantry();
  }
  mustStoreCmd("G90\n");
}

void update_gantry(void)
{
  if (OS_GetTime() > nowTime + update_time)
  {
    if (infoHost.connected == true && infoHost.wait == false){
      storeCmd("M114\n");
    }
    drawXYZ();
    nowTime = OS_GetTime();
  }
}
void drawXYZ(void){
  char tempstr[100];
  //GUI_SetColor(GANTRYLBL_BKCOLOR);
  //GUI_FillPrect(&RecXYZ);
  my_sprintf(tempstr, "X:%.1f  ", getAxisLocation(0));  
  GUI_DispString(START_X+1*SPACE_X+1*ICON_WIDTH,(TITLE_END_Y-BYTE_HEIGHT)/2,(u8 *)tempstr);
  my_sprintf(tempstr, "Y:%.1f  ", getAxisLocation(1));
  GUI_DispString(START_X+2*SPACE_X+2*ICON_WIDTH,(TITLE_END_Y-BYTE_HEIGHT)/2,(u8 *)tempstr);
  my_sprintf(tempstr, "Z:%.1f  ", getAxisLocation(2));
  GUI_DispString(START_X+3*SPACE_X+3*ICON_WIDTH,(TITLE_END_Y-BYTE_HEIGHT)/2,(u8 *)tempstr);
  
  //GUI_SetBkColor(BK_COLOR);
  //GUI_SetColor(FK_COLOR);
}
