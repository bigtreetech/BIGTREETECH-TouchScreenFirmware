#include "boot.h"
#include "includes.h"

#define STR_PATH_JOIN                         "%s/%s"
#define GET_FULL_PATH(buf, rootDir, filepath) sprintf(buf, STR_PATH_JOIN, rootDir, filepath)
#define PADDING                               10

typedef enum
{
  BMP_SUCCESS,
  BMP_NOTFOUND,
  BMP_NOT24BIT,
  BMP_INVALIDFILE
} BMPUPDATE_STAT;

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

// this list is Auto-Generated. Please add new icons in icon_list.inc only
const char * const iconBmpName[] = {
  #define X_ICON(NAME) #NAME ,
    #include "icon_list.inc"
  #undef X_ICON
};

GUI_POINT bmpSize;

BMPUPDATE_STAT bmpDecode(char * bmp, uint32_t addr)
{
  FIL bmpFile;
  char magic[2];
  uint16_t w, h;
  int bytePerLine;
  short bpp;
  int offset;
  uint8_t buf[256];
  uint8_t lcdColor[4];
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

  bmpSize.x = w;
  bmpSize.y = h;
  bpp >>= 3;
  bytePerLine = w * bpp;

  if (bytePerLine % 4 != 0)  // bmp
    bytePerLine = (bytePerLine / 4 + 1) * 4;

  for (bnum = 0; bnum < (w * h * 2 + W25QXX_SECTOR_SIZE - 1) / W25QXX_SECTOR_SIZE; bnum++)
  {
    W25Qxx_EraseSector(addr + bnum * W25QXX_SECTOR_SIZE);
  }

  // store size of BMP
  bnum = 0;
  memcpy(buf, (uint8_t *)&w, sizeof(uint16_t));
  bnum += sizeof(uint16_t);
  memcpy(buf + bnum, (uint8_t *)&h, sizeof(uint16_t));
  bnum += sizeof(uint16_t);

  for (int j = 0; j < h; j++)
  {
    f_lseek(&bmpFile, offset + (h - j - 1) * bytePerLine);

    for (int i = 0; i < w; i++)
    {
      f_read(&bmpFile, (char *)&lcdColor, bpp, &mybr);

      pix.RGB.r = lcdColor[2] >> 3;
      pix.RGB.g = lcdColor[1] >> 2;
      pix.RGB.b = lcdColor[0] >> 3;

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

  W25Qxx_WritePage(buf, addr, bnum);
  addr += bnum;
  f_close(&bmpFile);

  return BMP_SUCCESS;
}

void dispIconFail(uint8_t * lbl, BMPUPDATE_STAT bmpState)
{
  char * statTxt;
  char errorTxt[30];

  GUI_SetColor(infoSettings.reminder_color);
  GUI_ClearPrect(&labelFailedRect);
  GUI_DispString(labelFailedRect.x0, labelFailedRect.y0, lbl);

  switch (bmpState)
  {
    case BMP_INVALIDFILE:
      statTxt = "BMP file not valid ";
      break;

    case BMP_NOT24BIT:
      statTxt = "Format is not 24Bit";
      break;

    case BMP_NOTFOUND:
    default:
      statTxt = "BMP file not found ";
      break;
  }

  sprintf(errorTxt, "Error: %s", statTxt);
  GUI_DispString(labelFailedRect.x0, labelFailedRect.y0 + BYTE_HEIGHT + 2, (uint8_t *)errorTxt);
  GUI_RestoreColorDefault();
  Delay_ms(1000);  // give some time to the user to read failed icon name.
}

static inline bool updateIcon(char * rootDir)
{
  uint16_t found = 0;
  uint16_t notFound = 0;
  char curBmpPath[64];
  char tempStr[50];
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
    notFound++;
    dispIconFail((uint8_t *)(curBmpPath), bmpState);
  }

  GUI_Clear(infoSettings.bg_color);
  GUI_DispString(5, PADDING, (uint8_t *)"Updating Icons");

  for (int i = 0; i < COUNT(iconBmpName); i++)
  {
    GUI_POINT lastSize = bmpSize;

    sprintf(curBmpPath, "%s%s/%s.bmp", rootDir, BMP_UPDATE_DIR, iconBmpName[i]);
    bmpState = bmpDecode(curBmpPath, ICON_ADDR(i));

    GUI_ClearPrect(&labelUpdateRect);
    GUI_DispString(labelUpdateRect.x0, labelUpdateRect.y0, (uint8_t *)curBmpPath);
    GUI_ClearRect(iconUpdateRect.x0, iconUpdateRect.y0, iconUpdateRect.x0 + lastSize.x, iconUpdateRect.y0 + lastSize.y);

    if (bmpState == BMP_SUCCESS)
    { // display bmp update success
      found++;
      IMAGE_ReadDisplay(iconUpdateRect.x0, iconUpdateRect.y0, ICON_ADDR(i));
    }
    else
    { // display bmp update fail
      notFound++;
      dispIconFail((uint8_t *)curBmpPath, bmpState);
    }

    // display icon update progress
    sprintf(tempStr, "Updated: %d | Not Updated: %d", found, notFound);
    GUI_DispString(statUpdateRect.x0, statUpdateRect.y0, (uint8_t *)tempStr);
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
    notFound++;
    dispIconFail((uint8_t *)(curBmpPath), bmpState);
  }

  return !notFound;
}

static inline bool updateFont(char * font, uint32_t addr)
{
  uint8_t progress = 0;
  UINT rnum = 0;
  uint32_t offset = 0;
  FIL myfp;
  uint8_t * tempBuf = NULL;

  if (f_open(&myfp, font, FA_OPEN_EXISTING|FA_READ) != FR_OK)
    return false;

  tempBuf = malloc(W25QXX_SECTOR_SIZE);

  if (tempBuf == NULL)
    return false;

  GUI_Clear(infoSettings.bg_color);
  GUI_DispString(5, PADDING, (uint8_t *)"Updating Fonts");
  GUI_DispString(0, BYTE_HEIGHT * 3 + PADDING, (uint8_t *)font);
  GUI_DispString(0, BYTE_HEIGHT * 4 + PADDING, (uint8_t *)"Size:    KB");
  GUI_DispDec(0 + BYTE_WIDTH * 5, BYTE_HEIGHT * 4 + PADDING, (uint32_t)f_size(&myfp) >> 10, 4, RIGHT);
  GUI_DispString(0, BYTE_HEIGHT * 5 + PADDING, (uint8_t *)"Updating:   %");

  while (!f_eof(&myfp))
  {
    if (f_read(&myfp, tempBuf, W25QXX_SECTOR_SIZE, &rnum) != FR_OK)
      break;

    W25Qxx_EraseSector(addr + offset);
    W25Qxx_WriteBuffer(tempBuf, addr + offset, W25QXX_SECTOR_SIZE);
    offset += rnum;

    if (progress != (offset * 100) / f_size(&myfp))
    {
      progress = (offset * 100) / f_size(&myfp);
      GUI_DispDec(0 + BYTE_WIDTH * 9, BYTE_HEIGHT * 5 + PADDING, progress, 3, RIGHT);
    }

    if (rnum != W25QXX_SECTOR_SIZE)
      break;
  }

  f_close(&myfp);
  free(tempBuf);

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
    TS_Calibrate();
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
  { // UPDATE_DIR exists
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

  if (mountSDCard())
    rootDir = SD_ROOT_DIR;
  #ifdef USB_FLASH_DRIVE_SUPPORT
    // check USB flash drive for update file
    else if (mountUSBDisk())
      rootDir = USB_ROOT_DIR;
  #endif
  else
    return;

  char curFilePath[100];
  bool updatedFlashSign = false;
  uint32_t savedFlashSign[sign_count];

  W25Qxx_ReadBuffer((uint8_t *)&savedFlashSign, FLASH_SIGN_ADDR, sizeof(savedFlashSign));

  // check for font update
  GET_FULL_PATH(curFilePath, rootDir, FONT_UPDATE_DIR);

  if (f_dir_exists(curFilePath))
  {
    bool updateOk = true;

    for (uint8_t i = 0; i < COUNT(fontAddrList); i++)
    {
      GET_FULL_PATH(curFilePath, rootDir, fontPathList[i]);

      if (!updateFont(curFilePath, fontAddrList[i]))
        updateOk = false;  // set update to false if any font fails to update
    }

    if (updateOk && savedFlashSign[font_sign] != FONT_CHECK_SIGN)
    {
      savedFlashSign[font_sign] = FONT_CHECK_SIGN;
      updatedFlashSign = true;
    }
  }

  // check for icon/bmp update
  GET_FULL_PATH(curFilePath, rootDir, BMP_UPDATE_DIR);

  if (f_dir_exists(curFilePath))
  {
    if (updateIcon(rootDir) && (savedFlashSign[icon_sign] != ICON_CHECK_SIGN))
    {
      savedFlashSign[icon_sign] = ICON_CHECK_SIGN;
      updatedFlashSign = true;
    }
  }

  // check for config update
  GET_FULL_PATH(curFilePath, rootDir, CONFIG_FILE_PATH);

  if (getConfigFromFile(curFilePath) && (savedFlashSign[config_sign] != CONFIG_CHECK_SIGN))
  {
    savedFlashSign[config_sign] = CONFIG_CHECK_SIGN;
    updatedFlashSign = true;
  }

  // check for language update
  if (getLangFromFile(rootDir) && (savedFlashSign[lang_sign] != LANGUAGE_CHECK_SIGN))
  {
    savedFlashSign[lang_sign] = LANGUAGE_CHECK_SIGN;
    updatedFlashSign = true;
  }

  // rename files
  scanRenameUpdate(rootDir);

  // check for reset file
  scanResetDir(rootDir);

  // update flash sign
  if (updatedFlashSign)
    saveflashSign((uint8_t *)savedFlashSign, sizeof(savedFlashSign));
}
