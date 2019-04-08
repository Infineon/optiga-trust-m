/* hmac.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */




/*  hmac.h defines mini hamc openssl compatibility layer 
 *
 */


#ifndef WOLFSSL_HMAC_H_
#define WOLFSSL_HMAC_H_

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_PREFIX
#include "prefix_hmac.h"
#endif

#include <wolfssl/openssl/evp.h>
#include <wolfssl/wolfcrypt/hmac.h>

#ifdef __cplusplus
    extern "C" {
#endif


WOLFSSL_API unsigned char* wolfSSL_HMAC(const WOLFSSL_EVP_MD* evp_md,
                               const void* key, int key_len,
                               const unsigned char* d, int n, unsigned char* md,
                               unsigned int* md_len);


typedef struct WOLFSSL_HMAC_CTX {
    Hmac hmac;
    int  type;
} WOLFSSL_HMAC_CTX;


WOLFSSL_API void wolfSSL_HMAC_Init(WOLFSSL_HMAC_CTX* ctx, const void* key,
                                 int keylen, const EVP_MD* type);
WOLFSSL_API void wolfSSL_HMAC_Update(WOLFSSL_HMAC_CTX* ctx,
                                   const unsigned char* data, int len);
WOLFSSL_API void wolfSSL_HMAC_Final(WOLFSSL_HMAC_CTX* ctx, unsigned char* hash,
                                  unsigned int* len);
WOLFSSL_API void wolfSSL_HMAC_cleanup(WOLFSSL_HMAC_CTX* ctx);


typedef struct WOLFSSL_HMAC_CTX HMAC_CTX;

#define HMAC(a,b,c,d,e,f,g) wolfSSL_HMAC((a),(b),(c),(d),(e),(f),(g))

#define HMAC_Init    wolfSSL_HMAC_Init
#define HMAC_Update  wolfSSL_HMAC_Update
#define HMAC_Final   wolfSSL_HMAC_Final
#define HMAC_cleanup wolfSSL_HMAC_cleanup


#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* WOLFSSL_HMAC_H_ */
