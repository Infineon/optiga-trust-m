/* pkcs12.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */



#ifndef WOLF_CRYPT_PKCS12_H
#define WOLF_CRYPT_PKCS12_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef WOLFSSL_TYPES_DEFINED /* do not redeclare from ssl.h */
    typedef struct WC_PKCS12 WC_PKCS12;
#endif

typedef struct WC_DerCertList { /* dereferenced in ssl.c */
    byte* buffer;
    word32 bufferSz;
    struct WC_DerCertList* next;
} WC_DerCertList;



WOLFSSL_API WC_PKCS12* wc_PKCS12_new(void);
WOLFSSL_API void wc_PKCS12_free(WC_PKCS12* pkcs12);
WOLFSSL_API int wc_d2i_PKCS12(const byte* der, word32 derSz, WC_PKCS12* pkcs12);
WOLFSSL_API int wc_PKCS12_parse(WC_PKCS12* pkcs12, const char* psw,
        byte** pkey, word32* pkeySz, byte** cert, word32* certSz,
        WC_DerCertList** ca);

WOLFSSL_LOCAL int wc_PKCS12_SetHeap(WC_PKCS12* pkcs12, void* heap);
WOLFSSL_LOCAL void* wc_PKCS12_GetHeap(WC_PKCS12* pkcs12);


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_PKCS12_H */

