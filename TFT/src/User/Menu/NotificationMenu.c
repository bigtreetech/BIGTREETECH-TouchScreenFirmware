#include "NotificationMenu.h"
#include "includes.h"

void loadNotificationItems(void)
{
  LISTITEMS * itemlist = getCurListItems();
  uint8_t n = 0;

  for (uint8_t i = 0; i < MAX_MSG_COUNT; i++)
  {
    NOTIFICATION * tempNotify = getNotification(i);
    if (tempNotify != NULL)
    {
      switch (tempNotify->style)
      {
        case DIALOG_TYPE_ERROR:
          itemlist->items[i].icon = ICONCHAR_ERROR;
          break;
        case DIALOG_TYPE_ALERT:
          itemlist->items[i].icon = ICONCHAR_ALERT;
          break;
        default:
          itemlist->items[i].icon = ICONCHAR_INFO;
          break;
      }
      itemlist->items[i].titlelabel.address = tempNotify->text;
      n++;
    }
    else
    {
      itemlist->items[i].icon = ICONCHAR_BACKGROUND;
    }
    menuDrawListItem(&itemlist->items[i], i);
  }
  //return n;
}

void menuNotification(void)
{
  LISTITEMS notificationItems = {
    LABEL_NOTIFICATIONS,
    // icon                 ItemType    Item Title        item value text(only for custom value)
    {
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_DYNAMIC,    LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_DYNAMIC,    LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_DYNAMIC,    LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_DYNAMIC,    LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_DYNAMIC,    LABEL_BACKGROUND},
      {ICONCHAR_BLANK,      LIST_LABEL, LABEL_CLEAR,      LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACK,       LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  menuDrawListPage(&notificationItems);

  loadNotificationItems();
  setNotificationHandler(loadNotificationItems);

  while (infoMenu.menu[infoMenu.cur] == menuNotification)
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
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default:
        break;
    }
    loopProcess();
  }
  setNotificationHandler(NULL);
}
