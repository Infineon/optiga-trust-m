/**
* \copyright
* MIT License
*
* Copyright (c) 2020 Infineon Technologies AG
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
* \file ifx_i2c_config.c
*
* \brief   This file provides the ifx i2c platform specific context configurations.
*
* \ingroup  grIFXI2C
*
* @{
*/

// Protocol Stack Includes
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"

#if defined OPTIGA_COMMS_SHIELDED_CONNECTION    
/** @brief This is Data Store context. Only one context is supported per slave.
 *
 * - The following parameters in #ifx_i2c_datastore_config_t must be initialized with appropriate values <br>
 *   - <b>protocol_version</b> : Protocol version supported for secure communication.
 *   - <b>datastore_shared_secret_id</b> : ID to read and write the shared secret.
 *   - <b>datastore_shared_secret_id</b> : ID to read and write the shielded connection context data.<br>
 *   - <b>shared_secret_length</b> : Max supported shared secret length.
 */
 
ifx_i2c_datastore_config_t ifx_i2c_datastore_config =
{
    /// Protocol version
    PROTOCOL_VERSION_PRE_SHARED_SECRET,
    /// Platform binding shared secret host Pal datastore ID
    OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID,
    /// Manage context Pal datastore ID
    OPTIGA_COMMS_MANAGE_CONTEXT_ID,
    /// Max shared secret length
    OPTIGA_SHARED_SECRET_MAX_LENGTH,
};
#endif
/** @brief This is IFX I2C context. Only one context is supported per slave.
 *
 * - The following parameters in #ifx_i2c_context_t must be initialized with appropriate values <br>
 *   - <b>slave address</b> : Address of I2C slave
 *   - <b>frame_size</b> : Frame size in bytes. Minimum supported value is 16 bytes.<br> 
 *              - It is recommended not to use a value greater than the slave's frame size.
 *              - The user specified frame size is written to I2C slave's frame size register.
 *                The frame size register is read back from I2C slave.
 *                This frame value is used by the ifx-i2c protocol even if it is not equal to the user specified value.
 *
 *   - <b>frequency</b> : Frequency/speed of I2C master in KHz.
 *              - This must be lowest of the maximum frequency supported by the devices (master/slave) connected on the 
 *                bus.
 *              - Initial negotiation starts with a frequency of 100KHz.
 *              - If the user specified frequency is more than 400 KHz, the I2C slave is configured to operate in 
 *                "Fm+" mode, otherwise the I2C slave is configured for "SM & Fm" mode. <br>
 *              - If the user specified frequency negotiation fails, the I2C master frequency remains at 
 *                100KHz<br>
 * 
 *   - <b>upper_layer_event_handler</b> : Upper layer event handler. This is invoked when #ifx_i2c_open() is 
 *                                        asynchronously completed.
 *   - <b>upper_layer_ctx</b> : Context of upper layer.
 *   - <b>p_slave_vdd_pin</b> : GPIO pin for VDD. If not set, cold reset is not done. 
 *   - <b>p_slave_reset_pin</b> : GPIO pin for Reset. If not set, warm reset is not done.
 *   - <b>ifx_i2c_datastore_config</b> : Data store configuration instance
 * - The other fields must be initialized by the user of the structure.
 */
//lint --e{785} suppress "Only required fields are initialized by default, the rest are handled by user of this structure"
ifx_i2c_context_t ifx_i2c_context_0 =
{
    /// Slave address
    IFX_I2C_BASE_ADDR,
    /// i2c-master frequency
    400,
    /// IFX-I2C frame size
    IFX_I2C_FRAME_SIZE,
    /// Vdd pin
    &optiga_vdd_0,
    /// Reset pin
    &optiga_reset_0,
    /// optiga pal i2c context
    &optiga_pal_i2c_context_0,
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION            
    /// Data store context
    &ifx_i2c_datastore_config
#endif    
};

/**
* @}
*/
