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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "optiga/optiga_util.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"
#include "optiga/comms/optiga_comms.h"
#include "optiga/optiga_crypt.h"
#include "trustm_chipinfo.h"
#include "optiga/common/optiga_lib_types.h"


/* Callback when for optiga_util_read_data operation
*/
static volatile optiga_lib_status_t read_data_status;
//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_callback(void * context, optiga_lib_status_t return_status)
{
	read_data_status = return_status;
   if (NULL != context)
   {
       // callback to upper layer here
   }
}


#define PRINT_UID   1
#define DECODE_UID   0

/**********************************************************************
* example_optiga_util_read_uid(void)
**********************************************************************/
void example_optiga_util_read_uid(void)
{
    uint16_t offset, bytes_to_read;
    uint16_t optiga_oid;
    uint8_t read_data_buffer[27];
    optiga_util_t * me = NULL;
    uint8_t logging_status = 0;

    optiga_lib_status_t return_status;
    uint16_t i=0;
    utrustm_UID_t UID;

    configPRINTF((">example_optiga_util_read_uid()\r\n"));

    //Clear all memory
    memset(read_data_buffer, 0x0, sizeof(read_data_buffer));
    memset(UID.b, 0x0, sizeof(read_data_buffer) );

    /* Block for 50ms. */
    const TickType_t xDelay = 50 / portTICK_PERIOD_MS;

    do
    {
        /**
         * 1. Create OPTIGA Util Instance
         */
        me = optiga_util_create(0, optiga_util_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
         * 2. Read the device UID location 0xE0C2
         */
        //Read device UID
        optiga_oid = 0xE0C2;
        offset = 0x00;
        bytes_to_read = sizeof(read_data_buffer);

        read_data_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_read_data(me,
											  optiga_oid,
                                              offset,
                                              read_data_buffer,
                                              &bytes_to_read);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
        	configPRINTF(("trustm_readUID(): Failed to read data from OID\n"));
            break;
        }

        while (OPTIGA_LIB_BUSY == read_data_status)
        {
            //Wait until the optiga_util_read_data operation is completed
   			i++;
   			vTaskDelay(xDelay);
   			if (i == 50)
   				break;
        }

        if (OPTIGA_LIB_SUCCESS != read_data_status)
        {
        	configPRINTF(("trustm_readUID(): Failed to read data from OID.\n"));
            break;
        }

        logging_status = 1;
    } while(FALSE);

        if (me)
        {
            //Destroy the instance after the completion of usecase if not required.
            return_status = optiga_util_destroy(me);
        }

#if (PRINT_UID==1)
        if(logging_status == 1)
        {
			configPRINTF( ("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n",
						read_data_buffer[i],read_data_buffer[i+1],read_data_buffer[i+2],read_data_buffer[i+3],read_data_buffer[i+4],read_data_buffer[i+5],read_data_buffer[i+6],read_data_buffer[i+7]));
			configPRINTF( ("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n",
								read_data_buffer[i+8],read_data_buffer[i+9],read_data_buffer[i+10],read_data_buffer[i+11],read_data_buffer[i+12],read_data_buffer[i+13],read_data_buffer[i+14],read_data_buffer[i+15]));
			configPRINTF( ("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n",
								read_data_buffer[i+16],read_data_buffer[i+17],read_data_buffer[i+18],read_data_buffer[i+19],read_data_buffer[i+20],read_data_buffer[i+21],read_data_buffer[i+22],read_data_buffer[i+23]));
			configPRINTF( ("%.2x %.2x %.2x \r\n",
								read_data_buffer[i+24],read_data_buffer[i+25],read_data_buffer[i+26]));
        }
#endif

//fully decode the UID field
#if (PRINT_UID==1 && DECODE_UID==1)
        if(logging_status == 1)
        {
			for (i=0;i<bytes_to_read;i++)
			{
				UID.b[i] = read_data_buffer[i];
			}

			configPRINTF( ( "==================== Trust M Chip Info ===========================\n" ) );
			configPRINTF(("CIM Identifier             [bCimIdentifer]: 0x%.2x\n", UID.st.bCimIdentifer));
			configPRINTF(("Platform Identifer   [bPlatformIdentifier]: 0x%.2x\n", UID.st.bPlatformIdentifier));
			configPRINTF(("Model Identifer         [bModelIdentifier]: 0x%.2x\n", UID.st.bModelIdentifier));
			configPRINTF(("ID of ROM mask                  [wROMCode]: 0x%.2x%.2x\n",
									UID.st.wROMCode[0],
									UID.st.wROMCode[1]));
			configPRINTF(("Chip Type                    [rgbChipType]: 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x\n",
									UID.st.rgbChipType[0],
									UID.st.rgbChipType[1],
									UID.st.rgbChipType[2],
									UID.st.rgbChipType[3],
									UID.st.rgbChipType[4],
									UID.st.rgbChipType[5]));
			configPRINTF(("Batch Number              [rgbBatchNumber]: 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x\n",
									UID.st.rgbBatchNumber[0],
									UID.st.rgbBatchNumber[1],
									UID.st.rgbBatchNumber[2],
									UID.st.rgbBatchNumber[3],
									UID.st.rgbBatchNumber[4],
									UID.st.rgbBatchNumber[5]));
			configPRINTF(("X-coordinate              [wChipPositionX]: 0x%.2x%.2x\n",
									UID.st.wChipPositionX[0],
									UID.st.wChipPositionX[1]));
			configPRINTF(("Y-coordinate              [wChipPositionY]: 0x%.2x%.2x\n",
									UID.st.wChipPositionY[0],
									UID.st.wChipPositionY[1]));
			configPRINTF(("Firmware Identifier [dwFirmwareIdentifier]: 0x%.2x%.2x%.2x%.2x\n",
									UID.st.dwFirmwareIdentifier[0],
									UID.st.dwFirmwareIdentifier[1],
									UID.st.dwFirmwareIdentifier[2],
									UID.st.dwFirmwareIdentifier[3]));
			configPRINTF(("Build Number                 [rgbESWBuild]: %.2x %.2x\n",
									UID.st.rgbESWBuild[0],
									UID.st.rgbESWBuild[1]));

			configPRINTF(("\n"));

			configPRINTF(("====================================================================\n"));
        }
#endif

    configPRINTF(("<example_optiga_util_read_uid()\r\n"));
    return;
}
