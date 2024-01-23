/*!
    \file    usbh_core.c
    \brief   USB host core state machine driver

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

#include "drv_usb_hw.h"
#include "usbh_pipe.h"
#include "usbh_enum.h"
#include "usbh_core.h"
#include "drv_usbh_int.h"
#include <string.h>

usb_core_driver usbh_core;

/* local function prototypes ('static') */
static uint8_t usb_ev_sof           (usbh_host *puhost);
static uint8_t usb_ev_connect       (usbh_host *puhost);
static uint8_t usb_ev_disconnect    (usbh_host *puhost);

static usbh_status usbh_enum_task (usbh_host *puhost);

#if USB_LOW_POWER
static void usb_hwp_suspend(usb_core_driver *pudev);
static void usb_hwp_resume(usb_core_driver *pudev);
#endif /* USB_LOW_POWER */

usbh_ev_cb usbh_int_op = 
{
    usb_ev_connect,
    usb_ev_disconnect,
    usb_ev_sof,
};

usbh_ev_cb *usbh_int_fop = &usbh_int_op;

/*!
    \brief      USB host stack initializations
    \param[in]  puhost: pointer to USB host
    \param[in]  user_cb: pointer to user callback
    \param[out] none
    \retval     none
*/
void usbh_init (usbh_host *puhost, usbh_user_cb *user_cb)
{
    /* host de-initializations */
    usbh_deinit(puhost);

    puhost->usr_cb = user_cb;

    usbh_core.host.connect_status = 0U;

    for (uint8_t i = 0U; i < USBFS_MAX_TX_FIFOS; i++) {
        usbh_core.host.pipe[i].err_count = 0U;
        usbh_core.host.pipe[i].pp_status = PIPE_IDLE;
        usbh_core.host.backup_xfercount[i] = 0U;
    }

    usbh_core.host.pipe[0].ep.mps = 8U;

#ifdef USE_USB_FS
    usb_basic_init (&usbh_core.bp, &usbh_core.regs, USB_CORE_ENUM_FS);
#endif /* USE_USB_FS */

#ifndef DUAL_ROLE_MODE_ENABLED
    usb_globalint_disable(&usbh_core.regs);

    usb_core_init (usbh_core.bp, &usbh_core.regs);

#ifndef USE_OTG_MODE
    usb_curmode_set (&usbh_core.regs, HOST_MODE);
#endif /* USE_OTG_MODE */

    usb_host_init (&usbh_core);

    usb_globalint_enable(&usbh_core.regs);
#endif /* DUAL_ROLE_MODE_ENABLED */

    /* link driver to the stack */
    usbh_core.host.data = (void *)puhost;
    puhost->data = (void *)&usbh_core;

    /* upon init call usr call back */
    puhost->usr_cb->dev_init();
}

/*!
    \brief      USB host register device class
    \param[in]  puhost: pointer to usb host instance
    \param[in]  puclass: pointer to USB device class
    \param[out] none
    \retval     operation status
*/
usbh_status usbh_class_register (usbh_host *puhost, usbh_class *puclass)
{
    usbh_status status = USBH_OK;

    if (NULL != puclass) {
        if (puhost->class_num < USBH_MAX_SUPPORTED_CLASS) {
            puhost->uclass[puhost->class_num++] = puclass;
        } else {
            status = USBH_FAIL;
        }
    } else {
        status = USBH_FAIL;
    }

    return status;
}

/*!
    \brief      de-initialize USB host
    \param[in]  puhost: pointer to USB host
    \param[out] none
    \retval     operation status
*/
usbh_status usbh_deinit(usbh_host *puhost)
{
    /* software init */
    puhost->cur_state = HOST_DEFAULT;
    puhost->backup_state = HOST_DEFAULT;
    puhost->enum_state = ENUM_DEFAULT;

    puhost->control.ctl_state = CTL_IDLE;
    puhost->control.max_len = USB_FS_EP0_MAX_LEN;

    puhost->dev_prop.addr = USBH_DEV_ADDR_DEFAULT;
    puhost->dev_prop.speed = PORT_SPEED_FULL;
    puhost->dev_prop.cur_itf = 0xFFU;

    usbh_pipe_free(&usbh_core, puhost->control.pipe_in_num);
    usbh_pipe_free(&usbh_core, puhost->control.pipe_out_num);

    return USBH_OK;
}

/*!
    \brief      USB host core main state machine process
    \param[in]  puhost: pointer to USB host
    \param[out] none
    \retval     none
*/
void usbh_core_task (usbh_host *puhost)
{
    volatile usbh_status status = USBH_FAIL;

    /* check for host port events */
    if (((0U == usbh_core.host.connect_status) || (0U == usbh_core.host.port_enabled)) && (HOST_DEFAULT != puhost->cur_state)) {
        if (puhost->cur_state != HOST_DEV_DETACHED) {
            puhost->cur_state = HOST_DEV_DETACHED;
        }
    }

    switch (puhost->cur_state) {
    case HOST_DEFAULT:
        if (usbh_core.host.connect_status) {
            puhost->cur_state = HOST_DETECT_DEV_SPEED;

            usb_mdelay (100U);

            usb_port_reset (&usbh_core);

            puhost->usr_cb->dev_reset();
        }
        break;

    case HOST_DETECT_DEV_SPEED:
        if (usbh_core.host.port_enabled) {
            puhost->cur_state = HOST_DEV_CONNECT;

            puhost->dev_prop.speed = usb_curspeed_get (&usbh_core);

            puhost->usr_cb->dev_speed_detected(puhost->dev_prop.speed);

            usb_mdelay (50U);
        }
        break;

    case HOST_DEV_CONNECT:
        puhost->usr_cb->dev_attach();
        puhost->control.pipe_out_num = usbh_pipe_allocate(&usbh_core, 0x00U);
        puhost->control.pipe_in_num = usbh_pipe_allocate(&usbh_core, 0x80U);

        /* open IN control pipe */
        usbh_pipe_create (&usbh_core,
                          &puhost->dev_prop,
                          puhost->control.pipe_in_num,
                          USB_EPTYPE_CTRL,
                          (uint16_t)puhost->control.max_len);

        /* open OUT control pipe */
        usbh_pipe_create (&usbh_core,
                          &puhost->dev_prop,
                          puhost->control.pipe_out_num,
                          USB_EPTYPE_CTRL,
                          (uint16_t)puhost->control.max_len);

        puhost->cur_state = HOST_DEV_ENUM;
        break;

    case HOST_DEV_ENUM:
        /* check for enumeration status */
        if (USBH_OK == usbh_enum_task (puhost)) {
            /* the function shall return USBH_OK when full enumeration is complete */

            /* user callback for end of device basic enumeration */
            puhost->usr_cb->dev_enumerated();

#if USB_LOW_POWER
            puhost->cur_state = HOST_SUSPEND;

            /* judge device remote wakup function */
            if ((puhost->dev_prop.cfg_desc_set.cfg_desc.bmAttributes) & (1U << 5)) {
                puhost->dev_supp_remote_wkup = 1;
            }else{
                puhost->dev_supp_remote_wkup = 0;
            }
#else
            puhost->cur_state = HOST_PWR_FEATURE_SET;
#endif /* USB_LOW_POWER */
        }
        break;

    case HOST_PWR_FEATURE_SET:
        if ((puhost->dev_prop.cfg_desc_set.cfg_desc.bmAttributes) & (1U << 5)) {
            if (usbh_setdevfeature(puhost, FEATURE_SELECTOR_REMOTEWAKEUP, 0U) == USBH_OK) {
                puhost->cur_state = HOST_CLASS_CHECK;
            }
        } else {
            puhost->cur_state = HOST_CLASS_CHECK;
        }
        break;

    case HOST_CLASS_CHECK:
        if (puhost->class_num == 0U) {
            puhost->cur_state = HOST_ERROR;
        } else {
            puhost->active_class = NULL;

            uint8_t itf_class = puhost->dev_prop.cfg_desc_set.itf_desc_set[0][0].itf_desc.bInterfaceClass;

            for (uint8_t index = 0U; index < puhost->class_num; index++) {
                if ((puhost->uclass[index]->class_code == itf_class) || (0xFFU == itf_class)) {
                    puhost->active_class = puhost->uclass[index];
                }
            }

            if (puhost->active_class != NULL) {
                puhost->cur_state = HOST_USER_INPUT;
            } else {
                puhost->cur_state = HOST_ERROR;
            }
        }
        break;

    case HOST_USER_INPUT:
        /* the function should return user response true to move to class state */
        if (USR_IN_RESP_OK == puhost->usr_cb->dev_user_input()) {
            if ((USBH_OK == puhost->active_class->class_init(puhost))) {
                puhost->cur_state = HOST_CLASS_ENUM;
            }
        }
        break;

#if USB_LOW_POWER
    case HOST_SUSPEND:
        if(puhost->dev_supp_remote_wkup){
            /* send set feature command*/
            if (USBH_OK == usbh_setdevfeature(puhost, FEATURE_SELECTOR_REMOTEWAKEUP, 0U)) {

                usb_hwp_suspend(&usbh_core);

                usb_mdelay(20U);
                puhost->suspend_flag = 1;
                puhost->usr_cb->dev_user_input();

                /* MCU enter deep-sleep*/
                pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_DISABLE, WFI_CMD);
                puhost->cur_state = HOST_WAKEUP;
            }
        }else {
                /* host suspend */
                usb_hwp_suspend(&usbh_core);

                usb_mdelay(20U);
                puhost->suspend_flag = 1U;
                puhost->usr_cb->dev_user_input();

                /* MCU enter deep-sleep */
                pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_DISABLE, WFI_CMD);
                puhost->cur_state = HOST_WAKEUP;
        }
        break;

    case HOST_WAKEUP:
         /* judge suspend status */
        if (0 == puhost->suspend_flag) {
            usb_hwp_resume(&usbh_core);
            usb_mdelay(500U);
    
            if(puhost->dev_supp_remote_wkup){
                if (USBH_OK == usbh_clrdevfeature(puhost, FEATURE_SELECTOR_DEV, 0U)) {
                    /* user callback for initialization */
                    puhost->usr_cb->dev_init();
                    puhost->cur_state = HOST_CLASS_CHECK;
                }
            } else{
                    puhost->cur_state = HOST_CLASS_CHECK;
            }
        }
        break;
#endif /* USB_LOW_POWER */

    case HOST_CLASS_ENUM:
        /* process class standard contol requests state machine */
        status = puhost->active_class->class_requests(puhost);

        if (USBH_OK == status) {
            puhost->cur_state = HOST_CLASS_HANDLER;
        } else {
            usbh_error_handler (puhost, status);
        }
        break;

    case HOST_CLASS_HANDLER:
        /* process class state machine */
        status = puhost->active_class->class_machine(puhost);

        usbh_error_handler (puhost, status);
        break;

    case HOST_ERROR:
        /* re-initilaize host for new enumeration */
        usbh_deinit (puhost);
        puhost->usr_cb->dev_deinit();
        puhost->active_class->class_deinit(puhost);
        break;

    case HOST_DEV_DETACHED:
        /* manage user disconnect operations*/
        puhost->usr_cb->dev_detach();

        /* re-initilaize host for new enumeration */
        usbh_deinit(puhost);
        puhost->usr_cb->dev_deinit();
        puhost->active_class->class_deinit(puhost);
        usbh_pipe_delete(&usbh_core);
        puhost->cur_state = HOST_DEFAULT;
        break;

    default:
        break;
    }
}

/*!
    \brief      handle the error on USB host side
    \param[in]  puhost: pointer to USB host
    \param[in]  err_type: type of error or busy/OK state
    \param[out] none
    \retval     none
*/
void usbh_error_handler (usbh_host *puhost, usbh_status err_type)
{
    /* error unrecovered or not supported device speed */
    if ((USBH_SPEED_UNKNOWN_ERROR == err_type) || (USBH_UNRECOVERED_ERROR == err_type)) {
        puhost->usr_cb->dev_error();

        puhost->cur_state = HOST_ERROR;
    } else if (USBH_APPLY_DEINIT == err_type) {
        puhost->cur_state = HOST_ERROR;

        /* user callback for initalization */
        puhost->usr_cb->dev_init();
    } else {
        /* no operation */
    }
}

/*!
    \brief      USB SOF event function from the interrupt
    \param[in]  puhost: pointer to USB host
    \param[out] none
    \retval     operation status
*/
static uint8_t usb_ev_sof (usbh_host *puhost)
{
    /* this callback could be used to implement a scheduler process */
    puhost->control.timer = (uint16_t)usb_curframe_get(&usbh_core);

    if (puhost->active_class != NULL) {
        if (puhost->active_class->class_sof != NULL) {
            puhost->active_class->class_sof(puhost);
        }
    }

    return 0U;
}

/*!
    \brief      USB connect callback function from the interrupt
    \param[in]  puhost: pointer to usb host
    \param[out] none
    \retval     operation status
*/
static uint8_t usb_ev_connect (usbh_host *puhost)
{
    usbh_core.host.connect_status = 1U;

    return 0U;
}

/*!
    \brief      USB disconnect callback function from the interrupt
    \param[in]  puhost: pointer to usb host
    \param[out] none
    \retval     operation status
*/
static uint8_t usb_ev_disconnect (usbh_host *puhost)
{
    usbh_core.host.connect_status = 0U;

    return 0U;
}

/*!
    \brief      handle the USB enumeration task
    \param[in]  puhost: pointer to host
    \param[out] none
    \retval     none
*/
static usbh_status usbh_enum_task (usbh_host *puhost)
{
    uint8_t str_buf[512];

    usbh_status status = USBH_BUSY;

    static uint8_t index_mfc_str = 0U, index_prod_str = 0U, index_serial_str = 0U;

    switch (puhost->enum_state) {
    case ENUM_DEFAULT:
        /* get device descriptor for only 1st 8 bytes : to get ep0 maxpacketsize */
        if (USBH_OK == usbh_devdesc_get (puhost, 8U)) {
            puhost->control.max_len = puhost->dev_prop.dev_desc.bMaxPacketSize0;

            /* modify control channels configuration for maximum packet size */
            usbh_pipe_update (&usbh_core,
                              puhost->control.pipe_out_num,
                              0U, 0U,
                              (uint16_t)puhost->control.max_len);

            usbh_pipe_update (&usbh_core,
                              puhost->control.pipe_in_num,
                              0U, 0U,
                              (uint16_t)puhost->control.max_len);

            puhost->enum_state = ENUM_GET_DEV_DESC;
        }
        break;

    case ENUM_GET_DEV_DESC:
        /* get full device descriptor */
        if (USBH_OK == usbh_devdesc_get (puhost, USB_DEV_DESC_LEN)) {
            puhost->usr_cb->dev_devdesc_assigned(&puhost->dev_prop.dev_desc);

            index_mfc_str = puhost->dev_prop.dev_desc.iManufacturer;
            index_prod_str = puhost->dev_prop.dev_desc.iProduct;
            index_serial_str = puhost->dev_prop.dev_desc.iSerialNumber;

            puhost->enum_state = ENUM_SET_ADDR;
        }
        break;

    case ENUM_SET_ADDR: 
        /* set address */
        if (USBH_OK == usbh_setaddress (puhost, USBH_DEV_ADDR)) {
            usb_mdelay (2U);

            puhost->dev_prop.addr = USBH_DEV_ADDR;

            /* user callback for device address assigned */
            puhost->usr_cb->dev_address_set();

            /* modify control channels to update device address */
            usbh_pipe_update (&usbh_core,
                              puhost->control.pipe_in_num,
                              puhost->dev_prop.addr,
                              0U, 0U);

            usbh_pipe_update (&usbh_core,
                              puhost->control.pipe_out_num,
                              puhost->dev_prop.addr,
                              0U, 0U);

            puhost->enum_state = ENUM_GET_CFG_DESC;
        }
        break;

    case ENUM_GET_CFG_DESC:
        /* get standard configuration descriptor */
        if (USBH_OK == usbh_cfgdesc_get (puhost, USB_CFG_DESC_LEN)) {
            puhost->enum_state = ENUM_GET_CFG_DESC_SET;
        }
        break;

    case ENUM_GET_CFG_DESC_SET:
        /* get full config descriptor (config, interface, endpoints) */
        if (USBH_OK == usbh_cfgdesc_get (puhost, puhost->dev_prop.cfg_desc_set.cfg_desc.wTotalLength)) {
            /* user callback for configuration descriptors available */
            puhost->usr_cb->dev_cfgdesc_assigned (&puhost->dev_prop.cfg_desc_set.cfg_desc,
                                                  &puhost->dev_prop.cfg_desc_set.itf_desc_set[0][0].itf_desc,
                                                  &puhost->dev_prop.cfg_desc_set.itf_desc_set[0][0].ep_desc[0]);

            puhost->enum_state = ENUM_GET_STR_DESC;
        }
        break;

    case ENUM_GET_STR_DESC:
        if (index_mfc_str) {
            if (USBH_OK == usbh_strdesc_get (puhost, 
                                             puhost->dev_prop.dev_desc.iManufacturer, 
                                             str_buf,
                                             0xFFU)) {
                /* user callback for manufacturing string */
                puhost->usr_cb->dev_mfc_str(str_buf);

                index_mfc_str = 0U;
            }
        } else {
            if (index_prod_str) {
                /* check that product string is available */
                if (USBH_OK == usbh_strdesc_get (puhost, 
                                                 puhost->dev_prop.dev_desc.iProduct, 
                                                 str_buf, 
                                                 0xFFU)) {
                    puhost->usr_cb->dev_prod_str(str_buf);

                    index_prod_str = 0U;
                }
            } else {
                if (index_serial_str) {
                    if (USBH_OK == usbh_strdesc_get (puhost, 
                                                     puhost->dev_prop.dev_desc.iSerialNumber,
                                                     str_buf, 
                                                     0xFFU)) {
                        puhost->usr_cb->dev_seral_str(str_buf);
                        puhost->enum_state = ENUM_SET_CONFIGURATION;
                        index_serial_str = 0U;
                    }
                } else {
                    puhost->enum_state = ENUM_SET_CONFIGURATION;
                }
            }
        }
        break;

    case ENUM_SET_CONFIGURATION:
        if (USBH_OK == usbh_setcfg (puhost, (uint16_t)puhost->dev_prop.cfg_desc_set.cfg_desc.bConfigurationValue)) {
            puhost->enum_state = ENUM_DEV_CONFIGURED;
        }
        break;

    case ENUM_DEV_CONFIGURED:
        status = USBH_OK;
        break;

    default:
        break;
    }

    return status;
}


#if USB_LOW_POWER

/*!
    \brief      handles the USB resume from suspend mode
    \param[in]  pudev: pointer to selected USB device
    \param[out] none
    \retval     none
*/
static void usb_hwp_resume(usb_core_driver *pudev)
{
    __IO uint32_t hprt = 0U;

    /*  switch-on the clocks */
    *pudev->regs.PWRCLKCTL &= ~PWRCLKCTL_SUCLK;

    *pudev->regs.PWRCLKCTL &= ~PWRCLKCTL_SHCLK;

    hprt = usb_port_read(pudev);

    hprt &= ~HPCS_PSP;
    hprt |= HPCS_PREM;

    *pudev->regs.HPCS = hprt;

    usb_mdelay (20U);

    hprt &= ~HPCS_PREM;

    *pudev->regs.HPCS = hprt;
}

/*!
    \brief      handles the USB enter to suspend mode
    \param[in]  pudev: pointer to selected USB device
    \param[out] none
    \retval     none
*/
static void usb_hwp_suspend(usb_core_driver *pudev)
{
    __IO uint32_t hprt = 0U;

    hprt = usb_port_read(pudev);

    hprt |= HPCS_PSP;

    *pudev->regs.HPCS = hprt;

    /*  switch-off the clocks */
    *pudev->regs.PWRCLKCTL |= PWRCLKCTL_SUCLK;

    *pudev->regs.PWRCLKCTL |= PWRCLKCTL_SHCLK;
}

#endif /* USB_LOW_POWER */
