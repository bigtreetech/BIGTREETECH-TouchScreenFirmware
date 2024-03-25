#include "uart.h"
#include "variants.h"  // for USART1_TX_PIN etc.
#include "GPIO_Init.h"

// USART1 (GD USART0) default pins config
#ifndef USART1_TX_PIN
  #define USART1_TX_PIN PA9
#endif
#ifndef USART1_RX_PIN
  #define USART1_RX_PIN PA10
#endif
// USART2 (GD USART1) default pins config
#ifndef USART2_TX_PIN
  #define USART2_TX_PIN PA2
#endif
#ifndef USART2_RX_PIN
  #define USART2_RX_PIN PA3
#endif
// USART3 (GD USART2) default pins config
#ifndef USART3_TX_PIN
  #define USART3_TX_PIN PB10
#endif
#ifndef USART3_RX_PIN
  #define USART3_RX_PIN PB11
#endif
// UART4 (GD UART3) default pins config
#ifndef UART4_TX_PIN
  #define UART4_TX_PIN  PC10
#endif
#ifndef UART4_RX_PIN
  #define UART4_RX_PIN  PC11
#endif
// UART5 (GD UART4) default pins config
#ifndef UART5_TX_PIN
  #define UART5_TX_PIN  PC12
#endif
#ifndef UART5_RX_PIN
  #define UART5_RX_PIN  PD2
#endif

static rcu_periph_reset_enum rcu_uart_rst[_UART_CNT] = {
  RCU_USART0RST,
  RCU_USART1RST,
  RCU_USART2RST,
  RCU_UART3RST,
  RCU_UART4RST,
};

static rcu_periph_enum rcu_uart_en[_UART_CNT] = {
  RCU_USART0,
  RCU_USART1,
  RCU_USART2,
  RCU_UART3,
  RCU_UART4,
};

static uint32_t const uart[_UART_CNT] = {
  USART0,  // TX--PA9  RX--PA10
  USART1,  // TX--PA2  RX--PA3
  USART2,  // TX--PB10 RX--PB11
  UART3,   // TX--PC10 RX--PC11
  UART4,   // TX--PC12 RX--PD2
};

static const uint16_t uart_tx[_UART_CNT] = {USART1_TX_PIN, USART2_TX_PIN, USART3_TX_PIN, UART4_TX_PIN, UART5_TX_PIN};  // TX
static const uint16_t uart_rx[_UART_CNT] = {USART1_RX_PIN, USART2_RX_PIN, USART3_RX_PIN, UART4_RX_PIN, UART5_RX_PIN};  // RX

static inline void UART_GPIO_Init(uint8_t port)
{
  GPIO_InitSet(uart_tx[port], MGPIO_MODE_AF_PP, 0);
  GPIO_InitSet(uart_rx[port], MGPIO_MODE_IPU, 0);
}

static inline void UART_GPIO_DeInit(uint8_t port)
{
  // set tx/rx to input
  GPIO_InitSet(uart_tx[port], MGPIO_MODE_IPN, 0);
  GPIO_InitSet(uart_rx[port], MGPIO_MODE_IPN, 0);
}

static inline void UART_Protocol_Init(uint8_t port, uint32_t baud)
{
  rcu_periph_clock_enable(rcu_uart_en[port]);  // enable clock

  usart_deinit(uart[port]);
  usart_baudrate_set(uart[port], baud);
  usart_word_length_set(uart[port], USART_WL_8BIT);
  usart_stop_bit_set(uart[port], USART_STB_1BIT);
  usart_parity_config(uart[port], USART_PM_NONE);
  usart_hardware_flow_rts_config(uart[port], USART_RTS_DISABLE);
  usart_hardware_flow_cts_config(uart[port], USART_CTS_DISABLE);
  usart_receive_config(uart[port], USART_RECEIVE_ENABLE);
  usart_transmit_config(uart[port], USART_TRANSMIT_ENABLE);
  usart_enable(uart[port]);
}

static inline void UART_IRQ_Init(uint8_t port, uint16_t usart_it, bool idle_interrupt)
{
  uint32_t IRQ_Channel[_UART_CNT] = {USART0_IRQn, USART1_IRQn, USART2_IRQn, UART3_IRQn, UART4_IRQn};
  nvic_irq_enable(IRQ_Channel[port], 0U, 0U);

  if (idle_interrupt)  // enable or disable serial line IDLE interrupt
  {
    usart_interrupt_enable(uart[port], usart_it);
    usart_interrupt_flag_clear(uart[port], usart_it);
  }
}

void UART_Config(uint8_t port, uint32_t baud, uint16_t usart_it, bool idle_interrupt)
{
  UART_Protocol_Init(port, baud);
  UART_IRQ_Init(port, usart_it, idle_interrupt);
  UART_GPIO_Init(port);  // after all initialization is completed, enable IO, otherwise a 0xFF will be sent automatically after power-on
}

void UART_DeConfig(uint8_t port)
{
  UART_GPIO_DeInit(port);

  rcu_periph_reset_enable(rcu_uart_rst[port]);
  rcu_periph_reset_disable(rcu_uart_rst[port]);  // reset clock
}

void UART_Write(uint8_t port, uint8_t d)
{
  while ((USART_STAT0(uart[port]) & (1 << USART_FLAG_TC)) == (uint16_t)RESET);  // wait
  USART_DATA(uart[port]) = ((uint16_t)d & (uint16_t)0x01FF);
}

void UART_Puts(uint8_t port, uint8_t * str)
{
  while (*str)
  {
    while ((USART_STAT0(uart[port]) & (1 << USART_FLAG_TC)) == (uint16_t)RESET);  // wait
    USART_DATA(uart[port]) = ((uint16_t)*str++ & (uint16_t)0x01FF);
  }
}
