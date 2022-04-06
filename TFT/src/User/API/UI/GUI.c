#include "GUI.h"
#include "includes.h"
#include <math.h>

uint16_t foreGroundColor = WHITE;
uint16_t backGroundColor = BLACK;
GUI_TEXT_MODE guiTextMode = GUI_TEXTMODE_NORMAL;
GUI_NUM_MODE guiNumMode = GUI_NUMMODE_SPACE;
uint16_t *iconBuffer;

void GUI_SetColor(uint16_t color)
{
  foreGroundColor = color;
}

uint16_t GUI_GetColor(void)
{
  return foreGroundColor;
}

void GUI_SetBkColor(uint16_t bkcolor)
{
  backGroundColor = bkcolor;
}

uint16_t GUI_GetBkColor(void)
{
  return backGroundColor;
}

void GUI_SetTextMode(GUI_TEXT_MODE mode)
{
  guiTextMode = mode;
}

GUI_TEXT_MODE GUI_GetTextMode(void)
{
  return guiTextMode;
}

void GUI_SetNumMode(GUI_NUM_MODE mode)
{
  guiNumMode = mode;
}

GUI_NUM_MODE GUI_GetNumMode(void)
{
  return guiNumMode;
}

void GUI_Clear(uint16_t color)
{
  uint32_t index=0;
  LCD_SetWindow(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
  for (index=0; index<LCD_WIDTH*LCD_HEIGHT; index++)
  {
    LCD_WR_16BITS_DATA(color);
  }
}

static uint8_t pixel_limit_flag = 0;
static GUI_RECT pixel_limit_rect;

void GUI_SetRange(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  pixel_limit_flag = 1;
  pixel_limit_rect.x0 = x0;
  pixel_limit_rect.x1 = x1;
  pixel_limit_rect.y0 = y0;
  pixel_limit_rect.y1 = y1;
}

void GUI_CancelRange(void)
{
  pixel_limit_flag = 0;
}

/** @brief Draw a pixel/point
 * @param x - x point
 * @param y - y point
 * @param color - color to be filled
*/
void GUI_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
  if (pixel_limit_flag == 1
    &&(x <  pixel_limit_rect.x0
     ||x >= pixel_limit_rect.x1
     ||y <  pixel_limit_rect.y0
     ||y >= pixel_limit_rect.y1))
    return ;

  LCD_SetWindow(x, y, x, y);
  LCD_WR_16BITS_DATA(color);
}

void GUI_DrawPoint(uint16_t x, uint16_t y)
{
  LCD_SetWindow(x, y, x, y);
  LCD_WR_16BITS_DATA(foreGroundColor);
}

/** @brief Draw a filled rectangle
 * @param x1 - x point of top left corner
 * @param y1 - y point of top left corner
 * @param x2 - x point of bottom right corner
 * @param y2 - y point of bottom right corner
*/
void GUI_FillRect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  uint16_t i=0, j=0;
  LCD_SetWindow(sx, sy, ex-1, ey-1);
  for (i=sx; i<ex; i++)
  {
    for (j=sy; j<ey; j++)
    {
      LCD_WR_16BITS_DATA(foreGroundColor);
    }
  }
}

void GUI_FillPrect(const GUI_RECT *rect)
{
  GUI_FillRect(rect->x0, rect->y0, rect->x1, rect->y1);
}

/** @brief Clear a rectangular area
 * @param x1 - x point of top left corner
 * @param y1 - y point of top left corner
 * @param x2 - x point of bottom right corner
 * @param y2 - y point of bottom right corner
*/
void GUI_ClearRect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  uint16_t i=0, j=0;
  LCD_SetWindow( sx, sy, ex-1, ey-1);
  for (i=sx; i<ex; i++)
  {
    for (j=sy; j<ey; j++)
    {
      LCD_WR_16BITS_DATA(backGroundColor);
    }
  }
}

void GUI_ClearPrect(const GUI_RECT *rect)
{
  GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);
}

/** @brief Draw a filled rectangle defined with color
 * @param x1 - x point of top left corner
 * @param y1 - y point of top left corner
 * @param x2 - x point of bottom right corner
 * @param y2 - y point of bottom right corner
 * @param color - color to be filled
*/
void GUI_FillRectColor(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
  uint16_t i=0, j=0;
  LCD_SetWindow(sx, sy, ex-1, ey-1);
  for (i=sx; i<ex; i++)
  {
    for (j=sy; j<ey; j++)
    {
      LCD_WR_16BITS_DATA(color);
    }
  }
}

void GUI_FillRectArry(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint8_t * arry)
{
  uint16_t i = 0, j = 0, color;
  LCD_SetWindow(sx, sy, ex - 1, ey - 1);
  for (i = sx; i < ex; i++)
  {
    for (j = sy; j < ey; j++)
    {
      color = *arry;
      arry++;
      color = (color << 8) | (*arry);
      arry++;
      LCD_WR_16BITS_DATA(color);
    }
  }
}

/** @brief Draw a straight line
 * @param x1 - x point of line start
 * @param y1 - y point of line start
 * @param x2 - y point of line end
 * @param y2 - x point of line end
*/
void GUI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1;
  delta_y = y2 - y1;
  uRow = x1;
  uCol = y1;
  if (delta_x > 0)
    incx = 1;
  else if (delta_x == 0)
    incx = 0;
  else
  { incx = -1; delta_x = -delta_x;}

  if (delta_y > 0)
    incy = 1;
  else if (delta_y == 0)
    incy = 0;
  else
  { incy = -1; delta_y = -delta_y;}

  if (delta_x > delta_y)
    distance = delta_x;
  else
    distance = delta_y;

  for (t=0; t <= distance + 1; t++ )  // draw all point on line
  {
    GUI_DrawPoint(uRow, uCol); // draw calculated point
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance)
    {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance)
    {
      yerr -= distance;
      uCol += incy;
    }
  }
}

/** @brief Draw a line at an angle
 * @param x - x point of line origin
 * @param y - y point of line origin
 * @param r - radius/ length of line
 * @param angle - angle in degree
*/
void GUI_DrawAngleLine(uint16_t x, uint16_t y, uint16_t r, int16_t angle)
{
  uint16_t ex,ey;
  float a = angle * 3.1415926f / 180;
  ex = x + cos(a) * r;
  ey = y - sin(a) * r;
  GUI_DrawLine(x, y, ex, ey);
}

/** @brief Draw a horiontal line
 * @param x1 - x point of line start
 * @param y - y point of line start
 * @param x2 - x point of line end
*/
void GUI_HLine(uint16_t x1, uint16_t y, uint16_t x2)
{
  uint16_t i=0;
  LCD_SetWindow(x1, y, x2-1, y);
  for (i=x1; i<x2; i++)
  {
    LCD_WR_16BITS_DATA(foreGroundColor);
  }
}

/** @brief Draw a vertical line
 * @param x - x point of line start
 * @param y1 - y point of line start
 * @param y2 - y point of line end
*/
void GUI_VLine(uint16_t x, uint16_t y1, uint16_t y2)
{
  uint16_t i=0;
  LCD_SetWindow(x, y1, x, y2-1);
  for (i=y1; i<y2; i++)
  {
    LCD_WR_16BITS_DATA(foreGroundColor);
  }
}

/** @brief Draw a rectangle
 * @param x1 - x point of top left corner
 * @param y1 - y point of top left corner
 * @param x2 - x point of bottom right corner
 * @param y2 - y point of bottom right corner
*/
void GUI_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  GUI_HLine(x1, y1, x2);
  GUI_HLine(x1, y2-1, x2);
  GUI_VLine(x1, y1, y2);
  GUI_VLine(x2-1, y1, y2);
}

/** @brief Draw a rectangle with predetermined dimensions
 * @param rect - pointer to rectangle dimensions
*/
void GUI_DrawPrect(const GUI_RECT *rect)
{
  GUI_DrawRect(rect->x0, rect->y0, rect->x1, rect->y1);
}

/** @brief Draw a circle using Bresenham’s Algorithm
 * @param x0 - x point of circle center
 * @param y0 - y point of circle center
 * @param r  - radius of circle
*/
void GUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
  int16_t a = 0,
  b = r,
  di = 3 - (r << 1);  // Diameter

  // draw points for each quadrant
  while (a <= b)
  {
    GUI_DrawPoint(x0 + a, y0 - b);  // 5
    GUI_DrawPoint(x0 + b, y0 - a);  // 0
    GUI_DrawPoint(x0 + b, y0 + a);  // 4
    GUI_DrawPoint(x0 + a, y0 + b);  // 6
    GUI_DrawPoint(x0 - a, y0 + b);  // 1
    GUI_DrawPoint(x0 - b, y0 + a);  // 3
    GUI_DrawPoint(x0 - a, y0 - b);  // 2
    GUI_DrawPoint(x0 - b, y0 - a);  // 7
    a++;

    if (di<0)
      di += (a<<2) + 6;
    else
    {
      di += 10 + ((a-b)<<2);
      b--;
    }
  }
}

/** @brief Draw a filled circle using Bresenham’s Algorithm
 * @param x0 - x point of circle center
 * @param y0 - y point of circle center
 * @param r  - radius of circle
*/
void  GUI_FillCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
  int16_t  draw_x0, draw_y0;
  int16_t  draw_x1, draw_y1;
  int16_t  draw_x2, draw_y2;
  int16_t  draw_x3, draw_y3;
  int16_t  draw_x4, draw_y4;
  int16_t  draw_x5, draw_y5;
  int16_t  draw_x6, draw_y6;
  int16_t  draw_x7, draw_y7;
  int16_t  fill_x0, fill_y0;
  int16_t  fill_x1;
  int16_t  xx, yy;
  int16_t  di;

  // return if radius is 0
  if (0 == r) return;


  draw_x0 = draw_x1 = x0;
  draw_y0 = draw_y1 = y0 + r;
  if (draw_y0 < LCD_HEIGHT)
  {
    GUI_DrawPoint(draw_x0, draw_y0);  // 90 degree
  }

  draw_x2 = draw_x3 = x0;
  draw_y2 = draw_y3 = y0 - r;
  if (draw_y2 >= 0)
  {
    GUI_DrawPoint(draw_x2, draw_y2);  // 270 degree
  }

  draw_x4 = draw_x6 = x0 + r;
  draw_y4 = draw_y6 = y0;
  if (draw_x4 < LCD_WIDTH)
  {
    GUI_DrawPoint(draw_x4, draw_y4);  // 0 degree
    fill_x1 = draw_x4;
  }
  else
  {
    fill_x1 = LCD_WIDTH;
  }

  fill_y0 = y0;
  fill_x0 = x0 - r;
  if (fill_x0<0)
  fill_x0 = 0;

  GUI_HLine(fill_x0, fill_y0, fill_x1);

  draw_x5 = draw_x7 = x0 - r;
  draw_y5 = draw_y7 = y0;
  if (draw_x5 >= 0)
  {
    GUI_DrawPoint(draw_x5, draw_y5);  // 180 degree
  }
  if (1==r) return;

  di = 3 - 2*r;  // Diameter
  xx = 0;
  yy = r;
  while (xx < yy)
  {
    if (di<0)
    {
      di += 4*xx + 6;
    }
    else
    {
      di += 4*(xx - yy) + 10;

      yy--;
      draw_y0--;
      draw_y1--;
      draw_y2++;
      draw_y3++;
      draw_x4--;
      draw_x5++;
      draw_x6--;
      draw_x7++;
    }

    xx++;
    draw_x0++;
    draw_x1--;
    draw_x2++;
    draw_x3--;
    draw_y4++;
    draw_y5++;
    draw_y6--;
    draw_y7--;

    if ((draw_x0 <= LCD_WIDTH) && (draw_y0 >= 0))
    {
      GUI_DrawPoint(draw_x0, draw_y0);
    }
    if ((draw_x1 >= 0) && (draw_y1 >= 0))
    {
      GUI_DrawPoint(draw_x1, draw_y1);
    }

    if (draw_x1 >= 0)
    {
      fill_x0 = draw_x1;
      fill_y0 = draw_y1;

      // fill upto max screen limits
      fill_y0 = NOBEYOND(0, fill_y0, LCD_HEIGHT);

      fill_x1 = x0 * 2 - draw_x1;
      // fill upto max screen height if fill_x1 is beyond screen width
      if (fill_x1 > LCD_WIDTH)
        fill_x1 = LCD_WIDTH;

      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }

    if ((draw_x2 <= LCD_WIDTH) && (draw_y2 <= LCD_HEIGHT) )
    {
      GUI_DrawPoint(draw_x2, draw_y2);
    }

    if ((draw_x3 >= 0) && (draw_y3 <= LCD_HEIGHT) )
    {
      GUI_DrawPoint(draw_x3, draw_y3);
    }

    if (draw_x3 >= 0)
    {
      fill_x0 = draw_x3;
      fill_y0 = draw_y3;

      // fill upto max screen limits
      fill_y0 = NOBEYOND(0, fill_y0, LCD_HEIGHT);

      fill_x1 = x0 * 2 - draw_x3;
      // fill upto max screen height if fill_x1 is beyond screen width
      if (fill_x1 > LCD_WIDTH)
        fill_x1 = LCD_WIDTH;

      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }

    if ((draw_x4 <= LCD_WIDTH) && (draw_y4 >= 0))
    {
      GUI_DrawPoint(draw_x4, draw_y4);
    }
    if ((draw_x5 >= 0) && (draw_y5 >= 0))
    {
      GUI_DrawPoint(draw_x5, draw_y5);
    }

    if (draw_x5 >= 0)
    {
      fill_x0 = draw_x5;
      fill_y0 = draw_y5;

      // fill upto max screen limits
      fill_y0 = NOBEYOND(0, fill_y0, LCD_HEIGHT);

      fill_x1 = x0*2 - draw_x5;
      // fill upto max screen height if fill_x1 is beyond screen width
      if (fill_x1 > LCD_WIDTH)
        fill_x1 = LCD_WIDTH;

      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }

    if ((draw_x6 <= LCD_WIDTH) && (draw_y6 <= LCD_HEIGHT))
    {
      GUI_DrawPoint(draw_x6, draw_y6);
    }

    if ((draw_x7 >= 0) && (draw_y7 <= LCD_HEIGHT))
    {
      GUI_DrawPoint(draw_x7, draw_y7);
    }

    if (draw_x7 >= 0)
    {
      fill_x0 = draw_x7;
      fill_y0 = draw_y7;

      // fill upto max screen limits
      fill_y0 = NOBEYOND(0, fill_y0, LCD_HEIGHT);

      fill_x1 = x0*2 - draw_x7;
      // fill upto max screen height if fill_x1 is beyond screen width
      if (fill_x1 > LCD_WIDTH)
        fill_x1 = LCD_WIDTH;
      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }
  }
}

// get the text starting point on screen based on rectangle edges and desired display position
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

void GUI_DispOne(int16_t sx, int16_t sy, const CHAR_INFO *pInfo)
{
  if (pInfo->bytes == 0) return;

  uint8_t w = pInfo->pixelWidth;
  uint8_t h = pInfo->pixelHeight;
  uint16_t bitMapSize = (h * w / 8);
  uint8_t font[bitMapSize];
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t j = 0;
  uint8_t i = 0;
  uint8_t jj = (h + 8 - 1) / 8;
  uint32_t pixel = 1 << (h - 1);
  uint32_t temp = 0;

  W25Qxx_ReadBuffer(font, pInfo->bitMapAddr, bitMapSize);

  // NOTE: the following code was split intentionally for speedup performance despite some more flash usage

  if (guiTextMode == GUI_TEXTMODE_TRANS)
  {
    for (x = 0; x < w; x++)
    {
      for (j = 0; j < jj; j++)
      {
        temp <<= 8;
        temp |= font[i++];
      }

      for (y = 0; y < h; y++)
      {
        if (temp & pixel)  // draw text pixel
          GUI_DrawPixel(sx, sy + y, foreGroundColor);

        temp <<= 1;
      }

      sx++;
    }
  }
  else  // if GUI_TEXTMODE_NORMAL or GUI_TEXTMODE_ON_ICON
  {
    uint16_t _buf[h * w];
    uint16_t *buf;
    GUI_RECT limit = {0};

    if (guiTextMode == GUI_TEXTMODE_NORMAL)
      buf = _buf;
    else  // if GUI_TEXTMODE_ON_ICON, use iconBuffer
      buf = iconBuffer;

    if (pixel_limit_flag == 1)
    {
      if (sx < pixel_limit_rect.x0)
        limit.x0 = pixel_limit_rect.x0 - sx;

      if (sx + w >= pixel_limit_rect.x1)
        limit.x1 = sx + w - pixel_limit_rect.x1;

      if (sy < pixel_limit_rect.y0)
        limit.y0 = pixel_limit_rect.y0 - sy;

      if (sy + h >= pixel_limit_rect.y1)
        limit.y1 = sy + h - pixel_limit_rect.y1;
    }

    for (x = 0; x < w; x++)
    {
      for (j = 0; j < jj; j++)
      {
        temp <<= 8;
        temp |= font[i++];
      }

      for (y = 0; y < h; y++)
      {
        if (temp & pixel)                             // draw text pixel
          buf[(y * w) + x] = foreGroundColor;
        else if (guiTextMode == GUI_TEXTMODE_NORMAL)  // draw background pixel
          buf[(y * w) + x] = backGroundColor;

        temp <<= 1;
      }
    }

    lcd_buffer_display(sx, sy, w, h, buf, &limit);
  }
}

void _GUI_DispString(int16_t x, int16_t y, const uint8_t *p)
{
  if (p == NULL) return;

  CHAR_INFO info;

  while (*p)
  {
    getCharacterInfo(p, &info);
    GUI_DispOne(x, y, &info);
    x += info.pixelWidth;
    p += info.bytes;
  }
}

const uint8_t* _GUI_DispLenString(int16_t x, int16_t y, const uint8_t *p, uint16_t pixelWidth, bool truncate)
{
  if (p == NULL) return NULL;

  CHAR_INFO info;
  uint16_t curPixelWidth = 0;

  if (truncate) pixelWidth -= BYTE_HEIGHT;

  while (curPixelWidth < pixelWidth && *p)
  {
    getCharacterInfo(p, &info);
    if (curPixelWidth + info.pixelWidth > pixelWidth)
    {
      if (truncate)
      {
        getCharacterInfo((uint8_t *)"…", &info);
        GUI_DispOne(x, y, &info);
      }
      return p;
    }
    GUI_DispOne(x, y, &info);
    x += info.pixelWidth;
    curPixelWidth += info.pixelWidth;
    p += info.bytes;
  }
  return p;
}

void _GUI_DispStringRight(int16_t x, int16_t y, const uint8_t *p)
{
  x -= GUI_StrPixelWidth(p);
  _GUI_DispString(x, y, p);
}

void _GUI_DispStringCenter(int16_t x, int16_t y, const uint8_t *p)
{
  x -= GUI_StrPixelWidth(p)/2;
  _GUI_DispString(x, y, p);
}

void _GUI_DispStringInRect(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint8_t *p)
{
  uint16_t stringlen = GUI_StrPixelWidth(p);
  uint16_t width = ex - sx;
  uint16_t height = ey - sy;
  uint8_t nline = (stringlen + width - 1) / width;

  if (nline > height / BYTE_HEIGHT)
    nline = height / BYTE_HEIGHT;

  uint16_t x_offset = stringlen >= width ? 0 : (width - stringlen) >> 1;
  uint16_t y_offset = (nline * BYTE_HEIGHT) >= height ? 0 : ((height - (nline * BYTE_HEIGHT)) >> 1);
  uint16_t x = sx + x_offset, y = sy + y_offset;

  uint8_t i = 0;
  for (i = 0; i < nline; i++)
  {
    p = GUI_DispLenString(x, y, p, width, false);
    y += BYTE_HEIGHT;
  }
}

void _GUI_DispStringInPrect(const GUI_RECT *rect, const uint8_t *p)
{
  _GUI_DispStringInRect(rect->x0, rect->y0, rect->x1, rect->y1, p);
}

static GUI_POINT GUI_DisplayWordInPrect(const GUI_RECT *rect, GUI_POINT cursor, uint8_t *str)
{
  CHAR_INFO info;
  // return cursor to new line if line is full or on new line character
  if ((cursor.x + GUI_StrPixelWidth(str)) > rect->x1 || (*str == '\n'))
  {
    getCharacterInfo(str, &info);
    cursor.x = rect->x0;
    cursor.y += info.pixelHeight;

    if ((cursor.y + info.pixelHeight) > rect->y1)
      cursor.y = rect->y0;

    if (*str == ' ' || *str == '\n')  // treat space as new line if it causes line change
      return cursor;
  }

  // draw word
  while (*str)
  {
    getCharacterInfo(str, &info);
    // check line width after each character to clip long words to rect width
    if ((cursor.x + info.pixelWidth) > rect->x1)
    {
      cursor.x = rect->x0;
      cursor.y += info.pixelHeight;

      if ((cursor.y + info.pixelHeight) > rect->y1)
        cursor.y = rect->y0;
    }

    GUI_DispOne(cursor.x, cursor.y, &info);
    cursor.x += info.pixelWidth;
    str += info.bytes;
  }

  return cursor;
}

void _GUI_DispStringInRectEOL(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint8_t *p)
{
  if (p == NULL || *p == 0) return;

  GUI_RECT rect = (GUI_RECT){sx, sy, ex, ey};
  GUI_POINT cursor = (GUI_POINT){sx, sy};
  uint16_t bufLength = 0;
  char buf[100];

  while (*p)
  {
    if (*p == ' ' || *p == '\n')
    {
      if (bufLength)
      {
        // draw word from buffer before reading next word
        cursor = GUI_DisplayWordInPrect(&rect, cursor, (uint8_t*)buf);
        bufLength = 0;
        buf[bufLength] = 0;
      }
      // draw space or new line individually
      cursor = GUI_DisplayWordInPrect(&rect, cursor, (uint8_t *)((*p == ' ') ? " " : "\n"));
    }
    else  // copy word character to buffer
    {
      buf[bufLength++] = *p;
      buf[bufLength] = 0;
    }

    p++;

    if (!*p && bufLength)  // draw remaining text if reached end of string
    {
      cursor = GUI_DisplayWordInPrect(&rect, cursor, (uint8_t*)buf);
      bufLength = 0;
      buf[bufLength] = 0;
    }
  }
}

void _GUI_DispStringInPrectEOL(const GUI_RECT *rect, const uint8_t *p)
{
  _GUI_DispStringInRectEOL(rect->x0, rect->y0, rect->x1, rect->y1, p);
}

void _GUI_DispStringOnIcon(uint16_t iconIndex, GUI_POINT iconPoint, GUI_POINT textPos, const uint8_t *p)
{
  if (p == NULL) return;

  CHAR_INFO info;
  uint16_t _iconBuffer[LARGE_BYTE_WIDTH * LARGE_BYTE_HEIGHT];

  iconBuffer = _iconBuffer;
  GUI_SetTextMode(GUI_TEXTMODE_ON_ICON);

  while (*p)
  {
    getCharacterInfo(p, &info);
    ICON_ReadBuffer(iconBuffer, textPos.x, textPos.y, info.pixelWidth, info.pixelHeight, iconIndex);
    GUI_DispOne(iconPoint.x + textPos.x, iconPoint.y + textPos.y, &info);

    textPos.x += info.pixelWidth;
    p += info.bytes;
  }

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

const uint32_t GUI_Pow10[10] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

void GUI_DispDec(int16_t x, int16_t y, int32_t num, uint8_t len, uint8_t leftOrRight)
{
  uint8_t i;
  uint8_t bit_value;
  uint8_t blank_bit_len = 0;
  uint8_t notZero = 0;
  char    isNegative = 0;
  uint8_t decBuf[64];
  uint8_t bufIndex = 0;

  if (num < 0)
  {
    num = -num;
    isNegative = 1;
    len--;  // Negative '-' takes up a display length
  }

  for (i = 0; i < len; i++)
  {
    bit_value = (num / GUI_Pow10[len - i - 1]) % 10;
    if (notZero == 0)
    {
      if (bit_value == 0 && i < (len - 1))
      {
        if (leftOrRight == RIGHT)
        {
          decBuf[bufIndex++] = (guiNumMode == GUI_NUMMODE_SPACE) ? ' ' : '0';
        }
        else
        {
          blank_bit_len++;
        }
        continue;
      }
      else
      {
        notZero = 1;
        if (isNegative)
        {
          decBuf[bufIndex++] = '-';
        }
      }
    }
    decBuf[bufIndex++] = bit_value + '0';
  }
  for (; blank_bit_len > 0; blank_bit_len--)
  {
    decBuf[bufIndex++] = ' ';
  }
  decBuf[bufIndex] = 0;
  _GUI_DispString(x, y, decBuf);
}

void GUI_DispFloat(int16_t x, int16_t y, float num, uint8_t llen, uint8_t rlen, uint8_t leftOrRight)
{
  uint8_t  alen = 0;
  uint8_t  i = 0;
  uint8_t  notZero = 0;
  char     isNegative = 0;
  uint8_t  floatBuf[64];
  uint8_t  bufIndex = 0;

  if (num<0)
  {
    num = -num;
    isNegative = 1;
    llen--;  // Negative '-' takes up a display length
  }

  num *= GUI_Pow10[(unsigned)rlen];
  num += 0.5f;
  num = (float) floor(num);
  for (i=0; i<llen; i++)
  {
    uint8_t bit_value = ((uint32_t)(num/GUI_Pow10[llen+rlen-1-i]))%10;
    if (notZero == 0)
    {
      if (bit_value == 0 && i<(llen-1))
      {
        if (leftOrRight==RIGHT)
        {
          floatBuf[bufIndex++] = (guiNumMode == GUI_NUMMODE_SPACE) ? ' ' : '0';
          alen++;
        }
        continue;
      }
      else
      {
        notZero = 1;
        if (isNegative)
        {
          floatBuf[bufIndex++] = '-';
        }
      }
    }
    floatBuf[bufIndex++] = bit_value + '0';
    alen++;
  }
  floatBuf[bufIndex++] = '.';
  alen++;

  for (i=0;i<rlen;i++)
  {
    floatBuf[bufIndex++] = (int)(num/GUI_Pow10[rlen-1-i])%10+'0';
    alen++;
  }
  for (; alen < llen+rlen+1; alen++)
  {
    floatBuf[bufIndex++] = ' ';
  }
  floatBuf[bufIndex] = 0;
  _GUI_DispString(x, y, floatBuf);
}

void _GUI_DispLabel(int16_t x, int16_t y, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText((uint8_t*)&tempstr, index) == false) return;
  _GUI_DispString(x, y, tempstr);
}

const uint8_t* _GUI_DispLenLabel(int16_t x, int16_t y, uint16_t index, uint16_t pixelWidth, bool truncate)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return NULL;
  return _GUI_DispLenString(x, y, tempstr, pixelWidth, truncate);
}

void _GUI_DispLabelRight(int16_t x, int16_t y, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return;
  _GUI_DispStringRight(x, y, tempstr);
}

void _GUI_DispLabelCenter(int16_t x, int16_t y, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return;
  _GUI_DispStringCenter(x, y, tempstr);
}

void _GUI_DispLabelInRect(int16_t sx, int16_t sy, int16_t ex, int16_t ey, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return;
  _GUI_DispStringInRect(sx, sy, ex, ey, tempstr);
}

void _GUI_DispLabelInPrect(const GUI_RECT *rect, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return;
  _GUI_DispStringInPrect(rect, tempstr);
}

void _GUI_DispLabelInRectEOL(int16_t sx, int16_t sy, int16_t ex, int16_t ey, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return;
  _GUI_DispStringInRectEOL(sx, sy, ex, ey, tempstr);
}

void _GUI_DispLabelInPrectEOL(const GUI_RECT *rect, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return;
  _GUI_DispStringInPrectEOL(rect, tempstr);
}

void _GUI_DispLabelOnIcon(uint16_t iconIndex, GUI_POINT iconPoint, GUI_POINT textPos, uint16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText(tempstr, index) == false) return;
  _GUI_DispStringOnIcon(iconIndex, iconPoint, textPos, tempstr);
}

/****************************************************     Widget    *******************************************************************/
#define RADIO_SELECTED_COLOR GREEN
#define RADIO_IDLE_COLOR     WHITE

void RADIO_Create(RADIO *radio)
{
  uint16_t tmp = GUI_GetColor();
  uint8_t i=0;
  for (i=0;i<radio->num;i++)
  {
    if (i==radio->select)
      GUI_SetColor(RADIO_SELECTED_COLOR);
    else
      GUI_SetColor(RADIO_IDLE_COLOR);
    GUI_FillCircle(radio->sx+BYTE_HEIGHT/2, i*radio->distance+radio->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/8);
    GUI_DrawCircle(radio->sx+BYTE_HEIGHT/2, i*radio->distance+radio->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/4);
    GUI_DispString(radio->sx+BYTE_HEIGHT,   i*radio->distance+radio->sy, radio->context[i]);
  }
  GUI_SetColor(tmp);
}

void RADIO_Select(RADIO *radio, uint8_t select)
{
  uint16_t tmp = GUI_GetColor();
  uint8_t i=0;
  if (radio->select==select)
  return;
  for (i=0;i<2;i++)
  {
    if (i==0)
    {
      GUI_SetColor(RADIO_IDLE_COLOR);
    }
    else
    {
      radio->select=select;
      GUI_SetColor(RADIO_SELECTED_COLOR);
    }
    GUI_FillCircle(radio->sx+BYTE_HEIGHT/2, radio->select*radio->distance+radio->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/8);
    GUI_DrawCircle(radio->sx+BYTE_HEIGHT/2, radio->select*radio->distance+radio->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/4);
    GUI_DispString(radio->sx+BYTE_HEIGHT,   radio->select*radio->distance+radio->sy, radio->context[radio->select]);
  }
  GUI_SetColor(tmp);
}

void Scroll_CreatePara(SCROLL * para, const uint8_t *pstr, const GUI_RECT *prect)
{
  CHAR_INFO info;
  getCharacterInfo(pstr, &info);
  memset(para,0,sizeof(SCROLL));
  para->text = (uint8_t *const)pstr;
  para->maxByte = strlen((char *)pstr);
  para->curPixelWidth = para->totalPixelWidth = GUI_StrPixelWidth(pstr);
  para->maxPixelWidth = prect->x1 - prect->x0;
  para->rect.x0 = prect->x0;
  para->rect.x1 = prect->x1;
  para->rect.y0 = prect->y0 + (((prect->y1 - prect->y0) - info.pixelHeight) / 2);
  para->rect.y1 = para->rect.y0 + info.pixelHeight;
}

void Scroll_DispString(SCROLL * para, uint8_t align)
{
  uint16_t i = 0;
  CHAR_INFO info;

  if (para->text == NULL) return;
  if (para->totalPixelWidth > para->maxPixelWidth)
  {
    if (OS_GetTimeMs() > para->time)
    {
      para->time = OS_GetTimeMs() + 50;  // 50ms
      GUI_SetRange(para->rect.x0, para->rect.y0, para->rect.x1, para->rect.y1);
      if (para->curByte < para->maxByte)
      {
        getCharacterInfo(&para->text[para->curByte], &info);
        para->off_head++;
        if (para->off_head == info.pixelWidth)
        {
          para->curByte += info.bytes;
          para->off_head = 0;
        }

        GUI_DispLenString(para->rect.x0 - para->off_head, para->rect.y0, &para->text[para->curByte], para->maxPixelWidth + info.pixelWidth, false);

        para->curPixelWidth--;
        if (para->curPixelWidth < para->maxPixelWidth)
        {
          for (i = para->rect.y0; i<para->rect.y1; i++)
          {
            GUI_DrawPixel(para->rect.x0 + para->curPixelWidth, i, backGroundColor);
          }
        }
      }

      if (para->curPixelWidth + 2*BYTE_WIDTH < para->maxPixelWidth)
      {
        para->off_tail++;
        GUI_DispLenString(para->rect.x1-para->off_tail, para->rect.y0, para->text, para->off_tail, false);
        if (para->off_tail + para->rect.x0 >= para->rect.x1)
        {
          para->off_head=0;
          para->off_tail=0;
          para->curByte=0;
          para->curPixelWidth = para->totalPixelWidth;
        }
      }
      GUI_CancelRange();
    }
  }
  else if (!para->has_disp)
  {
    switch (align)
    {
      case LEFT:
      {
        GUI_DispString(para->rect.x0, para->rect.y0, para->text);
        break;
      }
      case RIGHT:
      {
        uint16_t x_offset=(para->rect.x1 - para->totalPixelWidth);
        GUI_DispString(x_offset, para->rect.y0, para->text);
        break;
      }
      case CENTER:
      {
        uint16_t x_offset=((para->rect.x1 - para->rect.x0 - para->totalPixelWidth) >> 1);
        GUI_DispString(para->rect.x0+x_offset, para->rect.y0, para->text);
        break;
      }
    }
    para->has_disp = 1;
  }
}

void GUI_DrawButton(const BUTTON *button, uint8_t pressed)
{
  const uint16_t radius = button->radius;
  const uint16_t lineWidth = button->lineWidth;
  const int16_t sx = button->rect.x0;
  const int16_t sy = button->rect.y0;
  const int16_t ex = button->rect.x1;
  const int16_t ey = button->rect.y1;
  const uint16_t nowBackColor = GUI_GetBkColor();
  const uint16_t nowFontColor = GUI_GetColor();
  const GUI_TEXT_MODE nowTextMode = GUI_GetTextMode();

  const uint16_t lineColor = pressed ? button->pLineColor : button->lineColor;
  const uint16_t backColor = pressed ? button->pBackColor : button->backColor;
  const uint16_t fontColor = pressed ? button->pFontColor : button->fontColor;

  GUI_SetColor(lineColor);
  GUI_FillCircle(sx + radius,     sy + radius,  radius);
  GUI_FillCircle(ex - radius - 1, sy + radius,  radius);
  GUI_FillCircle(sx + radius,     ey - radius - 1, radius);
  GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius);

  for (uint16_t i = 0; i < lineWidth; i++)
  {
    GUI_HLine(sx + radius, sy + i,      ex - radius);
    GUI_HLine(sx + radius, ey - 1 - i,  ex - radius);
    GUI_VLine(sx + i,      sy + radius, ey - radius);
    GUI_VLine(ex - 1 - i,  sy + radius, ey - radius);
  }

  GUI_SetColor(backColor);
  GUI_FillCircle(sx + radius,     sy + radius,  radius - lineWidth);
  GUI_FillCircle(ex - radius - 1, sy + radius,  radius - lineWidth);
  GUI_FillCircle(sx + radius,     ey - radius - 1, radius - lineWidth);
  GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius - lineWidth);
  GUI_FillRect(sx + radius, sy + lineWidth, ex - radius, sy + lineWidth + radius);
  GUI_FillRect(sx + lineWidth, sy + lineWidth + radius, ex - lineWidth, ey - lineWidth - radius);
  GUI_FillRect(sx + radius, ey - lineWidth - radius, ex - radius, ey - lineWidth);

  GUI_SetColor(fontColor);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispStringInPrect(&button->rect, button->context);

  GUI_SetBkColor(nowBackColor);
  GUI_SetColor(nowFontColor);
  GUI_SetTextMode(nowTextMode);
}

void GUI_DrawWindow(const WINDOW *window, const uint8_t *title, const uint8_t *inf, bool actionBar)
{
  uint16_t title_y0 = window->rect.y0 + (window->titleHeight - BYTE_HEIGHT) / 2;
  uint16_t title_y1 = window->rect.y0 + window->titleHeight;
  uint16_t info_y1 = (actionBar) ? window->rect.y1 - window->actionBarHeight : window->rect.y1;
  uint8_t margin = BYTE_WIDTH / 2;

  // draw title background
  GUI_SetColor(window->title.backColor);
  GUI_FillRect(window->rect.x0, window->rect.y0, window->rect.x1, title_y1);

  // draw info background
  GUI_SetColor(window->info.backColor);
  GUI_FillRect(window->rect.x0, title_y1, window->rect.x1, info_y1);

  if (actionBar)
  { // draw action bar background
    GUI_SetColor(window->actionBar.backColor);
    GUI_FillRect(window->rect.x0, info_y1, window->rect.x1, window->rect.y1);
  }

  // draw window type icon
  uint8_t * char_icon;

  switch (window->type)
  {
    case DIALOG_TYPE_ALERT:
      GUI_SetColor(ORANGE);
      char_icon = IconCharSelect(CHARICON_ALERT);
      break;

    case DIALOG_TYPE_QUESTION:
      GUI_SetColor(PURPLE);
      char_icon = IconCharSelect(CHARICON_QUESTION);
      break;

    case DIALOG_TYPE_ERROR:
      GUI_SetColor(RED);
      char_icon = IconCharSelect(CHARICON_ERROR);
      break;

    case DIALOG_TYPE_SUCCESS:
      GUI_SetColor(GREEN);
      char_icon = IconCharSelect(CHARICON_OK);
      break;

    case DIALOG_TYPE_INFO:
    default:
      GUI_SetColor(BLUE);
      char_icon = IconCharSelect(CHARICON_INFO);
      break;
  }

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispString(window->rect.x0 + BYTE_WIDTH, title_y0, char_icon);

  // draw title accent line
  GUI_DrawRect(window->rect.x0, title_y1 - 1, window->rect.x1, title_y1 + 1);

  if (actionBar)
  {  // draw actionbar accent line
    GUI_SetColor(GRAY);
    GUI_DrawRect(window->rect.x0, info_y1 - 1, window->rect.x1, info_y1 + 1);
  }

  // draw window border
  GUI_SetColor(window->lineColor);

  for (uint8_t i = 0; i < window->lineWidth; i++)
  {
    GUI_DrawRect(window->rect.x0 - i, window->rect.y0 - i, window->rect.x1 + i, window->rect.y1 + i);
  }

  // draw title text
  GUI_SetColor(window->title.fontColor);
  GUI_DispLenString(window->rect.x0 + BYTE_HEIGHT * 2, title_y0, title,
                    window->rect.x1 - (window->rect.x0 + BYTE_HEIGHT * 2), true);

  // draw info text
  GUI_SetColor(window->info.fontColor);

  if ((GUI_StrPixelWidth(inf) < window->rect.x1 - window->rect.x0) && (strchr((const char *)inf,'\n') == NULL))
    GUI_DispStringInRect(window->rect.x0, title_y1, window->rect.x1, info_y1, inf);
  else
    GUI_DispStringInRectEOL(window->rect.x0 + margin, title_y1 + margin, window->rect.x1 - margin, info_y1 - margin, inf);

  GUI_RestoreColorDefault();
}
