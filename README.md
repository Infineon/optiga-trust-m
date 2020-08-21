# OPTIGA&trade; Trust M V1 Software Framework

## Quick navigation

<a href="https://github.com/Infineon/optiga-trust-m#description"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_1.jpg" width="256" height="192"></a> <a href="https://github.com/Infineon/optiga-trust-m#examples"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_2.jpg" width="256" height="192"></a>  <a href="https://github.com/Infineon/optiga-trust-m#get-started"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_3.jpg" width="256" height="192"></a>

<a href="https://github.com/Infineon/optiga-trust-m#documentation"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_4.jpg" width="256" height="192"></a> <a href="https://github.com/Infineon/optiga-trust-m/wiki/Crypto-Performance"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_5.jpg" width="256" height="192"></a>  <a href="https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Crypt-API"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_6.jpg" width="256" height="192"></a>

## Security Chip

### <a name="description"></a>Description

<img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_uson10.png" width="300" >

This repository contains a target-agnostic Software Framework for the [OPTIGA™ Trust M](https://www.infineon.com/optiga-trust) security chip. It is a base for other application notes.

### <a name="key_features_and_benefits"></a>Key Features and Benefits
* High-end security controller
* Common Criteria Certified EAL6+ (high) hardware
* Turnkey solution
* Up to 10kB user memory
* PG-USON-10-2 package (3 x 3 mm)
* Temperature range (−40°C to +105°C)
* I2C interface with Shielded Connection (encrypted communication)
* Cryptographic support: **ECC NIST P256/P384**, **SHA-256**, **TRNG**, **DRNG**, **RSA® 1024/2048**
* OPTIGA™ Trust M Software Framework on Github
* Crypto ToolBox commands with **ECC NIST P256/P384**, **SHA-256** (sign, verify, key generation, ECDH(E), key
derivation), **RSA® 1024/2048** (sign, verify, key generation, encrypt and decrypt)
* Device Security Monitor
* Hibernate for zero power consumption
* Lifetime for Industrial Automation and Infrastructure is 20 years and 15 years for other Application Profiles    

## Get Started

### OPTIGA™ Trust M evaluation kit
<img src="https://github.com/Infineon/Assets/raw/master/Pictures/OPTIGA_Trust_M_Eval_Kit%2002.jpg" title="OPTIGA™ Trust M evaluation kit" width="600"> 

### XCM1400 XTREME Connectivity Kit
<img src="https://github.com/Infineon/Assets/blob/master/Pictures/xtreme-connectivity-kit.png" title="XCM1400 XTREME Connectivity Kit" width="400">

The [OPTIGA™ Trust M evaluation kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-m-eval-kit/) and [XCM1400 XTREME Connectivity Kit]() are a starting point for a lot of application notes mentioned below.

### <a name="examples"></a>Examples

1. [Get started guide](https://github.com/Infineon/getstarted-optiga-trust-m)
1. [Off-Chip TLS example (mbedTLS)](https://github.com/Infineon/mbedtls-optiga-trust-m)
1. [AWS FreeRTOS example](https://github.com/Infineon/amazon-freertos-optiga-trust)
1. [Zephyr OS driver](https://github.com/Infineon/zephyr-optiga-trust/tree/add_optiga_m/samples/drivers/optiga)
1. Secure Firmware Update and Secure Boot (link is pending)
1. [Arduino library](https://github.com/Infineon/arduino-optiga-trust-m)
1. [Personalize OPTIGA™ Trust](https://github.com/Infineon/personalize-optiga-trust)
1. [OpenSSL Engine and Command Line Interface (for RPi3)](https://github.com/Infineon/cli-optiga-trust-m)
1. [Python package](https://github.com/Infineon/python-optiga-trust)
1. [I2C Utilities](https://github.com/Infineon/i2c-utils-optiga-trust)

### <a name="software_framework_oveview"></a>Software Framework overview

![](https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_system_block_diagram.png)

1. See [Trust M rev.1 Crypt API](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Crypt-API) and [Trust M rev. 1 Util API](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M-Util-API)  to know more about CRYPT and UTIL modules
2. Information about the OPTIGA™ Trust M rev. 1 Command Library (CMD) can be found in the [Solution Reference Manual](https://github.com/Infineon/optiga-trust-m/blob/master/documents/OPTIGA_Trust_M_V1_Solution_Reference_Manual_v1.13.pdf)
In the same document you can find explanation of all Object IDs (OIDs) available for users as well as detailed technical explanation for all features and envisioned use cases.
3. Infineon I<sup>2</sup>C Protocol implementation details can be found [here](documents/Infineon_I2C_Protocol_v2.02.pdf)
4. Platform Abstraction Layer (PAL) overview and Porting Guide are presented in the [Wiki](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide)

For more information please refer to the [Wiki page](https://github.com/Infineon/optiga-trust-m/wiki) of this project

### <a name="evaluation_development_kits"></a>Evaluation and developement kits

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
* [XMC1400 OPTIGA™ Trust M Connectivity Kit](link is pending)
* [OPTIGA™ Trust M Feather Wing™](link is pending)

## <a name="documentation"></a>Documentation

### Usefull articles

  * [Initialisation hints (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Initialisation-hints)
  * [Porting guide (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide)
  * [Crypto performance (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Crypto-Performance)
  * [Data and Key Store Overview](https://github.com/Infineon/optiga-trust-m/wiki/Data-and-Key-Store-Overview)
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
1. [OPTIGA™ Trust M rev.1 Datasheet v1.72](documents/OPTIGA_Trust_M_V1_Datasheet_v1.72.pdf) (PDF)
2. [OPTIGA™ Trust M rev.1 Solution Reference Manual v1.13](documents/OPTIGA_Trust_M_V1_Solution_Reference_Manual_v1.13.pdf) (PDF)
3. [OPTIGA™ Trust M rev.1 Keys and Certificates v1.50](documents/OPTIGA_Trust_M_V1_Keys_And_Certificates_v1.50.pdf) (PDF)
4. [Infineon I2C protocol specification v2.02](documents/Infineon_I2C_Protocol_v2.02.pdf) (PDF)
5. [XMC1400_XTREME_Connectivity_Kit_Users_Guide v1.10](documents/XMC1400_XTREME_Connectivity_Kit_Users_Guide_v1.10.pdf) (PDF)

### Board assembly recommendations

If you are planning to integrate OPTIGA™ Trust M in your PCB design have a look at the recommendations found [here (external, opens in the same tab)](https://www.infineon.com/dgdl/Infineon-Recommendations_for_Board_Assembly_xQFN-P-v01_00-EN.pdf?fileId=5546d462580663ef015806ab383a05bf).


## <a name="contributing"></a>Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## <a name="license"></a>License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## <a name="disclaimer"></a>Disclaimer
Be aware that this software comes without any security claims and shall be used for evaluation purpose.
