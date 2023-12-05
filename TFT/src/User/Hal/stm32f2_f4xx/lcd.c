#include "lcd.h"
#include "GPIO_Init.h"
#include "delay.h"

#ifdef STM32_HAS_FSMC

uint16_t LCD_RD_DATA(void)
{
  volatile uint16_t ram;
  ram = LCD->LCD_RAM;
  return ram;
}

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  LCD->LCD_REG = LCD_Reg;
  LCD->LCD_RAM = LCD_RegValue;
}

uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  LCD_WR_REG(LCD_Reg);
  Delay_us(5);
  return LCD_RD_DATA();
}

void LCD_GPIO_Config(void)
{
  // fsmc 16bit data pins
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

  /*Configure the control line corresponding to FSMC
  * PD4-FSMC_NOE :LCD-RD
  * PD5-FSMC_NWE :LCD-WR
  * PD7-FSMC_NE1 :LCD-CS
  * PE2-FSMC_A23 :LCD-RS   LCD-RS data or cmd
  */
  GPIO_InitSet(PD4, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD5, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);

#ifdef MKS_TFT35_V1_0  // different pinout for MKS_TFT35_V1_0
  GPIO_InitSet(PD12, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
#else
  GPIO_InitSet(PE2, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
#endif
  GPIO_InitSet(PD7, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
}

void LCD_FSMC_Config(void)
{
  FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef readWriteTiming,writeTiming;

  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  readWriteTiming.FSMC_AddressSetupTime = 0x01;          // Address setup time (ADDSET) is 2 HCLK 1 / 36M = 27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;
  readWriteTiming.FSMC_DataSetupTime = 0x0f;
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;   // Mode A

  writeTiming.FSMC_AddressSetupTime = 0x00;              // Address setup time (ADDSET) is 1 HCLK
  writeTiming.FSMC_AddressHoldTime = 0x00;
  writeTiming.FSMC_DataSetupTime = TFTLCD_DRIVER_SPEED;  // Data save time
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;       // Mode A

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;                        // Select the address of the external storage area
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;      // Configure whether the data and address lines are multiplexed
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;                  // Configure the type of external storage
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;        // Set the data width of the FSMC interface

  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;    // Configure access mode
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;  // Configure the polarity of the wait signal
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                  // Configure whether to use non-alignment
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  // Configure when to wait for signals
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;                      // Configure whether to use wait signals
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;                      // Configure whether to allow burst writes

  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;       // Configuration write operation enabled
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;           // Configure whether to use extended mode

  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;          // Read timing
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                  // Write timing

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
  // Enable FSMC Bank1_SRAM Bank
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
