These files are part of the Application Notes
1. [TLS client implementation using mbed TLS crypto library with OPTIGA™ Trust M](https://github.com/Infineon/mbedtls-optiga-trust-m)
2. [AWS IoT FreeRTOS](https://github.com/Infineon/amazon-freertos-optiga-trust)
3. [Microsoft Azure IoT](https://github.com/Infineon/azure-optiga-trust-m) 

**Note A.** If you want to use these files standalone, please keep in mind, that you need to [initialise](https://github.com/Infineon/optiga-trust-m/wiki/Initialisation-hints) your chip prior calling the API.

**Note B.** Please keep in mind, that this mbedTLS interface implementation doesn't use private key provided by a user `d`. It relies on keys preconfigured at compilation time; e.g. like [here](https://github.com/Infineon/optiga-trust-m/blob/master/examples/mbedtls_port/trustm_ecdsa.c#L45), [here](https://github.com/Infineon/optiga-trust-m/blob/master/examples/mbedtls_port/trustm_ecdh.c#L42), and [here](https://github.com/Infineon/optiga-trust-m/blob/master/examples/mbedtls_port/trustm_rsa.c#L81).

TLS handshake and record exchange using RSA and ECC algorithm with mbedTLS
<details>
<summary><font size="+1">Expand Image for RSA</font></summary>
<br>
<img src="https://github.com/Infineon/mbedtls-optiga-trust-m/blob/feature/rsa/extra/pictures/TLS_RSA_with_TLS_ECDHE_RSA_WITH_AES_128.jpg" height="" width="">
<summary>TLS handhake using cipher as MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 with OPTIGA TrustM V1</summary>
<img src="https://github.com/Infineon/mbedtls-optiga-trust-m/raw/feature/rsa/extra/pictures/TLS_RSA_with_TLS_RSA_WITH_AES_128.jpg" height="" width="">
<summary>TLS handhake using cipher as MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256 with OPTIGA TrustM V1</summary>
</details>					 
<details>
<summary><font size="+1">Expand Image for ECC</font></summary>
<br>
<img src="https://github.com/Infineon/mbedtls-optiga-trust-m/raw/master/extra/pictures/API.png" height="" width="">
<summary>TLS handhake using cipher as MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 with OPTIGA™ Trust M V1</summary>
</details>

