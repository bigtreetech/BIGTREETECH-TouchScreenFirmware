#include "list_item.h"
#include "includes.h"


const uint16_t ICON_COLOR[ICONCHAR_NUM]=
{
  MATT_BLUE,   //ICONCHAR_INFO = 0,
  RED ,   //ICONCHAR_ERROR,
  YELLOW, //ICONCHAR_ALERT,
  WHITE,  //ICONCHAR_CHECKED,
  WHITE,  //ICONCHAR_UNCHECKED,
  WHITE,  //ICONCHAR_ON,
  WHITE,  //ICONCHAR_OFF,
  WHITE,  //ICON_RADIO_ON,
  WHITE,  //ICON_RADIO_OFF,
  WHITE,  //ICONCHAR_OK,
  WHITE,  //ICONCHAR_CANCEL,
  WHITE,  //ICONCHAR_PAGEUP,
  WHITE,  //ICONCHAR_PAGEDOWN,
  WHITE,  //ICONCHAR_NEXT,
  WHITE,  //ICONCHAR_PREVIOUS,
  WHITE,  //ICONCHAR_BACK,
  MATT_YELLOW, //ICONCHAR_FOLDER,
  MATT_BLUE,  //ICONCHAR_FILE,
  WHITE,  //ICONCHAR_NOZZLE,
  WHITE,  //ICONCHAR_BED,
  WHITE,  //ICONCHAR_FAN,

};

#define   CHAR_INFO       "ࢄ"
#define   CHAR_ERROR	    "ࢂ"
#define   CHAR_ALERT      ""
#define   CHAR_CHECKED    ""
#define   CHAR_UNCHECKED  ""
#define   CHAR_ON         ""
#define   CHAR_OFF        ""
#define   CHAR_RADIO_CHECKED   ""
#define   CHAR_RADIO_UNCHECKED  ""
#define   CHAR_OK         ""
#define   CHAR_CANCEL     ""
#define   CHAR_PAGEUP     "ࡧࡨ"
#define   CHAR_PAGEDOWN   "ࡩࡪ"
#define   CHAR_RIGHT       ""
#define   CHAR_LEFT         ""
#define   CHAR_BACK       "࡫࡬"
#define   CHAR_FOLDER     "ࡢ"
#define   CHAR_FILE       "ࡣ"
#define   CHAR_NOZZLE     ""
#define   CHAR_BED        ""
#define   CHAR_FAN        ""



const char *const GET_ICONCHAR[ICONCHAR_NUM]={
  CHAR_INFO,
  CHAR_ERROR,
  CHAR_ALERT,
  CHAR_CHECKED,
  CHAR_UNCHECKED,
  CHAR_ON,
  CHAR_OFF,
  CHAR_RADIO_CHECKED,
  CHAR_RADIO_UNCHECKED,
  CHAR_OK,
  CHAR_CANCEL,
  CHAR_PAGEUP,
  CHAR_PAGEDOWN,
  CHAR_RIGHT,
  CHAR_LEFT,
  CHAR_BACK,
  CHAR_FOLDER,
  CHAR_FILE,
  CHAR_NOZZLE,
  CHAR_BED,
  CHAR_FAN,
};
uint8_t * IconCharSelect(uint8_t sel)
{ 
return (uint8_t *)GET_ICONCHAR[sel];
}
char * IconChar(uint8_t sel)
{ 
return (char *)GET_ICONCHAR[sel];
}

void ListItem_Display(const GUI_RECT* rect, uint8_t iconchar, ICON_POS pos, uint8_t * label, bool pressed)
{
  uint16_t sx = rect->x0,
           sy = rect->y0,
           ex = rect->x1,
           ey = rect->y1;
  uint16_t w = ex- sx;
  uint16_t h = ey- sy;
  uint16_t icon_x= sx + 1;
  uint16_t icon_y= sy + 1;
  //GUI_SetBkColor(RED);
  //GUI_SetColor(WHITE);
  //uint16_t charIcon_w;
  //uint8_t * p = IconCharSelect(iconchar);
  size_t charIcon_w = strlen(IconChar(iconchar))/3 * BYTE_HEIGHT;
  //storeCmd("%d\n",charIcon_w);
  GUI_ClearPrect(rect);
  
  switch (pos)
  {
  case TOP_LEFT:
    icon_x = sx + 1;
    icon_y = sy + 1;
    break;

  case TOP_CENTER:
    icon_x = (sx + (w - charIcon_w) / 2);
    icon_y = sy + 1;
    break;

  case TOP_RIGHT:
    icon_x = sx + w - charIcon_w - 1;
    icon_y = sy + 1;
    break;

  case LEFT_CENTER:
    icon_x = sx + 1;
    icon_y = (sy + (h - BYTE_HEIGHT) / 2);
    break;

  case MIDDLE:
    icon_x = (sx +(w - charIcon_w) / 2);
    icon_y = (sy +(h - BYTE_HEIGHT) / 2);
    break;

  case RIGHT_CENTER:
    icon_x = sx - charIcon_w - 1;
    icon_y = (sy +(h - BYTE_HEIGHT) / 2);
    break;

  case BOTTOM_LEFT:
    icon_x = sx + 1;
    icon_y = sy + h - BYTE_HEIGHT - 1;
    break;

  case BOTTOM_CENTER:
    icon_x = (sx +(w - charIcon_w) / 2);
    icon_y = sy + h - BYTE_HEIGHT - 1;
    break;

  case BOTTOM_RIGHT:
    icon_x = sx + w - charIcon_w - 1;
    icon_y = sy + h - BYTE_HEIGHT - 1;
    break;

  default:
    break;
  }
  
  if (iconchar != ICONCHAR_BACKGROUND){
    GUI_SetColor(ICON_COLOR[iconchar]);
    GUI_DispString(icon_x, icon_y,IconCharSelect(iconchar));
    GUI_DispString(icon_x + BYTE_HEIGHT + 3, icon_y , label);
  }
  else
  {
    GUI_ClearRect(icon_x ,icon_y, icon_x + BYTE_HEIGHT, icon_y + BYTE_HEIGHT);
    GUI_DispString(icon_x + BYTE_HEIGHT + 3, icon_y, label);
  }
  if (pressed){
    GUI_SetColor(WHITE);
    GUI_DrawPrect(rect);

  }
      GUI_SetColor(FK_COLOR);
}
