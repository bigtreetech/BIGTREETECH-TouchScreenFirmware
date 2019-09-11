#include "Serial.h"
#include "includes.h"

//dma rx buffer
char dma_mem_buf[_USART_CNT][DMA_TRANS_LEN];

//Config for USART Channel
typedef struct
{
  USART_TypeDef *uart;
  uint32_t dma_rcc;
  DMA_Channel_TypeDef *dma_chanel;
}SERIAL_CFG;

static const SERIAL_CFG Serial[_USART_CNT] = {
  {USART1, RCC_AHBPeriph_DMA1, DMA1_Channel5},
  {USART2, RCC_AHBPeriph_DMA1, DMA1_Channel6},
  {USART3, RCC_AHBPeriph_DMA1, DMA1_Channel3},
  {UART4,  RCC_AHBPeriph_DMA2, DMA2_Channel3},
  //{UART5,  -1, -1}, //UART5 don't support DMA
};

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];
  
  RCC_AHBPeriphClockCmd(cfg->dma_rcc, ENABLE);  //DMA RCC EN

  cfg->uart->CR3 |= 1<<6;  //DMA enable receiver
  
  cfg->dma_chanel->CPAR = (u32)(&cfg->uart->DR);
  cfg->dma_chanel->CMAR = (u32)(&dma_mem_buf[port]);
  cfg->dma_chanel->CNDTR = DMA_TRANS_LEN;
  cfg->dma_chanel->CCR = 0X00000000;
  cfg->dma_chanel->CCR |= 3<<12;   //Channel priority level
  cfg->dma_chanel->CCR |= 1<<7;    //Memory increment mode
  cfg->dma_chanel->CCR |= 1<<0;    //DMA EN
}

void Serial_Config(u32 baud)
{
  USART_Config(SERIAL_PORT, baud, USART_IT_IDLE);  //IDLE interrupt
  Serial_DMA_Config(SERIAL_PORT);
  
  #ifdef SERIAL_PORT_2
    USART_Config(SERIAL_PORT_2, baud, USART_IT_IDLE);  //IDLE interrupt
    Serial_DMA_Config(SERIAL_PORT_2);
  #endif
  
  #ifdef SERIAL_PORT_3
    USART_Config(SERIAL_PORT_3, baud, USART_IT_IDLE);  //IDLE interrupt
    Serial_DMA_Config(SERIAL_PORT_3);
  #endif
  
  #ifdef SERIAL_PORT_4
    USART_Config(SERIAL_PORT_4, baud, USART_IT_IDLE);  //IDLE interrupt
    Serial_DMA_Config(SERIAL_PORT_4);
  #endif
}

void Serial_DeConfig(void)
{
  USART_DeConfig(SERIAL_PORT);
  
  #ifdef SERIAL_PORT_2
    USART_DeConfig(SERIAL_PORT_2);
  #endif
  
  #ifdef SERIAL_PORT_3
    USART_DeConfig(SERIAL_PORT_3);
  #endif
  
  #ifdef SERIAL_PORT_4
    USART_DeConfig(SERIAL_PORT_4);
  #endif
}

/*

*/
void Serial_DMAReEnable(uint8_t port)
{
  memset(dma_mem_buf[port], 0, DMA_TRANS_LEN);
  Serial[port].dma_chanel->CCR &= ~(1<<0);
  Serial[port].dma_chanel->CNDTR = DMA_TRANS_LEN;  
  Serial[port].dma_chanel->CCR |= 1<<0;    //DMA			
}

void USART_IRQHandler(uint8_t port)
{
  u16 rx_len=0;

  if((Serial[port].uart->SR & (1<<4))!=0)
  {
    Serial[port].uart->SR = ~(1<<4);
    Serial[port].uart->DR;

    rx_len = DMA_TRANS_LEN - Serial[port].dma_chanel->CNDTR;

    if(dma_mem_buf[port][rx_len-1]=='\n' || (rx_len > DMA_TRANS_LEN - 5))  //receive completed or overflow buffer
    {
      dma_mem_buf[port][rx_len] = 0; //end character
      infoHost.rx_ok[port]=true;
    }    
  }
}

void USART1_IRQHandler(void)
{
    USART_IRQHandler(_USART1);
}

void USART2_IRQHandler(void)
{
    USART_IRQHandler(_USART2);
}

void USART3_IRQHandler(void)
{
    USART_IRQHandler(_USART3);
}

void UART4_IRQHandler(void)
{
    USART_IRQHandler(_UART4);
}

void UART5_IRQHandler(void)
{
    USART_IRQHandler(_UART5);
}

void Serial_Puts(uint8_t port, char *s)
{
  while (*s)
  {
    while((Serial[port].uart->SR & USART_FLAG_TC) == (uint16_t)RESET);
    Serial[port].uart->DR = ((u16)*s++ & (uint16_t)0x01FF);
  }
}

#include "stdio.h"
int fputc(int ch, FILE *f)
{      
	while((Serial[SERIAL_PORT].uart->SR&0X40)==0);
    Serial[SERIAL_PORT].uart->DR = (u8) ch;
	return ch;
}
