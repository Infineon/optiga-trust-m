# Porting guide

## Port Platfrom Abstraction Layer

To port the Software Framework on you Host platfrom please follow this [guidance](NEW_PAL_TEMPLATE)

## Port Crypto module for Platfrom Abstraction Layer

The Crypto PAL helps a Host MCU to perfrom shielded communication (protected Infineon I2C protocol) between the Host and the Trust M
If you don't use shielded connection you can skip this module

Currently three Crypto PALs are supported via third-party libraries (should be provided at compilation/linking time)
1. mbed TLS Crypto Library
1. OpenSSL Crypto Library
1. WolfSSL Crypto Library

There are three functions required to be implemented by the Crypto PAL, these are:
1. `pal_crypt_tls_prf_sha256`
    * Test Vector
1. `pal_crypt_encrypt_aes128_ccm`
    * Test Vector
1. `pal_crypt_decrypt_aes128_ccm`
    * Test Vector
