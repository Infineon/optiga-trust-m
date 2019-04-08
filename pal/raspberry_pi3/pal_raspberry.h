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
* \file pal_raspberry.h
*
* \brief   This file provides the prototype declarations w.r.t raspberry Pi3.
*
* \ingroup  grPAL
* @{
*/

#ifndef _PAL_RASPBERRY_H_
#define _PAL_RASPBERRY_H_

#include "optiga/pal/pal.h"
#include <pthread.h>

#define false 0
#define true 1

#define HIGH 1
#define LOW 0
typedef uint32_t gpio_pin_t;

/** @brief PAL I2C context structure */
typedef struct pal_raspberry
{
    /// This field consists the handle for I2c device
    int32_t i2c_handle;
    /// Pointer to store the callers handler
    void * upper_layer_event_handler;
} pal_raspberry_t;


typedef void*(*thread_start_routine_t)(void*);
 
typedef struct thread_attr
{
	pthread_t thread;
	void* (*thread_function)(void*);
}thread_attr_t;

#endif
