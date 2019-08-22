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
* \file cmd_iface.c
*
* \brief   This file implements the command interface.
*
* \ingroup examples
*
* @{
*/

/*
*\Example to run mni-shell
*
*
* This is a basic reference in how to call start mini-shell from main 

* app_menu();
* optiga_lib_print_string("$");
* while(1)
* {
*	if (terminal_io_read(&ch))
*	{
*		sprintf(str, "%c", ch);
*		pal_logger_write((const uint8_t *)str, strlen(str));
*		if ( ch == 0x0d || ch == 0x0a ) 
*       {
*		  optiga_lib_print_string_with_newline("\n");
*		  app_parse_cmd();
*		} else {
*		  CBUF_Push( cmdQ, ch );  // queue character into circular buffer
*		}
*	}
* }
*
*
*/

#include <DAVE.h>
#include <string.h>
#include <stdio.h>
#include "cmd_iface.h"
#include "cbuf.h"

#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/common/optiga_lib_logger.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal.h"
#include "optiga/pal/pal_os_timer.h"
#include "optiga/pal/pal_ifx_i2c_config.h"

optiga_util_t * me_util;

/**
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
static void optiga_util_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
}

/**
 * This function is the entry point of sample.
 *
 * \retval
 *  0 on success
 *  1 on failure
 */
void example_log_function_name(const char_t* function)
{
    optiga_lib_print_string("Entering function : ");
    optiga_lib_print_string_with_newline(function);
}
void example_log_execution_status(const char_t* function, uint8_t status)
{
    //function name
    optiga_lib_print_string(function);
    if (status == 1)
    {
        optiga_lib_print_string_with_newline(" : SUCCEED ");
    }
    else
    {
        optiga_lib_print_string_with_newline(" : FAILED ");
    }

}


int app_menu(void)
{
	optiga_lib_print_string_with_newline("\e[1;1H\e[2J");
	optiga_lib_print_string_with_newline("*******************************************************************************************");
	optiga_lib_print_string_with_newline("*                                                                                        *");
	optiga_lib_print_string_with_newline("*  NOTE: To Start -> optm_optiga_open_ctx needs to be called first                       *");
	optiga_lib_print_string_with_newline("*                                                                                        *");
	optiga_lib_print_string_with_newline("*  Close Context when done by executing -> optm_optiga_close_ctx                         *");
	optiga_lib_print_string_with_newline("*                                                                                        *");
	optiga_lib_print_string_with_newline("*  NOTE: RSA 1024 IS USED FOR EXAMPLES. OPTIGA Trust M supports both RSA 1024/2048       *");
	optiga_lib_print_string_with_newline("*                                                                                        *");
	optiga_lib_print_string_with_newline("*******************************************************************************************");
	optiga_lib_print_string_with_newline(" ");
	optiga_lib_print_string_with_newline(" ");
	optiga_lib_print_string_with_newline("optm_optiga_open_ctx          ......................OPTIGA Trust M Opens Context 0");
	optiga_lib_print_string_with_newline("optm_optiga_close_ctx         ......................OPTIGA Trust M Closes Context 0");
	optiga_lib_print_string_with_newline("optm_optiga_read_uid          ......................OPTIGA Trust M Read OPTIGA Trust M Unique Identifier (UID)");
	optiga_lib_print_string_with_newline("optm_update_count             ......................OPTIGA Trust M Test Internal Counter");
	optiga_lib_print_string_with_newline("optm_read_data                ......................OPTIGA Trust M Reads Data at Rest From OPTIGA Trust M");
	optiga_lib_print_string_with_newline("optm_write_data               ......................OPTIGA Trust M Write Data at to OPTIGA Trust M");

	//This requires a Server and connectivity to it. Request example from Infineon
	//optiga_lib_print_string_with_newline("optm_protected_update         ......................OPTIGA Trust M Performs Protected Update");

	optiga_lib_print_string_with_newline("optm_get_random               ......................OPTIGA Trust M Gets Random Number");
	optiga_lib_print_string_with_newline("optm_get_hash                 ......................OPTIGA Trust M Gets Hash");
	optiga_lib_print_string_with_newline("optx_ecc_gen_key              ......................OPTIGA Trust M ECC Generate Key");
	optiga_lib_print_string_with_newline("optm_ecdsa_sign               ......................OPTIGA Trust M ECDSA Sign");
	optiga_lib_print_string_with_newline("optm_ecdsa_verify             ......................OPTIGA Trust M ECDSA Verify");
	optiga_lib_print_string_with_newline("optm_ecdh_gen_key             ......................OPTIGA Trust M ECDH Generate Key");
	optiga_lib_print_string_with_newline("optm_tls_prf_gen_key          ......................OPTIGA Trust M Derive the keys using TLS PRF SHA256");
	optiga_lib_print_string_with_newline("optm_rsa_gen_key              ......................OPTIGA Trust M RSA 1024 Generate Key");
	optiga_lib_print_string_with_newline("optm_rsa_sign                 ......................OPTIGA Trust M RSA 1024 Sign");
	optiga_lib_print_string_with_newline("optm_rsa_verify               ......................OPTIGA Trust M RSA 1024 Verify");
	optiga_lib_print_string_with_newline("optm_rsa_crypt_decrypt_exp    ......................OPTIGA Trust M Verify the RSA decrypt and export");
	optiga_lib_print_string_with_newline("optm_rsa_decrypt_sto          ......................OPTIGA Trust M Verify the RSA decrypt and store in session OID");
	optiga_lib_print_string_with_newline("optm_rsa_ecrypt_msg           ......................OPTIGA Trust M RSA Encryption of message");
	optiga_lib_print_string_with_newline("optm_rsa_ecrypt_session       ......................OPTIGA Trust M RSA Encryption of session");
	optiga_lib_print_string_with_newline("optm_test_all                 ......................OPTIGA Trust M Test all functionality");
	optiga_lib_print_string_with_newline(" ");
	return 0;
}

uint8_t cmdbytes[128];

int parse_cmd( char *commands )
{
	int status = -1;

	do{
		if ( !strcmp( commands, "help"))
		{
			app_menu();
		}else if (!strcmp( commands, "optm_optiga_open_ctx"))
		{
			optm_optiga_open_contex();

		}else if (!strcmp( commands, "optm_optiga_close_ctx"))
		{
			optm_optiga_close_contex();

		}else if (!strcmp( commands, "optm_optiga_read_uid"))
		{
			optm_optiga_read_uid();

		}else if (!strcmp( commands, "optm_update_count"))
		{
			optm_optiga_util_update_count();

		}else if (!strcmp( commands, "optm_read_data"))
		{
			optm_read_data();

		}else if (!strcmp( commands, "optm_write_data"))
		{
			optm_write_data();

		}else if (!strcmp( commands, "optm_protected_update"))
		{
			optm_protected_update();

		}else if (!strcmp( commands, "optm_get_random"))
		{
			optm_get_random();

		}else if (!strcmp( commands, "optm_get_hash"))
		{
			optm_get_hash();

		}else if (!strcmp( commands, "optx_ecc_gen_key"))
		{
			optm_gen_ecc_keypair();

		}else if (!strcmp( commands, "optm_ecdsa_sign"))
		{
			optm_ecdsa_sign();

		}else if (!strcmp( commands, "optm_ecdsa_verify"))
		{
			optm_ecdsa_verify();

		}else if (!strcmp( commands, "optm_ecdh_gen_key"))
		{
			optm_ecdh_gen_key();

		}else if (!strcmp( commands, "optm_tls_prf_gen_key"))
		{
			optm_tls_prf_gen_key();

		}else if (!strcmp( commands, "optm_rsa_gen_key"))
		{
			optm_rsa_gen_key();

		}else if (!strcmp( commands, "optm_rsa_sign"))
		{
			optm_rsa_sign();

		}else if (!strcmp( commands, "optm_rsa_verify"))
		{
			optm_rsa_verify();

		}else if (!strcmp( commands, "optm_rsa_crypt_decrypt_exp"))
		{
			optm_rsa_crypt_decrypt_exp();

		}else if (!strcmp( commands, "optm_rsa_decrypt_sto"))
		{
			optm_rsa_decrypt_sto();

		}else if (!strcmp( commands, "optm_rsa_ecrypt_msg"))
		{
			optm_rsa_ecrypt_msg();

		}else if (!strcmp( commands, "optm_rsa_ecrypt_session"))
		{
			optm_rsa_ecrypt_session();

		}else if (!strcmp( commands, "optm_test_all"))
		{
			optm_test_all();

		}else if ( strlen(commands) )
		{
			optiga_lib_print_string_with_newline("Syntax error in command");
		}
	}while(0);

	optiga_lib_print_string_with_newline(" ");

	return 0;
}

int optm_optiga_start_contex_init(void)
{
	optiga_lib_status_t return_status;

	do
	{
		//Create an instance of optiga_util to open the application on OPTIGA.
		me_util = optiga_util_create(0, optiga_util_callback, NULL);

		/**
		 * Open the application on OPTIGA which is a precondition to perform any other operations
		 * using optiga_util_open_application
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_open_application(me_util, 0);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		while (optiga_lib_status == OPTIGA_LIB_BUSY)
		{
			//Wait until the optiga_util_open_application is completed
		}
		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			//optiga util open application failed
			break;
		}
		/**
		 * Hibernate the application on OPTIGA
		 * using optiga_util_close_application
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_close_application(me_util, 1);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (optiga_lib_status == OPTIGA_LIB_BUSY)
		{
			//Wait until the optiga_util_close_application is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			//optiga util close application failed
			break;
		}

	}while(0);

	optiga_lib_print_string_with_newline("OPTIGA Trust Context Initialization: SUCCEED");
	return return_status;
}

int optm_optiga_open_contex(void)
{
	optiga_lib_status_t return_status;

	do{
		//Create an instance of optiga_util to open the application on OPTIGA.
		me_util = optiga_util_create(0, optiga_util_callback, NULL);
		/**
		 * Restore the application on OPTIGA
		 * using optiga_util_open_application
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_open_application(me_util, 0);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_util_open_application is completed
		}
		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			//optiga util open application failed
			break;
		}
	}while(0);

	optiga_lib_print_string_with_newline("OPTIGA Trust Open Context: SUCCEED");

	return return_status;
}

int optm_optiga_close_contex(void)
{
	optiga_lib_status_t return_status;

	do{
	/**
	 * Close the application on OPTIGA after all the operations are executed
	 * using optiga_util_close_application
	 */
	optiga_lib_status = OPTIGA_LIB_BUSY;
	return_status = optiga_util_close_application(me_util, 0);

	if (OPTIGA_LIB_SUCCESS != return_status)
	{
		break;
	}

	while (optiga_lib_status == OPTIGA_LIB_BUSY)
	{
		//Wait until the optiga_util_close_application is completed
	}

	if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
	{
		//optiga util close application failed
		break;
	}

	// destroy util and crypt instances
	optiga_util_destroy(me_util);
	}while(0);

	optiga_lib_print_string_with_newline("OPTIGA Trust Close Context: SUCCEED");

	return return_status;
}

int optm_optiga_read_uid(void)
{
	example_optiga_util_read_uid();
	return 0;
}

int optm_optiga_util_update_count(void)
{
	example_optiga_util_update_count();
	return 0;
}

int optm_read_data(void)
{
	example_optiga_util_read_data();
	return 0;
}

int optm_write_data(void)
{
	example_optiga_util_write_data();
	return 0;
}

int optm_protected_update(void)
{
	example_optiga_util_protected_update();
	return 0;
}

int optm_get_random(void)
{
	example_optiga_crypt_random();
	return 0;
}

int optm_get_hash(void)
{
	example_optiga_crypt_hash();
	return 0;
}

int optm_gen_ecc_keypair(void)
{
	example_optiga_crypt_ecc_generate_keypair();
	return 0;
}


int optm_ecdsa_sign(void)
{
	example_optiga_crypt_ecdsa_sign();
	return 0;
}

int optm_ecdsa_verify(void)
{
	example_optiga_crypt_ecdsa_verify();
	return 0;
}

int optm_ecdh_gen_key(void)
{
	example_optiga_crypt_ecdh();
	return 0;
}

int optm_tls_prf_gen_key(void)
{
	example_optiga_crypt_tls_prf_sha256();
	return 0;
}

int optm_rsa_gen_key(void)
{
	example_optiga_crypt_rsa_generate_keypair();
	return 0;
}

int optm_rsa_sign(void)
{
	example_optiga_crypt_rsa_sign();
	return 0;
}

int optm_rsa_verify(void)
{
	example_optiga_crypt_rsa_verify();
	return 0;
}

int optm_rsa_crypt_decrypt_exp(void)
{
	example_optiga_crypt_rsa_decrypt_and_export();
	return 0;
}

int optm_rsa_decrypt_sto(void)
{
	example_optiga_crypt_rsa_decrypt_and_store();
	return 0;
}

int  optm_rsa_ecrypt_msg(void)
{
	example_optiga_crypt_rsa_encrypt_message();
	return 0;
}

int optm_rsa_ecrypt_session(void)
{
	example_optiga_crypt_rsa_encrypt_session();
	return 0;
}

int optm_test_all(void)
{
	//Example: Read OPTIGA Trust M Unique ID
	example_optiga_util_read_uid();
	optiga_lib_print_string_with_newline(" ");

	//Example: Update counter for counter data object
	example_optiga_util_update_count();
	optiga_lib_print_string_with_newline(" ");

	// Example: Read data / metadata from a data object
	example_optiga_util_read_data();
	optiga_lib_print_string_with_newline(" ");

	// Example: Write data / metadata from a data object
	example_optiga_util_write_data();
	optiga_lib_print_string_with_newline(" ");

	// Example Util protected update
	example_optiga_util_protected_update();
	optiga_lib_print_string_with_newline(" ");

	//Example: Generate Random
	example_optiga_crypt_random();
	optiga_lib_print_string_with_newline(" ");

	//Example: Generate Hash
	example_optiga_crypt_hash();
	optiga_lib_print_string_with_newline(" ");

	//Example: Generate ECC Key pair
	example_optiga_crypt_ecc_generate_keypair();
	optiga_lib_print_string_with_newline(" ");

	//Example: Sign the given digest
	example_optiga_crypt_ecdsa_sign();
	optiga_lib_print_string_with_newline(" ");

	//Example: Verify the signature
	example_optiga_crypt_ecdsa_verify();
	optiga_lib_print_string_with_newline(" ");
//
	//Example: Generate shared secret using ECDH
	example_optiga_crypt_ecdh();
	optiga_lib_print_string_with_newline(" ");

	//Example: Derive the keys using TLS PRF SHA256
	example_optiga_crypt_tls_prf_sha256();
	optiga_lib_print_string_with_newline(" ");

	//Example: Generate RSA Key pair
	example_optiga_crypt_rsa_generate_keypair();
	optiga_lib_print_string_with_newline(" ");

	//Example: Sign the given digest using RSA signature scheme
	example_optiga_crypt_rsa_sign();
	optiga_lib_print_string_with_newline(" ");

	//Example: Verify the rsa signature
	example_optiga_crypt_rsa_verify();
	optiga_lib_print_string_with_newline(" ");

	//Example: Verify the rsa decrypt and export
	example_optiga_crypt_rsa_decrypt_and_export();
	optiga_lib_print_string_with_newline(" ");

	//Example: Verify the rsa decrypt and store in session OID
	example_optiga_crypt_rsa_decrypt_and_store();
	optiga_lib_print_string_with_newline(" ");

	// RSA Encryption of message
	example_optiga_crypt_rsa_encrypt_message();
	optiga_lib_print_string_with_newline(" ");

	// RSA Encryption of session
	example_optiga_crypt_rsa_encrypt_session();
	optiga_lib_print_string_with_newline(" ");

	return 0;
}


int app_parse_cmd(void)
{
	static char cmd[256];

	uint16_t i = 0;
	while( !CBUF_IsEmpty(cmdQ) && i < sizeof(cmd))
	cmd[i++] = CBUF_Pop( cmdQ );
	cmd[i] = '\0';
	parse_cmd(cmd);
	optiga_lib_print_string("$");
	return 0;
}

