#define _GNU_SOURCE

#include "ui_draw.h"
#include "includes.h"
#include <stdio.h>
#include <malloc.h>

#if (THUMBNAIL_PARSER == PARSER_BASE64PNG)
  #include "pngle.h"
  #include "base64.h"
#endif

#define COLOR_BYTE_SIZE sizeof(uint16_t)  // RGB565 color byte is equal to uint16_t

#ifdef STM32_HAS_FSMC

// defined and implemented on lcd_dma.h / .c
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

void lcd_buffer_display(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t * buf, GUI_RECT * limit)
{
  uint16_t wl = w - limit->x1;
  uint16_t hl = h - limit->y1;
  uint16_t x;
  uint16_t y;

  LCD_SetWindow(sx + limit->x0, sy + limit->y0, sx + wl - 1, sy + hl - 1);

  for (y = limit->y0; y < hl; y++)
  {
    for (x = limit->x0; x < wl; x++)
    {
      LCD_WR_16BITS_DATA(buf[(y * w) + x]);
    }
  }
}

void getBMPsize(BMP_INFO * bmp)
{
  if (!bmp->address && bmp->index < ICON_NULL)
    bmp->address = ICON_ADDR(bmp->index);

  W25Qxx_ReadBuffer((uint8_t *)&bmp->width, bmp->address, COLOR_BYTE_SIZE);
  bmp->address += COLOR_BYTE_SIZE;
  W25Qxx_ReadBuffer((uint8_t *)&bmp->height, bmp->address, COLOR_BYTE_SIZE);
  bmp->address += COLOR_BYTE_SIZE;
}

static inline void bmpToBuffer(uint16_t * buf, GUI_POINT startPoint, GUI_POINT endPoint, BMP_INFO * iconInfo)
{
  uint16_t frameLines = (endPoint.y - startPoint.y);  // total lines in frame
  uint16_t blockLines = (endPoint.y >= iconInfo->height) ? (iconInfo->height - startPoint.y) : frameLines;  // total drawable lines

  uint16_t frameWidth = (endPoint.x - startPoint.x);  // total frame width
  uint16_t blockWidth = (endPoint.x >= iconInfo->width) ? (iconInfo->width - startPoint.x) : frameWidth;  // total drawable width
  uint16_t bgWidth = frameWidth - blockWidth;  // total empty width to be filled with bg color
  uint16_t color = 0;

  // move address to block starting point
  iconInfo->address += ((iconInfo->width * startPoint.y) + startPoint.x) * COLOR_BYTE_SIZE;

  for (uint16_t y = 0; y < blockLines; y++)
  {
    W25Qxx_SPI_CS_Set(0);
    W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
    W25Qxx_SPI_Read_Write_Byte((iconInfo->address & 0xFF0000) >> 16);
    W25Qxx_SPI_Read_Write_Byte((iconInfo->address & 0xFF00) >> 8);
    W25Qxx_SPI_Read_Write_Byte(iconInfo->address & 0xFF);

    for (uint8_t x = 0; x < blockWidth; x++)
    {
      color = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE) << 8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
      buf[(y * frameWidth) + x] = color;
    }

    W25Qxx_SPI_CS_Set(1);

    if (bgWidth)
    {
      for (uint8_t x = blockWidth; x < frameWidth; x++)
      {
        buf[(y * frameWidth) + x] = infoSettings.bg_color;
      }
    }

    iconInfo->address += iconInfo->width * COLOR_BYTE_SIZE;
  }

  // fill empty frame lines with background color
  for (uint16_t i = (blockLines * frameWidth); i < (frameLines * frameWidth); i++)
  {
    buf[i] = infoSettings.bg_color;
  }
}

// draw an image from specific address on flash (sx & sy cordinates for top left of image, w width, h height, addr flash byte address)
void IMAGE_ReadDisplay(uint16_t sx, uint16_t sy, uint32_t address)
{
  BMP_INFO bmpInfo = {.index = ICON_NULL, .address = address};

  getBMPsize(&bmpInfo);
  lcd_frame_display(sx, sy, bmpInfo.width, bmpInfo.height, bmpInfo.address);
}

void LOGO_ReadDisplay(void)
{
  IMAGE_ReadDisplay(0, 0, LOGO_ADDR);
}

void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon)
{
  IMAGE_ReadDisplay(sx, sy, ICON_ADDR(icon));
}

// load the selected area of bmp icon from flash to buffer
void ICON_ReadBuffer(uint16_t * buf, uint16_t x, uint16_t y, int16_t w, int16_t h, uint16_t icon)
{
  BMP_INFO iconInfo = {.index = icon, .address = 0};
  GUI_POINT startPoint = {.x = x, .y = y};
  GUI_POINT endPoint = {.x = x + w, .y = y + h};

  getBMPsize(&iconInfo);
  bmpToBuffer(buf, startPoint, endPoint, &iconInfo);
}

uint16_t ICON_ReadPixel(uint32_t address, uint16_t w, uint16_t h, int16_t x, int16_t y)
{
  // out of range calls
  if (x > w || y > h)
    return infoSettings.bg_color;

  address += ((w * y) + x) * COLOR_BYTE_SIZE;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(address & 0xFF);

  uint16_t color;

  color = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE) << 8);
  color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);

  W25Qxx_SPI_CS_Set(1);

  return color;
}

static uint16_t modelFileReadHalfword(FIL * fp)
{
  uint8_t buf[4];
  uint8_t ascii[4];
  uint8_t * pd = ascii;
  UINT mybr;
  uint8_t rest = 4;

  while (rest > 0)
  {
    f_read(fp, buf, rest, &mybr);

    if (mybr != rest)
      return 0;

    // check buf for non-valid character i.e. not 0-9, a-f or A-F and skip
    for (uint8_t * ps = buf; ps < buf + rest; )
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

// define where to search for dedicated thumbnail comments at max (defaults to first 100kb)
#define BLOCKSIZE_THUMBNAIL_SEARCH (512)
#define MAX_THUMBNAIL_SEARCH_BLOCKS (100 * 2)

// search for the gcode thumbnail comment signature within the first BLOCKSIZE_THUMBNAIL_SEARCH * MAX_THUMBNAIL_SEARCH_BLOCKS bytes
static bool modelFileFind(FIL * fp, char * find)
{
  char search_buf[BLOCKSIZE_THUMBNAIL_SEARCH];
  char * cFind = find;

  dbg_printf("Find: '%s' starting from %d\n", find, f_tell(fp));

  for (int block = 0; block < MAX_THUMBNAIL_SEARCH_BLOCKS; block++)
  {
    UINT len = 0;

    if (f_read(fp, search_buf, BLOCKSIZE_THUMBNAIL_SEARCH, &len) != FR_OK)
      return false;

    if (len == 0)
      return false;

    for (char * cSearch = search_buf; cSearch < search_buf + len; cSearch++)
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

// read an unsigned int value from a file
static inline uint32_t modelFileReadValue(FIL * fp)
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

static inline uint32_t modelFileSeekToThumbnailBase64PNG(FIL * fp, uint16_t width, uint16_t height)
{
  uint32_t len = 0;
  char buf[32];

  // find thumbnail begin marker for the right thumbnail resolution and read the base64 length
  snprintf(buf, sizeof(buf), "; thumbnail begin %hux%hu ", width, height);
  dbg_print("Start search\n");

  // seek to the beginning of the file as the file pointer was moved during the RGB565 thumbnail search
  f_lseek(fp, 0);

  if (!modelFileFind(fp, buf))
    return 0;

  dbg_printf("Found signature '%s' at %ld\n", buf, f_tell(fp));
  // get the base64 length of the encoded PNG thumbnail file
  len = modelFileReadValue(fp);
  dbg_printf("Base64 len=%d\n", len);

  if (len == 0)
    return 0;

  // seek to the start of the base64 block
  if (!modelFileFind(fp, ";"))
    return 0;

  return len;
}

static b64_decoder_t gcode_thumb_b64;

static uint16_t color_alpha_565(const uint8_t r0, const uint8_t g0, const uint8_t b0, const uint8_t r1, const uint8_t g1, const uint8_t b1, const uint8_t alpha)
{
  const uint8_t r = ((255 - alpha) * r0 + alpha * r1) / 255;
  const uint8_t g = ((255 - alpha) * g0 + alpha * g1) / 255;
  const uint8_t b = ((255 - alpha) * b0 + alpha * b1) / 255;

  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

static inline void on_draw_png_pixel(pngle_t * pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
  GUI_POINT * pos = (GUI_POINT *)pngle_get_user_data(pngle);

  LCD_SetWindow(pos->x + x, pos->y + y, pos->x + x, pos->y + y);
  LCD_WR_16BITS_DATA(color_alpha_565(0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]));
}

typedef struct
{
    uint16_t bnum;
    uint32_t addr;
    uint8_t buf[256];
} W25Qxx_DECODE;

static void on_decode_png_pixel(pngle_t * pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
  W25Qxx_DECODE * session = (W25Qxx_DECODE *) pngle_get_user_data(pngle);

  uint16_t color = color_alpha_565(0, 0, 0, rgba[0], rgba[1], rgba[2], rgba[3]);

  session->buf[session->bnum++] = (uint8_t) (color >> 8);
  session->buf[session->bnum++] = (uint8_t) (color & 0xFF);

  if (session->bnum == 256)
  {
    W25Qxx_WritePage(session->buf, session->addr, 256);
    session->addr += 256;
    session->bnum = 0;
  }
}

static inline void on_decode_png_pixel_done(pngle_t * pngle)
{
  W25Qxx_DECODE * session = (W25Qxx_DECODE *) pngle_get_user_data(pngle);
  W25Qxx_WritePage(session->buf, session->addr, session->bnum);
}

/**
 * Thumbnail is a Base64 encoded PNG file.
 * Gcode format:
 * ; thumbnail begin <WIDTH>x<HEIGHT> <BASE64_LEN>
 * <BASE64 encoded PNG>
 * ; thumbnail end
 */
static bool model_Process_Base64PNG(FIL * gcodeFile, void * user_data, pngle_draw_callback_t draw_callback,
                                    pngle_done_callback_t done_callback)
{
  uint32_t base64_len;
  char buf[256];

  dbg_printf("Finding BASE64PNG\n");

  // find thumbnail block with correct picture size
  base64_len = modelFileSeekToThumbnailBase64PNG(gcodeFile, ICON_WIDTH, ICON_HEIGHT);

  if (base64_len == 0)
  {
    dbg_printf("thumbnail for w=%d,h=%d not found.\n", ICON_WIDTH, ICON_HEIGHT);

    return false;
  }

  // Base64 decode on the fly while reading the PNG
  b64_init(&gcode_thumb_b64, gcodeFile, base64_len);

  pngle_t * pngle = pngle_new();

  if (!pngle)
    goto pngle_new_failed;

  pngle_set_draw_callback(pngle, draw_callback);

  if (done_callback)
    pngle_set_done_callback(pngle, done_callback);

  pngle_set_user_data(pngle, user_data);

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

static inline bool model_DirectDisplay_Base64PNG(GUI_POINT pos, FIL * gcodeFile)
{
  return model_Process_Base64PNG(gcodeFile, &pos, on_draw_png_pixel, NULL);
}

static inline bool model_DecodeToFlash_Base64PNG(FIL * gcodeFile, uint32_t addr)
{
  uint16_t w = ICON_WIDTH;
  uint16_t h = ICON_HEIGHT;

  W25Qxx_DECODE session = {0, addr, {}};

  memcpy(session.buf, (uint8_t *)&w, sizeof(uint16_t));
  session.bnum += sizeof(uint16_t);
  memcpy(session.buf + session.bnum, (uint8_t *)&h, sizeof(uint16_t));
  session.bnum += sizeof(uint16_t);

  return model_Process_Base64PNG(gcodeFile, &session, on_decode_png_pixel, on_decode_png_pixel_done);
}

#endif

#if (THUMBNAIL_PARSER == PARSER_BASE64PNG) || (THUMBNAIL_PARSER == PARSER_RGB565)

static inline bool modelFileSeekToThumbnailRGB565(FIL * fp, uint16_t width, uint16_t height)
{
  char buf[39];

  dbg_printf("Finding RGB565 by signature\n");

  // find thumbnail begin marker for the right thumbnail resolution and read the base64 length
  snprintf(buf, sizeof(buf), "; bigtree thumbnail begin %hux%hu", width, height);

  if (modelFileFind(fp, buf))
  {
    dbg_printf("Found signature '%s' at %ld\n", buf, f_tell(fp));

    // seek to the start of the RGB565 block
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
static inline bool model_DirectDisplay_Classic(GUI_POINT pos, FIL * gcodeFile)
{
  // try finding RGB565 thumbnail signature
  #if (THUMBNAIL_PARSER >= PARSER_RGB565)
    // move the file cursor to the signature location if found
    if (!modelFileSeekToThumbnailRGB565(gcodeFile, ICON_WIDTH, ICON_HEIGHT))
  #endif
  {
    dbg_printf("Finding RGB565 by predefined offset\n");

    // move the file cursor to the predefined location
    f_lseek(gcodeFile, MODEL_PREVIEW_OFFSET);

    // check whether the icon size matches
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

bool model_DirectDisplay(GUI_POINT pos, char * gcode)
{
  FIL gcodeFile;

  dbg_printf("Opening file: %s\n", gcode);

  if (f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    return false;

  // try all available options from fastest to slowest
  if (model_DirectDisplay_Classic(pos, &gcodeFile))
    return true;

  #if (THUMBNAIL_PARSER == PARSER_BASE64PNG)
    if (model_DirectDisplay_Base64PNG(pos, &gcodeFile))
      return true;
  #endif

  return false;
}

static inline bool model_DecodeToFlash_Classic(FIL * gcodeFile, uint32_t addr)
{
  uint16_t bnum = 0;
  uint16_t w = ICON_WIDTH;
  uint16_t h = ICON_HEIGHT;
  uint8_t buf[256];

  // move the file cursor to the corresponding resolution area
  f_lseek(gcodeFile, MODEL_PREVIEW_OFFSET);

  // check whether the icon size matches
  if (modelFileReadHalfword(gcodeFile) != w || modelFileReadHalfword(gcodeFile) != h)
    return false;

  // move to next line
  f_lseek(gcodeFile, gcodeFile->fptr + 3);

  memcpy(buf, (uint8_t *) &w, sizeof(uint16_t));
  bnum += sizeof(uint16_t);
  memcpy(buf + bnum, (uint8_t *) &h, sizeof(uint16_t));
  bnum += sizeof(uint16_t);

  for (uint16_t y = 0; y < h; y++)
  {
    for (uint16_t x = 0; x < w; x++)
    {
      uint16_t color = modelFileReadHalfword(gcodeFile);

      buf[bnum++] = (uint8_t) (color >> 8);
      buf[bnum++] = (uint8_t) (color & 0xFF);

      if (bnum == 256)
      {
        W25Qxx_WritePage(buf, addr, 256);
        addr += 256;
        bnum = 0;
      }
    }

    // move to next line
    f_lseek(gcodeFile, gcodeFile->fptr + 3);
  }

  W25Qxx_WritePage(buf, addr, bnum);

  return true;
}

bool model_DecodeToFlash(char * gcode)
{
  FIL gcodeFile;

  if (f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    return false;

  uint32_t addr = ICON_ADDR(ICON_PREVIEW);

  for (uint16_t bnum = 0;
       bnum < (ICON_WIDTH * ICON_HEIGHT * 2 + W25QXX_SECTOR_SIZE - 1) / W25QXX_SECTOR_SIZE; bnum++)
  {
    W25Qxx_EraseSector(addr + bnum * W25QXX_SECTOR_SIZE);
  }

  if (model_DecodeToFlash_Classic(&gcodeFile, addr))
    return true;

  #if (THUMBNAIL_PARSER == PARSER_BASE64PNG)
    if (model_DecodeToFlash_Base64PNG(&gcodeFile, addr))
      return true;
  #endif

  return false;
}

void SMALLICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon)
{
  lcd_frame_display(sx, sy, SMALLICON_WIDTH, SMALLICON_HEIGHT, SMALL_ICON_ADDR(icon));
}

void ICON_PressedDisplay(uint16_t sx, uint16_t sy, uint8_t icon)
{
  uint16_t mode = 0x0FF0;
  uint16_t x, y;
  uint16_t color = 0;
  BMP_INFO bmpInfo = {.index = icon, .address = 0};

  getBMPsize(&bmpInfo);

  LCD_SetWindow(sx, sy, sx + bmpInfo.width - 1, sy + bmpInfo.height - 1);

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((bmpInfo.address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((bmpInfo.address & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(bmpInfo.address & 0xFF);

  for (y = sy; y < sy + bmpInfo.width; y++)
  {
    for (x = sx; x < sx + bmpInfo.height; x++)
    {
      color  = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE) << 8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
      LCD_WR_16BITS_DATA(color & mode);
    }
  }

  W25Qxx_SPI_CS_Set(1);
}
