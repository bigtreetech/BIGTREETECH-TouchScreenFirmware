/**
  ******************************************************************************
  * @file    usb_conf.h
  * @author  MCD Application Team
  * @version V2.2.1
  * @date    17-March-2018
  * @brief   General low level driver configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF__H__
#define __USB_CONF__H__

#include "variants.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */

/** @defgroup USB_CONF
  * @brief USB low level driver configuration file
  * @{
  */

/** @defgroup USB_CONF_Exported_Defines
  * @{
  */

/* USB Core and PHY interface configuration.
   Tip: To avoid modifying these defines each time you need to change the USB
        configuration, you can declare the needed define in your toolchain
        compiler preprocessor.
   */
/****************** USB OTG FS PHY CONFIGURATION *******************************
*  The USB OTG FS Core supports one on-chip Full Speed PHY.
*
*  The USE_EMBEDDED_PHY symbol is defined in the project compiler preprocessor
*  when FS core is used.
*******************************************************************************/
#ifndef USE_USB_OTG_FS
 //#define USE_USB_OTG_FS
#endif /* USE_USB_OTG_FS */

#ifdef USE_USB_OTG_FS
 #define USB_OTG_FS_CORE
#endif

/****************** USB OTG HS PHY CONFIGURATION *******************************
*  The USB OTG HS Core supports two PHY interfaces:
*   (i)  An ULPI interface for the external High Speed PHY: the USB HS Core will
*        operate in High speed mode
*   (ii) An on-chip Full Speed PHY: the USB HS Core will operate in Full speed mode
*
*  You can select the PHY to be used using one of these two defines:
*   (i)  USE_ULPI_PHY: if the USB OTG HS Core is to be used in High speed mode
*   (ii) USE_EMBEDDED_PHY: if the USB OTG HS Core is to be used in Full speed mode
*
*  Notes:
*   - The USE_ULPI_PHY symbol is defined in the project compiler preprocessor as
*     default PHY when HS core is used.
*   - On STM322xG-EVAL and STM324xG-EVAL boards, only configuration(i) is available.
*     Configuration (ii) need a different hardware, for more details refer to your
*     STM32 device datasheet.
*******************************************************************************/
#ifndef USE_USB_OTG_HS
 //#define USE_USB_OTG_HS
#endif /* USE_USB_OTG_HS */

#ifndef USE_ULPI_PHY
 //#define USE_ULPI_PHY
#endif /* USE_ULPI_PHY */

#ifndef USE_EMBEDDED_PHY
 //#define USE_EMBEDDED_PHY
#endif /* USE_EMBEDDED_PHY */

#ifdef USE_USB_OTG_HS
 #define USB_OTG_HS_CORE
#endif

/*******************************************************************************
*                     FIFO Size Configuration in Host mode
*
*  (i) Receive data FIFO size = (Largest Packet Size / 4) + 1 or
*                             2x (Largest Packet Size / 4) + 1,  If a
*                             high-bandwidth channel or multiple isochronous
*                             channels are enabled
*
*  (ii) For the host nonperiodic Transmit FIFO is the largest maximum packet size
*      for all supported nonperiodic OUT channels. Typically, a space
*      corresponding to two Largest Packet Size is recommended.
*
*  (iii) The minimum amount of RAM required for Host periodic Transmit FIFO is
*        the largest maximum packet size for all supported periodic OUT channels.
*        If there is at least one High Bandwidth Isochronous OUT endpoint,
*        then the space must be at least two times the maximum packet size for
*        that channel.
*******************************************************************************/

/****************** USB OTG HS CONFIGURATION **********************************/
#ifdef USB_OTG_HS_CORE
 #define RX_FIFO_HS_SIZE                          512
 #define TXH_NP_HS_FIFOSIZ                        256
 #define TXH_P_HS_FIFOSIZ                         256

// #define USB_OTG_HS_LOW_PWR_MGMT_SUPPORT
// #define USB_OTG_HS_SOF_OUTPUT_ENABLED

 #ifdef USE_ULPI_PHY
  #define USB_OTG_ULPI_PHY_ENABLED
 #endif
 #ifdef USE_EMBEDDED_PHY
   #define USB_OTG_EMBEDDED_PHY_ENABLED
 #endif
 #define USB_OTG_HS_INTERNAL_DMA_ENABLED
 #define USB_OTG_EXTERNAL_VBUS_ENABLED
// #define USB_OTG_INTERNAL_VBUS_ENABLED
#endif

/****************** USB OTG FS CONFIGURATION **********************************/
#ifdef USB_OTG_FS_CORE
 #define RX_FIFO_FS_SIZE                          128
 #define TXH_NP_FS_FIFOSIZ                         96
 #define TXH_P_FS_FIFOSIZ                          96

// #define USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
// #define USB_OTG_FS_SOF_OUTPUT_ENABLED
#endif

/****************** USB OTG MODE CONFIGURATION ********************************/
#define USE_HOST_MODE
//#define USE_DEVICE_MODE
//#define USE_OTG_MODE

#ifdef U_DISK_SUPPORT
  #ifndef USB_OTG_FS_CORE
   #ifndef USB_OTG_HS_CORE
      #error  "USB_OTG_HS_CORE or USB_OTG_FS_CORE should be defined"
   #endif
  #endif

  #ifndef USE_DEVICE_MODE
   #ifndef USE_HOST_MODE
      #error  "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
   #endif
  #endif

  #ifndef USE_USB_OTG_HS
   #ifndef USE_USB_OTG_FS
      #error  "USE_USB_OTG_HS or USE_USB_OTG_FS should be defined"
   #endif
  #else //USE_USB_OTG_HS
   #ifndef USE_ULPI_PHY
    #ifndef USE_EMBEDDED_PHY
       #error  "USE_ULPI_PHY or USE_EMBEDDED_PHY should be defined"
    #endif
   #endif
  #endif
#endif
/****************** C Compilers dependant keywords ****************************/
/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined   (__GNUC__)        /* GNU Compiler */
    #define __ALIGN_END    __attribute__ ((aligned (4)))
    #define __ALIGN_BEGIN
  #else
    #define __ALIGN_END
    #if defined   (__CC_ARM)      /* ARM Compiler */
      #define __ALIGN_BEGIN    __align(4)
    #elif defined (__ICCARM__)    /* IAR Compiler */
      #define __ALIGN_BEGIN
    #endif /* __CC_ARM */
  #endif /* __GNUC__ */
#else
  #define __ALIGN_BEGIN
  #define __ALIGN_END
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
  #define __packed    __packed
#elif defined (__ICCARM__)     /* IAR Compiler */
  #define __packed    __packed
#elif defined   ( __GNUC__ )   /* GNU Compiler */
  #define __packed    __attribute__((__packed__))
#endif /* __CC_ARM */

/**
  * @}
  */


/** @defgroup USB_CONF_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup USB_CONF_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USB_CONF_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USB_CONF_Exported_FunctionsPrototype
  * @{
  */
/**
  * @}
  */


#endif //__USB_CONF__H__


/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
