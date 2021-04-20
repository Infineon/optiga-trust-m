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
* \file pal_gpio.c
*
* \brief   This file implements the platform abstraction layer APIs for GPIO.
*
* \ingroup  grPAL
* @{
*/
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "errno.h"

#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "pal_linux.h"

#define PAL_GPIO_LOG(...) fprintf(stderr, __VA_ARGS__)

static const char *IN = "in";
static const char *OUT = "out";
static const char *LOW = "0";
static const char *HIGH = "1";

static pal_status_t pal_gpio_export_unexport(const char *op, int pin)
{
    char buffer[32];
    int ret = snprintf(buffer, sizeof(buffer), "/sys/class/gpio/%s", op);
    if (ret < 0 || ret == sizeof(buffer))
    {
        PAL_GPIO_LOG("Failed to format GPIO %s file\n", op);
        return PAL_STATUS_FAILURE;
    }

    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        PAL_GPIO_LOG("Failed to open GPIO %s file for writing: %s\n", op, strerror(errno));
        return PAL_STATUS_FAILURE;
    }

    ret = snprintf(buffer, sizeof(buffer), "%d", pin);
    if (ret < 0 || ret == sizeof(buffer))
    {
        PAL_GPIO_LOG("Failed to format string for GPIO %s\n", op);
        close(fd);
        return PAL_STATUS_FAILURE;
    }

    ssize_t written = write(fd, buffer, ret);
    if (written == -1)
    {
        PAL_GPIO_LOG("Failed to %s GPIO #%d: %s\n", op, pin, strerror(errno));
        close(fd);
        return PAL_STATUS_FAILURE;
    }

    close(fd);
    return PAL_STATUS_SUCCESS;
}

static pal_status_t pal_gpio_export(int pin)
{
    char path[48];
    int ret = snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    if (ret < 0 || ret == sizeof(path))
    {
        PAL_GPIO_LOG("Failed to format GPIO value file\n");
        return PAL_STATUS_FAILURE;
    }

    // test if already exported
    if (access(path, F_OK) == 0)
        return PAL_STATUS_SUCCESS;

    return pal_gpio_export_unexport("export", pin);
}

static pal_status_t pal_gpio_unexport(int pin)
{
    return pal_gpio_export_unexport("unexport", pin);
}

static pal_status_t pal_gpio_direction(int pin, const char *dir)
{
    char path[48];
    int ret = snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    if (ret <= 0 || ret == sizeof(path))
    {
        PAL_GPIO_LOG("Failed to format GPIO direction file\n");
        return PAL_STATUS_FAILURE;
    }

    int fd = open(path, O_WRONLY);
    if (fd == -1)
    {
        PAL_GPIO_LOG("Failed to open GPIO #%d direction file for writing: %s\n", pin, strerror(errno));
        return PAL_STATUS_FAILURE;
    }

    ssize_t written = write(fd, dir, strnlen(dir, 3));
    if (written == -1)
    {
        PAL_GPIO_LOG("Failed to write GPIO #%d direction file: %s\n", pin, strerror(errno));
        close(fd);
        return PAL_STATUS_FAILURE;
    }

    close(fd);
    return PAL_STATUS_SUCCESS;
}

static pal_status_t pal_gpio_write(pal_linux_gpio_t *pin, const char *value)
{
    ssize_t written = write(pin->fd, value, 1);
    if (written == -1)
	{
        PAL_GPIO_LOG("Failed to write GPIO pin #%d: %s\n", pin->pin_nr, strerror(errno));
        return PAL_STATUS_FAILURE;
    }
    return PAL_STATUS_SUCCESS;
}

//lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context)
{
    pal_linux_gpio_t *gpio = (pal_linux_gpio_t *) p_gpio_context->p_gpio_hw;
    if (gpio != NULL)
    {
        int pin = gpio->pin_nr;

        pal_status_t status = pal_gpio_export(pin);
        if (status != PAL_STATUS_SUCCESS)
            return status;

        status = pal_gpio_direction(pin, OUT);
        if (status != PAL_STATUS_SUCCESS)
            return status;

        char path[48];
        int ret = snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
        if (ret < 0 || ret == sizeof(path))
        {
            PAL_GPIO_LOG("Failed to format GPIO value file\n");
            return PAL_STATUS_FAILURE;
        }

        int fd = open(path, O_WRONLY);
        if (fd == -1)
        {
            PAL_GPIO_LOG("Failed to open GPIO value file for writing: %s\n", strerror(errno));
            return PAL_STATUS_I2C_BUSY;
        }
        gpio->fd = fd;
    }

    return PAL_STATUS_SUCCESS;
}

//lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context)
{
    pal_linux_gpio_t *gpio = (pal_linux_gpio_t *) p_gpio_context->p_gpio_hw;
    if (gpio != NULL)
    {
        pal_status_t status = pal_gpio_unexport(gpio->pin_nr);
        if (status != PAL_STATUS_SUCCESS)
            return status;

        close(gpio->fd);
    }

    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t *p_gpio_context)
{
    pal_linux_gpio_t *gpio = (pal_linux_gpio_t *) p_gpio_context->p_gpio_hw;
    if (gpio != NULL)
        pal_gpio_write(gpio, HIGH);
}

void pal_gpio_set_low(const pal_gpio_t *p_gpio_context)
{
    pal_linux_gpio_t *gpio = (pal_linux_gpio_t *) p_gpio_context->p_gpio_hw;
    if (gpio != NULL)
        pal_gpio_write(gpio, LOW);
}

/**
* @}
*/
