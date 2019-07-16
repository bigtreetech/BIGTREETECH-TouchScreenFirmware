#include "usart.h"

static USART_TypeDef *usart[_USART_CNT] = {USART1,  //TX--PA9  RX--PA10
                                           USART2,  //TX--PA2  RX--PA3
                                           USART3,  //TX--PB10 RX--PB11
                                           UART4,   //TX--PC10 RX--PC11
                                           UART5};  //TX--PC12 RX--PD2

void USART_GPIO_Init(uint8_t port, uint8_t deInit)
{
    uint32_t RCC_Define[_USART_CNT] = {RCC_APB2Periph_GPIOA, 
                                       RCC_APB2Periph_GPIOA, 
                                       RCC_APB2Periph_GPIOB, 
                                       RCC_APB2Periph_GPIOC, 
                                       RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD};
    GPIO_TypeDef *TX_GPIO_Port[_USART_CNT] = {GPIOA,       GPIOA,      GPIOB,       GPIOC,       GPIOC};         //TX
    uint16_t TX_GPIO_Pin[_USART_CNT]       = {GPIO_Pin_9,  GPIO_Pin_2, GPIO_Pin_10, GPIO_Pin_10, GPIO_Pin_12};
    GPIO_TypeDef *RX_GPIO_Port[_USART_CNT] = {GPIOA,       GPIOA,      GPIOB,       GPIOC,       GPIOD};         //RX
    uint16_t RX_GPIO_Pin[_USART_CNT]       = {GPIO_Pin_10, GPIO_Pin_3, GPIO_Pin_11, GPIO_Pin_11, GPIO_Pin_2};
        
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_Define[port],ENABLE);
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Mode = deInit ? GPIO_Mode_IN_FLOATING : GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin =  TX_GPIO_Pin[port];
    GPIO_Init(TX_GPIO_Port[port],&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = deInit ? GPIO_Mode_IN_FLOATING : GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin =  RX_GPIO_Pin[port];
    GPIO_Init(RX_GPIO_Port[port],&GPIO_InitStructure);
}

void USART_Protocol_Init(uint8_t port,uint32_t baud)
{
    USART_InitTypeDef USART_InitStructure;
    switch(port)
    {
        case _USART1: RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); break;
        case _USART2: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); break;
        case _USART3: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); break;
        case _UART4:  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);  break;
        case _UART5:  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);  break;    
    }
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(usart[port],&USART_InitStructure);
    
    USART_Cmd(usart[port],ENABLE);
}

void USART_IRQ_Init(uint8_t port, uint16_t usart_it)
{
    uint32_t IRQ_Channel[_USART_CNT] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn};
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel[port];
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(usart[port], usart_it, ENABLE);
    USART_ClearITPendingBit(usart[port], usart_it);
}

void USART_Config(uint8_t port, uint32_t baud, uint16_t usart_it)
{
    USART_Protocol_Init(port, baud);
    USART_IRQ_Init(port, usart_it);
    USART_GPIO_Init(port, 0);   //所有初始化完成后,再使能IO, 否则上电后会自动发送一个 0xFF
}

void USART_DeConfig(uint8_t port)
{
    USART_GPIO_Init(port, 1);
    switch(port)
    {
        case _USART1:
          RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
          RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);
          break;
        case _USART2:
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
          break;
        case _USART3:
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
          break;
        case _UART4:
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, DISABLE);
          break;
        case _UART5:
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, DISABLE);
          break;  
    }
}

void USART_Write(uint8_t port, uint8_t d)
{
    while((usart[port]->SR & USART_FLAG_TC) == (uint16_t)RESET);
    usart[port]->DR = ((u16)d & (uint16_t)0x01FF);
}
void USART_Puts(uint8_t port, uint8_t *str)
{
    while (*str)
    {
      while((usart[port]->SR & USART_FLAG_TC) == (uint16_t)RESET);
      usart[port]->DR = ((u16)*str++ & (uint16_t)0x01FF);
    }
}
