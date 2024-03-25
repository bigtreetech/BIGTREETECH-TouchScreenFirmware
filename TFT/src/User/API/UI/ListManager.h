#ifndef _LIST_MANAGER_H_
#define _LIST_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "menu.h"

/**
 * @brief set and innitialize list view menu
 *
 * @param title Title of menu
 * @param items Preset list of items. Set to NULL if not used.
 * @param maxItems Maximum number of items possilbe in current list.
 * @param curPage Display this page index.
 * @param handleBackPress Set true to handle back button automatically.
 * @param preparePage_action Pointer to function to execute for preparing page before display. Set to NULL if not used.
 * @param prepareItem_action Pointer to function to execute for preparing item before display. Set to NULL if not used.
 */
void listViewCreate(LABEL title, LISTITEM * items, uint16_t max_items, uint16_t * cur_page, bool handleBackPress,
                    void (* preparePage_action)(LISTITEMS * listItems, uint8_t pageIndex),
                    void (* prepareItem_action)(LISTITEM * item, uint16_t index, uint8_t itemPos));

void listViewSetTitle(LABEL title);       // set / update list view title
void listViewRefreshPage(void);           // refresh list items
void listViewRefreshMenu(void);           // refresh whole list view menu
void listViewRefreshItem(uint16_t item);  // refresh selected list item
uint8_t listViewGetCurPage(void);         // get current displayed page index
uint16_t listViewGetSelectedIndex(void);  // get index of selected item

#ifdef __cplusplus
}
#endif

#endif
