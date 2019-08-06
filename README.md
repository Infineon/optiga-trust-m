 ![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+) **For Engineering Samples** ![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)

# OPTIGA&trade; Trust M1 Software Framework

[tocstart]: # (toc start)

* [Description](#description)
* [Summary](#summary)
* [Key Features and Benefits](#key_features_and_benefits)
* [Evaluation and developement kits](#evaluation_development_kits)
* [Get Started](#get_started)
* [Overview](#oveview)
* [Documentation](#documentation)
* [Porting Guide](#porting_guide)
* [Contributing](#contributing)
* [License](#license)

[tocend]: # (toc end)

## <a name="description"></a>Description

<img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_uson10.png" width="320">

This repository contains a target-agnostic Software Framework for the OPTIGA™ Trust M1 security chip.

## <a name="summary"></a>Summary
As embedded systems (e.g. IoT devices) are increasingly gaining the attention of attackers, Infineon offers the OPTIGA™ Trust M1 as a turnkey security solution for industrial automation systems, smart homes, consumer devices and medical devices. This high-end security controller comes with full system integration support for easy and cost-effective deployment of high-end security for your assets.
The [OPTIGA™ Trust M1](documents/OPTIGA_Trust_M1_Datasheet_v1.65.pdf) is a security solution based on a secure microntroller. Each device is shipped with a unique elliptic-curve or RSA keypair and a corresponding X.509 certificate.
OPTIGA™ Trust M1 enables easy integration into existing PKI infrastructure.

## <a name="key_features_and_benefits"></a>Key Features and Benefits
* High-end security controller
* Common Criteria Certified EAL6+ (high) hardware
* Turnkey solution
* Up to 10kB user memory
* PG-USON-10-2 package (3 x 3 mm)
* Temperature range (−25°C to +105°C)
* I2C interface with Shielded Connection (encrypted communication)
* Cryptographic support: **ECC NIST P256/P384**, **SHA-256**, **TRNG**, **DRNG**, **RSA® 1024/2048**
* OPTIGA™ Trust M1 Software Framework on Github (https://github.com/infineon)
* Crypto ToolBox commands with **ECC NIST P256/P384**, **SHA-256** (sign, verify, key generation, ECDH(E), key
derivation), **RSA® 1024/2048** (sign, verify, key generation, encrypt and decrypt)
* Device Security Monitor
* Hibernate for zero power consumption
* Lifetime for Industrial Automation and Infrastructure is 20 years and 15 years for other Application Profiles  

## <a name="evaluation_development_kits"></a>Evaluation and developement kits
* OPTIGA™ Trust M1 Evaluation kit (pending)
* OPTIGA™ Trust M1 Security Shield2Go (pending)

## <a name="get_started"></a>Get Started
1. Get started guide (pending)
1. [Personalize OPTIGA™ Trust](https://github.com/Infineon/personalize-optiga-trust)

## <a name="oveview"></a>Overview

![](https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_system_block_diagram.jpg)

## <a name="documentation"></a>Documentation
For high level description and some important excerpts from the documentation please refer to [Wiki page](https://github.com/Infineon/optiga-trust-m/wiki)

Other downloadable PDF documents can be found below:
4. [Release Notes v1.30](documents/OPTIGA_Trust_M1_Release_Notes_v1.30.pdf) (PDF)
1. [Datasheet v1.65](documents/OPTIGA_Trust_M1_Datasheet_v1.65.pdf) (PDF)
2. [Host Library Documentation](documents/OPTIGA_Trust_M1_Host_Library_Documentation.chm) (CHM)
3. [Solution Reference Manual v1.12](documents/OPTIGA_Trust_M1_Solution_Reference_Manual_v1.12.pdf) (PDF)
3. [Keys and Certificates v1.40](documents/OPTIGA_Trust_M1_Keys_And_Certificates_v1.40.pdf) (PDF)
4. [Infineon I<sup>2</sup>C protocol specification v2.02](documents/Infineon_I2C_Protocol_v2.02.pdf) (PDF)

## <a name="porting_guide"></a>Porting Guide
You might find a list of supported Platforms [here](pal). These are:
* Infineon XMC4500

For other platform please refer to our porting guide on [Wiki](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide) page

## <a name="contributing"></a>Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## <a name="license"></a>License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
