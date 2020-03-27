#ifndef _GUI_H_
#define _GUI_H_

#include "stdint.h"
#include "utf8_decode.h"

enum
{
  LEFT=0,
  RIGHT,
  CENTER,
};

typedef enum
{
  GUI_TEXTMODE_TRANS,
  GUI_TEXTMODE_NORMAL,
}GUI_TEXT_MODE;

typedef enum
{
  GUI_NUMMODE_SPACE,
  GUI_NUMMODE_ZERO,
}GUI_NUM_MODE;

typedef struct
{
  int16_t x, y;
}GUI_POINT;

typedef struct
{
  int16_t x0, y0, x1, y1;
}GUI_RECT;


void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);

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
CHAR_INFO GUI_DispOne(int16_t sx, int16_t sy, const uint8_t *p);
void GUI_DispString(int16_t x, int16_t y, const uint8_t *p);
const uint8_t* GUI_DispLenString(int16_t x, int16_t y, const uint8_t *p, uint16_t pixelWidth);
void GUI_DispStringRight(int16_t x, int16_t y, const uint8_t *p);
void GUI_DispStringInRect(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint8_t *p);
void GUI_DispStringInPrect(const GUI_RECT *rect, const uint8_t *p);
void GUI_DispStringInRectEOL(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint8_t *p);

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
}RADIO;

void RADIO_Create(RADIO *raido);
void RADIO_Select(RADIO *raido, uint8_t select);

typedef struct
{
  const GUI_RECT *rect;
  uint8_t  *text;
  uint32_t time;
  int16_t	 off_head;
  int16_t  off_tail;
  uint16_t maxByte;
  uint16_t curByte;
  uint16_t totalPixelWidth;
  int16_t  curPixelWidth;
  uint16_t maxPixelWidth;
  uint8_t  has_disp;
}SCROLL;
void Scroll_CreatePara(SCROLL * para, uint8_t *pstr, const GUI_RECT *rect);
void Scroll_DispString(SCROLL * para, uint8_t align);

typedef struct
{
  const GUI_RECT rect;
  const uint8_t  *context;
  const uint16_t radius;
  const uint16_t lineWidth;
  const uint16_t lineColor;  //����״̬�µ���ɫ
  const uint16_t fontColor;
  const uint16_t backColor;
  const uint16_t pLineColor; //��ѹ״̬�µ���ɫ
  const uint16_t pFontColor;
  const uint16_t pBackColor;
}BUTTON;

void GUI_DrawButton(const BUTTON *button, uint8_t pressed);

typedef struct
{
  const uint16_t fontColor;
  const uint16_t backColor;
  const int16_t  height;
}WINDOW_ITEM;

typedef struct
{
  const GUI_RECT rect;
  const uint16_t radius;
  const uint16_t lineWidth;
  const uint16_t lineColor;
  const WINDOW_ITEM title;
  const WINDOW_ITEM info;
  const WINDOW_ITEM bottom;
}WINDOW;

void GUI_DrawWindow(const WINDOW *window, const uint8_t *title, const uint8_t *inf);

#endif
