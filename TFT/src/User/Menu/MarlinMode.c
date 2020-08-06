#include "MarlinMode.h"
#include "HD44780.h"
#include "includes.h"

CIRCULAR_QUEUE marlinQueue;

typedef void (*CB_INIT)(void);
typedef void (*CB_PARSE)(uint8_t);

const CB_INIT marlinInit[] = {HD44780_Config, SPI_Slave};
const CB_INIT marlinDeInit[] = {HD44780_DeConfig, SPI_SlaveDeInit};
const CB_PARSE marlinParse[] = {HD44780_ParseRecv, ST7920_ParseRecv};

void menuMarlinMode(void)
{
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

  marlinInit[infoSettings.marlin_type]();

  while(infoMenu.menu[infoMenu.cur] == menuMarlinMode)
  {
    while(marlinQueue.index_r != marlinQueue.index_w)
    {
      marlinParse[infoSettings.marlin_type](marlinQueue.data[marlinQueue.index_r]);
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

  marlinDeInit[infoSettings.marlin_type]();

  free(marlinQueue.data);
  marlinQueue.data = NULL;
}
