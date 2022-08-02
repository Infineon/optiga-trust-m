# What document do I need?

* **Infineon_I2C_Protocol_vX.XX.pdf** - Infineon I2C protocol specification. In this document you can find all information related to the communication protocol between a Host MCU and the Secure Element, for instance how shielded connection works.
* **License.pdf** - MIT License
* **OPTIGA_Trust_M_Datasheet_vX.XX.pdf** - Datasheet for the OPTIGA Trust M revision 1 product. This is a high level description of the hardware side of the product. You can find there the following information, though not limited to
    * Electrical Characteristics
    * Crypto Performance
    * Hardware integration guide including a reference schematics
    * External Interface which the security chip exposes to the host (not the host library)
    * Test Vectors for the security chip (on I2C level)
    * Compliance infomration
* **OPTIGA_Trust_M_Host_Library_Documentation.chm** - A Windows Help file with the API description
* **OPTIGA_Trust_M_Keys_And_Certificates_vX.XX.pdf** - A document which describes default PKI infrustructure, in which worm and what exactly do we store on chip in regards to Private Keys and Certificates. You might find the [Personalize OPTIGA™ Trust](https://github.com/Infineon/personalize-optiga-trust) Application Note usefull as well
* **OPTIGA_Trust_M_Release_Notes_vX.XX.pdf** - A history of changes for all the releases the product had
* **OPTIGA_Trust_M_Solution_Reference_Manual_vX.XX.pdf** - A document which describes in more details:
    * Features and properties of the security chip in more details
    * Use cases
    * Application areas
    * External and host side API
    * And many more other details related to the product and its application
* Some of the most frequent questions and topics like: "Why I can't call signature too often?", or "What is a Protected Update?" are duplicated from the SRM in the dedicated [Wiki articles](https://github.com/Infineon/optiga-trust-m/wiki)
