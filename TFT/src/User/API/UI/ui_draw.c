#include "ui_draw.h"
#include "includes.h"

#ifdef STM32_HAS_FSMC

void lcd_frame_display(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint32_t addr);

#else

void lcd_frame_display(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint32_t addr)
{
  uint16_t x, y;
  uint16_t color = 0;
  uint32_t address = addr;

  LCD_SetWindow(sx, sy, sx + w - 1, sy + h - 1);

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(address & 0xFF);

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

uint32_t getBMPsize(uint8_t * w, uint8_t * h, uint32_t address)
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

void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon)
{
  IMAGE_ReadDisplay(sx, sy, ICON_ADDR(icon));
}

uint16_t modelFileReadHalfword(FIL * fp)
{
  uint8_t ascii[4];
  UINT mybr;
  f_read(fp, ascii, 4 ,&mybr);
  if (mybr != 4) return 0;
    return string_2_uint32(ascii, 2);
}

bool model_DirectDisplay(GUI_POINT pos, char *gcode)
{
  FIL gcodeFile;

  if (f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK) return false;
  // Move the file cursor to the corresponding resolution area
  f_lseek(&gcodeFile, MODEL_PREVIEW_OFFSET);
  // Check whether the icon size matches
  if (modelFileReadHalfword(&gcodeFile) != ICON_WIDTH || modelFileReadHalfword(&gcodeFile) != ICON_HEIGHT) return false;
  // Move to next line
  f_lseek(&gcodeFile, gcodeFile.fptr + 3);

  LCD_SetWindow(pos.x, pos.y, pos.x + ICON_WIDTH - 1, pos.y + ICON_HEIGHT - 1);
  for (uint16_t y = 0; y < ICON_HEIGHT; y++)
  {
    for (uint16_t x = 0; x < ICON_WIDTH; x++)
    {
      LCD_WR_16BITS_DATA(modelFileReadHalfword(&gcodeFile));
    }
    // Move to next line
    f_lseek(&gcodeFile, gcodeFile.fptr + 3);
  }

  return true;
}

bool model_DecodeToFlash(char *gcode)
{
  uint32_t addr = ICON_ADDR(ICON_PREVIEW);
  uint16_t bnum;
  uint16_t w = ICON_WIDTH;
  uint16_t h = ICON_HEIGHT;

  uint8_t buf[256];
  FIL gcodeFile;

  if (f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK) return false;
  // Move the file cursor to the corresponding resolution area
  f_lseek(&gcodeFile, MODEL_PREVIEW_OFFSET);
  // Check whether the icon size matches
  if (modelFileReadHalfword(&gcodeFile) != w || modelFileReadHalfword(&gcodeFile) != h) return false;
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
