# Infineon OPTIGA&trade; Trust M

[View this ReadMe online](https://github.com/Infineon/optiga-trust-m/blob/develop/README.md)

## Quick navigation

1. [Security Solution](#security-solution)
    * [Description](#description)
    * [Key Features and benefits](#key-features-and-benefits)
    * [Features table](#features-table)
    * [Provisioning Options (V1, V3, Express, MTR)](#provisioning-options-v1-v3-express-mtr)
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

This repository contains a target-agnostic Software Framework for the [OPTIGA™ Trust M](https://www.infineon.com/optiga-trust) security solution. It is a base for other application notes.

**Be aware that this software comes without any security claims and shall be used for evaluation purpose.**

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

### Provisioning Options (V1, V3, Express, MTR)

#### Configurations

There are three main provisioning options/confgiurations availble:

**Note: All three configuration options have no effect on the corresponding features. For example, OPTIGA&trade; Trust M Express has the same features as OPTIGA&trade; Trust M V3, also the same API and hostcode can be used to work with all three solutions.**

1. **OPTIGA&trade; Trust M V1/V3** - a provisioning configuration which comes as a standard for all shipped devices. Unless mentioned differently all OPTIGA&trade;Trust M chips on the market have this configuration.
    * [Sample OPTIGA&trade; Trust M V1 Open Objects Dump](https://github.com/Infineon/optiga-trust-m/files/9281936/trust_m1_json.txt)
    * [Sample OPTIGA&trade; Trust M V3 Open Objects Dump](https://github.com/Infineon/optiga-trust-m/files/9281926/trust_m3_json.txt)
    * [Product Webpage](https://www.infineon.com/cms/en/product/security-smart-card-solutions/optiga-embedded-security-solutions/optiga-trust/optiga-trust-m-sls32aia/)
2. **OPTIGA&trade; Trust M Custom** - a custom provisioning option done on demand upon reaching a MoQ. Fully customisable solution including Security Monitor Configuration 
    * Please get in touch with your local Infineon Sales Representative to get more information
3. **OPTIGA&trade; Trust M Express** - a provisioning configuration which can be ordered standalone. This variant comes with three certificates/private keys pre-provisioned by Infineon. Certificates and communication secrets data can be downloaded through the CIRRENT™ Cloud ID
    * [Sample OPTIGA&trade;Trust M Express Open Objects Dump](https://github.com/Infineon/optiga-trust-m/files/9281778/trust_express_sample2_json.txt)
    * [Product Webpage](https://www.infineon.com/cms/en/product/security-smart-card-solutions/optiga-embedded-security-solutions/optiga-trust/optiga-trust-m-express/)
4. **OPTIGA&trade; Trust M MTR** - a provisioning configuration which can be ordered standalone. This variant comes with three certificates/private keys pre-provisioned by Infineon. The first certificate and key are meant to be used for Matter Device Attestation. Certificates and communication secrets data can be downloaded through Kudelski keySTREAM.
    * [Sample OPTIGA&trade;Trust M MTR Open Objects Dump](https://github.com/Infineon/optiga-trust-m/files/14259804/optiga_trust_m_mtr_object_dump.txt)
    * [Product Webpage](https://www.infineon.com/cms/en/product/security-smart-card-solutions/optiga-embedded-security-solutions/optiga-trust/optiga-trust-m-mtr)

#### Comparison Table

<table>
<thead>
  <tr>
    <th></th>
    <th>V1</th>
    <th>V3</th>
    <th colspan="3">Express</th>
    <th colspan="3">MTR³ </th>
  </tr>
</thead>
<tbody>
  <tr>
    <td></td>
    <th>Certificate - </br>Private Key</th>
    <th>Certificate - </br>Private Key</th>
    <th colspan="3">Certificate¹ - Private Key</th>
    <th colspan="3">Certificate¹ - Private Key</th>
  </tr>
  <tr>
    <td><a href="https://github.com/Infineon/optiga-trust-m/wiki/Data-and-Key-Store-Overview" target="_blank" rel="noopener noreferrer">Object IDs</a></td>
    <td>E0E0 - E0F0</td>
    <td>E0E0 - E0F0</td>
    <td>E0E0 - E0F0</td>
    <td>E0E1 - E0F1</td>
    <td>E0E2 - E0FC</td>
    <td>E0E0 - E0F0</td>
    <td>E0E1 - E0F1</td>
    <td>E0E2 - E0FC</td>
  </tr>
  <tr>
    <td><a href="https://github.com/Infineon/optiga-trust-m/blob/develop/documents/OPTIGA_Trust_M_Keys_And_Certificates_v3.10.pdf" target="_blank" rel="noopener noreferrer">PKI</a> Top Level</td>
    <td><a href="https://github.com/Infineon/optiga-trust-m/raw/support/m1/certificates/Infineon%20ECC%20Root%20CA%20C%20v01%2000.crt" target="_blank" rel="noopener noreferrer">ECC Root CA1</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root  CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaRSARootCA2/OptigaRSARootCA2.crt" target="_blank" rel="noopener noreferrer">RSA Root CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaECCRootCA2/OptigaECCRootCA2.crt" target="_blank" rel="noopener noreferrer">ECC Root  CA2</a></td>
    <td><a href="https://pki.infineon.com/OptigaRSARootCA2/OptigaRSARootCA2.crt" target="_blank" rel="noopener noreferrer">RSA Root CA2</a></td>
  </tr>
  <tr>
    <td><a href="https://github.com/Infineon/optiga-trust-m/blob/develop/documents/OPTIGA_Trust_M_Keys_And_Certificates_v3.10.pdf" target="_blank" rel="noopener noreferrer">PKI</a> Intermediate Level</td>
    <td><a href="https://github.com/Infineon/optiga-trust-m/raw/support/m1/certificates/Infineon%20OPTIGA(TM)%20Trust%20M%20CA%20101.crt" target="_blank" rel="noopener noreferrer">Int. CA 101</a></td>
    <td><a href="https://github.com/Infineon/optiga-trust-m/blob/develop/certificates/Infineon%20OPTIGA(TM)%20Trust%20M%20CA%20300.crt" target="_blank" rel="noopener noreferrer">Int. CA 300</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustEccCA306/OptigaTrustEccCA306.crt" target="_blank" rel="noopener noreferrer">Int. CA 306</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustEccCA306/OptigaTrustEccCA306.crt" target="_blank" rel="noopener noreferrer">Int. CA 306</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustRsaCA309/OptigaTrustRsaCA309.crt" target="_blank" rel="noopener noreferrer">Int. CA 309</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustEccCA306/OptigaTrustEccCA306.crt" target="_blank" rel="noopener noreferrer">Int. CA 306</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustEccCA306/OptigaTrustEccCA306.crt" target="_blank" rel="noopener noreferrer">Int. CA 306</a></td>
    <td><a href="https://pki.infineon.com/OptigaTrustRsaCA309/OptigaTrustRsaCA309.crt" target="_blank" rel="noopener noreferrer">Int. CA 309</a></td>
  </tr>
  <tr>
    <td><a href="https://github.com/Infineon/optiga-trust-m/blob/develop/documents/OPTIGA_Trust_M_Keys_And_Certificates_v3.10.pdf" target="_blank" rel="noopener noreferrer">PKI</a> Bottom Level: Key Algorithm</td>
    <td>NIST P-256</td>
    <td>NIST P-256</td>
    <td>NIST P-256</td>
    <td>NIST P-256</td>
    <td>RSA2048</td>
    <td>NIST P-256</td>
    <td>NIST P-256</td>
    <td>RSA2048</td>
  </tr>
  <tr>
    <td>Possible to Readout</td>
    <td>Yes</td>
    <td>Yes</td>
    <td>Yes</td>
    <td>With PBS¹</td>
    <td>Yes</td>
    <td>Yes</td>
    <td>Yes</td>
    <td>Yes</td>
  </tr>
  <tr>
    <td>Possible to Update</td>
    <td>Only Certificate</td>
    <td>Only Certificate</td>
    <td>Only Certificate with PBS¹ and Auth.Ref.¹</td>
    <td>Only Certificate with PBS¹ and Auth.Ref.¹</td>
    <td>Only Certificate with PBS¹ and Auth.Ref.¹</td>
    <td>Only Certificate: Always, if LcsO < Op, else with PBS¹ and Auth.Ref.¹</td>
    <td>Only Certificate with PBS¹ and Auth.Ref.¹</td>
    <td>Only Certificate with PBS¹ and Auth.Ref.¹</td>
  </tr>
  <tr>
    <td>Default Lifecycle State</td>
    <td>Creation</td>
    <td>Creation</td>
    <td>Operational</td>
    <td>Operational</td>
    <td>Operational</td>
    <td>Initialization</td>
    <td>Operational</td>
    <td>Operational</td>
  </tr>
    <tr>
    <td>Common Name²</td>
    <td>Static</td>
    <td>Static</td>
    <td>Unique</td>
    <td>Unique</td>
    <td>Unique</td>
    <td>Unique</td>
    <td>Unique</td>
    <td>Unique</td>
  </tr>
</tbody>
</table>

¹ *Certificate, Platform Binding Secret (PBS) and the Authorization Reference (Auth.Ref.) can be downloaded from CIRRENT™ Cloud ID (Express) or Kudelski keySTREAM (MTR) by claiming a Reel QR- or Bar- Code*

² *End Device Certificate Common Name has either the same value across all devices (Static), or has a chip-unique value (Unique)*

³ *It is expected from the Customer to perform "late-stage provisioning" on the OPTIGA&trade; Trust M MTR chips, i.e. to download the Matter Certificates (DAC/PAI) from Kudelski keySTREAM and inject into dedicated slots on the OPTIGA&trade; Trust M*

In addition to the certificates and private keys each OPTIGA&trade; Trust M Express and OPTIGA&trade; Trust M MTR comes with a chip unique Platform Binding Secret¹ and an Authorization Reference¹. The latter are two unique per chip 64 bytes long data objects which serve the following purposes:

- Platform Binding Secret (PBS) used to establish a Shielded Connection between a Host MCU and OPTIGA&trade; Trust M. Should be transfered from the Cloud Service to the respective MCU to run a protected I2C connection; e.g. readout a protected Certificate located in the 0xE0E1 Object ID (see table above). For more details about  Shielded Connection read [here](https://github.com/Infineon/optiga-trust-m/wiki/Shielded-Connection-101).
- Authorization Reference (Auth. Ref.). Used to update/change Certificate, PBS and the Authorization Reference itself. Similar to the PBS shall be tranfered to the Host MCU to be used. Find more details in the [Solution Reference Manual](documents/OPTIGA™%20Trust%20M%20Solution%20Reference%20Manual.md)

## Get Started

### Boards

| [OPTIGA&trade; Trust M Evaluation Kit](https://github.com/Infineon/getstarted-optiga-trust-m/tree/main/xmc4800_evaluation_kit) | [OPTIGA&trade; Trust IoT Security Development Kit](https://github.com/Infineon/getstarted-optiga-trust-m/tree/main/psoc62_secure_development_kit) | [OPTIGA™ Trust M Shields in Combination with PSoC™ 62S2 Wi-Fi BT Pioneer Kit](https://github.com/Infineon/getstarted-optiga-trust-m/tree/main/psoc62_cy8ckit_mikrobus)
|     :---:      |     :---:      |     :---:      |
| <a href="https://github.com/Infineon/getstarted-optiga-trust-m/tree/main/xmc4800_evaluation_kit"><img src="https://github.com/Infineon/Assets/raw/master/Pictures/OPTIGA_Trust_M_Eval_Kit%2002.jpg" width="480"></a>   | <a href="https://github.com/Infineon/getstarted-optiga-trust-m/tree/main/psoc62_secure_development_kit"><img src="https://github.com/Infineon/Assets/raw/master/Pictures/optiga_trust_iot_secdevkit.png" width="480"></a>     |<a href="https://github.com/Infineon/getstarted-optiga-trust-m/tree/main/psoc62_cy8ckit_mikrobus"><img src="https://github.com/Infineon/Assets/raw/master/Pictures/PSoC62S2_OPTIGA_Trust_M_MikroBUS.png" width="480"></a>     |

Get started with the [OPTIGA™ Trust M evaluation kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-m-eval-kit/), with the [OPTIGA™ Trust IoT Security Development kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-m-iot-kit/) or with any one of the available OPTIGA&trade; Trust M Shields ([Shield2Go](https://www.infineon.com/cms/en/product/evaluation-boards/s2go-security-optiga-m/), [Express Shield](https://www.infineon.com/optiga-trust-m-express-shield), [MTR Shield](https://www.infineon.com/cms/en/product/evaluation-boards/trust-m-mtr-shield/)) in combination with the [PSoC&trade; 62S2 Wi-Fi BT Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062s2-43012/) using [this](https://github.com/Infineon/getstarted-optiga-trust-m) Application Note.

### Examples

1. [Get started guide](https://github.com/Infineon/getstarted-optiga-trust-m)
1. ModusToolbox™ Code Examples
    - [OPTIGA™ Trust M: Cryptography](https://github.com/Infineon/mtb-example-optiga-crypto)
    - [OPTIGA™ Trust M: MQTT Client](https://github.com/Infineon/mtb-example-optiga-mqtt-client)
    - [OPTIGA™ Trust M: Power management](https://github.com/Infineon/mtb-example-optiga-power-management)
    - [OPTIGA™ Trust M: Data management](https://github.com/Infineon/mtb-example-optiga-data-management)
1. [Off-Chip TLS example (mbedTLS)](https://github.com/Infineon/mbedtls-optiga-trust-m)
1. [Linux Command Line Interface](https://github.com/Infineon/linux-optiga-trust-m)
1. Cloud:
    1. [AWS FreeRTOS example](https://github.com/Infineon/amazon-freertos-optiga-trust)
    1. [Microsoft Azure IoT example](https://github.com/Infineon/azure-esp32-optiga-trust/)
1. [Zephyr OS driver](https://github.com/Infineon/zephyr)
1. [Arduino library](https://github.com/Infineon/arduino-optiga-trust-m)
1. [Personalize OPTIGA™ Trust](https://github.com/Infineon/personalize-optiga-trust)
1. [Python package](https://github.com/Infineon/python-optiga-trust)
1. [I2C Utilities](https://github.com/Infineon/i2c-utils-optiga-trust)

### Software Framework overview

![](https://github.com/Infineon/Assets/raw/master/Pictures/optiga_trust_m_system_block_diagram_v4.png)

1. See [Trust M Crypt API](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Crypt-API) and [Trust M Util API](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Util-API)  to know more about CRYPT and UTIL modules
2. Information about the OPTIGA™ Trust M Command Library (CMD) can be found in the [Solution Reference Manual](documents/OPTIGA_Trust_M_Solution_Reference_Manual_v3.60.pdf)
    In the same document you can find explanation of all Object IDs (OIDs) available for users as well as detailed technical explanation for all features and envisioned use cases.
3. Infineon I<sup>2</sup>C Protocol implementation details can be found [here](documents/Infineon_I2C_Protocol_v2.03.pdf)
4. Platform Abstraction Layer (PAL) overview and Porting Guide are presented in the [Wiki](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide)

For more information please refer to the [Wiki page](https://github.com/Infineon/optiga-trust-m/wiki) of this project

### Evaluation and developement kits

External links, open in the same tab.

* [OPTIGA™ Trust M evaluation kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-m-eval-kit/)
* [OPTIGA™ Trust M Shield2Go](https://www.infineon.com/cms/en/product/evaluation-boards/s2go-security-optiga-m/)

#### Notes to the S2Go Security OPTIGA&trade;Trust M

* Supply voltage VCC is max. 5.5 V, please refer to the OPTIGA™ Trust M datasheet for more details about maximum ratings
* Ensure that no voltage applied to any of the pins exceeds the absolute maximum rating of VCC + 0.3 V
* Pin out on top (head) is directly connected to the pins of the OPTIGA™ Trust M
* If head is broken off, only one capacitor is connected to the OPTIGA™ Trust M

  <details>
  <summary> S2Go Security OPTIGA&trade; M Pinout</summary>
  <img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_shield2go_pinout.png" alt="drawing"/>
  </details>

  <details>
  <summary> S2Go Security OPTIGA&trade; M Schematic</summary>
  <img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_shield2go_schematics.png" alt="drawing"/>
  </details>

#### Notes to the OPTIGA&trade;Trust M [Variant] Shields

* The product variant can be identified through the configuration LED.
* The design of the Shield is for 3V3 VCC
* Absolute max. rating of VCC is 5.5 V, please refer to the OPTIGA™ Trust M datasheet for more details about maximum ratings
* Ensure that no voltage applied to any of the pins exceeds the absolute maximum rating of VCC + 0.3 V
* Pin out of the shield is directly connected to the pins of the OPTIGA™ Trust M

  <details>
  <summary> OPTIGA&trade; Trust M [Variant] Shield Pinout</summary>
  <img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_shield_pinout.png" alt="drawing"/>
  </details>

  <details>
  <summary> OPTIGA&trade; Trust M [Variant] Shield Schematic</summary>
  <img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_shield_mbus_schematics.png" alt="drawing"/>
  </details>

## Documentation

### Usefull articles

* [Initialisation hints (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Initialisation-hints)
* [Porting guide (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide)
* [Crypto performance (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Crypto-Performance)
* [In which form does OPTIGA&trade;return keys and signatures? (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Data-format-examples)
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

1. [OPTIGA™ Trust M Datasheet v3.61](documents/OPTIGA_Trust_M_Datasheet_v3.61.pdf) (PDF)
2. [OPTIGA™ Trust M Solution Reference Manual v3.60](documents/OPTIGA_Trust_M_Solution_Reference_Manual_v3.60.pdf) (PDF)
3. [OPTIGA™ Trust M Keys and Certificates v3.10](documents/OPTIGA_Trust_M_Keys_And_Certificates_v3.10.pdf) (PDF)
4. [OPTIGA™ Trust Config Guide v2.20](documents/OPTIGA_Trust_M_ConfigGuide_v2.2.pdf) (PDF)
5. [Infineon I2C protocol specification v2.03](documents/Infineon_I2C_Protocol_v2.03.pdf) (PDF)

### Board assembly recommendations

If you are planning to integrate OPTIGA™ Trust M in your PCB design have a look at the recommendations found [here (external, opens in the same tab)](https://www.infineon.com/dgdl/Infineon-Additional_product_information_SON_packages-AN-v00_01-EN.pdf?fileId=db3a30433e82b1cf013e82faab2000e5).


## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
