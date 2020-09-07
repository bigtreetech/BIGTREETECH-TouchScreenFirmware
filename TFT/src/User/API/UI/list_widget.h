#ifndef _LIST_WIDGET_H_
#define _LIST_WIDGET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

void listWidgetCreate(LABEL title, LISTITEM *items, uint16_t max_items, uint8_t cur_page);
void listWidgetSetCurPage(uint8_t cur_page);
bool listWidgetPreviousPage(void);
bool listWidgetNextPage(void);
void listWidgetRefreshPage(void);
void listWidgetRefreshMenu(void);
void listWidgetRefreshItem(uint16_t item);
uint8_t listWidgetGetCurPage(void);

#ifdef __cplusplus
}
#endif

#endif
