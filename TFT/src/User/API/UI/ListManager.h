#ifndef _LIST_MANAGER_H_
#define _LIST_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "menu.h"

void listViewCreate(LABEL title, LISTITEM * items, uint16_t max_items, uint16_t * cur_page, bool handleBackPress,
                    void (*preparePage_action)(LISTITEMS * listItems, uint8_t pageIndex),
                    void (*prepareItem_action)(LISTITEM * item, uint16_t index, uint8_t itemPos));

void listViewSetTitle(LABEL title);
void listViewSetCurPage(uint8_t cur_page);
bool listViewPreviousPage(void);
bool listViewNextPage(void);
void listViewRefreshPage(void);
void listViewRefreshMenu(void);
void listViewRefreshItem(uint16_t item);
uint8_t listViewGetCurPage(void);
uint16_t listViewGetSelectedIndex(void);

#ifdef __cplusplus
}
#endif

#endif
