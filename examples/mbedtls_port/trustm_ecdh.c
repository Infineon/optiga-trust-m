/**
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
 * @{
 */

#include "mbedtls/config.h"
#include "FreeRTOS.h"
#include "task.h"

#if defined(MBEDTLS_ECDH_C)

#include "mbedtls/ecdh.h"
#include "mbedtls/pk.h"

#include <string.h>
#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/common/optiga_lib_common.h"

#define PRINT_ECDH_PUBLICKEY   0

// We use here Session Context ID 0xE104 (you can choose between 0xE100 - E104)
#define OPTIGA_TRUSTM_KEYID_TO_STORE_SHARED_SECRET  0xE103

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
optiga_lib_status_t crypt_event_completed_status;

//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_crypt_event_completed(void * context, optiga_lib_status_t return_status)
{
	crypt_event_completed_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

#ifdef MBEDTLS_ECDH_GEN_PUBLIC_ALT
/*
 * Generate public key: simple wrapper around mbedtls_ecp_gen_keypair
 */
int mbedtls_ecdh_gen_public(mbedtls_ecp_group *grp, mbedtls_mpi *d,
		mbedtls_ecp_point *Q, int (*f_rng)(void *, unsigned char *, size_t),
		void *p_rng) {

	int ret = 0;
	uint8_t public_key[200];
	size_t public_key_len = sizeof(public_key);
	optiga_ecc_curve_t curve_id;
	optiga_key_id_t optiga_key_id = OPTIGA_TRUSTM_KEYID_TO_STORE_SHARED_SECRET;
	optiga_crypt_t * me = NULL;
    optiga_lib_status_t command_queue_status = OPTIGA_CRYPT_ERROR;

	//checking group against the supported curves of OPTIGA Trust M
	if ((grp->id != MBEDTLS_ECP_DP_SECP256R1) &&
		(grp->id != MBEDTLS_ECP_DP_SECP384R1))
	{
		ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
		goto cleanup;
	}

	curve_id = grp->id == MBEDTLS_ECP_DP_SECP256R1 ? OPTIGA_ECC_CURVE_NIST_P_256: OPTIGA_ECC_CURVE_NIST_P_384;

	me = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
	if (NULL == me)
	{
		ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
		goto cleanup;
	}

	crypt_event_completed_status = OPTIGA_LIB_BUSY;

	//invoke optiga command to generate a key pair.
	command_queue_status = optiga_crypt_ecc_generate_keypair(me, curve_id,
			(optiga_key_usage_t) (OPTIGA_KEY_USAGE_KEY_AGREEMENT | OPTIGA_KEY_USAGE_AUTHENTICATION),
			FALSE, &optiga_key_id, public_key, (uint16_t *) &public_key_len);

	if ( command_queue_status != OPTIGA_LIB_SUCCESS )
	{
		ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
	{
		pal_os_timer_delay_in_milliseconds(5);
	}

	if ( crypt_event_completed_status != OPTIGA_LIB_SUCCESS )
	{
		ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	//store public key generated from optiga into mbedtls structure .
	if (mbedtls_ecp_point_read_binary(grp, Q, (unsigned char *) &public_key[3], (size_t) public_key_len - 3) != 0)
	{
		ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	ret = mbedtls_ecp_point_read_binary(grp, Q, &public_key[3], public_key_len - 3);
cleanup:

	return ret;

}
#endif


#ifdef MBEDTLS_ECDH_COMPUTE_SHARED_ALT
/*
 * Compute shared secret (SEC1 3.3.1)
 */
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
		const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
		int (*f_rng)(void *, unsigned char *, size_t), void *p_rng) {

	int ret = 0;
	public_key_from_host_t pk;
	size_t pk_size;
	uint8_t pk_out[100];
	uint8_t buf[100];
	optiga_crypt_t * me = NULL;
    optiga_lib_status_t command_queue_status = OPTIGA_CRYPT_ERROR;


	//Step1: Prepare the public key material as expected by security chip
	//checking gid against the supported curves of OPTIGA Trust M
	if( (grp->id == MBEDTLS_ECP_DP_SECP256R1) || (grp->id == MBEDTLS_ECP_DP_SECP384R1))
	{
		pk.key_type = grp->id == MBEDTLS_ECP_DP_SECP256R1 ?
				OPTIGA_ECC_CURVE_NIST_P_256 : OPTIGA_ECC_CURVE_NIST_P_384;

		mbedtls_ecp_point_write_binary(grp, Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &pk_size, &pk_out[3], 100);

		if(pk.key_type == OPTIGA_ECC_CURVE_NIST_P_256)
		{
			pk_out[0] = 0x03;
			pk_out[1] = 0x42;
			pk_out[2] = 0x00;
		}
		else
		{
			pk_out[0] = 0x03;
			pk_out[1] = 0x62;
			pk_out[2] = 0x00;
		}

		pk.public_key = pk_out;
		pk.length = pk_size + 3;

#if (PRINT_ECDH_PUBLICKEY==1)
        for(int i=0; i<pk.length;)
        {
        	printf("%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X \r\n",
        			pk.public_key[i], pk.public_key[i+1],pk.public_key[i+2],pk.public_key[i+3],
        			pk.public_key[i+4],pk.public_key[i+5],pk.public_key[i+6],pk.public_key[i+7] );
        	i+=8;
        	if(pk.length-i<8)
			{
				int x=pk.length-i;
				while(x)
				{
					printf("%.2X ",pk.public_key[i]);
					i++;
					x--;
				}
				printf("\r\n");
			}
        }
#endif

		me = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
		if (NULL == me)
		{
			ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
			goto cleanup;
		}

		crypt_event_completed_status = OPTIGA_LIB_BUSY;
		//Invoke OPTIGA command to generate shared secret and store in the OID/buffer.
		command_queue_status = optiga_crypt_ecdh(me, OPTIGA_TRUSTM_KEYID_TO_STORE_SHARED_SECRET, &pk, 1, buf);

		if ( command_queue_status != OPTIGA_LIB_SUCCESS )
	    {
			ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
	        goto cleanup;
	    }

		 //Wait until the optiga_crypt_ecdh operation is completed
	    while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
	    {
			pal_os_timer_delay_in_milliseconds(5);
	    }

		if ( crypt_event_completed_status != OPTIGA_LIB_SUCCESS )
		{
			ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		}

		mbedtls_mpi_read_binary( z, buf, mbedtls_mpi_size( &grp->P ) );
	}
	else
	{
		//error state set indicates unexpected gid to OPTIGA Trust M
		ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
	}

cleanup:

	return ret;

}
#endif

#endif
/**
 * @}
 */
