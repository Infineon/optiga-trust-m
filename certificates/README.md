Each OPTIGA™ Trust M security chip is populated with individual X.509 certificate with a corresponding private key (RSA or ECC based).
The certificate has a following form:

_Example Certificate in DER format **without** pre-pended identity tags stored on the chip_
```
30 82 01 d8 30 82 01 7e a0 03 02 01 02 02 04 32
b7 ea 75 30 0a 06 08 2a 86 48 ce 3d 04 03 02 30
72 31 0b 30 09 06 03 55 04 06 13 02 44 45 31 21
30 1f 06 03 55 04 0a 0c 18 49 6e 66 69 6e 65 6f
6e 20 54 65 63 68 6e 6f 6c 6f 67 69 65 73 20 41
47 31 13 30 11 06 03 55 04 0b 0c 0a 4f 50 54 49
47 41 28 54 4d 29 31 2b 30 29 06 03 55 04 03 0c
22 49 6e 66 69 6e 65 6f 6e 20 4f 50 54 49 47 41
28 54 4d 29 20 54 72 75 73 74 20 4d 20 43 41 20
31 30 31 30 1e 17 0d 31 39 30 36 31 38 30 36 33
30 31 32 5a 17 0d 33 39 30 36 31 38 30 36 33 30
31 32 5a 30 1c 31 1a 30 18 06 03 55 04 03 0c 11
49 6e 66 69 6e 65 6f 6e 20 49 6f 54 20 4e 6f 64
65 30 59 30 13 06 07 2a 86 48 ce 3d 02 01 06 08
2a 86 48 ce 3d 03 01 07 03 42 00 04 cc 55 1e 37
50 07 b9 c6 5c a1 ae bf 5d 4b 6c aa 57 bd 80 85
d9 48 7e 61 9b 01 59 a4 27 ed 5e 5a 97 b5 db d7
df b9 80 cd 2b 5c 3c f4 f1 9c 2c 33 42 87 07 4a
e8 3e 03 5d c3 98 30 0b 11 5c 2a 1b a3 58 30 56
30 0e 06 03 55 1d 0f 01 01 ff 04 04 03 02 00 80
30 0c 06 03 55 1d 13 01 01 ff 04 02 30 00 30 15
06 03 55 1d 20 04 0e 30 0c 30 0a 06 08 2a 82 14
00 44 01 14 01 30 1f 06 03 55 1d 23 04 18 30 16
80 14 3c 30 8c 5c d5 8a e8 a3 5d 32 80 e4 54 83
b2 ff cd 86 4d 23 30 0a 06 08 2a 86 48 ce 3d 04
03 02 03 48 00 30 45 02 21 00 c8 dd 8a 9c d8 2f
ed 88 56 f5 2b d5 ee 79 cc b9 5b 21 94 e8 a4 36
ce b9 93 9e f4 db 3b a6 70 f6 02 20 3e 99 4b 1d
6f 3e c1 3d 0a 71 56 a3 0f 90 cb ad fc 01 63 3e
43 64 81 43 3c 16 45 69 77 27 76 ad

```

The same certificate in PEM format
```
-----BEGIN CERTIFICATE-----
MIIB2DCCAX6gAwIBAgIEMrfqdTAKBggqhkjOPQQDAjByMQswCQYDVQQGEwJERTEh
MB8GA1UECgwYSW5maW5lb24gVGVjaG5vbG9naWVzIEFHMRMwEQYDVQQLDApPUFRJ
R0EoVE0pMSswKQYDVQQDDCJJbmZpbmVvbiBPUFRJR0EoVE0pIFRydXN0IE0gQ0Eg
MTAxMB4XDTE5MDYxODA2MzAxMloXDTM5MDYxODA2MzAxMlowHDEaMBgGA1UEAwwR
SW5maW5lb24gSW9UIE5vZGUwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATMVR43
UAe5xlyhrr9dS2yqV72AhdlIfmGbAVmkJ+1eWpe129ffuYDNK1w89PGcLDNChwdK
6D4DXcOYMAsRXCobo1gwVjAOBgNVHQ8BAf8EBAMCAIAwDAYDVR0TAQH/BAIwADAV
BgNVHSAEDjAMMAoGCCqCFABEARQBMB8GA1UdIwQYMBaAFDwwjFzViuijXTKA5FSD
sv/Nhk0jMAoGCCqGSM49BAMCA0gAMEUCIQDI3Yqc2C/tiFb1K9Xuecy5WyGU6KQ2
zrmTnvTbO6Zw9gIgPplLHW8+wT0KcVajD5DLrfwBYz5DZIFDPBZFaXcndq0=
-----END CERTIFICATE-----
```

The same certificate parsed by openSSL
```
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number: 850913909 (0x32b7ea75)
        Signature Algorithm: ecdsa-with-SHA256
        Issuer: C = DE, O = Infineon Technologies AG, OU = OPTIGA(TM), CN = Infineon OPTIGA(TM) Trust M CA 101
        Validity
            Not Before: Jun 18 06:30:12 2019 GMT
            Not After : Jun 18 06:30:12 2039 GMT
        Subject: CN = Infineon IoT Node
        Subject Public Key Info:
            Public Key Algorithm: id-ecPublicKey
                Public-Key: (256 bit)
                pub:
                    04:cc:55:1e:37:50:07:b9:c6:5c:a1:ae:bf:5d:4b:
                    6c:aa:57:bd:80:85:d9:48:7e:61:9b:01:59:a4:27:
                    ed:5e:5a:97:b5:db:d7:df:b9:80:cd:2b:5c:3c:f4:
                    f1:9c:2c:33:42:87:07:4a:e8:3e:03:5d:c3:98:30:
                    0b:11:5c:2a:1b
                ASN1 OID: prime256v1
                NIST CURVE: P-256
        X509v3 extensions:
            X509v3 Key Usage: critical
                Digital Signature
            X509v3 Basic Constraints: critical
                CA:FALSE
            X509v3 Certificate Policies:
                Policy: 1.2.276.0.68.1.20.1

            X509v3 Authority Key Identifier:
                keyid:3C:30:8C:5C:D5:8A:E8:A3:5D:32:80:E4:54:83:B2:FF:CD:86:4D:23

    Signature Algorithm: ecdsa-with-SHA256
         30:45:02:21:00:c8:dd:8a:9c:d8:2f:ed:88:56:f5:2b:d5:ee:
         79:cc:b9:5b:21:94:e8:a4:36:ce:b9:93:9e:f4:db:3b:a6:70:
         f6:02:20:3e:99:4b:1d:6f:3e:c1:3d:0a:71:56:a3:0f:90:cb:
         ad:fc:01:63:3e:43:64:81:43:3c:16:45:69:77:27:76:ad
```
 Both the certificate and the private key are protected and can't be overwritten. A user **can** read the certificate out of the chip.

**More information about the PKI architecture can be found in the [official documentation](https://github.com/Infineon/optiga-trust-m/blob/master/documents/OPTIGA_Trust_M_V1_Keys_And_Certificates_v1.50.pdf)**
