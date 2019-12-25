#include "utf8_decode.h"
#include "includes.h"


static FONT_BITMAP font[] = {
  { // Visible ASCII code, from ' ' to '~'
    // start unicode code point for language
    0x20, // 0x20 means the first visible character ' '
    // end unicode code point for language
    0x7E, // 0x7E means the last visible character '~'
    // font display pixel height
    BYTE_HEIGHT,
    // font display pixel width
    BYTE_WIDTH,
    // dot matrix font library start address in w25qxx
    BYTE_ASCII_ADDR,
    // dot matrix font library pixel height
    BYTE_HEIGHT,
    // dot matrix font library pixel width
    BYTE_WIDTH,
    // the first character code point in this font bitmap file
    0x20, // the first character in BYTE_ASCII_ADDR is 0x20(' ')
  },
  { // Czech(Latin 1 Supplement, Extended-A&B)
    0x80,
    0x24F,
    BYTE_HEIGHT,
    BYTE_WIDTH,
    WORD_UNICODE,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2, // default "word_unicode.fon" dot matrix library font size is 24*24 / 16*16
    0x0,  // the first character in WORD_UNICODE is 0x0000
  },
  { // Cyrillic(Russia)
    0x400,
    0x451,
    BYTE_HEIGHT,
    BYTE_WIDTH,
    WORD_UNICODE,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,
    0,
  },
  { // Armenian
    0x530,
    0x58F,
    BYTE_HEIGHT,
    BYTE_WIDTH,
    WORD_UNICODE,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2, // default word_unicode.fon dot matrix library font size is 24*24 / 16*16
    0,
  },
  { // default font
    0x0,
    0xFFFF,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,
    WORD_UNICODE,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,
    0,
  },
};

// decode character encode info (UTF8)
static void getUTF8EncodeInfo(const uint8_t *ch, CHAR_INFO *pInfo)
{
  uint8_t i;
  uint8_t bytes = 0;
  uint8_t utfFlg = 0x80;
  
  while ((ch[0] & utfFlg) == utfFlg)
  {
    utfFlg |= (utfFlg >> 1);
    bytes++;
  }
  // bytes == 0 means 1 byte, ASCII, 0XXX XXXX
  pInfo->bytes = bytes ? bytes : 1;
  pInfo->codePoint = ch[0] & (~utfFlg);
  for (i = 1; i < bytes; i++)
  {
    pInfo->codePoint = (pInfo->codePoint << 6) | (ch[i] & 0x3F);
  }
}

// get character font bitmap info
static void getBitMapFontInfo(CHAR_INFO *pInfo)
{
  uint8_t i;
  for (i = 0; i < COUNT(font); i++)
  {
    if(pInfo->codePoint >= font[i].startCodePoint && pInfo->codePoint <= font[i].endCodePoint)
    {
      pInfo->pixelWidth = font[i].pixelWidth;
      pInfo->pixelHeight = font[i].pixelHeight;
      pInfo->bitMapAddr = font[i].bitMapStartAddr + (pInfo->codePoint - font[i].bitMapStartCodePoint) * (font[i].bitMapHeight * font[i].bitMapWidth / 8);
      return;
    }
  }
}

//
void getCharacterInfo(const uint8_t *ch, CHAR_INFO *pInfo)
{
  pInfo->bytes = 0;
    
  if(ch == NULL || *ch == 0) return;
  
  getUTF8EncodeInfo(ch, pInfo);
  
  getBitMapFontInfo(pInfo);
}

// decode UTF-8 char display bit width
uint16_t GUI_StrPixelWidth(const uint8_t *const str)
{
  uint16_t i = 0, len = 0;
  CHAR_INFO info;
  
  if(str == NULL) return 0;
  while(str[i])
  {
    getCharacterInfo(str + i, &info);
    i += info.bytes;
    len += info.pixelWidth;
  }
  return len;
}
