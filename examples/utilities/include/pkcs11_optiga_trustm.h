
/* pkcs11_optiga_trustm.h
 * OPTIGA pkcs11 helper file
 */

#ifndef PKCS11_OPTIGA_TRUSTM_H
#define PKCS11_OPTIGA_TRUSTM_H 1

#include <stdint.h>

#ifdef _WIN32
    #pragma pack(push, cryptoki, 1)
#endif

/**
 * @brief FreeRTOS PKCS#11 Interface.
 * The following definitions are required by the PKCS#11 standard public
 * headers.
 */

#define CK_PTR          *

#ifndef NULL_PTR
    #define NULL_PTR    0
#endif

/* CK_DEFINE_FUNCTION is deprecated.  Implementations should use CK_DECLARE_FUNCTION
 * instead when possible. */
#define CK_DEFINE_FUNCTION( returnType, name )             returnType name
#define CK_DECLARE_FUNCTION( returnType, name )            returnType name
#define CK_DECLARE_FUNCTION_POINTER( returnType, name )    returnType( CK_PTR name )
#define CK_CALLBACK_FUNCTION( returnType, name )           returnType( CK_PTR name )

/**
 * @brief PKCS #11 default user PIN.
 *
 * The PKCS #11 standard specifies the presence of a user PIN. That feature is
 * sensible for applications that have an interactive user interface and memory
 * protections. However, since typical microcontroller applications lack one or
 * both of those, the user PIN is assumed to be used herein for interoperability
 * purposes only, and not as a security feature.
 */
#define PKCS11_DEFAULT_USER_PIN    "0000"

/**
 * @brief Maximum length (in characters) for a PKCS #11 CKA_LABEL
 * attribute.
 */
#define MAX_LABEL_LENGTH     32
/**
 * @brief OID for curve P-256.
 */
#define pkcs11DER_ENCODED_OID_P256           { 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 }

/**
 * @brief OID for curve P-384.
 */
#define pkcs11DER_ENCODED_OID_P384           { 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22 }

/**
 * @brief OID for curve P-512.
 */
#define pkcs11DER_ENCODED_OID_P521           { 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x23 }


/**
 * @brief Maximum number of token objects that can be stored
 * by the PKCS #11 module.
 */
#define MAX_NUM_OBJECTS      6

/**
 * @brief Set to 1 if a PAL destroy object is implemented.
 *
 * If set to 0, no PAL destroy object is implemented, and this functionality
 * is implemented in the common PKCS #11 layer.
 */
#define PAL_DESTROY_SUPPORTED                  0

/**
 * @brief Set to 1 if OTA image verification via PKCS #11 module is supported.
 *
 * If set to 0, OTA code signing certificate is built in via
 * aws_ota_codesigner_certificate.h.
 */
#define OTA_SUPPORTED                          0

/**
 * @brief Set to 1 if PAL supports storage for JITP certificate,
 * code verify certificate, and trusted server root certificate.
 *
 * If set to 0, PAL does not support storage mechanism for these, and
 * they are accessed via headers compiled into the code.
 */
#define JITP_CODEVERIFY_ROOT_CERT_SUPPORTED    0

/**
 * @brief The PKCS #11 label for device private key.
 *
 * Private key for connection to AWS IoT endpoint.  The corresponding
 * public key should be registered with the AWS IoT endpoint.
 */
#define LABEL_DEVICE_PRIVATE_KEY_FOR_TLS       "0xE0F1"

#define LABEL_DEVICE_RSA_PRIVATE_KEY_FOR_TLS   "0xE0FC"
/**
 * @brief The PKCS #11 label for device public key.
 *
 * The public key corresponding to LABEL_DEVICE_PRIVATE_KEY_FOR_TLS.
 */
#define LABEL_DEVICE_PUBLIC_KEY_FOR_TLS        "0xF1D2"
#define LABEL_DEVICE_RSA_PUBLIC_KEY_FOR_TLS    "0xF1E0"


/**
 * @brief The PKCS #11 label for the device certificate.
 *
 * Device certificate corresponding to LABEL_DEVICE_PRIVATE_KEY_FOR_TLS.
 */
#define LABEL_DEVICE_CERTIFICATE_FOR_TLS       "0xE0E2"

/**
 * @brief The PKCS #11 label for the object to be used for code verification.
 *
 * Used by over-the-air update code to verify an incoming signed image.
 */
#define LABEL_CODE_VERIFICATION_KEY            "0xE0EF"

/**
 * @brief The PKCS #11 label for Just-In-Time-Provisioning.
 *
 * The certificate corresponding to the issuer of the device certificate
 * (LABEL_DEVICE_CERTIFICATE_FOR_TLS) when using the JITR or
 * JITP flow.
 */
#define LABEL_JITP_CERTIFICATE                 "JITP Cert"

/**
 * @brief The PKCS #11 label for the AWS Trusted Root Certificate.
 *
 * @see aws_default_root_certificates.h
 */
#define LABEL_ROOT_CERTIFICATE                 "0xE0E8"

/**
 * @brief Set to 1 if importing device private key via C_CreateObject is supported. 0 if not.
 */
#define IMPORT_PRIVATE_KEYS_SUPPORTED          0

/**
 *   @brief Length of a SHA256 digest, in bytes.
 */
#define pkcs11SHA256_DIGEST_LENGTH           32

/**
 * @brief Length of a curve P-256 ECDSA signature, in bytes.
 */
#define pkcs11ECDSA_P256_SIGNATURE_LENGTH    64
/**
 * @brief Length of a curve P-384 ECDSA signature, in bytes.
 */
#define pkcs11ECDSA_P384_SIGNATURE_LENGTH    96
/**
 * @brief Length of a curve P-521 ECDSA signature, in bytes.
 */
#define pkcs11ECDSA_P521_SIGNATURE_LENGTH    132


/**
 * @brief The number of bits in the RSA-1024 modulus.
 *
 */
#define pkcs11RSA_1024_MODULUS_BITS          1024
/**
 * @brief Length of PKCS #11 signature for RSA 1024 key, in bytes.
 */
#define pkcs11RSA_1024_SIGNATURE_LENGTH      ( pkcs11RSA_1024_MODULUS_BITS / 8 )
/**
 * @brief The number of bits in the RSA-2048 modulus.
 *
 */
#define pkcs11RSA_2048_MODULUS_BITS          2048
/**
 * @brief Length of PKCS #11 signature for RSA 1024 key, in bytes.
 */
#define pkcs11RSA_2048_SIGNATURE_LENGTH      ( pkcs11RSA_2048_MODULUS_BITS / 8 )



#include "pkcs11.h"

/* System dependencies.  */
#if defined(_WIN32) || defined(CRYPTOKI_FORCE_WIN32)
#pragma pack(pop, cryptoki)
#endif

#endif	/* PKCS11_H */
