#include "includes.h"
#include "xpt2046.h"
#include "GPIO_Init.h"
#include "Selectmode.h"

#ifdef ST7920_SPI

bool skipMode = false;

const GUI_RECT rect_of_mode[SELECTMODE]={
  //2 select icon
  {1*SPACE_SELEX+0*ICON_WIDTH, SPACE_SELEY, 1*SPACE_SELEX+1*ICON_WIDTH, SPACE_SELEY+ICON_HEIGHT},
  {3*SPACE_SELEX+1*ICON_WIDTH, SPACE_SELEY, 3*SPACE_SELEX+2*ICON_WIDTH, SPACE_SELEY+ICON_HEIGHT},
};

u32 select_mode [SELECTMODE]={
    ICON_MARLIN,
    ICON_BIGTREETECH,
};

void show_selectICON(void)
{
    for(u8 i=0;i<SELECTMODE;i++)
    {
      lcd_frame_display(rect_of_mode[i].x0, rect_of_mode[i].y0, ICON_WIDTH, ICON_HEIGHT,ICON_ADDR(select_mode[i]));
    }

	const GUI_RECT mode_title_rect[SELECTMODE] = {
		{0,        		rect_of_mode[0].y1 + BYTE_HEIGHT/2, 	text_startx,  rect_of_mode[0].y1 + BYTE_HEIGHT/2 + BYTE_HEIGHT},
		{text_startx, rect_of_mode[0].y1 + BYTE_HEIGHT/2, 	LCD_WIDTH, 		rect_of_mode[0].y1 + BYTE_HEIGHT/2 + BYTE_HEIGHT}
	};

	//GUI_ClearPrect(&mode_title_rect[1]);
	//GUI_ClearPrect(&mode_title_rect[0]);
	GUI_DispStringInPrect(&mode_title_rect[0],(uint8_t *)"Marlin Mode");
	GUI_DispStringInPrect(&mode_title_rect[1],(uint8_t *)"Touch Mode");
}

bool LCD_ReadPen(uint16_t intervals)
{
  static u32 TouchTime = 0;
  if(!XPT2046_Read_Pen())
  {
    if(OS_GetTimeMs() - TouchTime > intervals)
    {
      return true;
    }
  }
  else
  {
    TouchTime = OS_GetTimeMs();
  }
  return false;
}

MKEY_VALUES MKeyGetValue(void)
{
  return (MKEY_VALUES)KEY_GetValue(sizeof(rect_of_mode)/sizeof(rect_of_mode[0]), rect_of_mode);
}

void selectmode(int8_t  nowMode)
{

	GUI_RestoreColorDefault();
	u8 border_off = 4;

  if(nowMode==SERIAL_TSC)
  {
		GUI_SetColor(BACKGROUND_COLOR);
		GUI_DrawRect(rect_of_mode[0].x0 - border_off, rect_of_mode[0].y0 - border_off,rect_of_mode[0].x1 + border_off, rect_of_mode[0].y1 + border_off);
		GUI_SetColor(YELLOW);
		GUI_DrawRect(rect_of_mode[1].x0 - border_off, rect_of_mode[1].y0 - border_off,rect_of_mode[1].x1 + border_off, rect_of_mode[1].y1 + border_off);
  }
  else
  {
		GUI_SetColor(BACKGROUND_COLOR);
		GUI_DrawRect(rect_of_mode[1].x0 - border_off, rect_of_mode[1].y0 - border_off,rect_of_mode[1].x1 + border_off, rect_of_mode[1].y1 + border_off);
		GUI_SetColor(YELLOW);
		GUI_DrawRect(rect_of_mode[0].x0 - border_off, rect_of_mode[0].y0 - border_off,rect_of_mode[0].x1 + border_off, rect_of_mode[0].y1 + border_off);

  }
}

void loopCheckMode(void)
{
//  #ifndef CLEAN_MODE_SWITCHING_SUPPORT
//  IDEALLY I would like to be able to swap even when the TFT is in printing mode
//  but before I can allow that I need a way to make sure that we swap back into the right mode (and correct screen)
//  and I really want a reliable way to DETECT that the TFT should be in printing mode even when the print was started externally.
    if(isPrinting() || skipMode)  return;
//  #endif
  if(LCD_ReadPen(LCD_CHANGE_MODE_INTERVALS))
  {
    infoMenu.menu[++infoMenu.cur] = menuMode;
  }
}

void menuMode(void)
{
  #if defined(ST7920_BANNER_TEXT)
    RADIO modeRadio = {
      {(u8*)"Serial Touch Screen", (u8*)ST7920_BANNER_TEXT, (u8*)"LCD2004 Simulator"},
      SIMULATOR_XSTART, SIMULATOR_YSTART,
      BYTE_HEIGHT*2, 2,
      0
      };
  #else
    RADIO modeRadio = {
      {(u8*)"Serial Touch Screen", (u8*)"12864 Simulator", (u8*)"LCD2004 Simulator"},
      SIMULATOR_XSTART, SIMULATOR_YSTART,
      BYTE_HEIGHT*2, 2,
      0
      };
  #endif

  MKEY_VALUES  key_num;
  bool keyback = false;

  int16_t /*nowEncoder =*/ encoderPosition = 0;
  int8_t  nowMode = modeRadio.select = infoSettings.mode;

  GUI_Clear(BACKGROUND_COLOR);
  //RADIO_Create(&modeRadio);
  if (infoSettings.serial_alwaysOn != 1)
  {
    Serial_ReSourceDeInit();
  }
  resetInfoFile();
  SD_DeInit();

  show_selectICON();
  TSC_ReDrawIcon = NULL; // Disable icon redraw callback function

  selectmode(nowMode);

  #if LCD_ENCODER_SUPPORT
    while(!XPT2046_Read_Pen() || encoder_ReadBtn(LCD_BUTTON_INTERVALS));      //wait for button release
  #else
    while(!XPT2046_Read_Pen());      //wait for touch release
  #endif

  while(infoMenu.menu[infoMenu.cur] == menuMode)
  {
    key_num = MKeyGetValue();

    if(encoderPosition)
    {
      nowMode = limitValue(0, nowMode + encoderPosition, modeRadio.num - 1);
      selectmode(nowMode);
      encoderPosition = 0;
    }

    #if LCD_ENCODER_SUPPORT
      if(encoder_ReadBtn(LCD_BUTTON_INTERVALS))
      {
        break;
      }
      loopCheckEncoderSteps();
      loopCheckEncoder();
    #endif

    if (infoSettings.serial_alwaysOn == 1)
    {
      loopBackEnd();
    }

    if(key_num==MKEY_1)
		{
      keyback = true;
      nowMode = SERIAL_TSC;
		}

		if(key_num==MKEY_0)
		{
      keyback = true;
      nowMode = LCD12864;
		}

    if(keyback)
    {
      #if LCD_ENCODER_SUPPORT
        sendEncoder(2);
        sendEncoder(1);
			#endif
      while(!XPT2046_Read_Pen()){};
			break;
    }
  }

  if(infoSettings.mode != nowMode)
  {
    infoSettings.mode = nowMode;
    storePara();
  }

  infoMenuSelect();
}


#endif
