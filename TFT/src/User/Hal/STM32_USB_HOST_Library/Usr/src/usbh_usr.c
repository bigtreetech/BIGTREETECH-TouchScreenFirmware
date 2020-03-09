/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.2.1
  * @date    17-March-2018
  * @brief   This file includes the usb host library user callbacks
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
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "usb_log.h"

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;


#ifdef U_DISK_SUPPROT

uint8_t u_disk_inserted = 0;

/* Points to the DEVICE_PROP structure of current device */
/* The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb = {
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};

/** @defgroup USBH_USR_Private_Constants
* @{
*/
/*--------------- LCD Messages ---------------*/
uint8_t MSG_HOST_INIT[] = "> Host Library Initialized\n";
uint8_t MSG_DEV_ATTACHED[] = "> Device Attached \n";
uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
uint8_t MSG_DEV_ENUMERATED[] = "> Enumeration completed \n";
uint8_t MSG_DEV_HIGHSPEED[] = "> High speed device detected\n";
uint8_t MSG_DEV_FULLSPEED[] = "> Full speed device detected\n";
uint8_t MSG_DEV_LOWSPEED[] = "> Low speed device detected\n";
uint8_t MSG_DEV_ERROR[] = "> Device fault \n";

uint8_t MSG_MSC_CLASS[] = "> Mass storage device connected\n";
uint8_t MSG_HID_CLASS[] = "> HID device connected\n";
uint8_t MSG_DISK_SIZE[] = "> Size of the disk in MBytes: \n";
uint8_t MSG_LUN[] = "> LUN Available in the device:\n";
uint8_t MSG_ROOT_CONT[] = "> Exploring disk flash ...\n";
uint8_t MSG_WR_PROTECT[] = "> The disk is write protected\n";
uint8_t MSG_UNREC_ERROR[] = "> UNRECOVERED ERROR STATE\n";


/**
* @brief  USBH_USR_Init
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
  static uint8_t startup = 0;

  if (startup == 0)
  {
#ifdef USE_USB_OTG_HS
  #ifdef USE_EMBEDDED_PHY
  LOG_printf((uint8_t *) " USB OTG HS_IN_FS MSC Host");
  #else
  LOG_printf((uint8_t *) " USB OTG HS MSC Host");
  #endif
#else
    LOG_printf("USB OTG FS MSC Host");
#endif
    LOG_printf("> USB Host library started.\n");
    LOG_printf("  USB Host Library v2.2.1");
  }
}

/**
* @brief  USBH_USR_DeviceAttached
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
  u_disk_inserted = 1;
  LOG_printf((void *)MSG_DEV_ATTACHED);
}

/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError(void)
{
  /* Set default screen color */
  LOG_printf((void *)MSG_UNREC_ERROR);
}

/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Status
*/
void USBH_USR_DeviceDisconnected(void)
{
  u_disk_inserted = 0;
  /* Set default screen color */
  LOG_printf((void *)MSG_DEV_DISCONNECTED);
}

/**
* @brief  USBH_USR_ResetUSBDevice
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
}

/**
* @brief  USBH_USR_DeviceSpeedDetected
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if (DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    LOG_printf((void *)MSG_DEV_HIGHSPEED);
  }
  else if (DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    LOG_printf((void *)MSG_DEV_FULLSPEED);
  }
  else if (DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
    LOG_printf((void *)MSG_DEV_LOWSPEED);
  }
  else
  {
    LOG_printf((void *)MSG_DEV_ERROR);
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;
  hs = hs;    //just for ignore warning
  LOG_printf("VID : %04luh\n", (uint32_t) (*hs).idVendor);
  LOG_printf("PID : %04luh\n", (uint32_t) (*hs).idProduct);
}

/**
* @brief  USBH_USR_DeviceAddressAssigned
*         USB device is successfully assigned the Address
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{

}

/**
* @brief  USBH_USR_Conf_Desc
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef * itfDesc,
                                          USBH_EpDesc_TypeDef * epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;

  id = itfDesc;

  if ((*id).bInterfaceClass == 0x08)
  {
    LOG_printf((void *)MSG_MSC_CLASS);
  }
  else if ((*id).bInterfaceClass == 0x03)
  {
    LOG_printf((void *)MSG_HID_CLASS);
  }
}

/**
* @brief  USBH_USR_Manufacturer_String
*         Displays the message on LCD for Manufacturer String
* @param  Manufacturer String
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  LOG_printf("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
  LOG_printf("Product : %s\n", (char *)ProductString);
}

/**
* @brief  USBH_USR_SerialNum_String
*         Displays the message on LCD for SerialNum_String
* @param  SerialNum_String
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  LOG_printf("Serial Number : %s\n", (char *)SerialNumString);
}

/**
* @brief  EnumerationDone
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{

  /* Enumeration complete */
  LOG_printf((void *)MSG_DEV_ENUMERATED);
  LOG_printf("To see the root content of the disk : ");
  LOG_printf("Press Key...                       ");
}

/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
  LOG_printf("No registered class for this device. \n\r");
}

/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  return USBH_USR_RESP_OK;
}

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Status
*/
void USBH_USR_OverCurrentDetected(void)
{
  LOG_printf("Overcurrent detected.");
}

/**
* @brief  USBH_USR_MSC_Application
*         Demo application for mass storage
* @param  None
* @retval Status
*/
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
int USBH_USR_MSC_Application(void)
{
  switch (USBH_USR_ApplicationState)
  {
    case USH_USR_FS_INIT:
      USBH_USR_ApplicationState = USH_USR_FS_READLIST;
      break;

    case USH_USR_FS_READLIST:
      USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
      break;

    case USH_USR_FS_WRITEFILE:
      USBH_USR_ApplicationState = USH_USR_FS_DRAW;
      break;

    case USH_USR_FS_DRAW:
      break;

    default:
      break;
  }
  return (0);
}

/**
* @brief  USBH_USR_DeInit
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

uint8_t USBH_USR_Inserted(void)
{
  return u_disk_inserted;
}

uint8_t USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core);
}

uint8_t USBH_UDISK_Read(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  BYTE status = USBH_MSC_FAIL;

  if (HCD_IsDeviceConnected(&USB_OTG_Core))
  {
    do
    {
      status = USBH_MSC_Read10(&USB_OTG_Core, buf, sector,512 * cnt);
      USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);

      if (!HCD_IsDeviceConnected(&USB_OTG_Core))
      {
        return 1;
      }
    }while (status == USBH_MSC_BUSY );
  }

  if (status == USBH_MSC_OK)
    return 0;

  return 1;
}


uint8_t USBH_UDISK_Write(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  BYTE status = USBH_MSC_FAIL;

  if (HCD_IsDeviceConnected(&USB_OTG_Core))
  {
    do
    {
      status = USBH_MSC_Write10(&USB_OTG_Core, buf, sector, 512 * cnt);
      USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);

      if (!HCD_IsDeviceConnected(&USB_OTG_Core))
      {
        return 1;
      }
    }while (status == USBH_MSC_BUSY );
  }

  if (status == USBH_MSC_OK)
    return 0;
  return 1;
}
#else

uint32_t HCD_IsDeviceConnected(USB_OTG_CORE_HANDLE *pdev)
{
  return 0;  //disconnected
}

void USBH_Process(USB_OTG_CORE_HANDLE *pdev , USBH_HOST *phost)
{
}

uint8_t USBH_USR_Inserted(void)
{
  return 0;  //failed
}

uint8_t USBH_UDISK_Status(void)
{
	return 1;  //failed
}

uint8_t USBH_UDISK_Read(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  return 1;  //failed
}

uint8_t USBH_UDISK_Write(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  return 1;  //failed
}
#endif
