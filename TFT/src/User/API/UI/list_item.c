// *List View Mode ** Author: Gurmeet Athwal https://github.com/guruathwal **

#include "list_item.h"
#include "includes.h"
#include "menu.h"
#include "GUI.h"

char * dynamic_label[LISTITEM_PER_PAGE];
char dynamic_text_value[LISTITEM_PER_PAGE][10];

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
  WHITE,    //ICONCHAR_LEFT_TOP
  WHITE,    //ICONCHAR_LEFT_BOTTOM
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
  MAT_GREEN,    //ICONCHAR_OK
  MAT_RED,    //ICONCHAR_CANCEL
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
  MAT_BLUE,    //ICONCHAR_RESET
  WHITE,    //ICONCHAR_EDIT
  WHITE,    //ICONCHAR_SAVE
  MAT_ORANGE,    //ICONCHAR_DELETE
  MAT_LOWWHITE,    //ICONCHAR_RADIO_CHECKED
  MAT_LOWWHITE,    //ICONCHAR_RADIO_UNCHECKED
  MAT_GREEN,    //ICONCHAR_CHECKED
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
  MATT_DARKGRAY,  //ICONCHAR_TOGGLE_BODY
  WHITE,          //ICONCHAR_TOGGLE_SWITCH
  MAT_GREEN,      //ICONCHAR_TOGGLE_ON
  MAT_RED,        //ICONCHAR_TOGGLE_OFF


};

#define CHAR_BLANK                 ""
#define CHAR_NOZZLE                "\u085F"
#define CHAR_BED                   "\u0860"
#define CHAR_FAN                   "\u0861"
#define CHAR_FOLDER                "\u0862"
#define CHAR_FILE                  "\u0863"

#define CHAR_PAGEUP                "\u0867\u0868"
#define CHAR_PAGEDOWN              "\u0869\u086A"
#define CHAR_BACK                  "\u086B\u086C"
#define CHAR_LEFT_PART1            "\u086D"
#define CHAR_LEFT_PART2            "\u086E"
#define CHAR_RIGHT_PART1           "\u086F"
#define CHAR_RIGHT_PART2           "\u0870"

#define CHAR_ALERT                 "\u0880"
#define CHAR_WARNING               "\u0881"
#define CHAR_ERROR                 "\u0882"
#define CHAR_CAUTION               "\u0883"
#define CHAR_INFO                  "\u0884"
#define CHAR_HAND                  "\u0885"
#define CHAR_WAIT                  "\u0886"
#define CHAR_QUESTION              "\u0887"
#define CHAR_PLAY                  "\u0888"
#define CHAR_PAUSE                 "\u0889"
#define CHAR_STOP                  "\u088A"
#define CHAR_EJECT                 "\u088B"
#define CHAR_PLAY_ROUND            "\u088C"
#define CHAR_PAUSE_ROUND           "\u088D"
#define CHAR_OK_ROUND              "\u088E"
#define CHAR_CANCEL_ROUND          "\u088F"
#define CHAR_MINUS_ROUND           "\u0890"
#define CHAR_PLUS_ROUND            "\u0891"
#define CHAR_MINUS                 "\u0892"
#define CHAR_PLUS                  "\u0893"
#define CHAR_OK                    "\u0894"
#define CHAR_CANCEL                "\u0895"
#define CHAR_HALT                  "\u0896"
#define CHAR_UP_DOWN               "\u0897"
#define CHAR_LEFT_RIGHT            "\u0898"
#define CHAR_POINT_LEFT            "\u0899"
#define CHAR_POINT_RIGHT           "\u089A"
#define CHAR_RETURN                "\u089B"
#define CHAR_REDO                  "\u089C"
#define CHAR_UNDO                  "\u089D"
#define CHAR_DOWNLOAD              "\u089E"
#define CHAR_UPLOAD                "\u089F"
#define CHAR_BULLET                "\u08A0"
#define CHAR_BACK_SMALL            "\u08A1"
#define CHAR_EXPAND                "\u08A2"
#define CHAR_MOVE                  "\u08A3"
#define CHAR_ROTATE                "\u08A4"
#define CHAR_RESET                 "\u08A5"
#define CHAR_EDIT                  "\u08A6"
#define CHAR_SAVE                  "\u08A7"
#define CHAR_DELETE                "\u08A8"
#define CHAR_RADIO_CHECKED         "\u08A9"
#define CHAR_RADIO_UNCHECKED       "\u08AA"
#define CHAR_CHECKED               "\u08AB"
#define CHAR_UNCHECKED             "\u08AC"
#define CHAR_SOUND_OFF             "\u08AD"
#define CHAR_SOUND_ON              "\u08AE"
#define CHAR_ALERT_OFF             "\u08AF"
#define CHAR_ALERT_ON              "\u08B0"
#define CHAR_POWER_OFF             "\u08B1"
#define CHAR_POWER_ON              "\u08B2"
#define CHAR_WIFI_OFF              "\u08B3"
#define CHAR_WIFI_ON               "\u08B4"
#define CHAR_KEYBOARD              "\u08B5"
#define CHAR_SD_ERROR              "\u08B6"
#define CHAR_SD_OK                 "\u08B7"
#define CHAR_SD_REFRESH            "\u08B8"
#define CHAR_USB_ERROR             "\u08B9"
#define CHAR_USB_OK                "\u08BA"
#define CHAR_PRINTER_ERROR         "\u08BB"
#define CHAR_PRINTER_OK            "\u08BC"
#define CHAR_BOARD                 "\u08BD"
#define CHAR_EEPROM                "\u08BE"
#define CHAR_PRINT                 "\u08BF"
#define CHAR_SETTING1              "\u08C0"
#define CHAR_PLUGIN                "\u08C1"
#define CHAR_FEATURE               "\u08C2"
#define CHAR_SETTING2              "\u08C3"
#define CHAR_DETAIL                "\u08C4"
#define CHAR_DETAIL2               "\u08C5"
#define CHAR_ADJUST                "\u08C6"
#define CHAR_MENU                  "\u08C7"
#define CHAR_POWER                 "\u08C8"
#define CHAR_TOUCH                 "\u08C9"
#define CHAR_LANGUAGE              "\u08CA"
#define CHAR_CODE                  "\u08CB"
#define CHAR_POWER_PLUG            "\u08CC"
#define CHAR_ROTATE_DEVICE         "\u08CD"
#define CHAR_WINDOW                "\u08CE"
#define CHAR_BACKGROUND_COLOR      "\u08CF"
#define CHAR_FONT_COLOR            "\u08D0"
#define CHAR_PAINT                 "\u08D1"
#define CHAR_TOGGLE_SMALL_ON       "\u08D2"
#define CHAR_TOGGLE_SMALL_OFF      "\u08D3"
#define CHAR_TOGGLE_BODY           "\u08D4\u08D5"
#define CHAR_TOGGLE_SWITCH         "\u08D6"
#define CHAR_TOGGLE_ON             ""       //only for toggle state detection
#define CHAR_TOGGLE_OFF            ""       //only for toggle state detection

const char *const GET_ICONCHAR[ICONCHAR_NUM]={
  #define X_CHAR(NAME) CHAR_##NAME ,
  #include "Char_Icon.inc"
  #undef  X_CHAR
//add new icons in icon_list.inc only

};


uint8_t * IconCharSelect(uint8_t sel)
{
  return (uint8_t *)GET_ICONCHAR[sel];
}
char * IconChar(uint8_t sel)
{
  return (char *)GET_ICONCHAR[sel];
}

void setDynamicLabel(uint8_t i, char *label){
  dynamic_label[i] = label;
}

// get dynamic text label ( i : index of the label position)
char * getDynamicLabel(uint8_t i){
  return dynamic_label[i];
}

// save dynamic text value (upto 7 characters) ( i : index of the text value position, txt: char * to the text value)
void setDynamicTextValue(uint8_t i, char *txt){
  //dynamic_text_value[i] = txt;
  strcpy(dynamic_text_value[i],txt);
}

// get dynamic text value ( i : index of the text value position)
char * getDynamicTextValue(uint8_t i){
  return dynamic_text_value[i];
}

// save dynamic value (upto 7 digits) ( i : index of the value position, value:float value)
void setDynamicValue(uint8_t i,float value){
  if(value < 1.0f)
    sprintf(dynamic_text_value[i], "%.3f", value);
  else if (value < 1000.0f)
    sprintf(dynamic_text_value[i], "%.2f", value);
  else
    sprintf(dynamic_text_value[i], "%.1f", value);
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
void DrawCharIcon(const GUI_RECT * rect,ICON_POS iconalign, uint16_t iconindex,bool drawBgColor, uint16_t btn_color)
{
      GUI_POINT icon_p = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,iconalign,GET_ICONCHAR[iconindex]);
      GUI_SetColor(ICON_COLOR[iconindex]);
      if(drawBgColor)
      {
        GUI_SetBkColor(btn_color);
        GUI_ClearPrect(rect);
      }
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
        GUI_SetColor(infoSettings.bg_color);
        GUI_DrawPrect(rect);
        GUI_SetColor(infoSettings.list_border_color);
        GUI_DrawLine(rect->x0, rect->y0-1 , rect->x1-1, rect->y0-1 );
        GUI_DrawLine(rect->x0, rect->y1-1 , rect->x1-1, rect->y1-1 );
      }
      GUI_RestoreColorDefault();
}

void ListItem_Display(const GUI_RECT* rect, uint8_t position, const LISTITEM * curitem, bool pressed)
{
  //draw navigation icons
  if (getMenuType() != MENU_TYPE_LISTVIEW) return;

  if(position >= LISTITEM_PER_PAGE){
    if(curitem->icon != ICONCHAR_BACKGROUND)
    {
      if (curitem->icon != ICONCHAR_BLANK)
      {
        DrawCharIcon(rect, MIDDLE, curitem->icon, true, infoSettings.list_button_color);
      }
      else if (curitem->icon == ICONCHAR_BLANK && curitem->titlelabel.index != LABEL_BACKGROUND)
      {
        GUI_SetBkColor(infoSettings.list_button_color);
        GUI_ClearPrect(rect);
        GUI_DispStringInPrect(rect, curitem->titlelabel.index);
      }
      if (pressed != false)
      {
        GUI_SetColor(WHITE);
        GUI_DrawPrect(rect);
      }
    }
    else
    {
      GUI_ClearPrect(rect);
    }
      GUI_RestoreColorDefault();
  }
  //draw list tiems
  else if (curitem->icon != ICONCHAR_BACKGROUND){

    GUI_POINT pos = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,LEFT_CENTER,GET_ICONCHAR[curitem->icon]);
    int textarea_width;
    switch (curitem->itemType)
    {
    case LIST_LABEL:
      if(curitem->icon != ICONCHAR_BLANK) {
        DrawCharIcon(rect,LEFT_CENTER,curitem->icon, true, infoSettings.bg_color);
        pos.x += (BYTE_HEIGHT + 1);
       }
      textarea_width = LISTITEM_WIDTH - (pos.x + 1); //width after removing the width for icon
      draw_itemtitle(pos,curitem->titlelabel,position,textarea_width);

      DrawListItemPress(rect, pressed);
      break;

    case LIST_TOGGLE: {
        int16_t wy = (1 + GUI_StrPixelWidth(IconCharSelect(ICONCHAR_TOGGLE_BODY)) + 1);   //right edge of text area
        GUI_ClearRect(rect->x0, rect->y0, rect->x1 - wy, rect->y1);                     // clear only text area
        textarea_width = LISTITEM_WIDTH - (pos.x + wy);                                 //width after removing the width for icon
        draw_itemtitle(pos,curitem->titlelabel,position,textarea_width);

        pos = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,RIGHT_CENTER,GET_ICONCHAR[ICONCHAR_TOGGLE_BODY]);
        GUI_ClearRect(rect->x1-wy,rect->y0,rect->x1,rect->y1);
        ListItem_DisplayToggle(pos.x, pos.y, curitem->icon);
        DrawListItemPress(rect,pressed);
      }
      break;

    case LIST_MOREBUTTON:
      GUI_ClearPrect(rect);

      if(curitem->icon != ICONCHAR_BLANK) {
        DrawCharIcon(rect,LEFT_CENTER,curitem->icon, true, infoSettings.bg_color);
        pos.x += (BYTE_HEIGHT + 1);
       }
      textarea_width = LISTITEM_WIDTH - (pos.x + BYTE_HEIGHT + 2); //width after removing the width for icon
      draw_itemtitle(pos,curitem->titlelabel,position,textarea_width);

      pos = getTextStartPoint(rect->x0, rect->y0, rect->x1,rect->y1,RIGHT_CENTER,GET_ICONCHAR[ICONCHAR_DETAIL2]);
      GUI_SetColor(ICON_COLOR[ICONCHAR_DETAIL2]);
      GUI_DispString(pos.x, pos.y,IconCharSelect(ICONCHAR_DETAIL2));


      DrawListItemPress(rect,pressed);
      break;

    case LIST_CUSTOMVALUE:
      if(curitem->icon != ICONCHAR_BLANK) {
        DrawCharIcon(rect,LEFT_CENTER,curitem->icon, true, infoSettings.bg_color);
        pos.x += (BYTE_HEIGHT + 3);
      }
      GUI_ClearRect(pos.x, rect->y0, rect->x1 - BYTE_WIDTH*8 -1, rect->y1); // clear only text area
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
      GUI_DispLenString(pos.x, pos.y, (u8*)getDynamicLabel(position),textarea_width, true);
    else
      GUI_DispLenString(pos.x, pos.y, labelGetAddress(&label), textarea_width, true);
  }
}

//display toggle button
void ListItem_DisplayToggle(uint16_t sx, uint16_t sy, uint8_t iconchar_state)
{
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(infoSettings.list_border_color);
  _GUI_DispString(sx, sy, (uint8_t*)GET_ICONCHAR[ICONCHAR_TOGGLE_BODY]);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  GUI_SetColor(ICON_COLOR[iconchar_state]);
  if (iconchar_state == ICONCHAR_TOGGLE_OFF)
    _GUI_DispString(sx, sy, IconCharSelect(ICONCHAR_TOGGLE_SWITCH));
  else
    _GUI_DispString(sx+BYTE_HEIGHT, sy, IconCharSelect(ICONCHAR_TOGGLE_SWITCH));

  GUI_RestoreColorDefault();

}

//draw custom value for list item
void ListItem_DisplayCustomValue(const GUI_RECT* rect,LABEL value,int i)
{

  const GUI_RECT rectVal = {rect->x1-BYTE_WIDTH*8 -1,rect->y0+(LISTITEM_HEIGHT-BYTE_HEIGHT)/2,rect->x1-1,rect->y1-(LISTITEM_HEIGHT-BYTE_HEIGHT)/2};

  GUI_ClearPrect(&rectVal);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

  GUI_SetColor(infoSettings.list_border_color);
  GUI_DrawPrect(&rectVal);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_SetColor(MAT_LOWWHITE);

  if (value.index == LABEL_CUSTOM_VALUE || value.index == LABEL_DYNAMIC) //show custom text value
    GUI_DispStringInPrect(&rectVal, (u8 *)getDynamicTextValue(i));
  else //show regular text labels
    GUI_DispStringInPrect(&rectVal, (int32_t)value.index);

  GUI_RestoreColorDefault();
}
