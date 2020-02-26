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
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_RSA_C)

#include "mbedtls/rsa.h"
#include "mbedtls/rsa_internal.h"
#include "mbedtls/oid.h"
#include "mbedtls/platform_util.h"

#include <string.h>

#if defined(MBEDTLS_PKCS1_V21)
#include "mbedtls/md.h"
#endif

#if defined(MBEDTLS_PKCS1_V15) && !defined(__OpenBSD__)
#include <stdlib.h>
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#define mbedtls_calloc calloc
#define mbedtls_free   free
#endif

#if defined(MBEDTLS_RSA_ALT)

#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/common/optiga_lib_common.h"
#include "optiga/pal/pal_os_memory.h"
#include "optiga/pal/pal_os_timer.h"

/* Parameter validation macros */
#define RSA_VALIDATE_RET( cond )                                       \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_RSA_BAD_INPUT_DATA )
#define RSA_VALIDATE( cond )                                           \
    MBEDTLS_INTERNAL_VALIDATE( cond )

#if defined(MBEDTLS_PKCS1_V15)

#define TRUSTM_RSA_1024_KEYSIZE              (0x0080)
#define TRUSTM_RSA_2048_KEYSIZE              (0x0100)
#define TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE       (300)
#define TRUSTM_RSA_SIGNATURE_LEN_MAX_SIZE    (300)
#define TRUSTM_RSA_NEGATIVE_INTEGER          (0x7F)
#define TRUSTM_RSA_PRIVATE_KEY_OID           (0xE0FC)

#define TRUSTM_RSA_SET_DER_LENGTH(buffer, index, value) \
                                {\
                                    uint16_t position = *index; \
                                    if (0xFF < value) \
                                    { \
                                        buffer[position++] = (value & 0xFF00) >> 8; \
                                    } \
                                    buffer[position++] = (value & 0xFF); \
                                    *index = position; \
                                }

#define TRUSTM_RSA_CHECK_MODULUS_FIRST_BYTE_NEGATIVE(value) \
         ((TRUSTM_RSA_NEGATIVE_INTEGER < value) ? 1 : 0);

#define TRUSTM_RSA_GET_LENGTH_FIELD_INBYTES(value) \
         ((value > 0xFF)? 0x02 : 0x01);

static volatile optiga_lib_status_t crypt_event_completed_status;

/* constant-time buffer comparison */
static inline int mbedtls_safer_memcmp( const void *a, const void *b, size_t n )
{
    size_t i;
    const unsigned char *A = (const unsigned char *) a;
    const unsigned char *B = (const unsigned char *) b;
    unsigned char diff = 0;

    for( i = 0; i < n; i++ )
        diff |= A[i] ^ B[i];

    return( diff );
}

static void optiga_crypt_event_completed(void * context, optiga_lib_status_t return_status)
{
    crypt_event_completed_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

static void mbedtls_rsa_create_public_key_bit_string_format(const uint8_t * n_buffer,
                                                            uint16_t n_length,
                                                            const uint8_t * e_buffer,
                                                            uint16_t e_length,
                                                            uint8_t * key_buffer,
                                                            uint16_t * key_length)
{
#define TRUSTM_RSA_DER_BITSTRING_TAG     (0x03)
#define TRUSTM_RSA_DER_MIN_LEN_FIELD     (0x80)
#define TRUSTM_RSA_DER_SEQUENCE_TAG      (0x30)
#define TRUSTM_RSA_DER_INTEGER_TAG       (0x02)
#define TRUSTM_RSA_DER_NUM_UNUSED_BITS   (0x00)

    uint16_t index = 0;
    uint16_t modulus_signed_bit_len = 0;
    uint8_t length_field_in_bytes = 0;

    // Check if the first byte of modulus is a negative integer
    modulus_signed_bit_len = TRUSTM_RSA_CHECK_MODULUS_FIRST_BYTE_NEGATIVE(n_buffer[0]);

    // Check the number of bytes required to represent the length field
    length_field_in_bytes = TRUSTM_RSA_GET_LENGTH_FIELD_INBYTES(n_length);

    // RSA public key formation according to DER encoded format
    key_buffer[index++] = TRUSTM_RSA_DER_BITSTRING_TAG;
    key_buffer[index++] = TRUSTM_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    // 7 bytes = Unused Bits(1 byte) + Sequence Tag (1 byte) +
    //           Length tag (1 byte) * 3 occurrence + Integer tag (1 byte) * 2 occurrence
    TRUSTM_RSA_SET_DER_LENGTH(key_buffer, &index, (0x07 +
                                                   n_length +
                                                   modulus_signed_bit_len +
                                                   e_length +
                                                   (2 * length_field_in_bytes)))
    key_buffer[index++] = TRUSTM_RSA_DER_NUM_UNUSED_BITS;

    key_buffer[index++] = TRUSTM_RSA_DER_SEQUENCE_TAG;
    key_buffer[index++] = TRUSTM_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    // 4 bytes = Integer tag (1 byte) * 2 occurrence + Length tag (1 byte) * 2 occurrence
    TRUSTM_RSA_SET_DER_LENGTH(key_buffer, &index, (0x04 +
                                                   n_length +
                                                   modulus_signed_bit_len +
                                                   e_length +
                                                   length_field_in_bytes))

    key_buffer[index++] = TRUSTM_RSA_DER_INTEGER_TAG;
    key_buffer[index++] = TRUSTM_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    TRUSTM_RSA_SET_DER_LENGTH(key_buffer, &index, (n_length  + modulus_signed_bit_len))
    index += modulus_signed_bit_len;
    pal_os_memcpy(&key_buffer[index], n_buffer, n_length);
    index += n_length;

    key_buffer[index++] = TRUSTM_RSA_DER_INTEGER_TAG;
    key_buffer[index++] = e_length;
    pal_os_memcpy(&key_buffer[index], e_buffer, e_length);
    index += e_length;

    *key_length = index;

#undef TRUSTM_RSA_DER_BITSTRING_TAG
#undef TRUSTM_RSA_DER_MIN_LEN_FIELD
#undef TRUSTM_RSA_DER_SEQUENCE_TAG
#undef TRUSTM_RSA_DER_INTEGER_TAG
#undef TRUSTM_RSA_DER_NUM_UNUSED_BITS
}

static int mbedtls_rsa_get_sig_scheme_digest_len(mbedtls_md_type_t md_alg,
                                                 optiga_rsa_signature_scheme_t * signature_scheme,
                                                 uint8_t * digest_length)
{
    int return_status = 0;
    optiga_rsa_signature_scheme_t sig_scheme;
    uint8_t digest_len;

    do
    {
        if (MBEDTLS_MD_SHA256 == md_alg)
        {
            sig_scheme = OPTIGA_RSASSA_PKCS1_V15_SHA256;
            digest_len = 32;
        }
        else if (MBEDTLS_MD_SHA384 == md_alg)
        {
            sig_scheme = OPTIGA_RSASSA_PKCS1_V15_SHA384;
            digest_len = 48;
        }
        else
        {
            return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
            break;
        }
        if (NULL != signature_scheme)
        {
            *signature_scheme = sig_scheme;
        }
        if (NULL != digest_length)
        {
            *digest_length = digest_len;
        }
    } while (FALSE);
    return (return_status);
}

static int mbedtls_rsa_get_sig_len_key_type(uint16_t modulus_length,
                                            uint16_t * signature_length,
                                            uint8_t * key_type)
{
    int return_status = 0;
    uint16_t sig_len;
    uint8_t rsa_key;

    do
    {
        if (TRUSTM_RSA_2048_KEYSIZE == modulus_length)
        {
            sig_len = TRUSTM_RSA_2048_KEYSIZE;
            rsa_key = OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL;
        }
        else if (TRUSTM_RSA_1024_KEYSIZE == modulus_length)
        {
            sig_len = TRUSTM_RSA_1024_KEYSIZE;
            rsa_key = OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;
        }
        else
        {
            return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
            break;
        }
        if (NULL != signature_length)
        {
            *signature_length = sig_len;
        }
        if (NULL != key_type)
        {
            *key_type = rsa_key;
        }
    } while (FALSE);
    return (return_status);
}

#endif /* MBEDTLS_PKCS1_V15 */

int mbedtls_rsa_import( mbedtls_rsa_context *ctx,
                        const mbedtls_mpi *N,
                        const mbedtls_mpi *P, const mbedtls_mpi *Q,
                        const mbedtls_mpi *D, const mbedtls_mpi *E )
{
    int ret;
    RSA_VALIDATE_RET( ctx != NULL );

    if( ( N != NULL && ( ret = mbedtls_mpi_copy( &ctx->N, N ) ) != 0 ) ||
        ( P != NULL && ( ret = mbedtls_mpi_copy( &ctx->P, P ) ) != 0 ) ||
        ( Q != NULL && ( ret = mbedtls_mpi_copy( &ctx->Q, Q ) ) != 0 ) ||
        ( D != NULL && ( ret = mbedtls_mpi_copy( &ctx->D, D ) ) != 0 ) ||
        ( E != NULL && ( ret = mbedtls_mpi_copy( &ctx->E, E ) ) != 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }

    if( N != NULL )
        ctx->len = mbedtls_mpi_size( &ctx->N );

    return( 0 );
}

int mbedtls_rsa_import_raw( mbedtls_rsa_context *ctx,
                            unsigned char const *N, size_t N_len,
                            unsigned char const *P, size_t P_len,
                            unsigned char const *Q, size_t Q_len,
                            unsigned char const *D, size_t D_len,
                            unsigned char const *E, size_t E_len )
{
    int ret = 0;
    RSA_VALIDATE_RET( ctx != NULL );

    if( N != NULL )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->N, N, N_len ) );
        ctx->len = mbedtls_mpi_size( &ctx->N );
    }

    if( P != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->P, P, P_len ) );

    if( Q != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->Q, Q, Q_len ) );

    if( D != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->D, D, D_len ) );

    if( E != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->E, E, E_len ) );

cleanup:

    if( ret != 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );

    return( 0 );
}

/*
 * Checks whether the context fields are set in such a way
 * that the RSA primitives will be able to execute without error.
 * It does *not* make guarantees for consistency of the parameters.
 */
static int rsa_check_context( mbedtls_rsa_context const *ctx, int is_priv,
                              int blinding_needed )
{
#if !defined(MBEDTLS_RSA_NO_CRT)
    /* blinding_needed is only used for NO_CRT to decide whether
     * P,Q need to be present or not. */
    ((void) blinding_needed);
#endif

    if( ctx->len != mbedtls_mpi_size( &ctx->N ) ||
        ctx->len > MBEDTLS_MPI_MAX_SIZE )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

    /*
     * 1. Modular exponentiation needs positive, odd moduli.
     */

    /* Modular exponentiation wrt. N is always used for
     * RSA public key operations. */
    if( mbedtls_mpi_cmp_int( &ctx->N, 0 ) <= 0 ||
        mbedtls_mpi_get_bit( &ctx->N, 0 ) == 0  )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Modular exponentiation for P and Q is only
     * used for private key operations and if CRT
     * is used. */
    if( is_priv &&
        ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) <= 0 ||
          mbedtls_mpi_get_bit( &ctx->P, 0 ) == 0 ||
          mbedtls_mpi_cmp_int( &ctx->Q, 0 ) <= 0 ||
          mbedtls_mpi_get_bit( &ctx->Q, 0 ) == 0  ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif /* !MBEDTLS_RSA_NO_CRT */

    /*
     * 2. Exponents must be positive
     */

    /* Always need E for public key operations */
    if( mbedtls_mpi_cmp_int( &ctx->E, 0 ) <= 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

#if defined(MBEDTLS_RSA_NO_CRT)
    /* For private key operations, use D or DP & DQ
     * as (unblinded) exponents. */
    if( is_priv && mbedtls_mpi_cmp_int( &ctx->D, 0 ) <= 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
#else
    if( is_priv &&
        ( mbedtls_mpi_cmp_int( &ctx->DP, 0 ) <= 0 ||
          mbedtls_mpi_cmp_int( &ctx->DQ, 0 ) <= 0  ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Blinding shouldn't make exponents negative either,
     * so check that P, Q >= 1 if that hasn't yet been
     * done as part of 1. */
#if defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv && blinding_needed &&
        ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) <= 0 ||
          mbedtls_mpi_cmp_int( &ctx->Q, 0 ) <= 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif

    /* It wouldn't lead to an error if it wasn't satisfied,
     * but check for QP >= 1 nonetheless. */
#if !defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv &&
        mbedtls_mpi_cmp_int( &ctx->QP, 0 ) <= 0 )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif

    return( 0 );
}

int mbedtls_rsa_complete( mbedtls_rsa_context *ctx )
{
    int ret = 0;
    int have_N, have_P, have_Q, have_D, have_E;
    int n_missing, pq_missing, d_missing, is_pub, is_priv;

    RSA_VALIDATE_RET( ctx != NULL );

    have_N = ( mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 );
    have_P = ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 );
    have_Q = ( mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 );
    have_D = ( mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 );
    have_E = ( mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0 );

    /*
     * Check whether provided parameters are enough
     * to deduce all others. The following incomplete
     * parameter sets for private keys are supported:
     *
     * (1) P, Q missing.
     * (2) D and potentially N missing.
     *
     */

    n_missing  =              have_P &&  have_Q &&  have_D && have_E;
    pq_missing =   have_N && !have_P && !have_Q &&  have_D && have_E;
    d_missing  =              have_P &&  have_Q && !have_D && have_E;
    is_pub     =   have_N && !have_P && !have_Q && !have_D && have_E;

    /* These three alternatives are mutually exclusive */
    is_priv = n_missing || pq_missing || d_missing;

    if( !is_priv && !is_pub )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    /*
     * Step 1: Deduce N if P, Q are provided.
     */

    if( !have_N && have_P && have_Q )
    {
        if( ( ret = mbedtls_mpi_mul_mpi( &ctx->N, &ctx->P,
                                         &ctx->Q ) ) != 0 )
        {
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
        }

        ctx->len = mbedtls_mpi_size( &ctx->N );
    }

    /*
     * Step 2: Deduce and verify all remaining core parameters.
     */

    if( pq_missing )
    {
        ret = mbedtls_rsa_deduce_primes( &ctx->N, &ctx->E, &ctx->D,
                                         &ctx->P, &ctx->Q );
        if( ret != 0 )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );

    }
    else if( d_missing )
    {
        if( ( ret = mbedtls_rsa_deduce_private_exponent( &ctx->P,
                                                         &ctx->Q,
                                                         &ctx->E,
                                                         &ctx->D ) ) != 0 )
        {
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
        }
    }

    /*
     * Step 3: Deduce all additional parameters specific
     *         to our current RSA implementation.
     */

#if !defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv )
    {
        ret = mbedtls_rsa_deduce_crt( &ctx->P,  &ctx->Q,  &ctx->D,
                                      &ctx->DP, &ctx->DQ, &ctx->QP );
        if( ret != 0 )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }
#endif /* MBEDTLS_RSA_NO_CRT */

    /*
     * Step 3: Basic sanity checks
     */

    return( rsa_check_context( ctx, is_priv, 1 ) );
}

int mbedtls_rsa_export_raw( const mbedtls_rsa_context *ctx,
                            unsigned char *N, size_t N_len,
                            unsigned char *P, size_t P_len,
                            unsigned char *Q, size_t Q_len,
                            unsigned char *D, size_t D_len,
                            unsigned char *E, size_t E_len )
{
    int ret = 0;
    int is_priv;
    RSA_VALIDATE_RET( ctx != NULL );

    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0;

    if( !is_priv )
    {
        /* If we're trying to export private parameters for a public key,
         * something must be wrong. */
        if( P != NULL || Q != NULL || D != NULL )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    }

    if( N != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->N, N, N_len ) );

    if( P != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->P, P, P_len ) );

    if( Q != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->Q, Q, Q_len ) );

    if( D != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->D, D, D_len ) );

    if( E != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->E, E, E_len ) );

cleanup:

    return( ret );
}

int mbedtls_rsa_export( const mbedtls_rsa_context *ctx,
                        mbedtls_mpi *N, mbedtls_mpi *P, mbedtls_mpi *Q,
                        mbedtls_mpi *D, mbedtls_mpi *E )
{
    int ret;
    int is_priv;
    RSA_VALIDATE_RET( ctx != NULL );

    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0;

    if( !is_priv )
    {
        /* If we're trying to export private parameters for a public key,
         * something must be wrong. */
        if( P != NULL || Q != NULL || D != NULL )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    }

    /* Export all requested core parameters. */

    if( ( N != NULL && ( ret = mbedtls_mpi_copy( N, &ctx->N ) ) != 0 ) ||
        ( P != NULL && ( ret = mbedtls_mpi_copy( P, &ctx->P ) ) != 0 ) ||
        ( Q != NULL && ( ret = mbedtls_mpi_copy( Q, &ctx->Q ) ) != 0 ) ||
        ( D != NULL && ( ret = mbedtls_mpi_copy( D, &ctx->D ) ) != 0 ) ||
        ( E != NULL && ( ret = mbedtls_mpi_copy( E, &ctx->E ) ) != 0 ) )
    {
        return( ret );
    }

    return( 0 );
}

/*
 * Export CRT parameters
 * This must also be implemented if CRT is not used, for being able to
 * write DER encoded RSA keys. The helper function mbedtls_rsa_deduce_crt
 * can be used in this case.
 */
int mbedtls_rsa_export_crt( const mbedtls_rsa_context *ctx,
                            mbedtls_mpi *DP, mbedtls_mpi *DQ, mbedtls_mpi *QP )
{
    int ret;
    int is_priv;
    RSA_VALIDATE_RET( ctx != NULL );

    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0;

    if( !is_priv )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Export all requested blinding parameters. */
    if( ( DP != NULL && ( ret = mbedtls_mpi_copy( DP, &ctx->DP ) ) != 0 ) ||
        ( DQ != NULL && ( ret = mbedtls_mpi_copy( DQ, &ctx->DQ ) ) != 0 ) ||
        ( QP != NULL && ( ret = mbedtls_mpi_copy( QP, &ctx->QP ) ) != 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }
#else
    if( ( ret = mbedtls_rsa_deduce_crt( &ctx->P, &ctx->Q, &ctx->D,
                                        DP, DQ, QP ) ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }
#endif

    return( 0 );
}

/*
 * Initialize an RSA context
 */
void mbedtls_rsa_init( mbedtls_rsa_context *ctx,
               int padding,
               int hash_id )
{
    RSA_VALIDATE( ctx != NULL );
    RSA_VALIDATE( padding == MBEDTLS_RSA_PKCS_V15 ||
                  padding == MBEDTLS_RSA_PKCS_V21 );

    memset( ctx, 0, sizeof( mbedtls_rsa_context ) );

    mbedtls_rsa_set_padding( ctx, padding, hash_id );

#if defined(MBEDTLS_THREADING_C)
    mbedtls_mutex_init( &ctx->mutex );
#endif
}

/*
 * Set padding for an existing RSA context
 */
void mbedtls_rsa_set_padding( mbedtls_rsa_context *ctx, int padding,
                              int hash_id )
{
    RSA_VALIDATE( ctx != NULL );
    RSA_VALIDATE( padding == MBEDTLS_RSA_PKCS_V15 ||
                  padding == MBEDTLS_RSA_PKCS_V21 );

    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

/*
 * Get length in bytes of RSA modulus
 */

size_t mbedtls_rsa_get_len( const mbedtls_rsa_context *ctx )
{
    return( ctx->len );
}


#if defined(MBEDTLS_GENPRIME)

/*
 * Generate an RSA keypair
 *
 * This generation method follows the RSA key pair generation procedure of
 * FIPS 186-4 if 2^16 < exponent < 2^256 and nbits = 2048 or nbits = 3072.
 */
int mbedtls_rsa_gen_key( mbedtls_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 unsigned int nbits, int exponent )
{
    int ret;
    mbedtls_mpi H, G, L;
    int prime_quality = 0;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( f_rng != NULL );

    if( nbits < 128 || exponent < 3 || nbits % 2 != 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    /*
     * If the modulus is 1024 bit long or shorter, then the security strength of
     * the RSA algorithm is less than or equal to 80 bits and therefore an error
     * rate of 2^-80 is sufficient.
     */
    if( nbits > 1024 )
        prime_quality = MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR;

    mbedtls_mpi_init( &H );
    mbedtls_mpi_init( &G );
    mbedtls_mpi_init( &L );

    /*
     * find primes P and Q with Q < P so that:
     * 1.  |P-Q| > 2^( nbits / 2 - 100 )
     * 2.  GCD( E, (P-1)*(Q-1) ) == 1
     * 3.  E^-1 mod LCM(P-1, Q-1) > 2^( nbits / 2 )
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &ctx->E, exponent ) );

    do
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_gen_prime( &ctx->P, nbits >> 1,
                                                prime_quality, f_rng, p_rng ) );

        MBEDTLS_MPI_CHK( mbedtls_mpi_gen_prime( &ctx->Q, nbits >> 1,
                                                prime_quality, f_rng, p_rng ) );

        /* make sure the difference between p and q is not too small (FIPS 186-4 §B.3.3 step 5.4) */
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &H, &ctx->P, &ctx->Q ) );
        if( mbedtls_mpi_bitlen( &H ) <= ( ( nbits >= 200 ) ? ( ( nbits >> 1 ) - 99 ) : 0 ) )
            continue;

        /* not required by any standards, but some users rely on the fact that P > Q */
        if( H.s < 0 )
            mbedtls_mpi_swap( &ctx->P, &ctx->Q );

        /* Temporarily replace P,Q by P-1, Q-1 */
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &ctx->P, &ctx->P, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &ctx->Q, &ctx->Q, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &H, &ctx->P, &ctx->Q ) );

        /* check GCD( E, (P-1)*(Q-1) ) == 1 (FIPS 186-4 §B.3.1 criterion 2(a)) */
        MBEDTLS_MPI_CHK( mbedtls_mpi_gcd( &G, &ctx->E, &H  ) );
        if( mbedtls_mpi_cmp_int( &G, 1 ) != 0 )
            continue;

        /* compute smallest possible D = E^-1 mod LCM(P-1, Q-1) (FIPS 186-4 §B.3.1 criterion 3(b)) */
        MBEDTLS_MPI_CHK( mbedtls_mpi_gcd( &G, &ctx->P, &ctx->Q ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_div_mpi( &L, NULL, &H, &G ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( &ctx->D, &ctx->E, &L ) );

        if( mbedtls_mpi_bitlen( &ctx->D ) <= ( ( nbits + 1 ) / 2 ) ) // (FIPS 186-4 §B.3.1 criterion 3(a))
            continue;

        break;
    }
    while( 1 );

    /* Restore P,Q */
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_int( &ctx->P,  &ctx->P, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_int( &ctx->Q,  &ctx->Q, 1 ) );

    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->N, &ctx->P, &ctx->Q ) );

    ctx->len = mbedtls_mpi_size( &ctx->N );

#if !defined(MBEDTLS_RSA_NO_CRT)
    /*
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    MBEDTLS_MPI_CHK( mbedtls_rsa_deduce_crt( &ctx->P, &ctx->Q, &ctx->D,
                                             &ctx->DP, &ctx->DQ, &ctx->QP ) );
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Double-check */
    MBEDTLS_MPI_CHK( mbedtls_rsa_check_privkey( ctx ) );

cleanup:

    mbedtls_mpi_free( &H );
    mbedtls_mpi_free( &G );
    mbedtls_mpi_free( &L );

    if( ret != 0 )
    {
        mbedtls_rsa_free( ctx );
        return( MBEDTLS_ERR_RSA_KEY_GEN_FAILED + ret );
    }

    return( 0 );
}

#endif /* MBEDTLS_GENPRIME */

/*
 * Check a public RSA key
 */
int mbedtls_rsa_check_pubkey( const mbedtls_rsa_context *ctx )
{
    RSA_VALIDATE_RET( ctx != NULL );

    if( rsa_check_context( ctx, 0 /* public */, 0 /* no blinding */ ) != 0 )
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );

    if( mbedtls_mpi_bitlen( &ctx->N ) < 128 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mbedtls_mpi_get_bit( &ctx->E, 0 ) == 0 ||
        mbedtls_mpi_bitlen( &ctx->E )     < 2  ||
        mbedtls_mpi_cmp_mpi( &ctx->E, &ctx->N ) >= 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    return( 0 );
}

/*
 * Check for the consistency of all fields in an RSA private key context
 */
int mbedtls_rsa_check_privkey( const mbedtls_rsa_context *ctx )
{
    RSA_VALIDATE_RET( ctx != NULL );

    if( mbedtls_rsa_check_pubkey( ctx ) != 0 ||
        rsa_check_context( ctx, 1 /* private */, 1 /* blinding */ ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mbedtls_rsa_validate_params( &ctx->N, &ctx->P, &ctx->Q,
                                     &ctx->D, &ctx->E, NULL, NULL ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    else if( mbedtls_rsa_validate_crt( &ctx->P, &ctx->Q, &ctx->D,
                                       &ctx->DP, &ctx->DQ, &ctx->QP ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
#endif

    return( 0 );
}

/*
 * Check if contexts holding a public and private key match
 */
int mbedtls_rsa_check_pub_priv( const mbedtls_rsa_context *pub,
                                const mbedtls_rsa_context *prv )
{
    RSA_VALIDATE_RET( pub != NULL );
    RSA_VALIDATE_RET( prv != NULL );

    if( mbedtls_rsa_check_pubkey( pub )  != 0 ||
        mbedtls_rsa_check_privkey( prv ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mbedtls_mpi_cmp_mpi( &pub->N, &prv->N ) != 0 ||
        mbedtls_mpi_cmp_mpi( &pub->E, &prv->E ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    return( 0 );
}

/*
 * Do an RSA public key operation
 */
int mbedtls_rsa_public( mbedtls_rsa_context *ctx,
                const unsigned char *input,
                unsigned char *output )
{
    int ret;
    size_t olen;
    mbedtls_mpi T;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( output != NULL );

    if( rsa_check_context( ctx, 0 /* public */, 0 /* no blinding */ ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    mbedtls_mpi_init( &T );

#if defined(MBEDTLS_THREADING_C)
    if( ( ret = mbedtls_mutex_lock( &ctx->mutex ) ) != 0 )
        return( ret );
#endif

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &T, input, ctx->len ) );

    if( mbedtls_mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

    olen = ctx->len;
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &T, output, olen ) );

cleanup:
#if defined(MBEDTLS_THREADING_C)
    if( mbedtls_mutex_unlock( &ctx->mutex ) != 0 )
        return( MBEDTLS_ERR_THREADING_MUTEX_ERROR );
#endif

    mbedtls_mpi_free( &T );

    if( ret != 0 )
        return( MBEDTLS_ERR_RSA_PUBLIC_FAILED + ret );

    return( 0 );
}

/*
 * Generate or update blinding values, see section 10 of:
 *  KOCHER, Paul C. Timing attacks on implementations of Diffie-Hellman, RSA,
 *  DSS, and other systems. In : Advances in Cryptology-CRYPTO'96. Springer
 *  Berlin Heidelberg, 1996. p. 104-113.
 */
static int rsa_prepare_blinding( mbedtls_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret, count = 0;

    if( ctx->Vf.p != NULL )
    {
        /* We already have blinding values, just update them by squaring */
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vi, &ctx->Vi, &ctx->Vi ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vi, &ctx->Vi, &ctx->N ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vf, &ctx->Vf, &ctx->Vf ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vf, &ctx->Vf, &ctx->N ) );

        goto cleanup;
    }

    /* Unblinding value: Vf = random number, invertible mod N */
    do {
        if( count++ > 10 )
            return( MBEDTLS_ERR_RSA_RNG_FAILED );

        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &ctx->Vf, ctx->len - 1, f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_gcd( &ctx->Vi, &ctx->Vf, &ctx->N ) );
    } while( mbedtls_mpi_cmp_int( &ctx->Vi, 1 ) != 0 );

    /* Blinding value: Vi =  Vf^(-e) mod N */
    MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( &ctx->Vi, &ctx->Vf, &ctx->N ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &ctx->Vi, &ctx->Vi, &ctx->E, &ctx->N, &ctx->RN ) );


cleanup:
    return( ret );
}

/*
 * Exponent blinding supposed to prevent side-channel attacks using multiple
 * traces of measurements to recover the RSA key. The more collisions are there,
 * the more bits of the key can be recovered. See [3].
 *
 * Collecting n collisions with m bit long blinding value requires 2^(m-m/n)
 * observations on avarage.
 *
 * For example with 28 byte blinding to achieve 2 collisions the adversary has
 * to make 2^112 observations on avarage.
 *
 * (With the currently (as of 2017 April) known best algorithms breaking 2048
 * bit RSA requires approximately as much time as trying out 2^112 random keys.
 * Thus in this sense with 28 byte blinding the security is not reduced by
 * side-channel attacks like the one in [3])
 *
 * This countermeasure does not help if the key recovery is possible with a
 * single trace.
 */
#define RSA_EXPONENT_BLINDING 28

/*
 * Do an RSA private key operation
 */
int mbedtls_rsa_private( mbedtls_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 const unsigned char *input,
                 unsigned char *output )
{
    int ret;
    size_t olen;

    /* Temporary holding the result */
    mbedtls_mpi T;

    /* Temporaries holding P-1, Q-1 and the
     * exponent blinding factor, respectively. */
    mbedtls_mpi P1, Q1, R;

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Temporaries holding the results mod p resp. mod q. */
    mbedtls_mpi TP, TQ;

    /* Temporaries holding the blinded exponents for
     * the mod p resp. mod q computation (if used). */
    mbedtls_mpi DP_blind, DQ_blind;

    /* Pointers to actual exponents to be used - either the unblinded
     * or the blinded ones, depending on the presence of a PRNG. */
    mbedtls_mpi *DP = &ctx->DP;
    mbedtls_mpi *DQ = &ctx->DQ;
#else
    /* Temporary holding the blinded exponent (if used). */
    mbedtls_mpi D_blind;

    /* Pointer to actual exponent to be used - either the unblinded
     * or the blinded one, depending on the presence of a PRNG. */
    mbedtls_mpi *D = &ctx->D;
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Temporaries holding the initial input and the double
     * checked result; should be the same in the end. */
    mbedtls_mpi I, C;

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( input  != NULL );
    RSA_VALIDATE_RET( output != NULL );

    if( rsa_check_context( ctx, 1             /* private key checks */,
                                f_rng != NULL /* blinding y/n       */ ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

#if defined(MBEDTLS_THREADING_C)
    if( ( ret = mbedtls_mutex_lock( &ctx->mutex ) ) != 0 )
        return( ret );
#endif

    /* MPI Initialization */
    mbedtls_mpi_init( &T );

    mbedtls_mpi_init( &P1 );
    mbedtls_mpi_init( &Q1 );
    mbedtls_mpi_init( &R );

    if( f_rng != NULL )
    {
#if defined(MBEDTLS_RSA_NO_CRT)
        mbedtls_mpi_init( &D_blind );
#else
        mbedtls_mpi_init( &DP_blind );
        mbedtls_mpi_init( &DQ_blind );
#endif
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi_init( &TP ); mbedtls_mpi_init( &TQ );
#endif

    mbedtls_mpi_init( &I );
    mbedtls_mpi_init( &C );

    /* End of MPI initialization */

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &T, input, ctx->len ) );
    if( mbedtls_mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &I, &T ) );

    if( f_rng != NULL )
    {
        /*
         * Blinding
         * T = T * Vi mod N
         */
        MBEDTLS_MPI_CHK( rsa_prepare_blinding( ctx, f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T, &T, &ctx->Vi ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &T, &ctx->N ) );

        /*
         * Exponent blinding
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &P1, &ctx->P, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &Q1, &ctx->Q, 1 ) );

#if defined(MBEDTLS_RSA_NO_CRT)
        /*
         * D_blind = ( P - 1 ) * ( Q - 1 ) * R + D
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &R, RSA_EXPONENT_BLINDING,
                         f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &D_blind, &P1, &Q1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &D_blind, &D_blind, &R ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &D_blind, &D_blind, &ctx->D ) );

        D = &D_blind;
#else
        /*
         * DP_blind = ( P - 1 ) * R + DP
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &R, RSA_EXPONENT_BLINDING,
                         f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &DP_blind, &P1, &R ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &DP_blind, &DP_blind,
                    &ctx->DP ) );

        DP = &DP_blind;

        /*
         * DQ_blind = ( Q - 1 ) * R + DQ
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &R, RSA_EXPONENT_BLINDING,
                         f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &DQ_blind, &Q1, &R ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &DQ_blind, &DQ_blind,
                    &ctx->DQ ) );

        DQ = &DQ_blind;
#endif /* MBEDTLS_RSA_NO_CRT */
    }

#if defined(MBEDTLS_RSA_NO_CRT)
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T, &T, D, &ctx->N, &ctx->RN ) );
#else
    /*
     * Faster decryption using the CRT
     *
     * TP = input ^ dP mod P
     * TQ = input ^ dQ mod Q
     */

    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &TP, &T, DP, &ctx->P, &ctx->RP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &TQ, &T, DQ, &ctx->Q, &ctx->RQ ) );

    /*
     * T = (TP - TQ) * (Q^-1 mod P) mod P
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &T, &TP, &TQ ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &TP, &T, &ctx->QP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &TP, &ctx->P ) );

    /*
     * T = TQ + T * Q
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &TP, &T, &ctx->Q ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &T, &TQ, &TP ) );
#endif /* MBEDTLS_RSA_NO_CRT */

    if( f_rng != NULL )
    {
        /*
         * Unblind
         * T = T * Vf mod N
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T, &T, &ctx->Vf ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &T, &ctx->N ) );
    }

    /* Verify the result to prevent glitching attacks. */
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &C, &T, &ctx->E,
                                          &ctx->N, &ctx->RN ) );
    if( mbedtls_mpi_cmp_mpi( &C, &I ) != 0 )
    {
        ret = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;
    }

    olen = ctx->len;
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &T, output, olen ) );

cleanup:
#if defined(MBEDTLS_THREADING_C)
    if( mbedtls_mutex_unlock( &ctx->mutex ) != 0 )
        return( MBEDTLS_ERR_THREADING_MUTEX_ERROR );
#endif

    mbedtls_mpi_free( &P1 );
    mbedtls_mpi_free( &Q1 );
    mbedtls_mpi_free( &R );

    if( f_rng != NULL )
    {
#if defined(MBEDTLS_RSA_NO_CRT)
        mbedtls_mpi_free( &D_blind );
#else
        mbedtls_mpi_free( &DP_blind );
        mbedtls_mpi_free( &DQ_blind );
#endif
    }

    mbedtls_mpi_free( &T );

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi_free( &TP ); mbedtls_mpi_free( &TQ );
#endif

    mbedtls_mpi_free( &C );
    mbedtls_mpi_free( &I );

    if( ret != 0 )
        return( MBEDTLS_ERR_RSA_PRIVATE_FAILED + ret );

    return( 0 );
}

#if defined(MBEDTLS_PKCS1_V21)
/**
 * Generate and apply the MGF1 operation (from PKCS#1 v2.1) to a buffer.
 *
 * \param dst       buffer to mask
 * \param dlen      length of destination buffer
 * \param src       source of the mask generation
 * \param slen      length of the source buffer
 * \param md_ctx    message digest context to use
 */
static int mgf_mask( unsigned char *dst, size_t dlen, unsigned char *src,
                      size_t slen, mbedtls_md_context_t *md_ctx )
{
    unsigned char mask[MBEDTLS_MD_MAX_SIZE];
    unsigned char counter[4];
    unsigned char *p;
    unsigned int hlen;
    size_t i, use_len;
    int ret = 0;

    memset( mask, 0, MBEDTLS_MD_MAX_SIZE );
    memset( counter, 0, 4 );

    hlen = mbedtls_md_get_size( md_ctx->md_info );

    /* Generate and apply dbMask */
    p = dst;

    while( dlen > 0 )
    {
        use_len = hlen;
        if( dlen < hlen )
            use_len = dlen;

        if( ( ret = mbedtls_md_starts( md_ctx ) ) != 0 )
            goto exit;
        if( ( ret = mbedtls_md_update( md_ctx, src, slen ) ) != 0 )
            goto exit;
        if( ( ret = mbedtls_md_update( md_ctx, counter, 4 ) ) != 0 )
            goto exit;
        if( ( ret = mbedtls_md_finish( md_ctx, mask ) ) != 0 )
            goto exit;

        for( i = 0; i < use_len; ++i )
            *p++ ^= mask[i];

        counter[3]++;

        dlen -= use_len;
    }

exit:
    mbedtls_platform_zeroize( mask, sizeof( mask ) );

    return( ret );
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-ENCRYPT function
 */
int mbedtls_rsa_rsaes_oaep_encrypt( mbedtls_rsa_context *ctx,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng,
                            int mode,
                            const unsigned char *label, size_t label_len,
                            size_t ilen,
                            const unsigned char *input,
                            unsigned char *output )
{
    size_t olen;
    int ret;
    unsigned char *p = output;
    unsigned int hlen;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( label_len == 0 || label != NULL );

    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    if( f_rng == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    md_info = mbedtls_md_info_from_type( (mbedtls_md_type_t) ctx->hash_id );
    if( md_info == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    olen = ctx->len;
    hlen = mbedtls_md_get_size( md_info );

    /* first comparison checks for overflow */
    if( ilen + 2 * hlen + 2 < ilen || olen < ilen + 2 * hlen + 2 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    memset( output, 0, olen );

    *p++ = 0;

    /* Generate a random octet string seed */
    if( ( ret = f_rng( p_rng, p, hlen ) ) != 0 )
        return( MBEDTLS_ERR_RSA_RNG_FAILED + ret );

    p += hlen;

    /* Construct DB */
    if( ( ret = mbedtls_md( md_info, label, label_len, p ) ) != 0 )
        return( ret );
    p += hlen;
    p += olen - 2 * hlen - 2 - ilen;
    *p++ = 1;
    memcpy( p, input, ilen );

    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
        goto exit;

    /* maskedDB: Apply dbMask to DB */
    if( ( ret = mgf_mask( output + hlen + 1, olen - hlen - 1, output + 1, hlen,
                          &md_ctx ) ) != 0 )
        goto exit;

    /* maskedSeed: Apply seedMask to seed */
    if( ( ret = mgf_mask( output + 1, hlen, output + hlen + 1, olen - hlen - 1,
                          &md_ctx ) ) != 0 )
        goto exit;

exit:
    mbedtls_md_free( &md_ctx );

    if( ret != 0 )
        return( ret );

    return( ( mode == MBEDTLS_RSA_PUBLIC )
            ? mbedtls_rsa_public(  ctx, output, output )
            : mbedtls_rsa_private( ctx, f_rng, p_rng, output, output ) );
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-ENCRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_encrypt( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t ilen,
                                 const unsigned char *input,
                                 unsigned char *output )
{
    optiga_lib_status_t crypt_sync_status = OPTIGA_CRYPT_ERROR;
    optiga_crypt_t * me_crypt = NULL;
    int return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    public_key_from_host_t public_key_from_host;
    uint8_t * bit_string_pb_key = NULL;
    uint8_t * modulus_buffer = NULL;
    uint8_t public_exponent_buffer[4] = {0x00};
    uint16_t modulus_length = 0;
    uint16_t public_exponent_length = 4;

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
    						 mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output != NULL );
    RSA_VALIDATE_RET( input != NULL );

    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    me_crypt = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
    if (NULL == me_crypt)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    modulus_length = ctx->len;

    bit_string_pb_key = pal_os_calloc(1,TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE);
    if (NULL == bit_string_pb_key)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }
    modulus_buffer = pal_os_calloc(1,modulus_length);
    if (NULL == modulus_buffer)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    // Write the public key components
    return_status = mbedtls_mpi_write_binary(&(ctx->N), modulus_buffer, modulus_length);
    return_status |= mbedtls_mpi_write_binary(&(ctx->E), public_exponent_buffer, public_exponent_length);

    if (0 != return_status)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    // Get the RSA key type
    return_status = mbedtls_rsa_get_sig_len_key_type(modulus_length, NULL, &(public_key_from_host.key_type));
    if (0 != return_status)
    {
        goto cleanup;
    }

    // RSA public key formation according to DER encoded format
    mbedtls_rsa_create_public_key_bit_string_format(modulus_buffer,
                                                    modulus_length,
                                                    public_exponent_buffer,
                                                    public_exponent_length,
                                                    bit_string_pb_key,
                                                    &public_key_from_host.length);

    public_key_from_host.public_key = bit_string_pb_key;

    crypt_event_completed_status = OPTIGA_LIB_BUSY;
    crypt_sync_status = optiga_crypt_rsa_encrypt_message(me_crypt,
															OPTIGA_RSAES_PKCS1_V15,
															input,
															ilen,
															NULL,
															0,
															OPTIGA_CRYPT_HOST_DATA,
															&public_key_from_host,
															output,
															(uint16_t *)&ctx->len);
    if (OPTIGA_LIB_SUCCESS != crypt_sync_status)
    {
        goto cleanup;
    }

    //Wait until optiga_crypt_rsa_sign is completed
    while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
    {
        //Wait until the optiga_crypt_rsa_sign operation is completed
    	pal_os_timer_delay_in_milliseconds(5);
    }
    if (OPTIGA_LIB_SUCCESS != crypt_event_completed_status )
    {
        goto cleanup;
    }
    return_status = 0;
cleanup:
	// destroy crypt instances
	if (me_crypt != NULL)
	{
		(void)optiga_crypt_destroy(me_crypt);
	}
	pal_os_free(modulus_buffer);
	pal_os_free(bit_string_pb_key);

	return (return_status);
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Add the message padding, then do an RSA operation
 */
int mbedtls_rsa_pkcs1_encrypt( mbedtls_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t ilen,
                       const unsigned char *input,
                       unsigned char *output )
{
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output != NULL );
    RSA_VALIDATE_RET( input != NULL );

    switch( ctx->padding )
    {
#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsaes_pkcs1_v15_encrypt( ctx, f_rng, p_rng, mode, ilen,
                                                input, output );
#endif

#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsaes_oaep_encrypt( ctx, f_rng, p_rng, mode, NULL, 0,
                                           ilen, input, output );
#endif

        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}

#if defined(MBEDTLS_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-DECRYPT function
 */
int mbedtls_rsa_rsaes_oaep_decrypt( mbedtls_rsa_context *ctx,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng,
                            int mode,
                            const unsigned char *label, size_t label_len,
                            size_t *olen,
                            const unsigned char *input,
                            unsigned char *output,
                            size_t output_max_len )
{
    int ret;
    size_t ilen, i, pad_len;
    unsigned char *p, bad, pad_done;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    unsigned char lhash[MBEDTLS_MD_MAX_SIZE];
    unsigned int hlen;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output_max_len == 0 || output != NULL );
    RSA_VALIDATE_RET( label_len == 0 || label != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( olen != NULL );

    /*
     * Parameters sanity checks
     */
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    ilen = ctx->len;

    if( ilen < 16 || ilen > sizeof( buf ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    md_info = mbedtls_md_info_from_type( (mbedtls_md_type_t) ctx->hash_id );
    if( md_info == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    hlen = mbedtls_md_get_size( md_info );

    // checking for integer underflow
    if( 2 * hlen + 2 > ilen )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    /*
     * RSA operation
     */
    ret = ( mode == MBEDTLS_RSA_PUBLIC )
          ? mbedtls_rsa_public(  ctx, input, buf )
          : mbedtls_rsa_private( ctx, f_rng, p_rng, input, buf );

    if( ret != 0 )
        goto cleanup;

    /*
     * Unmask data and generate lHash
     */
    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
    {
        mbedtls_md_free( &md_ctx );
        goto cleanup;
    }

    /* seed: Apply seedMask to maskedSeed */
    if( ( ret = mgf_mask( buf + 1, hlen, buf + hlen + 1, ilen - hlen - 1,
                          &md_ctx ) ) != 0 ||
    /* DB: Apply dbMask to maskedDB */
        ( ret = mgf_mask( buf + hlen + 1, ilen - hlen - 1, buf + 1, hlen,
                          &md_ctx ) ) != 0 )
    {
        mbedtls_md_free( &md_ctx );
        goto cleanup;
    }

    mbedtls_md_free( &md_ctx );

    /* Generate lHash */
    if( ( ret = mbedtls_md( md_info, label, label_len, lhash ) ) != 0 )
        goto cleanup;

    /*
     * Check contents, in "constant-time"
     */
    p = buf;
    bad = 0;

    bad |= *p++; /* First byte must be 0 */

    p += hlen; /* Skip seed */

    /* Check lHash */
    for( i = 0; i < hlen; i++ )
        bad |= lhash[i] ^ *p++;

    /* Get zero-padding len, but always read till end of buffer
     * (minus one, for the 01 byte) */
    pad_len = 0;
    pad_done = 0;
    for( i = 0; i < ilen - 2 * hlen - 2; i++ )
    {
        pad_done |= p[i];
        pad_len += ((pad_done | (unsigned char)-pad_done) >> 7) ^ 1;
    }

    p += pad_len;
    bad |= *p++ ^ 0x01;

    /*
     * The only information "leaked" is whether the padding was correct or not
     * (eg, no data is copied if it was not correct). This meets the
     * recommendations in PKCS#1 v2.2: an opponent cannot distinguish between
     * the different error conditions.
     */
    if( bad != 0 )
    {
        ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
        goto cleanup;
    }

    if( ilen - ( p - buf ) > output_max_len )
    {
        ret = MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE;
        goto cleanup;
    }

    *olen = ilen - (p - buf);
    memcpy( output, p, *olen );
    ret = 0;

cleanup:
    mbedtls_platform_zeroize( buf, sizeof( buf ) );
    mbedtls_platform_zeroize( lhash, sizeof( lhash ) );

    return( ret );
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-DECRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_decrypt( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t *olen,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 size_t output_max_len )
{
    optiga_lib_status_t crypt_sync_status = OPTIGA_CRYPT_ERROR;
    optiga_crypt_t * me_crypt = NULL;
    int return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output_max_len == 0 || output != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( olen != NULL );

    me_crypt = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
    if (NULL == me_crypt)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }
    crypt_event_completed_status = OPTIGA_LIB_BUSY;
    crypt_sync_status = optiga_crypt_rsa_decrypt_and_export(me_crypt,
                                                            OPTIGA_RSAES_PKCS1_V15,
                                                            input,
                                                            ctx->len,
                                                            NULL,
                                                            0,
                                                            TRUSTM_RSA_PRIVATE_KEY_OID,
                                                            output,
                                                            (uint16_t *)&olen);
    if (OPTIGA_LIB_SUCCESS != crypt_sync_status)
    {
        goto cleanup;
    }

    //Wait until optiga_crypt_rsa_sign is completed
    while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
    {
        //Wait until the optiga_crypt_rsa_sign operation is completed
    	pal_os_timer_delay_in_milliseconds(5);
    }
    if (crypt_event_completed_status != OPTIGA_LIB_SUCCESS )
    {
        goto cleanup;
    }
    return_status = 0;
cleanup:
    // destroy crypt instances
    if (me_crypt != NULL)
    {
        (void)optiga_crypt_destroy(me_crypt);
    }

    return (return_status);
}
#endif /* MBEDTLS_PKCS1_V15 */
/*
 * Do an RSA operation, then remove the message padding
 */

int mbedtls_rsa_pkcs1_decrypt( mbedtls_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t *olen,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t output_max_len)
{
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output_max_len == 0 || output != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( olen != NULL );

    switch( ctx->padding )
    {

#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsaes_pkcs1_v15_decrypt( ctx, f_rng, p_rng, mode, olen,
                                                input, output, output_max_len );
#endif

#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsaes_oaep_decrypt( ctx, f_rng, p_rng, mode, NULL, 0,
                                           olen, input, output,
                                           output_max_len );
#endif

        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}

#if defined(MBEDTLS_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-SIGN function
 */
int mbedtls_rsa_rsassa_pss_sign( mbedtls_rsa_context *ctx,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         int mode,
                         mbedtls_md_type_t md_alg,
                         unsigned int hashlen,
                         const unsigned char *hash,
                         unsigned char *sig )
{
    size_t olen;
    unsigned char *p = sig;
    unsigned char salt[MBEDTLS_MD_MAX_SIZE];
    size_t slen, min_slen, hlen, offset = 0;
    int ret;
    size_t msb;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );
    RSA_VALIDATE_RET( sig != NULL );

    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    if( f_rng == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    olen = ctx->len;

    if( md_alg != MBEDTLS_MD_NONE )
    {
        /* Gather length of hash to sign */
        md_info = mbedtls_md_info_from_type( md_alg );
        if( md_info == NULL )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

        hashlen = mbedtls_md_get_size( md_info );
    }

    md_info = mbedtls_md_info_from_type( (mbedtls_md_type_t) ctx->hash_id );
    if( md_info == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    hlen = mbedtls_md_get_size( md_info );

    /* Calculate the largest possible salt length. Normally this is the hash
     * length, which is the maximum length the salt can have. If there is not
     * enough room, use the maximum salt length that fits. The constraint is
     * that the hash length plus the salt length plus 2 bytes must be at most
     * the key length. This complies with FIPS 186-4 §5.5 (e) and RFC 8017
     * (PKCS#1 v2.2) §9.1.1 step 3. */
    min_slen = hlen - 2;
    if( olen < hlen + min_slen + 2 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    else if( olen >= hlen + hlen + 2 )
        slen = hlen;
    else
        slen = olen - hlen - 2;

    memset( sig, 0, olen );

    /* Generate salt of length slen */
    if( ( ret = f_rng( p_rng, salt, slen ) ) != 0 )
        return( MBEDTLS_ERR_RSA_RNG_FAILED + ret );

    /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
    msb = mbedtls_mpi_bitlen( &ctx->N ) - 1;
    p += olen - hlen - slen - 2;
    *p++ = 0x01;
    memcpy( p, salt, slen );
    p += slen;

    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
        goto exit;

    /* Generate H = Hash( M' ) */
    if( ( ret = mbedtls_md_starts( &md_ctx ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_update( &md_ctx, p, 8 ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_update( &md_ctx, hash, hashlen ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_update( &md_ctx, salt, slen ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_finish( &md_ctx, p ) ) != 0 )
        goto exit;

    /* Compensate for boundary condition when applying mask */
    if( msb % 8 == 0 )
        offset = 1;

    /* maskedDB: Apply dbMask to DB */
    if( ( ret = mgf_mask( sig + offset, olen - hlen - 1 - offset, p, hlen,
                          &md_ctx ) ) != 0 )
        goto exit;

    msb = mbedtls_mpi_bitlen( &ctx->N ) - 1;
    sig[0] &= 0xFF >> ( olen * 8 - msb );

    p += hlen;
    *p++ = 0xBC;

    mbedtls_platform_zeroize( salt, sizeof( salt ) );

exit:
    mbedtls_md_free( &md_ctx );

    if( ret != 0 )
        return( ret );

    return( ( mode == MBEDTLS_RSA_PUBLIC )
            ? mbedtls_rsa_public(  ctx, sig, sig )
            : mbedtls_rsa_private( ctx, f_rng, p_rng, sig, sig ) );
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-V1_5-SIGN function
 */

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_rsassa_pkcs1_v15_sign( mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               int mode,
                               mbedtls_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               unsigned char *sig )
{
    optiga_rsa_signature_scheme_t signature_scheme;
    optiga_lib_status_t crypt_sync_status = OPTIGA_CRYPT_ERROR;
    optiga_crypt_t * me_crypt = NULL;
    uint8_t * signature_buffer = NULL;
    uint16_t  signature_len = 0;
    int return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    uint16_t modulus_length = 0;
    uint8_t digest_length = 0;

    // Input arguments check
    RSA_VALIDATE_RET(ctx != NULL);
    RSA_VALIDATE_RET(mode == MBEDTLS_RSA_PRIVATE || mode == MBEDTLS_RSA_PUBLIC);
    RSA_VALIDATE_RET((md_alg  == MBEDTLS_MD_NONE && hashlen == 0) || hash != NULL);
    RSA_VALIDATE_RET(sig != NULL);

    if(mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15)
        return(MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    // Create crypt instance
    me_crypt = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
    if (NULL == me_crypt)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    // Mapping mbedTLS signature scheme to TRUSTM
    return_status = mbedtls_rsa_get_sig_scheme_digest_len(md_alg, &signature_scheme, &digest_length);
    if (0 != return_status)
    {
        goto cleanup;
    }

    modulus_length = ctx->len;

    signature_buffer = pal_os_calloc(1,TRUSTM_RSA_SIGNATURE_LEN_MAX_SIZE);
    if (NULL == signature_buffer)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }
    signature_len = TRUSTM_RSA_SIGNATURE_LEN_MAX_SIZE;

    // Get the RSA signature length
    return_status = mbedtls_rsa_get_sig_len_key_type(modulus_length, &signature_len, NULL);
    if (0 != return_status)
    {
        goto cleanup;
    }

    // Invoke optiga_crypt_rsa_sign
    crypt_event_completed_status = OPTIGA_LIB_BUSY;
    crypt_sync_status = optiga_crypt_rsa_sign(me_crypt,
                                              signature_scheme,
                                              hash,
                                              digest_length,
                                              TRUSTM_RSA_PRIVATE_KEY_OID,
                                              signature_buffer,
                                              &signature_len,
                                              0);

    if (OPTIGA_LIB_SUCCESS != crypt_sync_status)
    {
        return_status = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;
    }

    //Wait until optiga_crypt_rsa_sign is completed
    while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
    {
        //Wait until the optiga_crypt_rsa_sign operation is completed
    	pal_os_timer_delay_in_milliseconds(5);
    }
    if (crypt_event_completed_status != OPTIGA_LIB_SUCCESS )
    {
        return_status = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;
    }

    // Check if signature length received is correct
    if (signature_len != modulus_length)
    {
        return_status = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;
    }

    pal_os_memcpy(sig, signature_buffer, modulus_length);
    return_status = 0;

cleanup:
    pal_os_free(signature_buffer);
    // destroy crypt instances
    if (me_crypt != NULL)
    {
        (void)optiga_crypt_destroy(me_crypt);
    }

    return (return_status);
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_pkcs1_sign( mbedtls_rsa_context *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t),
                    void *p_rng,
                    int mode,
                    mbedtls_md_type_t md_alg,
                    unsigned int hashlen,
                    const unsigned char *hash,
                    unsigned char *sig )
{
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );
    RSA_VALIDATE_RET( sig != NULL );

    switch( ctx->padding )
    {
#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsassa_pkcs1_v15_sign( ctx, f_rng, p_rng, mode, md_alg,
                                              hashlen, hash, sig );
#endif

#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsassa_pss_sign( ctx, f_rng, p_rng, mode, md_alg,
                                        hashlen, hash, sig );
#endif

        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}

#if defined(MBEDTLS_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int mbedtls_rsa_rsassa_pss_verify_ext( mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               int mode,
                               mbedtls_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               mbedtls_md_type_t mgf1_hash_id,
                               int expected_salt_len,
                               const unsigned char *sig )
{
    int ret;
    size_t siglen;
    unsigned char *p;
    unsigned char *hash_start;
    unsigned char result[MBEDTLS_MD_MAX_SIZE];
    unsigned char zeros[8];
    unsigned int hlen;
    size_t observed_salt_len, msb;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( sig != NULL );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );

    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    siglen = ctx->len;

    if( siglen < 16 || siglen > sizeof( buf ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    ret = ( mode == MBEDTLS_RSA_PUBLIC )
          ? mbedtls_rsa_public(  ctx, sig, buf )
          : mbedtls_rsa_private( ctx, f_rng, p_rng, sig, buf );

    if( ret != 0 )
        return( ret );

    p = buf;

    if( buf[siglen - 1] != 0xBC )
        return( MBEDTLS_ERR_RSA_INVALID_PADDING );

    if( md_alg != MBEDTLS_MD_NONE )
    {
        /* Gather length of hash to sign */
        md_info = mbedtls_md_info_from_type( md_alg );
        if( md_info == NULL )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

        hashlen = mbedtls_md_get_size( md_info );
    }

    md_info = mbedtls_md_info_from_type( mgf1_hash_id );
    if( md_info == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    hlen = mbedtls_md_get_size( md_info );

    memset( zeros, 0, 8 );

    /*
     * Note: EMSA-PSS verification is over the length of N - 1 bits
     */
    msb = mbedtls_mpi_bitlen( &ctx->N ) - 1;

    if( buf[0] >> ( 8 - siglen * 8 + msb ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    /* Compensate for boundary condition when applying mask */
    if( msb % 8 == 0 )
    {
        p++;
        siglen -= 1;
    }

    if( siglen < hlen + 2 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    hash_start = p + siglen - hlen - 1;

    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
        goto exit;

    ret = mgf_mask( p, siglen - hlen - 1, hash_start, hlen, &md_ctx );
    if( ret != 0 )
        goto exit;

    buf[0] &= 0xFF >> ( siglen * 8 - msb );

    while( p < hash_start - 1 && *p == 0 )
        p++;

    if( *p++ != 0x01 )
    {
        ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
        goto exit;
    }

    observed_salt_len = hash_start - p;

    if( expected_salt_len != MBEDTLS_RSA_SALT_LEN_ANY &&
        observed_salt_len != (size_t) expected_salt_len )
    {
        ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
        goto exit;
    }

    /*
     * Generate H = Hash( M' )
     */
    ret = mbedtls_md_starts( &md_ctx );
    if ( ret != 0 )
        goto exit;
    ret = mbedtls_md_update( &md_ctx, zeros, 8 );
    if ( ret != 0 )
        goto exit;
    ret = mbedtls_md_update( &md_ctx, hash, hashlen );
    if ( ret != 0 )
        goto exit;
    ret = mbedtls_md_update( &md_ctx, p, observed_salt_len );
    if ( ret != 0 )
        goto exit;
    ret = mbedtls_md_finish( &md_ctx, result );
    if ( ret != 0 )
        goto exit;

    if( memcmp( hash_start, result, hlen ) != 0 )
    {
        ret = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto exit;
    }

exit:
    mbedtls_md_free( &md_ctx );

    return( ret );
}

/*
 * Simplified PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int mbedtls_rsa_rsassa_pss_verify( mbedtls_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           int mode,
                           mbedtls_md_type_t md_alg,
                           unsigned int hashlen,
                           const unsigned char *hash,
                           const unsigned char *sig )
{
    mbedtls_md_type_t mgf1_hash_id;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( sig != NULL );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );

    mgf1_hash_id = ( ctx->hash_id != MBEDTLS_MD_NONE )
                             ? (mbedtls_md_type_t) ctx->hash_id
                             : md_alg;

    return( mbedtls_rsa_rsassa_pss_verify_ext( ctx, f_rng, p_rng, mode,
                                       md_alg, hashlen, hash,
                                       mgf1_hash_id, MBEDTLS_RSA_SALT_LEN_ANY,
                                       sig ) );

}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-v1_5-VERIFY function
 */

int mbedtls_rsa_rsassa_pkcs1_v15_verify( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode,
                                 mbedtls_md_type_t md_alg,
                                 unsigned int hashlen,
                                 const unsigned char *hash,
                                 const unsigned char *sig )
{


    optiga_rsa_signature_scheme_t signature_scheme;
    optiga_lib_status_t crypt_sync_status = OPTIGA_CRYPT_ERROR;
    public_key_from_host_t public_key;
    optiga_crypt_t * me_crypt = NULL;
    uint8_t * bit_string_pb_key = NULL;
    uint8_t * modulus_buffer = NULL;
    uint8_t public_exponent_buffer[4] = {0x00};
    uint16_t  signature_len = 0;
    int return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    uint16_t modulus_length = 0;
    uint16_t public_exponent_length = 4;
    uint8_t digest_length = 0;

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( sig != NULL );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );

    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    // Create crypt instance
    me_crypt = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
    if (NULL == me_crypt)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    // Mapping mbedTLS signature scheme to TRUSTM
    return_status = mbedtls_rsa_get_sig_scheme_digest_len(md_alg, &signature_scheme, &digest_length);
    if (0 != return_status)
    {
        goto cleanup;
    }

    bit_string_pb_key = pal_os_calloc(1,TRUSTM_RSA_PUBLIC_KEY_MAX_SIZE);
    if (NULL == bit_string_pb_key)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }
    modulus_buffer = pal_os_calloc(1,ctx->len);
    if (NULL == modulus_buffer)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    modulus_length = ctx->len;

    // Write the public key components
    return_status = mbedtls_mpi_write_binary(&(ctx->N), modulus_buffer, modulus_length);
    return_status |= mbedtls_mpi_write_binary(&(ctx->E), public_exponent_buffer, public_exponent_length);

    if (0 != return_status)
    {
        return_status = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    // Get the RSA key type and signature length
    return_status = mbedtls_rsa_get_sig_len_key_type(modulus_length, &signature_len, &(public_key.key_type));
    if (0 != return_status)
    {
        goto cleanup;
    }

    // RSA public key formation according to DER encoded format
    mbedtls_rsa_create_public_key_bit_string_format(modulus_buffer,
                                                    modulus_length,
                                                    public_exponent_buffer,
                                                    public_exponent_length,
                                                    bit_string_pb_key,
                                                    &public_key.length);

    public_key.public_key = bit_string_pb_key;

    crypt_event_completed_status = OPTIGA_LIB_BUSY;
    crypt_sync_status = optiga_crypt_rsa_verify(me_crypt,
                                                signature_scheme,
                                                hash,
                                                digest_length,
                                                sig,
                                                signature_len,
                                                OPTIGA_CRYPT_HOST_DATA,
                                                &public_key,
                                                0);

    if (OPTIGA_LIB_SUCCESS != crypt_sync_status)
    {
        return_status = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;
    }

    //Wait until optiga_crypt_rsa_verify is completed
    while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
    {
        //Wait until the optiga_crypt_rsa_verify operation is completed
    	pal_os_timer_delay_in_milliseconds(5);
    }
    if (crypt_event_completed_status != OPTIGA_LIB_SUCCESS )
    {
        return_status = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;

    }
    return_status = 0;
cleanup:

    pal_os_free(modulus_buffer);
    pal_os_free(bit_string_pb_key);
    // destroy crypt instances
    if (me_crypt != NULL)
    {
        (void)optiga_crypt_destroy(me_crypt);
    }

    return (return_status);
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation and check the message digest
 */
int mbedtls_rsa_pkcs1_verify( mbedtls_rsa_context *ctx,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng,
                      int mode,
                      mbedtls_md_type_t md_alg,
                      unsigned int hashlen,
                      const unsigned char *hash,
                      const unsigned char *sig )
{
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( sig != NULL );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );

    switch( ctx->padding )
    {
#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsassa_pkcs1_v15_verify( ctx, f_rng, p_rng, mode, md_alg,
                                                hashlen, hash, sig );
#endif

#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsassa_pss_verify( ctx, f_rng, p_rng, mode, md_alg,
                                          hashlen, hash, sig );
#endif

        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}

/*
 * Copy the components of an RSA key
 */
int mbedtls_rsa_copy( mbedtls_rsa_context *dst, const mbedtls_rsa_context *src )
{
    int ret;
    RSA_VALIDATE_RET( dst != NULL );
    RSA_VALIDATE_RET( src != NULL );

    dst->ver = src->ver;
    dst->len = src->len;

    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->N, &src->N ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->E, &src->E ) );

    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->D, &src->D ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->P, &src->P ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->Q, &src->Q ) );

#if !defined(MBEDTLS_RSA_NO_CRT)
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->DP, &src->DP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->DQ, &src->DQ ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->QP, &src->QP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->RP, &src->RP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->RQ, &src->RQ ) );
#endif

    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->RN, &src->RN ) );

    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->Vi, &src->Vi ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->Vf, &src->Vf ) );

    dst->padding = src->padding;
    dst->hash_id = src->hash_id;

cleanup:
    if( ret != 0 )
        mbedtls_rsa_free( dst );

    return( ret );
}

/*
 * Free the components of an RSA key
 */
void mbedtls_rsa_free( mbedtls_rsa_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_mpi_free( &ctx->Vi );
    mbedtls_mpi_free( &ctx->Vf );
    mbedtls_mpi_free( &ctx->RN );
    mbedtls_mpi_free( &ctx->D  );
    mbedtls_mpi_free( &ctx->Q  );
    mbedtls_mpi_free( &ctx->P  );
    mbedtls_mpi_free( &ctx->E  );
    mbedtls_mpi_free( &ctx->N  );

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi_free( &ctx->RQ );
    mbedtls_mpi_free( &ctx->RP );
    mbedtls_mpi_free( &ctx->QP );
    mbedtls_mpi_free( &ctx->DQ );
    mbedtls_mpi_free( &ctx->DP );
#endif /* MBEDTLS_RSA_NO_CRT */

#if defined(MBEDTLS_THREADING_C)
    mbedtls_mutex_free( &ctx->mutex );
#endif
}

#endif /* !MBEDTLS_RSA_ALT */

#endif /* MBEDTLS_RSA_C */
