/**
* \copyright
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
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
* \file pal_i2c.c
*
* \brief   This file implements the platform abstraction layer(pal) APIs for I2C.
*
* \ingroup  grPAL
* @{
*/

#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "optiga/pal/pal_i2c.h"
#include "pal_linux.h"

#if IFX_I2C_LOG_HAL == 1
#define LOG_HAL IFX_I2C_LOG
#else
#include <stdio.h>
#define LOG_HAL(...) //printf(__VA_ARGS__)
#endif

/// @cond hidden

static pal_status_t pal_i2c_acquire(const pal_i2c_t *p_i2c_context)
{
    pal_linux_t *pal_linux = (pal_linux_t *) p_i2c_context->p_i2c_hw_config;
    if (0 == lockf(pal_linux->i2c_handle, F_TLOCK, 0))
        return PAL_STATUS_SUCCESS;
    else
        return PAL_STATUS_FAILURE;
}

static void pal_i2c_release(const pal_i2c_t *p_i2c_context)
{
    pal_linux_t *pal_linux = (pal_linux_t *) p_i2c_context->p_i2c_hw_config;
    lockf(pal_linux->i2c_handle, F_ULOCK, 0);
}

/// @endcond

static void invoke_upper_layer_callback(const pal_i2c_t *p_pal_i2c_ctx, pal_status_t status)
{
    //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t  type"
    upper_layer_callback_t upper_layer_handler = (upper_layer_callback_t) p_pal_i2c_ctx->upper_layer_event_handler;

    optiga_lib_status_t event;
    switch (status)
    {
    case PAL_STATUS_SUCCESS:
        event = PAL_I2C_EVENT_SUCCESS;
        break;
    case PAL_STATUS_I2C_BUSY:
        event = PAL_I2C_EVENT_BUSY;
        break;
    default:
        event = PAL_I2C_EVENT_ERROR;
        break;
    }

    if (upper_layer_handler)
        upper_layer_handler(p_pal_i2c_ctx->p_upper_layer_ctx, event);
}

static void hexdump(const uint8_t *data, int length, const char *dir)
{
    LOG_HAL("[IFX-HAL]: I2C %s (%d): ", dir, length);

    for (int i = 0; i < length; i++)
    {
        LOG_HAL("%02X ", data[i]);
    }

    LOG_HAL("\n");
}

pal_status_t pal_i2c_init(const pal_i2c_t* p_i2c_context)
{
    pal_linux_t *pal_linux = (pal_linux_t *) p_i2c_context->p_i2c_hw_config;

    pal_linux->i2c_handle = open(pal_linux->i2c_if, O_RDWR);
    if (pal_linux->i2c_handle == -1)
    {
        LOG_HAL("open returned an error = %d (%s)\n", errno, strerror(errno));
        return PAL_STATUS_FAILURE;
    }

    // Assign the slave address
    int ret = ioctl(pal_linux->i2c_handle, I2C_SLAVE, p_i2c_context->slave_address);
    if (ret == -1)
    {
        LOG_HAL("ioctl returned an error = %d (%s)\n", errno, strerror(errno));
        return PAL_STATUS_FAILURE;
    }

    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_deinit(const pal_i2c_t* p_i2c_context)
{
    pal_linux_t *pal_linux = (pal_linux_t *) p_i2c_context->p_i2c_hw_config;
    if (pal_linux->i2c_handle >= 0)
    {
        int ret = close(pal_linux->i2c_handle);
        if (ret == -1)
        {
            LOG_HAL("close returned an error = %d (%s)\n", errno, strerror(errno));
            return PAL_STATUS_FAILURE;
        }
        pal_linux->i2c_handle = -1;
    }
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_write(const pal_i2c_t *p_i2c_context, uint8_t *p_data, uint16_t length)
{
    pal_status_t status;
    pal_linux_t *pal_linux = (pal_linux_t *) p_i2c_context->p_i2c_hw_config;

    hexdump(p_data, length, "TX");

    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        int ret = write(pal_linux->i2c_handle, p_data, length);
        if (-1 == ret)
            status = PAL_STATUS_FAILURE;
        else
            status = PAL_STATUS_SUCCESS;

        pal_i2c_release(p_i2c_context);
    }
    else
        status = PAL_STATUS_I2C_BUSY;

    invoke_upper_layer_callback(p_i2c_context, status);

    return status;
}

pal_status_t pal_i2c_read(const pal_i2c_t *p_i2c_context, uint8_t *p_data, uint16_t length)
{
    pal_status_t status;
    pal_linux_t *pal_linux = (pal_linux_t *) p_i2c_context->p_i2c_hw_config;

    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        int ret = read(pal_linux->i2c_handle, p_data, length);
        if (-1 == ret)
            status = PAL_STATUS_FAILURE;
        else
        {
            status = PAL_STATUS_SUCCESS;
            hexdump(p_data, length, "RX");
        }

        pal_i2c_release(p_i2c_context);
    }
    else
        status = PAL_STATUS_I2C_BUSY;

    invoke_upper_layer_callback(p_i2c_context, status);

    return status;
}

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t* p_i2c_context , uint16_t bitrate)
{
    // Linux' userspace i2c-dev does not allow changing the I2C clock rate
    pal_status_t status = PAL_STATUS_SUCCESS;
    invoke_upper_layer_callback(p_i2c_context, status);
    return status;
}

/**
* @}
*/
