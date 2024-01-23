/*!
    \file    usbh_msc_fatfs.c
    \brief   USB MSC host FATFS related functions

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
    \version 2022-06-10, V3.1.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#include "usb_conf.h"
#include "diskio.h"
#include "usbh_msc_core.h"

static volatile DSTATUS state = STA_NOINIT; /* disk status */

extern usbh_host usb_host;

/*!
    \brief      initialize the disk drive
    \param[in]  drv: physical drive number (0)
    \param[out] none
    \retval     operation status
*/
DSTATUS disk_initialize (BYTE drv)
{
    usb_core_driver *pudev = (usb_core_driver *)usb_host.data;

    if (pudev->host.connect_status) {
        state &= ~STA_NOINIT;
    }

    return state;
}

/*!
    \brief      get disk status
    \param[in]  drv: physical drive number (0)
    \param[out] none
    \retval     operation status
*/
DSTATUS disk_status (BYTE drv)
{
    if (drv) {
        return STA_NOINIT; /* supports only single drive */
    }

    return state;
}

/*!
    \brief      read sectors
    \param[in]  drv: physical drive number (0)
    \param[in]  buff: pointer to the data buffer to store read data
    \param[in]  sector: start sector number (LBA)
    \param[in]  count: sector count (1..255)
    \param[out] none
    \retval     operation status
*/
DRESULT disk_read (BYTE drv, BYTE *buff, DWORD sector, UINT count)
{
    BYTE status = USBH_OK;
    usb_core_driver *pudev = (usb_core_driver *)usb_host.data;

    if (drv || (!count)) {
        return RES_PARERR;
    }

    if (state & STA_NOINIT) {
        return RES_NOTRDY;
    }

    if (pudev->host.connect_status) {
        do {
            status = usbh_msc_read (&usb_host, drv, sector, buff, count);

            if (!pudev->host.connect_status) {
                return RES_ERROR;
            }
        } while(status == USBH_BUSY);
    }

    if (status == USBH_OK) {
        return RES_OK;
    }

    return RES_ERROR;
}

#if _READONLY == 0U

/*!
    \brief      write sectors
    \param[in]  drv: physical drive number (0)
    \param[in]  buff: pointer to the data buffer to store read data
    \param[in]  sector: start sector number (LBA)
    \param[in]  count: sector count (1..255)
    \param[out] none
    \retval     operation status
*/
DRESULT disk_write (BYTE drv, const BYTE *buff, DWORD sector, UINT count)
{
    BYTE status = USBH_OK;
    usb_core_driver *pudev = (usb_core_driver *)usb_host.data;

    if ((!count) || drv) {
        return RES_PARERR;
    }

    if (state & STA_NOINIT) {
        return RES_NOTRDY;
    }

    if (state & STA_PROTECT) {
        return RES_WRPRT;
    }

    if (pudev->host.connect_status) {
        do {
            status = usbh_msc_write (&usb_host, drv, sector, (BYTE*)buff, count);

            if (!pudev->host.connect_status) {
                return RES_ERROR;
            }
        } while(status == USBH_BUSY);
    }

    if (status == USBH_OK) {
        return RES_OK;
    }

    return RES_ERROR;
}

#endif /* _READONLY == 0 */

/*!
    \brief      I/O control function
    \param[in]  drv: physical drive number (0)
    \param[in]  ctrl: control code
    \param[in]  buff: pointer to the data buffer to store read data
    \param[out] none
    \retval     operation status
*/
DRESULT disk_ioctl (BYTE drv, BYTE ctrl, void *buff)
{
    DRESULT res = RES_OK;
    msc_lun info;

    if (drv) {
        return RES_PARERR;
    }

    res = RES_ERROR;

    if (state & STA_NOINIT) {
        return RES_NOTRDY;
    }

    switch (ctrl) {
    /* make sure that no pending write process */
    case CTRL_SYNC:
        res = RES_OK;
        break;

    /* get number of sectors on the disk (dword) */
    case GET_SECTOR_COUNT:
        if (USBH_OK == usbh_msc_lun_info_get(&usb_host, drv, &info)) {
            *(DWORD*)buff = (DWORD)info.capacity.block_nbr;
            res = RES_OK;
        }
        break;

    /* get r/w sector size (word) */
    case GET_SECTOR_SIZE:
        if (USBH_OK == usbh_msc_lun_info_get(&usb_host, drv, &info)) {
            *(WORD*)buff = (DWORD)info.capacity.block_size;
            res = RES_OK;
        }
        break;

    /* get erase block size in unit of sector (dword) */
    case GET_BLOCK_SIZE:
        *(DWORD*)buff = 512;
        break;

    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

/*!
    \brief      get fat time
    \param[in]  none
    \param[out] none
    \retval     time value
*/
DWORD get_fattime(void) {

    return    ((DWORD)(2019U - 1980U) << 25U)   /* Year 2019 */
            | ((DWORD)1U << 21U)                /* Month 1 */
            | ((DWORD)1U << 16U)                /* Mday 1 */
            | ((DWORD)0U << 11U)                /* Hour 0 */
            | ((DWORD)0U << 5U)                 /* Min 0 */
            | ((DWORD)0U >> 1U);                /* Sec 0 */
}
