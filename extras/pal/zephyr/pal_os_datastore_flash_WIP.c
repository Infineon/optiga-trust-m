/**
 * \copyright
 * MIT License
 *
 * Copyright (c) 2024 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
 * \brief   This file implements the platform abstraction layer APIs for data
 * store.
 *
 * \ingroup  grPAL
 *
 * @{
 */
// #ifdef CONFIG_OPTIGA_TRUST_M_FLASH_SUPPORT
#include "optiga/pal/pal_os_datastore.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
/// @cond hidden

/// Size of length field
#define LENGTH_SIZE (0x02)
/// Size of data store buffer to hold the shielded connection manage context
/// information (2 bytes length field + 64(0x40) bytes context)
// #define MANAGE_CONTEXT_BUFFER_SIZE   (0x42)

#define MANAGE_CONTEXT_BUFFER_OFFSET (0)
#define APP_CONTEXT_OFFSET (MANAGE_CONTEXT_BUFFER_SIZE + LENGTH_SIZE)
#define OPTIGA_SHARED_SECRET_OFFSET (APP_CONTEXT_OFFSET + APP_CONTEXT_SIZE + LENGTH_SIZE)

#define TEST_PARTITION storage_partition

#define TEST_PARTITION_OFFSET FIXED_PARTITION_OFFSET(TEST_PARTITION)
#define TEST_PARTITION_DEVICE FIXED_PARTITION_DEVICE(TEST_PARTITION)

#define FLASH_PAGE_SIZE 512

// Internal buffer to store the shielded connection manage context information
// (length field + Data)
// uint8_t data_store_manage_context_buffer[LENGTH_SIZE + MANAGE_CONTEXT_BUFFER_SIZE];

// Internal buffer to store the optiga application context data during
// hibernate(length field + Data)
// uint8_t data_store_app_context_buffer[LENGTH_SIZE + APP_CONTEXT_SIZE];

// Internal buffer to store the generated platform binding shared secret on Host
// (length field + shared secret)
static const uint8_t optiga_platform_binding_shared_secret[LENGTH_SIZE + OPTIGA_SHARED_SECRET_MAX_LENGTH] = {
    // Length of the shared secret, followed after the length information
    0x00, 0x40,
    // Shared secret. Buffer is defined to the maximum supported length [64
    // bytes]. But the actual size used is to be specified in the length
    // field.
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
    0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40};

uint8_t p_page_buffer[FLASH_PAGE_SIZE];

pal_status_t pal_os_datastore_write(uint16_t datastore_id, const uint8_t *p_buffer, uint16_t length)
{
    const struct device *flash_dev = TEST_PARTITION_DEVICE;
    pal_status_t return_status = PAL_STATUS_FAILURE;
    uint32_t offset = 0;

    printk("Write flash\n");
    if (p_buffer == NULL)
    {
        return PAL_STATUS_INVALID_INPUT;
    }

    if (!device_is_ready(flash_dev))
    {
        printk("Flash device not ready\n");
        return return_status;
    }

    if (p_buffer == NULL)
    {
        return return_status;
    }

    if (flash_read(flash_dev, TEST_PARTITION_OFFSET, &p_page_buffer[0], FLASH_PAGE_SIZE) != 0)
    {
        printk("Initializing flash page.\n");
        // Reading from uninitialized flash page causes error, try to erase and re-read
        if (flash_erase(flash_dev, TEST_PARTITION_OFFSET, FLASH_PAGE_SIZE) != 0)
        {
            printk("nvm_write: Erasing of flash page failed!\n");
            return return_status;
        }

        memset(p_page_buffer, 0x00, FLASH_PAGE_SIZE);
    }

    // Begin Test

    //     p_page_buffer[0] = (uint8_t) (length >> 8);
    //     p_page_buffer[1] = (uint8_t) (length);
    //     memcpy(&p_page_buffer[2], p_buffer, length);

    //     if (flash_write(flash_dev, TEST_PARTITION_OFFSET, &p_page_buffer[0], FLASH_PAGE_SIZE) != 0)
    //     {
    //         printk("Flash write failed!\n");
    //         return PAL_STATUS_FAILURE;
    //     }

    // return PAL_STATUS_SUCCESS;
    // End Test

    switch (datastore_id)
    {
    case OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID: {
        if (length <= OPTIGA_SHARED_SECRET_MAX_LENGTH)
        {
            offset += OPTIGA_SHARED_SECRET_OFFSET;
            return_status = PAL_STATUS_SUCCESS;
        }
        break;
    }
    case OPTIGA_COMMS_MANAGE_CONTEXT_ID: {
        if (length <= MANAGE_CONTEXT_BUFFER_SIZE)
        {
            offset += MANAGE_CONTEXT_BUFFER_OFFSET;
            return_status = PAL_STATUS_SUCCESS;
        }
        break;
    }
    case OPTIGA_HIBERNATE_CONTEXT_ID: {
        if (length <= APP_CONTEXT_SIZE)
        {
            offset += APP_CONTEXT_OFFSET;
            return_status = PAL_STATUS_SUCCESS;
        }
        break;
    }
    default: {
        return_status = PAL_STATUS_INVALID_INPUT;
        break;
    }
    }

    if (return_status == PAL_STATUS_SUCCESS)
    {
        p_page_buffer[offset++] = (uint8_t) (length >> 8);
        p_page_buffer[offset++] = (uint8_t) (length);
        if (length + offset <= FLASH_PAGE_SIZE)
        {
            // flawfinder: ignore
            printk("Writing buffer! Offset: 0x%x - Length %d\n", offset - 2, length + LENGTH_SIZE);
            memcpy(&p_page_buffer[offset], p_buffer, length);
            printk("0x%x 0x%x\n", p_page_buffer[offset - 2], p_page_buffer[offset - 1]);

            if (flash_write(flash_dev, TEST_PARTITION_OFFSET, p_page_buffer, FLASH_PAGE_SIZE) != 0)
            {
                printk("Flash write failed!\n");
                return PAL_STATUS_FAILURE;
            }
        }
        else
        {
            printk("Invalid flash write length: %d\n", length + offset);
            return PAL_STATUS_FAILURE;
        }
    }
    return return_status;
}

pal_status_t pal_os_datastore_read(uint16_t datastore_id, uint8_t *p_buffer, uint16_t *p_buffer_length)
{
    const struct device *flash_dev = TEST_PARTITION_DEVICE;
    pal_status_t return_status = PAL_STATUS_FAILURE;
    uint16_t data_length;
    uint32_t offset = 0; //(uint32_t) TEST_PARTITION_OFFSET;
    size_t max_length = 0;
    int rc = 0;

    if (p_buffer == NULL || p_buffer_length == NULL)
    {
        printk("Read flash failed: Null pointer\n");
        return return_status;
    }

    if (!device_is_ready(flash_dev))
    {
        printk("Flash device not ready\n");
        return return_status;
    }

    // rc = flash_read(flash_dev, TEST_PARTITION_OFFSET, &p_page_buffer[0], FLASH_PAGE_SIZE);
    // if (rc != 0)
    // //if (flash_read(flash_dev, TEST_PARTITION_OFFSET, &p_page_buffer[0], *p_buffer_length) != 0)
    // {
    //     printk("nvm_read: Flash read failed! %x\n", rc);
    //     return PAL_STATUS_FAILURE;
    // }
    // memcpy(p_buffer, &p_page_buffer[2], *p_buffer_length);
    // return PAL_STATUS_SUCCESS;

    switch (datastore_id)
    {
    case OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID: {
        offset += OPTIGA_SHARED_SECRET_OFFSET;
        max_length = OPTIGA_SHARED_SECRET_MAX_LENGTH;
        break;
    }
    case OPTIGA_COMMS_MANAGE_CONTEXT_ID: {
        offset += MANAGE_CONTEXT_BUFFER_OFFSET;
        max_length = MANAGE_CONTEXT_BUFFER_SIZE;
        break;
    }
    case OPTIGA_HIBERNATE_CONTEXT_ID: {
        offset += APP_CONTEXT_OFFSET;
        max_length = APP_CONTEXT_SIZE;
        break;
    }
    default: {
        *p_buffer_length = 0;
        printk("Read flash: Invalid ID");
        return PAL_STATUS_INVALID_INPUT;
    }
    }

    // uint8_t data_length_buffer[LENGTH_SIZE];
    printk("Reading flash! Offset: 0x%x - Length %d\n", offset, max_length + LENGTH_SIZE);

    // rc = flash_read(flash_dev, offset, p_page_buffer, max_length + LENGTH_SIZE);
    rc = flash_read(flash_dev, TEST_PARTITION_OFFSET, p_page_buffer, FLASH_PAGE_SIZE);
    if (rc != 0)
    {
        printk("Flash read failed! RC: 0x%x - Offset: 0x%x - Length %d\n", rc, offset, max_length + LENGTH_SIZE);
        return PAL_STATUS_FAILURE;
    }

    data_length = (uint16_t) (p_page_buffer[offset++] << 8);
    data_length |= (uint16_t) (p_page_buffer[offset++]);

    // No PBS stored in flash, use default value from ROM
    if (datastore_id == OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID && data_length == 0)
    {
        printk("No PBS in Flash, reading default ROM value!\n");
        data_length = (uint16_t) (optiga_platform_binding_shared_secret[0] << 8);
        data_length |= (uint16_t) (optiga_platform_binding_shared_secret[1]);

        memcpy(p_buffer, &optiga_platform_binding_shared_secret[LENGTH_SIZE], data_length);
        *p_buffer_length = data_length;

        return PAL_STATUS_SUCCESS;
    }

    if (data_length <= max_length && data_length <= *p_buffer_length && data_length + LENGTH_SIZE <= FLASH_PAGE_SIZE)
    {
        // printk("Read length: %d\n", data_length);
        //  if (flash_read(flash_dev, offset, p_buffer, data_length) != 0)
        //  {
        //      printf("Flash read failed!\n");
        //      return PAL_STATUS_FAILURE;
        //  }
        memcpy(p_buffer, &p_page_buffer[offset], data_length);
        *p_buffer_length = data_length;
        return_status = PAL_STATUS_SUCCESS;
        printk("Copied %d bytes.\n - 0x%x 0x%x\n", data_length, p_page_buffer[0], p_page_buffer[1]);
    }
    else
    {
        printk("Invalid read sizes\n");
    }
    return return_status;
}
// #endif //CONFIG_OPTIGA_TRUST_M_FLASH_SUPPORT
/// @endcond
/**
 * @}
 */
