/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_i2c.h
 *
 * \brief   This file provides the prototype declarations of PAL I2C.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#ifndef _PAL_I2C_H_
#define _PAL_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pal.h"

/// Event returned when I2C master completes execution
#define PAL_I2C_EVENT_SUCCESS (0x0000)
/// Event returned when I2C master operation fails
#define PAL_I2C_EVENT_ERROR (0x0001)
/// Event returned when lower level I2C bus is busy
#define PAL_I2C_EVENT_BUSY (0x0002)

/** @brief PAL I2C context structure */
typedef struct pal_i2c {
    /// Pointer to I2C master platform specific context
    void *p_i2c_hw_config;
    /// Pointer to store the callers context information
    void *p_upper_layer_ctx;
    /// Pointer to store the callers handler
    void *upper_layer_event_handler;
    /// I2C slave address
    uint8_t slave_address;
} pal_i2c_t;

/**
 * @brief Initializes the I2C master.
 *
 * Initializes the I2C master with the given context.
 * <br>
 *
 * \details
 * - The platform specific initialization of I2C master has to be implemented as part of this API, if required.<br>
 * - If the target platform does not demand explicit initialization of i2c master
 *   (Example: If the platform driver takes care of init after the reset), it would not be required to implement.<br>
 * - The implementation must take care the following scenarios depending upon the target platform selected.
 *   - The implementation must handle the acquiring and releasing of the I2C bus before initializing the I2C master to
 *     avoid interrupting the ongoing slave I2C transactions using the same I2C master.
 *   - If the I2C bus is in busy state, the API must not initialize and return #PAL_STATUS_I2C_BUSY status.
 *   - Repeated initialization must be taken care with respect to the platform requirements. (Example: Multiple users/applications
 *     sharing the same I2C master resource)
 *
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_i2c_context      Valid pointer to the PAL I2C context that should be initialized
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C master init it successful
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C init fails.
 */
LIBRARY_EXPORTS pal_status_t pal_i2c_init(const pal_i2c_t *p_i2c_context);

/**
 * @brief Sets the I2C Master bitrate
 *
 * Sets the bitrate/speed(KHz) of I2C master.
 * <br>
 *
 * \details
 * - Sets the bitrate of I2C master if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released after the setting the bitrate.<br>
 * - This API must take care of setting the bitrate to I2C master's maximum supported value.
 * - Eg. In XMC4800, the maximum supported bitrate is 400 KHz. If the supplied bitrate is greater than 400KHz, the API will
 *   set the I2C master's bitrate to 400KHz.
 * - If upper_layer_event_handler is initialized, the upper layer handler is invoked with the respective event
 *   status listed below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails to set the bit rate
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successful
 *<br>
 *
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_i2c_context  Valid pointer to the PAL I2C context
 * \param[in] bitrate        Bitrate to be used by I2C master in KHz
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the setting of bitrate is successfully completed
 * \retval  #PAL_STATUS_FAILURE  Returns when the setting of bitrate fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy.
 */
LIBRARY_EXPORTS pal_status_t pal_i2c_set_bitrate(const pal_i2c_t *p_i2c_context, uint16_t bitrate);

/**
 * @brief Writes on I2C bus.
 *
 * Writes the data to I2C slave.
 * <br>
 * <br>
 * \image html pal_i2c_write.png "pal_i2c_write()" width=20cm
 *
 *
 * \details
 * - The API attempts to write if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released only after the completion of transmission or after completion of error handling.<br>
 * - The API invokes the upper layer handler with the respective event status as explained below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed asynchronously
 *<br>
 *
 *
 * \pre
 * - None
 *
 * \note
 *  - Otherwise the below implementation has to be updated to handle different bitrates based on the input context.<br>
 *  - The caller of this API must take care of the guard time based on the slave's requirement.<br>
 *  - The upper_layer_event_handler must be initialized in the p_i2c_context before invoking the API.<br>
 *
 * \param[in] p_i2c_context  Valid pointer to the PAL I2C context #pal_i2c_t
 * \param[in] p_data         Pointer to the data to be written
 * \param[in] length         Length of the data to be written
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C write is invoked successfully
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C write fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy.
 */
LIBRARY_EXPORTS pal_status_t
pal_i2c_write(const pal_i2c_t *p_i2c_context, uint8_t *p_data, uint16_t length);

/**
 * @brief Reads from I2C bus.
 *
 * Reads the data from I2C slave.
 * <br>
 * <br>
 * \image html pal_i2c_read.png "pal_i2c_read()" width=20cm
 *
 * \details
 * - The API attempts to read if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released only after the completion of reception or after completion of error handling.<br>
 * - The API invokes the upper layer handler with the respective event status as explained below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed asynchronously
 *<br>
 *
 * \pre
 * - None
 *
 * \note
 *  - Otherwise the below implementation has to be updated to handle different bitrates based on the input context.<br>
 *  - The caller of this API must take care of the guard time based on the slave's requirement.<br>
 *
 * \param[in]  p_i2c_context  pointer to the PAL I2C context #pal_i2c_t
 * \param[in]  p_data         Pointer to the data buffer to store the read data
 * \param[in]  length         Length of the data to be read
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C read is invoked successfully
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C read fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy.
 */
LIBRARY_EXPORTS pal_status_t
pal_i2c_read(const pal_i2c_t *p_i2c_context, uint8_t *p_data, uint16_t length);

/**
 * @brief De-initializes the I2C master.
 *
 * De-initializes the I2C master with the specified context.
 * <br>
 *
 * \details
 * - The platform specific de-initialization of I2C master has to be implemented as part of this API, if required.<br>
 * - If the target platform does not demand explicit de-initialization of i2c master
 *   (Example: If the platform driver takes care of init after the reset), it would not be required to implement.<br>
 * - The implementation must take care the following scenarios depending upon the target platform selected.
 *   - The implementation must handle the acquiring and releasing of the I2C bus before de-initializing the I2C master to
 *     avoid interrupting the ongoing slave I2C transactions using the same I2C master.
 *   - If the I2C bus is in busy state, the API must not de-initialize and return #PAL_STATUS_I2C_BUSY status.
 *     - This API must ensure that multiple users/applications sharing the same I2C master resource is not impacted.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_i2c_context   Valid pointer to the PAL I2C context that should be de-initialized
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C master de-init it successful
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C de-init fails.
 */
LIBRARY_EXPORTS pal_status_t pal_i2c_deinit(const pal_i2c_t *p_i2c_context);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_I2C_H_ */

/**
 * @}
 */
