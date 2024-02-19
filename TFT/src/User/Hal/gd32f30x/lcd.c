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
  rcu_periph_clock_enable(RCU_EXMC);
  // fsmc 16bit data pins
  GPIO_InitSet(PD0, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD1, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD8, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD9, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD10, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD14, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD15, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE7, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE8, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE9, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE10, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE11, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE12, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE13, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE14, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE15, MGPIO_MODE_AF_PP, 0);

  /*Configure the control line corresponding to FSMC
  * PD4-FSMC_NOE :LCD-RD
  * PD5-FSMC_NWE :LCD-WR
  * PD7-FSMC_NE1 :LCD-CS
  * PE2-FSMC_A23 :LCD-RS   LCD-RS data or cmd
  */
  GPIO_InitSet(PD4, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD5, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PE2, MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(PD7, MGPIO_MODE_AF_PP, 0);
}

void LCD_EXMC_Config(void)
{
  exmc_norsram_parameter_struct EXMC_NORSRAMInitStructure;
  exmc_norsram_timing_parameter_struct readWriteTiming,writeTiming;

  /* EXMC configuration */
  readWriteTiming.asyn_address_setuptime = 1U;
  readWriteTiming.asyn_address_holdtime = 0U;
  readWriteTiming.asyn_data_setuptime = 15U;
  readWriteTiming.bus_latency = 0U;
  readWriteTiming.syn_clk_division = 0U;
  readWriteTiming.syn_data_latency = 0U;
  readWriteTiming.asyn_access_mode = EXMC_ACCESS_MODE_A;

  writeTiming.asyn_address_setuptime = 1U;
  writeTiming.asyn_address_holdtime = 0U;
  writeTiming.asyn_data_setuptime = TFTLCD_DRIVER_SPEED;
  writeTiming.bus_latency = 0U;
  writeTiming.syn_clk_division = 0U;
  writeTiming.syn_data_latency = 0U;
  writeTiming.asyn_access_mode = EXMC_ACCESS_MODE_A;

  EXMC_NORSRAMInitStructure.norsram_region = EXMC_BANK0_NORSRAM_REGION0;
  EXMC_NORSRAMInitStructure.write_mode = EXMC_ASYN_WRITE;
  EXMC_NORSRAMInitStructure.extended_mode = DISABLE;
  EXMC_NORSRAMInitStructure.asyn_wait = DISABLE;
  EXMC_NORSRAMInitStructure.nwait_signal = DISABLE;
  EXMC_NORSRAMInitStructure.memory_write = ENABLE;
  EXMC_NORSRAMInitStructure.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
  EXMC_NORSRAMInitStructure.wrap_burst_mode = DISABLE;
  EXMC_NORSRAMInitStructure.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
  EXMC_NORSRAMInitStructure.burst_mode = DISABLE;
  EXMC_NORSRAMInitStructure.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
  EXMC_NORSRAMInitStructure.memory_type = EXMC_MEMORY_TYPE_NOR;
  EXMC_NORSRAMInitStructure.address_data_mux = DISABLE;
  EXMC_NORSRAMInitStructure.read_write_timing = &readWriteTiming;
  EXMC_NORSRAMInitStructure.write_timing = &writeTiming;

  exmc_norsram_init(&EXMC_NORSRAMInitStructure);
  /* enable EXMC SARM bank0 */
  exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION0);
}

void LCD_HardwareConfig(void)
{
  LCD_GPIO_Config();
  LCD_EXMC_Config();
}

#else

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
  // LCD_DATA_PORT->CRL = 0X88888888;  1000 1000 1000 1000 1000 1000 1000 1000
  // LCD_DATA_PORT->CRH = 0X88888888;  1000 1000 1000 1000 1000 1000 1000 1000
  gpio_init(LCD_DATA_PORT, GPIO_MODE_IPU, 0, GPIO_PIN_ALL);
  // LCD_DATA_PORT->ODR = 0X0000;
  gpio_port_write(LCD_DATA_PORT, 0X0000);

  LCD_RS_SET;
  LCD_CS_CLR;
  LCD_RD_CLR;  // double for delay.
  LCD_RD_CLR;
  ram = DATAIN();
  LCD_RD_SET;
  LCD_CS_SET;

  // LCD_DATA_PORT->CRL = 0X33333333;  0011 0011 0011 0011 0011 0011 0011 0011
  // LCD_DATA_PORT->CRH = 0X33333333;  0011 0011 0011 0011 0011 0011 0011 0011
  gpio_init(LCD_DATA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);
  // LCD_DATA_PORT->ODR = 0XFFFF;
  gpio_port_write(LCD_DATA_PORT, 0XFFFF);
  return ram;
}

void LCD_GPIO_Config(void)
{
#if defined(MKS_TFT)
  // GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIO Ports Clock Enable */

  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOB, ENABLE);
  /* Configure GPIO pin Output Level */

  /* Configure GPIO pins : Pin1_Pin Pin2_Pin */

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_9;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_Init(GPIOB, &GPIO_InitStructure);
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14 | GPIO_PIN_9);

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_Init(GPIOD, &GPIO_InitStructure);
  gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_Init(GPIOC, &GPIO_InitStructure);
  gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9);

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_Init(GPIOC, &GPIO_InitStructure);
  gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  // GPIO_Init(GPIOE, &GPIO_InitStructure);
  gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);

  LCD_RD_SET;  // set this as we only change it when reading
#else
  #error "don't support LCD-GPIO yet"
#endif
}

void LCD_HardwareConfig(void)
{
  LCD_GPIO_Config();
}

#endif
