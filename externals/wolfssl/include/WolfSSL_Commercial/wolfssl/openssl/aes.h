/* aes.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */




/*  aes.h defines mini des openssl compatibility layer
 *
 */


#ifndef WOLFSSL_AES_H_
#define WOLFSSL_AES_H_

#include <wolfssl/wolfcrypt/settings.h>

#ifndef NO_AES
#ifdef WOLFSSL_AES_DIRECT

#ifdef __cplusplus
    extern "C" {
#endif


typedef Aes AES_KEY;

WOLFSSL_API void wolfSSL_AES_set_encrypt_key
    (const unsigned char *, const int bits, AES_KEY *);
WOLFSSL_API void wolfSSL_AES_set_decrypt_key
    (const unsigned char *, const int bits, AES_KEY *);
WOLFSSL_API void wolfSSL_AES_encrypt
    (const unsigned char* input, unsigned char* output, AES_KEY *);
WOLFSSL_API void wolfSSL_AES_decrypt
    (const unsigned char* input, unsigned char* output, AES_KEY *);

#define AES_set_encrypt_key wolfSSL_AES_set_encrypt_key
#define AES_set_decrypt_key wolfSSL_AES_set_decrypt_key
#define AES_encrypt         wolfSSL_AES_encrypt
#define AES_decrypt         wolfSSL_AES_decrypt

#define wolfSSL_AES_set_encrypt_key(key, bits, aes) \
    wc_AesSetKey(aes, key, ((bits)/8), NULL, AES_ENCRYPTION)
#define wolfSSL_AES_set_decrypt_key(key, bits, aes) \
    wc_AesSetKey(aes, key, ((bits)/8), NULL, AES_DECRYPTION)

#define wolfSSL_AES_encrypt(in, out, aes) wc_AesEncryptDirect(aes, out, in)
#define wolfSSL_AES_decrypt(in, out, aes) wc_AesDecryptDirect(aes, out, in)

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_AES_DIRECT */
#endif /* NO_AES */

#endif /* WOLFSSL_DES_H_ */
