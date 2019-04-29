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
* \file optiga_lib_config.h
*
* \brief   This file is defines the compilation switches to build code with required features.
*
* \ingroup grOptigaLibCommon
*
* @{
*/


#ifndef _OPTIGA_LIB_CONFIG_H_
#define _OPTIGA_LIB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OPTIGA_LIB_EXTERNAL
    #include "optiga_lib_config_external.h"   
#else
    /** @brief OPTIGA CRYPT random number generation feature enable/disable macro */
    #define OPTIGA_CRYPT_RANDOM_ENABLED
    /** @brief OPTIGA CRYPT hash feature enable/disable macro */
    #define OPTIGA_CRYPT_HASH_ENABLED
    /** @brief OPTIGA CRYPT ECC generate keypair feature enable/disable macro */
    #define OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED
    /** @brief OPTIGA CRYPT ECDSA signature feature enable/disable macro */
    #define OPTIGA_CRYPT_ECDSA_SIGN_ENABLED
    /** @brief OPTIGA CRYPT verify ECDSA signature feature enable/disable macro */
    #define OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
    /** @brief OPTIGA CRYPT ECDH feature enable/disable macro */
    #define OPTIGA_CRYPT_ECDH_ENABLED
    /** @brief OPTIGA CRYPT TLS prf sha256 feature enable/disable macro */
    #define OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED
    /** @brief OPTIGA CRYPT RSA generate keypair feature enable/disable macro */
    #define OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED
    /** @brief OPTIGA CRYPT RSA sign feature enable/disable macro */
    #define OPTIGA_CRYPT_RSA_SIGN_ENABLED
    /** @brief OPTIGA CRYPT RSA verify sign feature enable/disable macro */
    #define OPTIGA_CRYPT_RSA_VERIFY_ENABLED
    /** @brief OPTIGA CRYPT RSA Encrypt feature enable/disable macro */
    #define OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED
    /** @brief OPTIGA CRYPT RSA Decrypt feature enable/disable macro */
    #define OPTIGA_CRYPT_RSA_DECRYPT_ENABLED
    /** @brief OPTIGA CRYPT RSA pre master feature enable/disable macro */       
    #define OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED
    
    /** @brief NULL parameter check. To Enable, set 1 and to disable, set 0 */
    #define OPTIGA_LIB_DEBUG_NULL_CHECK                 (1U)
    /** @brief Maximum number of instance registration */
    #define OPTIGA_CMD_MAX_REGISTRATIONS                (0x06)
    /** @brief Maximum buffer size required to communicate with OPTIGA */
    #define OPTIGA_MAX_COMMS_BUFFER_SIZE                (0x615) //1557 in decimal

    /** @brief Macro to enable logger */
    #define OPTIGA_LIB_ENABLE_LOGGING                   (0x01)

    /** @brief OPTIGA COMMS shielded connection feature. 
     *         To disable the feature, undefine the macro
     * 
     */  
    //#define OPTIGA_COMMS_SHIELDED_CONNECTION

    /** @brief Default reset protection level for OPTIGA CRYPT and UTIL APIs */
    #define OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL           OPTIGA_COMMS_NO_PROTECTION
#endif

#ifdef __cplusplus
}
#endif

#endif /* _OPTIGA_LIB_CONFIG_H_*/

/**
* @}
*/
