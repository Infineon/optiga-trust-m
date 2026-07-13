/**
 * Copyright (c) 2026 Infineon Technologies AG
 *
 * SPDX-License-Identifier: MIT
 *
 * \file tf_psa_default_config.h
 *
 * \brief Configuration options for TF PSA (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 * 
 *  Reference : external/mbedtls-4.x/tf-psa-crypto/include/psa/crypto_config.h
 */

/**
 * \name SECTION: SECTION Cryptographic mechanism selection (PSA API)
 *
 * This section sets PSA API settings.
 * 
 * Uncomment for not using specific algorithm
 * \{
 */

// #undef PSA_WANT_ALG_CBC_NO_PADDING
// #undef PSA_WANT_ALG_CBC_PKCS7
// #undef PSA_WANT_ALG_CCM
// #undef PSA_WANT_ALG_CCM_STAR_NO_TAG
// #undef PSA_WANT_ALG_CMAC
// #undef PSA_WANT_ALG_CFB
#undef PSA_WANT_ALG_CHACHA20_POLY1305
// #undef PSA_WANT_ALG_CTR
// #undef PSA_WANT_ALG_DETERMINISTIC_ECDSA
// #undef PSA_WANT_ALG_ECB_NO_PADDING
// #undef PSA_WANT_ALG_ECDH
#undef PSA_WANT_ALG_FFDH
// #undef PSA_WANT_ALG_ECDSA
#undef PSA_WANT_ALG_JPAKE
// #undef PSA_WANT_ALG_GCM
// #undef PSA_WANT_ALG_HKDF
// #undef PSA_WANT_ALG_HKDF_EXTRACT
// #undef PSA_WANT_ALG_HKDF_EXPAND
// #undef PSA_WANT_ALG_HMAC
#undef PSA_WANT_ALG_MD5
// #undef PSA_WANT_ALG_OFB
#undef PSA_WANT_ALG_PBKDF2_HMAC
#undef PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128
#undef PSA_WANT_ALG_RIPEMD160
// #undef PSA_WANT_ALG_RSA_OAEP
// #undef PSA_WANT_ALG_RSA_PKCS1V15_CRYPT
// #undef PSA_WANT_ALG_RSA_PKCS1V15_SIGN
// #undef PSA_WANT_ALG_RSA_PSS
#undef PSA_WANT_ALG_SHA_1
#undef PSA_WANT_ALG_SHA_224
// #undef PSA_WANT_ALG_SHA_256
// #undef PSA_WANT_ALG_SHA_384
// #undef PSA_WANT_ALG_SHA_512
#undef PSA_WANT_ALG_SHA3_224
#undef PSA_WANT_ALG_SHA3_256
#undef PSA_WANT_ALG_SHA3_384
#undef PSA_WANT_ALG_SHA3_512
#undef PSA_WANT_ALG_STREAM_CIPHER
// #undef PSA_WANT_ALG_TLS12_PRF
// #undef PSA_WANT_ALG_TLS12_PSK_TO_MS
#undef PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS
#undef PSA_WANT_ALG_SHAKE128
#undef PSA_WANT_ALG_SHAKE256

#undef PSA_WANT_ECC_BRAINPOOL_P_R1_256
#undef PSA_WANT_ECC_BRAINPOOL_P_R1_384
#undef PSA_WANT_ECC_BRAINPOOL_P_R1_512
#undef PSA_WANT_ECC_MONTGOMERY_255
#undef PSA_WANT_ECC_MONTGOMERY_448
#undef PSA_WANT_ECC_SECP_K1_256
/* For secp256r1, consider enabling #MBEDTLS_PSA_P256M_DRIVER_ENABLED
 * (see the description in psa/crypto_config.h for details). */
// #undef PSA_WANT_ECC_SECP_R1_256
// #undef PSA_WANT_ECC_SECP_R1_384
// #undef PSA_WANT_ECC_SECP_R1_521

#undef PSA_WANT_DH_RFC7919_2048
#undef PSA_WANT_DH_RFC7919_3072
#undef PSA_WANT_DH_RFC7919_4096
#undef PSA_WANT_DH_RFC7919_6144
#undef PSA_WANT_DH_RFC7919_8192

// #undef PSA_WANT_KEY_TYPE_DERIVE
#undef PSA_WANT_KEY_TYPE_PASSWORD
#undef PSA_WANT_KEY_TYPE_PASSWORD_HASH
// #undef PSA_WANT_KEY_TYPE_HMAC
// #undef PSA_WANT_KEY_TYPE_AES
#undef PSA_WANT_KEY_TYPE_ARIA
#undef PSA_WANT_KEY_TYPE_CAMELLIA
#undef PSA_WANT_KEY_TYPE_CHACHA20
// #undef PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY
#undef PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY
// #undef PSA_WANT_KEY_TYPE_RAW_DATA 
// #undef PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY

/*
 * The following symbols extend and deprecate the legacy
 * PSA_WANT_KEY_TYPE_xxx_KEY_PAIR ones. They include the usage of that key in
 * the name's suffix. "_USE" is the most generic and it can be used to describe
 * a generic suport, whereas other ones add more features on top of that and
 * they are more specific.
 */
// #undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC
// #undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT
// #undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT
// #undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE
// #undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

// #undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
// #undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT
// #undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT
// #undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE
//#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE /* Not supported */

#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE
//#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE /* Not supported */
/** \} name SECTION Cryptographic mechanism selection (PSA API) */

/**
 * \name SECTION: Platform abstraction layer
 *
 * This section sets platform specific settings.
 * \{
 */

/**
 * \def MBEDTLS_MEMORY_BUFFER_ALLOC_C
 *
 * Enable the buffer allocator implementation that makes use of a (stack)
 * based buffer to 'allocate' dynamic memory. (replaces calloc() and free()
 * calls)
 *
 * Requires: MBEDTLS_PLATFORM_C, MBEDTLS_PLATFORM_MEMORY (to use it within Mbed TLS)
 */
//#define MBEDTLS_MEMORY_BUFFER_ALLOC_C

 /**
 * \def MBEDTLS_FS_IO
 *
 * Enable functions that use the filesystem.
 */
#undef MBEDTLS_FS_IO

/**
 * \def MBEDTLS_HAVE_TIME
 *
 * System has time.h and time().
 * The time does not need to be correct, only time differences are used,
 * by contrast with MBEDTLS_HAVE_TIME_DATE.
 *
 * Comment if your system does not support time functions.
 */
#undef MBEDTLS_HAVE_TIME

/**
 * \def MBEDTLS_HAVE_TIME_DATE
 *
 * System has time.h, time(), and an implementation for
 * mbedtls_platform_gmtime_r() (see platform_util.h).
 * Used to verify X.509 certificate validity periods.
 */
#undef MBEDTLS_HAVE_TIME_DATE

/**
 * \def MBEDTLS_MEMORY_DEBUG
 *
 * Enable debugging of buffer allocator memory issues.
 *
 * Requires: MBEDTLS_MEMORY_BUFFER_ALLOC_C
 */
//#define MBEDTLS_MEMORY_DEBUG

/**
 * \def MBEDTLS_MEMORY_BACKTRACE
 *
 * Include backtrace information with each allocated block.
 *
 * Requires: MBEDTLS_MEMORY_BUFFER_ALLOC_C
 *           GLIBC-compatible backtrace() and backtrace_symbols() support
 */
//#define MBEDTLS_MEMORY_BACKTRACE

/**
 * \def MBEDTLS_PLATFORM_C
 *
 * Enable the platform abstraction layer that allows re-assigning functions
 * like calloc(), free(), snprintf(), printf(), fprintf(), exit().
 *
 * \note This abstraction layer must be enabled on Windows (including MSYS2).
 */
#define MBEDTLS_PLATFORM_C

/*
 * MBEDTLS_PLATFORM_XXX_ALT: Uncomment a macro to let Mbed TLS support the
 * function in the platform abstraction layer. Requires MBEDTLS_PLATFORM_C.
 * Cannot be defined together with MBEDTLS_PLATFORM_XXX_MACRO.
 */
//#define MBEDTLS_PLATFORM_SETBUF_ALT
//#define MBEDTLS_PLATFORM_EXIT_ALT
//#define MBEDTLS_PLATFORM_TIME_ALT
//#define MBEDTLS_PLATFORM_FPRINTF_ALT
//#define MBEDTLS_PLATFORM_PRINTF_ALT
//#define MBEDTLS_PLATFORM_SNPRINTF_ALT
//#define MBEDTLS_PLATFORM_VSNPRINTF_ALT
//#define MBEDTLS_PLATFORM_NV_SEED_ALT
//#define MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT
//#define MBEDTLS_PLATFORM_MS_TIME_ALT

/**
 * \def MBEDTLS_PLATFORM_GMTIME_R_ALT
 *
 * Use an alternate implementation of mbedtls_platform_gmtime_r() supplied at
 * compile time. Bypasses the default detection in platform_util.c.
 */
//#define MBEDTLS_PLATFORM_GMTIME_R_ALT

/**
 * \def MBEDTLS_PLATFORM_MEMORY
 *
 * Enable the memory allocation layer.
 *
 * Requires: MBEDTLS_PLATFORM_C
 */
//#define MBEDTLS_PLATFORM_MEMORY

/**
 * \def MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
 *
 * Do not assign standard functions in the platform layer.
 *
 * Requires: MBEDTLS_PLATFORM_C
 */
//#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

/**
 * \def MBEDTLS_PLATFORM_ZEROIZE_ALT
 *
 * Use an alternate implementation of mbedtls_platform_zeroize() to wipe
 * sensitive data in memory.
 */
//#define MBEDTLS_PLATFORM_ZEROIZE_ALT

/**
 * \def MBEDTLS_THREADING_ALT
 *
 * Provide your own alternate implementation of threading primitives.
 *
 * Requires: MBEDTLS_THREADING_C
 */
//#define MBEDTLS_THREADING_ALT

/**
 * \def MBEDTLS_THREADING_PTHREAD
 *
 * Enable the pthread wrapper layer for the threading layer.
 *
 * Requires: MBEDTLS_THREADING_C
 */
//#define MBEDTLS_THREADING_PTHREAD

/**
 * \def MBEDTLS_THREADING_C
 *
 * Enable the threading abstraction layer. Must be enabled in multithreaded
 * environments. Enable either MBEDTLS_THREADING_ALT or MBEDTLS_THREADING_PTHREAD.
 */
//#define MBEDTLS_THREADING_C

/* Memory buffer allocator options */
//#define MBEDTLS_MEMORY_ALIGN_MULTIPLE      4

/*
 * MBEDTLS_PLATFORM_XXX_MACRO options. Requires MBEDTLS_PLATFORM_C.
 * MBEDTLS_PLATFORM_XXX_MACRO and MBEDTLS_PLATFORM_XXX_ALT cannot both be defined.
 */
//#define MBEDTLS_PLATFORM_CALLOC_MACRO        calloc
//#define MBEDTLS_PLATFORM_EXIT_MACRO            exit
//#define MBEDTLS_PLATFORM_FREE_MACRO            free
//#define MBEDTLS_PLATFORM_FPRINTF_MACRO      fprintf
//#define MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO   int64_t
//#define MBEDTLS_PLATFORM_NV_SEED_READ_MACRO   mbedtls_platform_std_nv_seed_read
//#define MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO  mbedtls_platform_std_nv_seed_write
//#define MBEDTLS_PLATFORM_PRINTF_MACRO        printf
//#define MBEDTLS_PLATFORM_SETBUF_MACRO      setbuf
//#define MBEDTLS_PLATFORM_SNPRINTF_MACRO    snprintf

/* Default standard-function overrides */
//#define MBEDTLS_PLATFORM_STD_CALLOC        calloc
//#define MBEDTLS_PLATFORM_STD_EXIT            exit
//#define MBEDTLS_PLATFORM_STD_EXIT_FAILURE       1
//#define MBEDTLS_PLATFORM_STD_EXIT_SUCCESS       0
//#define MBEDTLS_PLATFORM_STD_FPRINTF      fprintf
//#define MBEDTLS_PLATFORM_STD_FREE            free
//#define MBEDTLS_PLATFORM_STD_MEM_HDR   <stdlib.h>
//#define MBEDTLS_PLATFORM_STD_NV_SEED_FILE  "seedfile"
//#define MBEDTLS_PLATFORM_STD_NV_SEED_READ   mbedtls_platform_std_nv_seed_read
//#define MBEDTLS_PLATFORM_STD_NV_SEED_WRITE  mbedtls_platform_std_nv_seed_write
//#define MBEDTLS_PLATFORM_STD_PRINTF        printf
//#define MBEDTLS_PLATFORM_STD_SETBUF      setbuf
//#define MBEDTLS_PLATFORM_STD_SNPRINTF    snprintf
//#define MBEDTLS_PLATFORM_STD_TIME            time
//#define MBEDTLS_PLATFORM_TIME_MACRO            time
//#define MBEDTLS_PLATFORM_TIME_TYPE_MACRO       time_t
//#define MBEDTLS_PLATFORM_VSNPRINTF_MACRO    vsnprintf
//#define MBEDTLS_PRINTF_MS_TIME    PRId64

/**
 * \def MBEDTLS_PLATFORM_DEV_RANDOM
 *
 * Path to a special file that returns cryptographic-quality random bytes
 * when read. Used by the default platform entropy source on non-Windows
 * platforms unless a dedicated system call is available.
 */
//#define MBEDTLS_PLATFORM_DEV_RANDOM "/dev/random"

/** \} name SECTION: Platform abstraction layer */

/**
 * \name SECTION: General and test configuration options
 *
 * This section sets test specific settings.
 * \{
 */

/**
 * \def MBEDTLS_CHECK_RETURN_WARNING
 *
 * Emit a compile-time warning if application code calls a function without
 * checking its return value.
 */
//#define MBEDTLS_CHECK_RETURN_WARNING

/**
 * \def MBEDTLS_DEPRECATED_WARNING
 *
 * Mark deprecated functions and features so that they generate a warning if
 * used. Only works with GCC and Clang.
 */
//#define MBEDTLS_DEPRECATED_WARNING

/**
 * \def MBEDTLS_DEPRECATED_REMOVED
 *
 * Remove deprecated functions and features so that they generate an error if
 * used.
 */
//#define MBEDTLS_DEPRECATED_REMOVED

/** \def MBEDTLS_CHECK_RETURN
 *
 * Attribute used at the beginning of the declaration of a function to
 * indicate that its return value should be checked.
 */
//#define MBEDTLS_CHECK_RETURN __attribute__((__warn_unused_result__))

/** \def MBEDTLS_IGNORE_RETURN
 *
 * Suppress a MBEDTLS_CHECK_RETURN warning for a specific call.
 */
//#define MBEDTLS_IGNORE_RETURN( result ) ((void) !(result))

/**
 * \def TF_PSA_CRYPTO_CONFIG_FILE
 *
 * If defined, this is a header which will be included instead of
 * `"psa/crypto_config.h"`. It specifies which cryptographic mechanisms are
 * available through the PSA API.
 */
//#define TF_PSA_CRYPTO_CONFIG_FILE "psa/crypto_config.h"

/**
 * \def TF_PSA_CRYPTO_USER_CONFIG_FILE
 *
 * If defined, this is a header which will be included after
 * `"psa/crypto_config.h"` or #TF_PSA_CRYPTO_CONFIG_FILE.
 * This allows you to modify the default configuration, including the ability
 * to undefine options that are enabled by default.
 *
 * This macro is expanded after an <tt>\#include</tt> directive. This is a popular but
 * non-standard feature of the C language, so this feature is only available
 * with compilers that perform macro expansion on an <tt>\#include</tt> line.
 *
 * The value of this symbol is typically a path in double quotes, either
 * absolute or relative to a directory on the include search path.
 */
//#undef TF_PSA_CRYPTO_USER_CONFIG_FILE "/dev/null"

/**
 * \def MBEDTLS_SELF_TEST
 *
 * Enable the checkup functions (*_self_test).
 */
#undef MBEDTLS_SELF_TEST

/**
 * \def MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN
 *
 * Enable testing of the constant-flow nature of some sensitive functions
 * with clang's MemorySanitizer.
 *
 * \warning Not part of the library's API. Extended testing only.
 */
//#define MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN

/**
 * \def MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND
 *
 * Enable testing of the constant-flow nature of some sensitive functions
 * with valgrind's memcheck tool.
 *
 * \warning Not part of the library's API. Extended testing only.
 */
//#define MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND

/**
 * \def MBEDTLS_TEST_HOOKS
 *
 * Enable features for invasive testing such as introspection functions and
 * hooks for fault injection. Must never be enabled in production.
 */
//#define MBEDTLS_TEST_HOOKS

 /**
 * \def TF_PSA_CRYPTO_VERSION
 *
 * Enable run-time version information.
 *
 * This option enables functions for getting the version of TF-PSA-Crypto
 * at runtime defined in include/tf-psa-crypto/version.h.
 */
#undef TF_PSA_CRYPTO_VERSION

 /** \} name SECTION: General and test configuration options */

 /**
 * \name SECTION: Cryptographic mechanism selection (extended API)
 *
 * This section sets cryptographic mechanism settings.
 * \{
 */

/**
 * \def MBEDTLS_LMS_C
 *
 * Enable the LMS stateful-hash asymmetric signature algorithm.
 *
 * Module:  extras/lms.c
 * Caller:
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C
 *
 * Uncomment to enable the LMS verification algorithm and public key operations.
 */
#undef MBEDTLS_LMS_C

/**
 * \def MBEDTLS_LMS_PRIVATE
 *
 * Enable LMS private-key operations and signing code. Experimental, not for
 * production use.
 *
 * Requires: MBEDTLS_LMS_C
 */
//#define MBEDTLS_LMS_PRIVATE

/**
 * \def MBEDTLS_MD_C
 *
 * Enable the generic layer for message digest (hashing).
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C with at least one hash.
 * Module:  extras/md.c
 */
// #undef MBEDTLS_MD_C

/**
 * \def MBEDTLS_NIST_KW_C
 *
 * Enable the 128-bit key wrapping modes from NIST SP 800-38F:
 * KW (also known as RFC 3394) and KWP (RFC 5649).
 * Currently these modes are only supported with AES.
 *
 * Module:  extras/nist_kw.c
 *
 * Auto enables: PSA_WANT_ALG_ECB_NO_PADDING
 */
#undef MBEDTLS_NIST_KW_C

/**
 * \def MBEDTLS_PKCS5_C
 *
 * Enable PKCS#5 functions.
 *
 * Module:  utilities/pkcs5.c
 *
 * Auto-enables: MBEDTLS_MD_C
 *
 * This module adds support for the PKCS#5 functions.
 */
#undef MBEDTLS_PKCS5_C

/**
 * \def MBEDTLS_PK_C
 *
 * Enable the generic public (asymmetric) key layer.
 *
 * Module:  extras/pk.c
 * Requires: MBEDTLS_PSA_CRYPTO_CLIENT and at least one between
 *           PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY and PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY.
 */
// #undef MBEDTLS_PK_C

/**
 * \def MBEDTLS_PK_PARSE_C
 *
 * Enable the generic public (asymmetric) key parser.
 *
 * Module:  extras/pkparse.c
 * Requires: MBEDTLS_ASN1_PARSE_C, MBEDTLS_PK_C
 */
// #undef MBEDTLS_PK_PARSE_C

/**
 * \def MBEDTLS_PK_PARSE_EC_EXTENDED
 *
 * Enhance support for reading EC keys using variants of SEC1 not allowed by
 * RFC 5915 and RFC 5480.
 *
 * Currently this means parsing the SpecifiedECDomain choice of EC
 * parameters (only known groups are supported, not arbitrary domains, to
 * avoid validation issues).
 *
 * Disable if you only need to support RFC 5915 + 5480 key formats.
 */
// #undef MBEDTLS_PK_PARSE_EC_EXTENDED

/**
 * \def MBEDTLS_PK_PARSE_EC_COMPRESSED
 *
 * Enable the support for parsing public keys of type Short Weierstrass
 * (PSA_ECC_FAMILY_SECP_XXX and PSA_ECC_FAMILY_BRAINPOOL_XXX) which are using the
 * compressed point format.
 */
// #undef MBEDTLS_PK_PARSE_EC_COMPRESSED

/**
 * \def MBEDTLS_PK_WRITE_C
 *
 * Enable the generic public (asymmetric) key writer.
 *
 * Module:  extras/pkwrite.c
 *
 * Requires: MBEDTLS_ASN1_WRITE_C, MBEDTLS_PK_C
 *
 * Uncomment to enable generic public key write functions.
 */
// #undef MBEDTLS_PK_WRITE_C

 /** \} name SECTION: Cryptographic mechanism selection (extended API) */

 /**
 * \name SECTION: Data format support
 *
 * This section sets data-format specific settings.
 * \{
 */

/**
 * \def MBEDTLS_ASN1_PARSE_C
 *
 * Enable the generic ASN1 parser.
 *
 * Module:  utilities/asn1parse.c
 * Caller:  extras/pkparse.c
 *          utilities/pkcs5.c
 */
#define MBEDTLS_ASN1_PARSE_C

/**
 * \def MBEDTLS_ASN1_WRITE_C
 *
 * Enable the generic ASN1 writer.
 *
 * Module:  utilities/asn1write.c
 * Caller:  drivers/builtin/src/ecdsa.c
 *          extras/pkwrite.c
 */
#define MBEDTLS_ASN1_WRITE_C

/**
 * \def MBEDTLS_BASE64_C
 *
 * Enable the Base64 module.
 *
 * Module:  utilities/base64.c
 * Caller:  utilities/pem.c
 *
 * This module is required for PEM support (required by X.509).
 */
#define MBEDTLS_BASE64_C

/**
 * \def MBEDTLS_PEM_PARSE_C
 *
 * Enable PEM decoding / parsing.
 *
 * Module:  utilities/pem.c
 * Caller:  extras/pkparse.c
 *
 * Requires: MBEDTLS_BASE64_C
 *           optionally PSA_WANT_ALG_MD5
 *
 * This modules adds support for decoding / parsing PEM files.
 */
#define MBEDTLS_PEM_PARSE_C

/**
 * \def MBEDTLS_PEM_WRITE_C
 *
 * Enable PEM encoding / writing.
 *
 * Module:  utilities/pem.c
 * Caller:  extras/pkwrite.c
 *
 * Requires: MBEDTLS_BASE64_C
 *
 * This modules adds support for encoding / writing PEM files.
 */
#undef MBEDTLS_PEM_WRITE_C

 /** \} name SECTION: Data format support */

 /**
 * \name SECTION: PSA core
 *
 * This section sets PSA specific settings.
 * \{
 */

/**
 * \def MBEDTLS_CTR_DRBG_C
 *
 * Enable the CTR_DRBG AES-based random generator (AES-256 by default; set
 * MBEDTLS_PSA_CRYPTO_RNG_STRENGTH to 128 for AES-128).
 *
 * Module:  drivers/builtin/src/ctr_drbg.c
 * Requires: MBEDTLS_PSA_CRYPTO_C, PSA_WANT_KEY_TYPE_AES and
 *           PSA_WANT_ALG_ECB_NO_PADDING
 */
// #undef MBEDTLS_CTR_DRBG_C

/**
 * \def MBEDTLS_ENTROPY_NO_SOURCES_OK
 *
 * Allow the library to be built without any "true" entropy source. Only
 * meaningful together with MBEDTLS_ENTROPY_NV_SEED.
 */
//#define MBEDTLS_ENTROPY_NO_SOURCES_OK

/**
 * \def MBEDTLS_ENTROPY_NV_SEED
 *
 * Enable the non-volatile (NV) seed file-based entropy source.
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C, !MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG,
 *           MBEDTLS_PLATFORM_C
 */
//#define MBEDTLS_ENTROPY_NV_SEED

/**
 * \def MBEDTLS_HMAC_DRBG_C
 *
 * Enable the HMAC_DRBG random generator.
 *
 * Module:  drivers/builtin/src/hmac_drbg.c
 * Caller:
 *
 * Requires: MBEDTLS_MD_C
 *
 * Uncomment to enable the HMAC_DRBG random number generator.
 */
#undef MBEDTLS_HMAC_DRBG_C

/**
 * \def MBEDTLS_PSA_CRYPTO_C
 *
 * Enable the Platform Security Architecture cryptography API.
 *
 * Module:  core/psa_crypto.c
 *
 * Requires: one of the following:
 *           - MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
 *           - MBEDTLS_CTR_DRBG_C
 *           - MBEDTLS_HMAC_DRBG_C
 *
 *           If MBEDTLS_CTR_DRBG_C or MBEDTLS_HMAC_DRBG_C is used as the PSA
 *           random generator, then either PSA_WANT_ALG_SHA_256 or
 *           PSA_WANT_ALG_SHA_512 must be enabled for the entropy module.
 *
 * \note The PSA crypto subsystem prioritizes DRBG mechanisms as follows:
 *       - #MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG, if enabled
 *       - CTR_DRBG (AES), seeded by the entropy module, if
 *         #MBEDTLS_CTR_DRBG_C is enabled
 *       - HMAC_DRBG, seeded by the entropy module, if
 *         #MBEDTLS_HMAC_DRBG_C is enabled
 *
 *       A future version may reevaluate the prioritization of DRBG mechanisms.
 */
#define MBEDTLS_PSA_CRYPTO_C

/**
 * \def MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
 *
 * Assume all buffers passed to PSA functions are owned exclusively by the
 * PSA function and are not stored in shared memory. Reduces code/memory
 * footprint but disables input/output buffer overlap.
 */
//#define MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS

/** \def MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
 *
 * Enable support for platform built-in keys.
 * Requires the application to implement mbedtls_psa_platform_get_builtin_key().
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C
 * \warning Experimental interface.
 */
//#define MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS

/** \def MBEDTLS_PSA_CRYPTO_CLIENT
 *
 * Enable support for PSA crypto client (attribute get/set code) without the
 * full PSA implementation (MBEDTLS_PSA_CRYPTO_C disabled).
 *
 * \warning Experimental interface.
 */
//#define MBEDTLS_PSA_CRYPTO_CLIENT

/**
 * \def MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
 *
 * Make the PSA Crypto subsystem use an external random generator provided
 * by a driver, instead of Mbed TLS's built-in entropy + DRBG.
 */
// #define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

/**
 * \def MBEDTLS_PSA_BUILTIN_GET_ENTROPY
 *
 * Enable entropy sources for which the library has a built-in driver.
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C, !MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
 *
 * These are:
 * - getrandom() on Linux (if syscall() is available at compile time);
 * - getrandom() on FreeBSD and DragonFlyBSD (if available at compile time);
 * - `sysctl(KERN_ARND)` on FreeBSD and NetBSD;
 * - #MBEDTLS_PLATFORM_DEV_RANDOM on Unix-like platforms (unless one of the
 *   above is used);
 * - BCryptGenRandom() on Windows.
 *
 * You should enable this option if your platform has one of these. If not:
 *
 * - You can enable #MBEDTLS_PSA_DRIVER_GET_ENTROPY instead, and provide
 *   an entropy source callback for your platform.
 * - If your platform has a fast cryptographic-quality random generator,
 *   enable #MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG and provide a random generator
 *   callback instead.
 * - If your platform has no source of entropy at all, you can enable
 *   #MBEDTLS_ENTROPY_NV_SEED and provide a seed in nonvolatile memory
 *   during the provisioning of the device.
 * - The random generator requires a random generator callback,
 *   an entropy source or a seed in nonvolatile memory.
 *   Builds with no random generator are not officially supported yet, except
 *   client-only builds (#MBEDTLS_PSA_CRYPTO_CLIENT enabled and
 *   #MBEDTLS_PSA_CRYPTO_C disabled).
 */
// #undef MBEDTLS_PSA_BUILTIN_GET_ENTROPY

/* MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
 *
 * Enable key identifiers that encode a key owner identifier (int32_t).
 * Internal use only; may be removed without notice.
 */
//#define MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER

/**
 * \def MBEDTLS_PSA_CRYPTO_SPM
 *
 * Build the code for SPM (Secure Partition Manager) integration, separating
 * NSPE and SPE. Requires a "crypto_spe.h" header on the include path.
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C
 */
//#define MBEDTLS_PSA_CRYPTO_SPM

/**
 * \def MBEDTLS_PSA_CRYPTO_STORAGE_C
 *
 * Enable the Platform Security Architecture persistent key storage.
 *
 * Module:  core/psa_crypto_storage.c
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C,
 *           either MBEDTLS_PSA_ITS_FILE_C or a native implementation of
 *           the PSA ITS interface
 */
#undef MBEDTLS_PSA_CRYPTO_STORAGE_C

/**
 * \def MBEDTLS_PSA_DRIVER_GET_ENTROPY
 *
 * Enable the custom entropy callback mbedtls_platform_get_entropy()
 * (declared in mbedtls/platform.h). Provide this callback if the built-in
 * entropy callback (MBEDTLS_PSA_BUILTIN_GET_ENTROPY) does not work on your
 * platform.
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C, !MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
 * Cannot be enabled together with MBEDTLS_PSA_BUILTIN_GET_ENTROPY.
 */
//#define MBEDTLS_PSA_DRIVER_GET_ENTROPY

/**
 * \def MBEDTLS_PSA_ITS_FILE_C
 *
 * Enable the emulation of the Platform Security Architecture
 * Internal Trusted Storage (PSA ITS) over files.
 *
 * Module:  core/psa_its_file.c
 *
 * Requires: MBEDTLS_FS_IO
 */
#undef MBEDTLS_PSA_ITS_FILE_C

/**
 * \def MBEDTLS_PSA_KEY_STORE_DYNAMIC
 *
 * Dynamically resize the PSA key store to accommodate any number of
 * volatile keys (until the heap memory is exhausted).
 *
 * If this option is disabled, the key store has a fixed size
 * #MBEDTLS_PSA_KEY_SLOT_COUNT for volatile keys and loaded persistent keys
 * together.
 *
 * This option has no effect when #MBEDTLS_PSA_CRYPTO_C is disabled.
 *
 * Module:  core/psa_crypto.c
 * Requires: MBEDTLS_PSA_CRYPTO_C
 */
#undef MBEDTLS_PSA_KEY_STORE_DYNAMIC

/**
 * \def MBEDTLS_PSA_STATIC_KEY_SLOTS
 *
 * Statically preallocate memory to store keys' material in PSA instead of
 * allocating it dynamically. Allows heap-less builds.
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C
 */
//#define MBEDTLS_PSA_STATIC_KEY_SLOTS

/* Entropy options */

/**
 * \def MBEDTLS_PSA_CRYPTO_PLATFORM_FILE
 *
 * If defined, this header is included instead of `"psa/crypto_platform.h"`.
 */
//#define MBEDTLS_PSA_CRYPTO_PLATFORM_FILE "psa/crypto_platform_alt.h"

/**
 * \def MBEDTLS_PSA_CRYPTO_STRUCT_FILE
 *
 * If defined, this header is included instead of `"psa/crypto_struct.h"`.
 * Typically used in client-server PSA integrations to override operation
 * structure definitions.
 */
//#define MBEDTLS_PSA_CRYPTO_STRUCT_FILE "psa/crypto_struct_alt.h"

/** \def MBEDTLS_PSA_KEY_SLOT_COUNT
 *
 * When MBEDTLS_PSA_KEY_STORE_DYNAMIC is disabled, the maximum amount of PSA
 * keys simultaneously in memory. When enabled, the maximum number of loaded
 * persistent keys.
 */
//#define MBEDTLS_PSA_KEY_SLOT_COUNT 32

/**
 * \def MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE
 *
 * Size (in bytes) of each static key buffer when MBEDTLS_PSA_STATIC_KEY_SLOTS
 * is set. Autoderived from enabled PSA keys if not defined.
 */
//#define MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE       256

/**
 * \def MBEDTLS_PSA_CRYPTO_RNG_STRENGTH
 *
 * Minimum security strength (in bits) of the PSA RNG.
 * Valid values: 128 or default of 256.
 */
//#define MBEDTLS_PSA_CRYPTO_RNG_STRENGTH               256

/**
 * \def MBEDTLS_PSA_CRYPTO_RNG_HASH
 *
 * Hash algorithm to use for the entropy module and for HMAC_DRBG if
 * configured. Hash size must be at least MBEDTLS_PSA_CRYPTO_RNG_STRENGTH.
 * Currently allowed values: PSA_ALG_SHA_256 and PSA_ALG_SHA_512.
 */
//#define MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256

/**
 * \def MBEDTLS_PSA_RNG_RESEED_INTERVAL
 *
 * In CTR_DRBG and HMAC_DRBG, the interval before the DRBG is reseeded from
 * entropy. Counted in number of requests to the random generator.
 */
//#define MBEDTLS_PSA_RNG_RESEED_INTERVAL 1000

 /** \} name SECTION: PSA core */

 /**
 * \name SECTION: Builtin drivers
 *
 * This section sets driver specific settings.
 * \{
 */

/**
 * \def MBEDTLS_AESNI_C
 *
 * Enable AES-NI support on x86-64 or x86-32.
 *
 * \note AESNI is only supported with certain compilers and target options:
 * - Visual Studio: supported
 * - GCC, x86-64, target not explicitly supporting AESNI:
 *   requires MBEDTLS_HAVE_ASM.
 * - GCC, x86-32, target not explicitly supporting AESNI:
 *   not supported.
 * - GCC, x86-64 or x86-32, target supporting AESNI: supported.
 *   For this assembly-less implementation, you must currently compile
 *   `drivers/builtin/src/aesni.c` and `drivers/builtin/src/aes.c` with machine
 *   options to enable SSE2 and AESNI instructions: `gcc -msse2 -maes -mpclmul`
 *   or `clang -maes -mpclmul`.
 * - Non-x86 targets: this option is silently ignored.
 * - Other compilers: this option is silently ignored.
 *
 * \note
 * Above, "GCC" includes compatible compilers such as Clang.
 * The limitations on target support are likely to be relaxed in the future.
 *
 * Module:  drivers/builtin/src/aesni.c
 * Caller:  drivers/builtin/src/aes.c
 *
 * Requires: MBEDTLS_HAVE_ASM (on some platforms, see note)
 *
 * This modules adds support for the AES-NI instructions on x86.
 */
#undef MBEDTLS_AESNI_C

/**
 * \def MBEDTLS_AESCE_C
 *
 * Enable AES cryptographic extension support on Armv8.
 *
 * Module:  drivers/builtin/src/aesce.c
 * Caller:  drivers/builtin/src/aes.c
 *
 * Requires: The AES built-in implementation
 *
 * \warning Runtime detection only works on Linux. For non-Linux operating
 *          system, Armv8-A Cryptographic Extensions must be supported by
 *          the CPU when this option is enabled.
 *
 * \note    Minimum compiler versions for this feature when targeting aarch64
 *          are Clang 4.0; armclang 6.6; GCC 6.0; or MSVC 2019 version 16.11.2.
 *          Minimum compiler versions for this feature when targeting 32-bit
 *          Arm or Thumb are Clang 11.0; armclang 6.20; or GCC 6.0.
 *
 * \note \c CFLAGS must be set to a minimum of \c -march=armv8-a+crypto for
 * armclang <= 6.9
 *
 * This module adds support for the AES Armv8-A Cryptographic Extensions on Armv8 systems.
 */
#undef MBEDTLS_AESCE_C

/**
 * \def MBEDTLS_AES_ROM_TABLES
 *
 * Use precomputed AES tables stored in ROM instead of generating them in
 * RAM at runtime. Trade-off between RAM and ROM footprint.
 */
//#define MBEDTLS_AES_ROM_TABLES

/**
 * \def MBEDTLS_AES_FEWER_TABLES
 *
 * Use less ROM/RAM for AES tables (~6kB less) at the cost of more arithmetic
 * per operation. Independent of MBEDTLS_AES_ROM_TABLES.
 */
//#define MBEDTLS_AES_FEWER_TABLES

/**
 * \def MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
 *
 * Use only 128-bit keys in AES operations to save ROM (~300 bytes on Cortex-M).
 * Removes support for 192- and 256-bit keys.
 *
 * Requires: The AES built-in implementation.
 */
//#define MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH

/*
 * Disable plain C implementation for AES; use only hardware acceleration.
 * Requires at least one of MBEDTLS_AESCE_C or MBEDTLS_AESNI_C enabled.
 */
//#define MBEDTLS_AES_USE_HARDWARE_ONLY

/**
 * \def MBEDTLS_BLOCK_CIPHER_NO_DECRYPT
 *
 * Remove decryption operation for AES, ARIA and Camellia block cipher.
 * Incompatible with PSA_WANT_ALG_ECB_NO_PADDING, PSA_WANT_ALG_CBC_NO_PADDING,
 * PSA_WANT_ALG_CBC_PKCS7 and MBEDTLS_NIST_KW_C.
 */
//#define MBEDTLS_BLOCK_CIPHER_NO_DECRYPT

/**
 * \def MBEDTLS_CAMELLIA_SMALL_MEMORY
 *
 * Use less ROM for the Camellia implementation (~768 bytes).
 */
//#define MBEDTLS_CAMELLIA_SMALL_MEMORY

/**
 * \def MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED
 *
 * Enable the verified implementations of ECDH primitives from Project Everest
 * (Curve25519). Apache 2.0 only; not compatible with GPL v2.0-or-later.
 */
//#define MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED

/**
 * \def MBEDTLS_ECP_NIST_OPTIM
 *
 * Enable specific 'modulo p' routines for each NIST prime.
 * Depending on the prime and architecture, makes operations 4 to 8 times
 * faster on the corresponding curve.
 *
 * Comment this macro to disable NIST curves optimisation.
 */
#undef MBEDTLS_ECP_NIST_OPTIM

/**
 * \def MBEDTLS_ECP_RESTARTABLE
 *
 * Enable "non-blocking" ECC operations that can return early and be resumed.
 * Adds xxx_restartable() variants in ECP/ECDSA/PK/X509 modules.
 *
 * Requires: Builtin support of Elliptic Curves.
 */
//#define MBEDTLS_ECP_RESTARTABLE

/**
 * \def MBEDTLS_ECP_WITH_MPI_UINT
 *
 * Use the new bignum code in the ECC modules.
 * \warning Currently experimental and incomplete; not for production.
 */
//#define MBEDTLS_ECP_WITH_MPI_UINT

/**
 * \def MBEDTLS_GCM_LARGE_TABLE
 *
 * Enable large pre-computed tables for GCM. Increases mbedtls_gcm_context
 * size by ~3840 bytes and code by ~344 bytes; boosts throughput on systems
 * without GCM hardware acceleration.
 *
 * Requires: The GCM built-in implementation.
 */
//#define MBEDTLS_GCM_LARGE_TABLE

/**
 * \def MBEDTLS_HAVE_ASM
 *
 * The compiler has support for asm().
 *
 * Requires support for asm() in compiler.
 *
 * Used in:
 *      drivers/builtin/src/aesni.h
 *      drivers/builtin/src/aria.c
 *      drivers/builtin/src/bn_mul.h
 *      utilities/constant_time.c
 *
 * Required by:
 *      MBEDTLS_AESCE_C
 *      MBEDTLS_AESNI_C (on some platforms)
 *
 * Comment to disable the use of assembly code.
 */
#undef MBEDTLS_HAVE_ASM

/**
 * \def MBEDTLS_HAVE_SSE2
 *
 * CPU supports SSE2 instruction set (IA-32 specific).
 */
//#define MBEDTLS_HAVE_SSE2

/**
 * \def MBEDTLS_NO_UDBL_DIVISION
 *
 * The platform lacks support for double-width integer division.
 * Prevents the bignum code from relying on double-width division routines
 * that may need to be linked from a toolchain library.
 */
//#define MBEDTLS_NO_UDBL_DIVISION

/**
 * \def MBEDTLS_NO_64BIT_MULTIPLICATION
 *
 * The platform lacks support for 32x32 -> 64-bit multiplication. Embeds a
 * software implementation in Mbed TLS instead of relying on the toolchain.
 */
//#define MBEDTLS_NO_64BIT_MULTIPLICATION

/**
 * \def MBEDTLS_PSA_P256M_DRIVER_ENABLED
 *
 * Enable p256-m: an alternative implementation of key generation, ECDH and
 * (randomized) ECDSA on SECP256R1 with a much smaller footprint. Only
 * available via the PSA API.
 *
 * Recommended when the only ECC curve needed is SECP256R1 (ECDH/ECDSA).
 */
//#define MBEDTLS_PSA_P256M_DRIVER_ENABLED

/**
 * \def MBEDTLS_RSA_NO_CRT
 *
 * Do not use the Chinese Remainder Theorem for the RSA private operation.
 */
//#define MBEDTLS_RSA_NO_CRT

/**
 * \def MBEDTLS_SHA256_SMALLER
 *
 * Enable an implementation of SHA-256 with lower ROM footprint but lower
 * performance (~30% slower on Cortex-M4).
 */
//#define MBEDTLS_SHA256_SMALLER

/**
 * \def MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT
 *
 * Enable runtime acceleration of SHA-256/224 using ARMv8 crypto extensions
 * when available; fall back to the C implementation otherwise.
 *
 * Requires: The SHA-256 built-in implementation.
 * \warning Cannot be defined together with MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY.
 */
//#define MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT

/**
 * \def MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY
 *
 * Enable SHA-256/224 acceleration via ARMv8 crypto extensions; must be
 * present at runtime or an illegal instruction fault will occur. Smaller
 * code size than the IF_PRESENT variant.
 *
 * Requires: The SHA-256 built-in implementation.
 * \warning Cannot be defined together with MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT.
 */
//#define MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY

/**
 * \def MBEDTLS_SHA512_SMALLER
 *
 * Enable an implementation of SHA-512 with lower ROM footprint but lower
 * performance.
 */
//#define MBEDTLS_SHA512_SMALLER

/**
 * \def MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT
 *
 * Enable runtime acceleration of SHA-512/384 using ARMv8 A64 crypto
 * extensions when available; fall back to the C implementation otherwise.
 *
 * Requires: The SHA-512 built-in implementation.
 * \warning Cannot be defined together with MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY.
 */
//#define MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT

/**
 * \def MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY
 *
 * Enable SHA-512/384 acceleration via ARMv8 A64 crypto extensions; must be
 * present at runtime. Smaller code size than the IF_PRESENT variant.
 *
 * Requires: The SHA-512 built-in implementation.
 * \warning Cannot be defined together with MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT.
 */
//#define MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY

/* ECP options */
//#define MBEDTLS_ECP_FIXED_POINT_OPTIM      1 /**< Enable fixed-point speed-up */
//#define MBEDTLS_ECP_WINDOW_SIZE            4 /**< Maximum window size used */

/* MPI / BIGNUM options */
//#define MBEDTLS_MPI_MAX_SIZE            1024 /**< Maximum number of bytes for usable MPIs. */
//#define MBEDTLS_MPI_WINDOW_SIZE            2 /**< Maximum window size used. */

/* RSA OPTIONS */
//#define MBEDTLS_RSA_GEN_KEY_MIN_BITS            1024 /**< Minimum RSA key size that can be generated in bits (Minimum possible value is 128 bits). */

/** \} name SECTION: Builtin drivers */

/* Do not enable except for testing. Will be removed in a future minor version. */
//#define TF_PSA_CRYPTO_ALLOW_REMOVED_MECHANISMS
