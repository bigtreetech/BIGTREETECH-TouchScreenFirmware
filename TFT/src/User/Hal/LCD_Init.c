#include "LCD_Init.h"
#include "GPIO_Init.h"
#include "includes.h"


#ifdef LCD_LED_PIN
void LCD_LED_On()
{
  GPIO_SetLevel(LCD_LED_PIN, 1);
}
void LCD_LED_Off()
{
  GPIO_SetLevel(LCD_LED_PIN, 0);
}

const  uint32_t LCD_BRIGHTNESS[ITEM_BRIGHTNESS_NUM] = {
  LCD_5_PERCENT,
  LCD_10_PERCENT,
  LCD_20_PERCENT,
  LCD_30_PERCENT,
  LCD_40_PERCENT,
  LCD_50_PERCENT,
  LCD_60_PERCENT,
  LCD_70_PERCENT,
  LCD_80_PERCENT,
  LCD_90_PERCENT,
  LCD_100_PERCENT
};
const LABEL itemBrightness[ITEM_BRIGHTNESS_NUM] = {
  //item value text(only for custom value)
  LABEL_5_PERCENT,
  LABEL_10_PERCENT,
  LABEL_20_PERCENT,
  LABEL_30_PERCENT,
  LABEL_40_PERCENT,
  LABEL_50_PERCENT,
  LABEL_60_PERCENT,
  LABEL_70_PERCENT,
  LABEL_80_PERCENT,
  LABEL_90_PERCENT,
  LABEL_100_PERCENT
};

void LCD_LED_PWM_Init()
{
#if defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_12;  //LCD_LED_PIN PD12
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);

  TIM_OCInitTypeDef outputChannelInit = {0,};
    outputChannelInit.TIM_OCMode      = TIM_OCMode_PWM1;
    outputChannelInit.TIM_OCPolarity  = TIM_OCPolarity_High;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_Pulse       = F_CPUM;
  TIM_OC1Init(TIM4, &outputChannelInit);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
#endif
}

void LCD_LED_Init(void)
{
  LCD_LED_Off();
  GPIO_InitSet(LCD_LED_PIN, MGPIO_MODE_OUT_PP, 0);
}
#endif

#if defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)
//ILI9488
void LCD_init_RGB(void)
{
  LCD_WR_REG(0xC0);
  LCD_WR_DATA(0x0c);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xC1);
  LCD_WR_DATA(0x44);
  LCD_WR_REG(0xC5);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x16);
  LCD_WR_DATA(0x80);
  LCD_WR_REG(0x36);
  LCD_WR_DATA(0x28);
  LCD_WR_REG(0x3A); //Interface Mode Control
  LCD_WR_DATA(0x55);
  LCD_WR_REG(0XB0);  //Interface Mode Control
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB1);   //Frame rate 70HZ
  LCD_WR_DATA(0xB0);
  LCD_WR_REG(0xB4);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xB6); //RGB/MCU Interface Control
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xE9);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0XF7);
  LCD_WR_DATA(0xA9);
  LCD_WR_DATA(0x51);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x82);
  LCD_WR_REG(0x11);
  Delay_ms(120);
  LCD_WR_REG(0x29);
}
#elif defined(TFT35_V1_1) || defined(TFT35_V1_0)
//RM68042
void LCD_init_RGB(void)
{
  LCD_WR_REG(0X11);
  Delay_ms(20);
  LCD_WR_REG(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
  LCD_WR_DATA(0X07);
  LCD_WR_DATA(0X42);
  LCD_WR_DATA(0X1C);
  LCD_WR_REG(0XD1);//VCOMH VCOM_AC amplitude setting
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X19);
  LCD_WR_DATA(0X16);
  LCD_WR_REG(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
  LCD_WR_DATA(0X01);
  LCD_WR_DATA(0X11);
  LCD_WR_REG(0XE4);
  LCD_WR_DATA(0X00A0);
  LCD_WR_REG(0XF3);
  LCD_WR_DATA(0X0000);
  LCD_WR_DATA(0X002A);
  LCD_WR_REG(0XC0);//REV SM GS
  LCD_WR_DATA(0X10);
  LCD_WR_DATA(0X3B);
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X02);
  LCD_WR_DATA(0X11);
  LCD_WR_REG(0XC5);// Frame rate setting = 72HZ  when setting 0x03
  LCD_WR_DATA(0X03);
  LCD_WR_REG(0XC8);//Gamma setting
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X35);
  LCD_WR_DATA(0X23);
  LCD_WR_DATA(0X07);
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X04);
  LCD_WR_DATA(0X45);
  LCD_WR_DATA(0X53);
  LCD_WR_DATA(0X77);
  LCD_WR_DATA(0X70);
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X04);
  LCD_WR_REG(0X20);//Exit invert mode
  LCD_WR_REG(0X36);
  LCD_WR_DATA(0X28);
  LCD_WR_REG(0X3A);
  LCD_WR_DATA(0X55);//16λģʽ
  Delay_ms(120);
  LCD_WR_REG(0X29);
}

#elif defined(TFT28_V1_0)

void LCD_init_RGB(void)
{
  Delay_ms(50); // delay 50 ms

  LCD_WR_REG(0xCF);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC1);
  LCD_WR_DATA(0X30);

  LCD_WR_REG(0xED);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0X12);
  LCD_WR_DATA(0X81);

  LCD_WR_REG(0xE8);   /// @diff :Driver timing control A
  LCD_WR_DATA(0x8A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x78);

  LCD_WR_REG(0xEA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0xCB);
  LCD_WR_DATA(0x39);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x02);

  LCD_WR_REG(0xF7);
  LCD_WR_DATA(0x20);

  LCD_WR_REG(0xC0);    /// @diff Power control
  LCD_WR_DATA(0x25);   // VRH[5:0]

  LCD_WR_REG(0xC1);    /// @diff control
  LCD_WR_DATA(0x12);   // SAP[2:0];BT[3:0]

  LCD_WR_REG(0xC5);    /// @diff VCM control
  LCD_WR_DATA(0x33);
  LCD_WR_DATA(0x3C);

  LCD_WR_REG(0xC7);    ///@diff VCM control2
  LCD_WR_DATA(0x9A);

  LCD_WR_REG(0xB1);    /// @diff Frame Rate Control
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x15);

  LCD_WR_REG(0x3A);
  LCD_WR_DATA(0x55);

  LCD_WR_REG(0x36);    // Memory Access Control
  LCD_WR_DATA(0x68);

  LCD_WR_REG(0xB6);    // Display Function Control
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0xA2);

  LCD_WR_REG(0xF2);    // 3Gamma Function Disable
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0x26);    // Gamma curve selected
  LCD_WR_DATA(0x01);

  LCD_WR_REG(0xE0);    /// @diff Set Gamma
  LCD_WR_DATA(0x1F);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x47);
  LCD_WR_DATA(0xC8);
  LCD_WR_DATA(0x37);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x14);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0XE1);    /// @diff Set Gamma
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x25);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x38);
  LCD_WR_DATA(0x48);
  LCD_WR_DATA(0x48);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x35);
  LCD_WR_DATA(0x37);
  LCD_WR_DATA(0x1F);

  LCD_WR_REG(0x2A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x3f);

  LCD_WR_REG(0x2B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xef);

  LCD_WR_REG(0x11); //Exit Sleep
  Delay_ms(120);
  LCD_WR_REG(0x29); //display on
}


#elif defined(TFT28_V3_0)
void LCD_init_RGB(void)
{
 	LCD_WR_REG(0x11);
	Delay_ms(120); //Delay 120ms
	//------------------------------display and color format setting--------------------------------//
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x68);
	LCD_WR_REG(0x3a);
	LCD_WR_DATA(0x05);
	//--------------------------------ST7789V Frame rate setting----------------------------------//
	LCD_WR_REG(0xb2);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x33);
	LCD_WR_REG(0xb7);
	LCD_WR_DATA(0x35);
	//---------------------------------ST7789V Power setting--------------------------------------//
	LCD_WR_REG(0xbb);
	LCD_WR_DATA(0x28);
	LCD_WR_REG(0xc0);
	LCD_WR_DATA(0x2c);
	LCD_WR_REG(0xc2);
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xc3);
	LCD_WR_DATA(0x0b);
	LCD_WR_REG(0xc4);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xc6);
	LCD_WR_DATA(0x0f);
	LCD_WR_REG(0xd0);
	LCD_WR_DATA(0xa4);
	LCD_WR_DATA(0xa1);
	//--------------------------------ST7789V gamma setting---------------------------------------//
	LCD_WR_REG(0xe0);
	LCD_WR_DATA(0xd0);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x2a);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x55);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x3a);
	LCD_WR_DATA(0x0b);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xe1);
	LCD_WR_DATA(0xd0);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x0a);
	LCD_WR_DATA(0x0b);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x4a);
	LCD_WR_DATA(0x2b);
	LCD_WR_DATA(0x1b);
	LCD_WR_DATA(0x1c);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x1f);
	LCD_WR_REG(0x29);

}


#elif defined(TFT24_V1_1)
void LCD_init_RGB(void)
{
  Delay_ms(120);             // Delay 120 ms
  //************* Start Initial Sequence **********//
  LCD_WR_REG(0xCF);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC1);
  LCD_WR_DATA(0X30);
  LCD_WR_REG(0xED);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0X12);
  LCD_WR_DATA(0X81);
  LCD_WR_REG(0xE8);
  LCD_WR_DATA(0x85);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x79);
  LCD_WR_REG(0xCB);
  LCD_WR_DATA(0x39);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xF7);
  LCD_WR_DATA(0x20);
  LCD_WR_REG(0xEA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC0); //Power control
  LCD_WR_DATA(0x1D); //VRH[5:0]
  LCD_WR_REG(0xC1); //Power control
  LCD_WR_DATA(0x12); //SAP[2:0];BT[3:0]
  LCD_WR_REG(0xC5); //VCM control
  LCD_WR_DATA(0x33);
  LCD_WR_DATA(0x3F);
  LCD_WR_REG(0xC7); //VCM control
  LCD_WR_DATA(0x92);
  LCD_WR_REG(0x3A); // Memory Access Control
  LCD_WR_DATA(0x55);
  LCD_WR_REG(0x36);    // Memory Access Control
  LCD_WR_DATA(0x68);
  LCD_WR_REG(0xB6);    // Display Function Control
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0xA2);
  LCD_WR_REG(0xB1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x12);
  LCD_WR_REG(0x44);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xF2); // 3Gamma Function Disable
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0x26); //Gamma curve selected
  LCD_WR_DATA(0x01);
  LCD_WR_REG(0xE0); //Set Gamma
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x1B);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x48);
  LCD_WR_DATA(0xB8);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x0C);
  LCD_WR_DATA(0x09);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0XE1); //Set Gamma
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x38);
  LCD_WR_DATA(0x47);
  LCD_WR_DATA(0x4B);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x13);
  LCD_WR_DATA(0x06);
  LCD_WR_DATA(0x30);
  LCD_WR_DATA(0x38);
  LCD_WR_DATA(0x0F);
  LCD_WR_REG(0x11); //Exit Sleep
  Delay_ms(120);
  LCD_WR_REG(0x29); //Display on
}
#elif defined(MKS_32_V1_4)

void LCD_init_RGB(void) 
{
  Delay_ms(50); // delay 50 ms 
  
 LCD_WR_REG(0xFE);                     // 
 LCD_WR_REG(0xEF);
 LCD_WR_REG(0x3A);
 LCD_WR_DATA(5);
 LCD_WR_REG(0X36);
LCD_WR_DATA(0x64);
LCD_WR_REG(0xE8);
LCD_WR_DATA(0x12);
LCD_WR_DATA(0x22);
LCD_WR_REG(0xE3);
LCD_WR_DATA(1);
LCD_WR_DATA(4);
LCD_WR_REG(0xA5);
LCD_WR_DATA(0x40);
LCD_WR_DATA(0x40);
LCD_WR_REG(0xA4);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x44);
LCD_WR_REG(0xAB);
LCD_WR_DATA(8);
LCD_WR_REG(0xAA);
LCD_WR_DATA(0x88);
LCD_WR_DATA(0x88);
LCD_WR_REG(0xAE);
LCD_WR_DATA(0xB);
LCD_WR_REG(0xAC);
LCD_WR_DATA(0);
LCD_WR_REG(0xAF);
LCD_WR_DATA(0x77);
LCD_WR_REG(0xAD);
LCD_WR_DATA(0x77);

LCD_WR_REG(0x2A);
LCD_WR_DATA(0);
LCD_WR_DATA(0);
LCD_WR_DATA(0);
LCD_WR_DATA(0xEF);
LCD_WR_REG(0x2B);
LCD_WR_DATA(0);
LCD_WR_DATA(0);
LCD_WR_DATA(1);
LCD_WR_DATA(0x3F);

LCD_WR_REG(0x2C);

LCD_WR_REG(0xF0);
LCD_WR_DATA(2);
LCD_WR_DATA(0);
LCD_WR_DATA(0);
LCD_WR_DATA(1);
LCD_WR_DATA(1);
LCD_WR_DATA(7);

LCD_WR_REG(0xF1);
LCD_WR_DATA(1);
LCD_WR_DATA(3);
LCD_WR_DATA(0);
LCD_WR_DATA(0x36);
LCD_WR_DATA(41);
LCD_WR_DATA(0x13);

LCD_WR_REG(0xF2);
LCD_WR_DATA(8);
LCD_WR_DATA(6);
LCD_WR_DATA(0x24);
LCD_WR_DATA(3);
LCD_WR_DATA(5);
LCD_WR_DATA(0x34);

LCD_WR_REG(0xF3);
LCD_WR_DATA(0x16);
LCD_WR_DATA(0xC);
LCD_WR_DATA(0x5A);
LCD_WR_DATA(4);
LCD_WR_DATA(3);
LCD_WR_DATA(0x69);

LCD_WR_REG(0xF4);
LCD_WR_DATA(0xD);
LCD_WR_DATA(0x18);
LCD_WR_DATA(0x15);
LCD_WR_DATA(5);
LCD_WR_DATA(5);
LCD_WR_DATA(0);

LCD_WR_REG(0xF5);
LCD_WR_DATA(0xD);
LCD_WR_DATA(0x18);
LCD_WR_DATA(0x17);
LCD_WR_DATA(0x35);
LCD_WR_DATA(0x39);
LCD_WR_DATA(0);
LCD_WR_REG(0x11);
	
	    Delay_ms(150);
LCD_WR_REG(0x29);
LCD_WR_REG(0x2C);
}

#endif

u16 LCD_ReadID(void)
{
  u16 id = 0;
  LCD_WR_REG(0XD3);
  id = LCD_RD_DATA();	//dummy read
  id = LCD_RD_DATA();
  id = LCD_RD_DATA();
  id <<= 8;
  id |= LCD_RD_DATA();
  return id;
}

void LCD_RefreshDirection(void)
{
  if(infoSettings.rotate_ui)
  {
    LCD_WR_REG(0X36);
    #ifdef RM68042
      LCD_WR_DATA(0X2B);
    #endif
    #ifdef ILI9488
      LCD_WR_DATA(0XE8);
    #endif
    #ifdef ILI9341
      #if defined (TFT28_V3_0)
      LCD_WR_DATA(0X60);//
      #else
      LCD_WR_DATA(0XA8);
      #endif
    #endif
    #ifdef HX8558
     LCD_WR_DATA(0XA4);
    #endif
  }
  else
  {
    LCD_WR_REG(0X36);
    #ifdef RM68042
      LCD_WR_DATA(0X28);
    #endif
    #ifdef ILI9488
      LCD_WR_DATA(0X28);
    #endif
    #ifdef ILI9341
      #if defined (TFT28_V3_0)
      LCD_WR_DATA(0XA0);//
      #else
       LCD_WR_DATA(0X68);
      #endif
    #endif
    #ifdef HX8558
     LCD_WR_DATA(0xA4);
    #endif
  }
}

void LCD_Init(void)
{
  LCD_HardwareConfig();
  LCD_init_RGB();
  GUI_Clear(BLACK);
  Delay_ms(20);

#ifdef LCD_LED_PIN
  LCD_LED_Init();
  LCD_LED_On();
  LCD_LED_PWM_Init();
#endif

#ifdef STM32_HAS_FSMC
  LCD_DMA_Config();  //spi flash to fsmc lcd DMA channel configuration
#endif
}
