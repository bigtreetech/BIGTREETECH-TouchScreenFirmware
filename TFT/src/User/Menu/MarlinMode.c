#include "MarlinMode.h"
#include "HD44780.h"
#include "includes.h"

#if defined(ST7920_SPI) || defined(LCD2004_simulator)

CIRCULAR_QUEUE marlinQueue;

typedef void (*CB_INIT)(void);
typedef void (*CB_PARSE)(uint8_t);

void menuMarlinMode(void)
{
  CB_INIT  marlinInit = NULL;
  CB_INIT  marlinDeInit = NULL;
  CB_PARSE marlinParse = NULL;

  marlinQueue.data = malloc(QUEUE_MAX_BYTE);
  while(!marlinQueue.data); // malloc failed

  GUI_Clear(infoSettings.marlin_mode_bg_color);
  GUI_SetColor(infoSettings.marlin_mode_font_color);
  GUI_SetBkColor(infoSettings.marlin_mode_bg_color);
  if(infoSettings.marlin_mode_showtitle == 1){
    STRINGS_STORE tempST;
    W25Qxx_ReadBuffer((uint8_t *)&tempST,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));
    GUI_DispStringInRect(0, 0, LCD_WIDTH, SIMULATOR_YSTART, (uint8_t *)tempST.marlin_title);
  }

#if defined(ST7920_SPI)
  if (infoSettings.marlin_type == LCD12864) {
    marlinInit = SPI_Slave;
    marlinDeInit = SPI_SlaveDeInit;
    marlinParse = ST7920_ParseRecv;
  }
#elif defined(LCD2004_simulator)
  if (infoSettings.marlin_type == LCD2004) {
    marlinInit = HD44780_Config;
    marlinDeInit = HD44780_DeConfig;
    marlinParse = HD44780_ParseRecv;
  }
#endif

  marlinInit();

  while(infoMenu.menu[infoMenu.cur] == menuMarlinMode)
  {
    while(marlinQueue.index_r != marlinQueue.index_w)
    {
      marlinParse(marlinQueue.data[marlinQueue.index_r]);
      marlinQueue.index_r = (marlinQueue.index_r + 1) % QUEUE_MAX_BYTE;
    }
    #if LCD_ENCODER_SUPPORT
      sendEncoder(LCD_ReadTouch());

      if(LCD_BtnTouch(LCD_BUTTON_INTERVALS))
        sendEncoder(1);
      loopCheckEncoder();
    #endif
    loopCheckMode();

    if (infoSettings.serial_alwaysOn == 1)
    {
      loopBackEnd();
    }
  }

  marlinDeInit();

  free(marlinQueue.data);
  marlinQueue.data = NULL;
}

#endif
