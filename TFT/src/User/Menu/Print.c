#include "Print.h"
#include "includes.h"

MENUITEMS * printIconItems;
LISTITEMS * printListItems;


const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

// File list number per page
#define NUM_PER_PAGE 5
static bool list_mode = true;
SCROLL titleScroll;
const GUI_RECT titleRect = {10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH-10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

const GUI_RECT gcodeRect[NUM_PER_PAGE] = {
  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  1*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+1*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  2*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+2*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  3*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+3*SPACE_X_PER_ICON,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  4*SPACE_X_PER_ICON-BYTE_WIDTH/2,  1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},

  {BYTE_WIDTH/2+0*SPACE_X_PER_ICON,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2,
  1*SPACE_X_PER_ICON-BYTE_WIDTH/2,  2*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y+(SPACE_Y-BYTE_HEIGHT)/2+BYTE_HEIGHT},
};

void normalNameDisp(const GUI_RECT *rect, uint8_t *name)
{
  if (name == NULL) return;

  GUI_ClearPrect(rect);
  GUI_SetRange(rect->x0, rect->y0, rect->x1, rect->y1);
  GUI_DispStringInPrect(rect, name);
  GUI_CancelRange();
}

void gocdeIconDraw(void)
{
  uint8_t i = 0;
  ITEM curItem = {ICON_BACKGROUND, LABEL_BACKGROUND};

  Scroll_CreatePara(&titleScroll, (uint8_t*)infoFile.title, &titleRect);
  printIconItems->title.address = (uint8_t*)infoFile.title;
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_ClearPrect(&titleRect);
  GUI_SetBkColor(infoSettings.bg_color);

  //draw folders
  for (i = 0; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.folderCount) && (i < NUM_PER_PAGE); i++)
  {
    curItem.icon = ICON_FOLDER;
    menuDrawItem(&curItem, i);
    normalNameDisp(&gcodeRect[i], (uint8_t*)infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
  }

  //draw files
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

  //clear blank icons
  for (; (i < NUM_PER_PAGE); i++)
  {
    curItem.icon = ICON_BACKGROUND;
    menuDrawItem(&curItem, i);
  }
}

void gocdeListDraw(void)
{
  uint8_t i = 0;

  Scroll_CreatePara(&titleScroll, (uint8_t *)infoFile.title, &titleRect);
  printListItems->title.address = (uint8_t *)infoFile.title;
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
  GUI_SetBkColor(infoSettings.bg_color);

  // folder
  for (i = 0; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.folderCount) && (i < NUM_PER_PAGE); i++)
  {
     printListItems->items[i].icon = ICONCHAR_FOLDER;
    setDynamicLabel(i, infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
     printListItems->items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&printListItems->items[i], i);
  }
  // gcode file
  for (; (i + infoFile.cur_page * NUM_PER_PAGE < infoFile.fileCount + infoFile.folderCount) && (i < NUM_PER_PAGE); i++)
  {
     printListItems->items[i].icon = ICONCHAR_FILE;
    if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
    {
      setDynamicLabel(i, infoFile.Longfile[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.folderCount]);
    }
    else
    {
      setDynamicLabel(i, infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.folderCount]);
    }
     printListItems->items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&printListItems->items[i], i);
  }

  //background
  for (; (i < NUM_PER_PAGE); i++)
  {
     printListItems->items[i].icon = ICONCHAR_BACKGROUND;
     printListItems->items[i].titlelabel.index = LABEL_BACKGROUND;
    menuDrawListItem(& printListItems->items[i], i);
  }
  // set page up down button according to page count and current page
  int t_pagenum = (infoFile.folderCount + infoFile.fileCount + (LISTITEM_PER_PAGE - 1)) / LISTITEM_PER_PAGE;
  if ((infoFile.folderCount + infoFile.fileCount) <= LISTITEM_PER_PAGE)
  {
     printListItems->items[5].icon = ICONCHAR_BACKGROUND;
     printListItems->items[6].icon = ICONCHAR_BACKGROUND;
  }
  else
  {
    if (infoFile.cur_page == 0)
    {
       printListItems->items[5].icon = ICONCHAR_BACKGROUND;
       printListItems->items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if (infoFile.cur_page == (t_pagenum - 1))
    {
       printListItems->items[5].icon = ICONCHAR_PAGEUP;
       printListItems->items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
       printListItems->items[5].icon = ICONCHAR_PAGEUP;
       printListItems->items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
  menuDrawListItem(&printListItems->items[5], 5);
  menuDrawListItem(&printListItems->items[6], 6);
}

void startPrint(void)
{
  infoMenu.menu[++infoMenu.cur] = menuBeforePrinting;
}

void menuPrintFromSource(void)
{
  MENUITEMS _printIconItems = {
  // title
  LABEL_BACKGROUND,
  // icon              label
  {{ICON_BACKGROUND,   LABEL_BACKGROUND},
   {ICON_BACKGROUND,   LABEL_BACKGROUND},
   {ICON_BACKGROUND,   LABEL_BACKGROUND},
   {ICON_BACKGROUND,   LABEL_BACKGROUND},
   {ICON_BACKGROUND,   LABEL_BACKGROUND},
   {ICON_PAGE_UP,      LABEL_PAGE_UP},
   {ICON_PAGE_DOWN,    LABEL_PAGE_DOWN},
   {ICON_BACK,         LABEL_BACK},}
  };

  LISTITEMS _printListItems = {
  // title
  LABEL_BACKGROUND,
  // icon                 ItemType    Item Title        item value text(only for custom value)
  {
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    {ICONCHAR_BACK,       LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  uint8_t update = 0;

  printIconItems = &_printIconItems;
  printListItems = &_printListItems;

  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, LABEL_LOADING);

  if (mountFS() == true && scanPrintFiles() == true)
  {
    if (list_mode != true)
    {
      menuDrawPage(&_printIconItems);
      gocdeIconDraw();
    }
    else
    {
      menuDrawListPage(&_printListItems);
      gocdeListDraw();
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

  while(infoMenu.menu[infoMenu.cur] == menuPrintFromSource)
  {
    GUI_SetBkColor(infoSettings.title_bg_color);
    Scroll_DispString(&titleScroll, LEFT);
    GUI_SetBkColor(infoSettings.bg_color);

    key_num = menuKeyGetValue();

    switch(key_num)
    {
      case KEY_ICON_5:
        if (infoFile.cur_page > 0)
        {
          infoFile.cur_page--;
          update=1;
        }
        break;

      case KEY_ICON_6:
        if (infoFile.cur_page + 1 < (infoFile.folderCount + infoFile.fileCount + (NUM_PER_PAGE - 1)) / NUM_PER_PAGE)
        {
          infoFile.cur_page++;
          update=1;
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
        if (key_num < ITEM_PER_PAGE)
        {
          uint16_t start = infoFile.cur_page * NUM_PER_PAGE;
          if (key_num + start < infoFile.folderCount)  //folder
          {
            if (EnterDir(infoFile.folder[key_num + start]) == false)
              break;
            scanPrintFiles();
            update = 1;
            infoFile.cur_page = 0;
          }
          else if (key_num + start < infoFile.folderCount + infoFile.fileCount)  //gcode
          {
            if (infoHost.connected != true)
              break;
            infoFile.fileIndex = key_num + start - infoFile.folderCount;
            if (EnterDir(infoFile.file[infoFile.fileIndex]) == false)
              break;
            //load model preview in flash if icon exists
            setPrintModelIcon(infoFile.source < BOARD_SD && model_DecodeToFlash(infoFile.title));

            char temp_info[FILE_NUM + 50];
            sprintf(temp_info, (char *)textSelect(LABEL_START_PRINT),
                    (uint8_t *)((infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD) ?
                    infoFile.Longfile[infoFile.fileIndex] : infoFile.file[infoFile.fileIndex]));
            //confirm file selction
            setDialogText(LABEL_PRINT, (uint8_t *)temp_info, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_QUESTION, startPrint, ExitDir, NULL);
          }
        }
        break;
    }

    if (update)
    {
      update = 0;

      if (list_mode != true)
        gocdeIconDraw();
      else
        gocdeListDraw();
    }
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
  KEY_VALUES  key_num;

  MENUITEMS sourceSelItems = {
    // title
    LABEL_PRINT,
    // icon                         label
    {{ICON_ONTFT_SD,                LABEL_TFTSD},
  #ifdef U_DISK_SUPPORT
     {ICON_U_DISK,                  LABEL_U_DISK},
    #define ONBOARD_SD_INDEX 2
  #else
    #define ONBOARD_SD_INDEX 1
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
  #endif
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_SCREEN_INFO,          LABEL_PREVIOUS_PRINT_DATA},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK}}
  };

  sourceSelItems.items[ONBOARD_SD_INDEX].icon = (infoMachineSettings.onboard_sd_support == ENABLED) ? ICON_ONBOARD_SD : ICON_BACKGROUND;
  sourceSelItems.items[ONBOARD_SD_INDEX].label.index = (infoMachineSettings.onboard_sd_support == ENABLED) ? LABEL_ONBOARDSD : LABEL_BACKGROUND;

  menuDrawPage(&sourceSelItems);
  while(infoMenu.menu[infoMenu.cur] == menuPrint)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        list_mode = infoSettings.file_listmode; //follow list mode setting in TFT sd card
        infoFile.source = TFT_SD;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;
        infoMenu.menu[++infoMenu.cur] = menuPowerOff;
        goto selectEnd;

    #ifdef U_DISK_SUPPORT
      case KEY_ICON_1:
        list_mode = infoSettings.file_listmode; //follow list mode setting in usb disk
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
          list_mode = true; //force list mode in Onboard sd card
          infoFile.source = BOARD_SD;
          infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;   //TODO: fix here,  onboard sd card PLR feature
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
if(!infoHost.printing) // prevent reset if printing from other source
 {
    resetInfoFile();
    powerFailedSetDriverSource(getCurFileSource());
 }
}
