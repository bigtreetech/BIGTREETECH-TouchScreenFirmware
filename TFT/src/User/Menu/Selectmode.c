#include "includes.h"
#include "xpt2046.h"
#include "GPIO_Init.h"
#include "Selectmode.h"


const GUI_RECT rect_of_mode[SELECTMODE]={
  //2 select icon
  {1*SPACE_SELEX+0*selecticonw,SPACE_SELEY,1*SPACE_SELEX+1*selecticonw,SPACE_SELEY+selecticonw},
  {3*SPACE_SELEX+1*selecticonw,SPACE_SELEY,3*SPACE_SELEX+2*selecticonw,SPACE_SELEY+selecticonw},
};

u32 select_mode [SELECTMODE]={
    ICON_MARLIN,
    ICON_BIGTREE,
};

void show_selectICON(void)
{
    for(u8 i=0;i<SELECTMODE;i++)
    {
        lcd_frame_display(rect_of_mode[i].x0,rect_of_mode[i].y0-BYTE_HEIGHT,selecticonw,selecticonw,ICON_ADDR(select_mode[i]));
    }
    return ;
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

bool LCD_BtnTouch(uint16_t intervals)
{
	static u32 BtnTime = 0;
  u16 tx,ty;
  if(!XPT2046_Read_Pen())
  {
		TS_Get_Coordinates(&tx,&ty);
    if(OS_GetTimeMs() - BtnTime > intervals)
    {
			if(tx>LCD_WIDTH-LCD_WIDTH/5 && ty<LCD_HEIGHT/5)
      return true;
    }
  }
  else
  {
    BtnTime = OS_GetTimeMs();
  }
  return false;
}

#if 1
 uint8_t LCD_ReadTouch(void)
{
	u16 ex=0,ey=0;
  static u32 CTime = 0;
  static u16 sy;
	static bool MOVE = false;

	if(!XPT2046_Read_Pen() && CTime < OS_GetTimeMs())
  {
		TS_Get_Coordinates(&ex,&ey);
		if(!MOVE)
		sy = ey;

		MOVE = true;

		if((sy>ey) && ey!=0)
		{
            if(sy-ey > LCD_HEIGHT/9 && sy-ey < LCD_HEIGHT/7)//7-5
			{

				sy = ey;
				return 2;
			}
            return 0;
		}
    else
		{
            if(ey-sy > LCD_HEIGHT/9 && ey-sy < LCD_HEIGHT/7)
			{
				sy = ey;
				return 3;
			}
            return 0;
		}
	}
	else
	{
		CTime = OS_GetTimeMs();
		sy = ey =0;
		MOVE = false;
    return 0;
	}
}
#endif

#if LCD_ENCODER_SUPPORT
void Touch_Sw(uint8_t num)
{
  if(num==1 || num==2 || num ==3)
  {
  GPIO_InitSet(LCD_BTN_PIN, MGPIO_MODE_OUT_PP, 0);
	GPIO_InitSet(LCD_ENCA_PIN, MGPIO_MODE_OUT_PP, 0);
	GPIO_InitSet(LCD_ENCB_PIN, MGPIO_MODE_OUT_PP, 0);
  }
	switch(num)
	{
		case 0:
			break;
		case 1:
			GPIO_SetLevel(LCD_BTN_PIN, 0);
			GPIO_SetLevel(LCD_BTN_PIN, 1);
			break;
		case 2:
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			break;
		case 3:
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			break;
	}

  LCD_EncoderInit();
}

MKEY_VALUES MKeyGetValue(void)
{
  return (MKEY_VALUES)KEY_GetValue(sizeof(rect_of_mode)/sizeof(rect_of_mode[0]), rect_of_mode);
}

void selectmode(int8_t  nowMode)
{
	GUI_SetBkColor(BLACK);
    GUI_ClearRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH);
	GUI_ClearRect(0,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,text_startx,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH);

  if(nowMode==SERIAL_TSC)
  {
    GUI_SetColor(ST7920_FNCOLOR);
    GUI_DispStringInRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Touch Mode");
    GUI_SetColor(FONT_COLOR);
    GUI_DispStringInRect(0,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,text_startx,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Marlin Mode");
  }
  else
  {
    GUI_SetColor(ST7920_FNCOLOR);
    GUI_DispStringInRect(0,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,text_startx,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Marlin Mode");
    GUI_SetColor(FONT_COLOR);
    GUI_DispStringInRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+selecticonw+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+selecticonw+BYTE_WIDTH,(uint8_t *)"Touch Mode");
  }
}

#endif
