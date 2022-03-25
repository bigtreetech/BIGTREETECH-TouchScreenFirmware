#include "RRFMacros.h"
#include "includes.h"
#include "RRFM20Parser.hpp"

static const char *running_macro_name;
extern const GUI_RECT titleRect;

// Scan files in RRF
void scanInfoFilesFs(void)
{
  clearInfoFile();
  request_M20_rrf(infoFile.path, false, parseMacroListResponse);
}

void rrfShowRunningMacro(void)
{
  GUI_Clear(MENU_BACKGROUND_COLOR);
  GUI_SetColor(infoSettings.reminder_color);
  GUI_DispStringInPrectEOL(&titleRect, LABEL_BUSY);
  GUI_RestoreColorDefault();
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t *)running_macro_name);
}

void runMacro(const char *display_name)
{
  running_macro_name = display_name;
  rrfShowRunningMacro();

  request_M98(infoFile.path);

  exitFolder();
}

// Draw Macro file list
// update items in list mode
void macroListDraw(LISTITEM * item, uint16_t index, uint8_t itemPos)
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
    item->titlelabel.index = LABEL_DYNAMIC;
    item->itemType = LIST_LABEL;

    setDynamicLabel(itemPos, infoFile.file[index - infoFile.folderCount]);
  }
}

// View and run macros stored in RRF firmware
void menuCallMacro(void)
{
  uint16_t key_num = KEY_IDLE;
  uint8_t update = 1;
  infoFile.curPage = 0;
  infoFile.source = FS_ONBOARD_MEDIA;

  GUI_Clear(MENU_BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING));

  scanInfoFilesFs();

  while (MENU_IS(menuCallMacro))
  {
    GUI_SetBkColor(infoSettings.title_bg_color);
    Scroll_DispString(&scrollLine, LEFT);
    GUI_SetBkColor(infoSettings.bg_color);

    key_num = listViewGetSelectedIndex();

    switch (key_num)
    {
      case KEY_BACK:
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
          scanInfoFilesFs();
          update = 1;
        }
        break;

      case KEY_IDLE:
        break;

      default:
        if (key_num <= infoFile.fileCount + infoFile.folderCount)
        {
          if (key_num < infoFile.folderCount)  // folder
          {
            if (enterFolder(infoFile.folder[key_num]) == false)
              break;
            scanInfoFilesFs();
            update = 1;
            infoFile.curPage = 0;
          }
          else if (key_num < infoFile.fileCount + infoFile.folderCount)  // gcode
          {
            if (infoHost.connected != true)
              break;

            if (enterFolder(infoFile.longFile[key_num - infoFile.folderCount]) == false)
              break;

            runMacro(infoFile.file[key_num - infoFile.folderCount]);
            update = 1;
          }
        }
        break;
    }

    if (update)
    {
      update = 0;

      listViewCreate((LABEL){.address = (uint8_t *)infoFile.path}, NULL, infoFile.folderCount + infoFile.fileCount,
                     &infoFile.curPage, false, NULL, macroListDraw);

      // set scrolling title text
      Scroll_CreatePara(&scrollLine, (uint8_t *)infoFile.path, &titleRect);
      GUI_SetBkColor(infoSettings.title_bg_color);
      GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
      GUI_SetBkColor(infoSettings.bg_color);
    }

    loopProcess();
  }
}
