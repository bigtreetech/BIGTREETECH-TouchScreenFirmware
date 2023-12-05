#include "utf8_decode.h"
#include "includes.h"

// High 8bits: font height
// Low 8bits: font width
uint16_t fontSize = FONT_SIZE_NORMAL;

static FONT_BITMAP font[] = {
  { // Visible ASCII code, from ' ' to '~'
    // start unicode code point for language
    0x20,  // 0x20 means the first visible character ' '
    // end unicode code point for language
    0x7E,  // 0x7E means the last visible character '~'
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
    0x20,  // the first character in BYTE_ASCII_ADDR is 0x20(' ')
  },
  { // large ASCII font
    0x20,
    0x7E,
    LARGE_BYTE_HEIGHT,
    LARGE_BYTE_WIDTH,
    LARGE_FONT_ADDR,
    LARGE_BYTE_HEIGHT,
    LARGE_BYTE_WIDTH,
    0x20,
  },
  { // Czech(Latin 1 Supplement, Extended-A&B)
    0x80,
    0x24F,
    BYTE_HEIGHT,
    BYTE_WIDTH,
    WORD_UNICODE_ADDR,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,  // default "word_unicode.fon" dot matrix library font size is 24*24 / 16*16
    0x0,             // the first character in WORD_UNICODE_ADDR is 0x0000
  },
  { // Greek and Coptic
    0x370,
    0x3FF,
    BYTE_HEIGHT,
    BYTE_WIDTH,
    WORD_UNICODE_ADDR,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,  // default "word_unicode.fon" dot matrix library font size is 24*24 / 16*16
    0x0,             // the first character in WORD_UNICODE_ADDR is 0x0000
  },
  { // Cyrillic(Russia)
    0x400,
    0x451,
    BYTE_HEIGHT,
    BYTE_WIDTH,
    WORD_UNICODE_ADDR,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,
    0,
  },
  { // Armenian
    0x530,
    0x58F,
    BYTE_HEIGHT,
    BYTE_WIDTH,
    WORD_UNICODE_ADDR,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,  // default word_unicode.fon dot matrix library font size is 24*24 / 16*16
    0,
  },
  { // default font
    0x9,
    0xFFFF,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,
    WORD_UNICODE_ADDR,
    BYTE_HEIGHT,
    BYTE_WIDTH * 2,
    0,
  },
};

// set Large font
void setFontSize(uint16_t size)
{
  fontSize = size;
}

// decode character encode info (UTF8)
static void getUTF8EncodeInfo(const uint8_t *ch, CHAR_INFO *pInfo)
{
  if ((ch[0] & 0x80) == 0)  // 0x00 ~ 0x7F
  {
    pInfo->bytes = 1;
    pInfo->codePoint = (ch[0] & 0x7F);
  }
  else if ((ch[0] & 0xE0) == 0xC0)  // 0x80 ~ 0x7FF
  {
    pInfo->bytes = 2;
    pInfo->codePoint = (ch[0] & 0x1F);
  }
  else if ((ch[0] & 0xF0) == 0xE0)  // 0x800 ~ 0xFFFF
  {
    pInfo->bytes = 3;
    pInfo->codePoint = (ch[0] & 0x0F);
  }
  else if ((ch[0] & 0xF8) == 0xF0)  // 0x10000 ~ 0x1FFFFF
  {
    pInfo->bytes = 4;
    pInfo->codePoint = (ch[0] & 0x07);
  }
  else  // Wrong char return '?' means unkown
  {
    pInfo->bytes = 1;
    pInfo->codePoint = '?';
  }

  for (uint8_t i = 1; i < pInfo->bytes; i++)
  {
    pInfo->codePoint = (pInfo->codePoint << 6) | (ch[i] & 0x3F);
  }
}

// get character font bitmap info
static void getBitMapFontInfo(CHAR_INFO *pInfo)
{
  if (pInfo->codePoint < 9)
  {
    pInfo->pixelWidth = 0;
    pInfo->pixelHeight = 0;
    pInfo->bitMapAddr = 0;
    return;
  }

  for (uint8_t i = 0; i < COUNT(font); i++)
  {
    if (pInfo->codePoint >= font[i].startCodePoint && pInfo->codePoint <= font[i].endCodePoint)
    {
      if (pInfo->codePoint >= 0x20 && pInfo->codePoint <= 0x7E)  // The font size of ASCII is variable
      {
        if (_FONT_H(fontSize) != font[i].pixelHeight || _FONT_W(fontSize) != font[i].pixelWidth)
        {
          continue;
        }
      }

      pInfo->pixelWidth = font[i].pixelWidth;
      pInfo->pixelHeight = font[i].pixelHeight;
      pInfo->bitMapAddr = font[i].bitMapStartAddr + (pInfo->codePoint - font[i].bitMapStartCodePoint) * (font[i].bitMapHeight * font[i].bitMapWidth / 8);
      return;
    }
  }
}

void getCharacterInfo(const uint8_t *ch, CHAR_INFO *pInfo)
{
  pInfo->bytes = 0;

  if (ch == NULL || *ch == 0) return;

  getUTF8EncodeInfo(ch, pInfo);

  getBitMapFontInfo(pInfo);
}

// decode UTF-8 char display bit width
uint16_t GUI_StrPixelWidth_str(const uint8_t * str)
{
  uint16_t i = 0, len = 0;
  CHAR_INFO info;

  if (str == NULL) return 0;
  while (str[i])
  {
    getCharacterInfo(str + i, &info);
    i += info.bytes;
    len += info.pixelWidth;
  }
  return len;
}

// decode UTF-8 char display bit width
uint16_t GUI_StrPixelWidth_label(int16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH];
  if (loadLabelText((uint8_t*)tempstr, index) == false) return 0;
  return GUI_StrPixelWidth_str(tempstr);
}

uint16_t getUTF8Length(const uint8_t *const str)
{
  uint16_t i = 0, len = 0;
  CHAR_INFO info;

  if (str == NULL) return 0;
  while (str[i])
  {
    info.bytes = 0;
    getUTF8EncodeInfo(str + i, &info);
    i += info.bytes;
    len++;
  }
  return len;
}
