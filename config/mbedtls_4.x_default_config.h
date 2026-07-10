/**
 * Copyright (c) 2026 Infineon Technologies AG
 *
 * SPDX-License-Identifier: MIT
 *
 * \file mbedtls_4.x_default_config.h
 *
 * \brief Configuration options for mbedtls (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 * 
 *  Reference : external/mbedtls-4.x/include/mbedtls/mbedtls_config.h
 */


/**
 * \name SECTION: Platform abstraction layer
 *
 * This section sets platform specific settings.
 * \{
 */

 /**
 * \def MBEDTLS_NET_C
 *
 * Enable the TCP and UDP over IPv6/IPv4 networking routines.
 *
 * \note This module only works on POSIX/Unix (including Linux, BSD and OS X)
 * and Windows. For other platforms, you'll want to disable it, and write your
 * own networking callbacks to be passed to \c mbedtls_ssl_set_bio().
 *
 * \note See also our Knowledge Base article about porting to a new
 * environment:
 * https://mbed-tls.readthedocs.io/en/latest/kb/how-to/how-do-i-port-mbed-tls-to-a-new-environment-OS
 *
 * Module:  library/net_sockets.c
 *
 * This module provides networking routines.
 */
#undef MBEDTLS_NET_C 

/**
 * \def MBEDTLS_TIMING_C
 *
 * Enable a timer interface used by some sample and test programs.
 *
 * Requires: MBEDTLS_HAVE_TIME or MBEDTLS_TIMING_ALT
 *
 * Module:  library/timing.c
 */
#undef MBEDTLS_TIMING_C
/** \} name SECTION: Platform abstraction layer */

/**
 * \name SECTION: General configuration options
 *
 * This section contains Mbed TLS build settings that are not associated
 * with a particular module.
 * \{
 */

/**
 * \def MBEDTLS_ERROR_C
 *
 * Enable error code to error string conversion.
 *
 * Module:  library/error.c
 * Caller:
 *
 * This module enables mbedtls_strerror().
 */
#undef MBEDTLS_ERROR_C

/**
 * \def MBEDTLS_ERROR_STRERROR_DUMMY
 *
 * Enable a dummy error function to make use of mbedtls_strerror() in
 * third party libraries easier when MBEDTLS_ERROR_C is disabled
 * (no effect when MBEDTLS_ERROR_C is enabled).
 *
 * You can safely disable this if MBEDTLS_ERROR_C is enabled, or if you're
 * not using mbedtls_strerror() or error_strerror() in your application.
 *
 * Disable if you run into name conflicts and want to really remove the
 * mbedtls_strerror()
 */
#undef MBEDTLS_ERROR_STRERROR_DUMMY

/**
 * \def MBEDTLS_VERSION_C
 *
 * Enable run-time version information.
 *
 * Module:  library/version.c
 *
 * This module provides run-time version information.
 */
#undef MBEDTLS_VERSION_C

/**
 * \def MBEDTLS_VERSION_FEATURES
 *
 * Allow run-time checking of compile-time enabled features. Thus allowing users
 * to check at run-time if the library is for instance compiled with threading
 * support via mbedtls_version_check_feature().
 *
 * Requires: MBEDTLS_VERSION_C
 *
 * Comment this to disable run-time checking and save ROM space
 */
#undef MBEDTLS_VERSION_FEATURES

/** \} name SECTION: General configuration options */

/**
 * \name SECTION: TLS feature selection
 *
 * This section sets support for features that are or are not needed
 * within the modules that are enabled.
 * \{
 */

/**
 * \def MBEDTLS_DEBUG_C
 *
 * Enable the debug functions.
 *
 * Module:  library/debug.c
 * Caller:  library/ssl_msg.c
 *          library/ssl_tls.c
 *          library/ssl_tls12_*.c
 *          library/ssl_tls13_*.c
 *
 * This module provides debugging functions.
 */
#define MBEDTLS_DEBUG_C

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
 *
 * Enable the ECDHE-ECDSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: PSA_WANT_ALG_ECDH
 *           PSA_WANT_ALG_ECDSA
 *           MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256
 */
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
 *
 * Enable the ECDHE-PSK based ciphersuite modes in SSL / TLS.
 *
 * Requires: PSA_WANT_ALG_ECDH
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256
 */
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
 *
 * Enable the ECDHE-RSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: PSA_WANT_ALG_ECDH
 *           PSA_WANT_ALG_RSA_PKCS1V15_SIGN
 *           MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
 */
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED

/**
 * \def MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
 *
 * Enable the PSK based ciphersuite modes in SSL / TLS.
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256
 */
#undef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

/**
 * \def MBEDTLS_SSL_ALL_ALERT_MESSAGES
 *
 * Enable sending of alert messages in case of encountered errors as per RFC.
 * If you choose not to send the alert messages, Mbed TLS can still communicate
 * with other servers, only debugging of failures is harder.
 *
 * The advantage of not sending alert messages, is that no information is given
 * about reasons for failures thus preventing adversaries of gaining intel.
 *
 * Enable sending of all alert messages
 */
#undef MBEDTLS_SSL_ALL_ALERT_MESSAGES

/**
 * \def MBEDTLS_SSL_ALPN
 *
 * Enable support for RFC 7301 Application Layer Protocol Negotiation.
 *
 * Comment this macro to disable support for ALPN.
 */
#undef MBEDTLS_SSL_ALPN

/**
 * \def MBEDTLS_SSL_ASYNC_PRIVATE
 *
 * Enable asynchronous external private key operations in SSL. This allows
 * you to configure an SSL connection to call an external cryptographic
 * module to perform private key operations instead of performing the
 * operation inside the library.
 *
 * Requires: MBEDTLS_X509_CRT_PARSE_C
 */
//#undef MBEDTLS_SSL_ASYNC_PRIVATE

/**
 * \def MBEDTLS_SSL_CACHE_C
 *
 * Enable simple SSL cache implementation.
 *
 * Module:  library/ssl_cache.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_CACHE_C
 */
#undef MBEDTLS_SSL_CACHE_C

/**
 * \def MBEDTLS_SSL_CLI_C
 *
 * Enable the SSL/TLS client code.
 *
 * Module:  library/ssl*_client.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_TLS_C
 *
 * \warning You must call psa_crypto_init() before doing any TLS operations.
 *
 * This module is required for SSL/TLS client support.
 */
#define MBEDTLS_SSL_CLI_C

/**
 * \def MBEDTLS_SSL_COOKIE_C
 *
 * Enable basic implementation of DTLS cookies for hello verification.
 *
 * Module:  library/ssl_cookie.c
 * Caller:
 */
#undef MBEDTLS_SSL_COOKIE_C

/**
 * \def MBEDTLS_SSL_DTLS_ANTI_REPLAY
 *
 * Enable support for the anti-replay mechanism in DTLS.
 *
 * Requires: MBEDTLS_SSL_TLS_C
 *           MBEDTLS_SSL_PROTO_DTLS
 *
 * \warning Disabling this is often a security risk!
 * See mbedtls_ssl_conf_dtls_anti_replay() for details.
 *
 * Comment this to disable anti-replay in DTLS.
 */
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY

/**
 * \def MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
 *
 * Enable server-side support for clients that reconnect from the same port.
 *
 * Some clients unexpectedly close the connection and try to reconnect using the
 * same source port. This needs special support from the server to handle the
 * new connection securely, as described in section 4.2.8 of RFC 6347. This
 * flag enables that support.
 *
 * Requires: MBEDTLS_SSL_DTLS_HELLO_VERIFY
 *
 * Comment this to disable support for clients reusing the source port.
 */
#undef MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE

/**
 * \def MBEDTLS_SSL_DTLS_CONNECTION_ID
 *
 * Enable support for the DTLS Connection ID (CID) extension,
 * which allows to identify DTLS connections across changes
 * in the underlying transport. The CID functionality is described
 * in RFC 9146.
 *
 * Setting this option enables the SSL APIs `mbedtls_ssl_set_cid()`,
 * mbedtls_ssl_get_own_cid()`, `mbedtls_ssl_get_peer_cid()` and
 * `mbedtls_ssl_conf_cid()`. See the corresponding documentation for
 * more information.
 *
 * The maximum lengths of outgoing and incoming CIDs can be configured
 * through the options
 * - MBEDTLS_SSL_CID_OUT_LEN_MAX
 * - MBEDTLS_SSL_CID_IN_LEN_MAX.
 *
 * Requires: MBEDTLS_SSL_PROTO_DTLS
 *
 * Uncomment to enable the Connection ID extension.
 */
#undef MBEDTLS_SSL_DTLS_CONNECTION_ID

/**
 * \def MBEDTLS_SSL_DTLS_HELLO_VERIFY
 *
 * Enable support for HelloVerifyRequest on DTLS servers.
 *
 * This feature is highly recommended to prevent DTLS servers being used as
 * amplifiers in DoS attacks against other hosts. It should always be enabled
 * unless you know for sure amplification cannot be a problem in the
 * environment in which your server operates.
 *
 * \warning Disabling this can be a security risk! (see above)
 *
 * Requires: MBEDTLS_SSL_PROTO_DTLS
 *
 * Comment this to disable support for HelloVerifyRequest.
 */
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY

/** \def MBEDTLS_SSL_ENCRYPT_THEN_MAC
 *
 * Enable support for Encrypt-then-MAC, RFC 7366.
 *
 * This allows peers that both support it to use a more robust protection for
 * ciphersuites using CBC, providing deep resistance against timing attacks
 * on the padding or underlying cipher.
 *
 * This only affects CBC ciphersuites, and is useless if none is defined.
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Comment this macro to disable support for Encrypt-then-MAC
 */
#undef MBEDTLS_SSL_ENCRYPT_THEN_MAC

/** \def MBEDTLS_SSL_EXTENDED_MASTER_SECRET
 *
 * Enable support for RFC 7627: Session Hash and Extended Master Secret
 * Extension.
 *
 * This was introduced as "the proper fix" to the Triple Handshake family of
 * attacks, but it is recommended to always use it (even if you disable
 * renegotiation), since it actually fixes a more fundamental issue in the
 * original SSL/TLS design, and has implications beyond Triple Handshake.
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Comment this macro to disable support for Extended Master Secret.
 */
#undef MBEDTLS_SSL_EXTENDED_MASTER_SECRET

/**
 * \def MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
 *
 * This option controls the availability of the API mbedtls_ssl_get_peer_cert()
 * giving access to the peer's certificate after completion of the handshake.
 *
 * Unless you need mbedtls_ssl_peer_cert() in your application, it is
 * recommended to disable this option for reduced RAM usage.
 *
 * \note If this option is disabled, mbedtls_ssl_get_peer_cert() is still
 *       defined, but always returns \c NULL.
 *
 * \note This option has no influence on the protection against the
 *       triple handshake attack. Even if it is disabled, Mbed TLS will
 *       still ensure that certificates do not change during renegotiation,
 *       for example by keeping a hash of the peer's certificate.
 *
 * \note This option is required if MBEDTLS_SSL_PROTO_TLS1_3 is set.
 *
 * Comment this macro to disable storing the peer's certificate
 * after the handshake.
 */
#define MBEDTLS_SSL_KEEP_PEER_CERTIFICATE

/**
 * \def MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
 *
 * Enable support for RFC 6066 max_fragment_length extension in SSL.
 *
 * Comment this macro to disable support for the max_fragment_length extension
 */
#undef MBEDTLS_SSL_MAX_FRAGMENT_LENGTH

/**
 * \def MBEDTLS_SSL_PROTO_DTLS
 *
 * Enable support for DTLS (all available versions).
 *
 * Enable this and MBEDTLS_SSL_PROTO_TLS1_2 to enable DTLS 1.2.
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Comment this macro to disable support for DTLS
 */
#undef MBEDTLS_SSL_PROTO_DTLS

/**
 * \def MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Enable support for TLS 1.2 (and DTLS 1.2 if DTLS is enabled).
 *
 * Requires: PSA_WANT_ALG_SHA_256 or PSA_WANT_ALG_SHA_384
 *
 * Comment this macro to disable support for TLS 1.2 / DTLS 1.2
 */
#undef MBEDTLS_SSL_PROTO_TLS1_2

/**
 * \def MBEDTLS_SSL_PROTO_TLS1_3
 *
 * Enable support for TLS 1.3.
 *
 * \note See docs/architecture/tls13-support.md for a description of the TLS
 *       1.3 support that this option enables.
 *
 * Requires: MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
 * Requires: MBEDTLS_PSA_CRYPTO_C
 *
 * Uncomment this macro to enable the support for TLS 1.3.
 */
#define MBEDTLS_SSL_PROTO_TLS1_3

/**
 * \def MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
 *
 * When this option is enabled, the client and server can extract additional
 * shared symmetric keys after an SSL handshake using the function
 * mbedtls_ssl_export_keying_material().
 *
 * The process for deriving the keys is specified in RFC 5705 for TLS 1.2 and
 * in RFC 8446, Section 7.5, for TLS 1.3.
 *
 * Comment this macro to disable mbedtls_ssl_export_keying_material().
 */
#undef MBEDTLS_SSL_KEYING_MATERIAL_EXPORT

/**
 * \def MBEDTLS_SSL_RENEGOTIATION
 *
 * Enable support for TLS renegotiation.
 *
 * The two main uses of renegotiation are (1) refresh keys on long-lived
 * connections and (2) client authentication after the initial handshake.
 * If you don't need renegotiation, it's probably better to disable it, since
 * it has been associated with security issues in the past and is easy to
 * misuse/misunderstand.
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Comment this to disable support for renegotiation.
 *
 * \note   Even if this option is disabled, both client and server are aware
 *         of the Renegotiation Indication Extension (RFC 5746) used to
 *         prevent the SSL renegotiation attack (see RFC 5746 Sect. 1).
 *         (See \c mbedtls_ssl_conf_legacy_renegotiation for the
 *          configuration of this extension).
 *
 */
#undef MBEDTLS_SSL_RENEGOTIATION

/**
 * \def MBEDTLS_SSL_SERVER_NAME_INDICATION
 *
 * Enable support for RFC 6066 server name indication (SNI) in SSL.
 *
 * Requires: MBEDTLS_X509_CRT_PARSE_C
 *
 * Comment this macro to disable support for server name indication in SSL
 */
#define MBEDTLS_SSL_SERVER_NAME_INDICATION

/**
 * \def MBEDTLS_SSL_SESSION_TICKETS
 *
 * Enable support for RFC 5077 session tickets in SSL.
 * Client-side, provides full support for session tickets (maintenance of a
 * session store remains the responsibility of the application, though).
 * Server-side, you also need to provide callbacks for writing and parsing
 * tickets, including authenticated encryption and key management. Example
 * callbacks are provided by MBEDTLS_SSL_TICKET_C.
 *
 * Comment this macro to disable support for SSL session tickets
 */
#undef MBEDTLS_SSL_SESSION_TICKETS

/**
 * \def MBEDTLS_SSL_SRV_C
 *
 * Enable the SSL/TLS server code.
 *
 * Module:  library/ssl*_server.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_TLS_C
 *
 * \warning You must call psa_crypto_init() before doing any TLS operations.
 *
 * This module is required for SSL/TLS server support.
 */
#undef MBEDTLS_SSL_SRV_C

/**
 * \def MBEDTLS_SSL_TICKET_C
 *
 * Enable an implementation of TLS server-side callbacks for session tickets.
 *
 * Module:  library/ssl_ticket.c
 * Caller:
 *
 * Requires: PSA_WANT_ALG_GCM or PSA_WANT_ALG_CCM or PSA_WANT_ALG_CHACHA20_POLY1305
 */
#undef MBEDTLS_SSL_TICKET_C

/**
 * \def MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
 *
 * Enable TLS 1.3 middlebox compatibility mode.
 *
 * As specified in Section D.4 of RFC 8446, TLS 1.3 offers a compatibility
 * mode to make a TLS 1.3 connection more likely to pass through middle boxes
 * expecting TLS 1.2 traffic.
 *
 * Turning on the compatibility mode comes at the cost of a few added bytes
 * on the wire, but it doesn't affect compatibility with TLS 1.3 implementations
 * that don't use it. Therefore, unless transmission bandwidth is critical and
 * you know that middlebox compatibility issues won't occur, it is therefore
 * recommended to set this option.
 *
 * Comment to disable compatibility mode for TLS 1.3. If
 * MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not have any
 * effect on the build.
 *
 */
#define MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE

/**
 * \def MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
 *
 * Enable TLS 1.3 ephemeral key exchange mode.
 *
 * Requires: PSA_WANT_ALG_ECDH or PSA_WANT_ALG_FFDH
 *           MBEDTLS_X509_CRT_PARSE_C
 *           and at least one of:
 *               PSA_WANT_ALG_ECDSA
 *               PSA_WANT_ALG_RSA_PSS
 *
 * Comment to disable support for the ephemeral key exchange mode in TLS 1.3.
 * If MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not have any
 * effect on the build.
 *
 */
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED

/**
 * \def MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
 *
 * Enable TLS 1.3 PSK key exchange mode.
 *
 * Comment to disable support for the PSK key exchange mode in TLS 1.3. If
 * MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not have any
 * effect on the build.
 *
 */
#undef MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED

/**
 * \def MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
 *
 * Enable TLS 1.3 PSK ephemeral key exchange mode.
 *
 * Requires: PSA_WANT_ALG_ECDH or PSA_WANT_ALG_FFDH
 *
 * Comment to disable support for the PSK ephemeral key exchange mode in
 * TLS 1.3. If MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not
 * have any effect on the build.
 *
 */
#undef MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED

/**
 * \def MBEDTLS_SSL_TLS_C
 *
 * Enable the generic SSL/TLS code.
 *
 * Module:  library/ssl_tls.c
 * Caller:  library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * Requires: PSA_WANT_ALG_SHA_256 or PSA_WANT_ALG_SHA_384
 *           and at least one of the MBEDTLS_SSL_PROTO_XXX defines
 *
 * This module is required for SSL/TLS.
 */
#define MBEDTLS_SSL_TLS_C

 /** \} name SECTION: TLS feature selection */

 /**
 * \name SECTION: X.509 feature selection
 *
 * This section sets Certificate related options.
 * \{
 */

/**
 * \def MBEDTLS_PKCS7_C
 *
 * Enable PKCS #7 core for using PKCS #7-formatted signatures.
 * RFC Link - https://tools.ietf.org/html/rfc2315
 *
 * Module:  library/pkcs7.c
 *
 * Requires: MBEDTLS_ASN1_PARSE_C, MBEDTLS_PK_PARSE_C,
 *           MBEDTLS_X509_CRT_PARSE_C MBEDTLS_X509_CRL_PARSE_C,
 *           MBEDTLS_MD_C
 *
 * This module is required for the PKCS #7 parsing modules.
 */
#undef MBEDTLS_PKCS7_C

/**
 * \def MBEDTLS_X509_CREATE_C
 *
 * Enable X.509 core for creating certificates.
 *
 * Module:  library/x509_create.c
 *
 * Requires: MBEDTLS_ASN1_WRITE_C, MBEDTLS_PK_PARSE_C
 *
 * \warning You must call psa_crypto_init() before doing any X.509 operation.
 *
 * This module is the basis for creating X.509 certificates and CSRs.
 */
#undef MBEDTLS_X509_CREATE_C

/**
 * \def MBEDTLS_X509_CRL_PARSE_C
 *
 * Enable X.509 CRL parsing.
 *
 * Module:  library/x509_crl.c
 * Caller:  library/x509_crt.c
 *
 * Requires: MBEDTLS_X509_USE_C
 *
 * This module is required for X.509 CRL parsing.
 */
#undef MBEDTLS_X509_CRL_PARSE_C

/**
 * \def MBEDTLS_X509_CRT_WRITE_C
 *
 * Enable creating X.509 certificates.
 *
 * Module:  library/x509_crt_write.c
 *
 * Requires: MBEDTLS_X509_CREATE_C
 *
 * This module is required for X.509 certificate creation.
 */
#undef MBEDTLS_X509_CRT_WRITE_C

/**
 * \def MBEDTLS_X509_CSR_PARSE_C
 *
 * Enable X.509 Certificate Signing Request (CSR) parsing.
 *
 * Module:  library/x509_csr.c
 * Caller:  library/x509_crt_write.c
 *
 * Requires: MBEDTLS_X509_USE_C
 *
 * This module is used for reading X.509 certificate request.
 */
#undef MBEDTLS_X509_CSR_PARSE_C

/**
 * \def MBEDTLS_X509_CSR_WRITE_C
 *
 * Enable creating X.509 Certificate Signing Requests (CSR).
 *
 * Module:  library/x509_csr_write.c
 *
 * Requires: MBEDTLS_X509_CREATE_C
 *
 * This module is required for X.509 certificate request writing.
 */
#undef MBEDTLS_X509_CSR_WRITE_C

/**
 * \def MBEDTLS_X509_RSASSA_PSS_SUPPORT
 *
 * Enable parsing and verification of X.509 certificates, CRLs and CSRS
 * signed with RSASSA-PSS (aka PKCS#1 v2.1).
 *
 * Requires: PSA_WANT_ALG_RSA_PSS
 *
 * Comment this macro to disallow using RSASSA-PSS in certificates.
 */
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT

/**
 * \def MBEDTLS_X509_USE_C
 *
 * Enable X.509 core for using certificates.
 *
 * Module:  library/x509.c
 * Caller:  library/x509_crl.c
 *          library/x509_crt.c
 *          library/x509_csr.c
 *
 * Requires: MBEDTLS_ASN1_PARSE_C, MBEDTLS_PK_PARSE_C,
 *           (see suites in tf_psa_default_config.h for PK/ASN1 back-end)
 *
 * This module is required for the X.509 parsing modules.
 */
#define MBEDTLS_X509_USE_C

/**
 * \def MBEDTLS_X509_CRT_PARSE_C
 *
 * Enable X.509 certificate parsing.
 *
 * Module:  library/x509_crt.c
 * Caller:  library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * Requires: MBEDTLS_X509_USE_C
 *
 * This module is required for X.509 certificate parsing.
 */
#define MBEDTLS_X509_CRT_PARSE_C

 /** \} name SECTION: X.509 feature selection */