/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
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

extern uint32_t get_sys_tick_ms(void);

uint8_t init_flag = 0;

//lint --e{552,714} suppress "Accessed by user of this structure"
pal_logger_t logger_console =
{
        .logger_config_ptr = (void*)(&cunit_logger),
        .logger_rx_flag = 1,
        .logger_tx_flag = 1
};
//lint --e{552} suppress "Accessed by user of this structure"
pal_logger_t cunit_console =
{
        .logger_config_ptr = NULL,
        .logger_rx_flag = 1,
        .logger_tx_flag = 1
};

void cunit_console_uart_end_of_transmit_callback(void)
{
	logger_console.logger_tx_flag = 0;
}

void cunit_console_uart_end_of_receive_callback(void)
{
	logger_console.logger_rx_flag = 0;
}

/// @endcond


pal_status_t pal_logger_init(void * p_logger_context)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t * p_log_context = p_logger_context;
    DAVE_STATUS_t status;

    do
    {
    	status = (DAVE_STATUS_t)UART_Init(&cunit_logger);
		return_status = PAL_STATUS_SUCCESS;
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

            return_status = PAL_STATUS_SUCCESS;
        }
        else
        {
        	p_logger_context = NULL;
            return_status = PAL_STATUS_SUCCESS;
        }
    }while(FALSE);
    return return_status;
}


pal_status_t pal_logger_write(void * p_logger_context, const uint8_t * p_log_data, uint32_t log_data_length)
{

    int32_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t * p_log_context = p_logger_context;
    volatile uint32_t time_out = 0;
    static uint32_t len = 0;
    len = log_data_length;

    UART_t  * UART;

    UART = p_log_context->logger_config_ptr;

    do
    {
        // The instance of VMCOMS is internally declared
        if(NULL == p_log_context->logger_config_ptr)
        {

            return_status = PAL_STATUS_SUCCESS;
        }
        else
        {
			for (size_t i = 0; i < len; ++i)
			{
				time_out = 0;
				XMC_UART_CH_Transmit(UART->channel, *(const uint8_t *)p_log_data);
				p_log_data++;
				while(time_out++ < 0x1f5);
			}
			return_status = PAL_STATUS_SUCCESS;
        }
    } while(0);
    time_out = 0;
    return ((pal_status_t)return_status);
}

pal_status_t pal_logger_read(void * p_logger_context, uint8_t * p_log_data, uint32_t log_data_length)
{
	volatile uint32_t time_out = 0;
    int32_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t * p_log_context = p_logger_context;

    do
    {
        // The instance of VMCOMS is internally declared
        if(NULL == p_log_context->logger_config_ptr)
        {

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
				if(init_flag == 0)
				{
					time_out++;
					if(time_out > 0x1fffff)
					{
						p_log_context->logger_rx_flag = 0;
						UART_AbortReceive(p_log_context->logger_config_ptr);
						break;
					}
				}
            }
            return_status = PAL_STATUS_SUCCESS;
        }
    } while(0);
    return ((pal_status_t)return_status);
}
/**
 * @}
 */
