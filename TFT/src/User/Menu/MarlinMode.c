#include "MarlinMode.h"
#include "includes.h"
#include "spi_slave.h"
#include "HD44780.h"

#ifdef HAS_EMULATOR

typedef void (*CB_INIT)(CIRCULAR_QUEUE *);
typedef void (*CB_DEINIT)(void);
typedef bool (*CB_DATA)(uint8_t *);
typedef void (*CB_PARSE)(uint8_t);

void menuMarlinMode(void)
{
  CB_INIT   marlinInit = NULL;
  CB_DEINIT marlinDeInit = NULL;
  CB_DATA   marlinGetData = NULL;
  CB_PARSE  marlinParse = NULL;

  GUI_Clear(infoSettings.marlin_bg_color);
  GUI_SetColor(infoSettings.marlin_font_color);
  GUI_SetBkColor(infoSettings.marlin_bg_color);

  if (infoSettings.marlin_show_title == 1)
  {
    STRINGS_STORE tempST;
    W25Qxx_ReadBuffer((uint8_t *)&tempST, STRINGS_STORE_ADDR, sizeof(STRINGS_STORE));
    GUI_DispStringInRect(0, 0, LCD_WIDTH, ST7920_GYSTART_FULLSCREEN, (uint8_t *)tempST.marlin_title);
  }

  #if defined(ST7920_EMULATOR)
    ST7920 st7920;

    if (infoSettings.marlin_type == LCD12864)
    {
      marlinInit = SPI_Slave;
      marlinDeInit = SPI_SlaveDeInit;
      marlinGetData = SPI_SlaveGetData;
      marlinParse = ST7920_ParseRecv;

      ST7920_Init(&st7920);
    }
  #endif

  #if defined(LCD2004_EMULATOR)
    if (infoSettings.marlin_type == LCD2004)
    {
      marlinInit = HD44780_Config;
      marlinDeInit = HD44780_DeConfig;
      marlinGetData = HD44780_getData;
      marlinParse = HD44780_ParseRecv;
    }
  #endif

  CIRCULAR_QUEUE marlinQueue;
  uint8_t data;

  marlinInit(&marlinQueue);

  while (MENU_IS(menuMarlinMode))
  {
    while (marlinGetData(&data))
    {
      marlinParse(data);
    }

    #if LCD_ENCODER_SUPPORT
      if (Touch_Enc_ReadBtn(LCD_ENC_BUTTON_INTERVAL))
        LCD_Enc_SendPulse(1);

      LCD_Enc_SendPulse(Touch_Enc_ReadPos());
    #endif

    Mode_CheckSwitching();

    if (infoSettings.serial_always_on == ENABLED)
    {
      loopBackEnd();
    }
    #if defined(SCREEN_SHOT_TO_SD) || defined(LCD_LED_PWM_CHANNEL)  // loopScreenShot() and LCD_CheckDimming() are invoked by loopBackEnd(),
      else                                                          // so we guarantee they are invoked only once
      {
        #ifdef SCREEN_SHOT_TO_SD
          loopScreenShot();
        #endif

        #ifdef LCD_LED_PWM_CHANNEL
          LCD_CheckDimming();
        #endif
      }
    #endif
  }

  marlinDeInit();
}

#endif
