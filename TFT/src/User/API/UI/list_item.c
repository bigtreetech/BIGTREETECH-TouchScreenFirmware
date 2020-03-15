// *List View Mode ** Author: Gurmeet Athwal https://github.com/guruathwal **

#include "list_item.h"
#include "includes.h"
#include "menu.h"
#include "GUI.h"

char * dynamic_label[LISTITEM_PER_PAGE];

float dynamic_value[LISTITEM_PER_PAGE];

const uint16_t ICON_COLOR[ICONCHAR_NUM]=
{
  BLACK,     //ICONCHAR_BLANK = 0
  WHITE,    //ICONCHAR_NOZZLE
  WHITE,    //ICONCHAR_BED
  WHITE,    //ICONCHAR_FAN
  MAT_YELLOW,    //ICONCHAR_FOLDER
  MAT_BLUE,    //ICONCHAR_FILE
  WHITE,    //ICONCHAR_PAGEUP
  WHITE,    //ICONCHAR_PAGEDOWN
  WHITE,    //ICONCHAR_BACK
  WHITE,    //ICONCHAR_LEFT
  WHITE,    //ICONCHAR_LEFT_TOP
  WHITE,    //ICONCHAR_LEFT_BOTTOM
  WHITE,    //ICONCHAR_RIGHT
  WHITE,    //ICONCHAR_RIGHT_TOP
  WHITE,    //ICONCHAR_RIGHT_BOTTOM
  MAT_YELLOW,    //ICONCHAR_ALERT
  MAT_RED,    //ICONCHAR_WARNING
  MAT_RED,    //ICONCHAR_ERROR
  MAT_YELLOW,    //ICONCHAR_CAUTION
  MAT_BLUE,    //ICONCHAR_INFO
  MAT_YELLOW,    //ICONCHAR_HAND
  MAT_YELLOW,    //ICONCHAR_WAIT
  MAT_BLUE,    //ICONCHAR_QUESTION
  WHITE,    //ICONCHAR_PLAY
  WHITE,    //ICONCHAR_PAUSE
  WHITE,    //ICONCHAR_STOP
  WHITE,    //ICONCHAR_EJECT
  WHITE,    //ICONCHAR_PLAY_ROUND
  WHITE,    //ICONCHAR_PAUSE_ROUND
  WHITE,    //ICONCHAR_OK_ROUND
  WHITE,    //ICONCHAR_CANCEL_ROUND
  WHITE,    //ICONCHAR_MINUS_ROUND
  WHITE,    //ICONCHAR_PLUS_ROUND
  WHITE,    //ICONCHAR_MINUS
  WHITE,    //ICONCHAR_PLUS
  WHITE,    //ICONCHAR_OK
  WHITE,    //ICONCHAR_CANCEL
  MAT_RED,    //ICONCHAR_HALT
  WHITE,    //ICONCHAR_UP_DOWN
  WHITE,    //ICONCHAR_LEFT_RIGHT
  WHITE,    //ICONCHAR_POINT_LEFT
  WHITE,    //ICONCHAR_POINT_RIGHT
  WHITE,    //ICONCHAR_RETURN
  WHITE,    //ICONCHAR_REDO
  WHITE,    //ICONCHAR_UNDO
  WHITE,    //ICONCHAR_DOWNLOAD
  WHITE,    //ICONCHAR_UPLOAD
  WHITE,    //ICONCHAR_BULLET
  WHITE,    //ICONCHAR_BACK_SMALL
  WHITE,    //ICONCHAR_EXPAND
  WHITE,    //ICONCHAR_MOVE
  WHITE,    //ICONCHAR_ROTATE
  WHITE,    //ICONCHAR_RESET
  WHITE,    //ICONCHAR_EDIT
  WHITE,    //ICONCHAR_SAVE
  MAT_ORANGE,    //ICONCHAR_DELETE
  MAT_LOWWHITE,    //ICONCHAR_RADIO_CHECKED
  MAT_LOWWHITE,    //ICONCHAR_RADIO_UNCHECKED
  MAT_LOWWHITE,    //ICONCHAR_CHECKED
  MAT_LOWWHITE,    //ICONCHAR_UNCHECKED
  MAT_RED,    //ICONCHAR_SOUND_OFF
  MAT_GREEN,    //ICONCHAR_SOUND_ON
  MAT_RED,    //ICONCHAR_ALERT_OFF
  MAT_GREEN,    //ICONCHAR_ALERT_ON
  MAT_RED,    //ICONCHAR_POWER_OFF
  MAT_GREEN,    //ICONCHAR_POWER_ON
  MAT_RED,    //ICONCHAR_WIFI_OFF
  MAT_GREEN,    //ICONCHAR_WIFI_ON
  WHITE,    //ICONCHAR_KEYBOARD
  MAT_RED,    //ICONCHAR_SD_ERROR
  MAT_GREEN,    //ICONCHAR_SD_OK
  MAT_BLUE,    //ICONCHAR_SD_REFRESH
  MAT_RED,    //ICONCHAR_USB_ERROR
  MAT_GREEN,    //ICONCHAR_USB_OK
  MAT_RED,    //ICONCHAR_PRINTER_ERROR
  MAT_GREEN,    //ICONCHAR_PRINTER_OK
  WHITE,    //ICONCHAR_BOARD
  WHITE,    //ICONCHAR_EEPROM
  WHITE,    //ICONCHAR_PRINT
  WHITE,    //ICONCHAR_SETTING1
  WHITE,    //ICONCHAR_PLUGIN
  WHITE,    //ICONCHAR_FEATURE
  WHITE,    //ICONCHAR_SETTING2
  WHITE,    //ICONCHAR_DETAIL
  WHITE,    //ICONCHAR_DETAIL2
  WHITE,    //ICONCHAR_ADJUST
  WHITE,    //ICONCHAR_MENU
  WHITE,    //ICONCHAR_POWER
  WHITE,    //ICONCHAR_TOUCH
  WHITE,    //ICONCHAR_LANGUAGE
  WHITE,    //ICONCHAR_CODE
  WHITE,    //ICONCHAR_POWER_PLUG
  WHITE,    //ICONCHAR_ROTATE_DEVICE
  WHITE,    //ICONCHAR_WINDOW
  WHITE,    //ICONCHAR_BACKGROUND_COLOR
  WHITE,    //ICONCHAR_FONT_COLOR
  WHITE,    //ICONCHAR_PAINT
  MAT_GREEN,      //ICONCHAR_TOGGLE_SMALL_ON
  MAT_RED,        //ICONCHAR_TOGGLE_SMALL_OFF
  MAT_GREEN,      //ICONCHAR_TOGGLE_ON
  MAT_RED,        //ICONCHAR_TOGGLE_OFF
  MATT_DARKGRAY,  //ICONCHAR_TOGGLE_BODY
  WHITE,          //ICONCHAR_TOGGLE_SWITCH


};

#define CHAR_BLANK            ""
#define CHAR_NOZZLE           "࡟"
#define CHAR_BED              "ࡠ"
#define CHAR_FAN              "ࡡ"
#define CHAR_FOLDER           "ࡢ"
#define CHAR_FILE             "ࡣ"

#define CHAR_PAGEUP           "ࡧࡨ"
#define CHAR_PAGEDOWN         "ࡩࡪ"
#define CHAR_BACK             "࡫࡬"
#define CHAR_LEFT             ""
#define CHAR_LEFT_PART1       "࡭"
#define CHAR_LEFT_PART2       "࡮"
#define CHAR_RIGHT            ""
#define CHAR_RIGHT_PART1      "࡯"
#define CHAR_RIGHT_PART2      "ࡰ"

#define CHAR_ALERT            "ࢀ"
#define CHAR_WARNING          "ࢁ"
#define CHAR_ERROR            "ࢂ"
#define CHAR_CAUTION          "ࢃ"
#define CHAR_INFO             "ࢄ"
#define CHAR_HAND             "ࢅ"
#define CHAR_WAIT             "ࢆ"
#define CHAR_QUESTION         "ࢇ"
#define CHAR_PLAY             "࢈"
#define CHAR_PAUSE            "ࢉ"
#define CHAR_STOP             "ࢊ"
#define CHAR_EJECT            "ࢋ"
#define CHAR_PLAY_ROUND       "ࢌ"
#define CHAR_PAUSE_ROUND      "ࢍ"
#define CHAR_OK_ROUND         "ࢎ"
#define CHAR_CANCEL_ROUND     "࢏"
#define CHAR_MINUS_ROUND      "࢐"
#define CHAR_PLUS_ROUND       "࢑"
#define CHAR_MINUS            "࢒"
#define CHAR_PLUS             "࢓"
#define CHAR_OK               "࢔"
#define CHAR_CANCEL           "࢕"
#define CHAR_HALT             "࢖"
#define CHAR_UP_DOWN          "ࢗ"
#define CHAR_LEFT_RIGHT       "࢘"
#define CHAR_POINT_LEFT       "࢙"
#define CHAR_POINT_RIGHT      "࢚"
#define CHAR_RETURN           "࢛"
#define CHAR_REDO             "࢜"
#define CHAR_UNDO             "࢝"
#define CHAR_DOWNLOAD         "࢞"
#define CHAR_UPLOAD           "࢟"
#define CHAR_BULLET           "ࢠ"
#define CHAR_BACK_SMALL       "ࢡ"
#define CHAR_EXPAND           "ࢢ"
#define CHAR_MOVE             "ࢣ"
#define CHAR_ROTATE           "ࢤ"
#define CHAR_RESET            "ࢥ"
#define CHAR_EDIT             "ࢦ"
#define CHAR_SAVE             "ࢧ"
#define CHAR_DELETE           "ࢨ"
#define CHAR_RADIO_CHECKED     "ࢩ"
#define CHAR_RADIO_UNCHECKED   "ࢪ"
#define CHAR_CHECKED          "ࢫ"
#define CHAR_UNCHECKED        "ࢬ"
#define CHAR_SOUND_OFF        "ࢭ"
#define CHAR_SOUND_ON        "ࢮ"
#define CHAR_ALERT_OFF        "ࢯ"
#define CHAR_ALERT_ON         "ࢰ"
#define CHAR_POWER_OFF        "ࢱ"
#define CHAR_POWER_ON         "ࢲ"
#define CHAR_WIFI_OFF         "ࢳ"
#define CHAR_WIFI_ON          "ࢴ"
#define CHAR_KEYBOARD         "ࢵ"
#define CHAR_SD_ERROR         "ࢶ"
#define CHAR_SD_OK            "ࢷ"
#define CHAR_SD_REFRESH       "ࢸ"
#define CHAR_USB_ERROR        "ࢹ"
#define CHAR_USB_OK           "ࢺ"
#define CHAR_PRINTER_ERROR    "ࢻ"
#define CHAR_PRINTER_OK       "ࢼ"
#define CHAR_BOARD            "ࢽ"
#define CHAR_EEPROM           "ࢾ"
#define CHAR_PRINT            "ࢿ"
#define CHAR_SETTING1         "ࣀ"
#define CHAR_PLUGIN           "ࣁ"
#define CHAR_FEATURE          "ࣂ"
#define CHAR_SETTING2         "ࣃ"
#define CHAR_DETAIL           "ࣄ"
#define CHAR_DETAIL2          "ࣅ"
#define CHAR_ADJUST           "ࣆ"
#define CHAR_MENU             "ࣇ"
#define CHAR_POWER            "ࣈ"
#define CHAR_TOUCH            "ࣉ"
#define CHAR_LANGUAGE         "࣊"
#define CHAR_CODE             "࣋"
#define CHAR_POWER_PLUG       "࣌"
#define CHAR_ROTATE_DEVICE    "࣍"
#define CHAR_WINDOW           "࣎"
#define CHAR_BACKGROUND_COLOR "࣏"
#define CHAR_FONT_COLOR       "࣐"
#define CHAR_PAINT            "࣑"
#define CHAR_TOGGLE_SMALL_ON  "࣒"
#define CHAR_TOGGLE_SMALL_OFF "࣓"
#define CHAR_TOGGLE_ON         "ࣖ"
#define CHAR_TOGGLE_OFF        "ࣖ"
#define CHAR_TOGGLE_BODY       "ࣔࣕ"
#define CHAR_TOGGLE_SWITCH     "ࣖ"

const char *const GET_ICONCHAR[ICONCHAR_NUM]={
  CHAR_BLANK,
  CHAR_NOZZLE,
  CHAR_BED,
  CHAR_FAN,
  CHAR_FOLDER,
  CHAR_FILE,
  CHAR_PAGEUP,
  CHAR_PAGEDOWN,
  CHAR_BACK,
  CHAR_LEFT,
  CHAR_LEFT_PART1,
  CHAR_LEFT_PART2,
  CHAR_LEFT,
  CHAR_RIGHT_PART1,
  CHAR_RIGHT_PART2,
  CHAR_ALERT,
  CHAR_WARNING,
  CHAR_ERROR,
  CHAR_CAUTION,
  CHAR_INFO,
  CHAR_HAND,
  CHAR_WAIT,
  CHAR_QUESTION,
  CHAR_PLAY,
  CHAR_PAUSE,
  CHAR_STOP,
  CHAR_EJECT,
  CHAR_PLAY_ROUND,
  CHAR_PAUSE_ROUND,
  CHAR_OK_ROUND,
  CHAR_CANCEL_ROUND,
  CHAR_MINUS_ROUND,
  CHAR_PLUS_ROUND,
  CHAR_MINUS,
  CHAR_PLUS,
  CHAR_OK,
  CHAR_CANCEL,
  CHAR_HALT,
  CHAR_UP_DOWN,
  CHAR_LEFT_RIGHT,
  CHAR_POINT_LEFT,
  CHAR_POINT_RIGHT,
  CHAR_RETURN,
  CHAR_REDO,
  CHAR_UNDO,
  CHAR_DOWNLOAD,
  CHAR_UPLOAD,
  CHAR_BULLET,
  CHAR_BACK_SMALL,
  CHAR_EXPAND,
  CHAR_MOVE,
  CHAR_ROTATE,
  CHAR_RESET,
  CHAR_EDIT,
  CHAR_SAVE,
  CHAR_DELETE,
  CHAR_RADIO_CHECKED,
  CHAR_RADIO_UNCHECKED,
  CHAR_CHECKED,
  CHAR_UNCHECKED,
  CHAR_SOUND_OFF,
  CHAR_SOUND_ON,
  CHAR_ALERT_OFF,
  CHAR_ALERT_ON,
  CHAR_POWER_OFF,
  CHAR_POWER_ON,
  CHAR_WIFI_OFF,
  CHAR_WIFI_ON,
  CHAR_KEYBOARD,
  CHAR_SD_ERROR,
  CHAR_SD_OK,
  CHAR_SD_REFRESH,
  CHAR_USB_ERROR,
  CHAR_USB_OK,
  CHAR_PRINTER_ERROR,
  CHAR_PRINTER_OK,
  CHAR_BOARD,
  CHAR_EEPROM,
  CHAR_PRINT,
  CHAR_SETTING1,
  CHAR_PLUGIN,
  CHAR_FEATURE,
  CHAR_SETTING2,
  CHAR_DETAIL,
  CHAR_DETAIL2,
  CHAR_ADJUST,
  CHAR_MENU,
  CHAR_POWER,
  CHAR_TOUCH,
  CHAR_LANGUAGE,
  CHAR_CODE,
  CHAR_POWER_PLUG,
  CHAR_ROTATE_DEVICE,
  CHAR_WINDOW,
  CHAR_BACKGROUND_COLOR,
  CHAR_FONT_COLOR,
  CHAR_PAINT,
  CHAR_TOGGLE_SMALL_ON,
  CHAR_TOGGLE_SMALL_OFF,
  CHAR_TOGGLE_ON,
  CHAR_TOGGLE_OFF,
  CHAR_TOGGLE_BODY,
  CHAR_TOGGLE_SWITCH,

};


uint8_t * IconCharSelect(uint8_t sel)
{
return (uint8_t *)GET_ICONCHAR[sel];
}
char * IconChar(uint8_t sel)
{
return (char *)GET_ICONCHAR[sel];
}

// save dynamic text label ( i : index of the label position, label: char * to the text)
void setDynamicLabel(uint8_t i, char *label){
  dynamic_label[i] = label;
}

// get dynamic text label ( i : index of the label position)
char * getDynamicLabel(uint8_t i){
  return dynamic_label[i];
}

// save dynamic value ( i : index of the value position, value:float value)
void setDynamicValue(uint8_t i,float value){
dynamic_value[i] = value;
}

// get dynamic numerical value ( i : index of the value position)
float getDynamicValue(uint8_t i){
  return dynamic_value[i];
}

// get the text starting point on screen based on rectangle edges and desired icon position
GUI_POINT getTextStartPoint(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, ICON_POS pos, const char * textchar){

  GUI_POINT point_item = {sx, sy};
  uint16_t w = ex- sx;
  uint16_t h = ey- sy;

  size_t charIcon_w = strlen(textchar)/3 * BYTE_HEIGHT;

  switch (pos)
  {
  case TOP_LEFT:
    point_item.x = sx + 1;
    point_item.y = sy + 1;
    break;

  case TOP_CENTER:
    point_item.x = (sx + (w - charIcon_w) / 2);
    point_item.y = sy + 1;
    break;

  case TOP_RIGHT:
    point_item.x = ex - charIcon_w - 1;
    point_item.y = sy + 1;
    break;

  case LEFT_CENTER:
    point_item.x = sx + 1;
    point_item.y = (sy + (h - BYTE_HEIGHT) / 2);
    break;

  case MIDDLE:
    point_item.x = (sx +(w - charIcon_w) / 2);
    point_item.y = (sy +(h - BYTE_HEIGHT) / 2);
    break;

  case RIGHT_CENTER:
    point_item.x = ex - charIcon_w - 1;
    point_item.y = (sy +(h - BYTE_HEIGHT) / 2);
    break;

  case BOTTOM_LEFT:
    point_item.x = sx + 1;
    point_item.y = sy + h - BYTE_HEIGHT - 1;
    break;

  case BOTTOM_CENTER:
    point_item.x = (sx +(w - charIcon_w) / 2);
    point_item.y = sy + h - BYTE_HEIGHT - 1;
    break;

  case BOTTOM_RIGHT:
    point_item.x = sx + w - charIcon_w - 1;
    point_item.y = sy + h - BYTE_HEIGHT - 1;
    break;

  default:
    point_item.x = sx + 1;
    point_item.y = sy + 1;
    break;
  }
  return point_item;
}

//draw icons in item
void DrawCharIcon(const GUI_RECT * rect,ICON_POS iconalign, uint16_t iconindex, uint16_t btn_color){

      GUI_POINT icon_p = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,iconalign,GET_ICONCHAR[iconindex]);
      GUI_SetColor(ICON_COLOR[iconindex]);
      GUI_SetBkColor(btn_color);
      GUI_ClearPrect(rect);
      GUI_DispString(icon_p.x, icon_p.y, IconCharSelect(iconindex));
      GUI_RestoreColorDefault();

}

//draw item pressed feedback
void DrawListItemPress(const GUI_RECT * rect, bool pressed){
      if (pressed != false)
      {
        GUI_SetColor(WHITE);
        GUI_DrawPrect(rect);
      }
      else
      {
        GUI_SetColor(BACKGROUND_COLOR);
        GUI_DrawPrect(rect);

        GUI_SetColor(LISTBTN_BKCOLOR);
        GUI_DrawLine(rect->x0, rect->y0-1 , rect->x1-1, rect->y0-1 );
        GUI_DrawLine(rect->x0, rect->y1-1 , rect->x1-1, rect->y1-1 );
      }
      GUI_RestoreColorDefault();
}

void ListItem_Display(const GUI_RECT* rect, uint8_t position, const LISTITEM * curitem, bool pressed)
{
  //draw navigation icons
  if(position >= LISTITEM_PER_PAGE){
    if(curitem->icon != ICONCHAR_BACKGROUND){
      DrawCharIcon(rect,MIDDLE,curitem->icon,LISTBTN_BKCOLOR);
      if (pressed != false){
        GUI_DrawPrect(rect);
      }
    }
    else{
      GUI_ClearPrect(rect);
    }
  }
  //draw list tiems
  else if (curitem->icon != ICONCHAR_BACKGROUND){

    GUI_POINT pos = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,LEFT_CENTER,GET_ICONCHAR[curitem->icon]);
    int textarea_width;
    switch (curitem->itemType)
    {
    case LIST_LABEL:

      if(curitem->icon != ICONCHAR_BLANK) {
        DrawCharIcon(rect,LEFT_CENTER,curitem->icon, BACKGROUND_COLOR);
        pos.x += (BYTE_HEIGHT + 1);
       }
      textarea_width = LISTITEM_WIDTH - (pos.x + 1); //width after removing the width for icon
      draw_itemtitle(pos,curitem->titlelabel,position,textarea_width);

      DrawListItemPress(rect, pressed);
      break;

    case LIST_TOGGLE:;
      int16_t wy = (1 + GUI_StrPixelWidth(IconCharSelect(ICONCHAR_TOGGLE_BODY)) + 1);   //right edge of text area
      GUI_ClearRect(rect->x0, rect->y0, rect->x1 - wy, rect->y1);                     // clear only text area
      textarea_width = LISTITEM_WIDTH - (pos.x + wy);                                 //width after removing the width for icon
      draw_itemtitle(pos,curitem->titlelabel,position,textarea_width);

      pos = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,RIGHT_CENTER,GET_ICONCHAR[ICONCHAR_TOGGLE_BODY]);
      GUI_ClearRect(rect->x1-wy,rect->y0,rect->x1,rect->y1);
      ListItem_DisplayToggle(pos.x, pos.y, curitem->icon);
      DrawListItemPress(rect,pressed);
      break;

    case LIST_MOREBUTTON:

      GUI_ClearPrect(rect);

      if(curitem->icon != ICONCHAR_BLANK) {
        DrawCharIcon(rect,LEFT_CENTER,curitem->icon,BLACK);
        pos.x += (BYTE_HEIGHT + 1);
       }
      textarea_width = LISTITEM_WIDTH - (pos.x + BYTE_HEIGHT + 2);  //width after removing the width for icon
      draw_itemtitle(pos,curitem->titlelabel,position,textarea_width);

      pos = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,RIGHT_CENTER,GET_ICONCHAR[ICONCHAR_DETAIL2]);
      GUI_SetColor(ICON_COLOR[ICONCHAR_DETAIL2]);
      GUI_DispString(pos.x, pos.y,IconCharSelect(ICONCHAR_DETAIL2));


      DrawListItemPress(rect,pressed);
      break;

    case LIST_CUSTOMVALUE:
      if(curitem->icon != ICONCHAR_BLANK) {
        DrawCharIcon(rect,LEFT_CENTER,curitem->icon,BLACK);
        pos.x += (BYTE_HEIGHT + 3);
      }
      GUI_ClearRect(pos.x, rect->y0, rect->x1 - BYTE_WIDTH*8 -1, rect->y1);        // clear only text area
      textarea_width = LISTITEM_WIDTH - (pos.x + 1); //width after removing the width for icon
      draw_itemtitle(pos,curitem->titlelabel,position,textarea_width);

      ListItem_DisplayCustomValue(rect,curitem->valueLabel,position);

      DrawListItemPress(rect,pressed);
      break;

    default:
      break;
    }

  }
   else
  {
    GUI_ClearPrect(rect);
  }

  GUI_RestoreColorDefault();
}

//draw title text of list item
void draw_itemtitle(GUI_POINT pos,LABEL label, uint8_t position, int textarea_width){
  if (label.index != LABEL_BACKGROUND)
  {
    int textarea_width = LISTITEM_WIDTH - (pos.x + 1); //width after removing the width for icon
    if (label.index == LABEL_DYNAMIC)
    {GUI_DispLenString(pos.x, pos.y, (u8*)getDynamicLabel(position),textarea_width);
    }
    else
    {GUI_DispLenString(pos.x, pos.y, textSelect(label.index),textarea_width);
    }
  }
}

//display toggle button
void ListItem_DisplayToggle(uint16_t sx, uint16_t sy, uint8_t iconchar_state)
{
  //const GUI_RECT *rect = rect_of_keyListView + position;
  //GUI_POINT pos = getTextStartPoint(rect_of_keyListView.x0, rect->y0, rect->x1,rect->y1, pos, iconchar);

  //GUI_ClearPrect(&rect_item);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(LISTBTN_BKCOLOR);
  GUI_DispString(sx, sy, (uint8_t*)GET_ICONCHAR[ICONCHAR_TOGGLE_BODY]);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  GUI_SetColor(ICON_COLOR[iconchar_state]);
  if (iconchar_state == ICONCHAR_TOGGLE_OFF)
  {
    GUI_DispString(sx, sy, IconCharSelect(ICONCHAR_TOGGLE_SWITCH));
  }
  else
  {
    GUI_DispString(sx+BYTE_HEIGHT, sy, IconCharSelect(ICONCHAR_TOGGLE_SWITCH));
  }

  GUI_RestoreColorDefault();

}

//draw custom value for list item
void ListItem_DisplayCustomValue(const GUI_RECT* rect,LABEL value,int i)
{

  const GUI_RECT rectVal = {rect->x1-BYTE_WIDTH*8 -1,rect->y0+(LISTITEM_HEIGHT-BYTE_HEIGHT)/2,rect->x1-1,rect->y1-(LISTITEM_HEIGHT-BYTE_HEIGHT)/2};
//GUI_POINT pos = getTextStartPoint(rect_of_keyListView.x0, rect->y0, rect->x1,rect->y1, pos, iconchar);

  GUI_ClearPrect(&rectVal);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

  GUI_SetColor(LISTBTN_BKCOLOR);

  GUI_DrawPrect(&rectVal);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_SetColor(MAT_LOWWHITE);

  if(value.index == LABEL_DYNAMIC){
    GUI_DispFloat(rectVal.x0,rectVal.y0,dynamic_value[i],4,1,RIGHT);
  }
  else{
    GUI_DispStringInPrect(&rectVal,textSelect(value.index));
  }

  GUI_RestoreColorDefault();
}
