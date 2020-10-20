/**
* MIT License
*
* Copyright (c) 2020 Infineon Technologies AG
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
* OPTIGA(TM) Trust M - based PKCS#11 V1.0.0
*/


/**
 * @file pkcs11_trustm.c
 * @brief OPTIGA(TM) Trust M -based PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS #11 standard.
 */

/* C runtime includes. */
#include <stdio.h>
#include <string.h>
#include <limits.h>

/* OPTIGA(TM) Trust M Includes */
#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/pal/pal_os_lock.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal_i2c.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"
#include "optiga/pal/pal_ifx_i2c_config.h"

#include "pkcs11.h"
#include <semaphore.h>


#define PKCS11_PRINT( X )            //printf(X);//vLoggingPrintf X
#define PKCS11_WARNING_PRINT( X )    /* vLoggingPrintf X */

#define pkcs11NO_OPERATION            ( ( CK_MECHANISM_TYPE ) 0xFFFFFFFFF )

/* The size of the buffer malloc'ed for the exported public key in C_GenerateKeyPair */
#define pkcs11KEY_GEN_MAX_DER_SIZE    200

#define MAX_PUBLIC_KEY_SIZE           100

#define MAX_DELAY 					  50
// Value of Operational state
#define LCSO_STATE_CREATION       	(0x01)
// Value of Operational state
#define LCSO_STATE_OPERATIONAL      (0x07)

#define PKCS_ENCRYPT_ENABLE			(1 << 0)	

#define PKCS_DECRYPT_ENABLE			(1 << 1)

#define PKCS_SIGN_ENABLE			(1 << 2)

#define PKCS_VERIFY_ENABLE			(1 << 3)

//Currently set to Creation state(defualt value). At the real time/customer side this needs to be LCSO_STATE_OPERATIONAL (0x07)
#define FINAL_LCSO_STATE          (LCSO_STATE_CREATION)

typedef struct pkcs11_object_t
{
    CK_OBJECT_HANDLE object_handle;
    // If Label is associated with a private key slot.
    // This flag is used to mark it active/non-active, as we can't remove the private key
    CK_BYTE status_lable[ pkcs11configMAX_LABEL_LENGTH + 1 ]; /* Plus 1 for the null terminator. */
} pkcs11_object_t;


typedef struct pkcs11_object_list
{
    sem_t semaphore; /* Semaphore that protects write operations to the objects array. */
    struct timespec timeout;
    optiga_crypt_t* optiga_crypt_instance;
    optiga_util_t* optiga_util_instance;
    optiga_lib_status_t optiga_lib_status;
    pkcs11_object_t objects[ pkcs11configMAX_NUM_OBJECTS ];
} pkcs11_object_list;

/* PKCS #11 Object */
typedef struct pkcs11_context_struct
{
    CK_BBOOL is_initialized;
    pkcs11_object_list object_list;
    uint16_t certificate_oid;
    uint16_t private_key_oid;
} pkcs11_context_struct;

static pkcs11_context_struct pkcs11_context;

typedef struct optiga_sha256_ctx_t
{
    uint8_t hash_ctx_buff [209];
    optiga_hash_context_t hash_ctx;
}optiga_sha256_ctx_t;

/**
 * @brief Session structure.
 */
typedef struct pkcs11_session
{
    CK_ULONG state;
    CK_BBOOL opened;
    CK_MECHANISM_TYPE operation_in_progress;
    CK_BBOOL find_object_init;
    CK_BBOOL find_object_complete;
    CK_BYTE * find_object_lable; /* Pointer to the label for the search in progress. Should be NULL if no search in progress. */
    uint8_t find_object_lable_length;
    CK_MECHANISM_TYPE verify_mechanism;
    uint16_t verify_key_oid;
    CK_MECHANISM_TYPE sign_mechanism;      /* Mechanism of the sign operation in progress. Set during C_SignInit. */
    uint16_t sign_key_oid;
    CK_BBOOL sign_init_done;
    CK_BBOOL verify_init_done;
    CK_BBOOL encrypt_init_done;
    CK_BBOOL decrypt_init_done;
    optiga_sha256_ctx_t sha256_ctx;
	CK_ULONG rsa_key_size;
	CK_ULONG ec_key_size;
	optiga_ecc_curve_t ec_key_type;
	uint16_t encryption_key_oid;
	uint16_t decryption_key_oid;
	uint8_t key_template_enabled;
} pkcs11_session_t, * p_pkcs11_session_t;

pal_os_lock_t optiga_mutex;

/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID               1


/**
 * @brief Helper definitions.
 */
#define pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT             3

#define pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX     0
#define pkcs11GENERATE_KEY_PAIR_ECPARAMS_ATTRIBUTE_INDEX    1
#define PKCS11_MODULE_IS_INITIALIZED                        ( ( pkcs11_context.is_initialized == CK_TRUE ) ? 1 : 0 )
#define PKCS11_SESSION_IS_OPEN( xSessionHandle )                         ( ( ( ( p_pkcs11_session_t ) xSessionHandle )->opened ) == CK_TRUE ? CKR_OK : CKR_SESSION_CLOSED )
#define PKCS11_SESSION_IS_VALID( xSessionHandle )                        ( ( ( p_pkcs11_session_t ) xSessionHandle != NULL ) ? PKCS11_SESSION_IS_OPEN( xSessionHandle ) : CKR_SESSION_HANDLE_INVALID )
#define PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSessionHandle )    ( PKCS11_MODULE_IS_INITIALIZED ? PKCS11_SESSION_IS_VALID( xSessionHandle ) : CKR_CRYPTOKI_NOT_INITIALIZED )
/*-----------------------------------------------------------*/

#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE    1728

enum eObjectHandles
{
	eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
	DevicePrivateKey,
	TestPrivateKey,
	DevicePublicKey,
	TestPublicKey,
	DeviceCertificate,
	TestCertificate,
	CodeSigningKey
};

//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_callback(void * pvContext, optiga_lib_status_t xReturnStatus)
{
	optiga_lib_status_t * xInstanceStatus = (optiga_lib_status_t *)pvContext;

    if (NULL != xInstanceStatus)
    {
    	*xInstanceStatus = xReturnStatus;
    }
}


/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pxLabel         Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */
CK_OBJECT_HANDLE find_object( uint8_t * pLabel,
                                        uint8_t usLength )
{
    CK_OBJECT_HANDLE object_handle = eInvalidHandle;

    /* Translate from the PKCS#11 label to local storage file name. */
    if( 0 == memcmp( pLabel,
                     &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                     sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
    {
        object_handle = DeviceCertificate;
    }
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
    {
        /* This operation isn't supported for the OPTIGA(TM) Trust M due to a security considerations
         * You can only generate a keypair and export a private component if you like */
        /* We do assign a handle though, as the AWS can#t handle the lables without having a handle*/
        object_handle = DevicePrivateKey;
    }

    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
    {
        object_handle = DevicePublicKey;
    }
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                          sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
    {
        object_handle = CodeSigningKey;
    }

    return object_handle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Gets the value of an object in storage, by handle.
 *
 * Port-specific file access for cryptographic information.
 *
 * This call dynamically allocates the buffer which object value
 * data is copied into.  get_object_value_cleanup()
 * should be called after each use to free the dynamically allocated
 * buffer.
 *
 * @sa get_object_value_cleanup
 *
 * @param[in] pcFileName    The name of the file to be read.s
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 * @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
 *                          or exportable (CK_FALSE)
 *
 * @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
 * no such object handle was found, CKR_DEVICE_MEMORY if memory for
 * buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
 * error.
 */
long get_object_value( CK_OBJECT_HANDLE object_handle,
                                 uint8_t ** ppucData,
                                 uint32_t * pulDataSize,
                                 CK_BBOOL * pIsPrivate )
{
    long                 ulReturn = CKR_OK;
    optiga_lib_status_t  xReturn;
    long                 lOptigaOid = 0;
    char*                xEnd = NULL;
    uint8_t              xOffset = 0;

    *pIsPrivate = CK_FALSE;

    // We need to allocate a buffer for a certificate/certificate chain
    // This data is later should be freed with get_object_value_cleanup
    *ppucData = malloc( pkcs11OBJECT_CERTIFICATE_MAX_SIZE );
    if (NULL != *ppucData)
    {
        *pulDataSize = pkcs11OBJECT_CERTIFICATE_MAX_SIZE;
		*pIsPrivate = CK_FALSE;
	
		switch (object_handle) 
		{
			case DeviceCertificate:
				lOptigaOid = strtol(pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, &xEnd, 16);
				xOffset = 9;
				break;
			case DevicePublicKey:
				lOptigaOid = strtol(pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, &xEnd, 16);
				break;

			case CodeSigningKey:
				lOptigaOid = strtol(pkcs11configLABEL_CODE_VERIFICATION_KEY, &xEnd, 16);
				break;
			case DevicePrivateKey:
				/*
				 * This operation isn't supported for the OPTIGA(TM) Trust M due to a security considerations
				 * You can only generate a keypair and export a private component if you like
				 */
			default:
				ulReturn = CKR_KEY_HANDLE_INVALID;
				break;
		}

        if ( (0 != lOptigaOid) && (USHRT_MAX > lOptigaOid) && (NULL != pulDataSize))
        {
        	pal_os_lock_acquire(&optiga_mutex);

        	pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
			xReturn = optiga_util_read_data(pkcs11_context.object_list.optiga_util_instance, lOptigaOid, xOffset, *ppucData, (uint16_t*)pulDataSize);

			if (OPTIGA_LIB_SUCCESS == xReturn)
			{
				while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
				{
				
				}

				// In case the read was ok, but no data inside
				if (0x8008 == pkcs11_context.object_list.optiga_lib_status)
				{
					*ppucData = NULL;
					*pulDataSize = 0;
				}
				else if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
				{
					*ppucData = NULL;
					*pulDataSize = 0;
					ulReturn = CKR_KEY_HANDLE_INVALID;
				}
			}

			pal_os_lock_release(&optiga_mutex);
        }
    }


    return ulReturn;
}


/*-----------------------------------------------------------*/

/**
 * @brief Cleanup after get_object_value().
 *
 * @param[in] pucData       The buffer to free.
 *                          (*ppucData from get_object_value())
 * @param[in] ulDataSize    The length of the buffer to free.
 *                          (*pulDataSize from get_object_value())
 */
void get_object_value_cleanup( uint8_t * pucData,
                                       uint32_t ulDataSize )
{
    /* Unused parameters. */
    free( pucData );

    /* Since no buffer was allocated on heap, there is no cleanup
     * to be done. */
}


/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
p_pkcs11_session_t get_session_pointer( CK_SESSION_HANDLE xSession )
{
    return ( p_pkcs11_session_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}


/*
 * PKCS#11 module implementation.
 */

/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
static CK_FUNCTION_LIST prvP11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL, /*C_GetInfo */
    C_GetFunctionList,
    C_GetSlotList,
    NULL, /*C_GetSlotInfo*/
    C_GetTokenInfo,
    NULL, /*C_GetMechanismList*/
    C_GetMechanismInfo,
    C_InitToken,
    NULL, /*C_InitPIN*/
    NULL, /*C_SetPIN*/
    C_OpenSession,
    C_CloseSession,
    NULL,    /*C_CloseAllSessions*/
    NULL,    /*C_GetSessionInfo*/
    NULL,    /*C_GetOperationState*/
    NULL,    /*C_SetOperationState*/
    C_Login, /*C_Login*/
    NULL,    /*C_Logout*/
    C_CreateObject,
    NULL,    /*C_CopyObject*/
    C_DestroyObject,
    NULL,    /*C_GetObjectSize*/
    C_GetAttributeValue,
    NULL,    /*C_SetAttributeValue*/
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    C_EncryptInit,
    C_Encrypt,
    C_EncryptUpdate,
    C_EncryptFinal,
    C_DecryptInit,
    C_Decrypt,
    C_DecryptUpdate,
    C_DecryptFinal,
    C_DigestInit,
    NULL, /*C_Digest*/
    C_DigestUpdate,
    NULL, /* C_DigestKey*/
    C_DigestFinal,
    C_SignInit,
    C_Sign,
    C_SignUpdate,
    C_SignFinal,
    NULL, /*C_SignRecoverInit*/
    NULL, /*C_SignRecover*/
    C_VerifyInit,
    C_Verify,
    C_VerifyUpdate,
    C_VerifyFinal,
    NULL, /*C_VerifyRecoverInit*/
    NULL, /*C_VerifyRecover*/
    NULL, /*C_DigestEncryptUpdate*/
    NULL, /*C_DecryptDigestUpdate*/
    NULL, /*C_SignEncryptUpdate*/
    NULL, /*C_DecryptVerifyUpdate*/
    NULL, /*C_GenerateKey*/
    C_GenerateKeyPair,
    NULL, /*C_WrapKey*/
    NULL, /*C_UnwrapKey*/
    NULL, /*C_DeriveKey*/
    NULL, /*C_SeedRandom*/
    C_GenerateRandom,
    NULL, /*C_GetFunctionStatus*/
    NULL, /*C_CancelFunction*/
    NULL  /*C_WaitForSlotEvent*/
};

CK_RV pair_host_and_optiga_using_pre_shared_secret(void)
{
    uint16_t bytes_to_read;
    uint8_t platform_binding_secret[64];
    uint8_t platform_binding_secret_metadata[44];
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    pal_status_t pal_return_status;

	
	/* Platform Binding Shared Secret (0xE140) Metadata to be updated */
	
	const uint8_t platform_binding_shared_secret_metadata_final [] = {
		//Metadata to be updated
		0x20, 0x17,
			// LcsO
			0xC0, 0x01,
						FINAL_LCSO_STATE,		// Refer Macro to see the value or some more notes
			// Change/Write Access tag
			0xD0, 0x07,
						// This allows updating the binding secret during the runtime using shielded connection
						// If not required to update the secret over the runtime, set this to NEV and
						// update Metadata length accordingly
						0xE1, 0xFC, LCSO_STATE_OPERATIONAL,   // LcsO < Operational state
						0xFE,
						0x20, 0xE1, 0x40,
			// Read Access tag
			0xD1, 0x03,
						0xE1, 0xFC, LCSO_STATE_OPERATIONAL,   // LcsO < Operational state
			// Execute Access tag
			0xD3, 0x01,
						0x00,	// Always
			// Data object Type
			0xE8, 0x01,
						0x22,	// Platform binding secret type
	};

    do
    {


        /**
         * 1. Initialize the protection level and protocol version for the instances
         */
        OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(pkcs11_context.object_list.optiga_util_instance,OPTIGA_COMMS_NO_PROTECTION);
        OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(pkcs11_context.object_list.optiga_util_instance,OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);

        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(pkcs11_context.object_list.optiga_util_instance,OPTIGA_COMMS_NO_PROTECTION);
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(pkcs11_context.object_list.optiga_util_instance,OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);

        /**
         * 2. Read Platform Binding Shared secret (0xE140) data object metadata from OPTIGA
         *    using optiga_util_read_metadata.
         */
        bytes_to_read = sizeof(platform_binding_secret_metadata);
        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_read_metadata(pkcs11_context.object_list.optiga_util_instance,
                                                  0xE140,
                                                  platform_binding_secret_metadata,
                                                  &bytes_to_read);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}
		while (pkcs11_context.object_list.optiga_lib_status == OPTIGA_LIB_BUSY)
		{
			
		}
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}


        /**
         * 3. Validate LcsO in the metadata.
         *    Skip the rest of the procedure if LcsO is greater than or equal to operational state(0x07)
         */
        if (platform_binding_secret_metadata[4] >= LCSO_STATE_OPERATIONAL)
        {
            // The LcsO is already greater than or equal to operational state
            break;
        }

        /**
         * 4. Generate Random using optiga_crypt_random
         *       - Specify the Random type as TRNG
         *    a. The maximum supported size of secret is 64 bytes.
         *       The minimum recommended is 32 bytes.
         *    b. If the host platform doesn't support random generation,
         *       use OPTIGA to generate the maximum size chosen.
         *       else choose the appropriate length of random to be generated by OPTIGA
         *
         */
        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_random(pkcs11_context.object_list.optiga_crypt_instance,
                                            OPTIGA_RNG_TYPE_TRNG,
                                            platform_binding_secret,
                                            sizeof(platform_binding_secret));
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}
		while (pkcs11_context.object_list.optiga_lib_status == OPTIGA_LIB_BUSY)
		{
			
		}
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}


        /**
         * 5. Generate random on Host
         *    If the host platform doesn't support, skip this step
         */


        /**
         * 6. Write random(secret) to OPTIGA platform Binding shared secret data object (0xE140)
         */
        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(pkcs11_context.object_list.optiga_util_instance,OPTIGA_COMMS_NO_PROTECTION);
        return_status = optiga_util_write_data(pkcs11_context.object_list.optiga_util_instance,
                                               0xE140,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               0,
                                               platform_binding_secret,
                                               sizeof(platform_binding_secret));
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}
		while (pkcs11_context.object_list.optiga_lib_status == OPTIGA_LIB_BUSY)
		{
			
		}
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}


        /**
         * 7. Write/store the random(secret) on the Host platform
         *
         */
        pal_return_status = pal_os_datastore_write(OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID,
                                                   platform_binding_secret,
                                                   sizeof(platform_binding_secret));

        if (PAL_STATUS_SUCCESS != pal_return_status)
        {
            break;
        }


        /**
         * 8. Update metadata of OPTIGA Platform Binding shared secret data object (0xE140)
         */
        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(pkcs11_context.object_list.optiga_util_instance,OPTIGA_COMMS_NO_PROTECTION);
        return_status = optiga_util_write_metadata(pkcs11_context.object_list.optiga_util_instance,
                                                   0xE140,
                                                   platform_binding_shared_secret_metadata_final,
                                                   sizeof(platform_binding_shared_secret_metadata_final));

    	if (OPTIGA_LIB_SUCCESS != return_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}
		while (pkcs11_context.object_list.optiga_lib_status == OPTIGA_LIB_BUSY)
		{
			
		};
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			return_status = CKR_FUNCTION_FAILED;
			break;
		}

        return_status = OPTIGA_LIB_SUCCESS;

    } while(FALSE);

	return (CK_RV)return_status;
}

CK_RV optiga_trustm_initialize( void )
{
    CK_RV xResult = CKR_OK;
    uint16_t dOptigaOID;
	static uint8_t host_pair_done = 1;
	do
	{
	    if( pkcs11_context.is_initialized == CK_TRUE )
	    {
	        xResult = CKR_CRYPTOKI_ALREADY_INITIALIZED;
	    }
	    if( xResult == CKR_OK )
	    {
	        memset( &pkcs11_context, 0, sizeof( pkcs11_context ) );
	    	//pal_i2c_init(NULL);
	    	xResult = sem_init(&pkcs11_context.object_list.semaphore , 0, 1);
			if( xResult != CKR_OK )
			{
				break;
			}
			pal_gpio_init(&optiga_reset_0);
			pal_gpio_init(&optiga_vdd_0);
			pkcs11_context.object_list.timeout.tv_sec = 0;
			pkcs11_context.object_list.timeout.tv_nsec = 0xffff;
	        //pkcs11_context.object_list.semaphore = xSemaphoreCreateMutex();
	        pkcs11_context.object_list.optiga_crypt_instance =
	        		optiga_crypt_create(0, optiga_callback, &pkcs11_context.object_list.optiga_lib_status);

	        pkcs11_context.object_list.optiga_util_instance =
	        		optiga_util_create(0, optiga_callback, &pkcs11_context.object_list.optiga_lib_status);

			pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
			xResult = optiga_util_open_application(pkcs11_context.object_list.optiga_util_instance, 0);

			if (OPTIGA_LIB_SUCCESS != xResult)
			{
				xResult = CKR_FUNCTION_FAILED;
				break;
			}
			while (pkcs11_context.object_list.optiga_lib_status == OPTIGA_LIB_BUSY)
			{
				
			}

			if ((NULL == pkcs11_context.object_list.optiga_crypt_instance) ||
				(NULL == pkcs11_context.object_list.optiga_util_instance) ||
				(OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status) ||
				(CKR_FUNCTION_FAILED == xResult))
			{
				xResult = CKR_FUNCTION_FAILED;
				break;
			}
			else
			{
				// Current limitation
				dOptigaOID = 0xE0C4;
				// Maximum Power, Minimum Current limitation
				uint8_t cCurrentLimit = 15;
				pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
				xResult = optiga_util_write_data(pkcs11_context.object_list.optiga_util_instance,
												dOptigaOID, OPTIGA_UTIL_WRITE_ONLY,
												0, //offset
												&cCurrentLimit,
												1);

		        if (OPTIGA_LIB_SUCCESS != xResult)
		        {
		        	xResult = CKR_FUNCTION_FAILED;
					break;
		        }

		        while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		        {

		        }

		        // Either by timout or because of success it should end up here
		        if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		        {
		        	xResult = CKR_FUNCTION_FAILED;
					break;
		        }
		        #ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
				if(host_pair_done)
				{
					xResult = pair_host_and_optiga_using_pre_shared_secret();
					if (OPTIGA_LIB_SUCCESS != xResult)
			        {
			        	xResult = CKR_FUNCTION_FAILED;
						break;
			        }
					host_pair_done = 0;
				}
				#endif
			}

	    }
	} while(0);

    return xResult;
}

/*-----------------------------------------------------------*/

CK_RV optiga_trustm_deinitialize( void )
{
    CK_RV xResult = CKR_OK;

    if( pkcs11_context.is_initialized == CK_TRUE )
    {
		do
		{
			if( xResult == CKR_OK )
			{
				pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
				xResult = optiga_util_close_application(pkcs11_context.object_list.optiga_util_instance, 0);

				if (OPTIGA_LIB_SUCCESS != xResult)
				{
					xResult = CKR_FUNCTION_FAILED;
					break;
				}
				while (pkcs11_context.object_list.optiga_lib_status == OPTIGA_LIB_BUSY)
				{
					
				}
				if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		        {
		        	xResult = CKR_FUNCTION_FAILED;
					break;
		        }
				//Destroy the instances after the completion of usecase			
				xResult = optiga_crypt_destroy(pkcs11_context.object_list.optiga_crypt_instance);
				xResult |= optiga_util_destroy(pkcs11_context.object_list.optiga_util_instance);

				if(OPTIGA_LIB_SUCCESS != xResult)
				{
					xResult = CKR_FUNCTION_FAILED;
				}
			}
		}while(0);
    }

    return xResult;
}

/**
 * @brief Searches the PKCS #11 module's object list for label and provides handle.
 *
 * @param[in] pcLabel            Array containing label.
 * @param[in] xLableLength       Length of the label, in bytes.
 * @param[out] pxPalHandle       Pointer to the PAL handle to be provided.
 *                               CK_INVALID_HANDLE if no object found.
 * @param[out] pxAppHandle       Pointer to the application handle to be provided.
 *                               CK_INVALID_HANDLE if no object found.
 */
void find_object_in_list_by_label( uint8_t * pcLabel,
                                 size_t xLabelLength,
                                 CK_OBJECT_HANDLE_PTR pxPalHandle,
                                 CK_OBJECT_HANDLE_PTR pxAppHandle )
{
    uint8_t ucIndex;

    *pxPalHandle = CK_INVALID_HANDLE;
    *pxAppHandle = CK_INVALID_HANDLE;

    for( ucIndex = 0; ucIndex < pkcs11configMAX_NUM_OBJECTS; ucIndex++ )
    {
        if( 0 == memcmp( pcLabel, pkcs11_context.object_list.objects[ ucIndex ].status_lable, xLabelLength ) )
        {
            *pxPalHandle = pkcs11_context.object_list.objects[ ucIndex ].object_handle;
            *pxAppHandle = ucIndex + 1; /* Zero is not a valid handle, so let's offset by 1. */
            break;
        }
    }
}


/**
 * @brief Searches the PKCS #11 module's object list for handle and provides label info.
 *
 * @param[in] xAppHandle         The handle of the object being searched for, used by the application.
 * @param[out] xPalHandle        The handle of the object being used by the PAL.
 * @param[out] ppcLabel          Pointer to an array containing label.  NULL if object not found.
 * @param[out] pxLabelLength     Pointer to label length (includes a string null terminator).
 *                               0 if no object found.
 */
void find_object_in_list_by_handle( CK_OBJECT_HANDLE xAppHandle,
                                       CK_OBJECT_HANDLE_PTR pxPalHandle,
                                       uint8_t ** ppcLabel,
                                       size_t * pxLabelLength )
{
    int lIndex = xAppHandle - 1;

    *ppcLabel = NULL;
    *pxLabelLength = 0;

    if (pkcs11configMAX_NUM_OBJECTS >= lIndex)
    {
        if( pkcs11_context.object_list.objects[ lIndex ].object_handle != CK_INVALID_HANDLE )
        {
            *ppcLabel = pkcs11_context.object_list.objects[ lIndex ].status_lable;
            *pxLabelLength = strlen( ( const char * ) pkcs11_context.object_list.objects[ lIndex ].status_lable ) + 1;
            *pxPalHandle = pkcs11_context.object_list.objects[ lIndex ].object_handle;
        }
    }
    else{
        *ppcLabel = NULL;
        *pxLabelLength = 0;
        pxPalHandle = NULL;
    }
}

CK_RV delete_object_from_list( CK_OBJECT_HANDLE xAppHandle )
{
    CK_RV xResult = CKR_OK;
    int32_t get_semaphore;
    int lIndex = xAppHandle - 1;
	
    get_semaphore = sem_timedwait( &pkcs11_context.object_list.semaphore, &pkcs11_context.object_list.timeout );

    if( get_semaphore == 0 )
    {
        if( pkcs11_context.object_list.objects[ lIndex ].object_handle != CK_INVALID_HANDLE )
        {
            memset( &pkcs11_context.object_list.objects[ lIndex ], 0, sizeof( pkcs11_object_t ) );
        }
        else
        {
            xResult = CKR_OBJECT_HANDLE_INVALID;
        }

        sem_post( &pkcs11_context.object_list.semaphore );
    }
    else
    {
        xResult = CKR_CANT_LOCK;
    }

    return xResult;
}

CK_RV add_object_to_list( CK_OBJECT_HANDLE xPalHandle,
                          CK_OBJECT_HANDLE_PTR pxAppHandle,
                          uint8_t * pcLabel,
                          size_t xLabelLength )
{
    CK_RV xResult = CKR_OK;
   int32_t get_semaphore;

    CK_BBOOL xObjectFound = CK_FALSE;
    int lInsertIndex = -1;
    int lSearchIndex = pkcs11configMAX_NUM_OBJECTS - 1;

    get_semaphore = sem_timedwait( &pkcs11_context.object_list.semaphore, &pkcs11_context.object_list.timeout);

    if( get_semaphore == 0 )
    {
        for( lSearchIndex = pkcs11configMAX_NUM_OBJECTS - 1; lSearchIndex >= 0; lSearchIndex-- )
        {
            if( pkcs11_context.object_list.objects[ lSearchIndex ].object_handle == xPalHandle )
            {
                /* Object already exists in list. */
                xObjectFound = CK_TRUE;
                break;
            }
            else if( pkcs11_context.object_list.objects[ lSearchIndex ].object_handle == CK_INVALID_HANDLE )
            {

				lInsertIndex = lSearchIndex;
            }
        }

        if( xObjectFound == CK_FALSE )
        {
            if( lInsertIndex != -1 )
            {
                if( xLabelLength < pkcs11configMAX_LABEL_LENGTH )
                {
                    pkcs11_context.object_list.objects[ lInsertIndex ].object_handle = xPalHandle;
                    memcpy( pkcs11_context.object_list.objects[ lInsertIndex ].status_lable, pcLabel, xLabelLength );
                    *pxAppHandle = lInsertIndex + 1;
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }
            }
        }

        sem_post( &pkcs11_context.object_list.semaphore );
    }
    else
    {
        xResult = CKR_CANT_LOCK;
    }

    return xResult;
}


static uint8_t append_optiga_certificate_tags (uint16_t xCertWithoutTagsLength, 
										uint8_t* pxCertTags, uint16_t xCertTagsLength)
{
	char t1[3], t2[3], t3[3];

	int xCalc = xCertWithoutTagsLength,
		xCalc1 = 0,
		xCalc2 = 0;

	uint8_t ret = 0;
	do
	{
		if ((pxCertTags == NULL) || (xCertWithoutTagsLength == 0) ||
			(xCertTagsLength != 9))
		{
			break;
		}

		if (xCalc > 0xFF)
		{
			xCalc1 = xCalc >> 8;
			xCalc = xCalc%0x100;
			if (xCalc1 > 0xFF)
			{
				xCalc2 = xCalc1 >> 8;
				xCalc1 = xCalc1%0x100;
			}
		}
		t3[0] = xCalc2;
		t3[1] = xCalc1;
		t3[2] = xCalc;
		xCalc = xCertWithoutTagsLength + 3;
		if (xCalc > 0xFF)
		{
			xCalc1 = xCalc >> 8;
			xCalc = xCalc%0x100;
			if (xCalc1 > 0xFF)
			{
				xCalc2 = xCalc1 >> 8;
				xCalc1 = xCalc1%0x100;
			}
		}
		t2[0] = xCalc2;
		t2[1] = xCalc1;
		t2[2] = xCalc;
		xCalc = xCertWithoutTagsLength + 6;
		if (xCalc > 0xFF)
		{
			xCalc1 = xCalc >> 8;
			xCalc = xCalc%0x100;
			if (xCalc1 > 0xFF)
			{
				xCalc2 = xCalc1 >> 8;
				xCalc1 = xCalc1%0x100;
			}
		}
		t1[0] = 0xC0;
		t1[1] = xCalc1;
		t1[2] = xCalc;

		for (int i = 0; i < 3; i++) {
			pxCertTags[i] = t1[i];
		}
		for (int i = 0; i < 3; i++)
		{
			pxCertTags[i+3] = t2[i];
		}
		for (int i = 0; i < 3; i++) {
			pxCertTags[i+6] = t3[i];
		}

		ret = 1;

	}while(0);

	return ret;
}

static int32_t upload_certificate(char * pucLabel, uint8_t * pucData, uint32_t ulDataSize)
{
	long	 lOptigaOid = 0;
	const	 uint8_t xTagsLength = 9;
	uint8_t  pxCertTags[xTagsLength];
	optiga_lib_status_t xReturn = OPTIGA_UTIL_ERROR;
	char*	 xEnd = NULL;

	/**
	 * Write a certificate to a given cert object (e.g. E0E8)
	 * using optiga_util_write_data.
	 *
	 * We do create here another instance, as the certificate slot is shared bz all isntances
	 *
	 * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
	 * to clear the remaining data in the object
	 */

	lOptigaOid = strtol(pucLabel, &xEnd, 16);

	if ( (0 != lOptigaOid) && (USHRT_MAX > lOptigaOid) && (USHRT_MAX > ulDataSize))
	{
		// Certificates on OPTIGA Trust SE are stored with certitficate identifiers -> tags,
		// which are 9 bytes long
		if (append_optiga_certificate_tags(ulDataSize, pxCertTags, xTagsLength))
		{
			pal_os_lock_acquire(&optiga_mutex);

			pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
			xReturn = optiga_util_write_data(pkcs11_context.object_list.optiga_util_instance,
											 (uint16_t)lOptigaOid,
											 OPTIGA_UTIL_ERASE_AND_WRITE,
											 0,
											 pxCertTags,
											 9);

			if (OPTIGA_LIB_SUCCESS == xReturn)
			{
				while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
				{
					
				}

				if (OPTIGA_LIB_SUCCESS == pkcs11_context.object_list.optiga_lib_status)
				{
					pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
					xReturn = optiga_util_write_data(pkcs11_context.object_list.optiga_util_instance,
													 (uint16_t)lOptigaOid,
													 OPTIGA_UTIL_WRITE_ONLY,
													 xTagsLength,
													 pucData,
													 ulDataSize);

					if (OPTIGA_LIB_SUCCESS == xReturn)
					{
						while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
						{
							
						}

						if (OPTIGA_LIB_SUCCESS == pkcs11_context.object_list.optiga_lib_status)
						{
							xReturn = OPTIGA_LIB_SUCCESS;
						}
					}
				}
			}

			pal_os_lock_release(&optiga_mutex);
		}
	}

	return xReturn;
}
static int32_t upload_public_key(char * pucLabel, uint8_t * pucData, uint32_t ulDataSize)
{
	long	 lOptigaOid = 0;
	optiga_lib_status_t xReturn = OPTIGA_UTIL_ERROR;;
	char*	 xEnd = NULL;

	/**
	 * Write a public key to an arbitrary data object
	 * Note: You might need to lock the data object here. see optiga_util_write_metadata()
	 *
	 * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
	 * to clear the remaining data in the object
	 */
	lOptigaOid = strtol(pucLabel, &xEnd, 16);

	if ( (0 != lOptigaOid) && (USHRT_MAX >= lOptigaOid) && (USHRT_MAX >= ulDataSize))
	{
		pal_os_lock_acquire(&optiga_mutex);

		pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
		xReturn = optiga_util_write_data(pkcs11_context.object_list.optiga_util_instance,
										 (uint16_t)lOptigaOid,
										 OPTIGA_UTIL_ERASE_AND_WRITE,
										 0,
										 pucData,
										 ulDataSize);

		if (OPTIGA_LIB_SUCCESS == xReturn)
		{
			while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
			{
				
			}

			if (OPTIGA_LIB_SUCCESS == pkcs11_context.object_list.optiga_lib_status)
			{
				xReturn = OPTIGA_LIB_SUCCESS;
			}
		}

		pal_os_lock_release(&optiga_mutex);

	}

	return xReturn;
}
	

/**
 * @brief Saves an object in non-volatile storage.
 *
 * Port-specific file write for cryptographic information.
 *
 * @param[in] pxLabel		The label of the object to be stored.
 * @param[in] pucData		The object data to be saved
 * @param[in] pulDataSize	Size (in bytes) of object data.
 *
 * @return The object handle if successful.
 * eInvalidHandle = 0 if unsuccessful.
 */
CK_OBJECT_HANDLE save_object( CK_ATTRIBUTE_PTR pxLabel,
										uint8_t * pucData,
										uint32_t ulDataSize )
{
	CK_OBJECT_HANDLE object_handle = eInvalidHandle;

	long	 lOptigaOid = 0;
	uint8_t  bOffset = 0;
	char*	 xEnd = NULL;

	optiga_lib_status_t xReturn = OPTIGA_UTIL_ERROR;;


	if( ulDataSize <= pkcs11OBJECT_CERTIFICATE_MAX_SIZE )
	{
		/* Translate from the PKCS#11 label to local storage file name. */
		if( 0 == memcmp( pxLabel->pValue,
						 &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
						 sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
		{
			if ( upload_certificate(pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, pucData, ulDataSize) ==  OPTIGA_LIB_SUCCESS)
			{
				object_handle = DeviceCertificate;
			}
		}
		else if( (0 == memcmp( pxLabel->pValue,
							   &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
							   sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) )) ||
				 (0 == memcmp( pxLabel->pValue,
							   &pkcs11configLABEL_DEVICE_RSA_PRIVATE_KEY_FOR_TLS,
							   sizeof( pkcs11configLABEL_DEVICE_RSA_PRIVATE_KEY_FOR_TLS ) )))
		{
			/* This operation isn't supported for the OPTIGA(TM) Trust M due to a security considerations
			 * You can only generate a keypair and export a private component if you like */
			/* We do assign a handle though, as the AWS can#t handle the lables without having a handle*/
			object_handle = DevicePrivateKey;
		}
		else if( 0 == memcmp( pxLabel->pValue,
							  &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
							  sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
		{
			if (upload_public_key(pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, pucData, ulDataSize) ==  OPTIGA_LIB_SUCCESS)
			{
					object_handle = DevicePublicKey;
			}
		}
		else if( 0 == memcmp( pxLabel->pValue,
							  &pkcs11configLABEL_DEVICE_RSA_PUBLIC_KEY_FOR_TLS,
							  sizeof( pkcs11configLABEL_DEVICE_RSA_PUBLIC_KEY_FOR_TLS ) ) )
		{
			if (upload_public_key(pkcs11configLABEL_DEVICE_RSA_PUBLIC_KEY_FOR_TLS, pucData, ulDataSize) ==  OPTIGA_LIB_SUCCESS)
			{
					object_handle = DevicePublicKey;
			}
		}

		else if( 0 == memcmp( pxLabel->pValue,
							  &pkcs11configLABEL_CODE_VERIFICATION_KEY,
							  sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
		{
			/**
			 * Write a Code Verification Key/Certificate to an Trust Anchor data object
			 * Note: You might need to lock the data object here. see optiga_util_write_metadata()
			 *
			 * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
			 * to clear the remaining data in the object
			 */
			lOptigaOid = strtol(pkcs11configLABEL_CODE_VERIFICATION_KEY, &xEnd, 16);

			if ( (0 != lOptigaOid) && (USHRT_MAX > lOptigaOid) && (USHRT_MAX > ulDataSize))
			{
				pal_os_lock_acquire(&optiga_mutex);

				pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
				xReturn = optiga_util_write_data(pkcs11_context.object_list.optiga_util_instance,
												 (uint16_t)lOptigaOid,
												 OPTIGA_UTIL_ERASE_AND_WRITE,
												 bOffset,
												 pucData,
												 ulDataSize);

				if (OPTIGA_LIB_SUCCESS == xReturn)
				{
					while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
					{
						
					}

					if (OPTIGA_LIB_SUCCESS == pkcs11_context.object_list.optiga_lib_status)
					{
						if (OPTIGA_LIB_SUCCESS == xReturn)
							object_handle = CodeSigningKey;
					}
				}

				pal_os_lock_release(&optiga_mutex);
			}
		}

	}

	return object_handle;
}

	
CK_RV destroy_object( CK_OBJECT_HANDLE xAppHandle )
{
	uint8_t * pcLabel = NULL;
	char * pcTempLabel = NULL;
	size_t xLabelLength = 0;
	uint32_t ulObjectLength = 0;
	CK_RV xResult = CKR_OK;
	CK_BBOOL xFreeMemory = CK_FALSE;
	CK_BYTE_PTR pxObject = NULL;
	CK_OBJECT_HANDLE xPalHandle;
	CK_OBJECT_HANDLE xAppHandle2;
	CK_LONG lOptigaOid = 0;
	char* xEnd = NULL;

	find_object_in_list_by_handle( xAppHandle, &xPalHandle, &pcLabel, &xLabelLength );

	if( pcLabel != NULL )
	{
		if( (0 == memcmp( pcLabel, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, xLabelLength )))
		{
			find_object_in_list_by_label( ( uint8_t * ) pcLabel, strlen( ( char * ) pcLabel ), &xPalHandle, &xAppHandle2 );

			if( xPalHandle != CK_INVALID_HANDLE )
			{
				xResult = delete_object_from_list( xAppHandle2 );
			}

			lOptigaOid = strtol((const char *)pcLabel, &xEnd, 16);

			CK_BYTE pucDumbData[68];
			uint16_t ucDumbDataLength = 68;

			pal_os_lock_acquire(&optiga_mutex);

			pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
			xResult = optiga_crypt_ecc_generate_keypair(pkcs11_context.object_list.optiga_crypt_instance,
														OPTIGA_ECC_CURVE_NIST_P_256,
														(uint8_t)OPTIGA_KEY_USAGE_SIGN,
														FALSE,
														&lOptigaOid,
														pucDumbData,
														&ucDumbDataLength);
			if (OPTIGA_LIB_SUCCESS != xResult)
			{
				PKCS11_PRINT( ( "ERROR: Failed to invalidate a keypair \r\n" ) );
				xResult = CKR_FUNCTION_FAILED;
			}

			if (OPTIGA_LIB_SUCCESS == xResult)
			{
				while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
				{
					
				}

				// Either by timout or because of success it should end up here
				if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
				{
					PKCS11_PRINT( ( "ERROR: Failed to invalidate a keypair \r\n" ) );
					xResult = CKR_FUNCTION_FAILED;
				}
			}

			pal_os_lock_release(&optiga_mutex);

		}
		else
		{
			if( 0 == memcmp( pcLabel, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, xLabelLength ) )
			{
				pcTempLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
			}
			else if( 0 == memcmp( pcLabel, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, xLabelLength ) )
			{
				pcTempLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
			}
			else if( 0 == memcmp( pcLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, xLabelLength ) )
			{
				pcTempLabel = pkcs11configLABEL_CODE_VERIFICATION_KEY;
			}

			if (pcTempLabel != NULL)
			{
				lOptigaOid = strtol(pcTempLabel, &xEnd, 16);

				if ( (0 != lOptigaOid) && (USHRT_MAX >= lOptigaOid) )
				{

					// Erase the object
					CK_BYTE pucData[] = {0};

					pal_os_lock_acquire(&optiga_mutex);

					pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
					xResult = optiga_util_write_data(pkcs11_context.object_list.optiga_util_instance,
													 (uint16_t)lOptigaOid,
													 OPTIGA_UTIL_ERASE_AND_WRITE,
													 0, // Offset
													 pucData,
													 1);

					if (OPTIGA_LIB_SUCCESS != xResult)
					{
						xResult = CKR_FUNCTION_FAILED;
					}

					if (OPTIGA_LIB_SUCCESS == xResult)
					{
						while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
						{
							
						}

						// Either by timout or because of success it should end up here
						if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
						{
							xResult = CKR_FUNCTION_FAILED;
						}
						else
						{
							find_object_in_list_by_label( ( uint8_t * ) pcTempLabel, strlen( ( char * ) pcTempLabel ), &xPalHandle, &xAppHandle2 );

							if( xPalHandle != CK_INVALID_HANDLE )
							{
								xResult = delete_object_from_list( xAppHandle2 );
							}
						}
					}

					pal_os_lock_release(&optiga_mutex);
				}
			} else
			{
				xResult = CKR_KEY_HANDLE_INVALID;
			}
		}

		if (xAppHandle2 != xAppHandle)
			xResult = delete_object_from_list( xAppHandle );
	}
	else
	{
		xResult = CKR_KEY_HANDLE_INVALID;
	}

	if( xFreeMemory == CK_TRUE )
	{
		get_object_value_cleanup( pxObject, ulObjectLength );
	}

	return xResult;
}

CK_RV create_certificate( CK_ATTRIBUTE_PTR pxTemplate,
                            CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    CK_BYTE_PTR pxCertificateValue = NULL;
    CK_ULONG xCertificateLength = 0;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    CK_CERTIFICATE_TYPE xCertificateType = 0; /* = CKC_X_509; */
    uint32_t ulIndex = 0;
    CK_ATTRIBUTE xAttribute;

    /* Search for the pointer to the certificate VALUE. */
    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_VALUE ):
                pxCertificateValue = xAttribute.pValue;
                xCertificateLength = xAttribute.ulValueLen;
                break;

            case ( CKA_LABEL ):

                if( xAttribute.ulValueLen < pkcs11configMAX_LABEL_LENGTH )
                {
                    pxLabel = &pxTemplate[ ulIndex ];
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }

                break;

            case ( CKA_CERTIFICATE_TYPE ):
                memcpy( &xCertificateType, xAttribute.pValue, sizeof( CK_CERTIFICATE_TYPE ) );

                if( xCertificateType != CKC_X_509 )
                {
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_CLASS ):
            case ( CKA_SUBJECT ):
            case ( CKA_TOKEN ):

                /* Do nothing.  This was already parsed out of the template previously. */
                break;

            default:
                xResult = CKR_TEMPLATE_INCONSISTENT;
                break;
        }
    }

    if( ( pxCertificateValue == NULL ) || ( pxLabel == NULL ) )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;
    }

    if( xResult == CKR_OK )
    {
        xPalHandle = save_object( pxLabel, pxCertificateValue, xCertificateLength );

        if( xPalHandle == 0 ) /*Invalid handle. */
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = add_object_to_list( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
        /* TODO: If this fails, should the object be wiped back out of flash?  But what if that fails?!?!? */
    }

    return xResult;
}

#define PKCS11_INVALID_KEY_TYPE    ( ( CK_KEY_TYPE ) 0xFFFFFFFF )

CK_KEY_TYPE get_key_type( CK_ATTRIBUTE_PTR pxTemplate,
                           CK_ULONG ulCount )
{
    CK_KEY_TYPE xKeyType = PKCS11_INVALID_KEY_TYPE;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        if( xAttribute.type == CKA_KEY_TYPE )
        {
            memcpy( &xKeyType, xAttribute.pValue, sizeof( CK_KEY_TYPE ) );
            break;
        }
    }

    return xKeyType;
}

void get_label( CK_ATTRIBUTE_PTR * ppxLabel,
                  CK_ATTRIBUTE_PTR pxTemplate,
                  CK_ULONG ulCount )
{
    CK_ATTRIBUTE xAttribute;
    uint32_t ulIndex;

    *ppxLabel = NULL;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        if( xAttribute.type == CKA_LABEL )
        {
            *ppxLabel = &pxTemplate[ ulIndex ];
            break;
        }
    }
}



CK_RV create_ec_public_key( uint8_t* pxPublicKey,
                            uint32_t* pulKeySize,
                            CK_ATTRIBUTE_PTR * ppxLabel,
                            CK_ATTRIBUTE_PTR pxTemplate,
                            CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_CLASS ):
            case ( CKA_KEY_TYPE ):
            case ( CKA_TOKEN ):

                /* Do nothing.
                 * At this time there is only token object support.
                 * Key type and class were checked previously. */
                break;

            case ( CKA_LABEL ):

                if( xAttribute.ulValueLen < pkcs11configMAX_LABEL_LENGTH )
                {
                    *ppxLabel = &pxTemplate[ ulIndex ];
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }

                break;

            case ( CKA_VERIFY ):
                memcpy( &xBool, xAttribute.pValue, xAttribute.ulValueLen );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "Only EC Public Keys with verify permissions supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_EC_PARAMS ):

                if( memcmp( ( CK_BYTE[] ) pkcs11DER_ENCODED_OID_P256, xAttribute.pValue, xAttribute.ulValueLen ) )
                {
                    PKCS11_PRINT( ( "ERROR: Only elliptic curve P-256 is supported.\r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_EC_POINT ):
                if (*pulKeySize > (xAttribute.ulValueLen - 2))
                {
                    /* The first 2 bytes are for ASN1 type/length encoding. */
                    memcpy(pxPublicKey,  ( ( uint8_t * ) ( xAttribute.pValue ) + 2 ),  ( xAttribute.ulValueLen - 2 ));
                    *pulKeySize = xAttribute.ulValueLen - 2;
                }
                else
                {
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            default:
                PKCS11_PRINT( ( "Unsupported attribute found for EC public key. %d \r\n", xAttribute.type ) );
                xResult = CKR_ATTRIBUTE_TYPE_INVALID;
                break;
        }
    }

    return xResult;
}

CK_RV create_public_key( CK_ATTRIBUTE_PTR pxTemplate,
                          CK_ULONG ulCount,
                          CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_KEY_TYPE xKeyType;
    CK_RV xResult = CKR_OK;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;

    uint8_t pxPublicKey[MAX_PUBLIC_KEY_SIZE];
    uint32_t ulKeySize = MAX_PUBLIC_KEY_SIZE;

    xKeyType = get_key_type( pxTemplate, ulCount );

    if( ( xKeyType == CKK_EC ) || ( xKeyType == CKK_ECDSA ) )
    {
        get_label( &pxLabel, pxTemplate, ulCount );

        xResult = create_ec_public_key( pxPublicKey, &ulKeySize, &pxLabel, pxTemplate, ulCount, pxObject );
    }
    else
    {
        PKCS11_PRINT( ( "Invalid key type %d \r\n", xKeyType ) );
        xResult = CKR_TEMPLATE_INCONSISTENT;
    }

    if( xResult == CKR_OK )
    {
        xPalHandle = save_object( pxLabel, pxPublicKey, ulKeySize );

        if( xPalHandle == CK_INVALID_HANDLE )
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = add_object_to_list( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
    }

    return xResult;
}
CK_RV get_object_class( CK_ATTRIBUTE_PTR pxTemplate,
                         CK_ULONG ulCount,
                         CK_OBJECT_CLASS * pxClass )
{
    CK_RV xResult = CKR_TEMPLATE_INCOMPLETE;
    uint32_t ulIndex = 0;

    /* Search template for class attribute. */
    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        CK_ATTRIBUTE xAttribute = pxTemplate[ ulIndex ];

        if( xAttribute.type == CKA_CLASS )
        {
            memcpy( pxClass, xAttribute.pValue, sizeof( CK_OBJECT_CLASS ) );
            xResult = CKR_OK;
            break;
        }
    }

    return xResult;
}

/**
 * @brief Provides import and storage of a single client certificate.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulCount,
                                             CK_OBJECT_HANDLE_PTR pxObject )
{ 
	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    CK_OBJECT_CLASS xClass;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    /*
     * Check parameters.
     */
    if( ( NULL == pxTemplate ) ||
        ( NULL == pxObject ) ||
        (CKR_OK != xResult))
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xResult == CKR_OK )
    {
        xResult = get_object_class( pxTemplate, ulCount, &xClass );
    }

    if( xResult == CKR_OK )
    {
        switch( xClass )
        {
            case CKO_CERTIFICATE:
                xResult = create_certificate( pxTemplate, ulCount, pxObject );
                break;

            case CKO_PRIVATE_KEY:
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                break;

            case CKO_PUBLIC_KEY:
                xResult = create_public_key( pxTemplate, ulCount, pxObject );
                break;

            default:
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                break;
        }
    }

    return xResult;
}

/**
 * @brief Free resources attached to an object handle.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{
    CK_RV xResult;

    ( void ) xSession;
    ( void ) xObject;



    xResult = destroy_object( xObject );

    return xResult;
}
/**
 * @brief Initialize the Cryptoki module for use.
 */

CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{   
	/*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );

    CK_RV xResult = CKR_OK;

    /* Ensure that the FreeRTOS heap is used. */
//        CRYPTO_ConfigureHeap();

    if( pkcs11_context.is_initialized != CK_TRUE )
    {
        /*
         *   Reset OPTIGA(TM) Trust M and open an application on it
         */
        xResult = optiga_trustm_initialize();

        if (xResult == CKR_OK)
        {
        	pkcs11_context.is_initialized = CK_TRUE;
        }

    }
    else
    {
        xResult = CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    return xResult;
}


/**
 * @brief Un-initialize the Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
{
    /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL != pvReserved )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xResult == CKR_OK )
    {
    	if( pkcs11_context.is_initialized == CK_FALSE )
		{
			xResult = CKR_CRYPTOKI_NOT_INITIALIZED;
		}
		else
		{
			/*
			 *   Reset OPTIGA(TM) Trust M and open an application on it
			 */
			xResult = optiga_trustm_deinitialize();
		}

		if( xResult == CKR_OK )
		{

			sem_destroy( &pkcs11_context.object_list.semaphore );

			pkcs11_context.is_initialized = CK_FALSE;
		}
		pal_gpio_deinit(&optiga_reset_0);
        pal_gpio_deinit(&optiga_vdd_0);
    }

    return xResult;
}

/**
 * @brief Query the list of interface function pointers.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{   
	
	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL == ppxFunctionList )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        *ppxFunctionList = &prvP11FunctionList;
    }

    return xResult;
}

/**
 * @brief Query the list of slots. A single default slot is implemented.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL xTokenPresent,
                                            CK_SLOT_ID_PTR pxSlotList,
                                            CK_ULONG_PTR pulCount )
{   
	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    /* Since the implementation of PKCS#11 does not depend
     * on a physical token, this parameter is ignored. */
    ( void ) ( xTokenPresent );

    if( NULL == pulCount )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else if( NULL == pxSlotList )
    {
        *pulCount = 1;
    }
    else
    {
        if( 0u == *pulCount )
        {
            xResult = CKR_BUFFER_TOO_SMALL;
        }
        else
        {
            pxSlotList[ 0 ] = pkcs11SLOT_ID;
            *pulCount = 1;
        }
    }

    return xResult;
}

/**
 * @brief Returns firmware, hardware, manufacturer, and model information for
 * the crypto token.
 *
 * @return CKR_OK.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetTokenInfo )( CK_SLOT_ID slotID,
                                              CK_TOKEN_INFO_PTR pInfo )
{
	
    CK_RV xResult = CKR_SLOT_ID_INVALID;
	
	do
	{
		if ( pkcs11SLOT_ID != slotID)
		{
			break;
		}
	    if ( pInfo == NULL )
	    {
			xResult = CKR_ARGUMENTS_BAD;
			break;
	    }
		pInfo->firmwareVersion.major = 0x2;
        pInfo->firmwareVersion.minor = 0x28;

        pInfo->hardwareVersion.major = 1;
        pInfo->hardwareVersion.minor = 0;

        sprintf((char *)pInfo->manufacturerID, "Infineon Technologies AG");
        sprintf((char *)pInfo->model, "OPTIGA Trust M");
        pInfo->ulMaxSessionCount = 4;
		xResult = CKR_OK;
		
	} while(0);
	
    return xResult;
}


/**
 * @brief This function obtains information about a particular
 * mechanism possibly supported by a token.
 *
 *  \param[in]  xSlotID         This parameter is unused in this port.
 *  \param[in]  type            The cryptographic capability for which support
 *                              information is being queried.
 *  \param[out] pInfo           Algorithm sizes and flags for the requested
 *                              mechanism, if supported.
 *
 * @return CKR_OK if the mechanism is supported. Otherwise, CKR_MECHANISM_INVALID.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetMechanismInfo )( CK_SLOT_ID slotID,
                                                  CK_MECHANISM_TYPE type,
                                                  CK_MECHANISM_INFO_PTR pInfo )
{

    CK_RV xResult = CKR_MECHANISM_INVALID;

    struct CryptoMechanisms
    {
        CK_MECHANISM_TYPE xType;
        CK_MECHANISM_INFO xInfo;
    }
    pxSupportedMechanisms[] =
    {
        { CKM_ECDSA,           		 { 256,  521,  CKF_SIGN | CKF_VERIFY 							 } },
        { CKM_RSA_PKCS,              { 1024, 2048, CKF_SIGN | CKF_VERIFY | CKA_ENCRYPT | CKA_DECRYPT } },
        { CKM_RSA_PKCS_KEY_PAIR_GEN, { 1024, 2048, CKF_GENERATE_KEY_PAIR 							 } },
        { CKM_EC_KEY_PAIR_GEN, 		 { 256,  521,  CKF_GENERATE_KEY_PAIR 							 } },
        { CKM_SHA256,          		 { 0,    0,    CKF_DIGEST             							 } }
    };
    uint32_t ulMech = 0;

    /* Look for the requested mechanism in the above table. */
    for( ; ulMech < sizeof( pxSupportedMechanisms ) / sizeof( pxSupportedMechanisms[ 0 ] ); ulMech++ )
    {
        if( pxSupportedMechanisms[ ulMech ].xType == type )
        {
            /* The mechanism is supported. Copy out the details and break
             * out of the loop. */

            memcpy( pInfo, &( pxSupportedMechanisms[ ulMech ].xInfo ), sizeof( CK_MECHANISM_INFO ) );
            xResult = CKR_OK;
            break;
        }
    }


    return xResult;
}


/**
 * @brief This function is not implemented for this port.
 *
 * C_InitToken() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does not add any security.
 *
 * @return CKR_OK.
 */
CK_DEFINE_FUNCTION( CK_RV, C_InitToken )( CK_SLOT_ID slotID,
                                           CK_UTF8CHAR_PTR pPin,
                                           CK_ULONG ulPinLen,
                                           CK_UTF8CHAR_PTR pLabel )
{

    /* Avoid compiler warnings about unused variables. */
    ( void ) slotID;
    ( void ) pPin;
    ( void ) ulPinLen;
    ( void ) pLabel;

    return CKR_OK;
}


/**
 * @brief Start a session for a cryptographic command sequence.
 */
CK_DEFINE_FUNCTION( CK_RV, C_OpenSession )( CK_SLOT_ID xSlotID,
                                            CK_FLAGS xFlags,
                                            CK_VOID_PTR pvApplication,
                                            CK_NOTIFY xNotify,
                                            CK_SESSION_HANDLE_PTR pxSession )
{   

	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    p_pkcs11_session_t pxSessionObj = NULL;

    ( void ) ( xSlotID );
    ( void ) ( pvApplication );
    ( void ) ( xNotify );
    /* Check that the PKCS #11 module is initialized. */
    if( pkcs11_context.is_initialized != CK_TRUE )
    {
        xResult = CKR_CRYPTOKI_NOT_INITIALIZED;
    }
    /* Check arguments. */
    if( NULL == pxSession )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    /* For legacy reasons, the CKF_SERIAL_SESSION bit MUST always be set. */
    if( 0 == ( CKF_SERIAL_SESSION & xFlags ) )
    {
        xResult = CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }
    /*
     * Make space for the context.
     */
    if( CKR_OK == xResult )
    {
        pxSessionObj = ( p_pkcs11_session_t ) malloc( sizeof( struct pkcs11_session ) );
        if( NULL == pxSessionObj )
        {
            xResult = CKR_HOST_MEMORY;
        }

        /*
         * Zero out the session structure.
         */
        if( CKR_OK == xResult )
        {
            memset( pxSessionObj, 0, sizeof( pkcs11_session_t ) );
        }
    }

    if( CKR_OK == xResult )
    {
        /*
         * Assign the session.
         */
        pxSessionObj->state =
            0u != ( xFlags & CKF_RW_SESSION ) ? CKS_RW_PUBLIC_SESSION : CKS_RO_PUBLIC_SESSION;
        pxSessionObj->opened = CK_TRUE;

        /*
         * Return the session.
         */
        *pxSession = ( CK_SESSION_HANDLE ) pxSessionObj; /*lint !e923 Allow casting pointer to integer type for handle. */
    }

    /*
     *   Initialize the operation in progress.
     */
    if( CKR_OK == xResult )
    {
        pxSessionObj->operation_in_progress = pkcs11NO_OPERATION;
    }
    if( ( NULL != pxSessionObj ) && ( CKR_OK != xResult ) )
    {

        free( pxSessionObj );
    }

    return xResult;
}

/**
 * @brief Terminate a session and release resources.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE xSession )
{  

	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    p_pkcs11_session_t pxSession = get_session_pointer( xSession );

    if( (xResult == CKR_OK) && (NULL != pxSession) )
    {
        free( pxSession );
    }
    else
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_Login )( CK_SESSION_HANDLE hSession,
                                      CK_USER_TYPE userType,
                                      CK_UTF8CHAR_PTR pPin,
                                      CK_ULONG ulPinLen )
{

    /* THIS FUNCTION IS NOT IMPLEMENTED
     * If login capability is required, implement it here.
     * Defined for compatibility with other PKCS #11 ports. */
    return CKR_OK;
}

/**
 * @brief Query the value of the specified cryptographic object attribute.
 * Regarding keys, only ECDSA P256 is supported by this implementation.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetAttributeValue )( CK_SESSION_HANDLE xSession,
                                                  CK_OBJECT_HANDLE xObject,
                                                  CK_ATTRIBUTE_PTR pxTemplate,
                                                  CK_ULONG ulCount )
{

    /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    CK_BBOOL xIsPrivate = CK_TRUE;
    CK_ULONG iAttrib;
    CK_KEY_TYPE xPkcsKeyType = ( CK_KEY_TYPE ) ~0;
    CK_OBJECT_CLASS xClass;
    uint8_t * pxObjectValue = NULL;
    uint32_t ulLength = 0;
    uint8_t ucP256Oid[] = pkcs11DER_ENCODED_OID_P256;
    uint8_t ucP384Oid[] = pkcs11DER_ENCODED_OID_P384;
    uint8_t ucP521Oid[] = pkcs11DER_ENCODED_OID_P521;
	uint8_t temp_ec_value[11] = {0};
	uint8_t temp_ec_length = 0;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    size_t xSize;
    uint8_t * pcLabel = NULL;
	
	p_pkcs11_session_t session = get_session_pointer( xSession );
    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    if( (CKR_OK != xResult) || ( NULL == pxTemplate ) || ( 0 == ulCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        /*
         * Copy the object into a buffer.
         */
        find_object_in_list_by_handle( xObject, &xPalHandle, &pcLabel, &xSize ); /*pcLabel and xSize are ignored. */

        if( xPalHandle != CK_INVALID_HANDLE && xPalHandle != DevicePrivateKey )
        {
            xResult = get_object_value( xPalHandle, &pxObjectValue, &ulLength, &xIsPrivate );
        }
        else if (xPalHandle == CK_INVALID_HANDLE)
        {
            xResult = CKR_DATA_INVALID;
        }
    }

    if ( CKR_OK != xResult)
    {
        xResult = CKR_DATA_INVALID;
    }
    else {
        switch (xPalHandle) {
        case DevicePrivateKey:
            xClass = CKO_PRIVATE_KEY;
            break;
        case DevicePublicKey:
            xClass = CKO_PUBLIC_KEY;
            break;
        case DeviceCertificate:
            xClass = CKO_CERTIFICATE;
            break;
        default:
            xResult = CKR_DATA_INVALID;
            break;
        }
    }

    if( xResult == CKR_OK )
    {
        for( iAttrib = 0; iAttrib < ulCount && CKR_OK == xResult; iAttrib++ )
        {
            switch( pxTemplate[ iAttrib ].type )
            {
                case CKA_CLASS:

                    if( pxTemplate[ iAttrib ].pValue == NULL )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = sizeof( CK_OBJECT_CLASS );
                    }
                    else
                    {
                        if( pxTemplate[ iAttrib ].ulValueLen >= sizeof( CK_OBJECT_CLASS ) )
                        {
                            memcpy( pxTemplate[ iAttrib ].pValue, &xClass, sizeof( CK_OBJECT_CLASS ) );
                        }
                        else
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                    }

                    break;

                case CKA_VALUE:

                    if( xIsPrivate == CK_TRUE )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = CK_UNAVAILABLE_INFORMATION;
                        xResult = CKR_ATTRIBUTE_SENSITIVE;
                    }
                    else
                    {
                        if( pxTemplate[ iAttrib ].pValue == NULL )
                        {
                            pxTemplate[ iAttrib ].ulValueLen = ulLength;
                        }
                        else if( pxTemplate[ iAttrib ].ulValueLen < ulLength )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            memcpy( pxTemplate[ iAttrib ].pValue, pxObjectValue, ulLength );
                        }
                    }

                    break;

                case CKA_KEY_TYPE:

                    if( pxTemplate[ iAttrib ].pValue == NULL )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = sizeof( CK_KEY_TYPE );
                    }
                    else if( pxTemplate[ iAttrib ].ulValueLen < sizeof( CK_KEY_TYPE ) )
                    {
                        xResult = CKR_BUFFER_TOO_SMALL;
                    }
                    else
                    {
                        if( 0 != xResult )
                        {
                            xResult = CKR_FUNCTION_FAILED;
                        }
                        else
                        {
                            /* Elliptic-curve is the only asymmetric cryptosystem
                             * supported by this implementation. */
                            xPkcsKeyType = CKK_EC;

                            memcpy( pxTemplate[ iAttrib ].pValue, &xPkcsKeyType, sizeof( CK_KEY_TYPE ) );
                        }
                    }

                    break;

                case CKA_EC_PARAMS:

                    if ( session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_256)
					{
						temp_ec_length = sizeof(ucP256Oid);
						memcpy(temp_ec_value,ucP256Oid,temp_ec_length);
					}
                    else if ( session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_384)
					{
						temp_ec_length = sizeof(ucP384Oid);
						memcpy(temp_ec_value,ucP384Oid,temp_ec_length);
					}
                    else if ( session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_521)
					{
						temp_ec_length = sizeof(ucP521Oid);
						memcpy(temp_ec_value,ucP521Oid,temp_ec_length);
					}

                    pxTemplate[ iAttrib ].ulValueLen = temp_ec_length;

                    if( pxTemplate[ iAttrib ].pValue != NULL )
                    {
                        if( pxTemplate[ iAttrib ].ulValueLen < temp_ec_length )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            memcpy( pxTemplate[ iAttrib ].pValue, temp_ec_value, temp_ec_length );
                        }
                    }

                    break;

                case CKA_EC_POINT:

                    if( pxTemplate[ iAttrib ].pValue == NULL )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = 67; /* TODO: Is this large enough?*/
                    }
                    else
                    {
                        if (pxTemplate[ iAttrib ].ulValueLen < ulLength )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            memcpy( ( uint8_t * ) pxTemplate[ iAttrib ].pValue,  ( uint8_t * ) pxObjectValue, ulLength);
                            pxTemplate[ iAttrib ].ulValueLen = ulLength;
                        }
                    }

                    break;

                default:
                    xResult = CKR_ATTRIBUTE_TYPE_INVALID;
            }
        }

        /* Free the buffer where object was stored. */
        get_object_value_cleanup( pxObjectValue, ulLength );
    }

    return xResult;
}


/**
 * @brief Begin an enumeration sequence for the objects of the specified type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount )
{

    p_pkcs11_session_t pxSession = get_session_pointer( xSession );
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    CK_BYTE * find_object_lable = NULL;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    /* Check inputs. */
    if( ( pxSession == NULL ) || ( pxSession->opened != CK_TRUE ) )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
        PKCS11_PRINT( ( "ERROR: Invalid session. \r\n" ) );
    }
    else if( pxSession->find_object_lable != NULL )
    {
        xResult = CKR_OPERATION_ACTIVE;
        PKCS11_PRINT( ( "ERROR: Find object operation already in progress. \r\n" ) );
    }
    else if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else if( ( ulCount != 1 ) && ( ulCount != 2 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        PKCS11_PRINT( ( "ERROR: Find objects does not support searching by %d attributes. \r\n", ulCount ) );
    }

    /* Malloc space to save template information. */
    if( xResult == CKR_OK )
    {
        find_object_lable = malloc( pxTemplate->ulValueLen + 1 ); /* Add 1 to guarantee null termination for PAL. */
        pxSession->find_object_lable = find_object_lable;

        if( find_object_lable != NULL )
        {
            memset( find_object_lable, 0, pxTemplate->ulValueLen + 1 );
        }
        else
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    /* Search template for label.
     * NOTE: This port only supports looking up objects by CKA_LABEL and all
     * other search attributes are ignored. */
    if( xResult == CKR_OK )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;

        for( ulIndex = 0; ulIndex < ulCount; ulIndex++ ) /* TODO: Re-evaluate the need for this for loop... we are making bad assumptions if 2 objects have the same label anyhow! */
        {
            xAttribute = pxTemplate[ ulIndex ];

            if( xAttribute.type == CKA_LABEL )
            {
                memcpy( pxSession->find_object_lable, xAttribute.pValue, xAttribute.ulValueLen );
                xResult = CKR_OK;
            }
            else
            {
                PKCS11_WARNING_PRINT( ( "WARNING: Search parameters other than label are ignored.\r\n" ) );
            }
        }
    }

    /* Clean up memory if there was an error parsing the template. */
    if( xResult != CKR_OK )
    {
        if( find_object_lable != NULL )
        {
            free( find_object_lable );
            pxSession->find_object_lable = NULL;
        }
    }

    return xResult;
}

/**
 * @brief Query the objects of the requested type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE xSession,
                                            CK_OBJECT_HANDLE_PTR pxObject,
                                            CK_ULONG ulMaxObjectCount,
                                            CK_ULONG_PTR pulObjectCount )
{  

	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    long xDone = 0;
    p_pkcs11_session_t pxSession = get_session_pointer( xSession );

    CK_BYTE_PTR pcObjectValue = NULL;
    uint32_t xObjectLength = 0;
    CK_BBOOL xIsPrivate = CK_TRUE;
    CK_BYTE xByte = 0;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint32_t ulIndex;

    /*
     * Check parameters.
     */
    if( ( CKR_OK != xResult ) ||
        ( NULL == pxObject ) ||
        ( NULL == pulObjectCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = 1;
    }

    if( pxSession->find_object_lable == NULL )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
        xDone = 1;
    }

    if( 0u == ulMaxObjectCount )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = 1;
    }

    if( 1u != ulMaxObjectCount )
    {
        PKCS11_WARNING_PRINT( ( "WARN: Searching for more than 1 object not supported. \r\n" ) );
    }

    if( ( 0 == xDone ) && ( ( CK_BBOOL ) CK_TRUE == pxSession->find_object_complete ) )
    {
        *pulObjectCount = 0;
        xResult = CKR_OK;
        xDone = 1;
    }

    if( ( 0 == xDone ) )
    {
        /* Try to find the object in module's list first. */
        find_object_in_list_by_label( pxSession->find_object_lable, strlen( ( const char * ) pxSession->find_object_lable ), &xPalHandle, pxObject );

        /* Check with the PAL if the object was previously stored. */
        if( *pxObject == CK_INVALID_HANDLE )
        {
            xPalHandle = find_object( pxSession->find_object_lable, ( uint8_t ) strlen( ( const char * ) pxSession->find_object_lable ) );
        }

        if( xPalHandle != CK_INVALID_HANDLE && xPalHandle != DevicePrivateKey)
        {
            xResult = get_object_value( xPalHandle, &pcObjectValue, &xObjectLength, &xIsPrivate );

            if( ( xResult == CKR_OK ) && ( xObjectLength == 0 ) )
            {
                *pulObjectCount = 0;
                xResult = CKR_OK;
                xDone = 1;
            }
            else if ( xResult == CKR_OK )
            {
                for( ulIndex = 0; ulIndex < xObjectLength; ulIndex++ )
                {
                    xByte |= pcObjectValue[ ulIndex ];
                }

                if( xObjectLength == 1 ) /* Deleted objects are overwritten completely w/ zero. */
                {
                    *pxObject = CK_INVALID_HANDLE;
                }
                else
                {
                    xResult = add_object_to_list( xPalHandle, pxObject, pxSession->find_object_lable, strlen( ( const char * ) pxSession->find_object_lable ) );
                    *pulObjectCount = 1;
                }

                get_object_value_cleanup( pcObjectValue, xObjectLength );
            }
        }
        else if (xPalHandle == DevicePrivateKey)
        {
            *pulObjectCount = 1;
            xResult = CKR_OK;
        }
        else
        {
            /*xIsObjectWithNoNvmStorage(pxSession->find_object_lable, strlen(pxSession->find_object_lable), ) */
            PKCS11_PRINT( ( "ERROR: Object with label '%s' not found. \r\n", ( char * ) pxSession->find_object_lable ) );
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}

/**
 * @brief Terminate object enumeration.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE xSession )
{   

	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    p_pkcs11_session_t pxSession = get_session_pointer( xSession );

    /*
     * Check parameters.
     */
    if( pxSession->opened != CK_TRUE )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    if( pxSession->find_object_lable == NULL )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        /*
         * Clean-up find objects state.
         */
        free( pxSession->find_object_lable );
        pxSession->find_object_lable = NULL;
    }

    return xResult;
}



CK_RV verify_private_key_template(CK_SESSION_HANDLE xSession,
									 CK_ATTRIBUTE_PTR * ppxLabel,
                                     CK_ATTRIBUTE_PTR pxTemplate,
                                     CK_ULONG ulTemplateLength )
{

#define LABEL      ( 1U )
#define PRIVATE    ( 1U << 1 )
#define SIGN       ( 1U << 2 ) 
#define DECRYPT    ( 1U << 3 ) 

    CK_ATTRIBUTE xAttribute;
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
    CK_ULONG xTemp;
    CK_ULONG xIndex;
	CK_BBOOL is_error = FALSE;
	uint32_t received_attribute = 0;
	uint32_t ec_expected_attribute = ( LABEL | PRIVATE | SIGN );
	uint32_t rsa_expected_attribute[] = {( LABEL | PRIVATE | SIGN | DECRYPT ),
										  ( LABEL | PRIVATE | DECRYPT ),
										  ( LABEL | PRIVATE | SIGN )
										};

	p_pkcs11_session_t session = get_session_pointer( xSession );

    for( xIndex = 0; xIndex < ulTemplateLength; xIndex++ )
    {
    	if(TRUE == is_error)
    	{
			break;
		}

        xAttribute = pxTemplate[ xIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_LABEL ):
			{
                *ppxLabel = &pxTemplate[ xIndex ];
				received_attribute |= LABEL;
            }
			break;
            case ( CKA_TOKEN ):
			{
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key generation is supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
					is_error = TRUE;
					break;
                }
        	}
            break;
            case ( CKA_KEY_TYPE ):
			{
                memcpy( &xTemp, xAttribute.pValue, sizeof( CK_ULONG ) );

                if( (xTemp != CKK_EC) && (xTemp != CKK_RSA))
                {
                    PKCS11_PRINT( ( "ERROR: Only EC and RSA key pair generation is supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
					is_error = TRUE;
					break;
                }
        	}
            break;
            case ( CKA_PRIVATE ):
			{
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Generating private keys that are not marked private is not supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
					is_error = TRUE;
					break;
                }
				received_attribute |= PRIVATE;
            }
            break;
            case ( CKA_SIGN ):
			{
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
					break;
                }
				session->key_template_enabled |= PKCS_SIGN_ENABLE;
				received_attribute |= SIGN;
            }
            break;
			case ( CKA_DECRYPT ):
			{
				memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );
			
				if( xBool != CK_TRUE )
				{
					break;
				}
				session->key_template_enabled |= PKCS_DECRYPT_ENABLE;
				received_attribute |= DECRYPT;
			}
			break;
			default:
			{
				is_error = TRUE;
				xResult = CKR_TEMPLATE_INCONSISTENT;
			}
			break;

        }
    }
	
	if ( xTemp == CKK_EC )
	{
		if( ( received_attribute & ec_expected_attribute ) != ec_expected_attribute )
	    {
	        xResult = CKR_ATTRIBUTE_VALUE_INVALID;
	    }		
	}
    else if ( xTemp == CKK_RSA )
    {
			for(xIndex = 0; xIndex < sizeof(rsa_expected_attribute)/sizeof(uint32_t); xIndex++)
		    {
		    	if(received_attribute == rsa_expected_attribute[xIndex])
		    	{
					break;
				}
		        
		    }
			if (xIndex == sizeof(rsa_expected_attribute)/sizeof(uint32_t))
			{
				xResult = CKR_TEMPLATE_INCONSISTENT;
			}
    }
	else
	{
		xResult = CKR_ATTRIBUTE_VALUE_INVALID;
	}
	


    return xResult;
}

CK_RV verify_public_key_template( CK_SESSION_HANDLE xSession,
									  CK_ATTRIBUTE_PTR * ppxLabel,
                                      CK_ATTRIBUTE_PTR pxTemplate,
                                      CK_ULONG ulTemplateLength )
{

#define LABEL        ( 1U )
#define EC_PARAMS    ( 1U << 1 )
#define VERIFY       ( 1U << 2 )
#define ENCRYPT      ( 1U << 3 )
#define MODULUS      ( 1U << 4 )
#define EXPONENT	 ( 1U << 5 )

    CK_ATTRIBUTE xAttribute;
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
	CK_BBOOL is_error = FALSE;
	CK_ULONG modulus_bits;
	CK_BYTE exp_bits[] = {0x01, 0x00, 0x01};
    CK_KEY_TYPE xKeyType;
    CK_BYTE ec_param_p256[] = pkcs11DER_ENCODED_OID_P256;
    CK_BYTE ec_param_p384[] = pkcs11DER_ENCODED_OID_P384;
    CK_BYTE ec_param_p521[] = pkcs11DER_ENCODED_OID_P521;
    int lCompare;
    CK_ULONG ulIndex;
	uint32_t received_attribute = 0;
	uint32_t ec_expected_attribute = ( LABEL | EC_PARAMS | VERIFY );
	uint32_t rsa_expected_attribute[] = {( LABEL | ENCRYPT | VERIFY | MODULUS |EXPONENT ),
										 ( LABEL | ENCRYPT | MODULUS | EXPONENT ),
										 ( LABEL | VERIFY | MODULUS | EXPONENT ),
										};
	
	p_pkcs11_session_t session = get_session_pointer( xSession );
	
    for( ulIndex = 0; ulIndex < ulTemplateLength; ulIndex++ )
    {
    	if(TRUE == is_error)
    	{
			break;
		}
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_LABEL ):
			{
                *ppxLabel = &pxTemplate[ ulIndex ];
				received_attribute |= LABEL;
            }
			break;
            case ( CKA_KEY_TYPE ):
			{
                memcpy( &xKeyType, xAttribute.pValue, sizeof( CK_KEY_TYPE ) );

                if( ( xKeyType != CKK_EC ) && ( xKeyType != CKK_RSA ) )
                {
                    PKCS11_PRINT( ( "ERROR: Only EC and RSA key pair generation is supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
					is_error = TRUE;
                }
            }
            break;
            case ( CKA_EC_PARAMS ):
			{
                if (0 == memcmp( ec_param_p256, xAttribute.pValue, sizeof( ec_param_p256 ) ))
				{
					session->ec_key_type = OPTIGA_ECC_CURVE_NIST_P_256;
					session->ec_key_size = 0x44;
					received_attribute |= EC_PARAMS;
				}
				else if(0 == memcmp( ec_param_p384, xAttribute.pValue, sizeof( ec_param_p384 )))
				{
					session->ec_key_type = OPTIGA_ECC_CURVE_NIST_P_384;
					session->ec_key_size = 0x64;
					received_attribute |= EC_PARAMS;
				}
				else if(0 == memcmp( ec_param_p521, xAttribute.pValue, sizeof( ec_param_p521 )))
				{
					session->ec_key_type = OPTIGA_ECC_CURVE_NIST_P_521;
					session->ec_key_size = 0x89;
					received_attribute |= EC_PARAMS;
				}
                else
                {
                    PKCS11_PRINT( ( "ERROR: key generation is supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
					is_error = TRUE;
                }
            }
            break;
            case ( CKA_VERIFY ):
			{
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Generating public keys that cannot verify is not supported. \r\n" ) );
					break;
                }
				session->key_template_enabled |= PKCS_VERIFY_ENABLE;
				received_attribute |= VERIFY;
            }
            break;
            case ( CKA_TOKEN ):
			{
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key generation is supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
					is_error = TRUE;
                }
            }
            break;
			case ( CKA_ENCRYPT ):
			{
				memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );
			
				if( xBool != CK_TRUE )
				{
					PKCS11_PRINT( ( "ERROR: Generating public keys that cannot encrypt is not supported. \r\n" ) );
					break;
				}
				session->key_template_enabled |= PKCS_ENCRYPT_ENABLE;
				received_attribute |= ENCRYPT;
			}
			break;
			case ( CKA_MODULUS_BITS ):
			{
				memcpy( &modulus_bits, xAttribute.pValue, sizeof( CK_ULONG ) );
			
				if( modulus_bits == 0 )
				{
					PKCS11_PRINT( ( "ERROR: Generating public keys that cannot modulus size is not supported. \r\n" ) );
					xResult = CKR_ATTRIBUTE_VALUE_INVALID;
					is_error = TRUE;
					break;
				}
				session->rsa_key_size = modulus_bits;
				received_attribute |= MODULUS;
			}
			break;
			case ( CKA_PUBLIC_EXPONENT ):
			{ 
			  if (0 != memcmp( exp_bits, xAttribute.pValue, sizeof( exp_bits ) ))
				{
                	xResult = CKR_TEMPLATE_INCONSISTENT;
					is_error = TRUE;
					break;
				}
				received_attribute |= EXPONENT;
			}
			break;
            default:
			{
				is_error = TRUE;
                xResult = CKR_TEMPLATE_INCONSISTENT;
            }
			break;
        }
    }
	if ( CKR_OK == xResult )
	{
		if ( xKeyType == CKK_EC )
		{
			if( (( received_attribute & rsa_expected_attribute[0] ) == rsa_expected_attribute[0] ) ||
				(( received_attribute & ( LABEL | ENCRYPT | MODULUS | EXPONENT ) ) == 
				( LABEL | ENCRYPT | MODULUS | EXPONENT ) ))
		    {
		        xResult = CKR_ATTRIBUTE_VALUE_INVALID;
		    }
			else if( ( received_attribute & ec_expected_attribute ) != ec_expected_attribute )
		    {
		        xResult = CKR_TEMPLATE_INCONSISTENT;
		    }		
		}
	    else if ( xKeyType == CKK_RSA )
	    {
			for(ulIndex = 0; ulIndex < sizeof(rsa_expected_attribute)/sizeof(uint32_t); ulIndex++)
		    {
		    	if(received_attribute == rsa_expected_attribute[ulIndex])
		    	{
					break;
				}
		        
		    }
			if (ulIndex == sizeof(rsa_expected_attribute)/sizeof(uint32_t))
			{
				xResult = CKR_TEMPLATE_INCONSISTENT;
			}
			if (received_attribute & EC_PARAMS)
			{
				xResult = CKR_ATTRIBUTE_VALUE_INVALID;
			}
	    }
		else
		{
			xResult = CKR_ATTRIBUTE_VALUE_INVALID;
		}
	}
    return xResult;
}




/**
 * @brief Generate a new public-private key pair.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateKeyPair )( CK_SESSION_HANDLE xSession,
                                                CK_MECHANISM_PTR pxMechanism,
                                                CK_ATTRIBUTE_PTR pxPublicKeyTemplate,
                                                CK_ULONG ulPublicKeyAttributeCount,
                                                CK_ATTRIBUTE_PTR pxPrivateKeyTemplate,
                                                CK_ULONG ulPrivateKeyAttributeCount,
                                                CK_OBJECT_HANDLE_PTR pxPublicKey,
                                                CK_OBJECT_HANDLE_PTR pxPrivateKey )
{

    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    uint8_t * pucPublicKeyDer = NULL;
    uint16_t ucPublicKeyDerLength = 0;
    CK_ATTRIBUTE_PTR pxPrivateLabel = NULL;
    CK_ATTRIBUTE_PTR pxPublicLabel = NULL;
    CK_OBJECT_HANDLE xPalPublic = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE xPalPrivate = CK_INVALID_HANDLE;
    char* xEnd = NULL;
    long lOptigaOid = 0;
	optiga_rsa_key_type_t rsa_key_type = 0;
	uint8_t key_usage;
	
	do
	{
		p_pkcs11_session_t session = get_session_pointer( xSession );
	    if( (CKM_EC_KEY_PAIR_GEN != pxMechanism->mechanism) && (CKM_RSA_PKCS_KEY_PAIR_GEN != pxMechanism->mechanism) )
	    {
	        xResult = CKR_MECHANISM_PARAM_INVALID;
			break;
	    }
		
		xResult = verify_private_key_template(xSession,
											  &pxPrivateLabel,
											  pxPrivateKeyTemplate,
											  ulPrivateKeyAttributeCount );

	    if( xResult != CKR_OK )
	    {
	    	break;
	    }
		
		xResult = verify_public_key_template(xSession,
											 &pxPublicLabel,
											 pxPublicKeyTemplate,
											 ulPublicKeyAttributeCount );

	    if( xResult != CKR_OK )
	    {
			break;
	    }

        lOptigaOid = strtol((char*)pxPrivateLabel->pValue, &xEnd, 16);

        if ( 0 != lOptigaOid)
        {
        	pal_os_lock_acquire(&optiga_mutex);

            /* For the public key, the OPTIGA library will return the standard 65 
            		  bytes of uncompressed curve points plus a 3-byte tag. The latter will 
                       be intentionally overwritten below. */
        	pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
			if ( pxMechanism->mechanism == CKM_EC_KEY_PAIR_GEN )
			{
				ucPublicKeyDerLength = session->ec_key_size;
				pucPublicKeyDer = malloc(session->ec_key_size);

		        if (pucPublicKeyDer == NULL)
		        {
		            xResult = CKR_HOST_MEMORY;
					break;
		        }
	        	xResult = optiga_crypt_ecc_generate_keypair(pkcs11_context.object_list.optiga_crypt_instance,
	        												session->ec_key_type,
															(uint8_t)OPTIGA_KEY_USAGE_SIGN,
															FALSE,
															&lOptigaOid,
															pucPublicKeyDer,
															&ucPublicKeyDerLength);
			}
			else if ( pxMechanism->mechanism == CKM_RSA_PKCS_KEY_PAIR_GEN )
			{
				ucPublicKeyDerLength = session->rsa_key_size;
		        pucPublicKeyDer = malloc(session->rsa_key_size);

		        if (pucPublicKeyDer == NULL)
		        {
		            xResult = CKR_HOST_MEMORY;
					break;
		        }

				if (( session->key_template_enabled & PKCS_ENCRYPT_ENABLE ) && 
					( session->key_template_enabled & PKCS_DECRYPT_ENABLE ))
				{
					key_usage = OPTIGA_KEY_USAGE_ENCRYPTION;
				}
				
				if (( session->key_template_enabled & PKCS_SIGN_ENABLE ) && 
					( session->key_template_enabled & PKCS_VERIFY_ENABLE ))
				{
					key_usage |= OPTIGA_KEY_USAGE_SIGN;
				}
				rsa_key_type = (session->rsa_key_size == pkcs11RSA_2048_MODULUS_BITS ? 
														 OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL: 
														 OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL);
				xResult = optiga_crypt_rsa_generate_keypair(pkcs11_context.object_list.optiga_crypt_instance,
                                                            rsa_key_type,
                                                            key_usage,
                                                            FALSE,
                                                            &lOptigaOid,
															pucPublicKeyDer,
															&ucPublicKeyDerLength);
			}
			else
			{
				xResult = CKR_MECHANISM_PARAM_INVALID;
				break;
			}

			if (OPTIGA_LIB_SUCCESS != xResult)
			{
				PKCS11_PRINT( ( "ERROR: Failed to generate a keypair \r\n" ) );
				xResult = CKR_FUNCTION_FAILED;
				break;
			}

			while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
			{
			
			}

			// Either by timout or because of success it should end up here
			if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
			{
                PKCS11_PRINT( ( "ERROR: Failed to generate a keypair \r\n" ) );
                xResult = CKR_FUNCTION_FAILED;
				break;
			}
			pal_os_lock_release(&optiga_mutex);
        }
        else
        {
            xResult = CKR_FUNCTION_FAILED;
			break;
        }

	    if( xResult == CKR_OK)
	    {
	      
	       xPalPublic = save_object(pxPublicLabel,
	                                 (unsigned char *)pucPublicKeyDer,
	                                 ucPublicKeyDerLength);   

	    }
	    else
	    {
	        xResult = CKR_GENERAL_ERROR;
			break;
	    }

        /* This is a trick to have a handle to store */
        xPalPrivate = save_object( pxPrivateLabel, NULL, 0);

	    if( ( xPalPublic != CK_INVALID_HANDLE ) )
	    {
	        xResult = add_object_to_list( xPalPrivate, pxPrivateKey, pxPrivateLabel->pValue, pxPrivateLabel->ulValueLen );
	        if( xResult == CKR_OK )
	        {
	            xResult = add_object_to_list( xPalPublic, pxPublicKey, pxPublicLabel->pValue, pxPublicLabel->ulValueLen );
	            if( xResult != CKR_OK )
	            {
	                destroy_object( *pxPrivateKey );
					break;
	            }
	        }
	        
	    }
		else
		{
	        xResult = CK_INVALID_HANDLE;
			break;			
		}
	} while(0);
    /* Clean up. */
    if( NULL != pucPublicKeyDer )
    {
        free( pucPublicKeyDer );
    }
    return xResult;
}

CK_RV check_valid_rsa_signature_scheme(CK_MECHANISM_TYPE mechanism_type)
 {
	CK_RV return_status = CKR_OK;

    switch(mechanism_type)
	{
	    case CKM_RSA_PKCS:
	        break;
	    case CKM_SHA256_RSA_PKCS:
	        break;
	    case CKM_SHA384_RSA_PKCS:
	        break;
	    case CKM_SHA512_RSA_PKCS:
	        break;
	    default:
			return_status = CKR_MECHANISM_INVALID;
    }
	return return_status;
}

CK_RV set_valid_rsa_signature_scheme(CK_MECHANISM_TYPE mechanism_type,
										   optiga_rsa_signature_scheme_t* rsa_signature_scheme)
 {
	CK_RV return_status = CKR_OK;

    switch(mechanism_type)
	{
	    case CKM_RSA_PKCS:
		{
			*rsa_signature_scheme =  OPTIGA_RSASSA_PKCS1_V15_SHA256;
		}
        break;
	    case CKM_SHA256_RSA_PKCS:
		{
			*rsa_signature_scheme =  OPTIGA_RSASSA_PKCS1_V15_SHA256;
		}
        break;
	    case CKM_SHA384_RSA_PKCS:
		{
			*rsa_signature_scheme =  OPTIGA_RSASSA_PKCS1_V15_SHA384;
		}
        break;
	    case CKM_SHA512_RSA_PKCS:
		{
			*rsa_signature_scheme =  OPTIGA_RSASSA_PKCS1_V15_SHA512;
		}
        break;
	    default:
			return_status = CKR_MECHANISM_INVALID;
    }
	return return_status;
}

/**
 * @brief Begin creating a digital signature.
 */
CK_DEFINE_FUNCTION( CK_RV, C_SignInit )( CK_SESSION_HANDLE xSession,
                                         CK_MECHANISM_PTR pxMechanism,
                                         CK_OBJECT_HANDLE xKey )
{

    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    CK_OBJECT_HANDLE xPalHandle;
    uint8_t * pxLabel = NULL;
    size_t xLabelLength = 0;
    long     lOptigaOid = 0;
    char*    xEnd = NULL;
	
    p_pkcs11_session_t session = get_session_pointer( xSession );
	do
	{
	
	    /*lint !e9072 It's OK to have different parameter name. */
    	p_pkcs11_session_t pxSession = get_session_pointer( xSession );
	    if( NULL == pxMechanism )
	    {
	        PKCS11_PRINT( ( "ERROR: Null signing mechanism provided. \r\n" ) );
	        xResult = CKR_ARGUMENTS_BAD;
			break;
	    }
		

	    /* Retrieve key value from storage. */

        find_object_in_list_by_handle( xKey,
                                     &xPalHandle,
                                     &pxLabel,
                                     &xLabelLength );

        if( xPalHandle != CK_INVALID_HANDLE )
        {
            lOptigaOid = strtol((char*)pxLabel, &xEnd, 16);
            if (0 != lOptigaOid)
            {
                pxSession->sign_key_oid = (uint16_t) lOptigaOid;
            }
            else
            {
                PKCS11_PRINT( ("ERROR: Unable to retrieve value of private key for signing %d. \r\n", xResult) );
				xResult = CKR_KEY_HANDLE_INVALID;
				break;
            }
			
        }
        else
        {
            xResult = CKR_KEY_HANDLE_INVALID;
			break;
        }

		

	    /* Check that the mechanism and key type are compatible, supported. */
	    if( (pxMechanism->mechanism != CKM_ECDSA) && (check_valid_rsa_signature_scheme(pxMechanism->mechanism)) )
        {
            PKCS11_PRINT( ("ERROR: Unsupported mechanism type %d \r\n", pxMechanism->mechanism) );
            xResult = CKR_MECHANISM_INVALID;
			break;
        }
        else
        {
            pxSession->sign_mechanism = pxMechanism->mechanism;
        }

	    session->sign_init_done = TRUE;
	}while(0);
    return xResult;
}

/**
 * @brief Performs a digital signature operation.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Sign )( CK_SESSION_HANDLE xSession,
                                     CK_BYTE_PTR pucData,
                                     CK_ULONG ulDataLen,
                                     CK_BYTE_PTR pucSignature,
                                     CK_ULONG_PTR pulSignatureLen )
{  

	/*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    p_pkcs11_session_t session = get_session_pointer( xSession );
    CK_ULONG xSignatureLength = 0;
    /* Signature Length + 3x2 bytes reserved for DER tags */
    uint8_t ecSignature[ pkcs11ECDSA_P521_SIGNATURE_LENGTH + 3 + 3 ];
    uint16_t ecSignatureLength = sizeof(ecSignature);
	optiga_rsa_signature_scheme_t rsa_signature_scheme = 0;

	do
	{
		if ( TRUE != session->sign_init_done)
		{
            xResult = CKR_OPERATION_NOT_INITIALIZED;
			break;			
		}

	    if( NULL == pulSignatureLen )
	    {
	        xResult = CKR_ARGUMENTS_BAD;
			break;
	    }
        /* Update the signature length. */
    	if (session->sign_mechanism == CKM_ECDSA)
    	{
    		if(session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_256)
    		{
				xSignatureLength = pkcs11ECDSA_P256_SIGNATURE_LENGTH;
			}
			else if(session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_384)
    		{
				xSignatureLength = pkcs11ECDSA_P384_SIGNATURE_LENGTH;
			}
			else if(session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_521)
    		{
				xSignatureLength = pkcs11ECDSA_P521_SIGNATURE_LENGTH;
			}
			else
			{
		        xResult = CKR_ARGUMENTS_BAD;
				break;				
			}
		}
		else if ( CKR_OK == check_valid_rsa_signature_scheme(session->sign_mechanism))
		{
			xSignatureLength = (session->rsa_key_size == pkcs11RSA_2048_MODULUS_BITS ?
													     pkcs11RSA_2048_SIGNATURE_LENGTH :
													     pkcs11RSA_1024_SIGNATURE_LENGTH);
		}
		else
		{
	        xResult = CKR_ARGUMENTS_BAD;
			break;
		}

        /* Check that the signature buffer is long enough. */
        if( *pulSignatureLen < xSignatureLength )
        {
            xResult = CKR_BUFFER_TOO_SMALL;
			break;
        }

        if (0 != session->sign_key_oid)
        {
        	pal_os_lock_acquire(&optiga_mutex);

            /*
                     * An example of a returned signature
                     * 0x000000: 02 20 38 0f 56 c8 90 53 18 9d 8f 58 b4 46 35 a0 . 8.V..S...X.F5.
                     * 0x000010: d7 07 63 ef 9f a2 30 64 93 e4 3d bf 7b db 57 a1 ..c...0d..=.{.W.
                     * 0x000020: b6 d7 02 20 4f 5e 3a db 6b 1a eb ac 66 9a 15 69 ... O^:.k...f..i
                     * 0x000030: 0d 7d 46 5b 44 72 40 06 a5 7b 06 84 0f d7 6e 0f .}F[Dr@..{....n.
                     * 0x000040: 4b 45 7f 50                                     KE.P
                     */
        	pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
			if (session->sign_mechanism == CKM_ECDSA)
			{
            	xResult = optiga_crypt_ecdsa_sign(pkcs11_context.object_list.optiga_crypt_instance,
            									  pucData,
												  ulDataLen,
												  session->sign_key_oid,
												  ecSignature,
												  &ecSignatureLength);
			}
			else if ( CKR_OK == set_valid_rsa_signature_scheme(session->sign_mechanism, &rsa_signature_scheme) )
			{
            	xResult = optiga_crypt_rsa_sign(pkcs11_context.object_list.optiga_crypt_instance,
												rsa_signature_scheme,
            									pucData,
												ulDataLen,
												session->sign_key_oid,
												pucSignature,
												(uint16_t *)pulSignatureLen,
												0x0000);
												
			}
			else
			{
				xResult = CKR_ARGUMENTS_BAD;
				break;
			}
			if (OPTIGA_LIB_SUCCESS != xResult)
			{
				xResult = CKR_FUNCTION_FAILED;
				break;
			}

			while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
			{

			}

			// Either by timout or because of success it should end up here
			if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
			{
				xResult = CKR_FUNCTION_FAILED;
				break;
			}

			pal_os_lock_release(&optiga_mutex);
        }
        if (session->sign_mechanism == CKM_ECDSA)
		{
	        /* Reformat from DER encoded to 64-byte R & S components */
	        asn1_to_ecdsa_rs(ecSignature, ecSignatureLength, pucSignature, xSignatureLength);
			*pulSignatureLen = xSignatureLength;
        }
        /* Complete the operation in the context. */
	    if( xResult != CKR_BUFFER_TOO_SMALL )
	    {
	        session->sign_mechanism = pkcs11NO_OPERATION;
	    }
	}while(0);
    return xResult;
}


CK_DEFINE_FUNCTION(CK_RV, C_SignUpdate)( CK_SESSION_HANDLE xSession, 
										  CK_BYTE_PTR part, 
										  CK_ULONG part_len) 
{
    return CKR_OK;
}

CK_DEFINE_FUNCTION(CK_RV, C_SignFinal)( CK_SESSION_HANDLE xSession, 
									    CK_BYTE_PTR signature, 
									    CK_ULONG_PTR signature_len) 
{
    return CKR_OK;
}


/**
 * @brief Begin a digital signature verification.
 */
CK_DEFINE_FUNCTION( CK_RV, C_VerifyInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pxMechanism,
                                           CK_OBJECT_HANDLE xKey )
{

    CK_RV xResult = CKR_OK;
    p_pkcs11_session_t session;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint8_t * pxLabel = NULL;
    size_t xLabelLength = 0;
    CK_LONG lOptigaOid = 0;
    char* xEnd = NULL;

    /*lint !e9072 It's OK to have different parameter name. */
    //( void ) ( xSession );
	do
	{
    	session = get_session_pointer( xSession );

	    if( NULL == pxMechanism )
	    {
	        xResult = CKR_ARGUMENTS_BAD;
			break;
	    }

    /* Retrieve key value from storage. */
        find_object_in_list_by_handle( xKey, &xPalHandle, &pxLabel, &xLabelLength );

        if( xPalHandle != CK_INVALID_HANDLE )
        {
            lOptigaOid = strtol((char*)pxLabel, &xEnd, 16);

            if (0 != lOptigaOid)
            {
                session->verify_key_oid = (uint16_t) lOptigaOid;
            }
            else
            {
                PKCS11_PRINT( ("ERROR: Unable to retrieve value of private key for signing %d. \r\n", xResult) );
				xResult = CKR_ARGUMENTS_BAD;
				break;
            }
        }
        else
        {
            xResult = CKR_KEY_HANDLE_INVALID;
			break;
        }
    
	    /* Check for a supported crypto algorithm. */
	    if( (pxMechanism->mechanism == CKM_ECDSA) || !(check_valid_rsa_signature_scheme(pxMechanism->mechanism)) )
	    {
	        session->verify_mechanism = pxMechanism->mechanism;
	    }
	    else
	    {
	        xResult = CKR_MECHANISM_INVALID;
			break;
	    }
		session->verify_init_done = TRUE;
	} while (0);
    return xResult;
}


/**
 * @brief Verifies a digital signature.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Verify )( CK_SESSION_HANDLE xSession,
                                       CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataLen,
                                       CK_BYTE_PTR pucSignature,
                                       CK_ULONG ulSignatureLen )
{

    CK_RV xResult = CKR_OK;
    p_pkcs11_session_t session;
    uint8_t temp[2048];
    uint16_t tempLen = 2048;
	public_key_from_host_t xPublicKeyDetails = {0};
	optiga_rsa_signature_scheme_t rsa_signature_scheme = 0;
	CK_ULONG xSignatureLength = 0;
     /* (R component ) + (S component ) + DER tags 3 bytes max each*/
    CK_BYTE pubASN1Signature[pkcs11ECDSA_P521_SIGNATURE_LENGTH + 0x03 + 0x03];
    CK_ULONG pubASN1SignatureLength = sizeof(pubASN1Signature);   
	do
	{
		session = get_session_pointer( xSession );
		if ( TRUE != session->verify_init_done)
		{
            xResult = CKR_OPERATION_NOT_INITIALIZED;
			break;			
		}
	    /* Check parameters. */
	    if( ( NULL == pucData ) ||
	        ( NULL == pucSignature ) )
	    {
	        xResult = CKR_ARGUMENTS_BAD;
			break;
	    }
	    /* Check that the signature and data are the expected length.
	     * These PKCS #11 mechanism expect data to be pre-hashed/formatted. */
        if( session->verify_mechanism == CKM_ECDSA )
        {
            if( ulDataLen != pkcs11SHA256_DIGEST_LENGTH )
            {
                xResult = CKR_DATA_LEN_RANGE;
				break;
            }

        }
		else if( CKR_OK == check_valid_rsa_signature_scheme(session->verify_mechanism) )
		{
			xResult = CKR_OK;
		}
        else
        {
            xResult = CKR_OPERATION_NOT_INITIALIZED;
			break;
        }
		
        /* Update the signature length. */
    	if (session->verify_mechanism == CKM_ECDSA)
    	{
    		if(session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_256)
    		{
				xSignatureLength = pkcs11ECDSA_P256_SIGNATURE_LENGTH;
			}
			else if(session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_384)
    		{
				xSignatureLength = pkcs11ECDSA_P384_SIGNATURE_LENGTH;
			}
			else if(session->ec_key_type == OPTIGA_ECC_CURVE_NIST_P_521)
    		{
				xSignatureLength = pkcs11ECDSA_P521_SIGNATURE_LENGTH;
			}
			else
			{
		        xResult = CKR_ARGUMENTS_BAD;
				break;				
			}
			/* Perform an ECDSA verification. */
	        if ( !ecdsa_rs_to_asn1_integers(&pucSignature[ 0 ], &pucSignature[ xSignatureLength/2 ], xSignatureLength/2,
	                                        pubASN1Signature, (size_t*)&pubASN1SignatureLength))
	        {
	            xResult = CKR_SIGNATURE_INVALID;
	            PKCS11_PRINT( ( "Failed to parse EC signature \r\n" ) );
	        }
		}
		else if ( CKR_OK == check_valid_rsa_signature_scheme(session->verify_mechanism))
		{
			xSignatureLength = (session->rsa_key_size == pkcs11RSA_2048_MODULUS_BITS ?
													     pkcs11RSA_2048_SIGNATURE_LENGTH :
													     pkcs11RSA_1024_SIGNATURE_LENGTH);
		}
		else
		{
	        xResult = CKR_ARGUMENTS_BAD;
			break;
		}

        /* Check that the signature buffer is long enough. */
        if( ulSignatureLen < xSignatureLength )
        {
            xResult = CKR_SIGNATURE_LEN_RANGE;
			break;
        }

        pal_os_lock_acquire(&optiga_mutex);

        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        xResult = optiga_util_read_data(pkcs11_context.object_list.optiga_util_instance, 
										session->verify_key_oid, 
										0, 
										temp, 
										&tempLen);

		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			PKCS11_PRINT( "Failed to extract the Public Key from the SE\r\n" );
			xResult = CKR_SIGNATURE_INVALID;
			break;
		}
		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		
		}

		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			PKCS11_PRINT( "Failed to extract the Public Key from the SE\r\n" );
			xResult = CKR_SIGNATURE_INVALID;
			break;
		}

		pal_os_lock_release(&optiga_mutex);
		
        pal_os_lock_acquire(&optiga_mutex);
		
        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
		if (session->verify_mechanism == CKM_ECDSA)
		{
            xPublicKeyDetails.public_key = temp;
			xPublicKeyDetails.length = tempLen;
			xPublicKeyDetails.key_type = session->ec_key_type;

            xResult = optiga_crypt_ecdsa_verify (pkcs11_context.object_list.optiga_crypt_instance,
            									 pucData, 
            									 ulDataLen,
												 pubASN1Signature,
												 pubASN1SignatureLength,
												 OPTIGA_CRYPT_HOST_DATA, 
												 &xPublicKeyDetails );
		}
		else if ( CKR_OK == set_valid_rsa_signature_scheme(session->verify_mechanism, &rsa_signature_scheme)) 
		{
            xPublicKeyDetails.public_key = temp;
			xPublicKeyDetails.length = tempLen;
			xPublicKeyDetails.key_type = (session->rsa_key_size == pkcs11RSA_2048_MODULUS_BITS ?
																   OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL :
																   OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL);
            		
			xResult = optiga_crypt_rsa_verify (pkcs11_context.object_list.optiga_crypt_instance,
											   rsa_signature_scheme,
											   pucData,
											   ulDataLen,
											   pucSignature,
											   ulSignatureLen,
											   OPTIGA_CRYPT_HOST_DATA,
											   &xPublicKeyDetails,
											   0x0000);

		}
		else
		{
			xResult = CKR_ARGUMENTS_BAD;
		}
		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			PKCS11_PRINT( ("Failed to verify the signature\r\n") );
			xResult = CKR_SIGNATURE_INVALID;
			break;
		}
		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		
		}

		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
            PKCS11_PRINT( "Failed to verify the signature\r\n" );
			xResult = CKR_SIGNATURE_INVALID;
			break;
		}
		pal_os_lock_release(&optiga_mutex);
    
	}while (0);
    /* Return the signature verification result. */
    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_VerifyUpdate)( CK_SESSION_HANDLE xSession, 
										   CK_BYTE_PTR part, 
										   CK_ULONG part_len) 
{
    return CKR_OK;
}

CK_DEFINE_FUNCTION( CK_RV, C_VerifyFinal)( CK_SESSION_HANDLE xSession,
										  CK_BYTE_PTR signature, 
										  CK_ULONG signature_len) 
{
    return CKR_OK;
}


/**
 * @brief Generate cryptographically random bytes.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateRandom )( CK_SESSION_HANDLE xSession,
                                               CK_BYTE_PTR pucRandomData,
                                               CK_ULONG ulRandomLen )
{

    CK_RV xResult = CKR_OK;
    // this is to truncate random numbers to the required length, as OPTIGA(TM) Trust can generate
    // values starting from 8 bytes
    CK_BYTE xRandomBuf4SmallLengths[8];
    CK_ULONG xBuferSwitcherLength = ulRandomLen;
    CK_BYTE_PTR pxBufferSwitcher = pucRandomData;
	
	do
	{
    	xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
	    if( xResult != CKR_OK )
	    {
			break;
		}
        if( ( NULL == pucRandomData ) || ( ulRandomLen == 0 ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
			break;
        }
        if (xBuferSwitcherLength < 8) 
		{
            pxBufferSwitcher = xRandomBuf4SmallLengths;
            xBuferSwitcherLength = 8;
        }

        pal_os_lock_acquire(&optiga_mutex);

        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        xResult = optiga_crypt_random(pkcs11_context.object_list.optiga_crypt_instance,
        							  OPTIGA_RNG_TYPE_TRNG,
									  pxBufferSwitcher,
									  xBuferSwitcherLength);

		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			PKCS11_PRINT( ( "ERROR: Failed to generate a random value \r\n" ) );
			xResult = CKR_SIGNATURE_INVALID;
			break;
		}
		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		
		}

		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
            PKCS11_PRINT( ( "ERROR: Failed to generate a random value \r\n" ) );
            xResult = CKR_FUNCTION_FAILED;
			break;
		}
		pal_os_lock_release(&optiga_mutex);

		if (pxBufferSwitcher == xRandomBuf4SmallLengths)
        {
            memcpy(pucRandomData, xRandomBuf4SmallLengths, ulRandomLen);
        }
	        
	}while(0);
    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_EncryptInit )( CK_SESSION_HANDLE xSession,
                       					    CK_MECHANISM_PTR pxMechanism,
                                            CK_OBJECT_HANDLE xKey ) 
{

    CK_RV xResult = CKR_OK;
    p_pkcs11_session_t session;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint8_t * pxLabel = NULL;
    size_t xLabelLength = 0;
    CK_LONG lOptigaOid = 0;
    char* xEnd = NULL;
	
    session = get_session_pointer( xSession );
	

	do
	{
		if (!( session->key_template_enabled & PKCS_ENCRYPT_ENABLE ))
		{
	        xResult = CKR_KEY_FUNCTION_NOT_PERMITTED;
			break;
		}
	    if( NULL == pxMechanism )
	    {
	        xResult = CKR_ARGUMENTS_BAD;
			break;
	    }

	    /* Retrieve key value from storage. */
        find_object_in_list_by_handle( xKey, &xPalHandle, &pxLabel, &xLabelLength );

        if( xPalHandle != CK_INVALID_HANDLE )
        {
            lOptigaOid = strtol((char*)pxLabel, &xEnd, 16);

            if (0 != lOptigaOid)
            {
                session->encryption_key_oid = (uint16_t) lOptigaOid;
            }
            else
            {
                PKCS11_PRINT( ("ERROR: Unable to retrieve value of public key for encryption %d. \r\n", xResult) );
				xResult = CKR_ARGUMENTS_BAD;
				break;
            }
        }
        else
        {
            xResult = CKR_KEY_HANDLE_INVALID;
        }
		session->encrypt_init_done = TRUE;
	    
	} while (0);
    return xResult;

}

CK_DEFINE_FUNCTION(CK_RV, C_Encrypt) ( CK_SESSION_HANDLE xSession,
					                   CK_BYTE_PTR pxData,
					                   CK_ULONG ulDataLen,
					                   CK_BYTE_PTR pxEncryptedData,
					                   CK_ULONG_PTR pxulEncryptedDataLen ) 
{

    CK_RV xResult = CKR_OK;
    p_pkcs11_session_t session;
    uint8_t temp[2048];
    uint16_t tempLen = sizeof(temp);	
	uint8_t key_type;
	public_key_from_host_t xPublicKeyDetails = {0};
	
    session = get_session_pointer( xSession );
	

	do
	{	
		if ( FALSE == session->encrypt_init_done )
		{
			xResult = CKR_OPERATION_NOT_INITIALIZED;
			break;
		}	
		key_type = (uint8_t)(session->rsa_key_size == pkcs11RSA_2048_MODULUS_BITS ?
													  OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL :
													  OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL);
													  
		if (((key_type == OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL ) && 
			(ulDataLen > ((pkcs11RSA_1024_MODULUS_BITS / 8) - 11))) ||
			((key_type == OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL ) && 
			(ulDataLen > ((pkcs11RSA_2048_MODULUS_BITS / 8) - 11))))
		{
			xResult = CKR_ARGUMENTS_BAD;
			break;
		}
		if  (((key_type == OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL ) && 			
			 (*pxulEncryptedDataLen < (pkcs11RSA_1024_MODULUS_BITS / 8))) ||			
			 ((key_type == OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL ) && 			
			 (*pxulEncryptedDataLen < (pkcs11RSA_2048_MODULUS_BITS / 8))))		
	    {			
	    	xResult = CKR_BUFFER_TOO_SMALL ;
			break;
		}

		pal_os_lock_acquire(&optiga_mutex);
		
		pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
		xResult = optiga_util_read_data(pkcs11_context.object_list.optiga_util_instance, 
										session->encryption_key_oid,
										0, temp, &tempLen);
		
		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			PKCS11_PRINT( "Failed to extract the Public Key from the Encryption\r\n" );
			xResult = CKR_ENCRYPTED_DATA_INVALID;
			break;
		}
		
		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		
		}
	
		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			PKCS11_PRINT( "Failed to extract the Public Key from the Encryption\r\n" );
			xResult = CKR_ENCRYPTED_DATA_INVALID;
			break;
		}
		pal_os_lock_release(&optiga_mutex);
		
		pal_os_lock_acquire(&optiga_mutex);
		xPublicKeyDetails.public_key = temp;
		xPublicKeyDetails.length = tempLen;
		xPublicKeyDetails.key_type = key_type;
		
		pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
		xResult = optiga_crypt_rsa_encrypt_message(pkcs11_context.object_list.optiga_crypt_instance,
												   OPTIGA_RSAES_PKCS1_V15,
												   pxData,
												   ulDataLen,
												   NULL,
												   0,
												   OPTIGA_CRYPT_HOST_DATA,
												   &xPublicKeyDetails,
												   pxEncryptedData,
												   (uint16_t *)pxulEncryptedDataLen);
		
		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			PKCS11_PRINT( ( "ERROR: Failed to encrypt value \r\n" ) );
			xResult = CKR_ENCRYPTED_DATA_INVALID;
			break;
		}
		
		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		
		}
	
		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			PKCS11_PRINT( ( "ERROR: Failed to encrypt value \r\n" ) );
			xResult = CKR_FUNCTION_FAILED;
			break;
		}
		pal_os_lock_release(&optiga_mutex);

	} while (0);
	
    return xResult;

}


CK_DEFINE_FUNCTION(CK_RV, C_EncryptUpdate) ( CK_SESSION_HANDLE xSession, 
						                     CK_BYTE_PTR part, 
						                     CK_ULONG part_len, 
						                     CK_BYTE_PTR encrypted_part, 
						                     CK_ULONG_PTR encrypted_part_len) 
{
   return CKR_OK;
}

CK_DEFINE_FUNCTION(CK_RV, C_EncryptFinal) ( CK_SESSION_HANDLE xSession, 
						                    CK_BYTE_PTR last_encrypted_part, 
						                    CK_ULONG_PTR last_encrypted_part_len) 
{
    return CKR_OK;
}

CK_DEFINE_FUNCTION(CK_RV, C_DecryptInit) ( CK_SESSION_HANDLE xSession, 
					                       CK_MECHANISM *pxMechanism, 
					                       CK_OBJECT_HANDLE xKey) 
{

    CK_RV xResult = CKR_OK;
    p_pkcs11_session_t session;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint8_t * pxLabel = NULL;
    size_t xLabelLength = 0;
    CK_LONG lOptigaOid = 0;
    char* xEnd = NULL;
	
    session = get_session_pointer( xSession );
	

	do
	{
		if (!( session->key_template_enabled & PKCS_DECRYPT_ENABLE ))
		{
	        xResult = CKR_KEY_FUNCTION_NOT_PERMITTED;
			break;
		}
	    if( NULL == pxMechanism )
	    {
	        xResult = CKR_ARGUMENTS_BAD;
			break;
	    }

	    /* Retrieve key value from storage. */
        find_object_in_list_by_handle( xKey, &xPalHandle, &pxLabel, &xLabelLength );

        if( xPalHandle != CK_INVALID_HANDLE )
        {
            lOptigaOid = strtol((char*)pxLabel, &xEnd, 16);

            if (0 != lOptigaOid)
            {
                session->decryption_key_oid = (uint16_t) lOptigaOid;
            }
            else
            {
                PKCS11_PRINT( ("ERROR: Unable to retrieve value of private key for decryption %d. \r\n", xResult) );
				xResult = CKR_ARGUMENTS_BAD;
				break;
            }
        }
        else
        {
            xResult = CKR_KEY_HANDLE_INVALID;
        }
		session->decrypt_init_done = TRUE;
	    
	} while (0);
    return xResult;    
}

CK_DEFINE_FUNCTION(CK_RV, C_Decrypt) ( CK_SESSION_HANDLE xSession, 
									   CK_BYTE_PTR encrypted_data, 
					                   CK_ULONG encrypted_data_len, 
					                   CK_BYTE_PTR data, 
					                   CK_ULONG_PTR data_len) 
{

    CK_RV xResult = CKR_OK;
    p_pkcs11_session_t session;	
	uint8_t key_type;
	
    session = get_session_pointer( xSession );
	

	do
	{	
		if ( FALSE == session->decrypt_init_done )
		{
			xResult = CKR_OPERATION_NOT_INITIALIZED;
			break;
		}
		
		key_type = (uint8_t)(session->rsa_key_size == pkcs11RSA_2048_MODULUS_BITS ?
													  OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL :
													  OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL);
													  
		if  (((key_type == OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL ) && 
			(encrypted_data_len != (pkcs11RSA_1024_MODULUS_BITS / 8))) ||
			((key_type == OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL ) && 
			(encrypted_data_len != (pkcs11RSA_2048_MODULUS_BITS / 8))))
		{
			xResult = CKR_ENCRYPTED_DATA_LEN_RANGE ;
			break;
		}

		pal_os_lock_acquire(&optiga_mutex);;
		
		pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
		xResult = optiga_crypt_rsa_decrypt_and_export(pkcs11_context.object_list.optiga_crypt_instance,
                                                      OPTIGA_RSAES_PKCS1_V15,
                                                      encrypted_data,
                                                      encrypted_data_len,
                                                      NULL,
                                                      0,
                                                      session->decryption_key_oid,
                                                      data,
                                                      (uint16_t *)data_len);
		
		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			PKCS11_PRINT( ( "ERROR: Failed to decrypt value \r\n" ) );
			xResult = CKR_ENCRYPTED_DATA_INVALID;
			break;
		}
		
		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		
		}
	
		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			PKCS11_PRINT( ( "ERROR: Failed to decrypt value \r\n" ) );
			xResult = CKR_FUNCTION_FAILED;
			break;
		}
		pal_os_lock_release(&optiga_mutex);

	} while (0);
	
    return xResult;    
}

CK_DEFINE_FUNCTION(CK_RV, C_DecryptUpdate) ( CK_SESSION_HANDLE xSession, 
							                 CK_BYTE_PTR encrypted_part, 
							                 CK_ULONG encrypted_part_len, 
							                 CK_BYTE_PTR part, 
							                 CK_ULONG_PTR part_len) 
{
    return CKR_OK;
}

CK_DEFINE_FUNCTION(CK_RV, C_DecryptFinal) ( CK_SESSION_HANDLE xSession, 
						                    CK_BYTE_PTR last_part, 
						                    CK_ULONG_PTR last_part_len) 
{
    return CKR_OK;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pMechanism )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
	p_pkcs11_session_t session;
	
	do
	{
	    session = get_session_pointer( xSession );

	    if( session == NULL )
	    {
	        xResult = CKR_SESSION_HANDLE_INVALID;
			break;
	    }

	    if( pMechanism->mechanism != CKM_SHA256 )
	    {
	        xResult = CKR_MECHANISM_INVALID;
			break;
	    }

        session->sha256_ctx.hash_ctx.context_buffer = session->sha256_ctx.hash_ctx_buff;
        session->sha256_ctx.hash_ctx.context_buffer_length = sizeof(session->sha256_ctx.hash_ctx_buff);
        session->sha256_ctx.hash_ctx.hash_algo = OPTIGA_HASH_TYPE_SHA_256;

        pal_os_lock_acquire(&optiga_mutex);

        //Hash start
        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        xResult = optiga_crypt_hash_start( pkcs11_context.object_list.optiga_crypt_instance, 
										   &session->sha256_ctx.hash_ctx);

		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			xResult = CKR_FUNCTION_FAILED;
			break;
		}

		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		}

		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
			xResult = CKR_FUNCTION_FAILED;
			break;
		}
		session->operation_in_progress = pMechanism->mechanism;

		pal_os_lock_release(&optiga_mutex);
	    
	}while(0);
    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestUpdate )( CK_SESSION_HANDLE xSession,
                                             CK_BYTE_PTR pPart,
                                             CK_ULONG ulPartLen )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    p_pkcs11_session_t session;
    hash_data_from_host_t hash_data_host;
	do
	{
		session = get_session_pointer( xSession );
	    if( session == NULL )
	    {
	        xResult = CKR_SESSION_HANDLE_INVALID;
			break;
	    }
	    else if( session->operation_in_progress != CKM_SHA256 )
	    {
	        xResult = CKR_OPERATION_NOT_INITIALIZED;
			break;
	    }

        hash_data_host.buffer = pPart;
        hash_data_host.length = ulPartLen;

        pal_os_lock_acquire(&optiga_mutex);

        pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        xResult = optiga_crypt_hash_update(pkcs11_context.object_list.optiga_crypt_instance,
        								   &session->sha256_ctx.hash_ctx,
										   OPTIGA_CRYPT_HOST_DATA,
										   &hash_data_host);

		if (OPTIGA_LIB_SUCCESS != xResult)
		{
			xResult = CKR_FUNCTION_FAILED;
			break;
		}

		while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
		{
		}

		// Either by timout or because of success it should end up here
		if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
		{
            xResult = CKR_FUNCTION_FAILED;
            session->operation_in_progress = pkcs11NO_OPERATION;
			break;
		}

		pal_os_lock_release(&optiga_mutex);
	}while(0);
    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestFinal )( CK_SESSION_HANDLE xSession,
                                            CK_BYTE_PTR pDigest,
                                            CK_ULONG_PTR pulDigestLen )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED(xSession);
    p_pkcs11_session_t session;

	do
	{
		session = get_session_pointer( xSession );
	    if( session == NULL )
	    {
	        xResult = CKR_SESSION_HANDLE_INVALID;
			break;
	    }
	    else if( session->operation_in_progress != CKM_SHA256 )
	    {
	        xResult = CKR_OPERATION_NOT_INITIALIZED;
	        session->operation_in_progress = pkcs11NO_OPERATION;
			break;
	    }


        if( pDigest == NULL )
        {
            /* Supply the required buffer size. */
            *pulDigestLen = pkcs11SHA256_DIGEST_LENGTH;
        }
        else
        {
            if( *pulDigestLen < pkcs11SHA256_DIGEST_LENGTH )
            {
                xResult = CKR_BUFFER_TOO_SMALL;
				break;
            }
        	pal_os_lock_acquire(&optiga_mutex);

            // hash finalize
        	pkcs11_context.object_list.optiga_lib_status = OPTIGA_LIB_BUSY;
        	xResult = optiga_crypt_hash_finalize( pkcs11_context.object_list.optiga_crypt_instance,
                                      	  	  	  &session->sha256_ctx.hash_ctx,
												  pDigest);

			if (OPTIGA_LIB_SUCCESS != xResult)
			{
				xResult = CKR_FUNCTION_FAILED;
				break;
			}

			while (OPTIGA_LIB_BUSY == pkcs11_context.object_list.optiga_lib_status)
			{
			}

			// Either by timout or because of success it should end up here
			if (OPTIGA_LIB_SUCCESS != pkcs11_context.object_list.optiga_lib_status)
			{
				xResult = CKR_FUNCTION_FAILED;
				break;
			}

			pal_os_lock_release(&optiga_mutex);

            session->operation_in_progress = pkcs11NO_OPERATION;
            
        }
	}while(0);
    return xResult;
}
