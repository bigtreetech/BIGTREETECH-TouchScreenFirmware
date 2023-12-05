#include "boot.h"
#include "includes.h"

#define STR_PATH_JOIN                         "%s/%s"
#define GET_FULL_PATH(buf, rootDir, filepath) sprintf(buf, STR_PATH_JOIN, rootDir, filepath)

#define PADDING 10
const GUI_RECT labelUpdateRect = {0,     BYTE_HEIGHT + PADDING,                 LCD_WIDTH, (BYTE_HEIGHT*2) + PADDING};
const GUI_RECT iconUpdateRect  = {0, (BYTE_HEIGHT*2) + PADDING*2,               LCD_WIDTH, (BYTE_HEIGHT*2) + PADDING*3 + ICON_HEIGHT};
const GUI_RECT statUpdateRect  = {0, (BYTE_HEIGHT*3) + PADDING*3 + ICON_HEIGHT, LCD_WIDTH, (BYTE_HEIGHT*4) + PADDING*3 + ICON_HEIGHT};
const GUI_RECT labelFailedRect = {0, (BYTE_HEIGHT*4) + PADDING*4 + ICON_HEIGHT, LCD_WIDTH, (BYTE_HEIGHT*5) + PADDING*4 + ICON_HEIGHT};

const uint32_t fontAddrList[] = {
  BYTE_ASCII_ADDR,
  WORD_UNICODE_ADDR,
  LARGE_FONT_ADDR,
  _8X16_FONT_ADDR
};

const char * fontPathList[] = {
  FONT_UPDATE_DIR "/" FILE_ASCII_FONT,
  FONT_UPDATE_DIR "/" FILE_UNICODE_FONT,
  FONT_UPDATE_DIR "/" FILE_LARGE_ASCII_FONT,
  FONT_UPDATE_DIR "/" FILE_8X16_ASCII_FONT
};

GUI_POINT bmp_size;

// This List is Auto-Generated. Please add new icons in icon_list.inc only
const char * const iconBmpName[] = {
  #define X_ICON(NAME) #NAME ,
    #include "icon_list.inc"
  #undef X_ICON
  // add new icons in icon_list.inc only
};

// This List is Auto-Generated. Please add new icons in small_icon_list.inc only
const char * const smallIconBmpName[] = {
  #define X_SMALLICON(NAME) #NAME ,
    #include "small_icon_list.inc"
  #undef X_SMALLICON
  // add new icons in small_icon_list.inc only
};

BMPUPDATE_STAT bmpDecode(char * bmp, uint32_t addr)
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

static inline bool updateIcon(char * rootDir)
{
  uint16_t found = 0;
  uint16_t notfound = 0;
  char curBmpPath[64];
  char tempstr[50];
  BMPUPDATE_STAT bmpState;

  GUI_Clear(infoSettings.bg_color);
  GUI_DispString(5, PADDING, (uint8_t *)"Updating Logo");
  GUI_ClearPrect(&iconUpdateRect);

  GET_FULL_PATH(curBmpPath, rootDir, BMP_UPDATE_DIR "/Logo" STR_PORTRAIT ".bmp");
  bmpState = bmpDecode(curBmpPath, LOGO_ADDR);

  if (bmpState == BMP_SUCCESS)
  {
    LOGO_ReadDisplay();
    found++;
  }
  else
  {
    notfound++;
    dispIconFail((uint8_t *)(curBmpPath), bmpState);
  }

  GUI_Clear(infoSettings.bg_color);
  GUI_DispString(5, PADDING, (uint8_t *)"Updating Icons");

  for (int i = 0; i < COUNT(iconBmpName); i++)
  {
    GUI_POINT last_size = bmp_size;
    sprintf(curBmpPath, "%s" BMP_UPDATE_DIR "/%s.bmp", rootDir, iconBmpName[i]);
    GUI_ClearPrect(&labelUpdateRect);
    GUI_DispString(labelUpdateRect.x0, labelUpdateRect.y0, (uint8_t *)curBmpPath);

    bmpState = bmpDecode(curBmpPath, ICON_ADDR(i));

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
      dispIconFail((uint8_t *)curBmpPath, bmpState);
    }
    // Display icon update progress
    sprintf(tempstr, "Updated: %d | Not Updated: %d", found, notfound);
    GUI_DispString(statUpdateRect.x0, statUpdateRect.y0, (uint8_t *)tempstr);
  }

  GET_FULL_PATH(curBmpPath, rootDir, BMP_UPDATE_DIR "/InfoBox.bmp");
  bmpState = bmpDecode(curBmpPath, INFOBOX_ADDR);

  if (bmpState == BMP_SUCCESS)
  {
    IMAGE_ReadDisplay(iconUpdateRect.x0, iconUpdateRect.y0, INFOBOX_ADDR);
    found++;
  }
  else
  {
    notfound++;
    dispIconFail((uint8_t *)(curBmpPath), bmpState);
  }

  if (notfound == 0)
    return true;
  else
    return false;
}

void dispIconFail(uint8_t * lbl, BMPUPDATE_STAT bmpState)
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
  GUI_DispString(labelFailedRect.x0, labelFailedRect.y0 + BYTE_HEIGHT + 2, (uint8_t *)error_txt);
  GUI_RestoreColorDefault();
  Delay_ms(1000);  // give some time to the user to read failed icon name.
}

bool updateFont(char * font, uint32_t addr)
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
  sprintf((void *)buffer, "%s Size: %dKB", font, (uint32_t)f_size(&myfp) >> 10);
  GUI_DispString(0, 100, (uint8_t *)buffer);
  GUI_DispString(0, 140, (uint8_t *)"Updating:   %");

  while (!f_eof(&myfp))
  {
    if (f_read(&myfp, tempbuf, W25QXX_SECTOR_SIZE, &rnum) != FR_OK) break;

    W25Qxx_EraseSector(addr + offset);
    W25Qxx_WriteBuffer(tempbuf, addr + offset, W25QXX_SECTOR_SIZE);
    offset += rnum;

    if (progress != (offset * 100) / f_size(&myfp))
    {
      progress = (offset * 100) / f_size(&myfp);
      GUI_DispDec(0 + BYTE_WIDTH * 9, 140, progress, 3, RIGHT);
    }

    if (rnum !=W25QXX_SECTOR_SIZE) break;
  }

  f_close(&myfp);
  free(tempbuf);
  return true;
}

static inline void scanResetDir(char * rootDir)
{
  char curPath[100];
  char renamedPath[100];
  GET_FULL_PATH(curPath, rootDir, TFT_RESET_FILE);
  GET_FULL_PATH(renamedPath, rootDir, TFT_RESET_FILE ".DONE");

  if (f_file_exists(TFT_RESET_FILE))
  {
    if (f_file_exists(renamedPath))
      f_unlink(renamedPath);

    initSettings();
    LCD_RefreshDirection(infoSettings.rotated_ui);
    TSC_Calibration();
    storePara();
    f_rename(curPath, renamedPath);
  }
}

static inline void replaceOldFile(char * curPath, char * newPath)
{
  if (!f_file_exists(curPath))  // if source file does not exist, nothing to do
    return;

  if (f_file_exists(newPath))
    f_unlink(newPath);  // remove already existing file first

  f_rename(curPath, newPath);
}

static inline void scanRenameUpdate(char * rootDir)
{
  char curPath[100];
  char renamedPath[100];
  char * renamedDIR = UPDATE_DIR ".CUR";
  char * firmwareFile = FIRMWARE_NAME ".bin";
  char * firmwareFileShort = FIRMWARE_NAME_SHORT ".NEW";
  char * renamedFirmwareFile = FIRMWARE_NAME ".CUR";
  char * renamedConfigFile = CONFIG_FILE_PATH ".CUR";

  GET_FULL_PATH(curPath, rootDir, FILE_ADMIN_MODE);

  if (f_file_exists(curPath))
    return;  // admin mode, need not rename

  // rename icon/font update folder
  GET_FULL_PATH(curPath, rootDir, UPDATE_DIR);

  if (f_dir_exists(curPath))
  {  // UPDATE_DIR exists
    GET_FULL_PATH(renamedPath, rootDir, renamedDIR);

    if (f_dir_exists(renamedPath))  // old UPDATE_DIR also exists
    {
      GUI_Clear(infoSettings.bg_color);
      // It will take some time to delete the old directory, so display "Deleting" on the screen to tell user.
      GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t *)"Deleting old update files...");
      f_remove_full_dir(renamedPath);
    }
    f_rename(curPath, renamedPath);
  }

  // rename firmware file
  GET_FULL_PATH(curPath, rootDir, firmwareFile);
  GET_FULL_PATH(renamedPath, rootDir, renamedFirmwareFile);
  replaceOldFile(curPath, renamedPath);

  // rename firmware file from short to full name
  GET_FULL_PATH(curPath, rootDir, firmwareFileShort);
  GET_FULL_PATH(renamedPath, rootDir, firmwareFile);
  replaceOldFile(curPath, renamedPath);

  // rename config file
  GET_FULL_PATH(curPath, rootDir, CONFIG_FILE_PATH);
  GET_FULL_PATH(renamedPath, rootDir, renamedConfigFile);
  replaceOldFile(curPath, renamedPath);
}

static inline void saveflashSign(uint8_t * buf, uint32_t size)
{
  W25Qxx_EraseSector(FLASH_SIGN_ADDR);
  Delay_ms(100);  // give time for spi flash to settle
  W25Qxx_WriteBuffer(buf, FLASH_SIGN_ADDR, size);
}

void scanUpdates(void)
{
  char * rootDir = NULL;

  #ifdef USB_FLASH_DRIVE_SUPPORT
    bool checkUSBDisk = true;
  #endif

  if (mountSDCard())
    rootDir = SD_ROOT_DIR;

#ifdef USB_FLASH_DRIVE_SUPPORT
checkupdate:
#endif
  if (rootDir != NULL)
  {
    char curfilePath[100];
    bool flash_sign_updated = false;
    uint32_t saved_flash_sign[sign_count];

    W25Qxx_ReadBuffer((uint8_t *)&saved_flash_sign, FLASH_SIGN_ADDR, sizeof(saved_flash_sign));

    // check for font update
    GET_FULL_PATH(curfilePath, rootDir, FONT_UPDATE_DIR);
    if (f_dir_exists(curfilePath))
    {
      bool updateOk = true;
      for (uint8_t i = 0; i < COUNT(fontAddrList); i++)
      {
        GET_FULL_PATH(curfilePath, rootDir, fontPathList[i]);
        if (!updateFont(curfilePath, fontAddrList[i]))
          updateOk = false;  // set update to false if any font fails to update
      }

      if (updateOk && saved_flash_sign[font_sign] != FONT_CHECK_SIGN)
      {
        saved_flash_sign[font_sign] = FONT_CHECK_SIGN;
        flash_sign_updated = true;
      }
    }

    // check for icon/bmp update
    GET_FULL_PATH(curfilePath, rootDir, BMP_UPDATE_DIR);
    if (f_dir_exists(BMP_UPDATE_DIR))
    {
      if (updateIcon(rootDir) && (saved_flash_sign[icon_sign] != ICON_CHECK_SIGN))
      {
        saved_flash_sign[icon_sign] = ICON_CHECK_SIGN;
        flash_sign_updated = true;
      }
    }

    // check for config update
    GET_FULL_PATH(curfilePath, rootDir, CONFIG_FILE_PATH);
    if (getConfigFromFile(curfilePath) && (saved_flash_sign[config_sign] != CONFIG_CHECK_SIGN))
    {
      saved_flash_sign[config_sign] = CONFIG_CHECK_SIGN;
      flash_sign_updated = true;
    }

    // check for language update
    if (getLangFromFile(rootDir) && (saved_flash_sign[lang_sign] != LANGUAGE_CHECK_SIGN))
    {
      saved_flash_sign[lang_sign] = LANGUAGE_CHECK_SIGN;
      flash_sign_updated = true;
    }

    // rename files
    scanRenameUpdate(rootDir);

    // check for reset file
    scanResetDir(rootDir);

    // update flash sign
    if (flash_sign_updated)
    {
      saveflashSign((uint8_t *)saved_flash_sign, sizeof(saved_flash_sign));
    }
  }

  #ifdef USB_FLASH_DRIVE_SUPPORT
    // check USB flash drive for update file
    else if (checkUSBDisk && mountUSBDisk())
    {
      rootDir = USB_ROOT_DIR;
      checkUSBDisk = false;
      goto checkupdate;
    }
  #endif
}
