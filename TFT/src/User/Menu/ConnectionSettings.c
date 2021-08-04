#include "ConnectionSettings.h"
#include "includes.h"

const MENUITEMS connectionSettingsItems = {
  // title
  LABEL_CONNECTION_SETTINGS,
  // icon                          label
  {
    {ICON_BAUD_RATE,               LABEL_BAUDRATE},
    {ICON_DISCONNECT,              LABEL_DISCONNECT},
    {ICON_STOP,                    LABEL_EMERGENCYSTOP},
    {ICON_SHUT_DOWN,               LABEL_SHUT_DOWN},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

// Set uart pins to input, free uart
void menuDisconnect(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(20, 0, LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO));
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT * 2), LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  Serial_ReSourceDeInit();
  while (!isPress())
  {
    #ifdef LCD_LED_PWM_CHANNEL
      LCD_CheckDimming();
    #endif
  }
  while (isPress())
  {
    #ifdef LCD_LED_PWM_CHANNEL
      LCD_CheckDimming();
    #endif
  }
  Serial_ReSourceInit();

  infoMenu.cur--;
}

const char * const item_baudrate_str[BAUDRATE_COUNT] = {
  "2400", "9600", "19200", "38400", "57600", "115200", "250000", "500000", "1000000"};

void menuBaudrate(void)
{
  LABEL title = {LABEL_BAUDRATE};
  LISTITEM totalItems[BAUDRATE_COUNT];
  KEY_VALUES curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t cur_item = 0;

  // fill baudrate items
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (infoSettings.baudrate == i)
    {
      totalItems[i].icon = CHARICON_CHECKED;
      cur_item = i;
    }
    else
    {
      totalItems[i].icon = CHARICON_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = (uint8_t *) item_baudrate_str[i];
  }
  uint16_t curPage = cur_item / LISTITEM_PER_PAGE;

  listViewCreate(title, totalItems, COUNT(totalItems), &curPage, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuBaudrate)
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < COUNT(totalItems) && curIndex != cur_item)
    {  // has changed
      totalItems[cur_item].icon = CHARICON_UNCHECKED;
      listViewRefreshItem(cur_item);  // refresh unchecked status
      cur_item = curIndex;
      totalItems[cur_item].icon = CHARICON_CHECKED;
      listViewRefreshItem(cur_item);  // refresh checked status

      infoSettings.baudrate = cur_item;
      Serial_ReSourceDeInit();  // Serial_Init() will malloc a dynamic memory, so Serial_DeInit() first to free, then malloc again.
      Serial_ReSourceInit();
      reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT);
    }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuConnectionSettings(void)
{
  KEY_VALUES curIndex = KEY_IDLE;

  menuDrawPage(&connectionSettingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuConnectionSettings)
  {
    curIndex = menuKeyGetValue();
    switch (curIndex)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuBaudrate;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;

      case KEY_ICON_2:
        // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
        // it may need to wait for a space to open up in the command queue.
        // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
        Serial_Puts(SERIAL_PORT, "M112\n");
        break;

      case KEY_ICON_3:
        storeCmd("M81\n");
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        break;
    }

    loopProcess();
  }
}
