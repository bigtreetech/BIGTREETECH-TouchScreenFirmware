#include "boot.h"
#include "includes.h"

#define PADDING 10
const GUI_RECT labelUpdateRect = {0,     BYTE_HEIGHT + PADDING,                 LCD_WIDTH, (BYTE_HEIGHT*2) + PADDING};
const GUI_RECT iconUpdateRect  = {0, (BYTE_HEIGHT*2) + PADDING*2,               LCD_WIDTH, (BYTE_HEIGHT*2) + PADDING*3 + ICON_HEIGHT};
const GUI_RECT statUpdateRect  = {0, (BYTE_HEIGHT*3) + PADDING*3 + ICON_HEIGHT, LCD_WIDTH, (BYTE_HEIGHT*4) + PADDING*3 + ICON_HEIGHT};
const GUI_RECT labelFailedRect = {0, (BYTE_HEIGHT*4) + PADDING*4 + ICON_HEIGHT, LCD_WIDTH, (BYTE_HEIGHT*5) + PADDING*4 + ICON_HEIGHT};

GUI_POINT bmp_size;

// This List is Auto-Generated. Please add new icons in icon_list.inc only
const char * const  iconBmpName[] = {
  #define X_ICON(NAME) #NAME ,
    #include "icon_list.inc"
  #undef X_ICON
  // add new icons in icon_list.inc only
};

// This List is Auto-Generated. Please add new icons in small_icon_list.inc only
const char * const  smallIconBmpName[] = {
  #define X_SMALLICON(NAME) #NAME ,
    #include "small_icon_list.inc"
  #undef X_SMALLICON
  // add new icons in small_icon_list.inc only
};

BMPUPDATE_STAT bmpDecode(char *bmp, uint32_t addr)
{
  FIL bmpFile;
  char magic[2];
  uint16_t w, h;
  int bytePerLine;
  short bpp;
  int offset;
  uint8_t buf[256];
  uint8_t lcdcolor[4];
  uint16_t bnum = 0;
  UINT mybr;
  GUI_COLOR pix;

  if (f_open(&bmpFile, bmp, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    return BMP_NOTFOUND;

  f_read(&bmpFile, magic, 2, &mybr);

  if (memcmp(magic, "BM", 2))
  {
    f_close(&bmpFile);
    return BMP_INVALIDFILE;
  }

  f_lseek(&bmpFile, 10);
  f_read(&bmpFile, &offset, sizeof(int), &mybr);
  f_lseek(&bmpFile, 18);
  f_read(&bmpFile, &w, sizeof(int), &mybr);
  f_read(&bmpFile, &h, sizeof(int), &mybr);
  f_lseek(&bmpFile, 28);
  f_read(&bmpFile, &bpp, sizeof(short), &mybr);

  if (bpp < 24)
  {
    f_close(&bmpFile);
    return BMP_NOT24BIT;
  }

  bmp_size.x = w;
  bmp_size.y = h;
  bpp >>= 3;
  bytePerLine = w * bpp;

  if (bytePerLine % 4 != 0)  // bmp
    bytePerLine = (bytePerLine / 4 + 1) * 4;

  for (bnum = 0; bnum < (w * h * 2 + W25QXX_SECTOR_SIZE - 1) / W25QXX_SECTOR_SIZE; bnum++)
  {
    W25Qxx_EraseSector(addr + bnum * W25QXX_SECTOR_SIZE);
  }

  bnum = 0;
  // store size of BMP
  memcpy(buf, (uint8_t *)&w, sizeof(uint16_t));
  bnum += sizeof(uint16_t);
  memcpy(buf + bnum, (uint8_t *)&h, sizeof(uint16_t));
  bnum += sizeof(uint16_t);

  for (int j = 0; j < h; j++)
  {
    f_lseek(&bmpFile, offset + (h - j - 1) * bytePerLine);

    for (int i = 0; i < w; i++)
    {
      f_read(&bmpFile, (char *)&lcdcolor, bpp, &mybr);

      pix.RGB.r = lcdcolor[2] >> 3;
      pix.RGB.g = lcdcolor[1] >> 2;
      pix.RGB.b = lcdcolor[0] >> 3;

      // GUI_DrawPixel(iconUpdateRect.x0 + i,iconUpdateRect.y0 + j,pix.color);

      buf[bnum++] = (uint8_t)(pix.color >> 8);
      buf[bnum++] = (uint8_t)(pix.color & 0xFF);

      if (bnum == 256)
      {
        W25Qxx_WritePage(buf, addr, 256);
        addr += 256;
        bnum = 0;
      }
    }
  }

  W25Qxx_WritePage(buf,addr,bnum);
  addr+=bnum;
  f_close(&bmpFile);
  return BMP_SUCCESS;
}

bool updateIcon(void)
{
  uint16_t found = 0;
  uint16_t notfound = 0;
  char nowBmp[64];
  char tempstr[50];
  char * bootLogoPath = BMP_ROOT_DIR "/Logo.bmp";
  char * infoboxPath = BMP_ROOT_DIR "/InfoBox.bmp";
  BMPUPDATE_STAT bmpState;

  GUI_Clear(infoSettings.bg_color);
  GUI_DispString(5, PADDING, (uint8_t *)"Updating Logo");
  GUI_ClearPrect(&iconUpdateRect);

  bmpState = bmpDecode(bootLogoPath, LOGO_ADDR);

  if (bmpState == BMP_SUCCESS)
  {
    LOGO_ReadDisplay();
    found++;
  }
  else
  {
    notfound++;
    dispIconFail((uint8_t *)(bootLogoPath), bmpState);
  }

  GUI_Clear(infoSettings.bg_color);
  GUI_DispString(5, PADDING, (uint8_t *)"Updating Icons");

  for (int i = 0; i < COUNT(iconBmpName); i++)
  {
    GUI_POINT last_size = bmp_size;
    sprintf(nowBmp, BMP_ROOT_DIR "/%s.bmp", iconBmpName[i]);
    GUI_ClearPrect(&labelUpdateRect);
    GUI_DispString(labelUpdateRect.x0, labelUpdateRect.y0, (uint8_t *)nowBmp);

    bmpState = bmpDecode(nowBmp, ICON_ADDR(i));

    if (bmpState == BMP_SUCCESS)
    {  // display bmp update success
      found++;
      GUI_ClearRect(iconUpdateRect.x0, iconUpdateRect.y0, iconUpdateRect.x0 + last_size.x, iconUpdateRect.y0 + last_size.y);
      IMAGE_ReadDisplay(iconUpdateRect.x0, iconUpdateRect.y0, ICON_ADDR(i));
    }
    else
    {  // display bmp update fail
      notfound++;
      GUI_ClearRect(iconUpdateRect.x0, iconUpdateRect.y0, iconUpdateRect.x0 + last_size.x, iconUpdateRect.y0 + last_size.y);
      dispIconFail((uint8_t *)nowBmp, bmpState);
    }
    // Display icon update progress
    sprintf(tempstr, "Updated: %d | Not Updated: %d", found, notfound);
    GUI_DispString(statUpdateRect.x0, statUpdateRect.y0, (uint8_t *)tempstr);
  }
  /*
  for (int i = 0; i < COUNT(smallIconBmpName); i++)
  {
    sprintf(nowBmp, BMP_ROOT_DIR "/%s_small.bmp", smallIconBmpName[i]);
    GUI_ClearPrect(&labelUpdateRect);
    GUI_DispString(labelUpdateRect.x0, labelUpdateRect.y0, (uint8_t *)nowBmp);

    processIcon(nowBmp, SMALL_ICON_ADDR(i));
  }
  */
  bmpState = bmpDecode(infoboxPath, INFOBOX_ADDR);

  if (bmpState == BMP_SUCCESS)
  {
    IMAGE_ReadDisplay(iconUpdateRect.x0, iconUpdateRect.y0, INFOBOX_ADDR);
    found++;
  }
  else
  {
    notfound++;
    dispIconFail((uint8_t *)(infoboxPath), bmpState);
  }

  if (notfound == 0)
    return true;
  else
    return false;
}

void dispIconFail(uint8_t *lbl, BMPUPDATE_STAT bmpState)
{
  char * stat_txt;
  char error_txt[30];

  GUI_SetColor(infoSettings.reminder_color);
  GUI_ClearPrect(&labelFailedRect);
  GUI_DispString(labelFailedRect.x0, labelFailedRect.y0, lbl);

  switch (bmpState)
  {
    case BMP_INVALIDFILE:
      stat_txt = "BMP file not valid ";
      break;
    case BMP_NOT24BIT:
      stat_txt = "Format is not 24Bit";
      break;
    case BMP_NOTFOUND:
    default:
      stat_txt = "BMP file not found ";
      break;
  }

  sprintf(error_txt, "Error: %s", stat_txt);
  GUI_DispString(labelFailedRect.x0, labelFailedRect.y0 + BYTE_HEIGHT + 2, (uint8_t*)error_txt);
  GUI_RestoreColorDefault();
  Delay_ms(1000);  // give some time to the user to read failed icon name.
}

bool updateFont(char *font, uint32_t addr)
{
  uint8_t progress = 0;
  UINT rnum = 0;
  uint32_t offset = 0;
  char buffer[128];
  FIL myfp;
  uint8_t * tempbuf = NULL;

  if (f_open(&myfp, font, FA_OPEN_EXISTING|FA_READ) != FR_OK)
    return false;

  tempbuf = malloc(W25QXX_SECTOR_SIZE);

  if (tempbuf == NULL)
    return false;

  GUI_Clear(infoSettings.bg_color);
  sprintf((void *)buffer,"%s Size: %dKB",font, (uint32_t)f_size(&myfp) >> 10);
  GUI_DispString(0, 100, (uint8_t*)buffer);
  GUI_DispString(0, 140, (uint8_t*)"Updating:   %");

  while (!f_eof(&myfp))
  {
    if (f_read(&myfp, tempbuf, W25QXX_SECTOR_SIZE, &rnum) != FR_OK) break;

    W25Qxx_EraseSector(addr + offset);
    W25Qxx_WriteBuffer(tempbuf, addr + offset, W25QXX_SECTOR_SIZE);
    offset += rnum;

    if (progress != offset * 100 / f_size(&myfp))
    {
      progress = offset * 100 / f_size(&myfp);
      GUI_DispDec(0 + BYTE_WIDTH * 9, 140, progress, 3, RIGHT);
    }

    if (rnum !=W25QXX_SECTOR_SIZE) break;
  }

  f_close(&myfp);
  free(tempbuf);
  return true;
}

static inline void scanResetDir(void)
{
  char * renamedReset = TFT_RESET_FILE ".DONE";

  if (f_file_exists(TFT_RESET_FILE))
  {
    if (f_file_exists(renamedReset))
    {
      f_unlink(renamedReset);
    }

    initSettings();
    LCD_RefreshDirection(infoSettings.rotated_ui);
    TSC_Calibration();
    storePara();
    f_rename(TFT_RESET_FILE, renamedReset);
  }
}

static inline void scanRenameUpdate(void)
{
  char * renamedDIR = ROOT_DIR ".CUR";
  char * firmwareFile = FIRMWARE_NAME ".bin";
  char * firmwareFileShort = FIRMWARE_NAME_SHORT ".NEW";
  char * renamedFirmwareFile = FIRMWARE_NAME ".CUR";
  char * renamedConfigFile = CONFIG_FILE_PATH ".CUR";

  if (f_file_exists(ADMIN_MODE_FILE)) return;  // admin mode, need not rename

  if (f_dir_exists(ROOT_DIR))
  { // ROOT_DIR exists
    if (f_dir_exists(renamedDIR))
    { // old ROOT_DIR also exists
      GUI_Clear(infoSettings.bg_color);
      // It will take some time to delete the old ROOT_DIR, so display "Deleting" on the screen to tell user.
      GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t *)"Deleting old ROOT_DIR...");
      f_remove_full_dir(renamedDIR);
    }
    f_rename(ROOT_DIR, renamedDIR);
  }

  if (f_file_exists(firmwareFile))
  { // firmware exists
    if (f_file_exists(renamedFirmwareFile))
    { // old firmware also exists
      f_unlink(renamedFirmwareFile);
    }
    f_rename(firmwareFile, renamedFirmwareFile);
  }

  if (f_file_exists(firmwareFileShort))
  { // firmware exists
    if (f_file_exists(firmwareFile))
    { // long firmware also exists ? should not be
      f_unlink(firmwareFile);
    }
    f_rename(firmwareFileShort, firmwareFile);
  }

  if (f_file_exists(CONFIG_FILE_PATH))
  { // config exists
    if (f_file_exists(renamedConfigFile))
    { // old config also exists
      f_unlink(renamedConfigFile);
    }
    f_rename(CONFIG_FILE_PATH, renamedConfigFile);
  }

}

static inline void saveflashSign(uint8_t* buf, uint32_t size)
{
  W25Qxx_EraseSector(FLASH_SIGN_ADDR);
  Delay_ms(100);  // give time for spi flash to settle
  W25Qxx_WriteBuffer(buf, FLASH_SIGN_ADDR, size);
}

void scanUpdates(void)
{
  if (mountSDCard())
  {
    bool flash_sign_updated = false;
    uint32_t saved_flash_sign[sign_count];
    W25Qxx_ReadBuffer((uint8_t*)&saved_flash_sign, FLASH_SIGN_ADDR, sizeof(saved_flash_sign));

    if (f_dir_exists(FONT_ROOT_DIR))
    {
      if (updateFont(FONT_ROOT_DIR "/byte_ascii.fon", BYTE_ASCII_ADDR) &&
          updateFont(FONT_ROOT_DIR "/word_unicode.fon", WORD_UNICODE) &&
          updateFont(FONT_ROOT_DIR "/large_byte_ascii.fon", LARGE_FONT_ADDR) &&
          updateFont(FONT_ROOT_DIR "/8x16_byte_ascii.fon", _8X16_FONT_ADDR) &&
          (saved_flash_sign[font_sign] != FONT_CHECK_SIGN))
      {
        saved_flash_sign[font_sign] = FONT_CHECK_SIGN;
        flash_sign_updated = true;
      }
    }

    if (f_dir_exists(BMP_ROOT_DIR))
    {
      if (updateIcon() && (saved_flash_sign[icon_sign] != ICON_CHECK_SIGN))
      {
        saved_flash_sign[icon_sign] = ICON_CHECK_SIGN;
        flash_sign_updated = true;
      }
    }

    if (getConfigFromFile() && (saved_flash_sign[config_sign] != CONFIG_CHECK_SIGN))
    {
      saved_flash_sign[config_sign] = CONFIG_CHECK_SIGN;
      flash_sign_updated = true;
    }

    if (getLangFromFile() && (saved_flash_sign[lang_sign] != LANGUAGE_CHECK_SIGN))
    {
      saved_flash_sign[lang_sign] = LANGUAGE_CHECK_SIGN;
      flash_sign_updated = true;
    }

    scanRenameUpdate();
    scanResetDir();

    if (flash_sign_updated)
    {
      saveflashSign((uint8_t*)saved_flash_sign, sizeof(saved_flash_sign));
    }
  }
}
