/**
  ******************************************************************************
  * @file    usbh_ioreq.h
  * @author  MCD Application Team
  * @version V2.2.1
  * @date    17-March-2018
  * @brief   Header file for usbh_ioreq.c
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

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_IOREQ_H
#define __USBH_IOREQ_H

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
#include "usbh_core.h"
#include "usbh_def.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/

/** @defgroup USBH_IOREQ
  * @brief This file is the header file for usbh_ioreq.c
  * @{
  */


/** @defgroup USBH_IOREQ_Exported_Defines
  * @{
  */
#define USBH_SETUP_PKT_SIZE   8
#define USBH_EP0_EP_NUM       0
#define USBH_MAX_PACKET_SIZE  0x40
/**
  * @}
  */


/** @defgroup USBH_IOREQ_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_IOREQ_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_IOREQ_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_IOREQ_Exported_FunctionsPrototype
  * @{
  */
USBH_Status USBH_CtlSendSetup ( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *buff,
                                uint8_t hc_num);

USBH_Status USBH_CtlSendData ( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t hc_num);

USBH_Status USBH_CtlReceiveData( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t hc_num);

USBH_Status USBH_BulkReceiveData( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t hc_num);

USBH_Status USBH_BulkSendData ( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t hc_num);

USBH_Status USBH_InterruptReceiveData( USB_OTG_CORE_HANDLE *pdev,
                                       uint8_t             *buff,
                                       uint8_t             length,
                                       uint8_t             hc_num);

USBH_Status USBH_InterruptSendData( USB_OTG_CORE_HANDLE *pdev,
                                    uint8_t *buff,
                                    uint8_t length,
                                    uint8_t hc_num);

USBH_Status USBH_CtlReq (USB_OTG_CORE_HANDLE *pdev,
                         USBH_HOST *phost,
                         uint8_t             *buff,
                         uint16_t            length);

USBH_Status USBH_IsocReceiveData( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *buff,
                                uint32_t length,
                                uint8_t hc_num);


USBH_Status USBH_IsocSendData( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *buff,
                                uint32_t length,
                                uint8_t hc_num);
/**
  * @}
  */

#endif /* __USBH_IOREQ_H */

/**
  * @}
  */

/**
  * @}
  */

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
