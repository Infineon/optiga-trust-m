# Protected Update Dataset generation Tool

## Usage

`protected_update_data_set.exe input1=<value> input2=<value>`
	
```
Tool Version : 3.00.2490
Info : Default values are set
Usage : <.exe> input1=<value> input2=<value> ..

        (1) : To create manifest , provide the following details
        payload_version      default  :  0
                             note     :  Input is a decimal string.E.g. 10
        trust_anchor_oid     default  :  E0E8
                             note     :  Input is a hexadecimal string.E.g. E0E8
        target_oid           default  :  E0E2
                             note     :  Input is a hexadecimal string.E.g. E0E2
        couid                default  :  (null)
                             note     :  Unicast gets enabled if "couid" is provided otherwise it is broadcast.
                                      :  Input is a hexadecimal string.E.g. A1DE34
        sign_algo            default  :  ES_256
                             options  :  ES_256 , RSA-SSA-PKCS1-V1_5-SHA-256
        priv_key             default  :  (null)
                             options  :  private key file (pem format)
                             note     :  Refer : samples/integrity/sample_ec_256_priv.pem
        digest_algo          default  :  SHA256
                             options  :  SHA256
        payload_type         default  :  data
                             options  :  data , key , metadata

        (2) : To enable confidentiality,"secret" must be provided (All other options are ignored if there is no confidentiality)
        secret               default  :  (null)
                             options  :  Text file containing shared secret as hexadecimal string
                             note     :  Refer : samples/confidentiality/secret.txt
        secret_oid           default  :  F1D0
                             note     :  Input is a hexadecimal string.E.g. F1D0
        label                default  :  Confidentiality
                             note     :  Input is a string.E.g. Confidentiality
        enc_algo             default  :  AES-CCM-16-64-128
                             options  :  AES-CCM-16-64-128
        kdf                  default  :  IFX_KDF-TLS12_PRF_SHA256
                             options  :  IFX_KDF-TLS12_PRF_SHA256
        seed_length          default  :  64
                             note     :  Input is a decimal string.E.g. 64

        (3.1) : To update data object, "payload_type" should be "data" and provide the following details:
        offset               default  :  0
        write_type           default  :  2
                             options  :  Write (1), EraseAndWrite (2)
        data                 default  :  (null)
                             note     :  Input is a text file with hexadecimal or ascii string content
        in_data_format       default  :  hex
                             options  :  hex , ascii
                             note     :  Refer : samples/payload/data/ascii_data.txt for input_data_format=ascii
                                      :  Refer : samples/payload/data/hex_data.txt for input_data_format=hex

        (3.2) : To update key object, "payload_type" should be "key" and provide the following details:
        key_usage            default  :  02
                             options  :  AUTH (0x01) , ENC (0x02) , SIGN (0x10) , KEY_AGREE (0x20)
                             note     :  Input is a hexadecimal string.E.g. E3
                                      :  The values in "options" can be bitwise ORED and provided ( Refer SRM )
        key_algo             default  :
                             options  :  ECC-NIST-P-256 (3) , ECC-NIST-P-384 (4), ECC-NIST-P-521 (5), ECC-BRAINPOOL-P-256-R1 (19) , ECC-BRAINPOOL-P-384-R1 (21), ECC-BRAINPOOL-P-512-R1 (22), RSA-1024-Exp (65) , RSA-2048-Exp (66), AES-128 (129), AES-192 (129), AES-256 (131)
                             note     :  Input is a decimal string.E.g. 129
        key_data             default  :  (null)
                             options  :  ECC / RSA key in .pem format or AES key in txt file as hexadecimal string
                             note     :  Refer : samples/payload/key/sample_ec_256_priv.pem for ECC or RSA key
                                      :  Refer : samples/payload/key/aes_key.txt for AES key

        (3.3) : To update metadata object, "payload_type" should be "metadata" and provide the following details:
        content_reset        default  :  0
                             options  :  As per metadata identifier flag (0), Zeroes (1), Random (2)
        metadata             default  :  (null)
                             note     :  Input is a txt file as hexadecimal string

        (4) : To write dataset to file, "dataset_to_file" should be the file path
        dataset_to_file      default  :  (null)
                             options  :  Provide the filename for output dataset to be stored
```
        
## Sample

A sample script demonstrating the usage of the tool is available in `..\samples\sample.bat`
	
## Limitations

* Only SHA-256 digest algorithm is supported for hash calculation
* Manifest version number is 1	
    	
## Environment

* The available executable is built using microsoft visual studio 2010 in windows 10 (64 bit)
* Install Microsoft Visual C++ 2010 x86 redistributable (vcredist_x86.exe) package to execute application on machine which does not have Visual Studio.
* mbedTLS 2.16.0 is used for crypto operation.
* The following MACRO must be enabled when using mbedTLS for this tool.
    * MBEDTLS_FS_IO
    * MBEDTLS_PEM_PARSE_C
    * MBEDTLS_BASE64_C
    * MBEDTLS_ECDSA_DETERMINISTIC
    * MBEDTLS_HMAC_DRBG_C
    * MBEDTLS_ECDSA_DETERMINISTIC_DEBUG
    * MBEDTLS_RSA_C
    * MBEDTLS_PKCS1_V15
    * MBEDTLS_PK_RSA_ALT_SUPPORT
    * MBEDTLS_TIMING_C
    * MBEDTLS_ENTROPY_C
    * MBEDTLS_CTR_DRBG_C
    * MBEDTLS_ECP_DP_SECP256R1_ENABLED
    * MBEDTLS_ECP_DP_SECP384R1_ENABLED
    * MBEDTLS_ECP_DP_SECP521R1_ENABLED
    * MBEDTLS_ECP_DP_BP256R1_ENABLED
    * MBEDTLS_ECP_DP_BP384R1_ENABLED
    * MBEDTLS_ECP_DP_BP512R1_ENABLED
