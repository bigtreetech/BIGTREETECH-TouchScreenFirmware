// *List View Mode ** Author: Gurmeet Athwal https://github.com/guruathwal **

#include "ListItem.h"
#include "includes.h"
#include "menu.h"
#include "GUI.h"

char * dynamic_label[LISTITEM_PER_PAGE];
char dynamic_text_value[LISTITEM_PER_PAGE][10];

void setDynamicLabel(uint8_t i, char * label)
{
  dynamic_label[i] = label;
}

// get dynamic text label ( i : index of the label position)
char * getDynamicLabel(uint8_t i)
{
  return dynamic_label[i];
}

// save dynamic text value (upto 7 characters) ( i : index of the text value position, txt: char * to the text value)
void setDynamicTextValue(uint8_t i, char * txt)
{
  //dynamic_text_value[i] = txt;
  strcpy(dynamic_text_value[i], txt);
}

// get dynamic text value ( i : index of the text value position)
char * getDynamicTextValue(uint8_t i)
{
  return dynamic_text_value[i];
}

// save dynamic value (upto 7 digits) ( i : index of the value position, value:float value)
void setDynamicValue(uint8_t i, float value)
{
  if (value < 1.0f)
    sprintf(dynamic_text_value[i], "%.3f", value);
  else if (value < 1000.0f)
    sprintf(dynamic_text_value[i], "%.2f", value);
  else
    sprintf(dynamic_text_value[i], "%.1f", value);
}

// get the text starting point on screen based on rectangle edges and desired icon position
GUI_POINT getTextStartPoint(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey,ALIGN_POSITION pos, const char * textchar)
{
  GUI_POINT point_item = {sx, sy};
  uint16_t w = ex - sx;
  uint16_t h = ey - sy;
  size_t charIcon_w = strlen(textchar) / 3 * BYTE_HEIGHT;

  switch (pos)
  {
    case TOP_LEFT:
      point_item.x = sx + 1;
      point_item.y = sy + 1;
      break;

    case TOP:
      point_item.x = (sx + (w - charIcon_w) / 2);
      point_item.y = sy + 1;
      break;

    case TOP_RIGHT:
      point_item.x = ex - charIcon_w - 1;
      point_item.y = sy + 1;
      break;

    case LEFT:
      point_item.x = sx + 1;
      point_item.y = (sy + (h - BYTE_HEIGHT) / 2);
      break;

    case CENTER:
      point_item.x = (sx + (w - charIcon_w) / 2);
      point_item.y = (sy + (h - BYTE_HEIGHT) / 2);
      break;

    case RIGHT:
      point_item.x = ex - charIcon_w - 1;
      point_item.y = (sy + (h - BYTE_HEIGHT) / 2);
      break;

    case BOTTOM_LEFT:
      point_item.x = sx + 1;
      point_item.y = sy + h - BYTE_HEIGHT - 1;
      break;

    case BOTTOM:
      point_item.x = (sx + (w - charIcon_w) / 2);
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
void DrawCharIcon(const GUI_RECT * rect,ALIGN_POSITION iconalign, uint16_t iconindex, bool drawBgColor, uint16_t btn_color)
{
  GUI_POINT icon_p = getTextStartPoint(rect->x0, rect->y0, rect->x1, rect->y1, iconalign, (char *)IconCharSelect(iconindex));
  GUI_SetColor(charIconColor[iconindex]);
  if (drawBgColor)
  {
    GUI_SetBkColor(btn_color);
    GUI_ClearPrect(rect);
  }
  GUI_DispString(icon_p.x, icon_p.y, IconCharSelect(iconindex));
  GUI_RestoreColorDefault();
}

//draw item pressed feedback
void DrawListItemPress(const GUI_RECT * rect, bool pressed)
{
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
    GUI_DrawLine(rect->x0, rect->y0 - 1, rect->x1 - 1, rect->y0 - 1);
    GUI_DrawLine(rect->x0, rect->y1 - 1, rect->x1 - 1, rect->y1 - 1);
  }
  GUI_RestoreColorDefault();
}

void ListItem_Display(const GUI_RECT * rect, uint8_t position, const LISTITEM * curitem, bool pressed)
{
  //draw navigation icons
  if (getMenuType() != MENU_TYPE_LISTVIEW)
    return;

  if (position >= LISTITEM_PER_PAGE)
  {
    if (curitem->icon != CHARICON_BACKGROUND)
    {
      if (curitem->icon != CHARICON_BLANK)
      {
        DrawCharIcon(rect, CENTER, curitem->icon, true, infoSettings.list_button_color);
      }
      else if (curitem->icon == CHARICON_BLANK && curitem->titlelabel.index != LABEL_BACKGROUND)
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
  else if (curitem->icon != CHARICON_BACKGROUND)
  {
    GUI_POINT pos = getTextStartPoint(rect->x0, rect->y0, rect->x1, rect->y1, LEFT, (char *)IconCharSelect(curitem->icon));
    int textarea_width;
    switch (curitem->itemType)
    {
      case LIST_LABEL:
        if (curitem->icon != CHARICON_BLANK)
        {
          DrawCharIcon(rect, LEFT, curitem->icon, true, infoSettings.bg_color);
          pos.x += (BYTE_HEIGHT + 1);
        }
        textarea_width = LISTITEM_WIDTH - (pos.x + 1);  //width after removing the width for icon
        draw_itemtitle(pos, curitem->titlelabel, position, textarea_width);
        DrawListItemPress(rect, pressed);
        break;

      case LIST_TOGGLE:
        {
          int16_t wy = (1 + GUI_StrPixelWidth(IconCharSelect(CHARICON_TOGGLE_BODY)) + 1);  //right edge of text area
          GUI_ClearRect(rect->x0, rect->y0, rect->x1 - wy, rect->y1);                      // clear only text area
          textarea_width = LISTITEM_WIDTH - (pos.x + wy);  //width after removing the width for icon
          draw_itemtitle(pos, curitem->titlelabel, position, textarea_width);

          pos = getTextStartPoint(rect->x0, rect->y0, rect->x1, rect->y1, RIGHT, (char *)IconCharSelect(CHARICON_TOGGLE_BODY));
          GUI_ClearRect(rect->x1 - wy, rect->y0, rect->x1, rect->y1);
          ListItem_DisplayToggle(pos.x, pos.y, curitem->icon);
          DrawListItemPress(rect, pressed);
        }
        break;

      case LIST_MOREBUTTON:
        GUI_ClearPrect(rect);

        if (curitem->icon != CHARICON_BLANK)
        {
          DrawCharIcon(rect, LEFT, curitem->icon, true, infoSettings.bg_color);
          pos.x += (BYTE_HEIGHT + 1);
        }
        textarea_width = LISTITEM_WIDTH - (pos.x + BYTE_HEIGHT + 2);  //width after removing the width for icon
        draw_itemtitle(pos, curitem->titlelabel, position, textarea_width);

        pos = getTextStartPoint(rect->x0, rect->y0, rect->x1, rect->y1, RIGHT, (char *)IconCharSelect(CHARICON_DETAIL2));
        GUI_SetColor(charIconColor[CHARICON_DETAIL2]);
        GUI_DispString(pos.x, pos.y, IconCharSelect(CHARICON_DETAIL2));
        DrawListItemPress(rect, pressed);
        break;

      case LIST_CUSTOMVALUE:
        if (curitem->icon != CHARICON_BLANK)
        {
          DrawCharIcon(rect, LEFT, curitem->icon, true, infoSettings.bg_color);
          pos.x += (BYTE_HEIGHT + 3);
        }
        GUI_ClearRect(pos.x, rect->y0, rect->x1 - BYTE_WIDTH * 8 - 1, rect->y1);  // clear only text area
        textarea_width = LISTITEM_WIDTH - (pos.x + 1);                            //width after removing the width for icon
        draw_itemtitle(pos, curitem->titlelabel, position, textarea_width);
        ListItem_DisplayCustomValue(rect, curitem->valueLabel, position);
        DrawListItemPress(rect, pressed);
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
void draw_itemtitle(GUI_POINT pos, LABEL label, uint8_t position, int textarea_width)
{
  if (label.index != LABEL_BACKGROUND)
  {
    int textarea_width = LISTITEM_WIDTH - (pos.x + 1);  //width after removing the width for icon
    if (label.index == LABEL_DYNAMIC)
      GUI_DispLenString(pos.x, pos.y, (uint8_t *)getDynamicLabel(position), textarea_width, true);
    else
      GUI_DispLenString(pos.x, pos.y, labelGetAddress(&label), textarea_width, true);
  }
}

//display toggle button
void ListItem_DisplayToggle(uint16_t sx, uint16_t sy, uint8_t iconchar_state)
{
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(infoSettings.list_border_color);
  _GUI_DispString(sx, sy, (uint8_t *)(char *)IconCharSelect(CHARICON_TOGGLE_BODY));
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  GUI_SetColor(charIconColor[iconchar_state]);
  if (iconchar_state == CHARICON_TOGGLE_OFF)
    _GUI_DispString(sx, sy, IconCharSelect(CHARICON_TOGGLE_SWITCH));
  else
    _GUI_DispString(sx + BYTE_HEIGHT, sy, IconCharSelect(CHARICON_TOGGLE_SWITCH));

  GUI_RestoreColorDefault();
}

//draw custom value for list item
void ListItem_DisplayCustomValue(const GUI_RECT * rect, LABEL value, int i)
{
  const GUI_RECT rectVal = {rect->x1 - BYTE_WIDTH * 8 - 1, rect->y0 + (LISTITEM_HEIGHT - BYTE_HEIGHT) / 2, rect->x1 - 1,
                            rect->y1 - (LISTITEM_HEIGHT - BYTE_HEIGHT) / 2};

  GUI_ClearPrect(&rectVal);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

  GUI_SetColor(infoSettings.list_border_color);
  GUI_DrawPrect(&rectVal);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_SetColor(MAT_LOWWHITE);

  if (value.index == LABEL_CUSTOM_VALUE || value.index == LABEL_DYNAMIC)  //show custom text value
    GUI_DispStringInPrect(&rectVal, (uint8_t *)getDynamicTextValue(i));
  else  //show regular text labels
    GUI_DispStringInPrect(&rectVal, (int32_t)value.index);

  GUI_RestoreColorDefault();
}
