/* cavium-nitrox.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */


#ifndef _CAVIUM_NITROX_H_
#define _CAVIUM_NITROX_H_

#ifdef HAVE_CAVIUM

#include <wolfssl/wolfcrypt/logging.h>

#ifndef HAVE_CAVIUM_V
    #include "cavium_sysdep.h"
#endif
#include "cavium_common.h"
#ifndef HAVE_CAVIUM_V
    #include "cavium_ioctl.h"
#else
    #include "cavium_sym_crypto.h"
    #include "cavium_asym_crypto.h"
#endif
#include <errno.h>

#define CAVIUM_SSL_GRP      0
#define CAVIUM_DPORT        256

/* Compatibility with older Cavium SDK's */
#ifndef HAVE_CAVIUM_V
    typedef int CspHandle;
    typedef word32 CavReqId;

    #define AES_128 AES_128_BIT
    #define AES_192 AES_192_BIT
    #define AES_256 AES_256_BIT
#else
    #define CAVIUM_DEV_ID       0
    #define CAVIUM_BLOCKING     BLOCKING
    #define CAVIUM_NON_BLOCKING NON_BLOCKING
    #define CAVIUM_DIRECT       DMA_DIRECT_DIRECT
    typedef Uint64 CavReqId;
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #define CAVIUM_REQ_MODE CAVIUM_NON_BLOCKING
#else
    #define CAVIUM_REQ_MODE CAVIUM_BLOCKING
#endif


#ifdef WOLFSSL_ASYNC_CRYPT
    #define CAVIUM_MAX_PENDING  90
    #define CAVIUM_MAX_POLL     MAX_TO_POLL
#endif


typedef struct CaviumNitroxDev {
    CspHandle   devId;                      /* nitrox device id */
    ContextType type;                       /* Typically CONTEXT_SSL, but also ECC types */
    Uint64      contextHandle;              /* nitrox context memory handle */
    CavReqId    reqId;                      /* Current requestId */
} CaviumNitroxDev;

struct WOLF_EVENT;


/* Wrapper API's */
WOLFSSL_LOCAL int NitroxTranslateResponseCode(int ret);
WOLFSSL_LOCAL CspHandle NitroxGetDeviceHandle(void);
WOLFSSL_LOCAL CspHandle NitroxOpenDevice(int dma_mode, int dev_id);
WOLFSSL_LOCAL int NitroxAllocContext(CaviumNitroxDev* nitrox, CspHandle devId,
    ContextType type);
WOLFSSL_LOCAL void NitroxFreeContext(CaviumNitroxDev* nitrox);
WOLFSSL_LOCAL void NitroxCloseDevice(CspHandle devId);

#if defined(WOLFSSL_ASYNC_CRYPT)
WOLFSSL_LOCAL int NitroxCheckRequest(CspHandle devId, CavReqId reqId);
WOLFSSL_LOCAL int NitroxCheckRequests(CspHandle devId,
    CspMultiRequestStatusBuffer* req_stat_buf);
#endif /* WOLFSSL_ASYNC_CRYPT */


/* Crypto wrappers */
#ifndef NO_RSA
    struct RsaKey;
    WOLFSSL_LOCAL int NitroxRsaExptMod(
                            const byte* in, word32 inLen,
                            byte* exponent, word32 expLen,
                            byte* modulus, word32 modLen,
                            byte* out, word32* outLen, struct RsaKey* key);
    WOLFSSL_LOCAL int NitroxRsaPublicEncrypt(const byte* in, word32 inLen,
                                byte* out, word32 outLen, struct RsaKey* key);
    WOLFSSL_LOCAL int NitroxRsaPrivateDecrypt(const byte* in, word32 inLen,
                                byte* out, word32 outLen, struct RsaKey* key);
    WOLFSSL_LOCAL int NitroxRsaSSL_Sign(const byte* in, word32 inLen,
                                byte* out, word32 outLen, struct RsaKey* key);
    WOLFSSL_LOCAL int NitroxRsaSSL_Verify(const byte* in, word32 inLen,
                                byte* out, word32 outLen, struct RsaKey* key);
#endif /* !NO_RSA */

#ifndef NO_AES
    struct Aes;
    WOLFSSL_LOCAL int NitroxAesSetKey(struct Aes* aes, const byte* key,
                                                word32 length, const byte* iv);
    #ifdef HAVE_AES_CBC
        WOLFSSL_LOCAL int NitroxAesCbcEncrypt(struct Aes* aes, byte* out,
                                                const byte* in, word32 length);
    #ifdef HAVE_AES_DECRYPT
        WOLFSSL_LOCAL int NitroxAesCbcDecrypt(struct Aes* aes, byte* out,
                                                const byte* in, word32 length);
    #endif /* HAVE_AES_DECRYPT */
    #endif /* HAVE_AES_CBC */
#endif /* !NO_AES */

#ifndef NO_RC4
    struct Arc4;
    WOLFSSL_LOCAL void NitroxArc4SetKey(struct Arc4* arc4, const byte* key,
                                                                word32 length);
    WOLFSSL_LOCAL void NitroxArc4Process(struct Arc4* arc4, byte* out,
                                                const byte* in, word32 length);
#endif /* !NO_RC4 */

#ifndef NO_DES3
    struct Des3;
    WOLFSSL_LOCAL int NitroxDes3SetKey(struct Des3* des3, const byte* key,
                                                               const byte* iv);
    WOLFSSL_LOCAL int NitroxDes3CbcEncrypt(struct Des3* des3, byte* out,
                                                const byte* in, word32 length);
    WOLFSSL_LOCAL int NitroxDes3CbcDecrypt(struct Des3* des3, byte* out,
                                                const byte* in, word32 length);
#endif /* !NO_DES3 */

#ifndef NO_HMAC
    struct Hmac;
    WOLFSSL_LOCAL int NitroxHmacFinal(struct Hmac* hmac, byte* hash);
    WOLFSSL_LOCAL int NitroxHmacUpdate(struct Hmac* hmac, const byte* msg,
                                                                word32 length);
    WOLFSSL_LOCAL int NitroxHmacSetKey(struct Hmac* hmac, int type,
                                               const byte* key, word32 length);
#endif /* NO_HMAC */

#if !defined(HAVE_HASHDRBG) && !defined(NO_RC4)
    WOLFSSL_API void NitroxRngGenerateBlock(WC_RNG* rng, byte* output, word32 sz);
#endif


#endif /* HAVE_CAVIUM */

#endif /* _CAVIUM_NITROX_H_ */
