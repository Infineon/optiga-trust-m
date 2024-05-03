/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

// ********** typedef
typedef struct _tag_trustm_UID {
    uint8_t bCimIdentifer;
    uint8_t bPlatformIdentifier;
    uint8_t bModelIdentifier;
    uint8_t wROMCode[2];
    uint8_t rgbChipType[6];
    uint8_t rgbBatchNumber[6];
    uint8_t wChipPositionX[2];
    uint8_t wChipPositionY[2];
    uint8_t dwFirmwareIdentifier[4];
    uint8_t rgbESWBuild[2];
} trustm_UID_t;

typedef union _tag_utrustm_UID {
    uint8_t b[27];
    trustm_UID_t st;
} utrustm_UID_t;

// optiga_lib_status_t trustm_readUID(utrustm_UID_t *UID);
void example_optiga_util_read_uid(void);
// optiga_lib_status_t trustm_Open(void);
// optiga_lib_status_t trustm_Close(void);
