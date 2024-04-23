/**
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
* @{
*/

#include "optiga_crypt.h"
#include "optiga_util.h"
#include "pal_os_timer.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

#include "optiga_crypt.h"
#include "optiga_util.h"
#include "optiga_lib_common.h"

optiga_lib_status_t crypt_event_completed_status;

//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_crypt_event_completed(void * context, optiga_lib_status_t return_status)
{
    crypt_event_completed_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    int error = 0;
    optiga_crypt_t * me = NULL;
    optiga_lib_status_t command_queue_status = OPTIGA_CRYPT_ERROR;

    if (olen != NULL)
    {
        me = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
        if (NULL == me)
        {
            // MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE
            error = -0x0034;
        }
        else
        {
            crypt_event_completed_status = OPTIGA_LIB_BUSY;
            command_queue_status = optiga_crypt_random(me, OPTIGA_RNG_TYPE_TRNG, output, len);
            if( command_queue_status != OPTIGA_LIB_SUCCESS)
            {
                // MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE
                error = -0x0034;
            }

            if (!error)
            {
                while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
                {
                    pal_os_timer_delay_in_milliseconds(5);
                }

                if(crypt_event_completed_status!= OPTIGA_LIB_SUCCESS)
                {
                    // MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE
                    error = -0x0034;
                }
                else
                {
                    *olen = len;
                }
            }
        }
      optiga_crypt_destroy(me); //CCW => Seems to be missing?
    }

    return error;
    
}

#endif
