/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_datastore_flash.c
 *
 * \brief   This file implements the platform abstraction layer APIs for data store with the low-level Zephyr flash
 * driver.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>

#include "pal_os_datastore.h"

// Reserved sizes for persistent data buffer
/// Size of length field
#define LENGTH_SIZE (0x02)
/// Size of data store buffer to hold the shielded connection manage context information
/// (2 bytes length field + 64(0x40) bytes context)
#define MANAGE_CONTEXT_BUFFER_SIZE (0x42)

#define MANAGE_CONTEXT_BUFFER_OFFSET (0x10)
#define APP_CONTEXT_OFFSET (MANAGE_CONTEXT_BUFFER_OFFSET + MANAGE_CONTEXT_BUFFER_SIZE + LENGTH_SIZE)
#define OPTIGA_SHARED_SECRET_OFFSET (APP_CONTEXT_OFFSET + APP_CONTEXT_SIZE + LENGTH_SIZE)

#define OPTIGA_NVM_DATA_LENGTH \
    (OPTIGA_SHARED_SECRET_OFFSET + OPTIGA_SHARED_SECRET_MAX_LENGTH + LENGTH_SIZE)
#define PAGE_IDX (0)

// Platform specific flash properties
#define TEST_PARTITION storage_partition

#define TEST_PARTITION_OFFSET FIXED_PARTITION_OFFSET(TEST_PARTITION)
#define TEST_PARTITION_DEVICE FIXED_PARTITION_DEVICE(TEST_PARTITION)

#define FLASH_WRITE_BLOCK_SIZE \
    DT_PROP(DT_PARENT(DT_PARENT(DT_NODELABEL(TEST_PARTITION))), write_block_size)
#define FLASH_SECTION_BLOCK_SIZE \
    DT_PROP(DT_PARENT(DT_PARENT(DT_NODELABEL(TEST_PARTITION))), erase_block_size)

#if OPTIGA_NVM_DATA_LENGTH > FLASH_SECTION_BLOCK_SIZE
#error \
    "Platform unsupported: OPTIGA NVM data does not fit into flash block, use NVS driver instead!"
#endif

// Calculate optimal size of page buffer: Some platforms can write individual words, others support only larger blocks
// (e.g. 512 bytes) Buffer size = MAX(OPTIGA_NVM_DATA_LENGTH, FLASH_WRITE_BLOCK_SIZE)
#define FLASH_PAGE_BUFFER_SIZE \
    (OPTIGA_NVM_DATA_LENGTH > FLASH_WRITE_BLOCK_SIZE ? ( \
         (OPTIGA_NVM_DATA_LENGTH % FLASH_WRITE_BLOCK_SIZE) == 0 \
             ? /* Platform supports small writes, find optimal buffer size as multiple of write size */ \
             OPTIGA_NVM_DATA_LENGTH \
             : (OPTIGA_NVM_DATA_LENGTH + FLASH_WRITE_BLOCK_SIZE \
                - (OPTIGA_NVM_DATA_LENGTH % FLASH_WRITE_BLOCK_SIZE)) \
     ) \
                                                     : FLASH_WRITE_BLOCK_SIZE \
    ) /* FLASH_WRITE_BLOCK_SIZE is larger than OPTIGA_DATA, we need to buffer the whole \
                                                                   block */

uint8_t p_page_buffer[FLASH_PAGE_BUFFER_SIZE];

// Magic pattern to identify valid NVM sections
static uint8_t OPTIGA_MEMORY_ID[] = "OPTIGA";  // NOLINT
static off_t page_offset;

static pal_status_t prepare_storage_page() {
    const struct device *flash_dev = TEST_PARTITION_DEVICE;
    bool found_valid_slot = false;
    int8_t slot_index;

    for (slot_index = 1; slot_index >= 0; --slot_index) {
        off_t section_offset = page_offset + (slot_index * FLASH_SECTION_BLOCK_SIZE);
        if (flash_read(flash_dev, section_offset, &p_page_buffer[0], FLASH_PAGE_BUFFER_SIZE) != 0) {
            return PAL_STATUS_FAILURE;
        }

        if (memcmp(&p_page_buffer[0], &OPTIGA_MEMORY_ID[0], sizeof(OPTIGA_MEMORY_ID)) != 0) {
            continue;
        }

        found_valid_slot = true;
        break;
    }

    if (found_valid_slot && slot_index == 0) {
        // Slot 0 valid, no recovery needed.
        return PAL_STATUS_SUCCESS;
    }

    if (flash_erase(flash_dev, page_offset, FLASH_SECTION_BLOCK_SIZE) != 0) {
        return PAL_STATUS_FAILURE;
    }

    if (!found_valid_slot) {
        // Flash sections not initialized, prepare page_buffer for future use
        memset(&p_page_buffer[0], 0x00, FLASH_PAGE_BUFFER_SIZE);
        memcpy(&p_page_buffer[0], &OPTIGA_MEMORY_ID[0], sizeof(OPTIGA_MEMORY_ID));
    }

    if (flash_write(flash_dev, page_offset, &p_page_buffer[0], FLASH_PAGE_BUFFER_SIZE) != 0) {
        return PAL_STATUS_FAILURE;
    }

    // Cleanup recovery section
    if (found_valid_slot) {
        off_t section_offset = page_offset + FLASH_SECTION_BLOCK_SIZE;
        if (flash_erase(flash_dev, section_offset, FLASH_SECTION_BLOCK_SIZE) != 0) {
            return PAL_STATUS_FAILURE;
        }
    }

    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_os_datastore_init() {
    const struct device *flash_dev = TEST_PARTITION_DEVICE;
    struct flash_pages_info flash_info;

    if (flash_get_page_count(flash_dev) < PAGE_IDX) {
        return PAL_STATUS_FAILURE;
    }

    if (flash_get_page_info_by_idx(flash_dev, PAGE_IDX, &flash_info)) {
        return PAL_STATUS_FAILURE;
    }

    page_offset = (FLASH_SECTION_BLOCK_SIZE * PAGE_IDX) + TEST_PARTITION_OFFSET;

    if (prepare_storage_page() != PAL_STATUS_SUCCESS) {
        return PAL_STATUS_FAILURE;
    }

    return PAL_STATUS_SUCCESS;
}

pal_status_t
pal_os_datastore_write(uint16_t datastore_id, const uint8_t *p_buffer, uint16_t length) {
    pal_status_t return_status = PAL_STATUS_FAILURE;
    const struct device *flash_dev = TEST_PARTITION_DEVICE;
    size_t offset = 0;

    if (page_offset == 0) {
        if (pal_os_datastore_init() != PAL_STATUS_SUCCESS) {
            return PAL_STATUS_FAILURE;
        }
    }

    if (!device_is_ready(flash_dev)) {
        return return_status;
    }

    if (flash_read(flash_dev, page_offset, &p_page_buffer[0], FLASH_PAGE_BUFFER_SIZE) != 0) {
        return return_status;
    }

    switch (datastore_id) {
        case OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID: {
            if (length <= OPTIGA_SHARED_SECRET_MAX_LENGTH) {
                offset = OPTIGA_SHARED_SECRET_OFFSET;
                p_page_buffer[offset++] = (uint8_t)(length >> 8);
                p_page_buffer[offset++] = (uint8_t)(length);
                memcpy(&p_page_buffer[offset], p_buffer, length);
                return_status = PAL_STATUS_SUCCESS;
            }
            break;
        }
        case OPTIGA_COMMS_MANAGE_CONTEXT_ID: {
            if (length <= MANAGE_CONTEXT_BUFFER_SIZE) {
                offset = MANAGE_CONTEXT_BUFFER_OFFSET;
                p_page_buffer[offset++] = (uint8_t)(length >> 8);
                p_page_buffer[offset++] = (uint8_t)(length);
                memcpy(&p_page_buffer[offset], p_buffer, length);
                return_status = PAL_STATUS_SUCCESS;
            }
            break;
        }
        case OPTIGA_HIBERNATE_CONTEXT_ID: {
            if (length <= APP_CONTEXT_SIZE) {
                offset = APP_CONTEXT_OFFSET;
                p_page_buffer[offset++] = (uint8_t)(length >> 8);
                p_page_buffer[offset++] = (uint8_t)(length);
                memcpy(&p_page_buffer[offset], p_buffer, length);
                return_status = PAL_STATUS_SUCCESS;
            }
            break;
        }
        default: {
            break;
        }
    }

    // Data is written to mirror-section before erasing the original data. Mirror section is checked on startup,
    // recovers data in case of tearing.
    for (int8_t flash_section = 1; flash_section >= 0; --flash_section) {
        off_t section_offset = page_offset + (flash_section * FLASH_SECTION_BLOCK_SIZE);

        if (flash_erase(flash_dev, section_offset, FLASH_SECTION_BLOCK_SIZE) != 0) {
            return return_status;
        }

        if (flash_write(flash_dev, section_offset, &p_page_buffer[0], FLASH_PAGE_BUFFER_SIZE)
            != 0) {
            return PAL_STATUS_FAILURE;
        }
    }

    // Cleanup mirror section
    if (flash_erase(flash_dev, page_offset + FLASH_SECTION_BLOCK_SIZE, FLASH_SECTION_BLOCK_SIZE)
        != 0) {
        return return_status;
    }

    return return_status;
}

pal_status_t
pal_os_datastore_read(uint16_t datastore_id, uint8_t *p_buffer, uint16_t *p_buffer_length) {
    pal_status_t return_status = PAL_STATUS_FAILURE;
    const struct device *flash_dev = TEST_PARTITION_DEVICE;
    uint16_t data_length;
    size_t offset = 0;

    if (page_offset == 0) {
        if (pal_os_datastore_init() != PAL_STATUS_SUCCESS) {
            return PAL_STATUS_FAILURE;
        }
    }

    if (flash_read(flash_dev, page_offset, &p_page_buffer[0], FLASH_PAGE_BUFFER_SIZE) != 0) {
        return PAL_STATUS_FAILURE;
    }

    switch (datastore_id) {
        case OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID: {
            offset = OPTIGA_SHARED_SECRET_OFFSET;

            data_length = (uint16_t)(p_page_buffer[offset++] << 8);
            data_length |= (uint16_t)(p_page_buffer[offset++]);

            if (data_length > 0) {
                if (data_length > *p_buffer_length) {
                    *p_buffer_length = 0;
                    return_status = PAL_STATUS_INVALID_INPUT;
                } else if (data_length <= OPTIGA_SHARED_SECRET_MAX_LENGTH) {
                    memcpy(p_buffer, &p_page_buffer[offset], data_length);
                    *p_buffer_length = data_length;
                    return_status = PAL_STATUS_SUCCESS;
                }
                break;
            }
            break;
        }
        case OPTIGA_COMMS_MANAGE_CONTEXT_ID: {
            offset = MANAGE_CONTEXT_BUFFER_OFFSET;
            data_length = (uint16_t)(p_page_buffer[offset++] << 8);
            data_length |= (uint16_t)(p_page_buffer[offset++]);

            if (data_length > 0) {
                if (data_length > *p_buffer_length) {
                    *p_buffer_length = 0;
                    return_status = PAL_STATUS_INVALID_INPUT;
                } else if (data_length <= MANAGE_CONTEXT_BUFFER_SIZE) {
                    memcpy(p_buffer, &p_page_buffer[offset], data_length);
                    *p_buffer_length = data_length;
                    return_status = PAL_STATUS_SUCCESS;
                }
                break;
            }
            break;
        }
        case OPTIGA_HIBERNATE_CONTEXT_ID: {
            offset = APP_CONTEXT_OFFSET;
            data_length = (uint16_t)(p_page_buffer[offset++] << 8);
            data_length |= (uint16_t)(p_page_buffer[offset++]);

            if (data_length > 0) {
                if (data_length > *p_buffer_length) {
                    *p_buffer_length = 0;
                    return_status = PAL_STATUS_INVALID_INPUT;
                } else if (data_length <= APP_CONTEXT_SIZE) {
                    memcpy(p_buffer, &p_page_buffer[offset], data_length);
                    *p_buffer_length = data_length;
                    return_status = PAL_STATUS_SUCCESS;
                }
                break;
            }
            break;
        }
        default: {
            *p_buffer_length = 0;
            break;
        }
    }

    return return_status;
}
