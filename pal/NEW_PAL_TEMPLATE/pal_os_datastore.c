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
* \file pal_os_datastore.c
*
* \brief   This file implements the platform abstraction layer APIs for data store.
*
* \ingroup  grPAL
*
* @{
*/

#include "optiga/pal/pal_os_datastore.h"

/// Size of data store buffer
#define DATA_STORE_BUFFERSIZE      (0x42)

/// Maximum shared secret length 
#define SHARED_SECRET_MAX_LENGTH   (0x40U)

//Internal buffer to store manage context data use for data store
uint8_t data_store_buffer [DATA_STORE_BUFFERSIZE];

//Internal buffer to store application context data use for data store
uint8_t data_store_app_context_buffer [APP_CONTEXT_SIZE];

//Internal buffer to store the generated shared secret on Host
uint8_t optiga_platform_binding_shared_secret [SHARED_SECRET_MAX_LENGTH] = {0x00};


pal_status_t pal_os_datastore_write(uint16_t datastore_id,
                                    const uint8_t * p_buffer,
                                    uint16_t length)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;

    switch(datastore_id)
    {
        case OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID:
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            // This has to be enhanced by user only, in case of updating
            // the platform binding shared secret during the runtime into NVM.
            // In current implementation, platform binding shared secret is 
            // stored in RAM.
            if (length <= sizeof(optiga_platform_binding_shared_secret))
            {
                memcpy(optiga_platform_binding_shared_secret, p_buffer, length);
                return_status = PAL_STATUS_SUCCESS;
            }
            break;
        }
        case OPTIGA_COMMS_MANAGE_CONTEXT_ID:
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            // This has to be enhanced by user only, in case of storing 
            // the manage context information in non-volatile memory 
            // to reuse for later during hard reset scenarios where the 
            // RAM gets flushed out.
            memcpy(data_store_buffer,p_buffer,length);
            return_status = PAL_STATUS_SUCCESS;
            break;
        }
        case OPTIGA_HIBERNATE_CONTEXT_ID:
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            // This has to be enhanced by user only, in case of storing 
            // the application context information in non-volatile memory 
            // to reuse for later during hard reset scenarios where the 
            // RAM gets flushed out.
            memcpy(data_store_app_context_buffer,p_buffer,length);
            return_status = PAL_STATUS_SUCCESS;
            break;
        }
        default:
        {
            break;
        }
    }
    return return_status;
}


pal_status_t pal_os_datastore_read(uint16_t datastore_id, 
                                   uint8_t * p_buffer, 
                                   uint16_t * p_buffer_length)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;

    switch(datastore_id)
    {
        case OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID:
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            // This has to be enhanced by user only,
            // if the platform binding shared secret is stored in non-volatile 
            // memory with a specific location and not as a const text segement 
            // else updating the share secret content is good enough.

            if (*p_buffer_length >= sizeof(optiga_platform_binding_shared_secret))
            {
                memcpy(p_buffer,optiga_platform_binding_shared_secret, 
                       sizeof(optiga_platform_binding_shared_secret));
                *p_buffer_length = sizeof(optiga_platform_binding_shared_secret);
                return_status = PAL_STATUS_SUCCESS;
            }
            break;
        }
        case OPTIGA_COMMS_MANAGE_CONTEXT_ID:
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            // This has to be enhanced by user only,
            // if manage context information is stored in NVM during the hibernate, 
            // else this is not required to be enhanced.
            memcpy(p_buffer,data_store_buffer,*p_buffer_length);
            return_status = PAL_STATUS_SUCCESS;
            break;
        }
        case OPTIGA_HIBERNATE_CONTEXT_ID:
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            // This has to be enhanced by user only,
            // if application context information is stored in NVM during the hibernate, 
            // else this is not required to be enhanced.
            memcpy(p_buffer,data_store_app_context_buffer,*p_buffer_length);
            return_status = PAL_STATUS_SUCCESS;
            break;
        }
        default:
        {
            break;
        }
    }

    return return_status;
}

/**
* @}
*/
