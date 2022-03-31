#include "Print.h"
#include "includes.h"

// file list number per page
#define NUM_PER_PAGE 5

// key enums for media selection page
enum
{
  PRINT_KEY_TFT_SD,
  #ifdef USB_FLASH_DRIVE_SUPPORT
    PRINT_KEY_TFT_USB,
  #endif
  PRINT_KEY_ONBOARD_SD,
  PRINT_KEY_ONBOARD_USB
};

const GUI_RECT titleRect = {10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH - 10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

const GUI_RECT gcodeRect[NUM_PER_PAGE] = {
#ifdef PORTRAIT_MODE
  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   1*SPACE_X_PER_ICON-BYTE_WIDTH/2, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+1*SPACE_X_PER_ICON, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   2*SPACE_X_PER_ICON-BYTE_WIDTH/2, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+2*SPACE_X_PER_ICON, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   3*SPACE_X_PER_ICON-BYTE_WIDTH/2, 1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON, 2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   1*SPACE_X_PER_ICON-BYTE_WIDTH/2, 2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+1*SPACE_X_PER_ICON, 2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
   2*SPACE_X_PER_ICON-BYTE_WIDTH/2, 2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
#else
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
#endif
};

// error labels for files/Volume errors
const int16_t labelVolumeError[3] = {LABEL_TFT_SD_READ_ERROR, LABEL_TFT_USB_READ_ERROR, LABEL_ONBOARD_SD_READ_ERROR};

static bool list_mode = true;

void normalNameDisp(const GUI_RECT *rect, uint8_t *name)
{
  if (name == NULL)
    return;

  GUI_ClearPrect(rect);
  GUI_SetRange(rect->x0, rect->y0, rect->x1, rect->y1);
  GUI_DispStringInPrect(rect, name);
  GUI_CancelRange();
}

// update files menu in icon mode
void gocdeIconDraw(void)
{
  ITEM curItem = {ICON_NULL, LABEL_NULL};
  uint8_t baseIndex = infoFile.curPage * NUM_PER_PAGE;
  uint8_t i = 0;

  // draw folders
  for (; (baseIndex + i < infoFile.folderCount) && (i < NUM_PER_PAGE); i++)
  {
    curItem.icon = ICON_FOLDER;
    menuDrawItem(&curItem, i);
    normalNameDisp(&gcodeRect[i], (uint8_t*)infoFile.folder[baseIndex + i]);  // always use short folder name
  }

  // draw gcode files
  for (; (baseIndex + i < infoFile.folderCount + infoFile.fileCount) && (i < NUM_PER_PAGE); i++)
  {
    restoreFilenameExtension(baseIndex + i - infoFile.folderCount);  // restore filename extension if filename extension feature is disabled

    if (enterFolder(infoFile.file[baseIndex + i - infoFile.folderCount]) == false)  // always use short filename for file path
      break;

    // if model preview bmp exists, display bmp directly without writing to flash
    if (infoMachineSettings.firmwareType == FW_REPRAPFW || !model_DirectDisplay(getIconStartPoint(i), infoFile.path))
    {
      curItem.icon = ICON_FILE;
      menuDrawItem(&curItem, i);
    }

    exitFolder();

    hideFilenameExtension(baseIndex + i - infoFile.folderCount);  // hide filename extension if filename extension feature is disabled
    normalNameDisp(&gcodeRect[i], (uint8_t*)infoFile.file[baseIndex + i - infoFile.folderCount]);  // always use short filename
  }

  // clear blank icons
  for (; i < NUM_PER_PAGE; i++)
  {
    curItem.icon = ICON_NULL;
    menuDrawItem(&curItem, i);
  }
}

// update items in list mode
void gocdeListDraw(LISTITEM * item, uint16_t index, uint8_t itemPos)
{
  if (index < infoFile.folderCount)  // folder
  {
    item->icon = CHARICON_FOLDER;
    item->itemType = LIST_LABEL;
    item->titlelabel.index = LABEL_DYNAMIC;
    setDynamicLabel(itemPos, getFoldername(index));  // display short or long folder name
  }
  else if (index < infoFile.folderCount + infoFile.fileCount)  // gcode file
  {
    item->icon = CHARICON_FILE;
    item->itemType = LIST_LABEL;
    item->titlelabel.index = LABEL_DYNAMIC;
    setDynamicLabel(itemPos, hideFilenameExtension(index - infoFile.folderCount));  // hide filename extension if filename extension feature is disabled
  }
}

// open selected file/folder
bool printPageItemSelected(uint16_t index)
{
  bool hasUpdate = true;

  if (index < infoFile.folderCount)  // folder
  {
    if (enterFolder(infoFile.folder[index]) == false)  // always use short folder name for file path
    {
      hasUpdate = false;
    }
    else
    {
      scanPrintFiles();
      infoFile.curPage = 0;
    }
  }
  else if (index < infoFile.folderCount + infoFile.fileCount)  // gcode file
  {
    infoFile.fileIndex = index - infoFile.folderCount;
    char * filename = restoreFilenameExtension(infoFile.fileIndex);  // restore filename extension if filename extension feature is disabled

    if (infoHost.connected != true || enterFolder(infoFile.file[infoFile.fileIndex]) == false)  // always use short filename for file path
    {
      hasUpdate = false;
    }
    else
    {
      // load model preview in flash if icon exists
      setPrintModelIcon(infoFile.source < FS_ONBOARD_MEDIA && model_DecodeToFlash(infoFile.path));

      char temp_info[FILE_NUM + 50];
      sprintf(temp_info, (char *)textSelect(LABEL_START_PRINT), (uint8_t *)(filename));  // display short or long filename

      // confirm file selction
      setDialogText(LABEL_PRINT, (uint8_t *)temp_info, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_QUESTION, startPrint, exitFolder, NULL);

      hasUpdate = false;
    }
  }

  return hasUpdate;
}

void menuPrintFromSource(void)
{
  MENUITEMS printIconItems = {
    // title
    LABEL_NULL,
    // icon                          label
    {
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_PAGE_UP,                 LABEL_PAGE_UP},
      {ICON_PAGE_DOWN,               LABEL_PAGE_DOWN},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  uint8_t update = 1;     // 0: no update, 1: update with title bar, 2: update without title bar
  uint8_t pageCount;      // it will be used and handled in the icon view loop

  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, LABEL_LOADING);

  if (mountFS() == true && scanPrintFiles() == true)
  {
    if (MENU_IS_NOT(menuPrintFromSource))  // menu index has to be modified when "scanPrintFilesGcodeFs" (echo,error,warning popup windows)
      return;

    if (list_mode != true)
    {
      printIconItems.title.address = (uint8_t*)infoFile.path;
      menuDrawPage(&printIconItems);
    }
  }
  else
  {
    if (infoFile.source == FS_ONBOARD_MEDIA)  // error when the filesystem selected from TFT media not available
      GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t*)requestCommandInfo.cmd_rev_buf);
    else
      GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, labelVolumeError[infoFile.source]);

    Delay_ms(1000);
    CLOSE_MENU();
  }

  while (MENU_IS(menuPrintFromSource))
  {
    if (list_mode != true)  // select item from icon view
    {
      pageCount = (infoFile.folderCount + infoFile.fileCount + (NUM_PER_PAGE - 1)) / NUM_PER_PAGE;
      key_num = menuKeyGetValue();

      switch (key_num)
      {
        case KEY_ICON_5:
        case KEY_DECREASE:
          if (infoFile.curPage > 0)
          {
            infoFile.curPage--;
            update = 2;  // request no title bar update
          }
          break;

        case KEY_ICON_6:
        case KEY_INCREASE:
          if (infoFile.curPage + 1 < pageCount)
          {
            infoFile.curPage++;
            update = 2;  // request no title bar update
          }
          break;

        case KEY_ICON_7:
          infoFile.curPage = 0;

          if (isRootFolder() == true)
          {
            clearInfoFile();
            CLOSE_MENU();
            break;
          }
          else
          {
            exitFolder();
            scanPrintFiles();
            update = 1;
          }
          break;

        case KEY_IDLE:
          break;

        default:
          if (printPageItemSelected(infoFile.curPage * NUM_PER_PAGE + key_num))
            update = 1;
          break;
      }
    }
    else  // select item from list view
    {
      key_num = listViewGetSelectedIndex();

      switch (key_num)
      {
        case KEY_BACK:
          if (isRootFolder() == true)
          {
            clearInfoFile();
            CLOSE_MENU();
          }
          else
          {
            exitFolder();
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
    if (update != 0)
    {
      if (list_mode != true)
      {
        printIconItems.title.address = (uint8_t *)infoFile.path;
        gocdeIconDraw();

        if (update != 2)  // update title only when entering/exiting to/from directory
          menuDrawTitle();
      }
      else
      { // title bar is also drawn by listViewCreate
        listViewCreate((LABEL){.address = (uint8_t *)infoFile.path}, NULL, infoFile.folderCount + infoFile.fileCount,
                       &infoFile.curPage, false, NULL, gocdeListDraw);
      }

      Scroll_CreatePara(&scrollLine, (uint8_t *)infoFile.path, &titleRect);

      update = 0;  // finally reset update request
    }

    GUI_SetBkColor(infoSettings.title_bg_color);
    Scroll_DispString(&scrollLine, LEFT);
    GUI_SetBkColor(infoSettings.bg_color);

    #ifdef SD_CD_PIN
      if (volumeExists(infoFile.source) != true)
      {
        resetInfoFile();
        CLOSE_MENU();
      }
    #endif

    loopProcess();
  }
}

void menuPrint(void)
{
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    list_mode = infoSettings.files_list_mode;
    infoFile.source = FS_ONBOARD_MEDIA;
    REPLACE_MENU(menuPrintFromSource);
    goto selectEnd;
  }

  MENUITEMS sourceSelItems = {
    // title
    LABEL_PRINT,
    // icon                          label
    {
      {ICON_ONTFT_SD,                LABEL_TFT_SD},
      #ifdef USB_FLASH_DRIVE_SUPPORT
        {ICON_USB_DISK,                LABEL_TFT_USB},
      #else
        {ICON_NULL,                    LABEL_NULL},
      #endif
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_SCREEN_INFO,             LABEL_PREVIOUS_PRINT_DATA},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoMachineSettings.onboardSD == ENABLED)
  {
    sourceSelItems.items[PRINT_KEY_ONBOARD_SD].icon = ICON_ONBOARD_SD;
    sourceSelItems.items[PRINT_KEY_ONBOARD_SD].label.index = LABEL_ONBOARD_SD;

    if (infoMachineSettings.multiVolume == ENABLED)
    {
      sourceSelItems.items[PRINT_KEY_ONBOARD_USB].icon = ICON_USB_DISK;
      sourceSelItems.items[PRINT_KEY_ONBOARD_USB].label.index = LABEL_ONBOARD_USB;
    }
  }

  menuDrawPage(&sourceSelItems);

  while (MENU_IS(menuPrint))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case PRINT_KEY_TFT_SD:
        if (volumeExists(FS_TFT_SD))
        {
          list_mode = infoSettings.files_list_mode;  // follow list mode setting in TFT SD card
          infoFile.source = FS_TFT_SD;
          OPEN_MENU(menuPrintFromSource);
          OPEN_MENU(menuPrintRestore);
          goto selectEnd;
        }
        else
        {
          addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_TFT_SD_NOT_DETECTED));
        }
        break;

      #ifdef USB_FLASH_DRIVE_SUPPORT
        case PRINT_KEY_TFT_USB:
          if (volumeExists(FS_TFT_USB))
          {
            list_mode = infoSettings.files_list_mode;  // follow list mode setting in TFT USB disk
            infoFile.source = FS_TFT_USB;
            OPEN_MENU(menuPrintFromSource);
            OPEN_MENU(menuPrintRestore);
            goto selectEnd;
          }
          else
          {
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_TFT_USB_NOT_DETECTED));
          }
          break;
      #endif

      case PRINT_KEY_ONBOARD_SD:
      case PRINT_KEY_ONBOARD_USB:
        if (infoMachineSettings.onboardSD == ENABLED)
        {
          if (key_num == (KEY_VALUES)PRINT_KEY_ONBOARD_SD)
            infoFile.onboardSource = BOARD_SD;
          else if (infoMachineSettings.multiVolume == ENABLED)
            infoFile.onboardSource = BOARD_USB;

          if (key_num == (KEY_VALUES)PRINT_KEY_ONBOARD_SD ||
              (infoMachineSettings.multiVolume == ENABLED && key_num == (KEY_VALUES)PRINT_KEY_ONBOARD_USB))
          {
            list_mode = true;  // force list mode in onboard media
            infoFile.source = FS_ONBOARD_MEDIA;
            OPEN_MENU(menuPrintFromSource);
            goto selectEnd;
          }
        }
        break;

      case KEY_ICON_4:
        if (infoPrintSummary.name[0] != '\0')
          printSummaryPopup();
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        return;

      default:
        break;
    }

    loopProcess();
  }

selectEnd:
  if (!isHostPrinting())  // prevent reset if printing from other source
    resetInfoFile();
}
