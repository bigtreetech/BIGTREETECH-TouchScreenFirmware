/*!
    \file    usbh_usr.c
    \brief   user application layer for USBFS host-mode MSC class operation

    \version 2020-07-28, V3.0.0, firmware for GD32F20x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include <string.h>
#include "usbh_usr.h"
#include "drv_usb_hw.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bbb.h"

#ifdef USB_FLASH_DRIVE_SUPPORT

extern usb_core_driver usbh_core;
usbh_host usb_host;

uint8_t u_disk_inserted = 0;

/*  points to the DEVICE_PROP structure of current device */
usbh_user_cb usr_cb =
{
  usbh_user_init,
  usbh_user_deinit,
  usbh_user_device_connected,
  usbh_user_device_reset,
  usbh_user_device_disconnected,
  usbh_user_over_current_detected,
  usbh_user_device_speed_detected,
  usbh_user_device_desc_available,
  usbh_user_device_address_assigned,
  usbh_user_configuration_descavailable,
  usbh_user_manufacturer_string,
  usbh_user_product_string,
  usbh_user_serialnum_string,
  usbh_user_enumeration_finish,
  usbh_user_userinput,
  usbh_usr_msc_application,
  usbh_user_device_not_supported,
  usbh_user_unrecovered_error
};

const uint8_t MSG_HOST_HEADER[] = "USBFS MSC Host";
const uint8_t MSG_HOST_FOOTER[] = "USB Host Library v2.0.0";

void usbh_user_init(void)
{
}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
  u_disk_inserted = 1;
  LOG_printf((void *)"> Device Attached.\r\n");
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
  LOG_printf((void *)"> Unrecovered Error State .\r\n");
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
  u_disk_inserted = 0;
  LOG_printf((void *)"> Device Disconnected.\r\n");
}

/*!
    \brief      user operation for reset USB Device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
  LOG_printf((void *)"> Reset the USB device.\r\n");
}

/*!
    \brief      user operation for detecting device speed
    \param[in]  device_speed: device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint32_t device_speed)
{
  if (PORT_SPEED_HIGH == device_speed) {
    LOG_printf((void *)"> High speed device detected.\r\n");
  } else if(PORT_SPEED_FULL == device_speed) {
    LOG_printf((void *)"> Full speed device detected.\r\n");
  } else if(PORT_SPEED_LOW == device_speed) {
    LOG_printf((void *)"> Low speed device detected.\r\n");
  } else {
    LOG_printf((void *)"> Device Fault.\r\n");
  }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  device_desc: device descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_device_desc_available(void *device_desc)
{
  usb_desc_dev *pDevStr = device_desc;
  pDevStr = pDevStr;    //just for ignore warning
  LOG_printf("VID: %04Xh\r\n", (uint32_t)pDevStr->idVendor);
  LOG_printf("PID: %04Xh\r\n", (uint32_t)pDevStr->idProduct);
}

/*!
    \brief      usb device is successfully assigned the Address
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_address_assigned(void)
{
}

/*!
    \brief      user operation when configuration descriptor is available
    \param[in]  cfg_desc: pointer to configuration descriptor
    \param[in]  itf_desc: pointer to interface descriptor
    \param[in]  ep_desc: pointer to endpoint descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc,
                                           usb_desc_itf *itf_desc,
                                           usb_desc_ep *ep_desc)
{
  usb_desc_itf *id = itf_desc;

  if (0x08U  == (*id).bInterfaceClass) {
    LOG_printf((void *)"> Mass storage device connected.\r\n");
  } else if (0x03U  == (*id).bInterfaceClass) {
    LOG_printf((void *)"> HID device connected.\r\n");
  }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  manufacturer_string: manufacturer string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
  LOG_printf("Manufacturer: %s\r\n", (char *)manufacturer_string);
}

/*!
    \brief      user operation when product string exists
    \param[in]  product_string: product string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
  LOG_printf("Product: %s\r\n", (char *)product_string);
}

/*!
    \brief      user operation when serialNum string exists
    \param[in]  serial_num_string: serialNum string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
  LOG_printf("Serial Number: %s\r\n", (char *)serial_num_string);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
  LOG_printf((void *)"> Enumeration completed.\r\n");
  LOG_printf((void *)"---------------------------------------");
  LOG_printf((void *)"To see the disk information:  ");
  LOG_printf((void *)"Press User Key...             ");
}

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
  LOG_printf((void *)"> Device not supported.\r\n");
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     user response for user key
*/
usbh_user_status usbh_user_userinput(void)
{
  return USBH_USER_RESP_OK;
}

/*!
    \brief      user operation for device overcurrent detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected (void)
{
  LOG_printf((void *)"> Overcurrent detected.\r\n");
}

/*!
    \brief      demo application for mass storage
    \param[in]  pudev: pointer to device
    \param[in]  id: no use here
    \param[out] none
    \retval     status
*/
uint8_t usbh_usr_application_state = USBH_USR_FS_INIT;
int usbh_usr_msc_application(void)
{
  switch(usbh_usr_application_state)
  {
    case USBH_USR_FS_INIT:
      usbh_usr_application_state = USBH_USR_FS_READLIST;
      break;

    case USBH_USR_FS_READLIST:
      usbh_usr_application_state = USBH_USR_FS_WRITEFILE;
      break;

    case USBH_USR_FS_WRITEFILE:
      usbh_usr_application_state = USBH_USR_FS_DEMOEND;
      break;

    case USBH_USR_FS_DEMOEND:
      break;

    default:
      break;
  }
  return(0);
}

/*!
    \brief      deinit user state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{
  usbh_usr_application_state = USBH_USR_FS_INIT;
}

void USB_Init(void)
{
  usb_rcu_config();
  usbh_class_register(&usb_host, &usbh_msc);
  usbh_init(&usb_host, &usr_cb);
  usb_intr_config();
}

void USB_LoopProcess(void)
{
  usbh_core_task(&usb_host);
}

uint8_t USB_IsDeviceConnected(void)
{
  return usbh_core.host.connect_status;
}

uint8_t USBH_USR_Inserted(void)
{
  return u_disk_inserted;
}

uint8_t USBH_UDISK_Status(void)
{
  return usbh_core.host.connect_status;
}

uint8_t USBH_UDISK_Read(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  BYTE status = USBH_FAIL;
  usb_core_driver *udev = (usb_core_driver *)usb_host.data;
  usbh_host *uhost = &usb_host;

  if (udev->host.connect_status)
  {
    usbh_msc_handler *msc = (usbh_msc_handler *)uhost->active_class->class_data;
    while((HOST_CLASS_HANDLER != uhost->cur_state) || (MSC_IDLE != msc->unit[0].state))
    {
      // Precess the unfinished USB event before being called by FatFs
      usbh_core_task(&usb_host);
    }

    do
    {
      status = usbh_msc_read(&usb_host, 0, sector, buf, cnt);
      if (!udev->host.connect_status)
      {
        return 1;
      }
    } while(status == USBH_BUSY);
  }

  if (status == USBH_OK)
    return 0;

  return 1;
}

uint8_t USBH_UDISK_Write(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  BYTE status = USBH_FAIL;
  usb_core_driver *udev = (usb_core_driver *)usb_host.data;
  usbh_host *uhost = &usb_host;

  if (udev->host.connect_status)
  {
    usbh_msc_handler *msc = (usbh_msc_handler *)uhost->active_class->class_data;
    while((HOST_CLASS_HANDLER != uhost->cur_state) || (MSC_IDLE != msc->unit[0].state))
    {
      // Precess the unfinished USB event before being called by FatFs
      usbh_core_task(&usb_host);
    }

    do
    {
      status = usbh_msc_write (&usb_host, 0, sector, (BYTE*)buf, cnt);
      if (!udev->host.connect_status)
      {
        return 1;
      }
    } while(status == USBH_BUSY);
  }

  if (status == USBH_OK)
    return 0;

  return 1;
}

#else

void USB_Init(void)
{
}

void USB_LoopProcess(void)
{
}

uint8_t USB_IsDeviceConnected(void)
{
  return 0;  //failed
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
