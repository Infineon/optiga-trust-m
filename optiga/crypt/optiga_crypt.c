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
* \file optiga_crypt.c
*
* \brief   This file implements the OPTIGA Crypt module.
*
* \ingroup  grOptigaCrypt
*
* @{
*/


#include "optiga/optiga_crypt.h"
#include "optiga/common/optiga_lib_common_internal.h"
#include "optiga/pal/pal_memory_mgmt.h"

/// ECDSA FIPS 186-3 without hash
#define OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH                  (0x11)
/// Elliptic Curve Diffie-Hellman key agreement algorithm
#define OPTIGA_CRYPT_ECDH_KEY_AGREEMENT_ALGORITHM                   (0x01)
/// Minimum length of random data
#define OPTIGA_CRYPT_MINIMUM_RANDOM_DATA_LENGTH                     (0x08)
/// Param type for optiga pre master secret
#define OPTIGA_CRYTP_RANDOM_PARAM_PRE_MASTER_SECRET                 (0x04)
/// Minimum optional data length
#define OPTIGA_CRYTP_MINIMUM_OPTIONAL_DATA_LENGTH                   (0x28)

_STATIC_H void optiga_crypt_generic_event_handler(void * p_ctx, 
                                                  optiga_lib_status_t event)
{
    optiga_crypt_t * me = (optiga_crypt_t *)p_ctx;

    me->handler(me->caller_context, event);
    me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
}

_STATIC_H void optiga_crypt_reset_protection_level(optiga_crypt_t * me)
{
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
    if (NULL != me)
#endif
    {
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL);
    }
}

extern void optiga_cmd_set_shielded_connection_option(optiga_cmd_t * me,
                                                      uint8_t value,
                                                      uint8_t shielded_connection_option);

#if defined (OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED) || defined (OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED)
_STATIC_H optiga_lib_status_t optiga_crypt_generate_keypair(optiga_crypt_t * me,
                                                            uint8_t cmd_param,
                                                            uint8_t key_usage,
                                                            bool_t export_private_key,
                                                            void * p_private_key,
                                                            uint8_t * p_public_key,
                                                            uint16_t * p_public_key_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_gen_keypair_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == p_public_key) || (NULL == p_private_key) || (NULL == p_public_key_length))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_gen_keypair_params_t *)me->params;

        p_params->key_usage = key_usage;
        p_params->export_private_key = export_private_key;
        p_params->public_key_length = p_public_key_length;
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);

        if (export_private_key)
        {
            p_params->private_key = (uint8_t *)p_private_key;
        }
        else
        {
            p_params->private_key_oid = (optiga_key_id_t)(* ((uint16_t* )p_private_key));
        }

        p_params->public_key = p_public_key;
        p_params->public_key_length = p_public_key_length;

        return_value = optiga_cmd_gen_keypair(me->my_cmd,
                                              (uint8_t )cmd_param,
                                              (optiga_gen_keypair_params_t *)p_params);
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif // (OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED) || (OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED)

#if defined (OPTIGA_CRYPT_ECDSA_SIGN_ENABLED) || defined (OPTIGA_CRYPT_RSA_SIGN_ENABLED)
//lint --e{715} suppress "The salt_length argument is kept for future use"
_STATIC_H optiga_lib_status_t optiga_crypt_sign(optiga_crypt_t * me,
                                                uint8_t signature_scheme,
                                                const uint8_t * p_digest,
                                                uint8_t digest_length,
                                                optiga_key_id_t private_key,
                                                uint8_t * p_signature,
                                                uint16_t * p_signature_length,
                                                uint16_t salt_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_calc_sign_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == p_digest) ||
            (NULL == p_signature) || (NULL == p_signature_length))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;

        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_calc_sign_params_t *)me->params;

        p_params->p_digest = p_digest;
        p_params->digest_length = digest_length;
        p_params->private_key_oid = private_key;
        p_params->p_signature = p_signature;
        p_params->p_signature_length = p_signature_length;
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);

        return_value = optiga_cmd_calc_sign(me->my_cmd,
                                            signature_scheme,
                                            (optiga_calc_sign_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif // (OPTIGA_CRYPT_ECDSA_SIGN_ENABLED) || (OPTIGA_CRYPT_RSA_SIGN_ENABLED)

#if defined (OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED) || defined (OPTIGA_CRYPT_RSA_VERIFY_ENABLED)
//lint --e{715} suppress "The salt_length argument is kept for future use"
_STATIC_H optiga_lib_status_t optiga_crypt_verify(optiga_crypt_t * me,
                                                  uint8_t cmd_param,
                                                  const uint8_t * p_digest,
                                                  uint8_t digest_length,
                                                  const uint8_t * p_signature,
                                                  uint16_t signature_length,
                                                  uint8_t public_key_source_type,
                                                  const void * p_public_key,
                                                  uint16_t salt_length)
{
    optiga_verify_sign_params_t * p_params;
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == p_digest) || (NULL == p_signature) || (NULL == p_public_key))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_verify_sign_params_t *)me->params;

        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);
        p_params->p_digest = p_digest;
        p_params->digest_length = digest_length;
        p_params->p_signature = p_signature;
        p_params->signature_length = signature_length;
        p_params->public_key_source_type = public_key_source_type;

        if (OPTIGA_CRYPT_OID_DATA == public_key_source_type)
        {
            p_params->certificate_oid = *((uint16_t *)p_public_key);
        }
        else
        {
            p_params->public_key = (public_key_from_host_t *)p_public_key;
        }

        return_value = optiga_cmd_verify_sign(me->my_cmd,
                                             (uint8_t)cmd_param,
                                             (optiga_verify_sign_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //(OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED) || (OPTIGA_CRYPT_RSA_VERIFY_ENABLED)

#if defined (OPTIGA_CRYPT_RSA_DECRYPT_ENABLED) || defined (OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED)
//lint --e{715} suppress "label and label_length is RFU hence not used"
_STATIC_H optiga_lib_status_t optiga_crypt_rsa_enc_dec(optiga_crypt_t * me,
                                                       optiga_rsa_encryption_scheme_t encryption_scheme,
                                                       const uint8_t * p_message,
                                                       uint16_t message_length,
                                                       const uint8_t * p_label,
                                                       uint16_t label_length,
                                                       uint8_t public_key_source_type,
                                                       const void * p_key,
                                                       uint8_t * p_processed_message,
                                                       uint16_t * p_processed_message_length,
                                                       bool_t is_enc)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_encrypt_asym_params_t * p_params;

    do
    {
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }
        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;;

        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_encrypt_asym_params_t *)me->params;

        p_params->message = p_message;
        p_params->message_length = message_length;
        p_params->processed_message = p_processed_message;
        p_params->processed_message_length = p_processed_message_length;

        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);


        if (is_enc)
        {
            p_params->key = p_key;
            p_params->public_key_source_type = public_key_source_type;
#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED            
            return_value = optiga_cmd_encrypt_asym(me->my_cmd, (uint8_t)encryption_scheme, 
                                                   (optiga_encrypt_asym_params_t *)p_params);
#endif            
        }
        else
        {
            p_params->private_key_id = (optiga_key_id_t)*(optiga_key_id_t *)p_key;
            p_params->key = &p_params->private_key_id;
            p_params->public_key_source_type = 0;
#ifdef OPTIGA_CRYPT_RSA_DECRYPT_ENABLED            
            return_value = optiga_cmd_decrypt_asym(me->my_cmd, (uint8_t)encryption_scheme,
                                                   (optiga_encrypt_asym_params_t *)p_params);
#endif
        }
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //(OPTIGA_CRYPT_RSA_DECRYPT_ENABLED) || (OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED)
#if defined (OPTIGA_CRYPT_RANDOM_ENABLED) || defined (OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED)
_STATIC_H optiga_lib_status_t optiga_crypt_get_random(optiga_crypt_t * me,
                                                      uint8_t cmd_param,
                                                      uint8_t * p_random_data,
                                                      uint16_t random_data_length,
                                                      const uint8_t * p_optional_data,
                                                      uint16_t optional_data_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_get_random_params_t * p_params;

    do
    {
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_get_random_params_t *)me->params;

        p_params->optional_data = p_optional_data;
        p_params->optional_data_length = optional_data_length;
        p_params->random_data = p_random_data;
        p_params->random_data_length = random_data_length;
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);

        return_value = optiga_cmd_get_random(me->my_cmd,
                                             (uint8_t)cmd_param,
                                              (optiga_get_random_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //(OPTIGA_CRYPT_RANDOM_ENABLED) || (OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED)


#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
void optiga_crypt_set_comms_params(optiga_crypt_t * me,
                                   uint8_t parameter_type,
                                   uint8_t value)
{
    switch (parameter_type)
    {
        case OPTIGA_COMMS_PROTECTION_LEVEL:
        {
            me->protection_level = value;
            break;
        }
        case OPTIGA_COMMS_PROTOCOL_VERSION:
        {
            me->protocol_version = value;
            break;
        }
        default:
        {
            break;
        }
    }
}
#endif

optiga_crypt_t * optiga_crypt_create(uint8_t optiga_instance_id,
                                     callback_handler_t handler,
                                     void * caller_context)
{
    optiga_crypt_t * me = NULL;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if (NULL == handler)
        {
            break;
        }
#endif
        me = (optiga_crypt_t *)pal_os_calloc(1, sizeof(optiga_crypt_t));
        if (NULL == me)
        {
            break;
        }

        me->handler = handler;
        me->caller_context = caller_context;
        me->instance_state = OPTIGA_LIB_SUCCESS;
        me->my_cmd = optiga_cmd_create(optiga_instance_id,
                                       optiga_crypt_generic_event_handler,
                                       me);
        if (NULL == me->my_cmd)
        {
            pal_os_free(me);
            me = NULL;
        }

    } while (FALSE);

    return (me);
}

optiga_lib_status_t optiga_crypt_destroy(optiga_crypt_t * me)
{
    optiga_lib_status_t return_value;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if (NULL == me)
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }
        return_value = optiga_cmd_destroy(me->my_cmd);
        pal_os_free(me);

    } while (FALSE);
    return (return_value);
}

#ifdef OPTIGA_CRYPT_RANDOM_ENABLED
optiga_lib_status_t optiga_crypt_random(optiga_crypt_t * me,
                                        optiga_rng_type_t rng_type,
                                        uint8_t * random_data,
                                        uint16_t random_data_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == random_data))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        return_value = optiga_crypt_get_random(me,
                                               (uint8_t)rng_type,
                                               random_data,
                                               random_data_length,
                                               NULL,
                                               0x00);
    } while (FALSE);

    return (return_value);
}
#endif //OPTIGA_CRYPT_RANDOM_ENABLED

#ifdef OPTIGA_CRYPT_HASH_ENABLED
optiga_lib_status_t optiga_crypt_hash_start(optiga_crypt_t * me,
                                            optiga_hash_context_t * hash_ctx)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_calc_hash_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == hash_ctx))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_calc_hash_params_t *)me->params;

        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);
        p_params->hash_sequence = OPTIGA_CRYPT_HASH_START;
        p_params->p_hash_context = hash_ctx;
        p_params->apparent_context_size = 0;
        p_params->p_hash_data = NULL;
        p_params->p_hash_oid = NULL;

        return_value = optiga_cmd_calc_hash(me->my_cmd,
                                            hash_ctx->hash_algo,
                                            (optiga_calc_hash_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}

optiga_lib_status_t optiga_crypt_hash_update(optiga_crypt_t * me,
                                             optiga_hash_context_t * hash_ctx,
                                             uint8_t source_of_data_to_hash,
                                             const void * data_to_hash)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_calc_hash_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == hash_ctx) || (NULL == data_to_hash))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_calc_hash_params_t *)me->params;

        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);
        p_params->source_of_data_to_hash = source_of_data_to_hash;
        p_params->hash_sequence = OPTIGA_CRYPT_HASH_CONTINUE;
        p_params->p_hash_context = hash_ctx;
        p_params->apparent_context_size = p_params->p_hash_context->context_buffer_length;
        p_params->p_hash_data = NULL;
        p_params->p_hash_oid = NULL;

        if (source_of_data_to_hash)
        {
            p_params->p_hash_data = (hash_data_from_host_t *)data_to_hash;
        }
        else
        {
            p_params->hash_sequence |= OPTIGA_CRYPT_HASH_FOR_OID;
            p_params->p_hash_oid = (hash_data_in_optiga_t *)data_to_hash;
        }

        return_value = optiga_cmd_calc_hash(me->my_cmd,
                                            hash_ctx->hash_algo,
                                            (optiga_calc_hash_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}

optiga_lib_status_t optiga_crypt_hash_finalize(optiga_crypt_t * me,
                                               optiga_hash_context_t * hash_ctx,
                                               uint8_t * hash_output)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_calc_hash_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == hash_ctx) || (NULL == hash_output))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_calc_hash_params_t *)me->params;

        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);
        p_params->p_out_digest = hash_output;
        p_params->hash_sequence = OPTIGA_CRYPT_HASH_FINAL;
        p_params->p_hash_context = hash_ctx;
        p_params->apparent_context_size = p_params->p_hash_context->context_buffer_length;
        p_params->p_hash_data = NULL;
        p_params->p_hash_oid = NULL;

        return_value = optiga_cmd_calc_hash(me->my_cmd,
                                            hash_ctx->hash_algo,
                                            (optiga_calc_hash_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //OPTIGA_CRYPT_HASH_ENABLED

#ifdef OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED
optiga_lib_status_t optiga_crypt_ecc_generate_keypair(optiga_crypt_t * me,
                                                      optiga_ecc_curve_t curve_id,
                                                      uint8_t key_usage,
                                                      bool_t export_private_key,
                                                      void * private_key,
                                                      uint8_t * public_key,
                                                      uint16_t * public_key_length)
{

    return (optiga_crypt_generate_keypair(me,
                                         (uint8_t)curve_id,
                                         key_usage,
                                         export_private_key,
                                         private_key,
                                         public_key,
                                         public_key_length));
}
#endif //OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_SIGN_ENABLED
optiga_lib_status_t optiga_crypt_ecdsa_sign(optiga_crypt_t * me,
                                            const uint8_t * digest,
                                            uint8_t digest_length,
                                            optiga_key_id_t private_key,
                                            uint8_t * signature,
                                            uint16_t * signature_length)
{
    return (optiga_crypt_sign(me,
                              OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH,
                              digest,
                              digest_length,
                              private_key,
                              signature,
                              signature_length,
                              0x0000));
}
#endif //OPTIGA_CRYPT_ECDSA_SIGN_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
optiga_lib_status_t optiga_crypt_ecdsa_verify(optiga_crypt_t * me,
                                              const uint8_t * digest,
                                              uint8_t digest_length,
                                              const uint8_t * signature,
                                              uint16_t signature_length,
                                              uint8_t public_key_source_type,
                                              const void * public_key)
{
    return (optiga_crypt_verify(me,
                                OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH,
                                digest,
                                digest_length,
                                signature,
                                signature_length,
                                public_key_source_type,
                                public_key,
                                0x0000));
}
#endif //OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED


#ifdef OPTIGA_CRYPT_ECDH_ENABLED
optiga_lib_status_t optiga_crypt_ecdh(optiga_crypt_t * me,
                                      optiga_key_id_t private_key,
                                      public_key_from_host_t * public_key,
                                      bool_t export_to_host,
                                      uint8_t * shared_secret)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_calc_ssec_params_t * p_params;

    do
    {

#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == public_key) ||
            (NULL == public_key->public_key) ||
            ((TRUE == export_to_host) && (NULL == shared_secret)))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;

        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_calc_ssec_params_t *)me->params;

        p_params->private_key = private_key;
        p_params->public_key = public_key;
        p_params->export_to_host = export_to_host;
        p_params->shared_secret = shared_secret;
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);

        return_value = optiga_cmd_calc_ssec(me->my_cmd, OPTIGA_CRYPT_ECDH_KEY_AGREEMENT_ALGORITHM, 
                                            (optiga_calc_ssec_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif  //OPTIGA_CRYPT_ECDH_ENABLED

#ifdef OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED
optiga_lib_status_t optiga_crypt_tls_prf_sha256(optiga_crypt_t * me,
                                                uint16_t secret,
                                                const uint8_t * label,
                                                uint16_t label_length,
                                                const uint8_t * seed,
                                                uint16_t seed_length,
                                                uint16_t derived_key_length,
                                                bool_t export_to_host,
                                                uint8_t * derived_key)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_derive_key_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == seed) ||
           ((TRUE == export_to_host) && (NULL == derived_key)))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;

        pal_os_memset(me->params, 0x00, sizeof(me->params));
        p_params = (optiga_derive_key_params_t *)me->params;

        p_params->input_shared_secret_oid = secret;
        p_params->label = label;
        p_params->label_length = label_length;
        p_params->seed = (const uint8_t *)seed;
        p_params->seed_length = seed_length;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        me->protection_level |= OPTIGA_COMMS_COMMAND_PROTECTION;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);
        p_params->derived_key_length = derived_key_length;
        if (FALSE != export_to_host)
        {
            p_params->derived_key = (uint8_t *)derived_key;
        }

        return_value = optiga_cmd_derive_key(me->my_cmd,
                                             0x01,
                                             (optiga_derive_key_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED


#ifdef OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED
optiga_lib_status_t optiga_crypt_rsa_generate_keypair(optiga_crypt_t * me,
                                                      optiga_rsa_key_type_t key_type,
                                                      uint8_t key_usage,
                                                      bool_t export_private_key,
                                                      void * private_key,
                                                      uint8_t * public_key,
                                                      uint16_t * public_key_length)
{

    return (optiga_crypt_generate_keypair(me,
                                         (uint8_t)key_type,
                                         key_usage,
                                         export_private_key,
                                         private_key,
                                         public_key,
                                         public_key_length));
}
#endif //OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED


#ifdef OPTIGA_CRYPT_RSA_SIGN_ENABLED
optiga_lib_status_t optiga_crypt_rsa_sign(optiga_crypt_t * me,
                                          optiga_rsa_signature_scheme_t signature_scheme,
                                          const uint8_t * digest,
                                          uint8_t digest_length,
                                          optiga_key_id_t private_key,
                                          uint8_t * signature,
                                          uint16_t * signature_length,
                                          uint16_t salt_length)
{
    return (optiga_crypt_sign(me,
                             (uint8_t)signature_scheme,
                             digest,
                             digest_length,
                             private_key,
                             signature,
                             signature_length,
                             salt_length));
}

#endif //OPTIGA_CRYPT_RSA_SIGN_ENABLED
#ifdef OPTIGA_CRYPT_RSA_VERIFY_ENABLED
optiga_lib_status_t optiga_crypt_rsa_verify(optiga_crypt_t * me,
                                            optiga_rsa_signature_scheme_t signature_scheme,
                                            const uint8_t * digest,
                                            uint8_t digest_length,
                                            const uint8_t * signature,
                                            uint16_t signature_length,
                                            uint8_t public_key_source_type,
                                            const void * public_key,
                                            uint16_t salt_length)
{
    return (optiga_crypt_verify(me,
                                (uint8_t)signature_scheme,
                                digest,
                                digest_length,
                                signature,
                                signature_length,
                                public_key_source_type,
                                public_key,
                                salt_length));
}
#endif //OPTIGA_CRYPT_RSA_VERIFY_ENABLED

#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED
optiga_lib_status_t optiga_crypt_rsa_encrypt_message(optiga_crypt_t * me,
                                                     optiga_rsa_encryption_scheme_t encryption_scheme,
                                                     const uint8_t * message,
                                                     uint16_t message_length,
                                                     const uint8_t * label,
                                                     uint16_t label_length,
                                                     uint8_t public_key_source_type,
                                                     const void * public_key,
                                                     uint8_t * encrypted_message,
                                                     uint16_t * encrypted_message_length)
{

    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == message)||
            (NULL == public_key) || (NULL == encrypted_message) || (NULL == encrypted_message_length))
        {
            break;
        }
#endif
        return_value =  optiga_crypt_rsa_enc_dec(me,
                                                 encryption_scheme,
                                                 message,
                                                 message_length,
                                                 label,
                                                 label_length,
                                                 public_key_source_type,
                                                 public_key,
                                                 encrypted_message,
                                                 encrypted_message_length,
                                                 TRUE);
    } while (FALSE);
    return (return_value);
}

optiga_lib_status_t optiga_crypt_rsa_encrypt_session(optiga_crypt_t * me,
                                                     optiga_rsa_encryption_scheme_t encryption_scheme,
                                                     const uint8_t * label,
                                                     uint16_t label_length,
                                                     uint8_t public_key_source_type,
                                                     const void * public_key,
                                                     uint8_t * encrypted_message,
                                                     uint16_t * encrypted_message_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == encrypted_message)|| (NULL == encrypted_message_length) || 
            (NULL == public_key))
        {
            break;
        }
#endif
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        me->protection_level |= OPTIGA_COMMS_COMMAND_PROTECTION;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
        return_value =  optiga_crypt_rsa_enc_dec(me,
                                                 encryption_scheme,
                                                 NULL,
                                                 0,
                                                 label,
                                                 label_length,
                                                 public_key_source_type,
                                                 public_key,
                                                 encrypted_message,
                                                 encrypted_message_length,
                                                 TRUE);
    } while (FALSE);
    return (return_value);
}

#endif //OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED
#ifdef OPTIGA_CRYPT_RSA_DECRYPT_ENABLED
optiga_lib_status_t optiga_crypt_rsa_decrypt_and_export(optiga_crypt_t * me,
                                                        optiga_rsa_encryption_scheme_t encryption_scheme,
                                                        const uint8_t * encrypted_message,
                                                        uint16_t encrypted_message_length,
                                                        const uint8_t * label,
                                                        uint16_t label_length,
                                                        optiga_key_id_t private_key,
                                                        uint8_t * message,
                                                        uint16_t * message_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
    optiga_key_id_t private_key_id;
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == message) || (NULL == message_length)|| 
            (NULL == encrypted_message))
        {
            break;
        }
#endif
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        me->protection_level |= OPTIGA_COMMS_RESPONSE_PROTECTION;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
        private_key_id = private_key;
        return_value = optiga_crypt_rsa_enc_dec(me,
                                                encryption_scheme,
                                                encrypted_message,
                                                encrypted_message_length,
                                                label,
                                                label_length,
                                                0,
                                                (void *)&private_key_id,
                                                message,
                                                message_length,
                                                FALSE);
    } while (FALSE);

    return (return_value);
}

optiga_lib_status_t optiga_crypt_rsa_decrypt_and_store(optiga_crypt_t * me,
                                                       optiga_rsa_encryption_scheme_t encryption_scheme,
                                                       const uint8_t * encrypted_message,
                                                       uint16_t encrypted_message_length,
                                                       const uint8_t * label,
                                                       uint16_t label_length,
                                                       optiga_key_id_t private_key)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
    optiga_key_id_t private_key_id;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == encrypted_message))
        {
           break;
        }
#endif
        private_key_id = private_key;
        return_value = optiga_crypt_rsa_enc_dec(me,
                                                encryption_scheme,
                                                encrypted_message,
                                                encrypted_message_length,
                                                label,
                                                label_length,
                                                0,
                                                (void *)&private_key_id,
                                                NULL,
                                                NULL,
                                                FALSE);
    } while (FALSE);

    return (return_value);
}
#endif //OPTIGA_CRYPT_RSA_DECRYPT_ENABLED
#ifdef OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED
optiga_lib_status_t optiga_crypt_rsa_generate_pre_master_secret(optiga_crypt_t * me,
                                                                const uint8_t * optional_data,
                                                                uint16_t optional_data_length,
                                                                uint16_t pre_master_secret_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        /// Maximum length of shared secret supported by OPTIGA is 48 bytes, minimum is 8 bytes.
        if ((NULL == me) || (NULL == me->my_cmd))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        /// The minimum difference between shared secret length and optional data length should be 8 bytes,
        /// since the minimum random length OPTIGA expects is 8 bytes (optional_data_length <= shared_length – 8 bytes).
        if (((optional_data_length + OPTIGA_CRYPT_MINIMUM_RANDOM_DATA_LENGTH) > pre_master_secret_length) || 
            (optional_data_length > OPTIGA_CRYTP_MINIMUM_OPTIONAL_DATA_LENGTH))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }

        return_value = optiga_crypt_get_random(me,
                                               OPTIGA_CRYTP_RANDOM_PARAM_PRE_MASTER_SECRET,
                                               NULL,
                                               (pre_master_secret_length - optional_data_length),
                                               optional_data,
                                               optional_data_length);
    } while (FALSE);

    return (return_value);
}
#endif //OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED

/**
* @}
*/
