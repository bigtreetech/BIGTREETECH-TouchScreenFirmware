#include "Move.h"
#include "includes.h"

//1 title, ITEM_PER_PAGE item
MENUITEMS moveItems = {
//  title
LABEL_MOVE,
// icon                       label
 { 
  #ifdef ALTERNATIVE_MOVE_MENU 
  {ICON_Z_DEC,                LABEL_Z_DEC},
  {ICON_Y_INC,                LABEL_Y_INC},
  {ICON_Z_INC,                LABEL_Z_INC},
  {ICON_1_MM,                 LABEL_1_MM},
  {ICON_X_DEC,                LABEL_X_DEC},
  {ICON_Y_DEC,                LABEL_Y_DEC},
  {ICON_X_INC,                LABEL_X_INC},
  {ICON_BACK,                 LABEL_BACK},

  #else
  
  {ICON_X_INC,                LABEL_X_INC},
  {ICON_Y_INC,                LABEL_Y_INC},
  {ICON_Z_INC,                LABEL_Z_INC},
  {ICON_1_MM,                 LABEL_1_MM},
  {ICON_X_DEC,                LABEL_X_DEC},
  {ICON_Y_DEC,                LABEL_Y_DEC},
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

void menuMove(void)
{
  KEY_VALUES  key_num = KEY_IDLE;

  menuDrawPage(&moveItems,false);
  mustStoreCmd("G91\n");
  mustStoreCmd("G1 F%d\n",DEFAULT_SPEED_MOVE);
  while(infoMenu.menu[infoMenu.cur] == menuMove)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      #ifdef ALTERNATIVE_MOVE_MENU
      case KEY_ICON_0: storeCmd("G1 Z-%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_1: storeCmd("G1 Y%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_2: storeCmd("G1 Z%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_3: 
        item_move_len_i = (item_move_len_i+1)%ITEM_MOVE_LEN_NUM;            
        moveItems.items[key_num] = itemMoveLen[item_move_len_i];
        menuDrawItem(&moveItems.items[key_num], key_num);                         break;
      case KEY_ICON_4: storeCmd("G1 X-%.1f\n", item_move_len[item_move_len_i]);   break;
      case KEY_ICON_5: storeCmd("G1 Y-%.1f\n",   item_move_len[item_move_len_i]); break;
      case KEY_ICON_6: storeCmd("G1 X%.1f\n",   item_move_len[item_move_len_i]); break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default:break; 
      
    #else
 
      case KEY_ICON_0: storeCmd("G1 X%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_1: storeCmd("G1 Y%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_2: storeCmd("G1 Z%.1f\n",   item_move_len[item_move_len_i]);  break;
      case KEY_ICON_3: 
        item_move_len_i = (item_move_len_i+1)%ITEM_MOVE_LEN_NUM;            
        moveItems.items[key_num] = itemMoveLen[item_move_len_i];
        menuDrawItem(&moveItems.items[key_num], key_num);                         break;
      case KEY_ICON_4: storeCmd("G1 X-%.1f\n", item_move_len[item_move_len_i]);   break;
      case KEY_ICON_5: storeCmd("G1 Y-%.1f\n",   item_move_len[item_move_len_i]); break;
      case KEY_ICON_6: storeCmd("G1 Z-%.1f\n",   item_move_len[item_move_len_i]); break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default:break;  
      #endif            
    }
    loopProcess();
  }
  mustStoreCmd("G90\n");
}
