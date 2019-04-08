/* atecc508.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */


#ifndef _ATECC508_H_
#define _ATECC508_H_

#include <stdint.h>

#ifdef WOLFSSL_ATECC508A
    #undef  SHA_BLOCK_SIZE
    #define SHA_BLOCK_SIZE  SHA_BLOCK_SIZE_REMAP
    #include <cryptoauthlib.h>
    #include <tls/atcatls.h>
    #include <atcacert/atcacert_client.h>
    #include <tls/atcatls_cfg.h>
    #undef SHA_BLOCK_SIZE
#endif

/* ATECC508A only supports ECC-256 */
#define ATECC_KEY_SIZE      (32)
#define ATECC_MAX_SLOT      (0x7) /* Only use 0-7 */
#define ATECC_INVALID_SLOT  (-1)

struct WOLFSSL;
struct WOLFSSL_X509_STORE_CTX;

// Cert Structure
typedef struct t_atcert {
	uint32_t signer_ca_size;
	uint8_t signer_ca[512];
	uint8_t signer_ca_pubkey[64];	
	uint32_t end_user_size;
	uint8_t end_user[512];
	uint8_t end_user_pubkey[64];	
} t_atcert;

extern t_atcert atcert;


/* Amtel port functions */
void atmel_init(void);
int atmel_get_random_number(uint32_t count, uint8_t* rand_out);
long atmel_get_curr_time_and_date(long* tm);

int atmel_ecc_alloc(void);
void atmel_ecc_free(int slot);

#endif /* _ATECC508_H_ */
