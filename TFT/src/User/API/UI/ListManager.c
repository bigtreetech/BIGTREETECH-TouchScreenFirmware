#include "ListManager.h"
#include "includes.h"

#define KEY_INDEX_PAGEUP   (LISTITEM_PER_PAGE)
#define KEY_INDEX_PAGEDOWN (KEY_INDEX_PAGEUP + 1)
#define KEY_INDEX_BACK     (KEY_INDEX_PAGEDOWN + 1)

static LISTITEMS listItems;
static LISTITEM *totalItems;

static uint16_t maxItemCount;
static uint8_t maxPageCount;
static uint16_t curPageIndex;
static uint16_t * curPageIndexSource;
static bool handleBack = true;

static void (*action_preparePage)(LISTITEMS * listItems, uint8_t index) = NULL;
static void (*action_prepareItem)(LISTITEM * item, uint16_t index, uint8_t itemPos) = NULL;

/**
 * @brief Set and innitialize list menu
 *
 * @param title Title of menu
 * @param items Preset list of items. Set to NULL if not used.
 * @param maxItems Maximum number of items possilbe in current list.
 * @param curPage Display this page index.
 * @param handleBackPress Set true to handle back button automatically.
 * @param preparePage_action Pointer to function to execute for preparing page before display. Set to NULL if not used.
 * @param prepareItem_action Pointer to function to execute for preparing item before display. Set to NULL if not used.
 */
void listViewCreate(LABEL title, LISTITEM * items, uint16_t maxItems, uint16_t * curPage, bool handleBackPress,
                    void (*preparePage_action)(LISTITEMS * listItems, uint8_t pageIndex),
                    void (*prepareItem_action)(LISTITEM * item, uint16_t index, uint8_t itemPos))
{
  listItems.title = title;
  totalItems = items;
  maxItemCount = maxItems;
  maxPageCount = (maxItemCount + LISTITEM_PER_PAGE - 1) / LISTITEM_PER_PAGE;
  handleBack = handleBackPress;
  action_preparePage = preparePage_action;
  action_prepareItem = prepareItem_action;
  curPageIndexSource = curPage;

  if (curPage != NULL)
    curPageIndex = *curPage;
  else
    curPageIndex = 0;

  listViewSetCurPage(curPageIndex);
  menuDrawListPage(&listItems);
}

// Set/Update List view title
void listViewSetTitle(LABEL title)
{
  listItems.title = title;
  menuDrawTitle(labelGetAddress(&listItems.title));
}

// Get current displayed pade index
uint8_t listViewGetCurPage(void)
{
  return curPageIndex;
}

// display page at selected index
void listViewSetCurPage(uint8_t curPage)
{
  if (action_preparePage != NULL)
  {
    action_preparePage(&listItems, curPage);
  }
  else
  {
    for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++)
    {
      uint16_t index = curPage * LISTITEM_PER_PAGE + i;

      if (index < maxItemCount)
      {
        if (totalItems != NULL)
          listItems.items[i] = totalItems[index];

        if (action_prepareItem != NULL)
          action_prepareItem(&listItems.items[i], index, i);
      }
      else
      {
        listItems.items[i].icon = CHARICON_NULL;
      }
    }
  }

  // only 1 page or in first page, hide up button
  listItems.items[5].icon = ((maxItemCount <= LISTITEM_PER_PAGE) || (curPage == 0)) ? CHARICON_NULL : CHARICON_PAGEUP;
  // only 1 page or in last page, hide down button
  listItems.items[6].icon = ((maxItemCount <= LISTITEM_PER_PAGE) || (curPage == maxPageCount - 1)) ? CHARICON_NULL : CHARICON_PAGEDOWN;

  listItems.items[7].icon = CHARICON_BACK;

  curPageIndex = curPage;
}

// open next page
bool listViewNextPage(void)
{
  if (maxPageCount <= 1) return false;                 // only 0 or 1 page, can't goto next page
  if (curPageIndex + 1 >= maxPageCount) return false;  // already last page

  // goto next page
  curPageIndex++;

  if (curPageIndexSource != NULL)
    *curPageIndexSource = curPageIndex;

  listViewSetCurPage(curPageIndex);
  menuRefreshListPage();
  return true;
}

// open previous page
bool listViewPreviousPage(void)
{
  if (maxPageCount <= 1) return false;  // only 0 or 1 page, can't goto previous page
  if (curPageIndex == 0) return false;  // already first page

  // goto previous page
  curPageIndex--;

  if (curPageIndexSource != NULL)
    *curPageIndexSource = curPageIndex;

  listViewSetCurPage(curPageIndex);
  menuRefreshListPage();
  return true;
}

// refresh list items
void listViewRefreshPage(void)
{
  menuRefreshListPage();
}

// refresh whole list menu
void listViewRefreshMenu(void)
{
  menuDrawListPage(&listItems);
}

// refresh selected list item
void listViewRefreshItem(uint16_t item)
{
  uint8_t cur_i = item % LISTITEM_PER_PAGE;

  if (item > maxItemCount)
    return;  // error index.
  if (item < curPageIndex * LISTITEM_PER_PAGE || item >= (curPageIndex + 1) * LISTITEM_PER_PAGE)
    return;  // not in cur page

  if (item < maxItemCount)
  {
    listItems.items[cur_i] = totalItems[item];

    if (action_prepareItem != NULL)
      action_prepareItem(&listItems.items[cur_i], item, cur_i);
  }
  else
  {
    listItems.items[cur_i].icon = CHARICON_NULL;
  }
  menuDrawListItem(&listItems.items[cur_i], cur_i);
}

// get index of selected item
uint16_t listViewGetSelectedIndex(void)
{
  KEY_VALUES key_num = menuKeyGetValue();

  if (key_num < LISTITEM_PER_PAGE)
  {
    // return actual item index
    uint16_t cur_index = curPageIndex * LISTITEM_PER_PAGE + key_num;

    if (cur_index < maxItemCount)
    {
      if (totalItems[cur_index].icon != CHARICON_NULL)
        return cur_index;
      else
        return KEY_IDLE;
    }
  }

  // check function keypress
  switch (key_num)
  {
    case KEY_DECREASE:
    case KEY_INDEX_PAGEUP:
      listViewPreviousPage();
      return KEY_PAGEUP;

    case KEY_INCREASE:
    case KEY_INDEX_PAGEDOWN:
      listViewNextPage();
      return KEY_PAGEDOWN;

    case KEY_INDEX_BACK:
      if (handleBack)
        CLOSE_MENU();
      return KEY_BACK;

    default:
      return KEY_IDLE;  // if no key is pressed and no page scrolling is requested
  }
}
