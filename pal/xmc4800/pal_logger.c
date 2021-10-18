/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
*
* \author Infineon Technologies AG
 *
 * \file    pal_logger.c
 *
 * \brief   This file provides the prototypes declarations for pal logger.
 *
 * \ingroup grPAL
 *
 * @{
 */
#include <DAVE.h>
#include "optiga/pal/pal_logger.h"

/// @cond hidden

//lint --e{552,714} suppress "Accessed by user of this structure" 
pal_logger_t logger_console =
{
        .logger_config_ptr = NULL,
        .logger_rx_flag = 1,
        .logger_tx_flag = 1
};
//lint --e{552} suppress "Accessed by user of this structure"
pal_logger_t cunit_console =
{
        .logger_config_ptr = (void*)(&cunit_logger),
        .logger_rx_flag = 1,
        .logger_tx_flag = 1
};

void cunit_console_uart_end_of_transmit_callback(void)
{
    cunit_console.logger_tx_flag = 0;
}

void cunit_console_uart_end_of_receive_callback(void)
{
    cunit_console.logger_rx_flag = 0;
}

/// @endcond


pal_status_t pal_logger_init(void * p_logger_context)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t * p_log_context = p_logger_context;

    do
    {
        // The instance of VMCOMS is internally declared
        if(NULL == p_log_context->logger_config_ptr)
        {
            if(USBD_VCOM_STATUS_SUCCESS != USBD_VCOM_Connect())
            {
                break;
            }

            while(!USBD_VCOM_IsEnumDone()){};
            return_status = PAL_STATUS_SUCCESS;
        }
        else
        {
            return_status = PAL_STATUS_SUCCESS;
        }
    }while(FALSE);
    return return_status;
}


pal_status_t pal_logger_deinit(void * p_logger_context)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t * p_log_context = p_logger_context;

    do
    {
        // The instance of VMCOMS is internally declared
        if(NULL == p_log_context->logger_config_ptr)
        {
            //lint --e{534} suppress "The return status of USBD_VCOM_Disconnect is not required to be checked"
            USBD_VCOM_Disconnect();
            return_status = PAL_STATUS_SUCCESS;
        }
        else
        {
            return_status = PAL_STATUS_SUCCESS;
        }
    }while(FALSE);
    return return_status;
}


pal_status_t pal_logger_write(void * p_logger_context, const uint8_t * p_log_data, uint32_t log_data_length)
{

    int32_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t * p_log_context = p_logger_context;

    do
    {
        // The instance of VMCOMS is internally declared
        if(NULL == p_log_context->logger_config_ptr)
        {
            if (USBD_VCOM_STATUS_SUCCESS != USBD_VCOM_SendData((int8_t *)p_log_data,(uint16_t)log_data_length))
            {
                break;
            }
            CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
            return_status = PAL_STATUS_SUCCESS;
        }
        else
        {
            p_log_context->logger_tx_flag = 1U;

            return_status = (int32_t)UART_Transmit(p_log_context->logger_config_ptr, (uint8_t *)p_log_data, log_data_length);
            if (return_status != PAL_STATUS_SUCCESS)
            {
                break;
            }
            while (p_log_context->logger_tx_flag)
            {

            }
            return_status = PAL_STATUS_SUCCESS;
        }
    } while(0);
    return ((pal_status_t)return_status);
}

pal_status_t pal_logger_read(void * p_logger_context, uint8_t * p_log_data, uint32_t log_data_length)
{

    int32_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t * p_log_context = p_logger_context;

    do
    {
        // The instance of VMCOMS is internally declared
        if(NULL == p_log_context->logger_config_ptr)
        {
            if (USBD_VCOM_STATUS_SUCCESS != USBD_VCOM_ReceiveData((int8_t * )p_log_data,(uint16_t)log_data_length))
            {
                break;
            }
            CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
            return_status = PAL_STATUS_SUCCESS;
        }
        else
        {
            p_log_context->logger_rx_flag = 1U;
            return_status = (int32_t)UART_Receive(p_log_context->logger_config_ptr, p_log_data, log_data_length);
            if (return_status != PAL_STATUS_SUCCESS)
            {
                break;
            }
            while (p_log_context->logger_rx_flag)
            {

            }
            return_status = PAL_STATUS_SUCCESS;
        }
    } while(0);
    return ((pal_status_t)return_status);
}
/**
 * @}
 */
