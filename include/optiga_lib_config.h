/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_lib_config.h
 *
 * \brief   This file is defines the compilation switches to build code with required features.
 *
 * \ingroup grOptigaLibCommon
 *
 * @{
 */

#ifndef _OPTIGA_LIB_CONFIG_H_
#define _OPTIGA_LIB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OPTIGA_LIB_EXTERNAL
#include OPTIGA_LIB_EXTERNAL
#else  // Default Configuration
// Default Configuration for OPTIGA Trust M V1
#ifdef OPTIGA_TRUST_M_V1
#include "optiga_lib_config_m_v1.h"
#else  // Default Configuration for OPTIGA Trust M V3
#include "optiga_lib_config_m_v3.h"
#endif
#endif  // OPTIGA_LIB_EXTERNAL

#ifdef __cplusplus
}
#endif

#endif /* _OPTIGA_LIB_CONFIG_H_*/

/**
 * @}
 */
