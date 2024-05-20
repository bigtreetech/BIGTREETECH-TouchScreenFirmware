#include "lcd.h"
#include "GPIO_Init.h"
#include "delay.h"

#ifdef STM32_HAS_FSMC

static void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  LCD->LCD_REG = LCD_Reg;
  LCD->LCD_RAM = LCD_RegValue;
}

static uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  LCD_WR_REG(LCD_Reg);
  Delay_us(5);

  return LCD_RD_DATA();
}

uint16_t LCD_RD_DATA(void)
{
  volatile uint16_t ram;

  ram = LCD->LCD_RAM;

  return ram;
}

static inline void LCD_GPIO_Config(void)
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

  /*
   * configure the control line corresponding to FSMC
   * PD4-FSMC_NOE :LCD-RD
   * PD5-FSMC_NWE :LCD-WR
   * PD7-FSMC_NE1 :LCD-CS
   * PE2-FSMC_A23 :LCD-RS LCD-RS data or cmd
   */
  GPIO_InitSet(PD4, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  GPIO_InitSet(PD5, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);

  #if defined(MKS_TFT35_V1_0)  // different pinout for MKS_TFT35_V1_0
    GPIO_InitSet(PD12, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  #else
    GPIO_InitSet(PE2, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
  #endif
  GPIO_InitSet(PD7, MGPIO_MODE_AF_PP, GPIO_AF_FSMC);
}

static inline void LCD_FSMC_Config(void)
{
  FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef readWriteTiming,writeTiming;

  // FSMC configuration
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  readWriteTiming.FSMC_AddressSetupTime = 0x01;          // address setup time (ADDSET) is 2 HCLK 1 / 36M = 27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;
  readWriteTiming.FSMC_DataSetupTime = 0x0f;
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;   // mode A

  writeTiming.FSMC_AddressSetupTime = 0x00;              // address setup time (ADDSET) is 1 HCLK
  writeTiming.FSMC_AddressHoldTime = 0x00;
  writeTiming.FSMC_DataSetupTime = TFTLCD_DRIVER_SPEED;  // data save time
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;       // mode A

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;                        // select the address of the external storage area
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;      // configure whether the data and address lines are multiplexed
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;                  // configure the type of external storage
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;        // set the data width of the FSMC interface

  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;    // configure access mode
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;  // configure the polarity of the wait signal
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                  // configure whether to use non-alignment
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  // configure when to wait for signals
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;                      // configure whether to use wait signals
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;                      // configure whether to allow burst writes

  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;       // configuration write operation enabled
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;           // configure whether to use extended mode

  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;          // read timing
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                  // write timing

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
  // enable FSMC Bank1_SRAM bank
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void LCD_HardwareConfig(void)
{
  LCD_GPIO_Config();
  LCD_FSMC_Config();
}

#else

#error "don't support LCD-GPIO yet"

#endif  // STM32_HAS_FSMC
