#include "Macros.h"
#include "includes.h"

#define FOLDER_NUM 255
#define FILE_NUM 255
#define MAX_PATH_LEN 1024

typedef struct
{
  TCHAR title[MAX_PATH_LEN]; // max path depth
  TCHAR *folder[FOLDER_NUM]; // folder name buffer
  TCHAR *file[FILE_NUM];     // file name buffer
  uint16_t F_num;            // current folder number
  uint16_t f_num;            // current gcode file number
  uint16_t cur_page;         // current display page index (5 files per page)
  FS_SOURCE source;          //The source of the file. TFT SD or ONBOARD SD.
  TCHAR *Longfile[FILE_NUM]; // Long file name buffer from ONBOARD SD only
  TCHAR pageTitle[40];       // max path depth
} MACROFILE;

extern MACROFILE macroFile;

MACROFILE macroFile = {"?:", {0}, {0}, 0, 0, 0, BOARD_SD, {0}, "<>"};
LISTITEMS macroListItems = {
    // title
    LABEL_BACKGROUND,
    // icon                 ItemType      Item Title        item value text(only for custom value)
    {
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACK, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    }};

// File list number per page
#define NUM_PER_PAGE 5
SCROLL macroTitleScroll;
const GUI_RECT macroTitleRect = {10, (TITLE_END_Y - BYTE_HEIGHT) / 2, LCD_WIDTH - 10, (TITLE_END_Y - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};
const int16_t labelMacroError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};
/****/

/*** 
 * TODOS:
 *  proper subdirectory support,
 *  move away from half-using Vfs functions,
 *  sort order: Number_files first in order, hide Number_; alphabetical, directories alphabetical
 ***/

/************************************************************************/

/*
*/
bool setMacroFile(char *nextdir)
{
  strcpy(macroFile.title, nextdir);
  return 1;
}

/*
*/
bool EnterMacroDir(char *nextdir)
{
  if (strlen(macroFile.title) + strlen(nextdir) + 2 >= MAX_PATH_LEN)
    return 0;
  strcat(macroFile.title, "/");
  strcat(macroFile.title, nextdir);
  return 1;
}
/*
*/
bool IsMacroRootDir(void)
{
  return !strchr(macroFile.title, '/');
}
/*
*/
void clearMacroFile(void)
{
  uint8_t i = 0;
  for (i = 0; i < macroFile.F_num; i++)
  {
    free(macroFile.folder[i]);
    macroFile.folder[i] = 0;
  }
  for (i = 0; i < macroFile.f_num; i++)
  {
    free(macroFile.file[i]);
    macroFile.file[i] = 0;
    free(macroFile.Longfile[i]);
    macroFile.Longfile[i] = 0;
  }
  macroFile.F_num = 0;
  macroFile.f_num = 0;
}
/*
*/
void resetMacroFile(void)
{
  FS_SOURCE source = macroFile.source;
  clearMacroFile();
  memset(&macroFile, 0, sizeof(macroFile));
  macroFile.source = source;

  strcpy(macroFile.title, "/");
}

bool scanMacroFilesFs(void)
{
  //
  clearMacroFile();
  //
  strcpy(macroFile.title, "/");
  char *ret = request_M20_macros(macroFile.title);
  if (strlen(ret) <= 3)
    return false;
  char *data = malloc(strlen(ret) + 1);
  strcpy(data, ret);

  clearRequestCommandInfo();
  char s[3];

  strcpy(s, ","); // filenames containing "," will break

  data = strtok(data, "]"); //  to end of Array

  char *line = strtok(strstr(data, "files\":[") + 8, s);
  for (; line != NULL; line = strtok(NULL, s))
  {
    char *pline = line + 1;

    if (strchr(pline, '*') == NULL)
    {
      // FILE
      if (macroFile.f_num >= FILE_NUM)
        continue; /* Gcode max number is FILE_NUM*/

      char *Pstr_tmp = strrchr(line, '"');
      if (Pstr_tmp != NULL)
        *Pstr_tmp = 0;               //remove правую ковычку
      Pstr_tmp = strrchr(line, '"'); //remove начальная ковычка
      if (Pstr_tmp == NULL)
        Pstr_tmp = line;
      else
        Pstr_tmp++;
      macroFile.Longfile[macroFile.f_num] = malloc(strlen(Pstr_tmp) + 1);

      strcpy(macroFile.Longfile[macroFile.f_num], Pstr_tmp);

      macroFile.file[macroFile.f_num] = malloc(strlen(pline) + 1);
      if (macroFile.file[macroFile.f_num] == NULL)
        break;
      strcpy(macroFile.file[macroFile.f_num++], pline);
    }
    else
    {
      // DIRECTORY
      if (macroFile.F_num >= FOLDER_NUM)
        continue; /* floder max number is FOLDER_NUM */

      char *rest = pline + 1;
      char *folder = strtok_r(rest, "\"", &rest);

      bool found = false;
      for (int i = 0; i < macroFile.F_num; i++)
      {
        if (strcmp(folder, macroFile.folder[i]) == 0)
        {
          found = true;
          break;
        }
      }

      if (!found)
      {
        uint16_t len = strlen(folder) + 1;
        macroFile.folder[macroFile.F_num] = malloc(len);
        if (macroFile.folder[macroFile.F_num] == NULL)
          break;
        strcpy(macroFile.folder[macroFile.F_num++], folder);
      }
    }
  }
  free(data);
  return true;
}

void menuStartMacro(void)
{
  char info[100];
  sprintf(info, "%s - %s\n", textSelect(LABEL_MACROS), macroFile.title);
  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint8_t *)info);

  request_M98(macroFile.title);
  Delay_ms(500);
  infoMenu.cur--;

  //  loopProcess();
}

void menuExecMacro(void)
{
  u16 key_num = IDLE_TOUCH;

  char buf[89];
  sprintf(buf, "Do you want to start %.65s?\n", macroFile.title);

  popupDrawPage(DIALOG_TYPE_QUESTION, bottomDoubleBtn, textSelect(LABEL_INFO), (u8 *)buf, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

  while (infoMenu.menu[infoMenu.cur] == menuExecMacro)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch (key_num)
    {
    case KEY_POPUP_CONFIRM:
      infoMenu.menu[infoMenu.cur] = menuStartMacro;
      break;

    case KEY_POPUP_CANCEL:
      ExitDir();
      infoMenu.cur--;
      break;
    }
    loopProcess();
  }
}
/*
*/
void drawTitle(void)
{
  // menuDrawTitle(labelGetAddress(&listItems->title));
}
/*
*/
void menuCallMacro(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  u8 update = 0;
  //strcpy(macroFile.pageTitle, textSelect(LABEL_MACROS));
  sprintf(macroFile.pageTitle, "<%s>", textSelect(LABEL_MACROS));

  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(LABEL_LOADING));
  if (scanMacroFilesFs() == true)
  {
    menuDrawListPage(&macroListItems);
    macroListDraw();
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH, LCD_HEIGHT, textSelect(labelMacroError[macroFile.source]));
    Delay_ms(1000);
    infoMenu.cur--;
  }

  while (infoMenu.menu[infoMenu.cur] == menuCallMacro)
  {
    GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
    Scroll_DispString(&macroTitleScroll, LEFT); //
    GUI_SetBkColor(BACKGROUND_COLOR);

    key_num = menuKeyGetValue();

    switch (key_num)
    {
    case KEY_ICON_5:
      if (macroFile.cur_page > 0)
      {
        macroFile.cur_page--;
        update = 1;
      }
      break;

    case KEY_ICON_6:
      if (macroFile.cur_page + 1 < (macroFile.F_num + macroFile.f_num + (NUM_PER_PAGE - 1)) / NUM_PER_PAGE)
      {
        macroFile.cur_page++;
        update = 1;
      }
      break;

    case KEY_ICON_7:
      macroFile.cur_page = 0;
      if (IsRootDir() == true)
      {
        clearMacroFile();
        infoMenu.cur--;
        break;
      }
      else
      {
        ExitDir();
        scanMacroFilesFs();
        update = 1;
      }
      break;

    case KEY_IDLE:
      break;

    default:
      if (key_num <= KEY_ICON_4)
      {
        u16 start = macroFile.cur_page * NUM_PER_PAGE;
        if (key_num + start < macroFile.F_num) //folder
        {
          if (EnterMacroDir(macroFile.folder[key_num + start]) == false)
            break;
          scanMacroFilesFs();
          update = 1;
          macroFile.cur_page = 0;
        }
        else if (key_num + start < macroFile.F_num + macroFile.f_num) //gcode
        {
          if (infoHost.connected != true)
            break;
          if (setMacroFile(macroFile.file[key_num + start - macroFile.F_num]) == false)
            break;

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

#ifdef SD_CD_PIN
    if (isVolumeExist(macroFile.source) != true)
    {
      resetMacroFile();
      infoMenu.cur--;
    }
#endif
    loopProcess();
  }
}

void macroListDraw(void)
{
  u8 i = 0;
  /*
  Scroll_CreatePara(&macroTitleScroll, (u8 *)macroFile.title, &macroTitleRect);
  macroListItems.title.address = (u8 *)macroFile.title;
  */
  Scroll_CreatePara(&macroTitleScroll, (u8 *)macroFile.pageTitle, &macroTitleRect);
  macroListItems.title.address = (u8 *)macroFile.pageTitle;
  GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
  GUI_ClearRect(0, 0, LCD_WIDTH, TITLE_END_Y);
  GUI_SetBkColor(BACKGROUND_COLOR);

  for (i = 0; (i + macroFile.cur_page * NUM_PER_PAGE < macroFile.F_num) && (i < NUM_PER_PAGE); i++) // folder
  {
    macroListItems.items[i].icon = ICONCHAR_FOLDER;
    setDynamicLabel(i, macroFile.folder[i + macroFile.cur_page * NUM_PER_PAGE]);
    macroListItems.items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&macroListItems.items[i], i);
  }
  for (; (i + macroFile.cur_page * NUM_PER_PAGE < macroFile.f_num + macroFile.F_num) && (i < NUM_PER_PAGE); i++) // gcode file
  {
    macroListItems.items[i].icon = ICONCHAR_FILE;
    setDynamicLabel(i, (macroFile.source == BOARD_SD) ? macroFile.Longfile[i + macroFile.cur_page * NUM_PER_PAGE - macroFile.F_num] : macroFile.file[i + macroFile.cur_page * NUM_PER_PAGE - macroFile.F_num]);
    macroListItems.items[i].titlelabel.index = LABEL_DYNAMIC;
    menuDrawListItem(&macroListItems.items[i], i);
  }

  for (; (i < NUM_PER_PAGE); i++) //background
  {
    macroListItems.items[i].icon = ICONCHAR_BACKGROUND;
    macroListItems.items[i].titlelabel.index = LABEL_BACKGROUND;
    menuDrawListItem(&macroListItems.items[i], i);
  }
  // set page up down button according to page count and current page
  int t_pagenum = (macroFile.F_num + macroFile.f_num + (LISTITEM_PER_PAGE - 1)) / LISTITEM_PER_PAGE;
  if ((macroFile.F_num + macroFile.f_num) <= LISTITEM_PER_PAGE)
  {
    macroListItems.items[5].icon = ICONCHAR_BACKGROUND;
    macroListItems.items[6].icon = ICONCHAR_BACKGROUND;
  }
  else
  {
    if (macroFile.cur_page == 0)
    {
      macroListItems.items[5].icon = ICONCHAR_BACKGROUND;
      macroListItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if (macroFile.cur_page == (t_pagenum - 1))
    {
      macroListItems.items[5].icon = ICONCHAR_PAGEUP;
      macroListItems.items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      macroListItems.items[5].icon = ICONCHAR_PAGEUP;
      macroListItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
  menuDrawListItem(&macroListItems.items[5], 5);
  menuDrawListItem(&macroListItems.items[6], 6);
}
