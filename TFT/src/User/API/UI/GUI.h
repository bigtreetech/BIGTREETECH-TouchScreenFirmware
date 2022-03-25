#ifndef _GUI_H_
#define _GUI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "variants.h"
#include "utf8_decode.h"

typedef enum
{
  TOP_LEFT = 0,
  TOP,
  TOP_RIGHT,
  LEFT,
  CENTER,
  RIGHT,
  BOTTOM_LEFT,
  BOTTOM,
  BOTTOM_RIGHT
} ALIGN_POSITION;

typedef enum
{
  GUI_TEXTMODE_TRANS,
  GUI_TEXTMODE_NORMAL,
  GUI_TEXTMODE_ON_ICON,
} GUI_TEXT_MODE;

typedef enum
{
  GUI_NUMMODE_SPACE,
  GUI_NUMMODE_ZERO,
} GUI_NUM_MODE;

typedef struct
{
  int16_t x, y;
} GUI_POINT;

typedef struct
{
  int16_t x0, y0, x1, y1;
} GUI_RECT;

void GUI_SetColor(uint16_t color);
uint16_t GUI_GetColor(void);
void GUI_SetBkColor(uint16_t bkcolor);
uint16_t GUI_GetBkColor(void);
void GUI_SetTextMode(GUI_TEXT_MODE mode);
GUI_TEXT_MODE GUI_GetTextMode(void);
void GUI_SetNumMode(GUI_NUM_MODE mode);
GUI_NUM_MODE GUI_GetNumMode(void);

void GUI_Clear(uint16_t color);
void GUI_DrawPoint(uint16_t x, uint16_t y);
void GUI_FillRect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
void GUI_FillPrect(const GUI_RECT *rect);
void GUI_ClearRect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
void GUI_ClearPrect(const GUI_RECT *rect);
void GUI_FillRectColor(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void GUI_FillRectArry(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint8_t *arry);
void GUI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void GUI_HLine(uint16_t x1, uint16_t y, uint16_t x2);
void GUI_VLine(uint16_t x1, uint16_t y1, uint16_t y2);
void GUI_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void GUI_DrawPrect(const GUI_RECT *rect);
void GUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r);
void GUI_FillCircle(uint16_t x0, uint16_t y0, uint16_t r);

void GUI_SetRange(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void GUI_CancelRange(void);
void GUI_DrawPixel(int16_t x, int16_t y, uint16_t color);
GUI_POINT getTextStartPoint(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey,ALIGN_POSITION pos, const char * textchar);
void GUI_DispOne(int16_t sx, int16_t sy, const CHAR_INFO *pInfo);

// display string from char array pointers
void _GUI_DispString(int16_t x, int16_t y, const uint8_t *p);
const uint8_t* _GUI_DispLenString(int16_t x, int16_t y, const uint8_t *p, uint16_t pixelWidth, bool truncate);
void _GUI_DispStringRight(int16_t x, int16_t y, const uint8_t *p);
void _GUI_DispStringCenter(int16_t x, int16_t y, const uint8_t *p);
void _GUI_DispStringInRect(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint8_t *p);
void _GUI_DispStringInPrect(const GUI_RECT *rect, const uint8_t *p);
void _GUI_DispStringInRectEOL(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint8_t *p);
void _GUI_DispStringInPrectEOL(const GUI_RECT *rect, const uint8_t *p);
void _GUI_DispStringOnIcon(uint16_t iconIndex, GUI_POINT iconPoint, GUI_POINT textPos, const uint8_t *p);

// display string from label index
void _GUI_DispLabel(int16_t x, int16_t y, uint16_t index);
const uint8_t* _GUI_DispLenLabel(int16_t x, int16_t y, uint16_t index, uint16_t pixelWidth, bool truncate);
void _GUI_DispLabelRight(int16_t x, int16_t y, uint16_t index);
void _GUI_DispLabelCenter(int16_t x, int16_t y, uint16_t index);
void _GUI_DispLabelInRect(int16_t sx, int16_t sy, int16_t ex, int16_t ey, uint16_t index);
void _GUI_DispLabelInPrect(const GUI_RECT *rect, uint16_t index);
void _GUI_DispLabelInRectEOL(int16_t sx, int16_t sy, int16_t ex, int16_t ey, uint16_t index);
void _GUI_DispLabelInPrectEOL(const GUI_RECT *rect, uint16_t index);
void _GUI_DispLabelOnIcon(uint16_t iconIndex, GUI_POINT iconPoint, GUI_POINT textPos, uint16_t index);

// macros for selecting right function based on variable type
#define GUI_DispString(x, y, c)                       _Generic(((c+0)), const uint8_t*: _GUI_DispString, \
                                                                              uint8_t*: _GUI_DispString, \
                                                                               default: _GUI_DispLabel)(x,y,c)
#define GUI_DispLenString(x, y, c, width, truncate)   _Generic(((c+0)), const uint8_t*: _GUI_DispLenString, \
                                                                              uint8_t*: _GUI_DispLenString, \
                                                                               default: _GUI_DispLenLabel)(x,y,c,width,truncate)
#define GUI_DispStringRight(x, y, c)                  _Generic(((c+0)), const uint8_t*: _GUI_DispStringRight, \
                                                                              uint8_t*: _GUI_DispStringRight, \
                                                                               default: _GUI_DispLabelRight)(x,y,c)
#define GUI_DispStringCenter(x, y, c)                 _Generic(((c+0)), const uint8_t*: _GUI_DispStringCenter, \
                                                                              uint8_t*: _GUI_DispStringCenter, \
                                                                               default: _GUI_DispLabelCenter)(x,y,c)
#define GUI_DispStringInRect(sx, sy, ex, ey, c)       _Generic(((c+0)), const uint8_t*: _GUI_DispStringInRect, \
                                                                              uint8_t*: _GUI_DispStringInRect, \
                                                                               default: _GUI_DispLabelInRect)(sx,sy,ex,ey,c)
#define GUI_DispStringInPrect(rect, c)                _Generic(((c+0)), const uint8_t*: _GUI_DispStringInPrect, \
                                                                              uint8_t*: _GUI_DispStringInPrect, \
                                                                               default: _GUI_DispLabelInPrect)(rect,c)
#define GUI_DispStringInRectEOL(sx, sy, ex, ey, c)    _Generic(((c+0)), const uint8_t*: _GUI_DispStringInRectEOL, \
                                                                              uint8_t*: _GUI_DispStringInRectEOL, \
                                                                               default: _GUI_DispLabelInRectEOL)(sx,sy,ex,ey,c)
#define GUI_DispStringInPrectEOL(rect, c)             _Generic(((c+0)), const uint8_t*: _GUI_DispStringInPrectEOL, \
                                                                              uint8_t*: _GUI_DispStringInPrectEOL, \
                                                                               default: _GUI_DispLabelInPrectEOL)(rect,c)
#define GUI_DispStringOnIcon(icon, iconPt, txtPos, c) _Generic(((c+0)), const uint8_t*: _GUI_DispStringOnIcon, \
                                                                              uint8_t*: _GUI_DispStringOnIcon, \
                                                                               default: _GUI_DispLabelOnIcon)(icon,iconPt,txtPos,c)

void GUI_DispDec(int16_t x, int16_t y,int32_t num, uint8_t len, uint8_t leftOrRight);
void GUI_DispFloat(int16_t x, int16_t y, float num, uint8_t llen, uint8_t rlen, uint8_t leftOrRight);

/****************************************************     Widget    *******************************************************************/

#define RADIO_SIZE 5
typedef struct
{
  uint8_t  *context[RADIO_SIZE];
  uint16_t sx;
  uint16_t sy;
  uint8_t  distance;
  uint8_t  num;
  uint8_t  select;
} RADIO;

void RADIO_Create(RADIO *radio);
void RADIO_Select(RADIO *radio, uint8_t select);

typedef struct
{
  GUI_RECT rect;
  uint8_t  *text;
  uint32_t time;
  int16_t  off_head;
  int16_t  off_tail;
  uint16_t maxByte;
  uint16_t curByte;
  uint16_t totalPixelWidth;
  int16_t  curPixelWidth;
  uint16_t maxPixelWidth;
  uint8_t  has_disp;
} SCROLL;

void Scroll_CreatePara(SCROLL * para,const uint8_t *pstr, const GUI_RECT *prect);
void Scroll_DispString(SCROLL * para, uint8_t align);

typedef enum
{
  DIALOG_TYPE_INFO,
  DIALOG_TYPE_ALERT,
  DIALOG_TYPE_QUESTION,
  DIALOG_TYPE_ERROR,
  DIALOG_TYPE_SUCCESS,
}DIALOG_TYPE;

typedef struct
{
  const GUI_RECT rect;
  const uint8_t  *context;
  const uint16_t radius;
  const uint16_t lineWidth;
  const uint16_t lineColor;  // normal button colors
  const uint16_t fontColor;
  const uint16_t backColor;
  const uint16_t pLineColor;  // pressed button colors
  const uint16_t pFontColor;
  const uint16_t pBackColor;
} BUTTON;

void GUI_DrawButton(const BUTTON *button, uint8_t pressed);

typedef struct
{
  const uint16_t fontColor;
  const uint16_t backColor;
} WINDOW_ITEM;

typedef struct
{
  DIALOG_TYPE    type;
  const GUI_RECT rect;
  const uint16_t titleHeight;
  const uint16_t actionBarHeight;
  const uint16_t lineWidth;
  const uint16_t lineColor;
  const WINDOW_ITEM title;
  const WINDOW_ITEM info;
  const WINDOW_ITEM actionBar;
} WINDOW;

void GUI_DrawWindow(const WINDOW *window, const uint8_t *title, const uint8_t *inf, bool actionBar);

#ifdef __cplusplus
}
#endif

#endif
