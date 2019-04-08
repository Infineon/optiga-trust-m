![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+) **For Engineering Samples** ![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)![#f03c15](https://placehold.it/15/f03c15/000000?text=+)

# OPTIGA&trade; Trust M1 Software Framework

[tocstart]: # (toc start)
 
1. **Security Chip**
    * [Description](#description)
    * [Summary](#summary)
    * [Key Features and Benefits](#key_features_and_benefits)
2. **Get Started**
    * [Software Framework overview](#software_framework_oveview)
    * [Evaluation and developement kits](#evaluation_development_kits)
    * [Application Notes](#application_notes)
    * [Documentation](#documentation)
3. **Working with the Software Framework**
    * [Initialisation hints (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Initialisation-hints)
    * [Porting guide (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Porting-Guide)
    * [Error Codes (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Device-Error-Codes)
    * [Crypto performance (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Crypto-Performance)
    * [Shielded Connection 101 (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Shielded-Connection-101)
    * **User API**
        * [Crypt API (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M1-Crypt-API)
        * [Util API (Wiki)](https://github.com/Infineon/optiga-trust-m/wiki/Trust-M1-Util-API)
4. **General**
    * [Contributing](#contributing)
    * [License](#license)

[tocend]: # (toc end)

## Security Chip

### <a name="description"></a>Description

<img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_uson10.png" width="320">

This repository contains a target-agnostic Software Framework for the OPTIGA™ Trust M1 security chip.

### <a name="summary"></a>Summary
As embedded systems (e.g. IoT devices) are increasingly gaining the attention of attackers, Infineon offers the OPTIGA™ Trust M1 as a turnkey security solution for industrial automation systems, smart homes, consumer devices and medical devices. This high-end security controller comes with full system integration support for easy and cost-effective deployment of high-end security for your assets.
The [OPTIGA™ Trust M1](documents/OPTIGA_Trust_M1_Datasheet_v1.50.pdf) is a security solution based on a secure microntroller. Each device is shipped with a unique elliptic-curve or RSA keypair and a corresponding X.509 certificate.
OPTIGA™ Trust M1 enables easy integration into existing PKI infrastructure.

### <a name="key_features_and_benefits"></a>Key Features and Benefits
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

## Get Started

### <a name="#software_framework_oveview"></a>Software Framework oveview

![](https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_system_block_diagram.jpg)

### <a name="evaluation_development_kits"></a>Evaluation and developement kits
* OPTIGA™ Trust M1 Evaluation kit (pending)
* OPTIGA™ Trust M1 Security Shield2Go (pending)

### <a name="application_notes"></a>Application Notes
1. Get started guide (pending)
2. [I2C Utilities for OPTIGA™ Trust secure IC](https://github.com/Infineon/i2c-utils-optiga-trust)

### <a name="documentation"></a>Documentation
For high level description and some important excerpts from the documentation please refer to [Wiki page](https://github.com/Infineon/optiga-trust-m/wiki)

Other downloadable PDF documents can be found below:

1. [Release Notes v1.0](documents/OPTIGA_Trust_M1_Release_Notes_v1.00.pdf) (PDF)
2. [Datasheet v1.5](documents/OPTIGA_Trust_M1_Datasheet_v1.50.pdf) (PDF)
3. [Host Library Documentation](documents/OPTIGA_Trust_M1_Host_Library_Documentation.chm) (CHM)
4. [Solution Reference Manual v1.0](documents/OPTIGA_Trust_M1_Solution_Reference_Manual_v1.00.pdf) (PDF)
5. [Keys and Certificates v1.2](documents/OPTIGA_Trust_M1_Keys_And_Certificates_v1.2.pdf) (PDF)
6. [Infineon I<sup>2</sup>C protocol specification v2.02](documents/Infineon_I2C_Protocol_v2.02.pdf) (PDF)

## General

### <a name="contributing"></a>Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

### <a name="license"></a>License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
