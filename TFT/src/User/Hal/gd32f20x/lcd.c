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
  #error "don't support LCD-GPIO yet"
#endif
