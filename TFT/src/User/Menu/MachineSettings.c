#include "MachineSettings.h"
#include "includes.h"

LISTITEMS customItems = {
// title
LABEL_CUSTOM,
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
//add key number index of the items
//
typedef enum
{
  #ifdef CUSTOM_0_GCODE
    CKEY0,
  #endif
  #ifdef CUSTOM_1_GCODE
    CKEY1,
  #endif
  #ifdef CUSTOM_2_GCODE
    CKEY2,
  #endif
  #ifdef CUSTOM_3_GCODE
    CKEY3,
  #endif
  #ifdef CUSTOM_4_GCODE
    CKEY4,
  #endif
  #ifdef CUSTOM_5_GCODE
    CKEY5,
  #endif
  #ifdef CUSTOM_6_GCODE
    CKEY6,
  #endif
  #ifdef CUSTOM_7_GCODE
    CKEY7,
  #endif
  #ifdef CUSTOM_8_GCODE
    CKEY8,
  #endif
  #ifdef CUSTOM_9_GCODE
    CKEY9,
  #endif
  #ifdef CUSTOM_10_GCODE
    CKEY10,
  #endif
  #ifdef CUSTOM_11_GCODE
    CKEY11,
  #endif
  #ifdef CUSTOM_12_GCODE
    CKEY12,
  #endif
  #ifdef CUSTOM_13_GCODE
    CKEY13,
  #endif
  #ifdef CUSTOM_14_GCODE
    CKEY14,
  #endif

CKEY_COUNT //keep this always at the end
}CKEY_LIST;

#define GC_PAGE_COUNT  (CKEY_COUNT+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE
int gc_cur_page = 0;

const char *const gcodelist[CKEY_COUNT][2] = {
  #ifdef CUSTOM_0_GCODE
  {CUSTOM_0_LABEL, CUSTOM_0_GCODE},
  #endif
  #ifdef CUSTOM_1_GCODE
  {CUSTOM_1_LABEL, CUSTOM_1_GCODE},
  #endif
  #ifdef CUSTOM_2_GCODE
  {CUSTOM_2_LABEL, CUSTOM_2_GCODE},
  #endif
  #ifdef CUSTOM_3_GCODE
  {CUSTOM_3_LABEL, CUSTOM_3_GCODE},
  #endif
  #ifdef CUSTOM_4_GCODE
  {CUSTOM_4_LABEL, CUSTOM_4_GCODE},
  #endif
  #ifdef CUSTOM_5_GCODE
  {CUSTOM_5_LABEL, CUSTOM_5_GCODE},
  #endif
  #ifdef CUSTOM_6_GCODE
  {CUSTOM_6_LABEL, CUSTOM_6_GCODE},
  #endif
  #ifdef CUSTOM_7_GCODE
  {CUSTOM_7_LABEL, CUSTOM_7_GCODE},
  #endif
  #ifdef CUSTOM_8_GCODE
  {CUSTOM_8_LABEL, CUSTOM_8_GCODE},
  #endif
  #ifdef CUSTOM_9_GCODE
  {CUSTOM_9_LABEL, CUSTOM_9_GCODE},
  #endif
  #ifdef CUSTOM_10_GCODE
  {CUSTOM_10_LABEL, CUSTOM_10_GCODE},
  #endif
  #ifdef CUSTOM_11_GCODE
  {CUSTOM_11_LABEL, CUSTOM_11_GCODE},
  #endif
  #ifdef CUSTOM_12_GCODE
  {CUSTOM_12_LABEL, CUSTOM_12_GCODE},
  #endif
  #ifdef CUSTOM_13_GCODE
  {CUSTOM_13_LABEL, CUSTOM_13_GCODE},
  #endif
  #ifdef CUSTOM_14_GCODE
  {CUSTOM_14_LABEL, CUSTOM_14_GCODE},
  #endif
};
//
//perform action on button press
//
void sendCustomGcode(uint32_t key_val)
{
  uint32_t item_index = gc_cur_page*LISTITEM_PER_PAGE+ key_val;
    if (item_index < CKEY_COUNT){
    storeCmd(gcodelist[item_index][1]);
  }
}

//
//load values on page change and reload
//
void loaditemsCustomGcode(void){
  for (uint32_t i = 0; i < LISTITEM_PER_PAGE; i++)
  {
    uint32_t item_index = gc_cur_page*LISTITEM_PER_PAGE + i;
    if (item_index < CKEY_COUNT){
      customItems.items[i].icon = ICONCHAR_CODE;
      customItems.items[i].titlelabel.index = LABEL_DYNAMIC;
      //dynamic_label[i] = (char *)gcodelist[item_index][0];
      setDynamicLabel(i,(char *)gcodelist[item_index][0]);
    }
    else{
      customItems.items[i].icon = ICONCHAR_BACKGROUND;
      customItems.items[i].titlelabel.index = LABEL_BACKGROUND;
    }
  }
  // set page up down button according to page count and current page
  if (CKEY_COUNT <= LISTITEM_PER_PAGE)
  {
    customItems.items[5].icon = ICONCHAR_BACKGROUND;
    customItems.items[6].icon = ICONCHAR_BACKGROUND;
  }
  else
  {
    if(gc_cur_page == 0){
      customItems.items[5].icon = ICONCHAR_BACKGROUND;
      customItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if(gc_cur_page == (GC_PAGE_COUNT-1)){
      customItems.items[5].icon = ICONCHAR_PAGEUP;
      customItems.items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      customItems.items[5].icon = ICONCHAR_PAGEUP;
      customItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
      // menuDrawListItem(&customItems.items[5],5);
      //menuDrawListItem(&customItems.items[6],6);
}

void menuCustom(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  gc_cur_page = 0;
  loaditemsCustomGcode();
  menuDrawListPage(&customItems);

  while(infoMenu.menu[infoMenu.cur] == menuCustom)
  {
    key_num = menuKeyGetValue();

    if(key_num < LISTITEM_PER_PAGE){
      sendCustomGcode(key_num);
    }
    switch(key_num)
    {
      case KEY_ICON_5:
          if(GC_PAGE_COUNT > 1){
            if (gc_cur_page > 0){
              gc_cur_page--;
              loaditemsCustomGcode();
              menuRefreshListPage();
            }
          }
          break;
        case KEY_ICON_6:
          if(GC_PAGE_COUNT > 1){
            if (gc_cur_page < GC_PAGE_COUNT-1){
              gc_cur_page++;
              loaditemsCustomGcode();
              menuRefreshListPage();
            }
          }
          break;
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default:
        break;
    }
    loopProcess();
  }
}


MENUITEMS RGBItems = {
// title
LABEL_RGB_SETTINGS,
// icon                       label
 {{ICON_RGB_RED,              LABEL_RED},
  {ICON_RGB_GREEN,            LABEL_GREEN},
  {ICON_RGB_BLUE,             LABEL_BLUE},
  {ICON_RGB_WHITE,            LABEL_WHITE},
  {ICON_RGB_OFF,              LABEL_OFF},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuRGBSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&RGBItems);

  while(infoMenu.menu[infoMenu.cur] == menuRGBSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:  //Red
        storeCmd("M150 R255 U0 B0 P255\n");
        break;

      case KEY_ICON_1:  //Green
        storeCmd("M150 R0 U255 B0 P255\n");
        break;

      case KEY_ICON_2:  //Blue
        storeCmd("M150 R0 U0 B255 P255\n");
        break;

      case KEY_ICON_3:  //White
        storeCmd("M150 R255 U255 B255 P255\n");
        break;

      case KEY_ICON_4:  //Turn Off
        storeCmd("M150 R0 U0 B0 P0\n");
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }
}


MENUITEMS machineSettingsItems = {
// title
LABEL_MACHINE_SETTINGS,
// icon                       label
 {{ICON_CUSTOM,               LABEL_CUSTOM},
  {ICON_RGB_SETTINGS,         LABEL_RGB_SETTINGS},
  {ICON_GCODE,                LABEL_GCODE},
  {ICON_SHUT_DOWN,            LABEL_SHUT_DOWN},
  {ICON_PARAMETER,            LABEL_PARAMETER_SETTING},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuMachineSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&machineSettingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuMachineSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {

      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] =  menuCustom;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuRGBSettings;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
        break;

      case KEY_ICON_3:
        storeCmd("M81\n");
        break;

      case KEY_ICON_4:
        mustStoreCmd("M503 S0\n");
        infoMenu.menu[++infoMenu.cur] = menuParameterSettings;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
