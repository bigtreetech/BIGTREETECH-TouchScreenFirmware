#include "HD44780_Simulator.h"
#include "includes.h"
#include "GUI.h"
#include "../../Configuration.h"
#include "HD44780.h"
#include "ST7920_Simulator.h"

#ifdef LCD2004_simulator
uint8_t HD44780_CGRAM[8][8]; // [64*2] = [4 * 16*2*8], means 4 * [16*16] bitmap font,

HD44780_PIXEL HD44780 = {
  .x = 0, // current x pixel, range is 0 - 127
  .y = 0, // current y pixel, range is 0 - 63
  .address_is_y = 1, // Extended Instruction, The first address set Y, second address set X
};

HD44780_REG HD44780_reg = {
  .fs = //0x00, // default basic instruction, 8-bit MPU interface, Graphic display OFF
        0x26, // default extended instruction, 4-bit MPU interface, Graphic display On
  .bi = {
    .ems  = 0x02, // Cursor move to right ,DDRAM address counter (AC) plus 1
    .dc   = 0x04, // Display, cursor and blink are ALL OFF
    .cdsc = 0x00, // No cursor or display shift operation
  },
  .reverse = 0, // Begin with normal and toggle to reverse
  .cgram = 0,
  .data_type = HD44780_DATA_DDRAM,
};

/*** Basic instruction ***/
// cmd : 1 << 0
void HD44780_BI10_DisplayClear(uint8_t *cmd)
{
  GUI_FillRectColor(XSTART,
                    YSTART,
                    LCD_WIDTH,
                    LCD_HEIGHT,
                    infoSettings.marlin_mode_bg_color);
  HD44780.x = HD44780.y = 0;
  HD44780_reg.bi.ems.id = 1;
}
// cmd : 1 << 1
void HD44780_BI11_ReturnHome(uint8_t *cmd)
{
  // Set address counter (AC) to "00H".
  HD44780.x = HD44780.y = 0;
}
//cmd : 1 << 2
void HD44780_BI12_EntryModeSet(uint8_t *cmd)
{
  HD44780_reg.bi.ems.reg = *cmd;
}
//cmd : 1 << 3
void HD44780_BI13_DisplayControl(uint8_t *cmd)
{
  HD44780_reg.bi.dc.reg = *cmd;
}
//cmd : 1 << 4
void HD44780_BI14_CursorDisplayControl(uint8_t *cmd)
{
  HD44780_reg.bi.cdsc.reg = *cmd;
}
//cmd : 1 << 5
void HD44780_CI15_FunctionSet(uint8_t *cmd)
{
  HD44780_reg.fs.reg = *cmd;
}
//cmd : 1 << 6
void HD44780_BI16_SetCGRAMAddress(uint8_t *cmd)
{
  HD44780_reg.bi.cgrama.reg = *cmd;
  uint8_t address = HD44780_reg.bi.cgrama.ac;
  // Set CGRAM address 
  HD44780.y = (address >> 3) & 0x07;
  HD44780.x = 0;
  HD44780_reg.data_type = HD44780_DATA_CGRAM;
}
//cmd : 1 << 7
void HD44780_BI17_SetDDRAMAddress(uint8_t *cmd)
{
  HD44780_reg.bi.ddrama.reg = *cmd;            
  // Set DDRAM address                     
  // x is 0-20 . y is 0-4                       
  if(0x80 <= *cmd && *cmd <= 0x93){                // First line AC range is  80H … 93h
    HD44780.y = 0;
    HD44780.x = *cmd - 0x80;
  }
  if(0xC0 <= *cmd && *cmd <= 0xD3){                // Second line AC range is C0H … D3H
    HD44780.y = 1;
    HD44780.x = *cmd - 0xC0;
  }
  if(0x94 <= *cmd && *cmd <= 0xA7){                // Third line AC range is  94H … A7H
    HD44780.y = 2;
    HD44780.x = *cmd - 0x94;
  }
  if(0xD4 <= *cmd && *cmd <= 0xE7){                // Fourth line AC range is D4H … E7H
    HD44780.y = 3;
    HD44780.x = *cmd - 0xD4;
  }
  HD44780_reg.data_type = HD44780_DATA_DDRAM;
}

void HD44780_DrawPixel(int16_t x, int16_t y, bool isForeGround, bool isFont)
{
  if(isFont){
  GUI_FillRectColor(XSTART + FONT_PIXEL * x,
                    YSTART + FONT_PIXEL * y,
                    XSTART + FONT_PIXEL * (x+1),
                    YSTART + FONT_PIXEL * (y+1),
                    isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  }
  else{
  GUI_FillRectColor(XSTART + BITMAP_PIXEL * x,
                    YSTART + YOFFSET + BITMAP_PIXEL * y,
                    XSTART + BITMAP_PIXEL * (x+1),
                    YSTART + YOFFSET + BITMAP_PIXEL * (y+1),
                    isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  }
}

// Display CGRAM fonts
void HD44780_DispDDRAM(uint8_t *data)
{
  uint16_t i  = 0,
           ex = 0,  
           ey = 0;
  if(*data < 8){                          // 5*8 bitmap
    ex = HD44780.x * 6 + 6;
    ey = HD44780.y * 12 + 8;
    for(uint8_t y = HD44780.y * 12; y < ey; y++){
      uint8_t temp = HD44780_CGRAM[(*data)][i++];
      for (uint8_t x = HD44780.x * 6; x < ex; x++) {
        HD44780_DrawPixel(x, y, temp & 0x10, 0);
        temp <<= 1;
      }
    }
  }
  else{                                   //font
    if(data == NULL || *data < ' ' || *data > '~') return;  
    ex = HD44780.x * BYTE_WIDTH + BYTE_WIDTH-1;
    ey = HD44780.y * BYTE_HEIGHT + BYTE_HEIGHT-1;
    CHAR_INFO info = {.bytes = 0};

    getCharacterInfo(data, &info);
    uint8_t x = 0,
            y = 0,
            j = 0;
    uint16_t bitMapSize = (info.pixelHeight * info.pixelWidth / 8);
    uint8_t  font[BYTE_HEIGHT * BYTE_HEIGHT / 8]; // TODO: match bitMapSize
    uint32_t temp = 0;

    W25Qxx_ReadBuffer(font, info.bitMapAddr, bitMapSize);

    for(x = HD44780.x * BYTE_WIDTH; x < ex; x++)
    {
      for(j=0; j < (info.pixelHeight + 8-1)/8; j++)
      {
        temp <<= 8;
        temp |= font[i++];
      }

      for(y = HD44780.y * BYTE_HEIGHT; y < ey ;y++)
      {
        HD44780_DrawPixel(x, y, temp & (1<<(info.pixelHeight-1)), 1);
        temp <<= 1;
      }
    }
  }
  HD44780.x ++ ;
}

void HD44780_SetCGRAMData(uint8_t *data)
{
  HD44780_CGRAM[HD44780.y][HD44780.x++] = *data;
  if (HD44780.x > 7) {
    HD44780.x = 0;
    HD44780.y = (HD44780.y + 1) % (8 - 1);
  }
}

const HD44780_CMD cmdSettings[8] = {
// Basic Instruction                 Extended Instruction
HD44780_BI10_DisplayClear,                  // cmd 1 << 0
HD44780_BI11_ReturnHome,                    // cmd 1 << 1
HD44780_BI12_EntryModeSet,                  // cmd 1 << 2
HD44780_BI13_DisplayControl,                // cmd 1 << 3
HD44780_BI14_CursorDisplayControl,          // cmd 1 << 4
HD44780_CI15_FunctionSet,                   // cmd 1 << 5
HD44780_BI16_SetCGRAMAddress,               // cmd 1 << 6
HD44780_BI17_SetDDRAMAddress,               // cmd 1 << 7
};

void HD44780_ParseWCmd(uint8_t *cmd)
{
  for (int8_t i = 7; i >= 0; i--) {
    if (*cmd & (1 << i)) {
      (*cmdSettings[i])(cmd);
      break;
    }
  } 
}

void HD44780_ParseWData(uint8_t *data)
{
  switch (HD44780_reg.data_type) {
    case HD44780_DATA_DDRAM:
      HD44780_DispDDRAM(data);
      break;
    case HD44780_DATA_CGRAM:
      HD44780_SetCGRAMData(data);
      break;
  }
}

void HD44780_ParseRecv(uint8_t *val)
{
  if (*val == 0xff){              //0xff flag   next is instruction
    HD44780_queue.rIndex = (HD44780_queue.rIndex + 1) % HD44780_data_MAX;
    val = HD44780_queue.data + HD44780_queue.rIndex;
    HD44780_ParseWCmd(val);
  }
  else{
    HD44780_ParseWData(val);
  }
}

void menuHD44780(void)
{
  GUI_Clear(infoSettings.marlin_mode_bg_color);
  GUI_SetColor(infoSettings.marlin_mode_font_color);
  GUI_SetBkColor(infoSettings.marlin_mode_bg_color);
  if(infoSettings.marlin_mode_showtitle == 1){
    STRINGS_STORE tempST;
    W25Qxx_ReadBuffer((uint8_t *)&tempST,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));
    GUI_DispStringInRect(0, 0, LCD_WIDTH, SIMULATOR_YSTART, (uint8_t *)tempST.lcd2004_title);
  }
  HD44780_Config();

  while(infoMenu.menu[infoMenu.cur] == menuHD44780)
  {
    while(HD44780_queue.rIndex != HD44780_queue.wIndex)
    {
      HD44780_ParseRecv(HD44780_queue.data + HD44780_queue.rIndex);
      HD44780_queue.rIndex = (HD44780_queue.rIndex + 1) % HD44780_data_MAX;
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
  HD44780_DeConfig();
}
#endif