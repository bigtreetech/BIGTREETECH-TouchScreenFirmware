#include "RRFMacros.h"
#include "includes.h"

/***
 * TODOS:
 *  proper subdirectory support,
 *  move away from half-using Vfs functions,
 *  sort order: Number_files first in order, hide Number_; alphabetical, directories alphabetical
 ***/

/************************************************************************/

extern const GUI_RECT titleRect;

// Scan files in RRF
void scanInfoFilesFs(void)
{
  clearInfoFile();
  char *data = request_M20_macros(infoFile.title);

  data = strtok(data, "]");  // to end of array

  char *line = strtok(strstr(data, "files\":[") + 8, ",");
  for (; line != NULL; line = strtok(NULL, ","))
  {
    char *pline = line + 1;

    if (strchr(pline, '*') == NULL)
    {
      // FILE
      if (infoFile.fileCount >= FILE_NUM)
        continue;  // gcode max number is FILE_NUM

      char *Pstr_tmp = strrchr(line, '"');
      if (Pstr_tmp != NULL)
        *Pstr_tmp = 0;                // remove right quote
      Pstr_tmp = strrchr(line, '"');  // remove initial quote
      if (Pstr_tmp == NULL)
        Pstr_tmp = line;
      else
        Pstr_tmp++;
      infoFile.Longfile[infoFile.fileCount] = malloc(strlen(Pstr_tmp) + 1);

      strcpy(infoFile.Longfile[infoFile.fileCount], Pstr_tmp);

      infoFile.file[infoFile.fileCount] = malloc(strlen(pline) + 1);
      if (infoFile.file[infoFile.fileCount] == NULL)
        break;
      strcpy(infoFile.file[infoFile.fileCount++], pline);
    }
    else
    {
      // DIRECTORY
      if (infoFile.folderCount >= FOLDER_NUM)
        continue;  // folder max number is FOLDER_NUM

      char *rest = pline + 1;
      char *folder = strtok_r(rest, "\"", &rest);

      bool found = false;
      for (int i = 0; i < infoFile.folderCount; i++)
      {
        if (strcmp(folder, infoFile.folder[i]) == 0)
        {
          found = true;
          break;
        }
      }

      if (!found)
      {
        uint16_t len = strlen(folder) + 1;
        infoFile.folder[infoFile.folderCount] = malloc(len);
        if (infoFile.folder[infoFile.folderCount] == NULL)
          break;
        strcpy(infoFile.folder[infoFile.folderCount++], folder);
      }
    }
  }

  clearRequestCommandInfo();
}

void runMacro(void)
{
  char info[100];
  sprintf(info, "%s - %s\n", textSelect(LABEL_MACROS), infoFile.title);
  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t *)info);

  request_M98(infoFile.title);

  ExitDir();
  Delay_ms(500);
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

    if (infoFile.source == BOARD_SD)
      setDynamicLabel(itemPos, infoFile.Longfile[index - infoFile.folderCount]);
    else
      setDynamicLabel(itemPos, infoFile.file[index - infoFile.folderCount]);
  }
}

// View and run macros stored in RRF firmware
void menuCallMacro(void)
{
  uint16_t key_num = KEY_IDLE;
  uint8_t update = 1;
  infoFile.source = BOARD_SD;

  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING));

  scanInfoFilesFs();

  while (infoMenu.menu[infoMenu.cur] == menuCallMacro)
  {
    GUI_SetBkColor(infoSettings.title_bg_color);
    Scroll_DispString(&scrollLine, LEFT);
    GUI_SetBkColor(infoSettings.bg_color);

    key_num = listViewGetSelectedIndex();

    switch (key_num)
    {
      case KEY_BACK:
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
            if (EnterDir(infoFile.folder[key_num]) == false)
              break;
            scanInfoFilesFs();
            update = 1;
            infoFile.cur_page = 0;
          }
          else if (key_num < infoFile.fileCount + infoFile.folderCount)  // gcode
          {
            if (infoHost.connected != true)
              break;

            if (EnterDir(infoFile.file[key_num - infoFile.folderCount]) == false)
              break;

            char buf[93];
            sprintf(buf, "Do you want to start:\n %.65s?\n", infoFile.title);
            setDialogText(LABEL_INFO, (uint8_t *)buf, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_QUESTION, runMacro, ExitDir, NULL);
          }
        }
        break;
    }

    if (update)
    {
      update = 0;

      listViewCreate((LABEL){.address = (uint8_t *)infoFile.title}, NULL, infoFile.folderCount + infoFile.fileCount,
                       &infoFile.cur_page, false, NULL, macroListDraw);

      // set scrolling title text
      Scroll_CreatePara(&scrollLine, (uint8_t *)infoFile.title, &titleRect);
      GUI_SetBkColor(infoSettings.title_bg_color);
      GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
      GUI_SetBkColor(infoSettings.bg_color);
    }

    loopProcess();
  }
}

