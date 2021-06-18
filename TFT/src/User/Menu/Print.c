#include "Print.h"
#include "includes.h"

// File list number per page
#define NUM_PER_PAGE 5
// error labels for files/Volume errors
const int16_t labelVolumeError[3] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};
static bool list_mode = true;

const GUI_RECT titleRect = {10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH - 10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

const GUI_RECT gcodeRect[NUM_PER_PAGE] = {
  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   1*SPACE_X_PER_ICON-BYTE_WIDTH/2, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+1*SPACE_X_PER_ICON, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   2*SPACE_X_PER_ICON-BYTE_WIDTH/2, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+2*SPACE_X_PER_ICON, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   3*SPACE_X_PER_ICON-BYTE_WIDTH/2, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+3*SPACE_X_PER_ICON, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   4*SPACE_X_PER_ICON-BYTE_WIDTH/2, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON, 2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   1*SPACE_X_PER_ICON-BYTE_WIDTH/2, 2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
};

void normalNameDisp(const GUI_RECT *rect, uint8_t *name)
{
  if (name == NULL) return;

  GUI_ClearPrect(rect);
  GUI_SetRange(rect->x0, rect->y0, rect->x1, rect->y1);
  GUI_DispStringInPrect(rect, name);
  GUI_CancelRange();
}

// update files menu in icon mode
void gocdeIconDraw(void)
{
  uint8_t i = 0;
  ITEM curItem = {ICON_BACKGROUND, LABEL_BACKGROUND};

  // draw folders
  for (i = 0; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.folderCount) && (i < NUM_PER_PAGE); i++)
  {
    curItem.icon = ICON_FOLDER;
    menuDrawItem(&curItem, i);
    normalNameDisp(&gcodeRect[i], (uint8_t*)infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
  }

  // draw files
  for (; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.fileCount + infoFile.folderCount) && (i < NUM_PER_PAGE); i++)
  {
    if (EnterDir(infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.folderCount]) == false)
      break;
    // if model preview bmp exists, display bmp directly without writing to flash
    if (model_DirectDisplay(getIconStartPoint(i), infoFile.title) != true)
    {
      curItem.icon = ICON_FILE;
      menuDrawItem(&curItem, i);
    }
    ExitDir();
    normalNameDisp(&gcodeRect[i], (uint8_t*)infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.folderCount]);
  }

  // clear blank icons
  for (; (i < NUM_PER_PAGE); i++)
  {
    curItem.icon = ICON_BACKGROUND;
    menuDrawItem(&curItem, i);
  }
}

// update items in list mode
void gocdeListDraw(LISTITEM * item, uint16_t index, uint8_t itemPos)
{
  if (index < infoFile.folderCount)
  {
    // folder
    item->icon = CHARICON_FOLDER;
    item->titlelabel.index = LABEL_DYNAMIC;
    item->itemType = LIST_LABEL;
    setDynamicLabel(itemPos, infoFile.folder[index]);
  }
  else if (index < (infoFile.fileCount + infoFile.folderCount))
  {
    // gcode file
    item->icon = CHARICON_FILE;
    item->itemType = LIST_LABEL;
    item->titlelabel.index = LABEL_DYNAMIC;
    if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
      setDynamicLabel(itemPos, infoFile.Longfile[index - infoFile.folderCount]);
    else
      setDynamicLabel(itemPos, infoFile.file[index - infoFile.folderCount]);
  }
}

// start print
void startPrint(void)
{
  infoMenu.menu[++infoMenu.cur] = menuBeforePrinting;
}

// open selected file/folder
bool printPageItemSelected(uint16_t index)
{
  bool hasUpdate = true;
  if (index < infoFile.folderCount)  // folder
  {
    if (EnterDir(infoFile.folder[index]) == false)
    {
      hasUpdate = false;
    }
    else
    {
      scanPrintFiles();
      infoFile.cur_page = 0;
    }
  }
  else if (index < infoFile.folderCount + infoFile.fileCount)  // gcode
  {
    infoFile.fileIndex = index - infoFile.folderCount;

    if (infoHost.connected != true || EnterDir(infoFile.file[infoFile.fileIndex]) == false)
    {
      hasUpdate = false;
    }
    else
    {
      // load model preview in flash if icon exists
      setPrintModelIcon(infoFile.source < BOARD_SD && model_DecodeToFlash(infoFile.title));

      char temp_info[FILE_NUM + 50];
      sprintf(temp_info, (char *)textSelect(LABEL_START_PRINT),
              (uint8_t *)((infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD) ?
                              infoFile.Longfile[infoFile.fileIndex] :
                              infoFile.file[infoFile.fileIndex]));
      // confirm file selction
      setDialogText(LABEL_PRINT, (uint8_t *)temp_info, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_QUESTION, startPrint, ExitDir, NULL);

      hasUpdate = false;
    }
  }
  return hasUpdate;
}

void menuPrintFromSource(void)
{
  MENUITEMS printIconItems = {
    // title
    LABEL_BACKGROUND,
    // icon                          label
    {
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_PAGE_UP,                 LABEL_PAGE_UP},
      {ICON_PAGE_DOWN,               LABEL_PAGE_DOWN},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  uint8_t update = 1;

  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, LABEL_LOADING);

  if (mountFS() == true && scanPrintFiles() == true)
  {
    if (infoMenu.menu[infoMenu.cur] != menuPrintFromSource)  // Menu index be modify when "scanPrintFilesGcodeFs". (echo,error,warning popup windows)
    {
      return;
    }
    if (list_mode != true)
    {
      printIconItems.title.address = (uint8_t*)infoFile.title;
      menuDrawPage(&printIconItems);
    }
  }
  else
  {
    if (infoFile.source == BOARD_SD)  // error when the filesystem selected from TFT not available
      GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t*)requestCommandInfo.cmd_rev_buf);
    else
      GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, labelVolumeError[infoFile.source]);
    Delay_ms(1000);
    infoMenu.cur--;
  }

  while (infoMenu.menu[infoMenu.cur] == menuPrintFromSource)
  {

    if (list_mode != true) // select item from icon view
    {
      key_num = menuKeyGetValue();

      switch (key_num)
      {
        case KEY_ICON_5:
          if (infoFile.cur_page > 0)
          {
            infoFile.cur_page--;
            update = 1;
          }
          break;

        case KEY_ICON_6:
          if (infoFile.cur_page + 1 < (infoFile.folderCount + infoFile.fileCount + (NUM_PER_PAGE - 1)) / NUM_PER_PAGE)
          {
            infoFile.cur_page++;
            update = 1;
          }
          break;

        case KEY_ICON_7:
          infoFile.cur_page = 0;
          if (IsRootDir() == true)
          {
            clearInfoFile();
            infoMenu.cur--;
            break;
          }
          else
          {
            ExitDir();
            scanPrintFiles();
            update = 1;
          }
          break;

        case KEY_IDLE:
          break;

        default:
          update = printPageItemSelected(infoFile.cur_page * NUM_PER_PAGE + key_num);
          break;
      }
    }
    else // select item from list view
    {
      key_num = listViewGetSelectedIndex();

      switch (key_num)
      {
        case KEY_BACK:
          if (IsRootDir() == true)
          {
            clearInfoFile();
            infoMenu.cur--;
          }
          else
          {
            ExitDir();
            scanPrintFiles();
            update = 1;
          }
          break;

        case KEY_PAGEUP:
        case KEY_PAGEDOWN:
        case KEY_IDLE:
          break;

        default:
          if (printPageItemSelected(key_num))
            update = 1;
          break;
      }
    }

    // refresh file menu
    if (update)
    {
      update = 0;

      if (list_mode != true)
      {
        printIconItems.title.address = (uint8_t *)infoFile.title;
        gocdeIconDraw();
      }
      else
      {
        listViewCreate((LABEL){.address = (uint8_t *)infoFile.title}, NULL, infoFile.folderCount + infoFile.fileCount,
                       &infoFile.cur_page, false, NULL, gocdeListDraw);
      }

      Scroll_CreatePara(&scrollLine, (uint8_t *)infoFile.title, &titleRect);
      GUI_SetBkColor(infoSettings.title_bg_color);
      GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
      GUI_SetBkColor(infoSettings.bg_color);
    }

    GUI_SetBkColor(infoSettings.title_bg_color);
    Scroll_DispString(&scrollLine, LEFT);
    GUI_SetBkColor(infoSettings.bg_color);

    #ifdef SD_CD_PIN
      if (isVolumeExist(infoFile.source) != true)
      {
        resetInfoFile();
        infoMenu.cur--;
      }
    #endif

    loopProcess();
  }
}

void menuPrint(void)
{
  MENUITEMS sourceSelItems = {
    // title
    LABEL_PRINT,
    // icon                          label
    {
      {ICON_ONTFT_SD,                LABEL_TFTSD},
      #ifdef U_DISK_SUPPORT
        {ICON_U_DISK,                  LABEL_U_DISK},
        #define ONBOARD_SD_INDEX 2
      #else
        {ICON_BACKGROUND,              LABEL_BACKGROUND},
        #define ONBOARD_SD_INDEX 1
      #endif
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_SCREEN_INFO,             LABEL_PREVIOUS_PRINT_DATA},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  sourceSelItems.items[ONBOARD_SD_INDEX].icon = (infoMachineSettings.onboard_sd_support == ENABLED) ? ICON_ONBOARD_SD : ICON_BACKGROUND;
  sourceSelItems.items[ONBOARD_SD_INDEX].label.index = (infoMachineSettings.onboard_sd_support == ENABLED) ? LABEL_ONBOARDSD : LABEL_BACKGROUND;

  menuDrawPage(&sourceSelItems);

  while (infoMenu.menu[infoMenu.cur] == menuPrint)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        list_mode = infoSettings.file_listmode;  // follow list mode setting in TFT sd card
        infoFile.source = TFT_SD;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;
        infoMenu.menu[++infoMenu.cur] = menuPowerOff;
        goto selectEnd;

      #ifdef U_DISK_SUPPORT
        case KEY_ICON_1:
          list_mode = infoSettings.file_listmode;  // follow list mode setting in usb disk
          infoFile.source = TFT_UDISK;
          infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;
          infoMenu.menu[++infoMenu.cur] = menuPowerOff;
          goto selectEnd;
        case KEY_ICON_2:
      #else
        case KEY_ICON_1:
      #endif
        if (infoMachineSettings.onboard_sd_support == ENABLED)
        {
          list_mode = true;  // force list mode in Onboard sd card
          infoFile.source = BOARD_SD;
          infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;  // TODO: fix here,  onboard sd card PLR feature
          goto selectEnd;
        }
        break;

      case KEY_ICON_4:
        if (infoPrintSummary.name[0] != 0)
          printInfoPopup();
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        return;

      default:
        break;
    }
    loopProcess();
  }

selectEnd:
  if (!infoHost.printing)  // prevent reset if printing from other source
  {
    resetInfoFile();
    powerFailedSetDriverSource(getCurFileSource());
  }
}
