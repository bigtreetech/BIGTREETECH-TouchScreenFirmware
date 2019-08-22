#include "xpt2046.h"
#include "GPIO_Init.h"
#include "os_timer.h"
#include "stdbool.h"
#include "touch_process.h"
/***************************************** XPT2046 SPI ģʽ�ײ���ֲ�Ľӿ�********************************************/
//XPT2046 SPI��� - ʹ��ģ��SPI
_SW_SPI xpt2046;

//Ƭѡ
void XPT2046_CS_Set(u8 level)
{
  SW_SPI_CS_Set(&xpt2046, level);
}

//��д����
u8 XPT2046_ReadWriteByte(u8 TxData)
{		
  return SW_SPI_Read_Write(&xpt2046, TxData);			    
}

//XPT2046 SPI�ͱ��жϳ�ʼ��
void XPT2046_Init(void)
{
  //PA15-TPEN
  GPIO_InitSet(XPT2046_TPEN, MGPIO_MODE_IPN, 0);

  SW_SPI_Config(&xpt2046, _SPI_MODE3, 8, // 8bit
  XPT2046_CS,     //CS
  XPT2046_SCK,    //SCK
  XPT2046_MISO,   //MISO
  XPT2046_MOSI    //MOSI
  );
  XPT2046_CS_Set(1);
}

//�����ж�
u8 XPT2046_Read_Pen(void)
{
  return GPIO_GetLevel(XPT2046_TPEN);
}
/******************************************************************************************************************/


//��ȡ XPT2046 ת���õ�ADֵ
u16 XPT2046_Read_AD(u8 CMD)
{
  u16 ADNum;
  XPT2046_CS_Set(0);

  XPT2046_ReadWriteByte(CMD);
  ADNum=XPT2046_ReadWriteByte(0xff);
  ADNum= ((ADNum)<<8) | XPT2046_ReadWriteByte(0xff);
  ADNum >>= 4;         //XPT2046����ֻ��12bits,��������λ

  XPT2046_CS_Set(1);
  return ADNum;
}

#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
u16 XPT2046_Average_AD(u8 CMD)
{
  u16 i, j;
  u16 buf[READ_TIMES];
  u16 sum=0;
  u16 temp;
  for(i=0; i<READ_TIMES; i++) buf[i] = XPT2046_Read_AD(CMD);		 		    
  for(i=0; i<READ_TIMES-1; i++)//����
  {
    for(j=i+1; j<READ_TIMES; j++)
    {
      if(buf[i] > buf[j]) //��������
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  for(i=LOST_VAL; i < READ_TIMES-LOST_VAL; i++) sum += buf[i];
  temp = sum/(READ_TIMES-2*LOST_VAL);
  return temp;   
} 


#define ERR_RANGE 50 //��Χ 
u16 XPT2046_Repeated_Compare_AD(u8 CMD) 
{
  u16 ad1, ad2;
  ad1 = XPT2046_Average_AD(CMD);
  ad2 = XPT2046_Average_AD(CMD);

  if((ad2 <= ad1 && ad1 < ad2 + ERR_RANGE) 
  || (ad1 <= ad2 && ad2 < ad1 + ERR_RANGE)) //ǰ���������С�� ERR_RANGE
  {
    return (ad1+ad2)/2;
  }
  else return 0;	  
}

bool LCD_ReadPen(uint8_t intervals)
{
  static u32 TouchTime = 0;
  
  if(!XPT2046_Read_Pen())
  {
    if(OS_GetTime() - TouchTime > intervals)
    {
      return true;
    }
  }
  else
  {
    TouchTime = OS_GetTime();
  }
  return false;
}

bool LCD_BtnTouch(uint8_t intervals)
{
	static u32 BtnTime = 0;
  u16 tx,ty;
  if(!XPT2046_Read_Pen())
  {
		TS_Get_Coordinates(&tx,&ty);
    if(OS_GetTime() - BtnTime > intervals)
    {
			if(tx>280 && ty<40)
      return true;
    }
  }
  else
  {
    BtnTime = OS_GetTime();
  }
  return false;
}

 uint8_t LCD_ReadTouch(void)
{
	u16 ex=0,ey=0;
  static u32 CTime = 0;
  static u16 sx,sy;
	static bool MOVE = false;
	
	if(!XPT2046_Read_Pen() && CTime < OS_GetTime())
  {
		TS_Get_Coordinates(&ex,&ey);
		if(!MOVE)
			sy = ey;
			
		MOVE = true;
			
		if((ey>sy) && sy!=0)
		{
			if(ey > sy+25)
			{
				MOVE = false;
				sy = ey = 0;
				return 3;
			}
		}
		else if((sy>ey) && ey!=0)
		{
			if(sy > ey+25)
			{
				MOVE = false;
				sy = ey =0;
				return 2;
			}
		}	
	}
	else
	{
		CTime = OS_GetTime();
		sy = ey =0;
		MOVE = false;
	}
	
	return 0;	
}

void Touch_Sw(uint8_t num)
{
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
}

