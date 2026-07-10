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
 * \def MBEDTLS_FS_IO
 *
 * Enable functions that use the filesystem.
 */
#undef MBEDTLS_FS_IO

/** \} name SECTION: Platform abstraction layer */

/**
 * \name SECTION: General and test configuration options
 *
 * This section sets test specific settings.
 * \{
 */

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
#undef MBEDTLS_PK_PARSE_EC_EXTENDED

/**
 * \def MBEDTLS_PK_PARSE_EC_COMPRESSED
 *
 * Enable the support for parsing public keys of type Short Weierstrass
 * (PSA_ECC_FAMILY_SECP_XXX and PSA_ECC_FAMILY_BRAINPOOL_XXX) which are using the
 * compressed point format.
 */
#undef MBEDTLS_PK_PARSE_EC_COMPRESSED

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
#undef MBEDTLS_PK_WRITE_C

 /** \} name SECTION: Cryptographic mechanism selection (extended API) */

 /**
 * \name SECTION: Data format support
 *
 * This section sets data-format specific settings.
 * \{
 */

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
 * \def MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
 * Requires: MBEDTLS_PSA_CRYPTO_C
 */
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

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
#undef MBEDTLS_PSA_BUILTIN_GET_ENTROPY

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

/** \} name SECTION: Builtin drivers */