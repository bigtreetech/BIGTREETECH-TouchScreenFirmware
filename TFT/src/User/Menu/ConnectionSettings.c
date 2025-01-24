#include "ConnectionSettings.h"
#include "includes.h"

static SERIAL_PORT_INDEX portIndex = 0;  // index on serialPort array

static void updateListeningMode(MENUITEMS * menu)
{
  menu->items[4].label.index = (GET_BIT(infoSettings.general_settings, INDEX_LISTENING_MODE) == 1) ? LABEL_OFF : LABEL_ON;

  InfoHost_UpdateListeningMode();  // update listening mode
}

// disconnect (free uart), wait for a key press and finally connect again (set uart pins to input)
static void refreshConnection(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(20, 0, LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO));
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT * 2), LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  Serial_DeInit(ALL_PORTS);

  while (!TS_IsPressed())
  {
    #ifdef LCD_LED_PWM_CHANNEL
      LCD_CheckDimming();
    #endif
  }

  BUZZER_PLAY(SOUND_KEYPRESS);

  while (TS_IsPressed())
  {
    #ifdef LCD_LED_PWM_CHANNEL
      LCD_CheckDimming();
    #endif
  }

  Serial_Init(ALL_PORTS);
}

static void menuBaudrate(void)
{
  LABEL title = {LABEL_BAUDRATE};
  uint8_t minIndex = portIndex == PORT_1 ? 1 : 0;  // if primary serial port, set minIndex to 1 (value OFF is skipped)
  uint8_t size = BAUDRATE_COUNT - minIndex;
  LISTITEM totalItems[size];
  KEY_VALUES curIndex = KEY_IDLE;
  uint8_t curItem = 0;
  uint16_t curPage;

  // fill baudrate items
  for (uint8_t i = 0; i < size; i++)
  {
    if (infoSettings.serial_port[portIndex] == i + minIndex)
    {
      totalItems[i].icon = CHARICON_CHECKED;
      curItem = i;
    }
    else
    {
      totalItems[i].icon = CHARICON_UNCHECKED;
    }

    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = (uint8_t *) baudrateNames[i + minIndex];
  }

  curPage = curItem / LISTITEM_PER_PAGE;

  listViewCreate(title, totalItems, size, &curPage, true, NULL, NULL);

  while (MENU_IS(menuBaudrate))
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < size && curIndex != curItem)  // if changed
    {
      totalItems[curItem].icon = CHARICON_UNCHECKED;

      listViewRefreshItem(curItem);  // refresh unchecked status

      curItem = curIndex;
      totalItems[curItem].icon = CHARICON_CHECKED;

      listViewRefreshItem(curItem);  // refresh checked status

      infoSettings.serial_port[portIndex] = curItem + minIndex;
      Serial_DeInit(portIndex);
      Serial_Init(portIndex);
    }

    loopProcess();
  }

  saveSettings();  // save settings
}

static void menuSerialPorts(void)
{
  LABEL title = {LABEL_SERIAL_PORTS};
  LISTITEM totalItems[SERIAL_PORT_COUNT];
  KEY_VALUES curIndex = KEY_IDLE;

  for (SERIAL_PORT_INDEX i = PORT_1; i < SERIAL_PORT_COUNT; i++)
  {
    totalItems[i].icon = CHARICON_EDIT;
    totalItems[i].itemType = LIST_CUSTOMVALUE;
    totalItems[i].titlelabel.address = (uint8_t *) serialPort[i].desc;
    totalItems[i].valueLabel.index = LABEL_DYNAMIC;  // must be LABEL_DYNAMIC or LABEL_CUSTOM_VALUE in order to use dynamic text
    setDynamicTextValue(i, (char *) baudrateNames[infoSettings.serial_port[i]]);
  }

  listViewCreate(title, totalItems, SERIAL_PORT_COUNT, NULL, true, NULL, NULL);

  while (MENU_IS(menuSerialPorts))
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < (KEY_VALUES)SERIAL_PORT_COUNT)
    {
      portIndex = (SERIAL_PORT_INDEX)curIndex;

      OPEN_MENU(menuBaudrate);
    }

    loopProcess();
  }
}

void menuConnectionSettings(void)
{
  MENUITEMS connectionSettingsItems = {
    // title
    LABEL_CONNECTION_SETTINGS,
    // icon                          label
    {
      {ICON_BAUD_RATE,               LABEL_SERIAL_PORTS},
      {ICON_DISCONNECT,              LABEL_DISCONNECT},
      {ICON_STOP,                    LABEL_EMERGENCYSTOP},
      {ICON_SHUT_DOWN,               LABEL_SHUT_DOWN},
      {ICON_BAUD_RATE,               LABEL_ON},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES curIndex = KEY_IDLE;

  updateListeningMode(&connectionSettingsItems);

  menuDrawPage(&connectionSettingsItems);

  while (MENU_IS(menuConnectionSettings))
  {
    curIndex = menuKeyGetValue();

    switch (curIndex)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuSerialPorts);
        break;

      case KEY_ICON_1:
        refreshConnection();
        menuDrawPage(&connectionSettingsItems);
        break;

      case KEY_ICON_2:
        // Emergency Stop: Used for emergency stopping, a reset is required to return to operational mode.
        // It may need to wait for a space to open up in the command queue.
        // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command
        sendEmergencyCmd("M112\n");
        break;

      case KEY_ICON_3:
        storeCmd("M81\n");
        break;

      case KEY_ICON_4:
        TOGGLE_BIT(infoSettings.general_settings, INDEX_LISTENING_MODE);
        storePara();

        updateListeningMode(&connectionSettingsItems);

        menuDrawItem(&connectionSettingsItems.items[4], 4);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
