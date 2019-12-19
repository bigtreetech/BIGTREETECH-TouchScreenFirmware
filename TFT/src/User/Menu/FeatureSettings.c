#include "FeatureSettings.h"
#include "includes.h"

#ifndef MENU_LIST_MODE

    MENUITEMS featureSettingsItems = {
    // title
    LABEL_FEATURE_SETTINGS,
    // icon                       label
    {
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACK,                 LABEL_BACK},}
    };

    #define ITEM_PS_ON_NUM 2
    const ITEM itemPowerOff[ITEM_PS_ON_NUM] = {
    // icon                       label
      {ICON_POWER_SUPPLY,         LABEL_MANUAL_SHUT_DOWN},
      {ICON_POWER_SUPPLY,         LABEL_AUTO_SHUT_DOWN},
    };
    const  u8 item_power_off[ITEM_PS_ON_NUM] = {0, 1};
    static u8  item_power_off_i = 0;
      
    #ifdef FIL_RUNOUT_PIN
      #define ITEM_RUNOUT_NUM 3
      const ITEM itemRunout[ITEM_RUNOUT_NUM] = {
      // icon                       label
        {ICON_RUNOUT,               LABEL_RUNOUT_OFF},
        {ICON_RUNOUT,               LABEL_RUNOUT_ON},
        {ICON_RUNOUT,               LABEL_SMART_RUNOUT_ON},
      };
      const  u8 item_runout[ITEM_RUNOUT_NUM] = {FILAMENT_RUNOUT_OFF, FILAMENT_RUNOUT_ON, FILAMENT_SMART_RUNOUT_ON};
      static u8  item_runout_i = 0;
    #endif

    void menuFeatureSettings(void)
    {
      KEY_VALUES key_num = KEY_IDLE;
      SETTINGS now = infoSettings;

      for(u8 i=0; i<ITEM_PS_ON_NUM; i++)
      {
        if(infoSettings.auto_off == item_power_off[i])
        {
          item_power_off_i = i;
          featureSettingsItems.items[KEY_ICON_0] = itemPowerOff[item_power_off_i];
        }
      }
      
      #ifdef FIL_RUNOUT_PIN
      for(u8 i=0; i<ITEM_RUNOUT_NUM; i++)
      {
        if(infoSettings.runout == item_runout[i])
        {
          item_runout_i = i;
          featureSettingsItems.items[KEY_ICON_1] = itemRunout[item_runout_i];
        }
      }
      #endif  
      
      menuDrawPage(&featureSettingsItems);

      while(infoMenu.menu[infoMenu.cur] == menuFeatureSettings)
      {
        key_num = menuKeyGetValue();
        switch(key_num)
        {
          #ifdef FIL_RUNOUT_PIN
          case KEY_ICON_0:
            item_power_off_i = (item_power_off_i + 1) % ITEM_PS_ON_NUM;                
            featureSettingsItems.items[key_num] = itemPowerOff[item_power_off_i];
            menuDrawItem(&featureSettingsItems.items[key_num], key_num);
            infoSettings.auto_off = item_power_off[item_power_off_i];
            break;
          #endif
          
          #ifdef FIL_RUNOUT_PIN
          case KEY_ICON_1:
            item_runout_i = (item_runout_i + 1) % ITEM_RUNOUT_NUM;                
            featureSettingsItems.items[key_num] = itemRunout[item_runout_i];
            menuDrawItem(&featureSettingsItems.items[key_num], key_num);
            infoSettings.runout = item_runout[item_runout_i];
            break;
          #endif
          
          case KEY_ICON_7:
            infoMenu.cur--;
            break;
          
          default:
            break;
        }
        loopProcess();		
      }

      if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
      {
        storePara();
      }
    }

#else

    LISTITEMS featureSettingsItems = {
    // title
    LABEL_FEATURE_SETTINGS,
    // icon                 ItemType      Item Title        item value text(only for custom value)
    {
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_PAGEUP,     LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_PAGEDOWN,   LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
      {ICONCHAR_BACK,       LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},}
    };

//
//setup item states
//
    #ifdef PS_ON_PIN
      #define ITEM_PS_ON_NUM 2
      const LISTITEM itemPowerOff[ITEM_PS_ON_NUM] = {
      // icon                 ItemType      Item Title        item value text(only for custom value)
        {ICONCHAR_TOGGLE_OFF, LIST_TOGGLE,  LABEL_AUTO_SHUT_DOWN, LABEL_BACKGROUND},
        {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,  LABEL_AUTO_SHUT_DOWN, LABEL_BACKGROUND},
      };
      const  u8 item_power_off[ITEM_PS_ON_NUM] = {0, 1};
      static u8  item_power_off_i = 0; //for auto power
    #endif
    
    #ifdef FIL_RUNOUT_PIN
      #define ITEM_RUNOUT_NUM 3
      const LISTITEM itemRunout[ITEM_RUNOUT_NUM] = {
    // icon                 ItemType      Item Title        item value text(only for custom value)
        {ICONCHAR_BLANK, LIST_CUSTOMVALUE,  LABEL_FILAMENT_RUNOUT, LABEL_OFF},
        {ICONCHAR_BLANK, LIST_CUSTOMVALUE,  LABEL_FILAMENT_RUNOUT, LABEL_ON},
        {ICONCHAR_BLANK, LIST_CUSTOMVALUE,  LABEL_FILAMENT_RUNOUT, LABEL_SMART},
      };

      const  u8 item_runout[ITEM_RUNOUT_NUM] = {FILAMENT_RUNOUT_OFF, FILAMENT_RUNOUT_ON, FILAMENT_SMART_RUNOUT_ON};
      static u8  item_runout_i = 0; //for runout sensor
    #endif
    
    #define ITEM_SPEED_NUM 3
    const LISTITEM itemMoveSpeed[ITEM_SPEED_NUM] = {
      // icon          ItemType           Item Title              item value text(only for custom value)
      {ICONCHAR_BLANK, LIST_CUSTOMVALUE,  LABEL_MOVE_SPEED, LABEL_NORMAL_SPEED},
      {ICONCHAR_BLANK, LIST_CUSTOMVALUE,  LABEL_MOVE_SPEED, LABEL_SLOW_SPEED},
      {ICONCHAR_BLANK, LIST_CUSTOMVALUE,  LABEL_MOVE_SPEED, LABEL_FAST_SPEED},
    };
    const  u8 item_movespeed[ITEM_SPEED_NUM] = {LABEL_NORMAL_SPEED, LABEL_SLOW_SPEED, LABEL_FAST_SPEED};

    const uint16_t toggleitem[2] = {ICONCHAR_TOGGLE_OFF,ICONCHAR_TOGGLE_ON};
    const  u8  item_toggleState[2]    = {0, 1};

    static u8  item_terminalACK_i     = 0; //for hide ack
    static u8  item_invert_yaxis_i    = 0; //for invert y axis
    static u8  item_movespeed_i       = 0; //for move speed

//
//add key number index of the items
//
    typedef enum
    {
      SKEY_HIDEACK = 0,
      SKEY_INVERT_Y,
      #ifdef PS_ON_PIN
        SKEY_POWER,
      #endif
      #ifdef FIL_RUNOUT_PIN
        SKEY_RUNOUT,
      #endif
      SKEY_SPEED,
      SKEY_COUNT //keep this always at the end
    }SKEY_LIST; 
    
    #define FE_PAGE_COUNT  (SKEY_COUNT+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE
    int fe_cur_page = 0;

//
//set item types
//
    LISTITEM settingPage[SKEY_COUNT] = {  
      {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_TERMINAL_ACK,       LABEL_BACKGROUND},
      {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_INVERT_YAXIS,       LABEL_BACKGROUND},
      #ifdef PS_ON_PIN
      {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_AUTO_SHUT_DOWN,     LABEL_BACKGROUND},
      #endif
      #ifdef FIL_RUNOUT_PIN
      {ICONCHAR_TOGGLE_ON,  LIST_CUSTOMVALUE,   LABEL_FILAMENT_RUNOUT,    LABEL_OFF       },
      #endif
      {ICONCHAR_TOGGLE_ON,  LIST_CUSTOMVALUE,   LABEL_MOVE_SPEED,   LABEL_NORMAL_SPEED},
      
    };

//
//perform action on button press
//
    void updateFeatureSettings(uint8_t key_val)
    {
      uint8_t item_index = fe_cur_page*FE_PAGE_COUNT+ key_val;
      switch (item_index)
      {
        case SKEY_HIDEACK:
        item_terminalACK_i = (item_terminalACK_i + 1) % 2;
        settingPage[item_index].icon = toggleitem[item_terminalACK_i];
        featureSettingsItems.items[key_val].icon = toggleitem[item_terminalACK_i];;
        menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
        infoSettings.terminalACK = item_terminalACK_i;
        break;

        case SKEY_INVERT_Y:
        item_invert_yaxis_i = (item_invert_yaxis_i + 1) % 2;
        settingPage[item_index].icon = toggleitem[item_invert_yaxis_i];
        featureSettingsItems.items[key_val] = settingPage[item_index];
        menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
        infoSettings.invert_yaxis = item_invert_yaxis_i;
        break;

        #ifdef PS_ON_PIN
        case SKEY_POWER:
        item_power_off_i = (item_power_off_i + 1) % ITEM_PS_ON_NUM;
        settingPage[item_index] = itemPowerOff[item_power_off_i];
        featureSettingsItems.items[key_val] = settingPage[item_index];
        menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
        infoSettings.auto_off = item_power_off[item_power_off_i];
        break;
        #endif

        #ifdef FIL_RUNOUT_PIN
        case SKEY_RUNOUT:
        item_runout_i = (item_runout_i + 1) % ITEM_RUNOUT_NUM;
        settingPage[item_index] = itemRunout[item_runout_i];
        featureSettingsItems.items[key_val] = settingPage[item_index];
        menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
        infoSettings.runout = item_runout[item_runout_i];
        break;
        #endif

        case SKEY_SPEED:
        item_movespeed_i = (item_movespeed_i + 1) % ITEM_SPEED_NUM;
        settingPage[item_index] = itemMoveSpeed[item_movespeed_i];
        featureSettingsItems.items[key_val] = settingPage[item_index];
        menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
        infoSettings.move_speed = item_movespeed_i;
        break;
      default:
        break;
      }
    }

//
//load values on page change and reload
//
    void loadFeatureSettings(){
      for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++)
      {
        uint8_t item_index = fe_cur_page*FE_PAGE_COUNT + i;
        switch (item_index)
        {
          case SKEY_HIDEACK:
            item_terminalACK_i = infoSettings.terminalACK;
            settingPage[SKEY_HIDEACK].icon = toggleitem[item_terminalACK_i];
            featureSettingsItems.items[i] = settingPage[SKEY_HIDEACK];
            break;

          case SKEY_INVERT_Y:
            item_invert_yaxis_i = infoSettings.invert_yaxis;
            settingPage[SKEY_INVERT_Y].icon = toggleitem[item_invert_yaxis_i];
            featureSettingsItems.items[i] = settingPage[SKEY_INVERT_Y];
            break;

          #ifdef PS_ON_PIN
            case SKEY_POWER:
              for (u8 n = 0; n < ITEM_PS_ON_NUM; n++)
              {
                if (infoSettings.auto_off == item_power_off[n])
                { item_power_off_i = n; }
              }  
              featureSettingsItems.items[i] = itemPowerOff[item_power_off_i];
            break;
          #endif

          #ifdef FIL_RUNOUT_PIN
            case SKEY_RUNOUT:
              for (u8 n = 0; n < ITEM_RUNOUT_NUM; n++)
              {
                if (infoSettings.runout == item_runout[n])
                { item_runout_i = n; }
              }
              featureSettingsItems.items[i] = itemRunout[item_runout_i];
            break;
          #endif

          case SKEY_SPEED:
             item_movespeed_i = infoSettings.move_speed; 
              featureSettingsItems.items[i] = itemMoveSpeed[item_movespeed_i];
            break;
          default:
            settingPage[item_index].icon = ICONCHAR_BACKGROUND;
            featureSettingsItems.items[i] = settingPage[item_index];
          break;
        }
     }
      // set page up down button according to page count and current page
      if (SKEY_COUNT <= LISTITEM_PER_PAGE)
      {
        featureSettingsItems.items[5].icon = ICONCHAR_BACKGROUND;
        featureSettingsItems.items[6].icon = ICONCHAR_BACKGROUND;
      }
      else
      {
        if(fe_cur_page == 0){
          featureSettingsItems.items[5].icon = ICONCHAR_BACKGROUND;
          featureSettingsItems.items[6].icon = ICONCHAR_PAGEDOWN;
        }
        else if(fe_cur_page == (FE_PAGE_COUNT-1)){
          featureSettingsItems.items[5].icon = ICONCHAR_PAGEUP;
          featureSettingsItems.items[6].icon = ICONCHAR_BACKGROUND;
        }
        else
        {
          featureSettingsItems.items[5].icon = ICONCHAR_PAGEUP;
          featureSettingsItems.items[6].icon = ICONCHAR_PAGEDOWN;
        }
      }
      
    }

    void menuFeatureSettings(void)
    {
      KEY_VALUES key_num = KEY_IDLE;
      SETTINGS now = infoSettings;
      fe_cur_page = 0;
      loadFeatureSettings();
      menuDrawListPage(&featureSettingsItems);

      while (infoMenu.menu[infoMenu.cur] == menuFeatureSettings)
      {
        key_num = menuKeyGetValue();
        switch (key_num)
        {
        case KEY_ICON_5:
          if(FE_PAGE_COUNT > 1){
            if (fe_cur_page > 0){
              fe_cur_page--;
              loadFeatureSettings();
            }
          }
          break;
        case KEY_ICON_6:
          if(FE_PAGE_COUNT > 1){
            if (fe_cur_page < FE_PAGE_COUNT){
              fe_cur_page++;
              loadFeatureSettings();
            }
          }
          break;
        case KEY_ICON_7:
          infoMenu.cur--;
          break;
        default:
          updateFeatureSettings(key_num);
          break;
        }

      loopProcess();		
    }

      if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
      {
        storePara();
      }
    }

#endif
