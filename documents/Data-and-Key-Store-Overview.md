# Editing Data and Key Store Overview

This article is an excerpt of the [Solution Reference Manual](https://github.com/Infineon/optiga-trust-m/blob/master/documents) Section "OPTIGA™ Trust M Data Structures"

The figure below provides an overview of all data and key objects hosted by the OPTIGA™ and the recommended maximum
cycling (color coding) per object. In case those recommendations are meet and for higher cycled data objects the homogeneous distributed of applied programming cycles across lifetime are respected, the data integrity over lifetime could be considered being safe.

![](https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_datastore_overview_v3.jpg)

**Examples:**
* Each monotonic counter can be updated up to a maximum of 600 000 times.
* The maximum number of updates across all objects (key/data) is allowed up to 2 million times either due to external interface requests or due to internal operations (the list is given above). This means the maximum updates per object and the overall update limit across all objects must be respected to prevent reliability issues.

The data and key store has some important parameters to be considered while utilizing it. Those parameters are the data-retention-after-testing, the dataretention-after-cycling, hardening, the maximum endurance and the supported number of tearing-safe-programming-cycles.
* **data-retention-after-testing** defines how long NVM programmed during production (at wafer tester) is preserved, in case the NVM experiences no additional programming attempts, this time is the same as the device lifetime defined in the data sheet.
* **data-retention-after-cycling** defines how long NVM data, programmed throughout lifetime, are preserved. The number of experienced programming cycles is important for how long the data are preserved. For maximum 100 times the same as **data-retention-after-testing** applies. After e.g. 20 000 times the NVM data retention declines linearly down to 2 years and even more down to ½ year after about 40 000 NVM programming cycles. ½ year **data-retention-after-cycling** is the worst case. With other words, if NVM data get often cycled the time between programming attempts should not exceed half a year. If erases (sub-function of a programming cycle) are frequently done, than regular hardening is performed, in this case the **data-retention-after-cycling** worst case improves from ½ year to 3 years. In case of high cycling, beyond 20 000 times, the cycles shall be homogeneous distributed across lifetime.
* **hardening** is a process which is embedded in any erase cycle (each NVM programming cycle causes one or multiple erase cycles) and refreshes a randomly chosen NVM page. After a certain number of erase cycles, which is dependent on the physical NVM size (For OPTIGA™ Trust M about 5 000), all NVM is “refreshed”. Hardening is performed without erasing or physically moving the hardened data, i.e. hardening is neither susceptible to tearing nor does it count as a programming cycle.
* **maximum endurance** defines how often a NVM data could be programmed until it wears out.
* number of **tearing-safe-programming-cycles** defines how many tearing-safe-programming-cycles could be performed for the data and key store. It is worth to mention, that each data or key store programming attempt is performed in a tearing safe manner. The maximum number of **tearing-safe-programming-cycles** is 2 million.

The following list provides the cases when a tearing-safe-programming-cycle takes place:
* Update of a data object causes one **tearing-safe-programming-cycle**
* Update of a key object causes one to six **tearing-safe-programming-cycles** (average for ECC is one and for RSA is five)
* Usage (EXE or CHA or RD) of a data or key object which is linked to a usage counter causes one (additional) **tearing-safe-programming-cycle**
* The Security Event Counter (SEC) gets increased and subsequently decreased (refer to list of Security Events) and causes two **tearing-safe-programming-cycle**
