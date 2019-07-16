#include "includes.h"
#include "lcd.h"

#ifdef LCD_LED_SUPPORT
void LCD_LED_On()
{
  GPIO_SetBits(LCD_LED_PORT, LCD_LED_PIN);
}
void LCD_LED_Off()
{
  GPIO_ResetBits(LCD_LED_PORT, LCD_LED_PIN);
}

void LCD_LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(LCD_LED_RCC, ENABLE);

  LCD_LED_Off();
  //LED-K  背光灯
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = LCD_LED_PIN;
  GPIO_Init(LCD_LED_PORT, &GPIO_InitStructure);
}
#endif

#ifdef STM32_HAS_FSMC
u16 LCD_RD_DATA(void)
{										    	   
  vu16 ram;			
  ram=LCD->LCD_RAM;	
  return ram;	 		 
}					   

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
  LCD->LCD_REG = LCD_Reg;
  LCD->LCD_RAM = LCD_RegValue;    		 
}

u16 LCD_ReadReg(u8 LCD_Reg)
{
  LCD_WR_REG(LCD_Reg);
  Delay_us(5);
  return LCD_RD_DATA();
}

void LCD_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
          
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD |RCC_APB2Periph_GPIOE ,ENABLE);
 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
  /* 配置FSMC相对应的数据线,FSMC-D0~D15: PD 14 15 0 1 8 9 10,PE 7 8 9 10 11 12 13 14 15*/	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0  | GPIO_Pin_1
                              | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10;
  GPIO_Init(GPIOD, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10
                              | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
                              | GPIO_Pin_15 | GPIO_Pin_2;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  /* 配置FSMC相对应的控制线
  * PD4-FSMC_NOE   :LCD-RD
  * PD5-FSMC_NWE   :LCD-WR
  * PD7-FSMC_NE1	 :LCD-CS
  * PD11-FSMC_A23  :LCD-RS   LCD-RS发送的数据代表是命令还是显示数据
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7  | GPIO_Pin_11;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void LCD_FSMC_Config(void)
{	
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming,writeTiming; 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;
  readWriteTiming.FSMC_DataSetupTime = 0x0f;
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 

  writeTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK  
  writeTiming.FSMC_AddressHoldTime = 0x00;
  writeTiming.FSMC_DataSetupTime = 0x03;		 ////数据保存时间为4个HCLK	
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;  //选择外接存储区域的地址
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;//配置数据线和地址线是否复用
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;//配置外接存储器的类型
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//设置FSMC接口的数据宽度

  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;//配置访问模式
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//配置等待信号的极性
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;//配置是否使用非对齐方式
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//配置等待信号什么时间产生
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;//配置是否使用等待信号
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;//配置是否允许突发写操作

  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;//配置写操作使能
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable ;//配置是否使用扩展模式

  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;//读时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    /* 使能 FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void LCD_HardwareConfig(void)
{
  LCD_GPIO_Config();
  LCD_FSMC_Config();
}

#else
//写寄存器函数
//data:寄存器值
void LCD_WR_REG(u16 data)
{ 
	LCD_RS_CLR;//写地址  
 	LCD_CS_CLR; 
	DATAOUT(data); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET;   
}

//写数据函数
//可以替代LCD_WR_DATAX宏,拿时间换空间.
//data:寄存器值
void LCD_WR_DATA(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}

//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{
  vu16 ram;
 	GPIOC->CRL = 0X88888888; //PB0-7  上拉输入
	GPIOC->CRH = 0X88888888; //PB8-15 上拉输入
	GPIOC->ODR = 0X0000;     //全部输出0

	LCD_RS_SET;
	LCD_CS_CLR;
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD_CLR;
  
	ram = DATAIN();  
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOC->CRL = 0X33333333; //PC0-7  上拉输出
	GPIOC->CRH = 0X33333333; //PC8-15 上拉输出
	GPIOC->ODR = 0XFFFF;    //全部输出高
	return ram;  
}

void LCD_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
          
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC, ENABLE);
 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  /*D0 - D15: PC0 - PC15 */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_All);
  
  /* 配置控制线
  * PB6   :LCD-RD
  * PB7   :LCD-WR
  * PB9   :LCD-CS
  * PB8   :LCD-RS   LCD-RS发送的数据代表是命令还是显示数据
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}

void LCD_HardwareConfig(void)
{
  LCD_GPIO_Config();
}
#endif

#if defined(TFT35_V1_2) || defined(TFT35_V3_0)
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
  LCD_WR_DATA(0X55);//16位模式 
  Delay_ms(120);
  LCD_WR_REG(0X29); 	 
}

#elif defined(TFT28)

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

void LCD_Init(void)
{
  LCD_HardwareConfig();
  LCD_init_RGB();
  GUI_Clear(BLACK);
  Delay_ms(15);
  
#ifdef LCD_LED_SUPPORT
  LCD_LED_Init();
  LCD_LED_On();
#endif
  
#ifdef STM32_HAS_FSMC
  LCD_DMA_Config();  //spi flash to fsmc lcd DMA channel configuration
#endif
}
