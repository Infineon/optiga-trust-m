/* port/ti/ti-hash.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */


 
#ifndef WOLF_CRYPT_TI_HASH_H
#define WOLF_CRYPT_TI_HASH_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef WOLFSSL_TI_INITBUFF
#define WOLFSSL_TI_INITBUFF 64
#endif

#define WOLFSSL_MAX_HASH_SIZE  64

typedef struct {
    byte   *msg ;
    word32 used ;
    word32 len ;
    byte hash[WOLFSSL_MAX_HASH_SIZE] ;
} wolfssl_TI_Hash ;


#ifndef TI_HASH_TEST
#if !defined(NO_MD5)
typedef wolfssl_TI_Hash Md5 ;

#endif
#if !defined(NO_SHA)
typedef wolfssl_TI_Hash Sha ;
#endif
#if !defined(NO_SHA256)
typedef wolfssl_TI_Hash Sha256 ;
#endif

#if defined(HAVE_SHA224)
typedef wolfssl_TI_Hash Sha224 ;
#define SHA224_DIGEST_SIZE  28

WOLFSSL_API int wc_InitSha224(Sha224* sha224) ;
WOLFSSL_API int wc_Sha224Update(Sha224* sha224, const byte* data, word32 len) ;
WOLFSSL_API int wc_Sha224Final(Sha224* sha224, byte* hash) ;
WOLFSSL_API int wc_Sha224Hash(const byte* data, word32 len, byte*hash) ;

#endif
#endif
#endif /* WOLF_CRYPT_TI_HASH_H  */
