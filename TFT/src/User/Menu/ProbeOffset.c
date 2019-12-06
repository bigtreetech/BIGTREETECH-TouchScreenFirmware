#include "ProbeOffset.h"
#include "includes.h"

//1 titl, ITEM_PER_PAGE item
MENUITEMS probeOffsetItems = {
// title
LABEL_PROBE_OFFSET,
// icon                        label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_NORMAL_SPEED,         LABEL_VALUE_ZERO},
  {ICON_BACK,                 LABEL_BACK},}
};

typedef struct
{
  const ITEM *list;
  const float *ele;
  uint8_t cur;
  uint8_t totaled;
}ELEMENTS;

#define ITEM_PROBE_OFFSET_UNIT_NUM 3
const ITEM itemProbeOffsetUnit[ITEM_PROBE_OFFSET_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_probeOffset_unit[ITEM_PROBE_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;

static void initElements(u8 position)
{
  elementsUnit.totaled = ITEM_PROBE_OFFSET_UNIT_NUM;
  elementsUnit.list = itemProbeOffsetUnit;
  elementsUnit.ele  = item_probeOffset_unit;

  for(u8 i=0; i<elementsUnit.totaled; i++)
  {
    if(memcmp(&elementsUnit.list[i], &probeOffsetItems.items[position], sizeof(ITEM)) == 0)
    {
      elementsUnit.cur = i;
      break;
    }
  }
}

static float probe_offset_value=0.0;

#define PROBE_OFFSET_MAX_VALUE 20.0f
#define PROBE_OFFSET_MIN_VALUE -20.0f

void showProbeOffset(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, probe_offset_value, 3, 2, RIGHT);
}
void probeOffsetReDraw(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, probe_offset_value, 3, 2, RIGHT);
}

/* Set current offset */
void setCurrentOffset(float offset)
{
  //probe_offset_value = limitValue(PROBE_OFFSET_MIN_VALUE, offset, PROBE_OFFSET_MAX_VALUE);
  probe_offset_value = offset;
}

void menuProbeOffset(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  float now = probe_offset_value;
  initElements(KEY_ICON_5);
  menuDrawPage(&probeOffsetItems,false);
  showProbeOffset();
  while(infoMenu.menu[infoMenu.cur] == menuProbeOffset)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if(probe_offset_value - elementsUnit.ele[elementsUnit.cur] > PROBE_OFFSET_MIN_VALUE)
        {
          if(storeCmd("M851 Z%.2f\n",probe_offset_value-elementsUnit.ele[elementsUnit.cur]))
            probe_offset_value -= elementsUnit.ele[elementsUnit.cur];
        }
        break;
      case KEY_ICON_3:
        if(probe_offset_value + elementsUnit.ele[elementsUnit.cur] < PROBE_OFFSET_MAX_VALUE)
        {
          if(storeCmd("M851 Z%.2f\n",probe_offset_value+elementsUnit.ele[elementsUnit.cur]))
            probe_offset_value += elementsUnit.ele[elementsUnit.cur];
        }
        break;
      case KEY_ICON_4:
        storeCmd("M500\n");
        break;
      case KEY_ICON_5:
        elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
        probeOffsetItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
        menuDrawItem(&probeOffsetItems.items[key_num], key_num);
        break;
      case KEY_ICON_6:
        if(storeCmd("M851 Z%.2f\n",0))
          probe_offset_value = 0.0f;
        break;
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default :
        break;
      }
    if(now != probe_offset_value)
    {
      now = probe_offset_value;
      probeOffsetReDraw();
    }
    loopProcess();
  }
}





