/* sha256.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */



/* code submitted by raphael.huck@efixo.com */

#ifndef WOLF_CRYPT_SHA256_H
#define WOLF_CRYPT_SHA256_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_SHA256

#ifdef HAVE_FIPS
    /* for fips @wc_fips */
    #include <cyassl/ctaocrypt/sha256.h>
#endif

#ifdef FREESCALE_LTC_SHA
    #include "fsl_ltc.h"
#endif


#ifdef __cplusplus
    extern "C" {
#endif

#ifndef HAVE_FIPS /* avoid redefinition of structs */
#ifdef WOLFSSL_PIC32MZ_HASH
    #include "port/pic32/pic32mz-crypt.h"
#endif

/* in bytes */
enum {
    SHA256              =  2,   /* hash type unique */
    SHA256_BLOCK_SIZE   = 64,
    SHA256_DIGEST_SIZE  = 32,
    SHA256_PAD_SIZE     = 56
};

#ifndef WOLFSSL_TI_HASH

/* Sha256 digest */
typedef struct Sha256 {
#ifdef FREESCALE_LTC_SHA
    ltc_hash_ctx_t ctx;
#else
    /* alignment on digest and buffer speeds up ARMv8 crypto operations */
    ALIGN16 word32  digest[SHA256_DIGEST_SIZE / sizeof(word32)];
    ALIGN16 word32  buffer[SHA256_BLOCK_SIZE  / sizeof(word32)];
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    #ifdef WOLFSSL_PIC32MZ_HASH
        pic32mz_desc desc ; /* Crypt Engine descriptor */
    #endif
#endif /* FREESCALE_LTC_SHA */
} Sha256;

#else /* WOLFSSL_TI_HASH */
    #include "wolfssl/wolfcrypt/port/ti/ti-hash.h"
#endif

#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha256(Sha256*);
WOLFSSL_API int wc_Sha256Update(Sha256*, const byte*, word32);
WOLFSSL_API int wc_Sha256Final(Sha256*, byte*);

#ifdef WOLFSSL_SHA224

#ifndef HAVE_FIPS /* avoid redefinition of structs */
/* in bytes */
enum {
    SHA224              =   8,   /* hash type unique */
    SHA224_BLOCK_SIZE   =   SHA256_BLOCK_SIZE,
    SHA224_DIGEST_SIZE  =   28,
    SHA224_PAD_SIZE     =   SHA256_PAD_SIZE
};

typedef Sha256 Sha224;
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha224(Sha224*);
WOLFSSL_API int wc_Sha224Update(Sha224*, const byte*, word32);
WOLFSSL_API int wc_Sha224Final(Sha224*, byte*);

#endif /* WOLFSSL_SHA224 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_SHA256 */
#endif /* WOLF_CRYPT_SHA256_H */

