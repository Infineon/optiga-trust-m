/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
* \file example_utilities.c
*
* \brief   This file provides the utilities for example code.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "pal_os_memory.h"
#include "pal_os_timer.h"
#include "optiga_lib_logger.h"
#include "optiga_lib_return_codes.h"

char performance_buffer_string[30];

#define OPTIGA_EXAMPLE_UTIL_RSA_NEGATIVE_INTEGER          (0x7F)
#define OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG               (0x03)
#define OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS             (0x00)

#define OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(buffer, index, value) \
                                {\
                                    uint16_t position = *index; \
                                    if (0xFF < value) \
                                    { \
                                        buffer[position++] = (value & 0xFF00) >> 8; \
                                    } \
                                    buffer[position++] = (value & 0xFF); \
                                    *index = position; \
                                }
                                
//lint --e{838} suppress "first time use of modulus_signed_bit_len and length_field_in_bytes"
void example_util_encode_rsa_public_key_in_bit_string_format(const uint8_t * n_buffer,
                                                             uint16_t n_length,
                                                             const uint8_t * e_buffer,
                                                             uint16_t e_length,
                                                             uint8_t * pub_key_buffer,
                                                             uint16_t * pub_key_length)
{
#define OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD     (0x80)
#define OPTIGA_EXAMPLE_UTIL_RSA_DER_SEQUENCE_TAG      (0x30)
#define OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG       (0x02)

    uint16_t index = 0;
    uint16_t modulus_signed_bit_len = 0;
    uint8_t length_field_in_bytes = 0;

    // Check if the first byte of modulus is a negative integer
    modulus_signed_bit_len = ((OPTIGA_EXAMPLE_UTIL_RSA_NEGATIVE_INTEGER < n_buffer[0]) ? 0x01 : 0x00);

    // Check the number of bytes required to represent the length field
    length_field_in_bytes = ((n_length > 0xFF)? 0x02 : 0x01);

    // RSA public key formation according to DER encoded format
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    // 7 bytes = Unused Bits(1 byte) + Sequence Tag (1 byte) +
    //           Length tag (1 byte) * 3 occurrence + Integer tag (1 byte) * 2 occurrence
    OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(pub_key_buffer, &index, (0x07 +
                                                   n_length +
                                                   modulus_signed_bit_len +
                                                   e_length +
                                                   (2 * length_field_in_bytes)))
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS;

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_SEQUENCE_TAG;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    // 4 bytes = Integer tag (1 byte) * 2 occurrence + Length tag (1 byte) * 2 occurrence
    OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(pub_key_buffer, &index, (0x04 +
                                                   n_length +
                                                   modulus_signed_bit_len +
                                                   e_length +
                                                   length_field_in_bytes))

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(pub_key_buffer, &index, (n_length  + modulus_signed_bit_len))
    index += modulus_signed_bit_len;
    pal_os_memcpy(&pub_key_buffer[index], n_buffer, n_length);
    index += n_length;

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG;
    pub_key_buffer[index++] = (uint8_t)e_length;
    pal_os_memcpy(&pub_key_buffer[index], e_buffer, e_length);
    index += e_length;

    *pub_key_length = index;

#undef OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD
#undef OPTIGA_EXAMPLE_UTIL_RSA_DER_SEQUENCE_TAG
#undef OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG
}

void example_util_encode_ecc_public_key_in_bit_string_format(const uint8_t * q_buffer,
                                                             uint8_t q_length,
                                                             uint8_t * pub_key_buffer,
                                                             uint16_t * pub_key_length)
{
#define OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH (0x02)

    uint16_t index = 0;

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG;
    pub_key_buffer[index++] = q_length + OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS;
    // Compression format. Supports only 04 [uncompressed]
    pub_key_buffer[index++] = 0x04;

    pal_os_memcpy(&pub_key_buffer[index], q_buffer, q_length);
    index += q_length;

    *pub_key_length = index;

#undef OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH
}
optiga_lib_status_t example_check_tag_in_metadata(const uint8_t * buffer, 
                                                  const uint8_t buffer_length, 
                                                  const uint8_t tag,
                                                  bool_t * tag_available)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    uint8_t offset = 1;
    uint8_t expected_buffer_length = buffer[offset++] + 0x02; // 0x02 is length for tag and length
    uint8_t tag_length = 0;
    do
    {
        *tag_available = FALSE;
        // Check if expected buffer length matches input buffer length
        if (buffer_length != expected_buffer_length)
        {
            break;
        }

        return_status = OPTIGA_LIB_SUCCESS;
        // Check for tag in the input buffer
        while(offset < (buffer_length - 0x02))
        {
            if (tag == buffer[offset++])
            {
                *tag_available = TRUE;                
                break;
            }
            tag_length = buffer[offset];
            offset += (tag_length + 1);
        }
    } while (FALSE);
    return (return_status);
}

void example_performance_measurement(uint32_t* time_value, uint8_t time_reset_flag)
{
    if(TRUE == time_reset_flag)
    {
        *time_value = pal_os_timer_get_time_in_milliseconds();
    }
    else if(FALSE == time_reset_flag)
    {
        *time_value = pal_os_timer_get_time_in_milliseconds() - *time_value;
    }
}

/**
* @}
*/
