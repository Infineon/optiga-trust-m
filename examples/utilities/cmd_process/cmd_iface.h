/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
*
* \author Infineon Technologies AG
*
* \file cmd_iface.h
*
* \brief   This file implements the command interface.
*
* \ingroup examples
*
* @{
*/
#ifndef CMD_IFACE_H_
#define CMD_IFACE_H_

#include "stdint.h"
#define cmdQ_SIZE    512

extern char command[256];
extern int processCmd(void);

volatile struct
{
	uint8_t     m_getIdx;
	uint8_t     m_putIdx;
	uint8_t     m_entry[ cmdQ_SIZE ];
} cmdQ;

// Ticks are generated every 100ms
#define TICKS_PER_100_MS 10UL
// Timer set to 2 sec
#define OPTIGA_LIB_TIME_OUT	20

int optm_optiga_start_contex_init(void);
int optm_optiga_open_contex(void);
int optm_optiga_close_contex(void);

int optm_optiga_util_update_count(void);
int optm_read_data(void);
int optm_write_data(void);
int optm_protected_update(void);
int optm_get_random(void);
int optm_get_hash(void);
int optm_gen_ecc_keypair(void);
int optm_ecdsa_sign(void);
int optm_ecdsa_verify(void);
int optm_ecdh_gen_key(void);
int optm_tls_prf_gen_key(void);
int optm_rsa_gen_key(void);
int optm_rsa_sign(void);
int optm_rsa_verify(void);
int optm_rsa_decrypt_exp(void);
int optm_rsa_decrypt_sto(void);
int optm_rsa_ecrypt_session(void);

void example_optiga_crypt_hash (void);
void example_optiga_crypt_ecc_generate_keypair(void);
void example_optiga_crypt_ecdsa_sign(void);
void example_optiga_crypt_ecdsa_verify(void);
void example_optiga_crypt_ecdh(void);
void example_optiga_crypt_random(void);
void example_optiga_crypt_tls_prf_sha256(void);
void example_optiga_util_read_data(void);
void example_optiga_util_read_uid(void);
void example_optiga_util_write_data(void);
void example_optiga_crypt_rsa_generate_keypair(void);
void example_optiga_crypt_rsa_sign(void);
void example_optiga_crypt_rsa_verify(void);
void example_optiga_crypt_rsa_decrypt_and_export(void);
void example_optiga_crypt_rsa_decrypt_and_store(void);
void example_optiga_crypt_rsa_encrypt_message(void);
void example_optiga_crypt_rsa_encrypt_session(void);
void example_optiga_util_update_count(void);
void example_optiga_util_protected_update( void);

int app_parse_cmd(void);
int app_menu(void);
void Systick_Start(void);
void Systick_Stop(void);


#endif /* CMD_IFACE_H_ */
