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
* \file ifx_i2c_presentation_layer.c
*
* \brief   This file implements the IFX I2C Presentation Layer of the Infineon I2C Protocol Stack library.
*
* \ingroup  grIFXI2C
*
* @{
*/

#include "optiga/ifx_i2c/ifx_i2c_presentation_layer.h"
#include "optiga/ifx_i2c/ifx_i2c_transport_layer.h"
#include "optiga/pal/pal_crypt.h"
#include "optiga/common/optiga_lib_common.h"
#include "optiga/pal/pal_ifx_i2c_config.h"
// include lower layer header
/// @cond hidden

// Setup debug log statements
#if IFX_I2C_LOG_PRL == 1
#define LOG_PRL IFX_I2C_LOG
#else
#define LOG_PRL(...)  //printf(__VA_ARGS__);
#endif

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION        

/// @cond hidden

// Presentation Layer states
#define PRL_STATE_UNINIT                     (0x00)
#define PRL_STATE_IDLE                       (0x01)
#define PRL_STATE_START                      (0x02)
#define PRL_STATE_VERIFY                     (0x03)
#define PRL_STATE_TXRX                       (0x04)
#define PRL_STATE_HANDSHAKE                  (0x05)
#define PRL_STATE_ALERT                      (0x06)
#define PRL_STATE_ERROR                      (0x07)
#define PRL_STATE_MANAGE_CONTEXT             (0x08)

///Manage context state
#define PRL_MANAGE_CONTEXT_TX_STATE          (0x01)
#define PRL_MANAGE_CONTEXT_RX_STATE          (0x02)

/// Handshake status
#define PRL_HS_SEND_HELLO                    (0x11)
#define PRL_HS_VERIFY_HELLO                  (0x22)
#define PRL_HS_SEND_FINISHED                 (0x33)
#define PRL_HS_VERIFY_FINISHED               (0x44)
#define PRL_HS_ERROR                         (0x55)

/// Handshake type
#define PRL_MASTER_HELLO_MSG                 (0x00)
#define PRL_MASTER_FINISHED_MSG              (0x08)

///Protocol type
#define PRL_HANDSHAKE_PROTOCOL               (0x00)
#define PRL_RECORD_EXCHANGE_PROTOCOL         (0x20)
#define PRL_ALERT_PROTOCOL                   (0x40)

///Message type
#define PRL_FATAL_ALERT_MSG                  (0x00)
#define PRL_INTEGRITY_VIOLATED_ALERT_MSG     (0x04)
#define PRL_INVALID_ALERT                    (0xFF)
#define PRL_DEFAULT_ALERT                    (0x11)

///Handshake completion state
#define PRL_NEGOTIATION_NOT_DONE             (0x00)
#define PRL_NEGOTIATION_DONE                 (0x01)

///Offset calculation values
#define PRL_MASTER_ENCRYPTION_KEY_OFFSET     (0x00)
#define PRL_MASTER_ENCRYPTION_NONCE_OFFSET   (0x20)
#define PRL_MASTER_DECRYPTION_KEY_OFFSET     (0x10)
#define PRL_MASTER_DECRYPTION_NONCE_OFFSET   (0x24)
#define PRL_SLAVE_HELLO_SEQ_NUMBER_OFFSET    (0x22)
#define PRL_PLAIN_TEST_OFFSET                (0x04)
#define PRL_SCTR_OFFSET                      (0x00)
#define PRL_PROTOCOL_VERSION_OFFSET          (0x01)
#define PRL_RANDOM_DATAOFFSET                (0x02)

///Length calculation values
#define PRL_RANDOM_DATA_LENGTH               (0x20)
#define PRL_MASTER_NONCE_LENGTH              (0x04)
#define PRL_FINISHED_DATA_LENGTH             (0x30)
#define PRL_SEQ_NUMBER_LENGTH                (0x04)
#define PRL_SLAVE_HELLO_LENGTH               (0x26)
#define PRL_NONCE_LENGTH                     (0x08)

///Manage context message type
#define PRL_SAVE_CONTEXT_MSG                 (0x60)
#define PRL_CONTEXT_SAVED_MSG                (0x64)
#define PRL_RESTORE_CONTEXT_MSG              (0x68)
#define PRL_CONTEXT_RESTORED_MSG             (0x6C)

///Restore operation state
#define PRL_RESTORE_NOT_DONE                 (0x00)
#define PRL_RESTORE_DONE                     (0x01)

///Mask values
#define PRL_MANAGE_CONTEXT_MASK              (0xFC)
#define PRL_PROTECTION_MASK                  (0x03)

#define PRL_SEQUENCE_THRESHOLD               (0xFFFFFFF0)
#define PRL_TRANS_REPEAT                     (DL_TRANS_REPEAT)

#define PRL_LABLE                            "Platform Binding"
#define SHARED_SECRET_LENGTH                 (0x40)

#define FORM_SCTR_HEADER(ctx, protocol, msg, protection){\
                         p_ctx->prl.sctr = (protocol | msg | protection);}\

#define ERROR_STATE_PREPARATION(ctx, exit_machine){\
                                ctx->prl.alert_type = PRL_INVALID_ALERT;\
                                ctx->prl.state = PRL_STATE_ERROR;\
                                ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;\
                                ctx->prl.return_status = IFX_I2C_SESSION_ERROR;\
                                ctx->prl.trans_repeat_status = FALSE;\
                                exit_machine = TRUE;}\

#define CLEAR_SESSION_CONTEXT(ctx){\
                              memset(ctx->prl.session_key,0,sizeof(ctx->prl.session_key));\
                              ctx->prl.master_sequence_number = 0;\
                              ctx->prl.slave_sequence_number = 0;\
                              ctx->prl.save_slave_sequence_number = 0;\
                              ctx->prl.decryption_failure_counter = 0;\
                              ctx->prl.data_retransmit_counter = 0;\
                              ctx->prl.trans_repeat_status = FALSE;\
                              ctx->prl.state = PRL_STATE_IDLE;}\

#define CLEAR_SAVED_SESSION_CONTEXT(ctx){\
                                    memset(ctx.session_key,0,sizeof(ctx.session_key));\
                                    ctx.master_sequence_number = 0;\
                                    ctx.negotiation_state = 0;\
                                    ctx.save_slave_sequence_number = 0;\
                                    ctx.decryption_failure_counter = 0;\
                                    ctx.data_retransmit_counter = 0;\
                                    ctx.stored_context_flag = FALSE;}\

#define COPY_MANAGE_CONTEXT_DATA(source, destination){\
                                 memcpy(destination.session_key,source.session_key,sizeof(source.session_key));\
                                 destination.master_sequence_number = source.master_sequence_number;\
                                 destination.save_slave_sequence_number = source.save_slave_sequence_number;\
                                 destination.decryption_failure_counter = source.decryption_failure_counter;\
                                 destination.data_retransmit_counter = source.data_retransmit_counter;\
                                 destination.negotiation_state = source.negotiation_state;}\

#define DO_NEGOTIATION(ctx, exit_machine){\
                       ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;\
                       ctx->prl.state = PRL_STATE_HANDSHAKE;\
                       exit_machine = TRUE;\
                       ctx->prl.hs_state = PRL_HS_SEND_HELLO;}\

_STATIC_H optiga_lib_status_t ifx_i2c_prl_do_handshake(ifx_i2c_context_t * p_ctx,
                                                       const uint8_t * p_data);
_STATIC_H void ifx_i2c_prl_event_handler(ifx_i2c_context_t * p_ctx,
                                         optiga_lib_status_t event,
                                         const uint8_t * p_data,
                                         uint16_t data_len);
_STATIC_H optiga_lib_status_t ifx_i2c_prl_prf(ifx_i2c_context_t * p_ctx);
_STATIC_H optiga_lib_status_t ifx_i2c_prl_send_alert(ifx_i2c_context_t * p_ctx);
/// @endcond

optiga_lib_status_t ifx_i2c_prl_init(ifx_i2c_context_t * p_ctx,
                                     ifx_i2c_event_handler_t handler)
{
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;
    LOG_PRL("[IFX-PRL]: Init\n");

    p_ctx->tl.state = PRL_STATE_UNINIT;

    do
    {
        // Initialize transport layer (and register event handler)
        if (IFX_I2C_STACK_SUCCESS != ifx_i2c_tl_init(p_ctx, ifx_i2c_prl_event_handler))
        {
            break;
        }

        p_ctx->prl.upper_layer_event_handler = handler;
        if (IFX_I2C_SESSION_CONTEXT_RESTORE == p_ctx->manage_context_operation)
        {
            p_ctx->prl.restore_context_flag = PRL_RESTORE_NOT_DONE;
        }
        else
        {
            p_ctx->prl.restore_context_flag = PRL_RESTORE_DONE;
        }
        p_ctx->prl.state = PRL_STATE_IDLE;

        p_ctx->protocol_version = PROTOCOL_VERSION_PRE_SHARED_SECRET;
        p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
        p_ctx->prl.return_status = IFX_I2C_STACK_SUCCESS;
        p_ctx->prl.hs_state = PRL_HS_SEND_HELLO;
        p_ctx->prl.alert_type = PRL_DEFAULT_ALERT;
        return_status = IFX_I2C_STACK_SUCCESS;
    } while (FALSE);
    
    return (return_status);
}

optiga_lib_status_t ifx_i2c_prl_close(ifx_i2c_context_t * p_ctx, ifx_i2c_event_handler_t handler)
{
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;
    LOG_PRL("[IFX-PRL]: Close\n");
    do
    {
        // Presentation Layer must be idle
        if (PRL_STATE_IDLE != p_ctx->prl.state)
        {
            break;
        }
        p_ctx->prl.upper_layer_event_handler = handler;
        if (IFX_I2C_SESSION_CONTEXT_NONE == p_ctx->manage_context_operation)
        {
            p_ctx->prl.upper_layer_event_handler(p_ctx, IFX_I2C_STACK_SUCCESS, 0, 0);
            return_status = IFX_I2C_STACK_SUCCESS;
            break;
        }

        if ((PRL_NEGOTIATION_DONE == p_ctx->prl.negotiation_state) &&
            (IFX_I2C_SESSION_CONTEXT_SAVE == p_ctx->manage_context_operation))
        {
            p_ctx->prl.state = PRL_STATE_MANAGE_CONTEXT;
            p_ctx->prl.mc_state = PRL_MANAGE_CONTEXT_TX_STATE;
            ifx_i2c_prl_event_handler(p_ctx, IFX_I2C_STACK_SUCCESS, p_ctx->prl.prl_txrx_buffer, 1);
            return_status = IFX_I2C_STACK_SUCCESS;
            break;
        }
    } while (FALSE);
    return (return_status);
}

optiga_lib_status_t ifx_i2c_prl_transceive(ifx_i2c_context_t * p_ctx,
                                           uint8_t * p_tx_data,
                                           uint16_t tx_data_len,
                                           uint8_t * p_rx_data,
                                           uint16_t * p_rx_data_len)
{
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;
    LOG_PRL("[IFX-PRL]: Transceive txlen %d\n", tx_data_len);

    do
    {
        // Check function arguments and presentation Layer must be idle
        if ((NULL == p_tx_data) || (0 == tx_data_len) || (PRL_STATE_IDLE != p_ctx->prl.state))
        {
            break;
        }

        if ((IFX_I2C_SESSION_CONTEXT_RESTORE == p_ctx->manage_context_operation) && 
            (PRL_RESTORE_NOT_DONE == p_ctx->prl.restore_context_flag))
        {
            p_ctx->prl.state = PRL_STATE_MANAGE_CONTEXT;
            p_ctx->prl.mc_state = PRL_MANAGE_CONTEXT_TX_STATE;
        }
        else
        {
            p_ctx->prl.state = PRL_STATE_START;
        }
        p_ctx->prl.p_actual_payload = p_tx_data;
        p_ctx->prl.actual_payload_length = tx_data_len;

        if ((SLAVE_PROTECTION == (p_ctx->protection_level & PRL_PROTECTION_MASK)) ||
            (FULL_PROTECTION == (p_ctx->protection_level & PRL_PROTECTION_MASK)))
        {
            p_ctx->prl.p_recv_payload_buffer = p_rx_data;
        }
        else
        {
            p_ctx->prl.p_recv_payload_buffer = &p_rx_data[PRL_PLAIN_TEST_OFFSET];
        }
        p_ctx->prl.p_recv_payload_buffer_length = p_rx_data_len;
        p_ctx->prl.data_retransmit_counter = 0;
        p_ctx->prl.trans_repeat_status = FALSE;
        p_ctx->prl.decryption_failure_counter = 0;
        p_ctx->prl.return_status = IFX_I2C_STACK_SUCCESS;
        p_ctx->prl.alert_type = PRL_DEFAULT_ALERT;

        ifx_i2c_prl_event_handler(p_ctx,
                                  IFX_I2C_STACK_SUCCESS,
                                  p_tx_data, tx_data_len);
        return_status = IFX_I2C_STACK_SUCCESS;

    } while (FALSE);
    return (return_status);
}

_STATIC_H optiga_lib_status_t ifx_i2c_prl_prf(ifx_i2c_context_t * p_ctx)
{
    optiga_lib_status_t return_status = IFX_I2C_HANDSHAKE_ERROR;
    uint8_t label_input[] = PRL_LABLE;
    uint8_t secret_input[SHARED_SECRET_LENGTH];
    do
    {
        //Reading pre-shared secret from datastore
        return_status = pal_os_datastore_read(p_ctx->ifx_i2c_datastore_config->datastore_shared_secret_id,
                                              secret_input,
                                              &p_ctx->ifx_i2c_datastore_config->shared_secret_length);
        if (PAL_STATUS_SUCCESS != return_status)
        {
            return_status = IFX_I2C_HANDSHAKE_ERROR;
            break;
        }
        if (PAL_STATUS_SUCCESS != pal_crypt_tls_prf_sha256(NULL, secret_input,
                                          sizeof(secret_input),
                                          label_input,
                                          sizeof(label_input) - 1,
                                          p_ctx->prl.random,
                                          sizeof(p_ctx->prl.random),
                                          p_ctx->prl.session_key,
                                          sizeof(p_ctx->prl.session_key)))
        {
            return_status = IFX_I2C_HANDSHAKE_ERROR;
        }
        memset(secret_input, 0, p_ctx->ifx_i2c_datastore_config->shared_secret_length);

    } while (FALSE);
    return (return_status);
}

_STATIC_H void ifx_i2c_prl_form_associated_data(ifx_i2c_context_t * p_ctx,
                                                uint16_t data_len,
                                                uint32_t seq_number,
                                                uint8_t sctr)
{
    p_ctx->prl.associate_data[0] = sctr;
    optiga_common_set_uint32(&p_ctx->prl.associate_data[1], seq_number);
    p_ctx->prl.associate_data[5] = p_ctx->protocol_version;
    optiga_common_set_uint16(&p_ctx->prl.associate_data[6], data_len);
}

_STATIC_H optiga_lib_status_t ifx_i2c_prl_encrypt_msg(ifx_i2c_context_t * p_ctx,
                                                      uint8_t * p_data,
                                                      uint16_t data_len,
                                                      uint32_t seq_number,
                                                      uint8_t sctr)
{
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;
    uint8_t nonce_data[PRL_NONCE_LENGTH];
    do
    {
        //Form associated data
        ifx_i2c_prl_form_associated_data(p_ctx, data_len,seq_number,sctr);
        //Prepare nonce data
        memcpy(nonce_data, &p_ctx->prl.session_key[PRL_MASTER_ENCRYPTION_NONCE_OFFSET], PRL_MASTER_NONCE_LENGTH);
        optiga_common_set_uint32(&nonce_data[PRL_MASTER_NONCE_LENGTH], seq_number);

        if (PAL_STATUS_SUCCESS != (pal_crypt_encrypt_aes128_ccm(NULL,
                                                                p_data,
                                                                data_len,
                                                                &p_ctx->prl.
                                                                session_key[PRL_MASTER_ENCRYPTION_KEY_OFFSET],
                                                                nonce_data, PRL_MASTER_NONCE_LENGTH + 
                                                                PRL_SEQ_NUMBER_LENGTH,
                                                                p_ctx->prl.associate_data,
                                                                sizeof(p_ctx->prl.associate_data),
                                                                IFX_I2C_PRL_MAC_SIZE,
                                                                p_data)))
        {
            break;
        }
        return_status = IFX_I2C_STACK_SUCCESS;
    } while (FALSE);
    return (return_status);
}

_STATIC_H optiga_lib_status_t ifx_i2c_prl_decrypt_msg(ifx_i2c_context_t * p_ctx,
                                                      const uint8_t * p_data,
                                                      uint16_t data_len,
                                                      uint32_t seq_number,
                                                      uint8_t * out_data,
                                                      uint8_t decrypt_key_offset,
                                                      uint8_t decrypt_nonce_offset,
                                                      uint8_t sctr)
{
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;
    uint8_t nonce_data[PRL_NONCE_LENGTH];
    do
    {
        //Form associated data
        ifx_i2c_prl_form_associated_data(p_ctx, data_len, seq_number, sctr);
        //Prepare nonce data
        memcpy(nonce_data, &p_ctx->prl.session_key[decrypt_nonce_offset], PRL_MASTER_NONCE_LENGTH);
        optiga_common_set_uint32(&nonce_data[PRL_MASTER_NONCE_LENGTH], seq_number);

        if (PAL_STATUS_SUCCESS != (pal_crypt_decrypt_aes128_ccm(NULL,
                                                                p_data,
                                                                (data_len + IFX_I2C_PRL_MAC_SIZE),
                                                                &p_ctx->prl.session_key[decrypt_key_offset],
                                                                nonce_data,
                                                                PRL_MASTER_NONCE_LENGTH + PRL_SEQ_NUMBER_LENGTH,
                                                                p_ctx->prl.associate_data,
                                                                sizeof(p_ctx->prl.associate_data),
                                                                IFX_I2C_PRL_MAC_SIZE,
                                                                out_data)))
        {
            break;
        }
        return_status = IFX_I2C_STACK_SUCCESS;
    } while (FALSE);
    return (return_status);
}

_STATIC_H optiga_lib_status_t ifx_i2c_prl_send_alert(ifx_i2c_context_t * p_ctx)
{
    optiga_lib_status_t return_status = IFX_I2C_STACK_SUCCESS;
    do
    {
        p_ctx->prl.state = PRL_STATE_IDLE;
        if (PRL_INTEGRITY_VIOLATED_ALERT_MSG == p_ctx->prl.alert_type)
        {
            p_ctx->prl.decryption_failure_counter++;
            p_ctx->prl.state = PRL_STATE_VERIFY;
            p_ctx->prl.return_status = IFX_I2C_STACK_SUCCESS;
            FORM_SCTR_HEADER(p_ctx, PRL_ALERT_PROTOCOL, p_ctx->prl.alert_type, 0);
            p_ctx->prl.prl_txrx_buffer[0] = p_ctx->prl.sctr;
            return_status = ifx_i2c_tl_transceive(p_ctx,
                                                  p_ctx->prl.prl_txrx_buffer,
                                                  1,
                                                  p_ctx->prl.p_recv_payload_buffer,
                                                  &p_ctx->prl.prl_receive_length);
            *p_ctx->prl.p_recv_payload_buffer_length = p_ctx->prl.prl_receive_length;
        }
        else
        {
            p_ctx->prl.return_status = IFX_I2C_SESSION_ERROR;
            p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
            p_ctx->prl.alert_type = PRL_INVALID_ALERT;
            break;
        }

    } while (FALSE);
    return (return_status);
}

_STATIC_H optiga_lib_status_t ifx_i2c_prl_do_handshake(ifx_i2c_context_t * p_ctx, const uint8_t * p_data)
{
    uint8_t sseq[4];
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;
    uint32_t exit_machine = FALSE;
    LOG_PRL("[IFX-PRL]: Do Handshake \n");

    do
    {
        switch (p_ctx->prl.hs_state)
        {
            case PRL_HS_SEND_HELLO:
            {
                //Preparing hello message
                FORM_SCTR_HEADER(p_ctx,PRL_HANDSHAKE_PROTOCOL, PRL_MASTER_HELLO_MSG, 0);
                p_ctx->prl.prl_txrx_buffer[PRL_SCTR_OFFSET] = p_ctx->prl.sctr;
                p_ctx->prl.prl_txrx_buffer[PRL_PROTOCOL_VERSION_OFFSET] = p_ctx->protocol_version;
                p_ctx->prl.prl_receive_length = PRL_SLAVE_HELLO_LENGTH;
                //Sending hello message
                return_status = ifx_i2c_tl_transceive(p_ctx,
                                                      p_ctx->prl.prl_txrx_buffer,
                                                      2,
                                                      p_ctx->prl.prl_txrx_buffer,
                                                      &p_ctx->prl.prl_receive_length);
                if (IFX_I2C_STACK_ERROR == return_status)
                {
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    exit_machine = TRUE;
                    break;
                }
                p_ctx->prl.hs_state = PRL_HS_VERIFY_HELLO;
            }
            break;
            case PRL_HS_VERIFY_HELLO:
            {
                exit_machine = TRUE;
                if ((((p_data[PRL_SCTR_OFFSET] & (PRL_ALERT_PROTOCOL | PRL_FATAL_ALERT_MSG)) ||
                    (p_data[PRL_SCTR_OFFSET] & (PRL_ALERT_PROTOCOL | PRL_INTEGRITY_VIOLATED_ALERT_MSG))) &&
                    (1 == p_ctx->prl.prl_receive_length)))
                {
                    return_status = IFX_I2C_HANDSHAKE_ERROR;
                    p_ctx->prl.alert_type = PRL_INVALID_ALERT;
                    exit_machine = FALSE;
                    break;
                }
                //Check valid message type and protocol version
                if ((PRL_SLAVE_HELLO_LENGTH != p_ctx->prl.prl_receive_length) || (0 != p_data[PRL_SCTR_OFFSET]) ||
                    (PROTOCOL_VERSION_PRE_SHARED_SECRET != p_data[PRL_PROTOCOL_VERSION_OFFSET]))
                {
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    break;
                }
                //Store slave sequence number and random number
                p_ctx->prl.slave_sequence_number = optiga_common_get_uint32(&p_data[PRL_SLAVE_HELLO_SEQ_NUMBER_OFFSET]);
                p_ctx->prl.save_slave_sequence_number = p_ctx->prl.slave_sequence_number;
                memcpy(p_ctx->prl.random, &p_data[PRL_RANDOM_DATAOFFSET], PRL_RANDOM_DATA_LENGTH);

                //Generating session key
                return_status = ifx_i2c_prl_prf(p_ctx);
                if (IFX_I2C_HANDSHAKE_ERROR == return_status)
                {
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    break;
                }
                p_ctx->prl.hs_state = PRL_HS_SEND_FINISHED;

            }
            break;
            case PRL_HS_SEND_FINISHED:
            {
                FORM_SCTR_HEADER(p_ctx, PRL_HANDSHAKE_PROTOCOL, PRL_MASTER_FINISHED_MSG,0);
                exit_machine = FALSE;
                //Creating payload for encryption
                memcpy(&p_ctx->prl.prl_txrx_buffer[IFX_I2C_PRL_HEADER_SIZE] ,p_ctx->prl.random, PRL_RANDOM_DATA_LENGTH);
                optiga_common_set_uint32(&p_ctx->prl.prl_txrx_buffer[PRL_RANDOM_DATA_LENGTH + IFX_I2C_PRL_HEADER_SIZE],
                                         p_ctx->prl.slave_sequence_number);

                //Preparing encryption data
                return_status = ifx_i2c_prl_encrypt_msg(p_ctx,
                                                        &p_ctx->prl.prl_txrx_buffer[IFX_I2C_PRL_HEADER_SIZE],
                                                        PRL_RANDOM_DATA_LENGTH + PRL_SEQ_NUMBER_LENGTH,
                                                        p_ctx->prl.slave_sequence_number,
                                                        p_ctx->prl.sctr);
                if (IFX_I2C_STACK_ERROR == return_status)
                {
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    exit_machine = TRUE;
                    break;
                }

                //Append presentation layer header
                p_ctx->prl.prl_txrx_buffer[PRL_SCTR_OFFSET] = p_ctx->prl.sctr;
                optiga_common_set_uint32(&p_ctx->prl.prl_txrx_buffer[1], p_ctx->prl.slave_sequence_number);

                p_ctx->prl.prl_receive_length = PRL_FINISHED_DATA_LENGTH + 1;
                return_status = ifx_i2c_tl_transceive(p_ctx,
                                                      p_ctx->prl.prl_txrx_buffer,
                                                      PRL_FINISHED_DATA_LENGTH + 1,
                                                      p_ctx->prl.prl_txrx_buffer,
                                                      &p_ctx->prl.prl_receive_length);
                if (IFX_I2C_STACK_SUCCESS != return_status)
                {
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    exit_machine = TRUE;
                    break;
                }
                p_ctx->prl.hs_state = PRL_HS_VERIFY_FINISHED;
            }
            break;
            case PRL_HS_VERIFY_FINISHED:
            {
                if ((PRL_MASTER_FINISHED_MSG != p_data[PRL_SCTR_OFFSET]) ||
                    ((PRL_FINISHED_DATA_LENGTH + 1) != p_ctx->prl.prl_receive_length))
                {
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    exit_machine = TRUE;
                    break;
                }
                p_ctx->prl.master_sequence_number = optiga_common_get_uint32(&p_data[1]);
                return_status = ifx_i2c_prl_decrypt_msg(p_ctx,
                                                        &p_data[IFX_I2C_PRL_HEADER_SIZE],
                                                        PRL_RANDOM_DATA_LENGTH + PRL_SEQ_NUMBER_LENGTH,
                                                        p_ctx->prl.master_sequence_number,
                                                        p_ctx->prl.prl_txrx_buffer,
                                                        PRL_MASTER_DECRYPTION_KEY_OFFSET,
                                                        PRL_MASTER_DECRYPTION_NONCE_OFFSET,
                                                        p_ctx->prl.sctr);
                if (IFX_I2C_STACK_ERROR == return_status)
                {
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    exit_machine = TRUE;
                    break;
                }
                optiga_common_set_uint32(sseq, p_ctx->prl.master_sequence_number);
                ///Validating decrpt data
                if (0 != (memcmp(p_ctx->prl.random, p_ctx->prl.prl_txrx_buffer, PRL_RANDOM_DATA_LENGTH)))
                {
                    return_status = IFX_I2C_HANDSHAKE_ERROR;
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    exit_machine = TRUE;
                    break;
                }
                ///Validating sequence number
                if (0 != (memcmp(sseq, &p_ctx->prl.prl_txrx_buffer[PRL_RANDOM_DATA_LENGTH], PRL_SEQ_NUMBER_LENGTH)))
                {
                    return_status = IFX_I2C_HANDSHAKE_ERROR;
                    p_ctx->prl.hs_state = PRL_HS_ERROR;
                    exit_machine = TRUE;
                    break;
                }
                p_ctx->prl.negotiation_state = PRL_NEGOTIATION_DONE;
            }
            break;
            case PRL_HS_ERROR:
            {
                return_status = IFX_I2C_HANDSHAKE_ERROR;
                p_ctx->prl.alert_type = PRL_INVALID_ALERT;
                exit_machine = FALSE;
            }
            break;
            default:
            break;
        }

    } while (TRUE == exit_machine);

    return (return_status);
}

_STATIC_H optiga_lib_status_t ifx_i2c_prl_do_manage_context(ifx_i2c_context_t * p_ctx,
                                                            const uint8_t * p_data,
                                                            uint16_t data_len,
                                                            uint8_t * exit_machine)
{
    ifx_i2c_prl_manage_context_t prl_saved_ctx;
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;
    uint8_t sctr;

    switch (p_ctx->prl.mc_state)
    {
        case PRL_MANAGE_CONTEXT_TX_STATE:
        {
            LOG_PRL("[IFX-PRL]: PRL_STATE_MANAGE_CONTEXT \n");
            if (0 != (p_ctx->protection_level & RE_ESTABLISH))
            {
                p_ctx->prl.state = PRL_STATE_START;
                return_status = IFX_I2C_STACK_SUCCESS;
                break;
            }

            if (IFX_I2C_SESSION_CONTEXT_SAVE == p_ctx->manage_context_operation)
            {
                p_ctx->prl.prl_txrx_buffer[0] = PRL_SAVE_CONTEXT_MSG;
                p_ctx->prl.prl_receive_length = 1;
                p_ctx->prl.prl_txrx_receive_length = 1;
            }
            else if (IFX_I2C_SESSION_CONTEXT_RESTORE == p_ctx->manage_context_operation)
            {
                /// Restoring saved context from data store
                if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id)
                {
                    p_ctx->prl.prl_receive_length = sizeof(p_ctx->prl.prl_saved_ctx);
                    return_status = pal_os_datastore_read(p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id,
                                                          (uint8_t * )&p_ctx->prl.prl_saved_ctx,
                                                          &p_ctx->prl.prl_receive_length);
                    if (PAL_STATUS_FAILURE == return_status)
                    {
                        break;
                    }
                    p_ctx->prl.negotiation_state = p_ctx->prl.prl_saved_ctx.negotiation_state;
                    if (p_ctx->prl.prl_saved_ctx.stored_context_flag == FALSE)
                    {
                        p_ctx->prl.state = PRL_STATE_START;
                        p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
                        p_ctx->prl.restore_context_flag = PRL_RESTORE_DONE;
                        *exit_machine = TRUE;
                        return_status = IFX_I2C_STACK_SUCCESS;
                        break;
                    }
                }
                else
                {
                    if (p_ctx->prl.prl_saved_ctx.stored_context_flag == FALSE)
                    {
                        p_ctx->prl.state = PRL_STATE_START;
                        p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
                        p_ctx->prl.restore_context_flag = PRL_RESTORE_DONE;
                        *exit_machine = TRUE;
                        return_status = IFX_I2C_STACK_SUCCESS;
                        break;
                    }
                    else
                    {
                        p_ctx->prl.negotiation_state = p_ctx->prl.prl_saved_ctx.negotiation_state;
                    }
                }
                ///Prepare restore message
                p_ctx->prl.prl_txrx_buffer[PRL_SCTR_OFFSET] = PRL_RESTORE_CONTEXT_MSG;
                optiga_common_set_uint32(&p_ctx->prl.prl_txrx_buffer[1], 
                                         p_ctx->prl.prl_saved_ctx.save_slave_sequence_number);
                p_ctx->prl.prl_txrx_receive_length = 5;
                prl_saved_ctx.stored_context_flag = FALSE;
                p_ctx->prl.prl_receive_length = 5;
                p_ctx->prl.prl_saved_ctx.stored_context_flag = FALSE;
                ///Clearing the saved context in data store
                if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id)
                {
                    memset((uint8_t * )&prl_saved_ctx,0,sizeof(prl_saved_ctx));
                    return_status = pal_os_datastore_write(p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id,
                                                           (uint8_t * )&prl_saved_ctx,
                                                           sizeof(prl_saved_ctx));
                    if (PAL_STATUS_FAILURE == return_status)
                    {
                        break;
                    }
                }
            }
            else
            {
                /// Message type invalid
                p_ctx->prl.alert_type = PRL_INVALID_ALERT;
                p_ctx->prl.state = PRL_STATE_ERROR;
                p_ctx->prl.return_status = IFX_I2C_STACK_ERROR;
                break;
            }
            return_status = ifx_i2c_tl_transceive(p_ctx,
                                                  p_ctx->prl.prl_txrx_buffer,
                                                  p_ctx->prl.prl_txrx_receive_length,
                                                  p_ctx->prl.prl_txrx_buffer,
                                                  &p_ctx->prl.prl_receive_length);
            if (IFX_I2C_STACK_SUCCESS != return_status)
            {
                break;
            }
            p_ctx->prl.mc_state = PRL_MANAGE_CONTEXT_RX_STATE;
            return_status = IFX_I2C_STACK_SUCCESS;
            *exit_machine = FALSE;
        }
        break;
        case PRL_MANAGE_CONTEXT_RX_STATE:
        {
            *exit_machine = TRUE;
            sctr = p_data[PRL_SCTR_OFFSET] & PRL_MANAGE_CONTEXT_MASK;
            if ((PRL_CONTEXT_RESTORED_MSG == sctr) &&
                (IFX_I2C_SESSION_CONTEXT_RESTORE == p_ctx->manage_context_operation) && (5 == data_len))
            {
                ///Restore the saved context to active context structure
                COPY_MANAGE_CONTEXT_DATA(p_ctx->prl.prl_saved_ctx,p_ctx->prl);
                p_ctx->prl.restore_context_flag = PRL_RESTORE_DONE;
                p_ctx->prl.state = PRL_STATE_TXRX;
            }
            else if ((PRL_CONTEXT_SAVED_MSG == sctr) &&
                     (IFX_I2C_SESSION_CONTEXT_SAVE == p_ctx->manage_context_operation) && (1 == data_len))
            {
                p_ctx->prl.prl_saved_ctx.stored_context_flag = TRUE;
                COPY_MANAGE_CONTEXT_DATA(p_ctx->prl, p_ctx->prl.prl_saved_ctx);
                
                if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id)
                {
                    ///Store active session to data store
                    p_ctx->prl.prl_receive_length = sizeof(p_ctx->prl.prl_saved_ctx);
                    return_status = pal_os_datastore_write(p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id,
                                                           (uint8_t * )&p_ctx->prl.prl_saved_ctx,
                                                           p_ctx->prl.prl_receive_length);
                    if (PAL_STATUS_SUCCESS != return_status)
                    {
                        ERROR_STATE_PREPARATION(p_ctx,*exit_machine);
                        break;
                    }
                }
                ///Clearing the active session
                CLEAR_SESSION_CONTEXT(p_ctx);
                p_ctx->prl.state = PRL_STATE_IDLE;
            }
            else
            {
                memset((uint8_t * )&p_ctx->prl.prl_saved_ctx,0,sizeof(p_ctx->prl.prl_saved_ctx));
                if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id)
                {
                    return_status = pal_os_datastore_write(p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id,
                                                           (uint8_t * )&p_ctx->prl.prl_saved_ctx,
                                                           sizeof(p_ctx->prl.prl_saved_ctx));
                }
                return_status = IFX_I2C_STACK_ERROR;
                break;
            }
            p_ctx->prl.return_status = IFX_I2C_STACK_SUCCESS;
            return_status = IFX_I2C_STACK_SUCCESS;
        }
        break;
        default:
        {
            ERROR_STATE_PREPARATION(p_ctx,*exit_machine);
        }
        break;
    }
  return (return_status);
}

_STATIC_H void ifx_i2c_prl_event_handler(ifx_i2c_context_t * p_ctx,
                                         optiga_lib_status_t event,
                                         const uint8_t * p_data,
                                         uint16_t data_len)
{
    //if handshake
    uint8_t exit_machine = TRUE;
    ifx_i2c_prl_manage_context_t prl_saved_ctx;
    optiga_lib_status_t return_status = IFX_I2C_STACK_ERROR;

    LOG_PRL("[IFX-PRL]: ifx_i2c_prl_event_handler %d\n", data_len);
    if ((0 != (event & IFX_I2C_STACK_MEM_ERROR)) || (0 != (event & IFX_I2C_STACK_ERROR)))
    {
        p_ctx->prl.alert_type = PRL_INVALID_ALERT;
        if (PRL_STATE_HANDSHAKE == p_ctx->prl.state)
        {
            p_ctx->prl.return_status = IFX_I2C_HANDSHAKE_ERROR;
        }
        else if ((PRL_NEGOTIATION_DONE == p_ctx->prl.negotiation_state) && (IFX_I2C_STACK_MEM_ERROR != event))
        {
            p_ctx->prl.return_status = IFX_I2C_SESSION_ERROR;
            p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
        }
        else
        {
            p_ctx->prl.return_status = event;
        }
        p_ctx->prl.state = PRL_STATE_ERROR;
    }

    do
    {
        //exit_machine = FALSE;
        switch (p_ctx->prl.state)
        {
            case PRL_STATE_IDLE:
            {
                LOG_PRL("[IFX-PRL]: PRL_STATE_IDLE %d\n", p_ctx->prl.return_status);
                p_ctx->prl.upper_layer_event_handler(p_ctx, p_ctx->prl.return_status, 0, 0);
                exit_machine = FALSE;
            }
            break;
            case PRL_STATE_MANAGE_CONTEXT:
            {
                LOG_PRL("[IFX-PRL]: PRL_STATE_MANAGE_CONTEXT \n");
                return_status = ifx_i2c_prl_do_manage_context(p_ctx, p_data,data_len,&exit_machine);
                if (IFX_I2C_STACK_SUCCESS != return_status)
                {
                  ERROR_STATE_PREPARATION(p_ctx,exit_machine);
                }
            }
            break;
            case PRL_STATE_START:
            {
                if (((p_ctx->protection_level & RE_ESTABLISH) ||
                    ((p_ctx->protection_level != NO_PROTECTION ) &&
                    (PRL_NEGOTIATION_NOT_DONE == p_ctx->prl.negotiation_state))))
                {
                    p_ctx->prl.state =  PRL_STATE_HANDSHAKE;
                    p_ctx->prl.hs_state = PRL_HS_SEND_HELLO;
                    p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
                }
                else
                {
                    p_ctx->prl.state =  PRL_STATE_TXRX;
                }
            }
            break;
            case PRL_STATE_HANDSHAKE:
            {
                ///Clearing the saved context in data store
                CLEAR_SAVED_SESSION_CONTEXT(p_ctx->prl.prl_saved_ctx);
                memset((uint8_t * )&prl_saved_ctx,0,sizeof(prl_saved_ctx));
                if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id)
                {
                    return_status = pal_os_datastore_write(p_ctx->ifx_i2c_datastore_config->datastore_manage_context_id,
                                                           (uint8_t * )&prl_saved_ctx,
                                                           sizeof(prl_saved_ctx));
                    if (PAL_STATUS_SUCCESS != return_status)
                    {
                        ERROR_STATE_PREPARATION(p_ctx,exit_machine);
                        p_ctx->prl.return_status = IFX_I2C_HANDSHAKE_ERROR;
                        break;
                    }
                }
                return_status = ifx_i2c_prl_do_handshake(p_ctx,p_data);
                if (IFX_I2C_HANDSHAKE_ERROR == return_status)
                {
                    p_ctx->prl.return_status = return_status;
                    p_ctx->prl.state = PRL_STATE_ERROR;
                    break;
                }

                if (PRL_NEGOTIATION_DONE == p_ctx->prl.negotiation_state)
                {
                    p_ctx->prl.state = PRL_STATE_TXRX;
                }
                else
                {
                    exit_machine = FALSE;
                }
            }
            break;
            case PRL_STATE_TXRX:
            {
                FORM_SCTR_HEADER(p_ctx,
                                 PRL_RECORD_EXCHANGE_PROTOCOL,
                                 0,
                                 (p_ctx->protection_level & PRL_PROTECTION_MASK));
                ///Preparing receive length based on the protection level
                if ((SLAVE_PROTECTION == (p_ctx->protection_level & PRL_PROTECTION_MASK)) ||
                    (FULL_PROTECTION == (p_ctx->protection_level & PRL_PROTECTION_MASK)))
                {
                    if ((PRL_SEQUENCE_THRESHOLD < (p_ctx->prl.save_slave_sequence_number + 1)) &&
                        (FALSE == p_ctx->prl.trans_repeat_status))
                    {
                        DO_NEGOTIATION(p_ctx,exit_machine);
                        break;
                    }
                    *p_ctx->prl.p_recv_payload_buffer_length += (IFX_I2C_PRL_MAC_SIZE + IFX_I2C_PRL_HEADER_SIZE);
                }
                p_ctx->prl.saved_sctr = p_ctx->prl.sctr;

                if ((MASTER_PROTECTION == (p_ctx->protection_level & PRL_PROTECTION_MASK)) ||
                    (FULL_PROTECTION == (p_ctx->protection_level & PRL_PROTECTION_MASK)))
                {
                    if ((PRL_SEQUENCE_THRESHOLD < (p_ctx->prl.master_sequence_number + 1)) &&
                        (FALSE == p_ctx->prl.trans_repeat_status))
                    {
                        DO_NEGOTIATION(p_ctx,exit_machine);
                        break;
                    }
                    ///Decrypt messag to use for retansmit
                    if (PRL_INTEGRITY_VIOLATED_ALERT_MSG == p_ctx->prl.alert_type)
                    {
                        return_status = ifx_i2c_prl_decrypt_msg(p_ctx,
                                                                &p_ctx->prl.p_actual_payload[IFX_I2C_PRL_HEADER_SIZE],
                                                                p_ctx->prl.actual_payload_length,
                                                                p_ctx->prl.master_sequence_number,
                                                                &p_ctx->prl.p_actual_payload[IFX_I2C_PRL_HEADER_SIZE],
                                                                PRL_MASTER_ENCRYPTION_KEY_OFFSET,
                                                                PRL_MASTER_ENCRYPTION_NONCE_OFFSET,
                                                                p_ctx->prl.saved_sctr);
                        if (IFX_I2C_STACK_ERROR == return_status)
                        {
                            ERROR_STATE_PREPARATION(p_ctx,exit_machine);
                            break;
                        }
                        *p_ctx->prl.p_recv_payload_buffer_length = p_ctx->prl.prl_receive_length;
                    }

                    p_ctx->prl.master_sequence_number += 1;
                    p_ctx->prl.prl_header_offset = IFX_I2C_PRL_HEADER_SIZE;
                    optiga_common_set_uint32(&p_ctx->prl.p_actual_payload[1], p_ctx->prl.master_sequence_number);
                    //Preparing encription data
                    return_status = ifx_i2c_prl_encrypt_msg(p_ctx,
                                                            &p_ctx->prl.p_actual_payload[IFX_I2C_PRL_HEADER_SIZE],
                                                            p_ctx->prl.actual_payload_length,
                                                            p_ctx->prl.master_sequence_number,
                                                            p_ctx->prl.saved_sctr);
                    if (IFX_I2C_STACK_ERROR == return_status)
                    {
                        ERROR_STATE_PREPARATION(p_ctx,exit_machine);
                        break;
                    }
                    p_ctx->prl.p_actual_payload[PRL_SCTR_OFFSET] = p_ctx->prl.sctr;
                    return_status = ifx_i2c_tl_transceive(p_ctx,
                                                          p_ctx->prl.p_actual_payload,
                                                          p_ctx->prl.actual_payload_length + 
                                                          p_ctx->prl.prl_header_offset + IFX_I2C_PRL_MAC_SIZE,
                                                          p_ctx->prl.p_recv_payload_buffer,
                                                          p_ctx->prl.p_recv_payload_buffer_length);
                }
                else
                {
                    ///Sending plan data
                    p_ctx->prl.prl_header_offset = 1;
                    p_ctx->prl.p_actual_payload[4] = p_ctx->prl.sctr;
                    return_status = ifx_i2c_tl_transceive(p_ctx,
                                                          &p_ctx->prl.p_actual_payload[4],
                                                          p_ctx->prl.actual_payload_length + 
                                                          p_ctx->prl.prl_header_offset,
                                                          p_ctx->prl.p_recv_payload_buffer,
                                                          p_ctx->prl.p_recv_payload_buffer_length);
                }
                if (IFX_I2C_STACK_ERROR == return_status)
                {
                    ERROR_STATE_PREPARATION(p_ctx,  exit_machine);
                    p_ctx->prl.return_status = IFX_I2C_STACK_ERROR;
                    break;
                }
                p_ctx->prl.prl_receive_length = *p_ctx->prl.p_recv_payload_buffer_length;
                p_ctx->prl.state = PRL_STATE_VERIFY;
                exit_machine = FALSE;
            }
            break;
            case PRL_STATE_VERIFY:
            {
                p_ctx->prl.slave_sequence_number = optiga_common_get_uint32(&p_ctx->prl.p_recv_payload_buffer[1]);
                if (0 == data_len)
                {
                    ERROR_STATE_PREPARATION(p_ctx,exit_machine);
                    break;
                }
                else if (0 != (p_data[PRL_SCTR_OFFSET] & PRL_ALERT_PROTOCOL))
                {
                    //Check invalid message
                    if (((p_data[PRL_SCTR_OFFSET] != (PRL_ALERT_PROTOCOL | PRL_FATAL_ALERT_MSG)) &&
                        (p_data[PRL_SCTR_OFFSET] != (PRL_ALERT_PROTOCOL | PRL_INTEGRITY_VIOLATED_ALERT_MSG))) ||
                        ((1 != data_len) && (PRL_NEGOTIATION_DONE == p_ctx->prl.negotiation_state)) ||
                        (PRL_TRANS_REPEAT == p_ctx->prl.data_retransmit_counter))
                    {
                        p_ctx->prl.return_status = IFX_I2C_SESSION_ERROR;
                        p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
                    }
                    //After restore operation if invalid message received
                    else if ((PRL_RESTORE_DONE == p_ctx->prl.restore_context_flag) &&
                             (IFX_I2C_SESSION_CONTEXT_RESTORE == p_ctx->manage_context_operation))
                    {
                        p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
                        p_ctx->prl.return_status = IFX_I2C_SESSION_ERROR;
                    }
                    else
                    {
                        p_ctx->prl.return_status = IFX_I2C_STACK_ERROR;
                    }
                    p_ctx->prl.state = PRL_STATE_ALERT;
                    p_ctx->prl.alert_type = PRL_INVALID_ALERT;
                }
                ///Sctr value mismatch
                else if (p_data[PRL_SCTR_OFFSET] != p_ctx->prl.saved_sctr)
                {
                    ERROR_STATE_PREPARATION(p_ctx, exit_machine);
                    break;
                }
                ///For slave encrypted message
                else if (((uint8_t)SLAVE_PROTECTION == (p_data[PRL_SCTR_OFFSET] & PRL_PROTECTION_MASK)) ||
                         ((uint8_t)FULL_PROTECTION == (p_data[PRL_SCTR_OFFSET] & PRL_PROTECTION_MASK)))
                {
                    if (data_len <= (IFX_I2C_PRL_HEADER_SIZE + IFX_I2C_PRL_MAC_SIZE))
                    {
                        ERROR_STATE_PREPARATION(p_ctx, exit_machine);
                        break;
                    }
                    //Check invalid sequence number
                    else if ((p_ctx->prl.slave_sequence_number > (p_ctx->prl.save_slave_sequence_number + PRL_TRANS_REPEAT))
                             || (p_ctx->prl.slave_sequence_number <= p_ctx->prl.save_slave_sequence_number ))
                    {
                        ERROR_STATE_PREPARATION(p_ctx,exit_machine);
                        break;
                    }

                    p_ctx->prl.save_slave_sequence_number = p_ctx->prl.slave_sequence_number;
                    *p_ctx->prl.p_recv_payload_buffer_length = *p_ctx->prl.p_recv_payload_buffer_length -
                                                               (IFX_I2C_PRL_MAC_SIZE + IFX_I2C_PRL_HEADER_SIZE);
                    return_status = ifx_i2c_prl_decrypt_msg(p_ctx,
                                                            &p_ctx->prl.p_recv_payload_buffer[IFX_I2C_PRL_HEADER_SIZE],
                                                            (*p_ctx->prl.p_recv_payload_buffer_length),
                                                            p_ctx->prl.slave_sequence_number,
                                                            &p_ctx->prl.p_recv_payload_buffer[IFX_I2C_PRL_HEADER_SIZE],
                                                            PRL_MASTER_DECRYPTION_KEY_OFFSET,
                                                            PRL_MASTER_DECRYPTION_NONCE_OFFSET,
                                                            p_ctx->prl.saved_sctr);
                    if (IFX_I2C_STACK_ERROR == return_status)
                    {
                        p_ctx->prl.state = PRL_STATE_ALERT;
                        p_ctx->prl.alert_type = PRL_INTEGRITY_VIOLATED_ALERT_MSG;
                        break;
                    }
                    else
                    {
                        p_ctx->prl.return_status = IFX_I2C_STACK_SUCCESS;
                        p_ctx->prl.state = PRL_STATE_IDLE;
                    }
                }
                else if (data_len <= 1)
                {
                    p_ctx->prl.state = PRL_STATE_ERROR;
                    p_ctx->prl.alert_type = PRL_INVALID_ALERT;
                    if (PRL_NEGOTIATION_NOT_DONE == p_ctx->prl.negotiation_state)
                    {
                        p_ctx->prl.return_status = IFX_I2C_STACK_ERROR;
                    }
                    else
                    {
                        p_ctx->prl.return_status = IFX_I2C_SESSION_ERROR;
                        p_ctx->prl.negotiation_state = PRL_NEGOTIATION_NOT_DONE;
                    }
                }
                else
                {
                    *p_ctx->prl.p_recv_payload_buffer_length -= 1;
                    p_ctx->prl.return_status = IFX_I2C_STACK_SUCCESS;
                    p_ctx->prl.state = PRL_STATE_IDLE;
                }
            }
            break;
            case PRL_STATE_ALERT:
            {
                if ((PRL_INTEGRITY_VIOLATED_ALERT_MSG | PRL_ALERT_PROTOCOL) == p_data[PRL_SCTR_OFFSET])
                {
                    if ((1 !=  data_len) || (p_ctx->prl.data_retransmit_counter >= PRL_TRANS_REPEAT))
                    {
                        ERROR_STATE_PREPARATION(p_ctx,exit_machine);
                        break;
                    }
                    else if ((MASTER_PROTECTION == p_ctx->protection_level) ||
                             (FULL_PROTECTION == p_ctx->protection_level))
                    {
                        p_ctx->prl.data_retransmit_counter++;
                        p_ctx->prl.trans_repeat_status = TRUE;
                        if (p_ctx->prl.data_retransmit_counter > PRL_TRANS_REPEAT)
                        {
                            ERROR_STATE_PREPARATION(p_ctx, exit_machine);
                            p_ctx->prl.state = PRL_STATE_IDLE;
                            break;
                        }
                        p_ctx->prl.alert_type = PRL_INTEGRITY_VIOLATED_ALERT_MSG;
                        p_ctx->prl.state = PRL_STATE_TXRX;
                    }
                    else
                    {
                      p_ctx->prl.state = PRL_STATE_ERROR;
                    }
                }
                else
                {
                    if (PRL_TRANS_REPEAT == p_ctx->prl.decryption_failure_counter)
                    {
                      ERROR_STATE_PREPARATION(p_ctx, exit_machine);
                    }
                    p_ctx->prl.state = PRL_STATE_ERROR;
                }
            }
            break;
            case PRL_STATE_ERROR:
            {
                if (PRL_INVALID_ALERT != p_ctx->prl.alert_type)
                {
                    return_status = ifx_i2c_prl_send_alert(p_ctx);
                    p_ctx->prl.alert_type = (uint8_t)PRL_INVALID_ALERT;
                    exit_machine = FALSE;
                }
                else
                {
                    p_ctx->prl.state = PRL_STATE_IDLE;
                }
            }
            break;
            default:
            {
                p_ctx->prl.state = PRL_STATE_IDLE;
                p_ctx->prl.upper_layer_event_handler(p_ctx, IFX_I2C_STACK_ERROR, 0, 0);
                exit_machine = FALSE;
            }
            break;
        }
    } while (TRUE == exit_machine);
}
#endif

/**
* @}
*/
