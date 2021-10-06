#include "SelectMode.h"
#include "includes.h"

typedef enum
{
  MKEY_0 = 0,
  MKEY_1,
  MKEY_IDLE = IDLE_TOUCH,
} MKEY_VALUES;

const GUI_RECT rect_of_mode[MODE_COUNT] = {
  // 2 select icon
  {1 * SPACE_SELEX + 0 * ICON_WIDTH, SPACE_SELEY, 1 * SPACE_SELEX + 1 * ICON_WIDTH, SPACE_SELEY + ICON_HEIGHT},
  {3 * SPACE_SELEX + 1 * ICON_WIDTH, SPACE_SELEY, 3 * SPACE_SELEX + 2 * ICON_WIDTH, SPACE_SELEY + ICON_HEIGHT},
};

const uint8_t icon_mode [MODE_COUNT] = {
  ICON_MARLIN,
  ICON_BIGTREETECH,
};

void drawModeIcon(void)
{
  for (uint8_t i = 0; i < MODE_COUNT; i++)
  {
    ICON_ReadDisplay(rect_of_mode[i].x0, rect_of_mode[i].y0, icon_mode[i]);
  }

  const GUI_RECT mode_title_rect[MODE_COUNT] = {
    {0,           rect_of_mode[0].y1 + BYTE_HEIGHT / 2, text_startx, rect_of_mode[0].y1 + BYTE_HEIGHT / 2 + BYTE_HEIGHT},
    {text_startx, rect_of_mode[0].y1 + BYTE_HEIGHT / 2, LCD_WIDTH,   rect_of_mode[0].y1 + BYTE_HEIGHT / 2 + BYTE_HEIGHT},
  };

  GUI_RestoreColorDefault();
  GUI_DispStringInPrect(&mode_title_rect[0],(uint8_t *)"Marlin Mode");
  GUI_DispStringInPrect(&mode_title_rect[1],(uint8_t *)"Touch Mode");
}

static inline MKEY_VALUES MKeyGetValue(void)
{
  return (MKEY_VALUES)KEY_GetValue(COUNT(rect_of_mode), rect_of_mode);
}

void drawSelectedMode(int8_t nowMode)
{
  const uint8_t border_off = 4;

  for (uint8_t i = 0; i < MODE_COUNT; i++)
  {
    GUI_SetColor(i == nowMode ? infoSettings.marlin_font_color : MENU_BACKGROUND_COLOR);
    GUI_DrawRect(rect_of_mode[i].x0 - border_off, rect_of_mode[i].y0 - border_off,
                 rect_of_mode[i].x1 + border_off, rect_of_mode[i].y1 + border_off);
  }

  GUI_RestoreColorDefault();
}

// open mode switching menu
void menuMode(void)
{
  int8_t nowMode = GET_BIT(infoSettings.mode, 0);
  TSC_ReDrawIcon = NULL;  // disable icon redraw callback function

  GUI_Clear(infoSettings.bg_color);
  drawModeIcon();
  drawSelectedMode(nowMode);

  #if LCD_ENCODER_SUPPORT
    while (!XPT2046_Read_Pen() || LCD_Enc_ReadBtn(LCD_ENC_BUTTON_INTERVAL))
      ;  // wait for button release
  #else
    while (!XPT2046_Read_Pen())
      ;  // wait for touch release
  #endif

  while (MENU_IS(menuMode))
  {
    MKEY_VALUES key_num = MKeyGetValue();

    if (key_num == MKEY_0 || key_num == MKEY_1)
    {
      nowMode = key_num;
      break;
    }

    #if LCD_ENCODER_SUPPORT
      if (encoderPosition)
      {
        nowMode = NOBEYOND(0, nowMode + encoderPosition, MODE_COUNT - 1);
        drawSelectedMode(nowMode);
        encoderPosition = 0;
      }

      if (LCD_Enc_ReadBtn(LCD_ENC_BUTTON_INTERVAL))
        break;

      LCD_Enc_CheckSteps();
    #endif

    if (infoSettings.mode == MODE_SERIAL_TSC || infoSettings.serial_always_on == 1)
    {
      loopBackEnd();
    }
    #ifdef LCD_LED_PWM_CHANNEL  // LCD_CheckDimming() is invoked by loopBackEnd(), so we guarantee it is invoked only once
      else
      {
        LCD_CheckDimming();
      }
    #endif
  }

  if (infoSettings.mode != nowMode)
  {
    infoSettings.mode = nowMode;
    storePara();
  }

  Mode_Switch();
}
