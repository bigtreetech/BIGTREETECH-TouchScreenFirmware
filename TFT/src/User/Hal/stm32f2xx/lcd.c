#include "includes.h"
#include "lcd.h"
#include "GPIO_Init.h"

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
  //fsmc 16bit data pins
  GPIO_InitSet(PD0, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD1, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD8, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD9, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD10, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD14, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD15, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE7, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE8, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE9, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE10, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE11, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE12, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE13, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE14, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE15, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);

  /* 配置FSMC相对应的控制线
  * PD4-FSMC_NOE :LCD-RD
  * PD5-FSMC_NWE :LCD-WR
  * PD7-FSMC_NE1 :LCD-CS
  * PE2-FSMC_A23 :LCD-RS   LCD-RS data or cmd
  */
  GPIO_InitSet(PD4, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD5, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PE2, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD7, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
}

void LCD_FSMC_Config(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming,writeTiming;

  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

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
  #error "don't support LCD-GPIO yet"
#endif
