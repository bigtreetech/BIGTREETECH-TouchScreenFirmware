/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V2.2.1
  * @date    17-March-2018
  * @brief   This file implements the board support package for the USB host library
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      <http://www.st.com/SLA0044>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include "usbh_usr.h"
#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "delay.h"
#include "variants.h"

#ifdef USB_FLASH_DRIVE_SUPPORT
/**
  * @brief  USB_OTG_BSP_Init
  *         Initializes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE * pdev)
{
#ifdef USE_USB_FS
  rcu_usbfs_trng_clock_config(RCU_CKUSB_CKPLL_DIV2_5);
  rcu_periph_clock_enable(RCU_USBFS);
#else
  #ifdef STM32F10X_CL
    RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLVCO_Div3);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE);
  #else
    #ifdef USE_USB_OTG_FS
      GPIO_InitTypeDef GPIO_InitStructure;

      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

      /* Configure DM DP Pins */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;

      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(GPIOA, &GPIO_InitStructure);

      GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG1_FS);
      GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG1_FS);

      RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
      RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);
    #endif  // USE_USB_OTG_HS
  #endif  // USB_OTG_HS
#endif  // USE_USB_FS

  Delay_init();
}

/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE * pdev)
{
#ifdef USE_USB_FS
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);
#else
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif  // USE_USB_FS
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE * pdev, uint8_t state)
{
  /*
   * On-chip 5 V VBUS generation is not supported. For this reason, a charge
   * pump or, if 5 V are available on the application board, a basic power
   * switch, must be added externally to drive the 5 V VBUS line. The external
   * charge pump can be driven by any GPIO output. When the application
   * decides to power on VBUS using the chosen GPIO, it must also set the port
   * power bit in the host port control and status register (PPWR bit in
   * OTG_FS_HPRT).
   *
   * Bit 12 PPWR: Port power The application uses this field to control power
   * to this port, and the core clears this bit on an overcurrent condition. */
}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE * pdev)
{
  //VBus(+5v) ctrl pins init
}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay(const uint32_t usec)
{
  Delay_us(usec);
}


/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay(const uint32_t msec)
{
  Delay_ms(msec);
}

#ifdef USE_USB_FS
void USBFS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
