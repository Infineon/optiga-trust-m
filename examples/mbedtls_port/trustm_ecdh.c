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

#if defined(MBEDTLS_ECDH_C)

#include "mbedtls/ecdh.h"

#include <string.h>
#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/pal/pal_os_timer.h"
#include "optiga/common/optiga_lib_common.h"

#define PRINT_ECDH_PUBLICKEY   0

// We use here Session Context ID 0xE103 (you can choose between 0xE100 - E104)
#define OPTIGA_TRUSTM_KEYID_TO_STORE_PRIVATE_KEY  0xE103

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

	int return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
	uint8_t public_key[200];
	size_t public_key_len = sizeof(public_key);
	uint8_t public_key_offset = 3;
	optiga_ecc_curve_t curve_id;
	optiga_key_id_t optiga_key_id = OPTIGA_TRUSTM_KEYID_TO_STORE_PRIVATE_KEY;
	optiga_crypt_t * me = NULL;
    optiga_lib_status_t crypt_sync_status = OPTIGA_CRYPT_ERROR;

	//checking group against the supported curves of OPTIGA Trust M
	if ((grp->id <  MBEDTLS_ECP_DP_SECP256R1) ||
		 (grp->id > MBEDTLS_ECP_DP_BP512R1))
	{
		return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
		goto cleanup;
	}

	curve_id = OPTIGA_ECC_CURVE_NIST_P_256;
	if(grp->id == MBEDTLS_ECP_DP_SECP384R1)
	{
		curve_id = OPTIGA_ECC_CURVE_NIST_P_384;
	}
	else if(grp->id == MBEDTLS_ECP_DP_SECP521R1)
	{
		curve_id = OPTIGA_ECC_CURVE_NIST_P_521;
		public_key_offset = 4;
	}
	else if(grp->id == MBEDTLS_ECP_DP_BP256R1)
	{
		curve_id = OPTIGA_ECC_CURVE_BRAIN_POOL_P_256R1;
	}
	else if(grp->id == MBEDTLS_ECP_DP_BP384R1)
	{
		curve_id = OPTIGA_ECC_CURVE_BRAIN_POOL_P_384R1;
	}
	else
	{
		curve_id = OPTIGA_ECC_CURVE_BRAIN_POOL_P_512R1;
		public_key_offset = 4;
	}

	me = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
	if (NULL == me)
	{
		return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
		goto cleanup;
	}

	crypt_event_completed_status = OPTIGA_LIB_BUSY;

	//invoke optiga command to generate a key pair.
	crypt_sync_status = optiga_crypt_ecc_generate_keypair(me, curve_id,
			(optiga_key_usage_t) (OPTIGA_KEY_USAGE_KEY_AGREEMENT | OPTIGA_KEY_USAGE_AUTHENTICATION),
			FALSE, &optiga_key_id, public_key, (uint16_t *) &public_key_len);

	if (OPTIGA_LIB_SUCCESS != crypt_sync_status)
	{
		return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
	{
		pal_os_timer_delay_in_milliseconds(10);
	}

	if (crypt_event_completed_status != OPTIGA_LIB_SUCCESS)
	{
		return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	//store public key generated from optiga into mbedtls structure .
	if (mbedtls_ecp_point_read_binary(grp, Q, (unsigned char *) &public_key[public_key_offset], (size_t) public_key_len - public_key_offset) != 0)
	{
		return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	return_status = 0;
cleanup:
	// destroy crypt instances
	if (me != NULL)
	{
		(void)optiga_crypt_destroy(me);
	}

	return return_status;

}
#endif


#ifdef MBEDTLS_ECDH_COMPUTE_SHARED_ALT
/*
 * Compute shared secret (SEC1 3.3.1)
 */
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
		const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
		int (*f_rng)(void *, unsigned char *, size_t), void *p_rng) {

	int return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
	public_key_from_host_t pk;
	size_t pk_size;
	uint8_t pk_out[150];
	uint8_t buf[150];
	size_t public_key_len = 0;
	uint8_t publickey_offset = 3;
	optiga_crypt_t * me = NULL;
    optiga_lib_status_t crypt_sync_status = OPTIGA_CRYPT_ERROR;

#ifdef OPTIGA_TRUSTM_EXTRACT_OID_FROM_PRIVKEY
    uint8_t private_key_in[32];
    size_t private_key_in_len=sizeof(private_key_in);
#endif
    uint16_t optiga_key_id = OPTIGA_TRUSTM_KEYID_TO_STORE_PRIVATE_KEY;

	//Step1: Prepare the public key material as expected by security chip
	//checking group against the supported curves of OPTIGA Trust M
	if ((grp->id <  MBEDTLS_ECP_DP_SECP256R1) ||
		 (grp->id > MBEDTLS_ECP_DP_BP512R1))
	{
		return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
		goto cleanup;
	}
	
	pk_out[0] = 0x03;
	pk_out[publickey_offset-2] = 0x42;
		
	pk.key_type = OPTIGA_ECC_CURVE_NIST_P_256;
	if(grp->id == MBEDTLS_ECP_DP_SECP384R1)
	{
		pk.key_type = OPTIGA_ECC_CURVE_NIST_P_384;
		pk_out[publickey_offset-2] = 0x62;
	}
	else if(grp->id == MBEDTLS_ECP_DP_SECP521R1)
	{
		pk.key_type = OPTIGA_ECC_CURVE_NIST_P_521;
		publickey_offset = 4;
		pk_out[publickey_offset-3] = 0x81;
		pk_out[publickey_offset-2] = 0x86;		
	}
	else if(grp->id == MBEDTLS_ECP_DP_BP256R1)
	{
		pk.key_type = OPTIGA_ECC_CURVE_BRAIN_POOL_P_256R1;
		pk_out[publickey_offset-2] = 0x42;
	}
	else if(grp->id == MBEDTLS_ECP_DP_BP384R1)
	{
		pk.key_type = OPTIGA_ECC_CURVE_BRAIN_POOL_P_384R1;
		pk_out[publickey_offset-2] = 0x62;
	}
	else
	{
		pk.key_type = OPTIGA_ECC_CURVE_BRAIN_POOL_P_512R1;
		publickey_offset = 4;
		pk_out[publickey_offset-3] = 0x81;
		pk_out[publickey_offset-2] = 0x82;		
	}
	
	pk_out[publickey_offset-1] = 0x00;
	mbedtls_ecp_point_write_binary(grp, Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &pk_size, &pk_out[publickey_offset], 150);

	pk.public_key = pk_out;
	pk.length = pk_size + publickey_offset;

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

#ifdef OPTIGA_TRUSTM_EXTRACT_OID_FROM_PRIVKEY
	//Get the private key location and range check
	mbedtls_mpi_write_binary(d, private_key_in, private_key_in_len);
	optiga_key_id  = ((private_key_in[0]<<8) + private_key_in[1]) & 0xffff;
	if((optiga_key_id < 0xe100) || (optiga_key_id > 0xe103)) {
	   return_status = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }
#endif

	me = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
	if (NULL == me)
	{
		return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
		goto cleanup;
	}

	crypt_event_completed_status = OPTIGA_LIB_BUSY;
	//Invoke OPTIGA command to generate shared secret and store in the OID/buffer.
	crypt_sync_status = optiga_crypt_ecdh(me, (optiga_key_id_t *)&optiga_key_id , &pk, 1, buf);

	if (OPTIGA_LIB_SUCCESS != crypt_sync_status)
	{
		return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	 //Wait until the optiga_crypt_ecdh operation is completed
	while (OPTIGA_LIB_BUSY == crypt_event_completed_status)
	{
		pal_os_timer_delay_in_milliseconds(10);
	}

	if (crypt_event_completed_status != OPTIGA_LIB_SUCCESS)
	{
		return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	mbedtls_mpi_read_binary( z, buf, mbedtls_mpi_size( &grp->P ) );
	return_status = 0;

cleanup:
	// destroy crypt instances
	if (me != NULL)
	{
    	(void)optiga_crypt_destroy(me);
	}
	return return_status;

}
#endif

#endif
/**
 * @}
 */
