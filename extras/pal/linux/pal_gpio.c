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

#include "pal_gpio.h"
#include "pal_ifx_i2c_config.h"
#include "pal_linux.h"

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1


static int
GPIOExport(int pin)
{
#define BUFFER_MAX 4
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    usleep(100000);
    return(0);
}

static int
GPIOUnexport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    usleep(100000);
    return(0);
}

static int
GPIODirection(int pin, int dir)
{
    static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 34
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio direction for writing!\n");
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
        fprintf(stderr, "Failed to set direction!\n");
        return(-1);
    }

    close(fd);
    usleep(100000);
    return(0);
}

static int
GPIOWrite(pal_linux_gpio_t* pin, int value)
{
    if (write(pin->fd, value == LOW ? "0" : "1", 1) != 1) 
	{
        // can't use printf, because it may execute in signal handler
		int errsv = errno;
		char err_msg[100];
		sprintf(err_msg, "Failed to write value! Erro code = %d, fd = %d\n", errsv, pin->fd);
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        return(-1);
    }
    return(0);
}

#define GPIO_VALUE_FMT_STR "/sys/class/gpio/gpio%d/value"

//lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_init(const pal_gpio_t * p_gpio_context)
{
#define VALUE_MAX 30
    char path[VALUE_MAX] = {0};

    if (p_gpio_context->p_gpio_hw != NULL)
    {
        pal_linux_gpio_t* gpio = p_gpio_context->p_gpio_hw;
        int res_pin = gpio->pin_nr;
        
		/*
         * Enable GPIO pins
         */
        if (-1 == GPIOExport(res_pin))
            return(1);

        /*
         * Set GPIO directions
         */
        if (-1 == GPIODirection(res_pin, OUT))
            return(2);

        snprintf(path, VALUE_MAX, GPIO_VALUE_FMT_STR, res_pin);
        int fd = open(path, O_WRONLY);
        if (fd < 0) {
            fprintf(stderr, "Failed to open gpio value for writing!\n");
            return(2);
        }

        // store fd
        gpio->fd = fd;
    }

    return PAL_STATUS_SUCCESS;
}

//lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_deinit(const pal_gpio_t * p_gpio_context)
{
    if (p_gpio_context->p_gpio_hw != NULL)
    {
        pal_linux_gpio_t* gpio = (pal_linux_gpio_t*)(p_gpio_context->p_gpio_hw);
        /*
         * Disable GPIO pins
         */
        if (-1 == GPIOUnexport(gpio->pin_nr))
            return(1);

        close(gpio->fd);
    }
        
    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t * p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        /*
        * Write GPIO value
        */
        GPIOWrite((pal_linux_gpio_t*)(p_gpio_context->p_gpio_hw), HIGH );
    }
}

void pal_gpio_set_low(const pal_gpio_t* p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        /*
        * Write GPIO value
        */
        GPIOWrite((pal_linux_gpio_t*)(p_gpio_context->p_gpio_hw), LOW);
    }
}

/**
* @}
*/
