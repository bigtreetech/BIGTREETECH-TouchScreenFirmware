#include "BabyStep.h"
#include "includes.h"

//1 title, ITEM_PER_PAGE items(icon+label)
MENUITEMS babyStepItems = {
//title
  LABEL_BABYSTEP,
//icon                        label
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

#define ITEM_BABYSTEP_UNIT_NUM 3
const ITEM itemBabyStepUnit[ITEM_BABYSTEP_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_babystep_unit[ITEM_BABYSTEP_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;

static void initElements(u8 position)
{
  elementsUnit.totaled = ITEM_BABYSTEP_UNIT_NUM;
  elementsUnit.list = itemBabyStepUnit;
  elementsUnit.ele  = item_babystep_unit;

  for(u8 i=0; i<elementsUnit.totaled; i++)
  {
    if(memcmp(&elementsUnit.list[i], &babyStepItems.items[position], sizeof(ITEM)) == 0)
    {
      elementsUnit.cur = i;
      break;
    }
  }
}

static float baby_step_value = 0.0f;

void babyStepReset(void)
{
  baby_step_value = 0.0f;
}

#define BABYSTEP_MAX_VALUE 5.0f
#define BABYSTEP_MIN_VALUE -5.0f

void showBabyStep(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, baby_step_value, 3, 2, RIGHT);
}
void babyStepReDraw(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, baby_step_value, 3, 2, RIGHT);
}


void menuBabyStep(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  float now = baby_step_value;

  initElements(KEY_ICON_5);
  menuDrawPage(&babyStepItems);
  showBabyStep();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuBabyStep)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if(baby_step_value - elementsUnit.ele[elementsUnit.cur] > BABYSTEP_MIN_VALUE)
        {
          if(storeCmd("M290 Z-%.2f\n",elementsUnit.ele[elementsUnit.cur]))
            baby_step_value -= elementsUnit.ele[elementsUnit.cur];
        }
        break;
      case KEY_ICON_3:
        if(baby_step_value + elementsUnit.ele[elementsUnit.cur] < BABYSTEP_MAX_VALUE)
        {
          if(storeCmd("M290 Z%.2f\n",elementsUnit.ele[elementsUnit.cur]))
            baby_step_value += elementsUnit.ele[elementsUnit.cur];
        }
        break;
      case KEY_ICON_4:
        if(infoMachineSettings.EEPROM == 1){
           storeCmd("M500\n");
        }
        break;
      case KEY_ICON_5:
        elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
        babyStepItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
        menuDrawItem(&babyStepItems.items[key_num], key_num);
        break;
      case KEY_ICON_6:
        if(storeCmd("M290 Z%.2f\n",-baby_step_value))
          baby_step_value = 0.0f;
        break;
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default :
        #if LCD_ENCODER_SUPPORT
          if(encoderPosition)
          {
            baby_step_value += elementsUnit.ele[elementsUnit.cur]*encoderPosition;
            encoderPosition = 0;
          }
          LCD_LoopEncoder();
        #endif
        break;
      }
    if(now != baby_step_value)
    {
      now = baby_step_value;
      babyStepReDraw();
    }
    loopProcess();
  }
}
