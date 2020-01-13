#include "Move.h"
#include "includes.h"


//const GUI_RECT RecXYZ = {START_X + 1*ICON_WIDTH,        STATUS_GANTRY_YOFFSET,
//                         4*ICON_WIDTH+3*SPACE_X+START_X,ICON_START_Y-STATUS_GANTRY_YOFFSET};
#ifdef MENU_LIST_MODE
    #define YGCODE_INC "G1 Y%.1f\n"
    #define YGCODE_DEC "G1 Y-%.1f\n"
    #define ZGCODE_INC "G1 Z%.1f\n"
    #define ZGCODE_DEC "G1 Z-%.1f\n"
#else
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
  
  #ifdef INVERT_ZAXIS
    #define LABEL_ZAXIS_UP LABEL_Z_DEC
    #define LABEL_ZAXIS_DOWN LABEL_Z_INC
    #define ZGCODE_UP "G1 Z-%.1f\n"
    #define ZGCODE_DOWN "G1 Z%.1f\n"
  #else
    #define LABEL_ZAXIS_UP LABEL_Z_INC
    #define LABEL_ZAXIS_DOWN LABEL_Z_DEC
    #define ZGCODE_UP "G1 Z%.1f\n"
    #define ZGCODE_DOWN "G1 Z-%.1f\n"
  #endif

#endif

//1 title, ITEM_PER_PAGE item
MENUITEMS moveItems = {
//  title
LABEL_MOVE,
// icon                       label
 { 
  #ifdef ALTERNATIVE_MOVE_MENU 
    #ifdef MENU_LIST_MODE
      {ICON_Z_DEC,                LABEL_Z_DEC},
      {ICON_Y_INC,                LABEL_Y_INC},
      {ICON_Z_INC,                LABEL_Z_INC},
      {ICON_1_MM,                 LABEL_1_MM},
      {ICON_X_DEC,                LABEL_X_DEC},
      {ICON_Y_DEC,                LABEL_Y_DEC},
      {ICON_X_INC,                LABEL_X_INC},
      {ICON_BACK,                 LABEL_BACK},
    #else
      {ICON_Z_DEC,                LABEL_Z_DEC},
      {ICON_Y_INC,                LABEL_YAXIS_UP},
      {ICON_Z_INC,                LABEL_Z_INC},
      {ICON_1_MM,                 LABEL_1_MM},
      {ICON_X_DEC,                LABEL_X_DEC},
      {ICON_Y_DEC,                LABEL_YAXIS_DOWN},
      {ICON_X_INC,                LABEL_X_INC},
      {ICON_BACK,                 LABEL_BACK},
    #endif
  #else
    #ifdef MENU_LIST_MODE  
    {ICON_X_INC,                LABEL_X_INC},
    {ICON_Y_INC,                LABEL_Y_INC},
    {ICON_Z_INC,                LABEL_Z_INC},
    {ICON_1_MM,                 LABEL_1_MM},
    {ICON_X_DEC,                LABEL_X_DEC},
    {ICON_Y_DEC,                LABEL_Y_DEC},
    {ICON_Z_DEC,                LABEL_Z_DEC},
    {ICON_BACK,                 LABEL_BACK},
    #else
    {ICON_X_INC,                LABEL_X_INC},
    {ICON_Y_INC,                LABEL_YAXIS_UP},
    {ICON_Z_INC,                LABEL_ZAXIS_UP},
    {ICON_1_MM,                 LABEL_1_MM},
    {ICON_X_DEC,                LABEL_X_DEC},
    {ICON_Y_DEC,                LABEL_YAXIS_DOWN},
    {ICON_Z_DEC,                LABEL_ZAXIS_DOWN},
    {ICON_BACK,                 LABEL_BACK},
    #endif
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


  #ifdef ALTERNATIVE_MOVE_MENU
    #ifdef MENU_LIST_MODE
      if(infoSettings.invert_yaxis == 1){
        moveItems.items[1].label.index = LABEL_Y_DEC;
        moveItems.items[5].label.index = LABEL_Y_INC;
      }
      else{
        moveItems.items[1].label.index = LABEL_Y_INC;
        moveItems.items[5].label.index = LABEL_Y_DEC;
      }
        if(infoSettings.invert_zaxis == 1){
        moveItems.items[0].label.index = LABEL_Z_INC;
        moveItems.items[2].label.index = LABEL_Z_DEC;
      }
      else{
        moveItems.items[0].label.index = LABEL_Z_DEC;
        moveItems.items[2].label.index = LABEL_Z_INC;
      }
    #endif
  #else
    #ifdef MENU_LIST_MODE
      if(infoSettings.invert_yaxis == 1){
        moveItems.items[1].label.index = LABEL_Y_DEC;
        moveItems.items[5].label.index = LABEL_Y_INC;
      }
      else{
        moveItems.items[1].label.index = LABEL_Y_INC;
        moveItems.items[5].label.index = LABEL_Y_DEC;
      }
        if(infoSettings.invert_zaxis == 1){
        moveItems.items[2].label.index = LABEL_Z_DEC;
        moveItems.items[6].label.index = LABEL_Z_INC;
      }
      else{
        moveItems.items[2].label.index = LABEL_Z_INC;
        moveItems.items[6].label.index = LABEL_Z_DEC;
      }
    #endif
  #endif

  menuDrawPage(&moveItems);
  mustStoreCmd("G91\n");

  #ifdef MENU_LIST_MODE
    switch (infoSettings.move_speed)
    {
    case 1:
      mustStoreCmd("G1 F%d\n",SPEED_MOVE_SLOW);
      break;
    case 2:
      mustStoreCmd("G1 F%d\n",SPEED_MOVE_FAST);
      break;
    default:
      mustStoreCmd("G1 F%d\n",DEFAULT_SPEED_MOVE);
      break;
    }
  #else
    mustStoreCmd("G1 F%d\n",DEFAULT_SPEED_MOVE);
  #endif
  mustStoreCmd("M114\n");
  drawXYZ();

  while(infoMenu.menu[infoMenu.cur] == menuMove)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      #ifdef ALTERNATIVE_MOVE_MENU
          
            case KEY_ICON_0:
                  #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_zaxis == 1){
                      storeCmd(ZGCODE_INC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(ZGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                  #else
                    storeCmd(ZGCODE_DOWN,   item_move_len[item_move_len_i]);  break;
                  #endif
                  break;

            case KEY_ICON_1:
                  #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_yaxis == 1){
                      storeCmd(YGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(YGCODE_INC, item_move_len[item_move_len_i]);
                    }
                  #else
                    storeCmd(YGCODE_UP, item_move_len[item_move_len_i]);
                  #endif
                  break;

            case KEY_ICON_2: 
                  #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_zaxis == 1){
                      storeCmd(ZGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(ZGCODE_INC, item_move_len[item_move_len_i]);
                    }
                  #else
                    storeCmd(ZGCODE_UP,   item_move_len[item_move_len_i]);  break;
                  #endif
                  break;

            case KEY_ICON_3: 
                  item_move_len_i = (item_move_len_i+1)%ITEM_MOVE_LEN_NUM;            
                  moveItems.items[key_num] = itemMoveLen[item_move_len_i];
                  menuDrawItem(&moveItems.items[key_num], key_num);
                  break;

            case KEY_ICON_4: storeCmd("G1 X-%.1f\n", item_move_len[item_move_len_i]);  break;

            case KEY_ICON_5:
                  #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_yaxis == 1){
                      storeCmd(YGCODE_INC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(YGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                  #else
                    storeCmd(YGCODE_DOWN, item_move_len[item_move_len_i]);
                  #endif
                  break;

            case KEY_ICON_6: storeCmd("G1 X%.1f\n",  item_move_len[item_move_len_i]);  break;
            case KEY_ICON_7: infoMenu.cur--; break;
            default:break; 
        
      #else

            case KEY_ICON_0: storeCmd("G1 X%.1f\n",  item_move_len[item_move_len_i]);  break;

            case KEY_ICON_1:
                  #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_yaxis == 1){
                      storeCmd(YGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(YGCODE_INC, item_move_len[item_move_len_i]);
                    }
                  #else
                    storeCmd(YGCODE_UP, item_move_len[item_move_len_i]);
                  #endif
                  break;

            case KEY_ICON_2:
            #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_zaxis == 1){
                      storeCmd(ZGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(ZGCODE_INC, item_move_len[item_move_len_i]);
                    }
                  #else
                  storeCmd(ZGCODE_UP,   item_move_len[item_move_len_i]);  break;
                  #endif
                  break;

            case KEY_ICON_3: 
                  item_move_len_i = (item_move_len_i+1)%ITEM_MOVE_LEN_NUM;            
                  moveItems.items[key_num] = itemMoveLen[item_move_len_i];
                  menuDrawItem(&moveItems.items[key_num], key_num);
                  break;

            case KEY_ICON_4: storeCmd("G1 X-%.1f\n", item_move_len[item_move_len_i]);  break;

            case KEY_ICON_5:
                  #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_yaxis == 1){
                      storeCmd(YGCODE_INC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(YGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                  #else
                    storeCmd(YGCODE_DOWN, item_move_len[item_move_len_i]);
                  #endif
                  break;

            case KEY_ICON_6: 
                  #ifdef MENU_LIST_MODE
                    if(infoSettings.invert_zaxis == 1){
                      storeCmd(ZGCODE_INC, item_move_len[item_move_len_i]);
                    }
                    else{
                      storeCmd(ZGCODE_DEC, item_move_len[item_move_len_i]);
                    }
                  #else
                    storeCmd(ZGCODE_DOWN,   item_move_len[item_move_len_i]);  break;
                  #endif
                  break;

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
  GUI_DispString(START_X+1*SPACE_X+1*ICON_WIDTH,(ICON_START_Y-BYTE_HEIGHT)/2,(u8 *)tempstr);
  my_sprintf(tempstr, "Y:%.1f  ", getAxisLocation(1));
  GUI_DispString(START_X+2*SPACE_X+2*ICON_WIDTH,(ICON_START_Y-BYTE_HEIGHT)/2,(u8 *)tempstr);
  my_sprintf(tempstr, "Z:%.1f  ", getAxisLocation(2));
  GUI_DispString(START_X+3*SPACE_X+3*ICON_WIDTH,(ICON_START_Y-BYTE_HEIGHT)/2,(u8 *)tempstr);
  
  //GUI_SetBkColor(BACKGROUND_COLOR);
  //GUI_SetColor(FONT_COLOR);
}
