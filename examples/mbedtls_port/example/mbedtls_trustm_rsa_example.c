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

#include "mbedtls/config.h"

#if defined(MBEDTLS_RSA_C)

#include "mbedtls/rsa.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/pk.h"
//#include "../include/trustm_rsa.h"

#include <string.h>
#include "optiga_crypt.h"
#include "optiga_util.h"
#include "optiga_lib_common.h"
#include "pal_os_memory.h"

#define TRUSTM_RSA_SIGNATURE_LEN_MAX_SIZE    (300)  
#define TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE       (300)
static const unsigned char rsa_1024_key_pair[] =
"-----BEGIN RSA PRIVATE KEY-----\n"
"MIICWwIBAAKBgQDGAQa8eT5T9FjSP2Xcw/uj5LZrq5/hdpEGG7XO10IfPy0wbwqP\n"
"j+omaCxJlAPXuxFy0cYFNQlangIu0HAJ/TMAZXPJLBSRwdK7X/aZn/Ds2vRNAcp5\n"
"av+Pym9cfnfgMoS+6CvbUMAduLhzrnh4tQv4lb/AkliomHuoczdbcWHvKwIDAQAB\n"
"AoGAXzxrIwgmBHeIqUe5FOBnDsOZQlyAQA+pXYjCf8Rll2XptFwUdkzAUMzWUGWT\n"
"G5ZspA9l8Wc7IozRe/bhjMxuVK5yZhPDKbjqRdWICA95Jd7fxlIirHOVMQRdzI7x\n"
"NKqMNQN05MLJfsEHUYtOLhZE+tfhJTJnnmB7TMwnJgc4O5ECQQD8oOJ45tyr46zc\n"
"OAt6ao7PefVLiW5Qu+PxfoHmZmDV2UQqeM5XtZg4O97VBSugOs3+quIdAC6LotYl\n"
"/6N+E4y3AkEAyKWD2JNCrAgtjk2bfF1HYt24tq8+q7x2ek3/cUhqwInkrZqOFoke\n"
"x3+yBB879TuUOadvBXndgMHHcJQKSAJlLQJAXRuGnHyptAhTe06EnHeNbtZKG67p\n"
"I4Q8PJMdmSb+ZZKP1v9zPUxGb+NQ+z3OmF1T8ppUf8/DV9+KAbM4NI1L/QJAdGBs\n"
"BKYFObrUkYE5+fwwd4uao3sponqBTZcH3jDemiZg2MCYQUHu9E+AdRuYrziLVJVk\n"
"s4xniVLb1tRG0lVxUQJASfjdGT81HDJSzTseigrM+JnBKPPrzpeEp0RbTP52Lm23\n"
"YARjLCwmPMMdAwYZsvqeTuHEDQcOHxLHWuyN/zgP2Ao=\n"
"-----END RSA PRIVATE KEY-----\n";

static const unsigned char rsa_2048_key_pair[] =
"-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEpAIBAAKCAQEAtM/RXjMp7AvPrnb1/i3ImcZ4ebkY+AvUurTXngJSBgn0GJNM\n"
"1HDRQqApE5JzUHf2BImsAyzW8QarrWzA2dWmq8rNWtJWJlHlSwiKr8wZDyU0kLAq\n"
"KUEPVfFrk9uds8zc7OvHVRjXQiXeSTUUMpKcHsZp4zz79Jr4+4vF4Bt+/U8luj/l\n"
"lleaJHlJFyfXiUtqLg2HUdkjPQaFVvhYMQ7ugZl4aM1uRH7J2oxaexy/JEApSNED\n"
"nO/cripd+Pdqx+m8xbBZ9pX8FsvYnO3D/BKQk3hadbRWg/r8QYT2ZHk0NRyseoUO\n"
"c3hyAeckiSWe2n9lvK+HkxmM23UVtuAwxwj4WQIDAQABAoIBAE76H0d4La2PEy3v\n"
"hE98DA0vJdx1PzTJZigPacb42H8OxfIeFQcOKDlj381OwNO7MliVEe9pHJG3CjH8\n"
"ONhtfBm5wa0UBtFCIFd/6aQUEDYPWECC0kemxV4Sz5yL5vxsVWufKThAW3XnOIrd\n"
"hm74nvzKSeIZ9yvGrU6ipNHY8MUPm0DQVrVYE5MiKjKVExQ4uRAolV2hlmeQDlSt\n"
"k85S0TUOWO1EvJZhsVVs7dBjjY10hIjv3gZPAO8CN85JzMeaNbmWv4RQj0B997in\n"
"rqlOa5qYYt80tAWO4hmPRKCrv6PgThz8C0Cd8AgwNzvQD2d4JpmxxTzBT6/5lRng\n"
"Hhj/wQECgYEA2jxC0a4lGmp1q2aYE1Zyiq0UqjxA92pwFYJg3800MLkf96A+dOhd\n"
"wDAc5aAKN8vQV5g33vKi5+pIHWUCskhTS8/PPGrfeqIvtphCj6b7LKosBOhdzrRD\n"
"Osr+Az/SiR2h5l2lr/v7I8I86RTY7MBk4QcRb601kSagWLDNVzSSdhECgYEA1Bm0\n"
"0sByqkQmFoUNRjwmShPfJeVLTCr1G4clljl6MqHmGyRDHxtcp1+CXlyJJemLQY2A\n"
"qrM7/T4x2ta6ME2WgDydFe9M8oU3BbefNYovS6YnoyBqxCx7yZ1vO0Jo40rZI8Bi\n"
"KoCi6e0Hugg4xyPRz9TTNLmr/yEC1qQesMhM9ckCgYEArsT7rfgMdq8zNOSgfTwJ\n"
"1sztc7d1P67ZvCABfLlVRn+6/hAydGVyTus4+RvFkxGB8+RPOhiOJbQVtJSkKCqL\n"
"qnbtu7DK7+ba1xvwkiJjnE1bm0KLfXIXNQpDik6eSHiWo2nzuo/Ne8GeDftIDbG2\n"
"GBAVAp5v+6I3X0+X4nKTqEECgYEAwT4Cj5mjXxnkEdR7eahHwmpEf0RfzC+/Tate\n"
"RXZsrUDwY34wYWEOk7fjEZIBqrcTl1ATEHNojpxh096bmHK4UnHnNRrn4nYY4W6g\n"
"8ajK2oOxzWA1pjJZPiHgO/+PjLafC4G2br7wr2y0A3yGLnmmKVLgc0NPP42WBnVV\n"
"OP/ljnECgYABlDdJCAehDNSv4mdEzY5bfD+VBFd2QsgE1hYhmUYYRNlgIfIL9Y8e\n"
"CduqXFLNZ/LHdmtYembgUqrMiJTUqcbSrJt26kBQx0az3LAV+J2p68PQ85KR9ZPy\n"
"N1jEnRqpAwEdw7S+8l0yVyaNkm66eRI80p+w3AxNbS9hJ/7UlV3lGAo=\n"
"-----END RSA PRIVATE KEY-----\n";
static const uint8_t digest_sha256[] = 
{
    0x91, 0x70, 0x02, 0x48, 0x3F, 0xBD, 0x5F, 0xDD, 0xD5, 0x38, 0xEB, 0xDA, 0x9A, 0x5E, 0x1F, 0x46,
    0xFC, 0xAD, 0x8F, 0x1E, 0x2C, 0x75, 0xB0, 0x83, 0xD0, 0x71, 0x2B, 0x80, 0xD4, 0xAA, 0xC6, 0x9B
};
static const uint8_t digest_sha384[] = 
{
    0x0C, 0x51, 0xDE, 0xBB, 0x3E, 0xE7, 0x88, 0x1C, 0xC8, 0x07, 0xA0, 0x5E, 0x61, 0xE3, 0x4D, 0x71,
    0xC1, 0x28, 0x78, 0x8D, 0xF2, 0xBD, 0x7E, 0xAF, 0xA4, 0xBC, 0x50, 0xAB, 0x91, 0x60, 0xD9, 0x5C,
    0x13, 0xB4, 0x10, 0xAC, 0x74, 0x10, 0x38, 0x12, 0xC1, 0x59, 0xDB, 0x8C, 0x26, 0x9A, 0x1D, 0x3A
};

static const uint8_t signature_sha256_rsa1024[] =
{
    0x74, 0x9C, 0xE3, 0xB9, 0xCB, 0xDF, 0x57, 0x15, 0xA7, 0x96, 0x0B, 0xB5, 0xD9, 0xB6, 0x13, 0xDA,
    0xC2, 0xD5, 0xDE, 0xE8, 0x6D, 0xB1, 0xB4, 0x5A, 0x0A, 0x83, 0xC5, 0xA9, 0xF9, 0x9E, 0xD1, 0xB7,
    0xD0, 0xA0, 0xDA, 0x7C, 0x70, 0x11, 0x2B, 0xCF, 0x90, 0x76, 0xEF, 0x8E, 0xC8, 0xF5, 0xC0, 0xB0,
    0xCB, 0xE4, 0x0D, 0xD3, 0x46, 0xEF, 0x37, 0x13, 0xC4, 0xA5, 0xBB, 0x7D, 0x48, 0x69, 0x6E, 0xC3,
    0xD4, 0xE7, 0x18, 0x1A, 0x63, 0xFB, 0xD8, 0x69, 0x29, 0x4D, 0xD1, 0x29, 0xE1, 0x53, 0xC1, 0x53,
    0x11, 0x43, 0xF9, 0xD2, 0x48, 0xD3, 0x23, 0xC9, 0xFA, 0x93, 0xEB, 0x28, 0xFD, 0x18, 0x67, 0x82,
    0xD9, 0x41, 0x8B, 0x84, 0x8B, 0xE2, 0x40, 0x68, 0xF4, 0xAA, 0x08, 0xCD, 0x0B, 0x0C, 0x13, 0xE3,
    0xEA, 0x0A, 0x0A, 0x2E, 0x54, 0x8D, 0x32, 0xF2, 0xC3, 0x7A, 0x89, 0x6B, 0x7B, 0x4E, 0xCC, 0xE4,
};

static const uint8_t signature_sha384_rsa2048[] =
{
    0x07, 0x56, 0xB7, 0xE5, 0x7C, 0xEE, 0xAE, 0x1A, 0xFE, 0xAF, 0x57, 0x79, 0x71, 0xDE, 0x99, 0x59,
    0x09, 0xF0, 0x6A, 0x33, 0x76, 0xB0, 0x47, 0x7A, 0x2E, 0x5E, 0xE6, 0x62, 0xAD, 0x87, 0x5A, 0x10,
    0xB7, 0x5E, 0xB7, 0x2C, 0xB0, 0x82, 0xB9, 0xC3, 0xBD, 0x64, 0xA6, 0xAE, 0x8E, 0x60, 0xC3, 0xFD,
    0x5C, 0x3F, 0x32, 0xE0, 0x0C, 0xE1, 0x20, 0x73, 0x58, 0x03, 0x9E, 0xD7, 0x37, 0x53, 0x96, 0xD7,
    0x0B, 0x6C, 0xA0, 0xDE, 0xB4, 0x9F, 0x79, 0x7B, 0xA1, 0x87, 0x1F, 0x67, 0xC4, 0x42, 0x10, 0xEC,
    0xAF, 0x0E, 0x81, 0xEB, 0xCE, 0x0A, 0x46, 0xFB, 0x16, 0xB0, 0xCE, 0x40, 0xC9, 0x72, 0x11, 0xF3,
    0x9A, 0xCA, 0x1B, 0x79, 0xDF, 0x97, 0x5C, 0x58, 0x7E, 0xEB, 0x3A, 0x55, 0xF8, 0xB3, 0x72, 0x82,
    0xDD, 0xF7, 0xEC, 0xDC, 0x3E, 0x82, 0xA7, 0x98, 0xBD, 0xB6, 0xB4, 0x11, 0xA1, 0x3D, 0x0C, 0x77,
    0x4E, 0xB9, 0xFB, 0xA6, 0x23, 0x7F, 0x02, 0xF6, 0x7F, 0x19, 0xEE, 0xED, 0x57, 0x02, 0xD7, 0x62,
    0xBF, 0xC0, 0x1E, 0xEF, 0xCF, 0xB6, 0x16, 0x4E, 0x1F, 0x4B, 0x36, 0x65, 0x26, 0x45, 0x43, 0x00,
    0x8B, 0xA3, 0x35, 0x25, 0xB2, 0x24, 0xA2, 0xEA, 0xEB, 0xE7, 0xFA, 0x42, 0x7E, 0x09, 0x27, 0xC8,
    0x37, 0x5E, 0x92, 0x7E, 0xD2, 0x56, 0xF8, 0x1F, 0xBE, 0x51, 0x62, 0xCF, 0x7F, 0xE1, 0xD0, 0xB2,
    0x4F, 0x2A, 0xAF, 0x34, 0xCB, 0xC0, 0x7C, 0x51, 0x3B, 0x69, 0xC7, 0xE4, 0x85, 0x4D, 0x40, 0x2E,
    0x12, 0xCD, 0x3C, 0x87, 0x3B, 0x47, 0x34, 0x08, 0xD6, 0xBF, 0x12, 0x54, 0x27, 0x28, 0xAE, 0x05,
    0x42, 0xEF, 0xEE, 0x97, 0x30, 0xC1, 0x38, 0xB0, 0x93, 0xF1, 0x95, 0x95, 0x5E, 0xB8, 0x0F, 0x7E,
    0xDE, 0xBE, 0x34, 0xDE, 0xCE, 0x65, 0x5E, 0x2D, 0x4D, 0xFF, 0x20, 0x2D, 0x7B, 0x3A, 0x75, 0xA2,
};

static const uint8_t signature_sha384_rsa1024[] =
{
    0x9D, 0x93, 0x7E, 0xE8, 0x9B, 0xC1, 0xBB, 0x52, 0x02, 0x77, 0x24, 0xA7, 0x39, 0x82, 0x7F, 0x90,
    0xFC, 0x17, 0x71, 0x1F, 0x38, 0xA9, 0x8D, 0x9D, 0x01, 0x05, 0x42, 0xE6, 0xE5, 0xDE, 0xCB, 0x25,
    0x41, 0x79, 0xF5, 0xAC, 0x1E, 0xE2, 0x84, 0x53, 0x60, 0xE1, 0x5F, 0x31, 0xF2, 0xA0, 0x59, 0x11,
    0x57, 0xA8, 0xA6, 0x0C, 0xA1, 0xB0, 0xEC, 0xF5, 0x68, 0xCD, 0x24, 0x03, 0xC6, 0x7A, 0xA3, 0xD9,
    0x25, 0x38, 0x13, 0x49, 0x82, 0xF7, 0x45, 0x1E, 0x45, 0xE1, 0x58, 0x19, 0x37, 0xE4, 0x58, 0xD0,
    0xEA, 0xCA, 0x91, 0xAB, 0x9D, 0xDB, 0xDF, 0x3B, 0x0F, 0x80, 0xE1, 0xF7, 0x8D, 0xFE, 0x4B, 0x89,
    0x53, 0x0F, 0x75, 0xF9, 0x2B, 0x3D, 0xBD, 0x04, 0xD6, 0x54, 0x18, 0x64, 0x01, 0xB6, 0xE0, 0x84,
    0x33, 0x58, 0x9F, 0x32, 0x63, 0x06, 0x3B, 0x53, 0x97, 0xA9, 0xF2, 0x70, 0xA0, 0xB7, 0xF4, 0xC0,
};

static const uint8_t signature_sha256_rsa2048[] =
{
    0x12, 0x21, 0x02, 0x0D, 0x77, 0x9F, 0x07, 0x68, 0x7A, 0x4C, 0x8D, 0xCD, 0x3D, 0x97, 0xE5, 0x11,
    0xA9, 0x27, 0x81, 0x30, 0x91, 0x62, 0xC3, 0x88, 0x7D, 0x73, 0x27, 0xC6, 0xD4, 0x7C, 0x96, 0xC6,
    0x99, 0xAA, 0xA7, 0x90, 0x6B, 0xD6, 0x98, 0xAC, 0x65, 0x63, 0x73, 0xA7, 0x5E, 0x17, 0x59, 0x1C,
    0x82, 0x29, 0x8F, 0x37, 0x6F, 0x58, 0x38, 0x2C, 0x6E, 0x6A, 0x29, 0xDC, 0x0B, 0xCC, 0x61, 0xFA,
    0x74, 0x58, 0xEE, 0xBD, 0x5C, 0x79, 0xCB, 0xF3, 0x2B, 0xB6, 0x1B, 0xA2, 0x9E, 0x14, 0xE9, 0xAF,
    0xA8, 0x31, 0xAE, 0xFC, 0x17, 0xD9, 0x65, 0x5D, 0xB6, 0x68, 0x19, 0xE3, 0x7F, 0xA3, 0x68, 0x80,
    0x24, 0xB6, 0x1B, 0x41, 0xEA, 0x6F, 0xB7, 0x21, 0x6F, 0x61, 0x73, 0xFA, 0xE2, 0x41, 0x15, 0xC2,
    0x51, 0x79, 0xE2, 0xA9, 0x85, 0x43, 0xCC, 0x4A, 0x9E, 0x69, 0x7F, 0xEE, 0xA6, 0x28, 0x1A, 0x34,
    0x04, 0x64, 0x10, 0x09, 0xE6, 0x8A, 0x5F, 0x53, 0xBB, 0xF6, 0xD4, 0x12, 0xB7, 0xDB, 0x72, 0xE6,
    0x92, 0x55, 0xC0, 0x47, 0x19, 0xAE, 0xF5, 0x7F, 0x3F, 0x63, 0x4C, 0x41, 0xD7, 0xEC, 0x04, 0x12,
    0xE5, 0x9A, 0xA4, 0x3D, 0x39, 0xFF, 0xA4, 0x85, 0x6F, 0x20, 0x1E, 0x65, 0x64, 0x95, 0x92, 0x5B,
    0x47, 0x66, 0xF5, 0x6F, 0xBF, 0x5D, 0x10, 0xEE, 0xE4, 0xE1, 0x91, 0xF0, 0xCC, 0x8A, 0x5F, 0xFB,
    0x45, 0x23, 0xB0, 0x57, 0xE3, 0x78, 0xFB, 0x5C, 0x35, 0xB4, 0x6B, 0x29, 0x3B, 0x66, 0x45, 0x4E,
    0x53, 0xA4, 0xDB, 0x79, 0xE3, 0x64, 0x1C, 0x85, 0x86, 0xC6, 0x79, 0xAF, 0x7C, 0x24, 0xB7, 0x8E,
    0x56, 0x1C, 0xD9, 0xF6, 0x74, 0xE4, 0x91, 0x64, 0xB1, 0x3C, 0x83, 0x8E, 0x2D, 0xC1, 0x27, 0x41,
    0xA1, 0x6F, 0xCA, 0xD4, 0x2F, 0x09, 0x8A, 0x49, 0x68, 0x08, 0x38, 0xB5, 0xE1, 0xFC, 0x92, 0xAA,
};

static volatile optiga_lib_status_t crypt_event_completed_status;

static optiga_lib_status_t optiga_generate_rsa_keypair(optiga_crypt_t * me,
                                                       optiga_rsa_key_type_t key_type,
                                                       uint8_t key_usage,
                                                       void * private_key,
                                                       uint8_t * public_key,
                                                       uint16_t * public_key_length)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    do
    {
        crypt_event_completed_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_rsa_generate_keypair(me,
                                                          key_type,
                                                          key_usage,
                                                          FALSE,
                                                          private_key,
                                                          public_key,
                                                          public_key_length);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        
        while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
        {
            //Wait until the optiga_crypt_rsa_generate_keypair operation is completed
        }
        
        if (OPTIGA_LIB_SUCCESS != crypt_event_completed_status)
        {
            //RSA Key pair generation failed
            return_status = crypt_event_completed_status;
            break;
        }    
        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    return (return_status);    
    
}

static optiga_lib_status_t optiga_rsa_verify(optiga_crypt_t * me,
                                             optiga_rsa_signature_scheme_t signature_scheme,
                                             const uint8_t * digest,
                                             uint8_t digest_length,
                                             const uint8_t * signature,
                                             uint16_t signature_length,
                                             const void * public_key)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    do
    {
        crypt_event_completed_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_rsa_verify(me,
                                                signature_scheme,
                                                digest,
                                                digest_length,
                                                signature,
                                                signature_length,
                                                OPTIGA_CRYPT_HOST_DATA,
                                                public_key,
                                                0);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        
        while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
        {
            //Wait until the optiga_crypt_rsa_generate_keypair operation is completed
        }
        
        if (OPTIGA_LIB_SUCCESS != crypt_event_completed_status)
        {
            //RSA Key pair generation failed
            return_status = crypt_event_completed_status;
            break;
        }    
        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    return (return_status);
}

//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_crypt_callback(void * context, optiga_lib_status_t return_status)
{
    crypt_event_completed_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

int example_mbedtls_optiga_crypt_rsa_verify()
{
    int return_status = 0;
    uint8_t loop_count = 0;
    mbedtls_pk_context ctx;
    mbedtls_rsa_context * rsa_ctx;
    mbedtls_md_type_t hash_type[] = {MBEDTLS_MD_SHA256, 
                                     MBEDTLS_MD_SHA384, 
                                     MBEDTLS_MD_SHA256, 
                                     MBEDTLS_MD_SHA384};
    const uint8_t * digest_buffer[] = {&digest_sha256[0], 
                                       &digest_sha384[0], 
                                       &digest_sha256[0], 
                                       &digest_sha384[0]};
    const uint8_t * signature_buffer[] = {&signature_sha256_rsa1024[0], 
                                          &signature_sha384_rsa1024[0], 
                                          &signature_sha256_rsa2048[0], 
                                          &signature_sha384_rsa2048[0]};
    const unsigned char * rsa_keypair[] = {&rsa_1024_key_pair[0],
                                           &rsa_1024_key_pair[0], 
                                           &rsa_2048_key_pair[0], 
                                           &rsa_2048_key_pair[0]};
    size_t keypair_size[] = {sizeof(rsa_1024_key_pair), 
                             sizeof(rsa_1024_key_pair), 
                             sizeof(rsa_2048_key_pair), 
                             sizeof(rsa_2048_key_pair)};
    
    
    for (loop_count = 0; loop_count < sizeof(hash_type)/sizeof(mbedtls_md_type_t); loop_count++)
    {
        mbedtls_pk_init(&ctx);
        do
        {
            if (0 != mbedtls_pk_parse_key(&ctx, rsa_keypair[loop_count], keypair_size[loop_count], NULL, 0))
            {
                return_status = -1;
                break;
            }
            rsa_ctx = (mbedtls_rsa_context *)ctx.pk_ctx;
    
            return_status |= mbedtls_rsa_rsassa_pkcs1_v15_verify(rsa_ctx,
                                                                 NULL,
                                                                 NULL,
                                                                 MBEDTLS_RSA_PUBLIC,
                                                                 hash_type[loop_count],
                                                                 0,
                                                                 digest_buffer[loop_count],
                                                                 signature_buffer[loop_count]);
        } while (0);
        if (0 != return_status)
        {
            break;
        }
        mbedtls_pk_free(&ctx);
    }
    return(return_status);
}


int example_mbedtls_optiga_crypt_rsa_sign()
{
    int return_status = 0;
    uint8_t loop_count = 0;
    optiga_crypt_t * me_crypt = NULL;
    //mbedtls_pk_context ctx;
    mbedtls_rsa_context rsa_ctx;
    uint8_t * signature = NULL;
    uint8_t * rsa_public_key = NULL;
    uint16_t rsa_key_length[] = {128,128,256,256};
    optiga_key_id_t optiga_key_id;
    uint16_t public_key_length = 0;
    public_key_from_host_t public_key;
    optiga_rsa_signature_scheme_t signature_scheme;
    mbedtls_md_type_t hash_type[] = {MBEDTLS_MD_SHA256,
                                     MBEDTLS_MD_SHA384,
                                     MBEDTLS_MD_SHA256,
                                     MBEDTLS_MD_SHA384};
                                     
    optiga_rsa_key_type_t rsa_key_type[] = {OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
                                            OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
                                            OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL,
                                            OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL};
                                     
    const uint8_t * digest[] = {&digest_sha256[0],
                                &digest_sha384[0],
                                &digest_sha256[0],
                                &digest_sha384[0]};
                                
    uint8_t digest_len[] =  {sizeof(digest_sha256),
                             sizeof(digest_sha384),
                             sizeof(digest_sha256),
                             sizeof(digest_sha384)};                                
                                   

    signature = pal_os_calloc(1,TRUSTM_RSA_SIGNATURE_LEN_MAX_SIZE);
    rsa_public_key = pal_os_calloc(1,TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE);
    
    mbedtls_rsa_init(&rsa_ctx, MBEDTLS_RSA_PKCS_V15, 0);
    me_crypt = optiga_crypt_create(0, optiga_crypt_callback, NULL);
    
    do
    { 
        if (NULL == me_crypt)
        {
            break;
        }    
        for (loop_count = 0; loop_count < sizeof(hash_type)/sizeof(mbedtls_md_type_t); loop_count++)
        {
            pal_os_memset(signature, 0x00, TRUSTM_RSA_SIGNATURE_LEN_MAX_SIZE);
            public_key_length = TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE;
            do
            {
                if (MBEDTLS_MD_SHA256 == hash_type[loop_count])
                {
                    signature_scheme = OPTIGA_RSASSA_PKCS1_V15_SHA256;
                }
                else
                {
                    signature_scheme = OPTIGA_RSASSA_PKCS1_V15_SHA384;
                }
        
                optiga_key_id = OPTIGA_KEY_ID_E0FC;
                return_status = optiga_generate_rsa_keypair(me_crypt,
                                                            rsa_key_type[loop_count],
                                                            (uint8_t)OPTIGA_KEY_USAGE_SIGN,
                                                            &optiga_key_id,
                                                            rsa_public_key,
                                                            &public_key_length);
                if (OPTIGA_LIB_SUCCESS != return_status)
                {
                    break;
                }          
                
                rsa_ctx.len = rsa_key_length[loop_count];
                return_status |= mbedtls_rsa_rsassa_pkcs1_v15_sign(&rsa_ctx,
                                                                   NULL,
                                                                   NULL,
                                                                   MBEDTLS_RSA_PRIVATE,
                                                                   hash_type[loop_count],
                                                                   0,
                                                                   digest[loop_count],
                                                                   signature);
                                                                   
                if (0 != return_status)
                {
                    break;
                }        
                            
                public_key.public_key = rsa_public_key;
                public_key.key_type = rsa_key_type[loop_count];
                public_key.length = public_key_length;
        
                return_status = optiga_rsa_verify(me_crypt,
                                                  signature_scheme,
                                                  digest[loop_count],
                                                  digest_len[loop_count],
                                                  signature,
                                                  rsa_ctx.len,
                                                  &public_key);
                if (OPTIGA_LIB_SUCCESS != return_status)
                {
                    break;
                }
                return_status = OPTIGA_LIB_SUCCESS;                                             
            } while (0);
            
            if (0 != return_status)
            {
                break;
            }
        }
    } while (FALSE);
    
    mbedtls_rsa_free(&rsa_ctx);
    pal_os_free(signature);
    pal_os_free(rsa_public_key);
    // destroy crypt instances
    if (me_crypt != NULL)
    {
        (void)optiga_crypt_destroy(me_crypt);
    }   
    return(return_status);
}

int example_mbedtls_optiga_crypt_rsa_decrypt()
{
    int return_status = 0;
    uint8_t loop_count = 0;
    optiga_crypt_t * me_crypt = NULL;
    //mbedtls_pk_context ctx;
    mbedtls_rsa_context rsa_ctx;
    uint8_t * rsa_public_key = NULL;
    uint16_t rsa_key_length[] = {128,256};
    optiga_key_id_t optiga_key_id;
    uint16_t public_key_length = 0;
    public_key_from_host_t public_key;
    uint8_t encrypted_message[256];
    size_t encrypted_message_length[] = {128,256};
    uint8_t rsa_plaintext[] = {"RSA PKCS1_v1.5 Encryption of user message"};
    uint8_t decrypted_message[256];
    uint16_t  decrypted_message_length = sizeof(decrypted_message);

    optiga_rsa_key_type_t rsa_key_type[] = {OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
                                            OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL};


    rsa_public_key = pal_os_calloc(1,TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE);

    mbedtls_rsa_init(&rsa_ctx, MBEDTLS_RSA_PKCS_V15, 0);
	for (loop_count = 0; loop_count < sizeof(rsa_key_type)/sizeof(optiga_rsa_key_type_t); loop_count++)
	{
	    public_key_length = TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE;
		//mbedtls_pk_init(&ctx);
		do
		{
            me_crypt = optiga_crypt_create(0, optiga_crypt_callback, NULL);
            if (NULL == me_crypt)
            {
                break;
            }

            optiga_key_id = OPTIGA_KEY_ID_E0FC;
            return_status = optiga_generate_rsa_keypair(me_crypt,
                                                        rsa_key_type[loop_count],
                                                        (uint8_t)OPTIGA_KEY_USAGE_ENCRYPTION,
                                                        &optiga_key_id,
														rsa_public_key,
                                                        &public_key_length);
            if (OPTIGA_LIB_SUCCESS != return_status)
            {
                break;
            }

            public_key.public_key = rsa_public_key;
            public_key.length = public_key_length;
            public_key.key_type = (uint8_t)rsa_key_type[loop_count];
            crypt_event_completed_status = OPTIGA_LIB_BUSY;
            return_status = optiga_crypt_rsa_encrypt_message(me_crypt,
            												 OPTIGA_RSAES_PKCS1_V15,
															 rsa_plaintext,
															 sizeof(rsa_plaintext),
															 NULL,
															 0,
															 OPTIGA_CRYPT_HOST_DATA,
															 &public_key,
															 encrypted_message,
															 (uint16_t *)&encrypted_message_length[loop_count]);
            if (OPTIGA_LIB_SUCCESS != return_status)
            {
                break;
            }

            while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
            {
                //Wait until the optiga_crypt_rsa_generate_keypair operation is completed
            }

            if (OPTIGA_LIB_SUCCESS != crypt_event_completed_status)
            {
                //RSA Key pair generation failed
                return_status = crypt_event_completed_status;
                break;
            }

			//rsa_ctx = (mbedtls_rsa_context *)ctx.pk_ctx;
			rsa_ctx.len = rsa_key_length[loop_count];
			return_status |= mbedtls_rsa_rsaes_pkcs1_v15_decrypt(&rsa_ctx,
															     NULL,
															     NULL,
															     MBEDTLS_RSA_PRIVATE,
															     &encrypted_message_length[loop_count],
															     encrypted_message,
															     decrypted_message,
															     decrypted_message_length);

		    if (0 != return_status)
		    {
		    	break;
		    }

		    if (memcmp(rsa_plaintext,decrypted_message,sizeof(rsa_plaintext)))
		    {
		    	break;
		    }

            return_status = OPTIGA_LIB_SUCCESS;
		} while (0);

		if (0 != return_status)
		{
			break;
		}
	    if (me_crypt != NULL)
	    {
	        (void)optiga_crypt_destroy(me_crypt);
	    }
	}
	mbedtls_rsa_free(&rsa_ctx);
	pal_os_free(rsa_public_key);
    // destroy crypt instances

    return(return_status);
}

int example_mbedtls_optiga_crypt_rsa_encrypt()
{
    int return_status = -1;
    uint8_t loop_count = 0;
    uint16_t public_key_modulus_length = 0;
    uint16_t public_key_exponent_length = 0;
    uint8_t public_key_modulus_exponent_buffer[300] = {0x00};

    mbedtls_rsa_context rsa_ctx;
    mbedtls_mpi N, E;
    mbedtls_asn1_bitstring bs;
    optiga_crypt_t * me_crypt = NULL;
    uint8_t * rsa_public_key = NULL;
    uint8_t rsa_public_key_array[300] = {0x00};
    uint8_t * buffer_pointer = NULL;

    optiga_key_id_t optiga_key_id;
    uint16_t public_key_length = 0;
    uint8_t encrypted_message[256];
    uint16_t encrypted_message_length[] = {128,256};
    uint8_t rsa_plaintext[] = {0x77, 0x65, 0x6C, 0x63, 0x6F, 0x6D, 0x65, 0x20, 0x74, 0x6F, 0x20, 0x69, 0x6E, 0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E};
    uint8_t decrypted_message[256];
    uint16_t  decrypted_message_length = sizeof(decrypted_message);
    optiga_rsa_key_type_t rsa_key_type[] = {OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL, OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL};

    mbedtls_rsa_init(&rsa_ctx, MBEDTLS_RSA_PKCS_V15, 0);
    mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &E );

    for (loop_count = 0; loop_count < sizeof(rsa_key_type)/sizeof(optiga_rsa_key_type_t); loop_count++)
    {
	    public_key_length = TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE;

	    if (me_crypt == NULL)
	    {
			me_crypt = optiga_crypt_create(0, optiga_crypt_callback, NULL);
			if (NULL == me_crypt)
			{
				break;
			}
	    }

		optiga_key_id = OPTIGA_KEY_ID_E0FC;
		return_status = optiga_generate_rsa_keypair(me_crypt,
													rsa_key_type[loop_count],
													(uint8_t)OPTIGA_KEY_USAGE_ENCRYPTION,
													&optiga_key_id,
													rsa_public_key_array,
													&public_key_length);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			return_status = -1;
			break;
		}
		rsa_public_key = rsa_public_key_array;
		memset(encrypted_message, 0x00, sizeof(encrypted_message));

		// Extract the N and E component
		bs.len = public_key_length;
		bs.unused_bits = 0;
		bs.p = &public_key_modulus_exponent_buffer[0];
		if(0 != mbedtls_asn1_get_bitstring(&rsa_public_key, rsa_public_key+public_key_length, &bs))
		{
			return_status = -1;
			break;
		}
		buffer_pointer = bs.p;
		buffer_pointer++;

		if( 0x01 == (*buffer_pointer & 0x7F) )
		{
			buffer_pointer = buffer_pointer + 4;
			public_key_modulus_length = *buffer_pointer;
			buffer_pointer = buffer_pointer - 2;
			if(0 != mbedtls_asn1_get_mpi( &buffer_pointer, buffer_pointer+public_key_modulus_length+3, &N))
			{
				return_status = -1;
				break;
			}
			buffer_pointer++;
			public_key_exponent_length = *buffer_pointer;
			buffer_pointer--;
			if(0 != mbedtls_asn1_get_mpi( &buffer_pointer, buffer_pointer+public_key_exponent_length+2, &E))
			{
				return_status = -1;
				break;
			}
			if(0 != mbedtls_rsa_import( &rsa_ctx, &N, NULL, NULL, NULL, &E ))
			{
				return_status = -1;
				break;
			}
		}
		else if( 0x02 == (*buffer_pointer & 0x7F) )
		{
			buffer_pointer = buffer_pointer + 5;
			public_key_modulus_length = ((uint16_t )*buffer_pointer << 8);
			buffer_pointer++;
			public_key_modulus_length |= *buffer_pointer;
			buffer_pointer = buffer_pointer - 3;
			if(0 != mbedtls_asn1_get_mpi( &buffer_pointer, buffer_pointer+public_key_modulus_length+4, &N))
			{
				return_status = -1;
				break;
			}
			buffer_pointer++;
			public_key_exponent_length = *buffer_pointer;
			buffer_pointer--;
			if(0 != mbedtls_asn1_get_mpi( &buffer_pointer, buffer_pointer+public_key_exponent_length+2, &E))
			{
				return_status = -1;
				break;
			}
			if(0 != mbedtls_rsa_import( &rsa_ctx, &N, NULL, NULL, NULL, &E ))
			{
				return_status = -1;
				break;
			}
		}

		return_status = mbedtls_rsa_rsaes_pkcs1_v15_encrypt(&rsa_ctx,
															 NULL,
															 NULL,
															 MBEDTLS_RSA_PUBLIC,
															 sizeof(rsa_plaintext),
															 rsa_plaintext,
															 encrypted_message);
		if (0 != return_status)
		{
			return_status = -1;
			break;
		}
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(me_crypt, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me_crypt, OPTIGA_COMMS_RESPONSE_PROTECTION);

		crypt_event_completed_status = OPTIGA_LIB_BUSY;
		memset(decrypted_message, 0x00, sizeof(decrypted_message));
		return_status = optiga_crypt_rsa_decrypt_and_export(me_crypt,
															OPTIGA_RSAES_PKCS1_V15,
															encrypted_message,
															encrypted_message_length[loop_count],
															NULL,
															0,
															optiga_key_id,
															decrypted_message,
															&decrypted_message_length);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			return_status = -1;
			break;
		}

		while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
		{
			//Wait until the optiga_crypt_rsa_decrypt_and_export operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != crypt_event_completed_status)
		{
			//RSA decrypt failed
			return_status = crypt_event_completed_status;
			break;
		}

		if (0 != memcmp(rsa_plaintext,decrypted_message,sizeof(rsa_plaintext)))
		{
			return_status = -1;
			break;
		}

    }

    if (me_crypt != NULL)
    {
        (void)optiga_crypt_destroy(me_crypt);
    }
	mbedtls_rsa_free(&rsa_ctx);
    mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &E );

    return(return_status);
}

#endif /* MBEDTLS_PKCS1_V15 */
/**
* @}
*/
