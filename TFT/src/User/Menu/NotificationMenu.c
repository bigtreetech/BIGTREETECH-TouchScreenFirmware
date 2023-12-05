#include "NotificationMenu.h"
#include "includes.h"

void loadNotificationItems(void)
{
  LISTITEMS * itemlist = getCurListItems();

  for (uint8_t i = 0; i < MAX_MSG_COUNT; i++)
  {
    NOTIFICATION * tempNotify = getNotification(i);

    if (tempNotify != NULL)
    {
      switch (tempNotify->style)
      {
        case DIALOG_TYPE_ERROR:
          itemlist->items[i].icon = CHARICON_ERROR;
          break;

        case DIALOG_TYPE_ALERT:
          itemlist->items[i].icon = CHARICON_ALERT;
          break;

        default:
          itemlist->items[i].icon = CHARICON_INFO;
          break;
      }

      itemlist->items[i].titlelabel.address = tempNotify->text;
    }
    else
    {
      itemlist->items[i].icon = CHARICON_NULL;
    }

    menuDrawListItem(&itemlist->items[i], i);
  }
}

void menuNotification(void)
{
  LISTITEMS notificationItems = {
    LABEL_NOTIFICATIONS,
    // icon            item type   item title     item value text(only for custom value)
    {
      {CHARICON_NULL,  LIST_LABEL, LABEL_DYNAMIC, LABEL_NULL},
      {CHARICON_NULL,  LIST_LABEL, LABEL_DYNAMIC, LABEL_NULL},
      {CHARICON_NULL,  LIST_LABEL, LABEL_DYNAMIC, LABEL_NULL},
      {CHARICON_NULL,  LIST_LABEL, LABEL_DYNAMIC, LABEL_NULL},
      {CHARICON_NULL,  LIST_LABEL, LABEL_DYNAMIC, LABEL_NULL},
      {CHARICON_BLANK, LIST_LABEL, LABEL_CLEAR,   LABEL_NULL},
      #ifdef DEBUG_MONITORING
        {CHARICON_BLANK, LIST_LABEL, LABEL_INFO,    LABEL_NULL},
      #else
        {CHARICON_NULL,  LIST_LABEL, LABEL_NULL,    LABEL_NULL},
      #endif
      {CHARICON_BACK,  LIST_LABEL, LABEL_NULL,    LABEL_NULL},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawListPage(&notificationItems);
  loadNotificationItems();
  setNotificationHandler(loadNotificationItems);

  while (MENU_IS(menuNotification))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
        replayNotification(key_num);
        break;

      case KEY_ICON_5:
        clearNotification();
        loadNotificationItems();
        break;

      #ifdef DEBUG_MONITORING
        case KEY_ICON_6:
          OPEN_MENU(menuMonitoring);
          break;
      #endif

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }

  setNotificationHandler(NULL);
}
