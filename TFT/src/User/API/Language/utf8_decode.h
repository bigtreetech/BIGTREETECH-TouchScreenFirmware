#ifndef _UTF8_DECODE_H_
#define _UTF8_DECODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  uint32_t startCodePoint;        // start unicode code point for language
  uint32_t endCodePoint;          // end unicode code point for language
  uint8_t  pixelHeight;           // font display pixel height
  uint8_t  pixelWidth;            // font display pixel width
  uint32_t bitMapStartAddr;       // dot matrix font library start address in w25qxx
  uint8_t  bitMapHeight;          // dot matrix font library pixel height
  uint8_t  bitMapWidth;           // dot matrix font library pixel width
  uint32_t bitMapStartCodePoint;  // the first character code point in this font bitmap file
} FONT_BITMAP;

typedef struct
{
  // encode info
  uint8_t  bytes;        // number of bytes occupied by one character
  uint32_t codePoint;    // actual encoding index of characters
  // font info
  uint8_t  pixelHeight;  // the pixel height of a character display
  uint8_t  pixelWidth;   // the pixel width of a character display
  uint32_t bitMapAddr;   // the address of font bitmap in w25qxx
} CHAR_INFO;

void setFontSize(uint16_t size);

void getCharacterInfo(const uint8_t * ch, CHAR_INFO * pInfo);
uint16_t getUTF8Length(const uint8_t * const str);

uint16_t GUI_StrPixelWidth_str(const uint8_t * str);
uint16_t GUI_StrPixelWidth_label(int16_t index);

#define GUI_StrPixelWidth(X) _Generic(((X+0)), const uint8_t * const: GUI_StrPixelWidth_str, \
                                                     const uint8_t *: GUI_StrPixelWidth_str, \
                                                           uint8_t *: GUI_StrPixelWidth_str, \
                                                             default: GUI_StrPixelWidth_label)(X)
#ifdef __cplusplus
}
#endif

#endif
