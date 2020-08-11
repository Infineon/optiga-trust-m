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

*/

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

// ********** typedef
typedef struct _tag_trustm_UID {
	uint8_t	bCimIdentifer;
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

//optiga_lib_status_t trustm_readUID(utrustm_UID_t *UID);
void example_optiga_util_read_uid(void);
//optiga_lib_status_t trustm_Open(void);
//optiga_lib_status_t trustm_Close(void);
