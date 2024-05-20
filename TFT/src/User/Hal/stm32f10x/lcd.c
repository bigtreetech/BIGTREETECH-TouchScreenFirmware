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
  GPIO_InitTypeDef GPIO_InitStructure;

  // fsmc 16bit data pins
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

  // FSMC-D0~D15: PD 14 15 0 1 8 9 10,PE 7 8 9 10 11 12 13 14 15
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0  | GPIO_Pin_1
                              | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10
                              | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
                              | GPIO_Pin_15 | GPIO_Pin_2;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /*
   * configure the control line corresponding to FSMC
   * PD4-FSMC_NOE :LCD-RD
   * PD5-FSMC_NWE :LCD-WR
   * PD7-FSMC_NE1 :LCD-CS
   * PE2-FSMC_A23 :LCD-RS
   */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_11;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static inline void LCD_FSMC_Config(void)
{
  FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef readWriteTiming,writeTiming;

  // FSMC configuration
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  readWriteTiming.FSMC_AddressSetupTime = 0x01;  // HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;
  readWriteTiming.FSMC_DataSetupTime = 0x0f;
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

  writeTiming.FSMC_AddressSetupTime = 0x00;
  writeTiming.FSMC_AddressHoldTime = 0x00;
  writeTiming.FSMC_DataSetupTime = TFTLCD_DRIVER_SPEED;
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;

  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;

  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;

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

#if defined(MKS_TFT)
  /*
  #define LCD_WR PB14
  #define LCD_RS PD13
  #define LCD_CS PC8
  #define LCD_RD PD15
  */
  #define LCD_CS_SET GPIOC->BSRR=1<<8               // Chip Select Port  PC8   //片选端口
  #define LCD_RS_SET GPIOD->BSRR=1<<13              // Data / command    PD13  //数据/命令
  #define LCD_WR_SET GPIOB->BSRR=1<<14              // Write data        PB14  //写数据
  #define LCD_RD_SET GPIOD->BSRR=1<<15              // Read data         PD15  //读数据

  #define LCD_CS_CLR GPIOC->BRR=1<<8                // Chip select port  PC8   //片选端口
  #define LCD_RS_CLR GPIOD->BRR=1<<13               // Data / command    PD13  //数据/命令
  #define LCD_WR_CLR GPIOB->BRR=1<<14               // Write data        PB14  //写数据
  #define LCD_RD_CLR GPIOD->BRR=1<<15               // Read data         PD15  //读数据

  #define DATAOUT(x) do{ GPIOE->ODR = x; }while(0)  // Data output  //数据输出
  #define DATAIN()   GPIOE->IDR                     // Data input   //数据输入
#else
  /*
   * TFT24-V1.1
   * PB6 :LCD-RD
   * PB7 :LCD-WR
   * PB9 :LCD-CS
   * PB8 :LCD-RS
   */
  #define LCD_CS_SET GPIOB->BSRR=1<<9               // Chip Select Port  PB9  //片选端口
  #define LCD_RS_SET GPIOB->BSRR=1<<8               // Data / command    PB8  //数据/命令
  #define LCD_WR_SET GPIOB->BSRR=1<<7               // Write data        PB7  //写数据
  #define LCD_RD_SET GPIOB->BSRR=1<<6               // Read data         PB6  //读数据

  #define LCD_CS_CLR GPIOB->BRR=1<<9                // Chip Select Port  PB9  //片选端口
  #define LCD_RS_CLR GPIOB->BRR=1<<8                // Data / command    PB8  //数据/命令
  #define LCD_WR_CLR GPIOB->BRR=1<<7                // Write data        PB7  //写数据
  #define LCD_RD_CLR GPIOB->BRR=1<<6                // Read data         PB6  //读数据

  // PB0~15, as a data line  // PB0~15, 作为数据线
  #define DATAOUT(x) do{ GPIOC->ODR = x; }while(0)  // Data output  //数据输出
  #define DATAIN()   GPIOC->IDR                     // Data input   //数据输入
#endif

void LCD_WR_REG(uint16_t data)
{
  LCD_RS_CLR;
  LCD_CS_CLR;
  DATAOUT(data);
  LCD_WR_CLR;
  LCD_WR_SET;
  LCD_CS_SET;
}

void LCD_WR_DATA(uint16_t data)
{
  LCD_RS_SET;
  LCD_CS_CLR;
  DATAOUT(data);
  LCD_WR_CLR;
  LCD_WR_SET;
  LCD_CS_SET;
}

uint16_t LCD_RD_DATA(void)
{
  #if defined(MKS_TFT)
    #define LCD_DATA_PORT GPIOE
  #else
    #define LCD_DATA_PORT GPIOC
  #endif

  volatile uint16_t ram;

  LCD_DATA_PORT->CRL = 0X88888888;
  LCD_DATA_PORT->CRH = 0X88888888;
  LCD_DATA_PORT->ODR = 0X0000;

  LCD_RS_SET;
  LCD_CS_CLR;
  LCD_RD_CLR;  // double for delay
  LCD_RD_CLR;
  ram = DATAIN();
  LCD_RD_SET;
  LCD_CS_SET;

  LCD_DATA_PORT->CRL = 0X33333333;
  LCD_DATA_PORT->CRH = 0X33333333;
  LCD_DATA_PORT->ODR = 0XFFFF;

  return ram;
}

static inline void LCD_GPIO_Config(void)
{
  #if defined(MKS_TFT)
    GPIO_InitTypeDef GPIO_InitStructure;
    // GPIO Ports Clock Enable

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOB, ENABLE);
    // configure GPIO pin Output Level

    // configure GPIO pins : Pin1_Pin Pin2_Pin

    // GPIO_InitStructure.GPIO_Pin = LCD_nWR_Pin | FLASH_nCS_Pin | FILAMENT_DI_Pin | POWER_DI_Pin;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = SDCARD_nCS_Pin | LCD_RS_Pin | LCD_BACKLIGHT_Pin | LCD_nRD_Pin;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    LCD_RD_SET;  // set this as we only change it when reading
  #else
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    // D0 - D15: PC0 - PC15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_All);

    /*
     * PB6 :LCD-RD
     * PB7 :LCD-WR
     * PB9 :LCD-CS
     * PB8 :LCD-RS
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
  #endif
}

void LCD_HardwareConfig(void)
{
  LCD_GPIO_Config();
}

#endif  // STM32_HAS_FSMC
