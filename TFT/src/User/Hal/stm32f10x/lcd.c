#include "includes.h"
#include "lcd.h"

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
  /* ����FSMC���Ӧ��������,FSMC-D0~D15: PD 14 15 0 1 8 9 10,PE 7 8 9 10 11 12 13 14 15*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0  | GPIO_Pin_1
                              | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10
                              | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
                              | GPIO_Pin_15 | GPIO_Pin_2;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  /* ����FSMC���Ӧ�Ŀ�����
  * PD4-FSMC_NOE   :LCD-RD
  * PD5-FSMC_NWE   :LCD-WR
  * PD7-FSMC_NE1	 :LCD-CS
  * PE2-FSMC_A23  :LCD-RS   LCD-RS���͵����ݴ������������ʾ����
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

  readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //??????????ADDSET???2??HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;
  readWriteTiming.FSMC_DataSetupTime = 0x0f;
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA

  writeTiming.FSMC_AddressSetupTime = 0x00;	 //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK
  writeTiming.FSMC_AddressHoldTime = 0x00;
  writeTiming.FSMC_DataSetupTime = 0x03;		 ////���ݱ���ʱ��Ϊ4��HCLK
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;  //ѡ����Ӵ洢����ĵ�ַ
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;//���������ߺ͵�ַ���Ƿ���
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;//������Ӵ洢��������
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//����FSMC�ӿڵ����ݿ��

  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;//���÷���ģʽ
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//���õȴ��źŵļ���
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;//�����Ƿ�ʹ�÷Ƕ��뷽ʽ
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//���õȴ��ź�ʲôʱ�����
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;//�����Ƿ�ʹ�õȴ��ź�
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;//�����Ƿ�����ͻ��д����

  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;//����д����ʹ��
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable ;//�����Ƿ�ʹ����չģʽ

  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;//��ʱ��
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //дʱ��

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    /* ʹ�� FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void LCD_HardwareConfig(void)
{
  LCD_GPIO_Config();
  LCD_FSMC_Config();
}

#else
//д�Ĵ�������
//data:�Ĵ���ֵ
void LCD_WR_REG(u16 data)
{
	LCD_RS_CLR;//д��ַ
 	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
 	LCD_CS_SET;
}

//д���ݺ���
//�������LCD_WR_DATAX��,��ʱ�任�ռ�.
//data:�Ĵ���ֵ
void LCD_WR_DATA(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}

//��LCD����
//����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{
  vu16 ram;
 	GPIOC->CRL = 0X88888888; //PB0-7  ��������
	GPIOC->CRH = 0X88888888; //PB8-15 ��������
	GPIOC->ODR = 0X0000;     //ȫ�����0

	LCD_RS_SET;
	LCD_CS_CLR;
	//��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;

	ram = DATAIN();
	LCD_RD_SET;
	LCD_CS_SET;

	GPIOC->CRL = 0X33333333; //PC0-7  �������
	GPIOC->CRH = 0X33333333; //PC8-15 �������
	GPIOC->ODR = 0XFFFF;    //ȫ�������
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

  /* ���ÿ�����
  * PB6   :LCD-RD
  * PB7   :LCD-WR
  * PB9   :LCD-CS
  * PB8   :LCD-RS   LCD-RS���͵����ݴ������������ʾ����
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
