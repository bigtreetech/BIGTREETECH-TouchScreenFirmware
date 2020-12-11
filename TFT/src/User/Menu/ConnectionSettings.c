#include "ConnectionSettings.h"
#include "includes.h"

const MENUITEMS connectionSettingsItems = {
  // title
  LABEL_CONNECTION_SETTINGS,
  // icon                         label
  {{ICON_BAUD_RATE,               LABEL_BAUDRATE},
    {ICON_DISCONNECT,              LABEL_DISCONNECT},
    {ICON_STOP,                    LABEL_EMERGENCYSTOP},
    {ICON_SHUT_DOWN,               LABEL_SHUT_DOWN},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},}
};

// Set uart pins to input, free uart
void menuDisconnect(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(20, 0, LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO));
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  Serial_ReSourceDeInit();
  while(!isPress()) {
  #ifdef LCD_LED_PWM_CHANNEL
    loopDimTimer();
  #endif
  }
  while(isPress()) {
  #ifdef LCD_LED_PWM_CHANNEL
    loopDimTimer();
  #endif
  }
  Serial_ReSourceInit();

  infoMenu.cur--;
}

const char *const item_baudrate_str[ITEM_BAUDRATE_NUM] = {"2400", "9600", "19200", "38400", "57600", "115200", "250000", "500000", "1000000"};
const u32 item_baudrate[ITEM_BAUDRATE_NUM] = {2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000};

void menuBaudrate(void)
{
  LABEL title = {LABEL_BAUDRATE};
  LISTITEM totalItems[ITEM_BAUDRATE_NUM];
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t cur_item = 0;

  // fill baudrate items
  for(uint8_t i = 0; i < COUNT(totalItems); i++) {
    if (infoSettings.baudrate == item_baudrate[i]) {
      totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    } else {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = (uint8_t *) item_baudrate_str[i];
  }

  listWidgetCreate(title, totalItems, COUNT(totalItems), cur_item / LISTITEM_PER_PAGE);

  while (infoMenu.menu[infoMenu.cur] == menuBaudrate)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_5:
      listWidgetPreviousPage();
      break;

    case KEY_ICON_6:
      listWidgetNextPage();
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default:
      if(key_num < LISTITEM_PER_PAGE){
        uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
        if (tmp_i != cur_item) { // has changed
          totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
          listWidgetRefreshItem(cur_item); // refresh unchecked status
          cur_item = tmp_i;
          totalItems[cur_item].icon = ICONCHAR_CHECKED;
          listWidgetRefreshItem(cur_item); // refresh checked status

          infoSettings.baudrate = item_baudrate[cur_item];
          Serial_ReSourceDeInit(); // Serial_Init() will malloc a dynamic memory, so Serial_DeInit() first to free, then malloc again.
          Serial_ReSourceInit();
          reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT);
        }
      }
      break;
    }

    loopProcess();
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuConnectionSettings(void)
{

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&connectionSettingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuConnectionSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
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
