#include "RRFMacros.h"
#include "includes.h"

/***
 * TODOS:
 *  proper subdirectory support,
 *  move away from half-using Vfs functions,
 *  sort order: Number_files first in order, hide Number_; alphabetical, directories alphabetical
 ***/

/************************************************************************/

LISTITEMS * macroListItems;
const GUI_RECT macroTitleRect = {10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH - 10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};
const int16_t labelMacroError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

//Scan files in RRF
bool scaninfoFilesFs(void)
{
  clearInfoFile();
  strcpy(infoFile.title, "/");
  char *ret = request_M20_macros(infoFile.title);
  if (strlen(ret) <= 3)
    return false;

  char *data = malloc(strlen(ret) + 1);
  strcpy(data, ret);
  clearRequestCommandInfo();
  char s[3];

  strcpy(s, ","); // filenames containing "," will break

  data = strtok(data, "]"); // to end of Array

  char *line = strtok(strstr(data, "files\":[") + 8, s);
  for (; line != NULL; line = strtok(NULL, s))
  {
    char *pline = line + 1;

    if (strchr(pline, '*') == NULL)
    {
      // FILE
      if (infoFile.folderCount >= FILE_NUM)
        continue; // Gcode max number is FILE_NUM

      char *Pstr_tmp = strrchr(line, '"');
      if (Pstr_tmp != NULL)
        *Pstr_tmp = 0;               //remove right quote
      Pstr_tmp = strrchr(line, '"'); //remove initial quote
      if (Pstr_tmp == NULL)
        Pstr_tmp = line;
      else
        Pstr_tmp++;
      infoFile.Longfile[infoFile.folderCount] = malloc(strlen(Pstr_tmp) + 1);

      strcpy(infoFile.Longfile[infoFile.folderCount], Pstr_tmp);

      infoFile.file[infoFile.folderCount] = malloc(strlen(pline) + 1);
      if (infoFile.file[infoFile.folderCount] == NULL)
        break;
      strcpy(infoFile.file[infoFile.folderCount++], pline);
    }
    else
    {
      // DIRECTORY
      if (infoFile.folderCount >= FOLDER_NUM)
        continue; // floder max number is FOLDER_NUM

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
  free(data);
  return true;
}

void runMacro(void)
{
  char info[100];
  sprintf(info, "%s - %s\n", textSelect(LABEL_MACROS), infoFile.title);
  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t *)info);

  request_M98(infoFile.title);
  Delay_ms(500);
}

// Run selected macro
void menuExecMacro(void)
{
  char buf[89];

  sprintf(buf, "Do you want to start %.65s?\n", infoFile.title);
  setDialogText(textSelect(LABEL_INFO), (uint8_t *)buf, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));
  showDialog(DIALOG_TYPE_QUESTION, runMacro, ExitDir, NULL);

  infoMenu.cur--;
}

//Draw Macro file list
void macroListDraw(void)
{
  uint8_t i = 0;

  GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
  GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
  GUI_SetBkColor(BACKGROUND_COLOR);

  for (i = 0; (i + infoFile.cur_page * LISTITEM_PER_PAGE < infoFile.folderCount) && (i < LISTITEM_PER_PAGE); i++) // folder
  {
    macroListItems->items[i].icon = ICONCHAR_FOLDER;
    setDynamicLabel(i, infoFile.folder[i + infoFile.cur_page * LISTITEM_PER_PAGE]);
    macroListItems->items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&macroListItems->items[i], i);
  }
  for (; (i + infoFile.cur_page * LISTITEM_PER_PAGE < infoFile.folderCount + infoFile.folderCount) && (i < LISTITEM_PER_PAGE); i++) // gcode file
  {
    macroListItems->items[i].icon = ICONCHAR_FILE;
    setDynamicLabel(i, (infoFile.source == BOARD_SD) ? infoFile.Longfile[i + infoFile.cur_page * LISTITEM_PER_PAGE - infoFile.folderCount] : infoFile.file[i + infoFile.cur_page * LISTITEM_PER_PAGE - infoFile.folderCount]);
    macroListItems->items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&macroListItems->items[i], i);
  }

  for (; (i < LISTITEM_PER_PAGE); i++) //background
  {
    macroListItems->items[i].icon = ICONCHAR_BACKGROUND;
    macroListItems->items[i].titlelabel.index = LABEL_BACKGROUND;
    menuDrawListItem(&macroListItems->items[i], i);
  }
  // set page up down button according to page count and current page
  int t_pagenum = (infoFile.folderCount + infoFile.folderCount + (LISTITEM_PER_PAGE - 1)) / LISTITEM_PER_PAGE;
  if ((infoFile.folderCount + infoFile.folderCount) <= LISTITEM_PER_PAGE)
  {
    macroListItems->items[5].icon = ICONCHAR_BACKGROUND;
    macroListItems->items[6].icon = ICONCHAR_BACKGROUND;
  }
  else
  {
    if (infoFile.cur_page == 0)
    {
      macroListItems->items[5].icon = ICONCHAR_BACKGROUND;
      macroListItems->items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if (infoFile.cur_page == (t_pagenum - 1))
    {
      macroListItems->items[5].icon = ICONCHAR_PAGEUP;
      macroListItems->items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      macroListItems->items[5].icon = ICONCHAR_PAGEUP;
      macroListItems->items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
  menuDrawListItem(&macroListItems->items[5], 5);
  menuDrawListItem(&macroListItems->items[6], 6);
}

// View and run macros stored in RRF firmware
void menuCallMacro(void)
{
  LISTITEMS _macroListItems = {
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
      {ICONCHAR_BACK,       LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  uint8_t update = 0;
  char pageTitle[40];
  macroListItems = &_macroListItems;
  infoFile.source = BOARD_SD;

  resetInfoFile();
  sprintf(pageTitle, "<%s>", textSelect(LABEL_MACROS));
  _macroListItems.title.address = (uint8_t *)pageTitle;

  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING));

  if (scaninfoFilesFs() == true)
  {
    menuDrawListPage(&_macroListItems);
    macroListDraw();
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(labelMacroError[infoFile.source]));
    Delay_ms(1000);
    infoMenu.cur--;
  }

  while (infoMenu.menu[infoMenu.cur] == menuCallMacro)
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
        if (infoFile.cur_page + 1 < (infoFile.folderCount + infoFile.folderCount + (LISTITEM_PER_PAGE - 1)) / LISTITEM_PER_PAGE)
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
          scaninfoFilesFs();
          update = 1;
        }
        break;

      case KEY_IDLE:
        break;

      default:
        if (key_num <= KEY_ICON_4)
        {
          u16 start = infoFile.cur_page * LISTITEM_PER_PAGE;
          if (key_num + start < infoFile.folderCount) //folder
          {
            if (EnterDir(infoFile.folder[key_num + start]) == false)
              break;
            scaninfoFilesFs();
            update = 1;
            infoFile.cur_page = 0;
          }
          else if (key_num + start < infoFile.folderCount + infoFile.folderCount) //gcode
          {
            if (infoHost.connected != true)
              break;
            //set file path
            strcpy(infoFile.title, infoFile.file[key_num + start - infoFile.folderCount]);

            infoMenu.menu[++infoMenu.cur] = menuExecMacro;
          }
        }
        break;
    }

    if (update)
    {
      update = 0;
      macroListDraw();
    }

    loopProcess();
  }
}

