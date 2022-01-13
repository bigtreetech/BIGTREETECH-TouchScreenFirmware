#define _GNU_SOURCE

#include "ui_draw.h"
#include "includes.h"
#include <stdio.h>
#include <malloc.h>

#if (THUMBNAIL_PARSER == PARSER_BASE64PNG)
  #include "pngle.h"
  #include "base64.h"
#endif

#ifdef STM32_HAS_FSMC

void lcd_frame_display(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint32_t addr);

#else

void lcd_frame_display(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint32_t addr)
{
  uint16_t x, y;
  uint16_t color = 0;

  LCD_SetWindow(sx, sy, sx + w - 1, sy + h - 1);

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((addr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((addr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(addr & 0xFF);

  for (y = sy; y < sy + h; y++)
  {
    for (x = sx; x < sx + w; x++)
    {
      color = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE) << 8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
      LCD_WR_16BITS_DATA(color);
    }
  }

  W25Qxx_SPI_CS_Set(1);
}

#endif

void lcd_frame_partial_display(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t w, uint16_t h, uint32_t addr)
{
  uint16_t x, y;
  uint16_t color;

  LCD_SetWindow(sx, sy, ex - 1, ey - 1);

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((addr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((addr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(addr & 0xFF);

  for (y = sy; y < ey; y++)
  {
    for (x = sx; x < sx + w; x++)
    {
      color = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE) << 8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);

      if (x < ex)
        LCD_WR_16BITS_DATA(color);
    }
  }

  W25Qxx_SPI_CS_Set(1);
}

uint32_t getBMPsize(uint8_t *w, uint8_t *h, uint32_t address)
{
  uint16_t len = sizeof(uint16_t);

  W25Qxx_ReadBuffer(w, address, len);
  address +=len;
  W25Qxx_ReadBuffer(h, address, len);
  address +=len;

  return address;
}

// draw an image from specific address on flash (sx & sy cordinates for top left of image, w width, h height, addr flash byte address)
void IMAGE_ReadDisplay(uint16_t sx, uint16_t sy, uint32_t address)
{
  uint16_t w, h;

  address = getBMPsize((uint8_t *)&w, (uint8_t *)&h, address);
  lcd_frame_display(sx, sy, w, h, address);
}

void LOGO_ReadDisplay(void)
{
  IMAGE_ReadDisplay(0, 0, LOGO_ADDR);
}

void ICON_PartialReadDisplay(uint16_t sx, uint16_t sy, int16_t width, int16_t height, uint8_t icon, uint16_t isx, uint16_t isy)
{
  uint16_t w, h;
  uint32_t address;

  address = getBMPsize((uint8_t *)&w, (uint8_t *)&h, ICON_ADDR(icon));

  if (width < 0 || (isx + width) > w)
    width = w - isx;

  if (height < 0 || (isy + height) > h)
    height = h - isy;

  lcd_frame_partial_display(sx, sy, sx + width, sy + height, w, h, address + (isy * w * 2) + (isx * 2));
}

void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon)
{
  IMAGE_ReadDisplay(sx, sy, ICON_ADDR(icon));
}

typedef struct
{
  uint16_t sx;
  uint16_t sy;
  uint16_t w;
  uint16_t h;
  uint32_t address;
  GUI_TEXT_MODE pre_mode;
  uint16_t pre_bgColor;
} TEXT_ON_ICON;

TEXT_ON_ICON backgroundIcon;

void ICON_PrepareRead(uint16_t sx, uint16_t sy, uint8_t icon)
{
  backgroundIcon.pre_bgColor = GUI_GetBkColor();
  backgroundIcon.pre_mode = GUI_GetTextMode();
  backgroundIcon.address = getBMPsize((uint8_t *)&backgroundIcon.w, (uint8_t *)&backgroundIcon.h, ICON_ADDR(icon));
  backgroundIcon.sx = sx;
  backgroundIcon.sy = sy;

  GUI_SetTextMode(GUI_TEXTMODE_ON_ICON);
}

void ICON_PrepareReadEnd(void)
{
  GUI_SetBkColor(backgroundIcon.pre_bgColor);
  GUI_SetTextMode(backgroundIcon.pre_mode);
}

uint16_t ICON_ReadPixel(int16_t x, int16_t y)
{
  // // Out of range calls
  // if ((x > backgroundIcon.sx + backgroundIcon.w) || (y > backgroundIcon.sy + backgroundIcon.h))
  //   return 0;
  uint16_t color;
  uint32_t address = backgroundIcon.address + ((x - backgroundIcon.sx) + (y - backgroundIcon.sy) * backgroundIcon.w) * 2;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(address & 0xFF);

  color  = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE) << 8);
  color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);

  W25Qxx_SPI_CS_Set(1);

  return color;
}

uint16_t modelFileReadHalfword(FIL *fp)
{
  uint8_t buf[4];
  uint8_t ascii[4];
  uint8_t *pd = ascii;
  UINT mybr;
  uint8_t rest = 4;

  while (rest > 0)
  {
    f_read(fp, buf, rest, &mybr);

    if (mybr != rest)
      return 0;

    // Check buf for non-valid character i.e. not 0-9, a-f or A-F and skip
    for (uint8_t *ps = buf; ps < buf + rest; )
    {
      char c = *ps++;

      if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        *pd++ = c;
    }

    rest = (ascii + 4) - pd;
  }

  return string_2_uint32(ascii, 2);
}

#if (THUMBNAIL_PARSER == PARSER_RGB565) || (THUMBNAIL_PARSER == PARSER_BASE64PNG)

// Define where to search for dedicated thumbnail comments at max (defaults to first 100kb)
#define BLOCKSIZE_THUMBNAIL_SEARCH (512)
#define MAX_THUMBNAIL_SEARCH_BLOCKS (100 * 2)

// Search for the gcode thumbnail comment signature within the first BLOCKSIZE_THUMBNAIL_SEARCH * MAX_THUMBNAIL_SEARCH_BLOCKS bytes
bool modelFileFind(FIL *fp, char *find)
{
  char search_buf[BLOCKSIZE_THUMBNAIL_SEARCH];
  char *cFind = find;

  dbg_printf("Find: '%s' starting from %d\n", find, f_tell(fp));

  for (int block = 0; block < MAX_THUMBNAIL_SEARCH_BLOCKS; block++)
  {
    UINT len = 0;

    if (f_read(fp, search_buf, BLOCKSIZE_THUMBNAIL_SEARCH, &len) != FR_OK)
      return false;

    if (len == 0)
      return false;

    for (char *cSearch = search_buf; cSearch < search_buf + len; cSearch++)
    {
      if (*cSearch == *cFind)
        cFind++;
      else
        cFind = find;

      if (*cFind == 0)
      {
        // seek to the end of the found string
        f_lseek(fp, f_tell(fp) - (search_buf + len - cSearch) + 1);

        return true;
      }
    }
  }

  return false;
}

#endif

#if (THUMBNAIL_PARSER == PARSER_BASE64PNG)

// Read an unsigned int value from a file
uint32_t modelFileReadValue(FIL *fp)
{
  char current = 0;
  UINT br = 0;
  uint32_t value = 0;

  while (f_read(fp, &current, 1, &br) == FR_OK)
  {
    if (br == 0)
      return 0;

    if (current < '0' || current > '9')
      return value;

    value *= 10;
    value += current - '0';
  }

  return 0;
}

uint32_t modelFileSeekToThumbnailBase64PNG(FIL *fp, uint16_t width, uint16_t height)
{
  uint32_t len = 0;
  char buf[32];

  // Find thumbnail begin marker for the right thumbnail resolution and read the base64 length
  snprintf(buf, sizeof(buf), "; thumbnail begin %hux%hu ", width, height);
  dbg_print("Start search\n");

  if (!modelFileFind(fp, buf))
    return 0;

  dbg_printf("Found signature '%s' at %ld\n", buf, f_tell(fp));
  // Get the base64 length of the encoded PNG thumbnail file
  len = modelFileReadValue(fp);
  dbg_printf("Base64 len=%d\n", len);

  if (len == 0)
    return 0;

  // Seek to the start of the base64 block
  if (!modelFileFind(fp, ";"))
    return 0;

  return len;
}

static b64_decoder_t gcode_thumb_b64;

uint16_t color_alpha_565(const uint8_t r0, const uint8_t g0, const uint8_t b0, const uint8_t r1, const uint8_t g1, const uint8_t b1, const uint8_t alpha)
{
  const uint8_t r = ((255 - alpha) * r0 + alpha * r1) / 255;
  const uint8_t g = ((255 - alpha) * g0 + alpha * g1) / 255;
  const uint8_t b = ((255 - alpha) * b0 + alpha * b1) / 255;

  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void on_draw_png_pixel(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
  GUI_POINT *pos = (GUI_POINT *)pngle_get_user_data(pngle);

  LCD_SetWindow(pos->x + x, pos->y + y, pos->x + x, pos->y + y);
  LCD_WR_16BITS_DATA(color_alpha_565(0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]));
}

/**
 * Thumbnail is a Base64 encoded PNG file.
 * Gcode format:
 * ; thumbnail begin <WIDTH>x<HEIGHT> <BASE64_LEN>
 * <BASE64 encoded PNG>
 * ; thumbnail end
 */
bool model_DirectDisplay_Base64PNG(GUI_POINT pos, FIL *gcodeFile)
{
  uint32_t base64_len;
  char buf[256];

  dbg_printf("Finding BASE64PNG\n");

  // Find thumbnail block with correct picture size
  base64_len = modelFileSeekToThumbnailBase64PNG(gcodeFile, ICON_WIDTH, ICON_HEIGHT);

  if (base64_len == 0)
  {
    dbg_printf("thumbnail for w=%d,h=%d not found.\n", ICON_WIDTH, ICON_HEIGHT);
    return false;
  }

  // Base64 decode on the fly while reading the PNG
  b64_init(&gcode_thumb_b64, gcodeFile, base64_len);

  pngle_t *pngle = pngle_new();

  if (!pngle)
    goto pngle_new_failed;

  pngle_set_draw_callback(pngle, on_draw_png_pixel);
  pngle_set_user_data(pngle, &pos);

  int remain = 0;
  int len;

  while ((len = b64_read(&gcode_thumb_b64, buf, sizeof(buf) - remain)) > 0)
  {
    int fed = pngle_feed(pngle, buf, remain + len);

    if (fed < 0)
    {
      dbg_printf("pngle error: %s\n", pngle_error(pngle));
      goto pngle_failed;
    }

    remain = remain + len - fed;

    if (remain > 0)
      memmove(buf, buf + fed, remain);
  }

  pngle_destroy(pngle);

  return true;

pngle_failed:
  pngle_destroy(pngle);

pngle_new_failed:
  return false;
}

#endif

#if (THUMBNAIL_PARSER == PARSER_BASE64PNG) || (THUMBNAIL_PARSER == PARSER_RGB565)

bool modelFileSeekToThumbnailRGB565(FIL *fp, uint16_t width, uint16_t height)
{
  char buf[39];

  dbg_printf("Finding RGB565 by signature\n");

  // Find thumbnail begin marker for the right thumbnail resolution and read the base64 length
  snprintf(buf, sizeof(buf), "; bigtree thumbnail begin %hux%hu", width, height);

  if (modelFileFind(fp, buf))
  {
    dbg_printf("Found signature '%s' at %ld\n", buf, f_tell(fp));

    // Seek to the start of the RGB565 block
    if (modelFileFind(fp, ";"))
      return true;
  }

  dbg_printf("bigtree thumbnail for w=%d,h=%d not found.\n", ICON_WIDTH, ICON_HEIGHT);
  return false;
}

#endif

/**
 * Thumbnail is a raw RGB565 file encoded as hexstring.
 * Gcode format:
 * ; bigtreetech thumbnail begin <WIDTH>x<HEIGHT>
 * <WIDTHxHEIGHT 16-Bit RGB565 Data as Hexstring>
 * ; bigtreetech thumbnail end
 */
bool model_DirectDisplay_Classic(GUI_POINT pos, FIL *gcodeFile)
{
  // try finding RGB565 thumbnail signature
  #if (THUMBNAIL_PARSER >= PARSER_RGB565)
    // Move the file cursor to the signature location if found
    if (!modelFileSeekToThumbnailRGB565(gcodeFile, ICON_WIDTH, ICON_HEIGHT))
  #endif
  {
    dbg_printf("Finding RGB565 by predefined offset\n");

    // Move the file cursor to the predefined location
    f_lseek(gcodeFile, MODEL_PREVIEW_OFFSET);

    // Check whether the icon size matches
    if (modelFileReadHalfword(gcodeFile) != ICON_WIDTH || modelFileReadHalfword(gcodeFile) != ICON_HEIGHT)
    {
      dbg_printf("RGB565 not found\n");
      return false;
    }
  }

  LCD_SetWindow(pos.x, pos.y, pos.x + ICON_WIDTH - 1, pos.y + ICON_HEIGHT - 1);

  for (uint16_t i = 0; i < ICON_WIDTH * ICON_HEIGHT; i++)
  {
    LCD_WR_16BITS_DATA(modelFileReadHalfword(gcodeFile));
  }
  return true;
}

bool model_DirectDisplay(GUI_POINT pos, char *gcode)
{
  FIL gcodeFile;

  dbg_printf("Opening file: %s\n", gcode);

  if (f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    return false;

  // Try all available options from fastest to slowest
  if (model_DirectDisplay_Classic(pos, &gcodeFile))
    return true;

  #if (THUMBNAIL_PARSER == PARSER_BASE64PNG)
    if (model_DirectDisplay_Base64PNG(pos, &gcodeFile))
      return true;
  #endif

  return false;
}

bool model_DecodeToFlash(char *gcode)
{
  uint32_t addr = ICON_ADDR(ICON_PREVIEW);
  uint16_t bnum;
  uint16_t w = ICON_WIDTH;
  uint16_t h = ICON_HEIGHT;

  uint8_t buf[256];
  FIL gcodeFile;

  if (f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    return false;

  // Move the file cursor to the corresponding resolution area
  f_lseek(&gcodeFile, MODEL_PREVIEW_OFFSET);

  // Check whether the icon size matches
  if (modelFileReadHalfword(&gcodeFile) != w || modelFileReadHalfword(&gcodeFile) != h)
    return false;

  // Move to next line
  f_lseek(&gcodeFile, gcodeFile.fptr + 3);

  for (bnum = 0; bnum < (w * h * 2 + W25QXX_SECTOR_SIZE - 1) / W25QXX_SECTOR_SIZE; bnum++)
  {
    W25Qxx_EraseSector(addr + bnum * W25QXX_SECTOR_SIZE);
  }

  bnum = 0;

  memcpy(buf, (uint8_t *)&w, sizeof(uint16_t));
  bnum += sizeof(uint16_t);
  memcpy(buf + bnum, (uint8_t *)&h, sizeof(uint16_t));
  bnum += sizeof(uint16_t);

  for (uint16_t y = 0; y < h; y++)
  {
    for (uint16_t x = 0; x < w; x++)
    {
      uint16_t color = modelFileReadHalfword(&gcodeFile);

      buf[bnum++] = (uint8_t)(color >> 8);
      buf[bnum++] = (uint8_t)(color & 0xFF);

      if (bnum == 256)
      {
        W25Qxx_WritePage(buf, addr, 256);
        addr += 256;
        bnum = 0;
      }
    }

    // Move to next line
    f_lseek(&gcodeFile, gcodeFile.fptr + 3);
  }

  W25Qxx_WritePage(buf, addr, bnum);

  return true;
}

void SMALLICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon)
{
  lcd_frame_display(sx, sy, SMALLICON_WIDTH, SMALLICON_HEIGHT, SMALL_ICON_ADDR(icon));
}

void ICON_PressedDisplay(uint16_t sx, uint16_t sy, uint8_t icon)
{
  uint16_t mode = 0x0FF0;
  uint16_t x, y;
  uint16_t w, h;
  uint16_t color = 0;
  uint32_t address = getBMPsize((uint8_t *)&w, (uint8_t *)&h, ICON_ADDR(icon));

  LCD_SetWindow(sx, sy, sx + w - 1, sy + h - 1);

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(address & 0xFF);

  for (y = sy; y < sy + w; y++)
  {
    for (x = sx; x < sx + h; x++)
    {
      color  = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE) << 8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
      LCD_WR_16BITS_DATA(color & mode);
    }
  }

  W25Qxx_SPI_CS_Set(1);
}
