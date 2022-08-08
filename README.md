# OPTIGA&trade; Trust M Software Framework

## Quick navigation

1. [Security Solution](#security-solution)
    * [Description](#description)
    * [Key Features and benefits](#key-features-and-benefits)
    * [Features table](#features-table)
    * [Provisioning Options](#provisioning-options)
2. [Get Started](#get-started)
    * [Boards](#boards)
    * [Examples](#examples)
    * [Software Framework overview](#software-framework-overview)
    * [Evaluation and developement kits](#evaluation-and-developement-kits)
3. [Documentation](#documentation)
    * [Usefull articles](#usefull-articles)
    * [Datasheet and Co.](#datasheet-and-co)
    * [Board assembly recommendations](#board-assembly-recommendations)
4. [Contributing](#contributing)
5. [License](#license)

## Security Solution

### Description

<img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_uson10.png" width="300" >

This repository contains a target-agnostic Software Framework for the [OPTIGA™ Trust M](https://www.infineon.com/optiga-trust) security chip. It is a base for other application notes. **Be aware that this software comes without any security claims and shall be used for evaluation purpose.**

### Key Features and Benefits
* High-end security controller
* Common Criteria Certified EAL6+ (high) hardware
* Turnkey solution
* Up to 10kB user memory
* PG-USON-10 package (3 x 3 mm)
* Temperature range (−40°C to +105°C)
* I2C interface with Shielded Connection (encrypted communication)
* Cryptographic support: 
  * ECC : NIST curves up to P-521, Brainpool r1 curve up to 512,
  * RSA® up to 2048
  * AES key up to 256 , HMAC up to SHA512
  * TLS v1.2 PRF and HKDF up to SHA512
* Crypto ToolBox commands for SHA-256, ECC and RSA® Feature, AES, HMAC and Key derivation
* Configurable device security monitor, 4 Monotonic up counters
* Protected(integrity and confidentiality) update of data, key and metadata objects
* Hibernate for zero power consumption
* Lifetime for Industrial Automation and Infrastructure is 20 years and 15 years for other Application Profiles  

### Features table

<table class="tg">
  <tr>
    <th class="tg-fymr">Features</th>
    <th class="tg-fymr">Supported Curve/Algorithm</th>
    <th class="tg-fymr">V1</th>
    <th class="tg-fymr">V3</th>
  </tr>
  <tr>
    <td class="tg-c3ow" rowspan="2">ECC</td>
    <td class="tg-0pky">ECC NIST P256/384 </td>
    <td class="tg-0pky">✓</td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-0pky">ECC NIST P521, ECC Brainpool P256/384/512 r1</td>
    <td class="tg-0pky"></td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow">RSA</td>
    <td class="tg-0pky">RSA® 1024/2048 </td>
    <td class="tg-0pky">✓</td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow" rowspan="3">Key Derivation</td>
    <td class="tg-0pky">TLS v1.2 PRF SHA 256 </td>
    <td class="tg-0pky">✓</td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-0pky">TLS v1.2 PRF SHA 384/512 </td>
    <td class="tg-0pky"> </td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-0pky">HKDF SHA-256/384/512  </td>
    <td class="tg-0pky"> </td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow">AES</td>
    <td class="tg-0pky">Key size - 128/192/256 (ECB, CBC, CBC-MAC, CMAC)</td>
    <td class="tg-0pky"> </td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Random Generation</td>
    <td class="tg-0pky">TRNG, DRNG, Pre-Master secret for RSA® Key exchange</td>
    <td class="tg-0pky">✓</td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow">HMAC</td>
    <td class="tg-0pky">HMAC with SHA256/384/512 </td>
    <td class="tg-0pky"> </td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Hash </td>
    <td class="tg-0pky">SHA256 </td>
    <td class="tg-0pky">✓</td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow" rowspan="2">Protected data (object) update (Integrity)</td>
    <td class="tg-0pky">ECC NIST P256/384</br>RSA® 1024/2048 </br>Signature scheme as ECDSA FIPS 186-3/RSA SSA PKCS#1 v1.5 without hashing </td>
    <td class="tg-0pky">✓</td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow">ECC NIST P521,</br>ECC Brainpool P256/384/512 r1</br>Signature scheme as ECDSA FIPS 186-3/RSA SSA PKCS#1 v1.5 without hashing</td>
    <td class="tg-0pky"> </td>
    <td class="tg-0pky">✓</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Protected Data/key/metadata update (Integrity and/or confidentiality)</td>
    <td class="tg-0pky">ECC NIST P256/384/521</br>ECC Brainpool P256/384/512 r1</br>RSA® 1024/2048</br>Signature scheme as ECDSA FIPS 186-3/RSA SSA PKCS#1 v1.5 without hashing</td>
    <td class="tg-0pky"> </td>
    <td class="tg-0pky">✓</td>
  </tr>
</table>

### Provisioning Options

#### Confgiurations

There are three main provisioning options/confgiurations availble:

1. **OPTIGA&trade; Trust M V1/V3** - a provisioning configuration which comes as a standard for all shipped devices. Unless mentioned differently all OPTIGA Trust M chips on the market have this configuration. 
    * [Sample OPTIGA&trade; Trust M V1 Certificate](https://github.com/Infineon/optiga-trust-m/files/9281936/trust_m1_json.txt)
    * [Sample OPTIGA&trade; Trust M V3 Certificate](https://github.com/Infineon/optiga-trust-m/files/9281926/trust_m3_json.txt)
2. **Custom** - a custom provisioning option done on demand upon reaching a MoQ. Fully customisable solution including Security Monitor Configuration  
3. **Express** - a provisioning configuration which can be ordered standalone. Credentials and unique data can be downlaoded through CIRRENT™ Cloud ID
    * [Sample OPTIGA Trust M Express Certificate](https://github.com/Infineon/optiga-trust-m/files/9281778/trust_express_sample2_json.txt)

#### Comparison Table

<table>
<thead>
  <tr>
    <th></th>
    <th>OPTIGA™ Trust M V1</th>
    <th>OPTIGA™ Trust M V3</th>
    <th colspan="5">OPTIGA™ </br>Trust M Express</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td></td>
    <th>Certificate - Private Key</th>
    <th>Certificate - Private Key</th>
    <th colspan="3">Certificate - </br>Private Key*</th>
    <th>Platform Binding Secret*</th>
    <th>Authorization Reference*</th>
  </tr>
  <tr>
    <th>Object IDs</th>
    <td>E0E0 - E0F0</td>
    <td>E0E0 - E0F0</td>
    <td>E0E0 - E0F0</td>
    <td>E0E1 - E0F1</td>
    <td>E0E2 - E0FC</td>
    <td>E140</td>
    <td>F1D0</td>
  </tr>
  <tr>
    <th>Encoding</th>
    <td>X509 ASN.1 DER</td>
    <td>X509 ASN.1 DER</td>
    <td>X509 ASN.1 DER</td>
    <td>X509 ASN.1 DER</td>
    <td>X509 ASN.1 DER</td>
    <td>64 random bytes</td>
    <td>64 random bytes</td>
  </tr>
  <tr>
    <th>Level 0: Root CA</th>
    <td><a href="https://github.com/Infineon/optiga-trust-m/raw/support/m1/certificates/Infineon%20ECC%20Root%20CA%20C%20v01%2000.crt" target="_blank" rel="noopener noreferrer">ECC Root CA1</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root  CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaRSARootCA2/OptigaRSARootCA2.crt" target="_blank" rel="noopener noreferrer">RSA Root CA2</a></td>
    <td>N/A</td>
    <td>N/A</td>
  </tr>
  <tr>
    <th>Level 1: Intermediate CA</th>
    <td><a href="https://github.com/Infineon/optiga-trust-m/raw/support/m1/certificates/Infineon%20OPTIGA(TM)%20Trust%20M%20CA%20101.crt" target="_blank" rel="noopener noreferrer">Int. CA 101</a></td>
    <td><a href="https://github.com/Infineon/optiga-trust-m/blob/develop/certificates/Infineon%20OPTIGA(TM)%20Trust%20M%20CA%20300.crt" target="_blank" rel="noopener noreferrer">Int. CA 300</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustEccCA306/OptigaTrustEccCA306.crt" target="_blank" rel="noopener noreferrer">Int. CA 306</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustEccCA306/OptigaTrustEccCA306.crt" target="_blank" rel="noopener noreferrer">Int. CA 306</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustRsaCA309/OptigaTrustRsaCA309.crt" target="_blank" rel="noopener noreferrer">Int. CA 309</a></td>
    <td>N/A</td>
    <td>N/A</td>
  </tr>
  <tr>
    <th>Level 2: Certificate Key Algorithm</th>
    <td>NIST P-256 (secp256r1)</td>
    <td>NIST P-256 (secp256r1)</td>
    <td>NIST P-256 (secp256r1)</td>
    <td>NIST P-256 (secp256r1)</td>
    <td>RSA2048</td>
    <td>N/A</td>
    <td>N/A</td>
  </tr>
  <tr>
    <th>Possible to Readout</th>
    <td>Yes</td>
    <td>Yes</td>
    <td>Yes</td>
    <td>With Shielded Connection</td>
    <td>Yes</td>
    <td>No</td>
    <td>No</td>
  </tr>
  <tr>
    <th>Possible to Update</th>
    <td>Only Cert</td>
    <td>Only Cert</td>
    <td>Only Cert</td>
    <td>Only Cert</td>
    <td>Only Cert</td>
    <td>Yes</td>
    <td>Yes</td>
  </tr>
  <tr>
    <th>Default Lifecycle State</th>
    <td>Creation</td>
    <td>Creation</td>
    <td>Operational</td>
    <td>Operational</td>
    <td>Operational</td>
    <td>Operational</td>
    <td>Operational</td>
  </tr>
</tbody>
</table>

\* Can be downloaded from CIRRENT™ Cloud ID by claiming a Reel QR Code 

## Get Started

### Boards

| [OPTIGA&trade; Trust M Evaluation Kit](https://github.com/Infineon/getstarted-optiga-trust-m/wiki/Get-Started-with-OPTIGA%E2%84%A2-Trust-M-Evaluation-Kit) | [OPTIGA&trade; Trust IoT Security Development Kit](https://github.com/Infineon/getstarted-optiga-trust-m/wiki/Get-Started-with-OPTIGA%E2%84%A2-Trust-M-IoT-Security-Development-Kit) |
|     :---:      |     :---:      |
| <a href="https://github.com/Infineon/getstarted-optiga-trust-m/wiki/Get-Started-with-OPTIGA%E2%84%A2-Trust-M-IoT-Security-Development-Kit"><img src="https://github.com/Infineon/Assets/raw/master/Pictures/OPTIGA_Trust_M_Eval_Kit%2002.jpg" width="480" height="326"></a>   | <a href="https://github.com/Infineon/getstarted-optiga-trust-m/wiki/Get-Started-with-OPTIGA%E2%84%A2-Trust-IoT-Security-Development-Kit"><img src="https://github.com/Infineon/Assets/raw/master/Pictures/optiga_trust_iot_secdevkit.png" width="480" height="318"></a>     |

Get started with the [OPTIGA™ Trust M evaluation kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-m-eval-kit/) or with the [OPTIGA™ Trust IoT Security Development kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-m-iot-kit/) using [this](https://github.com/Infineon/getstarted-optiga-trust-m) Application Note

### Examples

1. [Get started guide](https://github.com/Infineon/getstarted-optiga-trust-m)
1. [Off-Chip TLS example (mbedTLS)](https://github.com/Infineon/mbedtls-optiga-trust-m)
1. Cloud:
    1. [AWS FreeRTOS example](https://github.com/Infineon/amazon-freertos-optiga-trust)
    1. [Microsoft Azure IoT example](https://github.com/Infineon/azure-optiga-trust-m)
1. [Zephyr OS driver](https://github.com/Infineon/zephyr)
1. Secure Firmware Update and Secure Boot (link is pending)
1. [Arduino library](https://github.com/Infineon/arduino-optiga-trust-m)
1. [Personalize OPTIGA™ Trust](https://github.com/Infineon/personalize-optiga-trust)
1. [OpenSSL Engine Command Line Interface and AWS IoT C SDK (for RPi3)](https://github.com/Infineon/cli-optiga-trust-m)
1. [Python package](https://github.com/Infineon/python-optiga-trust)
1. [I2C Utilities](https://github.com/Infineon/i2c-utils-optiga-trust)

### Software Framework overview

![](https://github.com/Infineon/Assets/raw/master/Pictures/optiga_trust_m_system_block_diagram_v3.png)

1. See [Trust M Crypt API](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Crypt-API) and [Trust M Util API](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Util-API)  to know more about CRYPT and UTIL modules
2. Information about the OPTIGA™ Trust M Command Library (CMD) can be found in the [Solution Reference Manual](documents/OPTIGA_Trust_M_Solution_Reference_Manual_v3.00.pdf)
    In the same document you can find explanation of all Object IDs (OIDs) available for users as well as detailed technical explanation for all features and envisioned use cases.
3. Infineon I<sup>2</sup>C Protocol implementation details can be found [here](documents/Infineon_I2C_Protocol_v2.02.pdf)
4. Platform Abstraction Layer (PAL) overview and Porting Guide are presented in the [Wiki](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide)

For more information please refer to the [Wiki page](https://github.com/Infineon/optiga-trust-m/wiki) of this project

### Evaluation and developement kits

External links, open in the same tab.
* [OPTIGA™ Trust M evaluation kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-m-eval-kit/)
* [OPTIGA™ Trust M Shield2Go](https://www.infineon.com/cms/en/product/evaluation-boards/s2go-security-optiga-m/)
  Notes to the S2Go Security OPTIGA M:
  * Supply voltage VCC is max. 5.5 V, please refer to the OPTIGA™ Trust M datasheet for more details about maximum ratings
  * Ensure that no voltage applied to any of the pins exceeds the absolute maximum rating of VCC + 0.3 V
  * Pin out on top (head) is directly connected to the pins of the OPTIGA™ Trust M
  * If head is broken off, only one capacitor is connected to the OPTIGA™ Trust M

  <details>
  <summary> S2Go Security OPTIGA M Pinout</summary>
  <img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_shield2go_pinout.png" alt="drawing"/>
  </details>

  <details>
  <summary> S2Go Security OPTIGA M Schematic</summary>
  <img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_shield2go_schematics.png" alt="drawing"/>
  </details>

## Documentation

### Usefull articles

  * [Initialisation hints (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Initialisation-hints)
  * [Porting guide (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide)
  * [Crypto performance (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Crypto-Performance)
  * [In which form does OPTIGA return keys and signatures? (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Data-format-examples)
  * [Code Footprint (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Code-Footprint)
  * [Device Error Codes (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Device-Error-Codes)
  * [Protected Update for Data Objects](https://github.com/Infineon/optiga-trust-m/wiki/Protected-Update-for-Data-Objects)
  * [Shielded Connection (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Shielded-Connection-101)
  * **User API**
      * [Crypt API (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Crypt-API)
      * [Util API (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Util-API)
      
  * [Hardware-Security: "Einfach (und) Sicher" (external link, opens in the same tab)](https://vimeo.com/279839814) in German, [Slides](https://github.com/Infineon/Assets/blob/master/PDFs/2018-06-04_Building-IoT_HW-Sec_Lesjak_vFinal.pdf) in English

### Datasheet and Co.

For high level description and some important excerpts from the documentation please refer to [Wiki page](https://github.com/Infineon/optiga-trust-m/wiki)

Other downloadable PDF documents can be found below:
1. [OPTIGA™ Trust M Datasheet v3.40](documents/OPTIGA_Trust_M_Datasheet_v3.40.pdf) (PDF)
2. [OPTIGA™ Trust M Solution Reference Manual v3.15](documents/OPTIGA_Trust_M_Solution_Reference_Manual_v3.15.pdf) (PDF)
3. [OPTIGA™ Trust M Keys and Certificates v3.10](documents/OPTIGA_Trust_M_Keys_And_Certificates_v3.10.pdf) (PDF)
4. [Infineon I2C protocol specification v2.03](documents/Infineon_I2C_Protocol_v2.03.pdf) (PDF)

### Board assembly recommendations

If you are planning to integrate OPTIGA™ Trust M in your PCB design have a look at the recommendations found [here (external, opens in the same tab)](https://www.infineon.com/dgdl/Infineon-Additional_product_information_SON_packages-AN-v00_01-EN.pdf?fileId=db3a30433e82b1cf013e82faab2000e5).


## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
