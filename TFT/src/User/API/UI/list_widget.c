#include "list_widget.h"
#include "includes.h"

static LISTITEMS listItems;
static LISTITEM *totalItems;

static uint16_t list_max_items;
static uint8_t list_max_page;
static uint8_t list_cur_page;

void listWidgetCreate(LABEL title, LISTITEM *items, uint16_t max_items, uint8_t cur_page)
{
  listItems.title = title;
  totalItems = items;
  list_max_items = max_items;
  list_max_page = (list_max_items + LISTITEM_PER_PAGE - 1) / LISTITEM_PER_PAGE;

  listWidgetSetCurPage(cur_page);
  menuDrawListPage(&listItems);
}

void listWidgetSetCurPage(uint8_t cur_page)
{
  for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++) {
    if (cur_page * LISTITEM_PER_PAGE + i < list_max_items) {
      listItems.items[i] = totalItems[cur_page * LISTITEM_PER_PAGE + i];
    } else {
      listItems.items[i].icon = ICONCHAR_BACKGROUND;
    }
  }

  // only 1 page or in first page, haven't up button
  listItems.items[5].icon = ((list_max_items <= LISTITEM_PER_PAGE) || (cur_page == 0)) ? ICONCHAR_BACKGROUND : ICONCHAR_PAGEUP;
  // only 1 page or in last page, haven't down button
  listItems.items[6].icon = ((list_max_items <= LISTITEM_PER_PAGE) || (cur_page == list_max_page-1)) ? ICONCHAR_BACKGROUND : ICONCHAR_PAGEDOWN;

  listItems.items[7].icon = ICONCHAR_BACK;

  list_cur_page = cur_page;
}

bool listWidgetNextPage(void)
{
  if (list_max_page <= 1) return false;  // only 0 or 1 page, cann't goto next page
  if (list_cur_page + 1 >= list_max_page) return false;  // already last page

  // goto next page
  list_cur_page++;
  listWidgetSetCurPage(list_cur_page);
  menuRefreshListPage();
  return true;
}

bool listWidgetPreviousPage(void)
{
  if (list_max_page <= 1) return false;  // only 0 or 1 page, cann't goto previous page
  if (list_cur_page == 0) return false;  // already first page

  // goto previous page
  list_cur_page--;
  listWidgetSetCurPage(list_cur_page);
  menuRefreshListPage();
  return true;
}

void listWidgetRefreshPage(void)
{
  listWidgetSetCurPage(list_cur_page);
  menuRefreshListPage();
}

void listWidgetRefreshMenu(void)
{
  listWidgetSetCurPage(list_cur_page);
  menuDrawListPage(&listItems);
}

//
void listWidgetRefreshItem(uint16_t item)
{
  uint8_t cur_i = item % LISTITEM_PER_PAGE;
  if (item > list_max_items) return; // error index.
  if (item < list_cur_page * LISTITEM_PER_PAGE || item >= (list_cur_page + 1) * LISTITEM_PER_PAGE) return; // not in cur page

  if (item < list_max_items) {
    listItems.items[cur_i] = totalItems[item];
  } else {
    listItems.items[cur_i].icon = ICONCHAR_BACKGROUND;
  }
  menuDrawListItem(&listItems.items[cur_i], cur_i);
}

uint8_t listWidgetGetCurPage(void)
{
  return list_cur_page;
}
