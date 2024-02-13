#include "FeatureSettings.h"
#include "includes.h"

static uint16_t fe_cur_page = 0;

// parameter values

#define ITEM_TOGGLE_AUTO_NUM 3
const LABEL itemToggleAuto[ITEM_TOGGLE_AUTO_NUM] =
{
  LABEL_OFF,
  LABEL_ON,
  LABEL_AUTO
};

#define ITEM_TOGGLE_SMART_NUM 2
const LABEL itemToggleSmart[ITEM_TOGGLE_SMART_NUM] =
{
  LABEL_ON,
  LABEL_SMART
};

// add key number index of the items
typedef enum
{
  SKEY_ADVANCED_OK = 0,
  SKEY_COMMAND_CHECKSUM,
  SKEY_EMULATED_M600,
  SKEY_EMULATED_M109_M190,
  SKEY_EVENT_LED,
  SKEY_FILE_COMMENT_PARSING,
  SKEY_SERIAL_ALWAYS_ON,
  SKEY_SPEED,
  SKEY_AUTO_LOAD_LEVELING,
  SKEY_PROBING_Z_OFFSET,
  SKEY_Z_STEPPERS_ALIGNMENT,

  #ifdef PS_ON_PIN
    SKEY_PS_AUTO_SHUTDOWN,
  #endif

  #ifdef FIL_RUNOUT_PIN
    SKEY_FIL_RUNOUT,
  #endif

  SKEY_PL_RECOVERY,
  SKEY_PL_RECOVERY_HOME,
  SKEY_BTT_MINI_UPS,
  SKEY_START_GCODE_ENABLED,
  SKEY_END_GCODE_ENABLED,
  SKEY_CANCEL_GCODE_ENABLED,
  SKEY_RESET_SETTINGS,        // keep reset always at the bottom of the settings menu list
  SKEY_COUNT                  // keep this always at the end
} SKEY_LIST;

void resetSettings(void)
{
  initSettings();
  storePara();
  popupReminder(DIALOG_TYPE_SUCCESS, LABEL_INFO, LABEL_SETTINGS_RESET_DONE);
}

// perform action on button press
static inline void updateFeatureSettings(uint8_t item_index)
{
  switch (item_index)
  {
    case SKEY_ADVANCED_OK:
    case SKEY_COMMAND_CHECKSUM:
    case SKEY_EMULATED_M600:
    case SKEY_EMULATED_M109_M190:
    case SKEY_EVENT_LED:
    case SKEY_FILE_COMMENT_PARSING:
      TOGGLE_BIT(infoSettings.general_settings, ((item_index - SKEY_ADVANCED_OK) + INDEX_ADVANCED_OK));
      break;

    case SKEY_SERIAL_ALWAYS_ON:
      TOGGLE_BIT(infoSettings.serial_always_on, 0);
      break;

    case SKEY_SPEED:
      infoSettings.move_speed = (infoSettings.move_speed + 1) % ITEM_SPEED_NUM;
      break;

    case SKEY_AUTO_LOAD_LEVELING:
      TOGGLE_BIT(infoSettings.auto_load_leveling, 0);
      break;

    case SKEY_PROBING_Z_OFFSET:
      TOGGLE_BIT(infoSettings.probing_z_offset, 0);
      break;

    case SKEY_Z_STEPPERS_ALIGNMENT:
      TOGGLE_BIT(infoSettings.z_steppers_alignment, 0);
      break;

    #ifdef PS_ON_PIN
      case SKEY_PS_AUTO_SHUTDOWN:
        infoSettings.auto_shutdown = (infoSettings.auto_shutdown + 1) % ITEM_TOGGLE_AUTO_NUM;
        break;
    #endif

    #ifdef FIL_RUNOUT_PIN
      case SKEY_FIL_RUNOUT:
        TOGGLE_BIT(infoSettings.runout, 0);
        break;
    #endif

    case SKEY_PL_RECOVERY:
      TOGGLE_BIT(infoSettings.plr, 0);
      break;

    case SKEY_PL_RECOVERY_HOME:
      TOGGLE_BIT(infoSettings.plr_home, 0);
      break;

    case SKEY_BTT_MINI_UPS:
      TOGGLE_BIT(infoSettings.btt_ups, 0);
      break;

    case SKEY_START_GCODE_ENABLED:
    case SKEY_END_GCODE_ENABLED:
    case SKEY_CANCEL_GCODE_ENABLED:
      TOGGLE_BIT(infoSettings.send_gcodes, (item_index - SKEY_START_GCODE_ENABLED));
      break;

    case SKEY_RESET_SETTINGS:
      popupDialog(DIALOG_TYPE_ALERT, LABEL_SETTINGS_RESET, LABEL_SETTINGS_RESET_INFO, LABEL_CONFIRM, LABEL_CANCEL, resetSettings, NULL, NULL);
      break;

    default:
      return;
  }
}  // updateFeatureSettings

// load values on page change and reload
void loadFeatureSettings(LISTITEM * item, uint16_t item_index, uint8_t itemPos)
{
  if (item_index < SKEY_COUNT)
  {
    switch (item_index)
    {
      case SKEY_ADVANCED_OK:
      case SKEY_COMMAND_CHECKSUM:
      case SKEY_EMULATED_M600:
      case SKEY_EMULATED_M109_M190:
      case SKEY_EVENT_LED:
      case SKEY_FILE_COMMENT_PARSING:
        item->icon = iconToggle[GET_BIT(infoSettings.general_settings, ((item_index - SKEY_ADVANCED_OK) + INDEX_ADVANCED_OK))];
        break;

      case SKEY_SERIAL_ALWAYS_ON:
        item->icon = iconToggle[infoSettings.serial_always_on];
        break;

      case SKEY_SPEED:
        item->valueLabel = itemSpeed[infoSettings.move_speed].label;
        break;

      case SKEY_AUTO_LOAD_LEVELING:
        item->icon = iconToggle[infoSettings.auto_load_leveling];
        break;

      case SKEY_PROBING_Z_OFFSET:
        item->icon = iconToggle[infoSettings.probing_z_offset];
        break;

      case SKEY_Z_STEPPERS_ALIGNMENT:
        item->icon = iconToggle[infoSettings.z_steppers_alignment];
        break;

      #ifdef PS_ON_PIN
        case SKEY_PS_AUTO_SHUTDOWN:
          item->valueLabel = itemToggleAuto[infoSettings.auto_shutdown];
          break;
      #endif

      #ifdef FIL_RUNOUT_PIN
        case SKEY_FIL_RUNOUT:
        {
          LABEL sensorLabel = itemToggleSmart[GET_BIT(infoSettings.runout, 1)];
          item->valueLabel.index = (GET_BIT(infoSettings.runout, 0)) ? sensorLabel.index : LABEL_OFF;
          break;
        }
      #endif

      case SKEY_PL_RECOVERY:
        item->icon = iconToggle[infoSettings.plr];
        break;

      case SKEY_PL_RECOVERY_HOME:
        item->icon = iconToggle[infoSettings.plr_home];
        break;

      case SKEY_BTT_MINI_UPS:
        item->icon = iconToggle[infoSettings.btt_ups];
        break;

      case SKEY_START_GCODE_ENABLED:
      case SKEY_END_GCODE_ENABLED:
      case SKEY_CANCEL_GCODE_ENABLED:
        item->icon = iconToggle[GET_BIT(infoSettings.send_gcodes, (item_index - SKEY_START_GCODE_ENABLED))];
        break;

      case SKEY_RESET_SETTINGS:
        break;

      default:
        break;
    }
  }
}  // loadFeatureSettings

void menuFeatureSettings(void)
{
  LABEL title = {LABEL_FEATURE_SETTINGS};

  // set item types
  LISTITEM settingPage[SKEY_COUNT] = {
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_ADVANCED_OK,            LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_COMMAND_CHECKSUM,       LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EMULATED_M600,          LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EMULATED_M109_M190,     LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EVENT_LED,              LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FILE_COMMENT_PARSING,   LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_SERIAL_ALWAYS_ON,       LABEL_NULL},
    {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_MOVE_SPEED,             LABEL_NORMAL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_AUTO_LOAD_LEVELING,     LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PROBING_Z_OFFSET,       LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_Z_STEPPERS_ALIGNMENT,   LABEL_NULL},

    #ifdef PS_ON_PIN
      {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_PS_AUTO_SHUTDOWN,       LABEL_OFF},
    #endif

    #ifdef FIL_RUNOUT_PIN
      {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_FIL_RUNOUT,             LABEL_OFF},
    #endif

    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY,            LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY_HOME,       LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_BTT_MINI_UPS,           LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_START_GCODE_ENABLED,    LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_END_GCODE_ENABLED,      LABEL_NULL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_CANCEL_GCODE_ENABLED,   LABEL_NULL},
    // keep reset settings always at the bottom of the settings menu list
    {CHARICON_BLANK,       LIST_MOREBUTTON,    LABEL_SETTINGS_RESET,         LABEL_NULL}
  };

  uint16_t index = KEY_IDLE;

  listViewCreate(title, settingPage, SKEY_COUNT, &fe_cur_page, true, NULL, loadFeatureSettings);

  while (MENU_IS(menuFeatureSettings))
  {
    index = listViewGetSelectedIndex();

    if (index < SKEY_COUNT)
    {
      updateFeatureSettings(index);
      listViewRefreshItem(index);
    }

    loopProcess();
  }

  saveSettings();  // save settings
}
