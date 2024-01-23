/*!
    \file    usbh_msc_bbb.h
    \brief   header file for usbh_msc_bbb.c

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

#ifndef __USBH_MSC_BBB_H
#define __USBH_MSC_BBB_H

#include "usbh_enum.h"
#include "msc_bbb.h"

typedef union 
{
    msc_bbb_cbw field;

    uint8_t CBWArray[31];
}usbh_cbw_pkt;

typedef union
{
    msc_bbb_csw field;

    uint8_t CSWArray[13];
}usbh_csw_pkt;

enum usbh_msc_state 
{
    USBH_MSC_BBB_INIT_STATE = 0U,
    USBH_MSC_BBB_RESET,
    USBH_MSC_GET_MAX_LUN,
    USBH_MSC_TEST_UNIT_READY,
    USBH_MSC_READ_CAPACITY10,
    USBH_MSC_MODE_SENSE6,
    USBH_MSC_REQUEST_SENSE,
    USBH_MSC_BBB_USB_TRANSFERS,
    USBH_MSC_DEFAULT_APPLI_STATE,
    USBH_MSC_CTRL_ERROR_STATE,
    USBH_MSC_UNRECOVERED_STATE
};

typedef enum
{
    BBB_OK = 0U,
    BBB_FAIL,
    BBB_PHASE_ERROR,
    BBB_BUSY
} bbb_status;

typedef enum
{
    BBB_CMD_IDLE = 0U,
    BBB_CMD_SEND,
    BBB_CMD_WAIT,
} bbb_cmd_state;

/* csw status definitions */
typedef enum
{
    BBB_CSW_CMD_PASSED = 0U,
    BBB_CSW_CMD_FAILED,
    BBB_CSW_PHASE_ERROR,
} bbb_csw_status;

typedef enum
{
    BBB_SEND_CBW = 1U,
    BBB_SEND_CBW_WAIT,
    BBB_DATA_IN,
    BBB_DATA_IN_WAIT,
    BBB_DATA_OUT,
    BBB_DATA_OUT_WAIT,
    BBB_RECEIVE_CSW,
    BBB_RECEIVE_CSW_WAIT,
    BBB_ERROR_IN,
    BBB_ERROR_OUT,
    BBB_UNRECOVERED_ERROR
} bbb_state;

typedef struct
{
    uint8_t                *pbuf;
    uint32_t                data[16];
    bbb_state               state;
    bbb_state               prev_state;
    bbb_cmd_state           cmd_state;
    usbh_cbw_pkt            cbw;
    usbh_csw_pkt            csw;
} bbb_handle;

#define USBH_MSC_BBB_CBW_TAG                0x20304050U

#define USBH_MSC_CSW_MAX_LENGTH             63U

#define USBH_MSC_SEND_CSW_DISABLE           0U
#define USBH_MSC_SEND_CSW_ENABLE            1U

#define USBH_MSC_DIR_IN                     0U
#define USBH_MSC_DIR_OUT                    1U
#define USBH_MSC_BOTH_DIR                   2U

#define USBH_MSC_PAGE_LENGTH                512U

#define CBW_CB_LENGTH                       16U
#define CBW_LENGTH                          10U
#define CBW_LENGTH_TEST_UNIT_READY          0U

#define MAX_BULK_STALL_COUNT_LIMIT          0x04U   /*!< If STALL is seen on Bulk 
                                                      Endpoint continously, this means 
                                                      that device and Host has phase error
                                                      Hence a Reset is needed */

/* function declarations */
/* initialize the mass storage parameters */
void usbh_msc_bbb_init (usbh_host *puhost);
/* manage the different states of BOT transfer and updates the status to upper layer */
usbh_status usbh_msc_bbb_process (usbh_host *puhost, uint8_t lun);
/* manages the different error handling for stall */
usbh_status usbh_msc_bbb_abort (usbh_host *puhost, uint8_t direction);
/* reset MSC bot request structure */
usbh_status usbh_msc_bbb_reset (usbh_host *puhost);
/* decode the CSW received by the device and updates the same to upper layer */
bbb_csw_status usbh_msc_csw_decode (usbh_host *puhost);

#endif /* __USBH_MSC_BBB_H */
