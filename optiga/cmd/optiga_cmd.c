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
* \file optiga_cmd.c
*
* \brief   This file implements cmd modules which covers OPTIGA command formation, locking mechanism, session
*          acquisition and asynchronous data handling
*
* \ingroup  grOptigaCmd
*
* @{
*/


#include "optiga/cmd/optiga_cmd.h"
#include "optiga/common/optiga_lib_common_internal.h"
#include "optiga/common/optiga_lib_logger.h"
#include "optiga/comms/optiga_comms.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal_os_lock.h"
#include "optiga/pal/pal_os_timer.h"
#include "optiga/pal/pal_os_memory.h"
#include "optiga/pal/pal_ifx_i2c_config.h"

//cmd byte for clearing last error code
#define OPTIGA_CMD_CLEAR_LAST_ERROR              (0x80)
//cmd byte for OpenApplication command
#define OPTIGA_CMD_OPEN_APPLICATION              (0x70 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for CloseApplication command
#define OPTIGA_CMD_CLOSE_APPLICATION             (0x71 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for GetDataObject command
#define OPTIGA_CMD_GET_DATA_OBJECT               (0x01 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for SetDataObject command
#define OPTIGA_CMD_SET_DATA_OBJECT               (0x02 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for GetRandom command
#define OPTIGA_CMD_GET_RANDOM                    (0x0C | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for CalcHash command
#define OPTIGA_CMD_CALC_HASH                     (0x30 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for CalcSign command
#define OPTIGA_CMD_CALC_SIGN                     (0x31 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for VerifySign command
#define OPTIGA_CMD_VERIFY_SIGN                   (0x32 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for CalcSsec command
#define OPTIGA_CMD_CALC_SSEC                     (0x33 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for DeriveKey command
#define OPTIGA_CMD_DERIVE_KEY                    (0x34 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for GenKeyPair command
#define OPTIGA_CMD_GEN_KEYPAIR                   (0x38 | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for EncryptAsym command
#define OPTIGA_CMD_ENCRYPT_ASYM                  (0x1E | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for DecryptAsym command
#define OPTIGA_CMD_DECRYPT_ASYM                  (0x1F | OPTIGA_CMD_CLEAR_LAST_ERROR)
//cmd byte for SetObjectProtected command
#define OPTIGA_CMD_SET_OBJECT_PROTECTED          (0x03 | OPTIGA_CMD_CLEAR_LAST_ERROR)


#define OPTIGA_CMD_MAX_NUMBER_OF_SESSIONS        (0x04)
#define OPTIGA_CMD_START_SESSION_OID             (0xE100)

// Considering the size w.r.t maximum APDU size + shielded connection if enabled.
#define OPTIGA_CMD_TOTAL_COMMS_BUFFER_SIZE       (OPTIGA_MAX_COMMS_BUFFER_SIZE + OPTIGA_COMMS_PRL_OVERHEAD)

#define OPTIGA_CMD_APDU_HEADER_SIZE              (0x04)

#define OPTIGA_CMD_LAST_ERROR_CODE               (0xF1C2)

#define OPTIGA_CMD_APDU_INDATA_OFFSET            (OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_COMMS_DATA_OFFSET)

#define OPTIGA_CMD_HASH_HEADER_SIZE              (0x06) //(hash_input_header_size + context_header_size)
// Intermediate context header size
#define OPTIGA_CMD_INTERMEDIATE_CONTEX_HEADER    (0x03)
// Zero length or value
#define OPTIGA_CMD_ZERO_LENGTH_OR_VALUE          (0x0000)
// OID data length
#define OPTIGA_CMD_OID_DATA_LENGTH               (0x0006)

//Number of bytes in tag
#define OPTIGA_CMD_NO_OF_BYTES_IN_TAG            (0x01)

//Number of bytes in OFFSET
#define OPTIGA_CMD_UINT16_SIZE_IN_BYTES          (0x02)

#define START_OPTIGA_COMMS_CLOSE                 (0x02)

#define OPTIGA_CMD_APDU_SUCCESS                  (0x00)
#define OPTIGA_CMD_APDU_FAILURE                  (0xFF)

#define CMD_WRITE_ONLY                           (0x00)
#define CMD_READ_DATA                            (0x00)
#define CMD_GET_DATA_OBJECT_NO_ERROR_CLEAR       (0x01)

//Calc sign tag values
#define CMD_SIGN_DIGEST_TAG                      (0x01)
#define CMD_SIGN_OID_TAG                         (0x03)
#define CMD_SIGN_OID_LEN                         (0x0002)

//Calc SSec tag values
#define CMD_SSEC_PRIVATE_KEY_TAG                 (0x01)
#define CMD_SSEC_PRIVATE_KEY_LEN                 (0x0002)
#define CMD_SSEC_ALG_ID_TAG                      (0x05)
#define CMD_SSEC_ALG_ID_LEN                      (0x01)
#define CMD_SSEC_PUB_KEY_TAG                     (0x06)
#define CMD_SSEC_EXPORT_TAG                      (0x07)
#define CMD_SSEC_EXPORT_LEN                      (0x0000)
#define CMD_SSEC_STORE_SESSION_TAG               (0x08)
#define CMD_SSEC_STORE_SESSION_LEN               (0x0002)

//Verify sign tag values
#define CMD_VERIFY_SIGN_DIGEST_TAG               (0x01)
#define CMD_VERIFY_SIGN_SIGNATURE_TAG            (0x02)
#define CMD_VERIFY_SIGN_PUB_KEY_CERT_OID_TAG     (0x04)
#define CMD_VERIFY_SIGN_PUB_KEY_CERT_OID_LENGTH  (0x0002)
#define CMD_VERIFY_ALGO_ID_TAG                   (0x05)
#define CMD_VERIFY_ALGO_ID_LENGTH                (0x0001)
#define CMD_VERIFY_PUBLIC_KEY_TAG                (0x06)

//Generate key pair tag and length value
#define CMD_GEN_KEY_PAIR_PRIVATE_KEY_OID_TAG     (0x01)
#define CMD_GEN_KEY_PAIR_PRIVATE_KEY_OID_LENGTH  (0x0002)
#define CMD_GEN_KEY_PAIR_KEY_USAGE_TAG           (0x02)
#define CMD_GEN_KEY_PAIR_KEY_USAGE_LENGTH        (0x0001)
#define CMD_GEN_KEY_PAIR_EXPORT_KEY_TAG          (0x07)
#define CMD_GEN_KEY_PAIR_EXPORT_KEY_LENGTH       (0x0000)
#define CMD_GEN_KEY_PAIR_PUBLIC_KEY_TAG          (0x02)
#define CMD_GEN_KEY_PAIR_PRIVATE_KEY_TAG         (0x01)

//Derive key tag value
#define CMD_DERIVE_KEY_SEC_OID_TAG               (0x01)
#define CMD_DERIVE_KEY_SEC_OID_TAG_LENGTH        (0x0002)
#define CMD_DERIVE_KEY_DERIVATION_DATA_TAG       (0x02)
#define CMD_DERIVE_KEY_KEY_LEN_TAG_LENGTH        (0x0002)
#define CMD_DERIVE_KEY_KEY_LEN_TAG               (0x03)
#define CMD_DERIVE_KEY_EXPORT_TAG                (0x07)
#define CMD_DERIVE_KEY_STORE_TAG                 (0x08)
#define CMD_DERIVE_KEY_STORE_TAG_LENGTH          (0x0002)
#define CMD_DERIVE_KEY_DERIVE_KEY_LEN_MIN        (0x10)

//Encrypt asym tag values
#define CMD_ENCRYPT_ASYM_SESSION_OID_TAG         (0x02)
#define CMD_ENCRYPT_ASYM_SESSION_OID_LENGTH      (0x0002)
#define CMD_ENCRYPT_ASYM_MESSAGE_TAG             (0x61)
#define CMD_ENCRYPT_ASYM_PUB_KEY_CERT_OID_TAG    (0x04)
#define CMD_ENCRYPT_ASYM_PUB_KEY_CERT_OID_LENGTH (0x0002)
#define CMD_ENCRYPT_ASYM_ALGO_ID_TAG             (0x05)
#define CMD_ENCRYPT_ASYM_ALGO_ID_LENGTH          (0x0001)
#define CMD_ENCRYPT_ASYM_PUBLIC_KEY_TAG          (0x06)

//Decrypt asym key tag value
#define CMD_DECRYPT_ASYM_ENCRYPT_MESSAGE_TAG     (0x61)
#define CMD_DECRYPT_ASYM_DECRYPT_KEY_TAG         (0x03)
#define CMD_DECRYPT_ASYM_DECRYPT_KEY_LENGTH      (0x0002)
#define CMD_DECRYPT_ASYM_SESSION_OID_TAG         (0x02)
#define CMD_DECRYPT_ASYM_SESSION_OID_LENGTH      (0x0002)

#define OPTIGA_PROTECTION_LEVEL_MASK             (0x03)

#define OPTIGA_CMD_ERROR_CODE_PREPARE            (0x01)
#define OPTIGA_CMD_ERROR_CODE_TX                 (0x02)
#define OPTIGA_CMD_ERROR_CODE_RX                 (0x03)
#define OPTIGA_CMD_ERROR_CODE_STATE_MASK         (0x03)
#define OPTIGA_CMD_ENTER_HANDLER_CALL_MASK       (0x80)
#define OPTIGA_CMD_ENTER_HANDLER_CALL            (0x80)
#define OPTIGA_CMD_EXIT_HANDLER_CALL             (0x00)
#define OPTIGA_CMD_OUT_OF_BOUNDARY_ERROR         (0x08)

//RSA pre master optional data tag and length value
#define CMD_RSA_PRE_MASTER_OPTIONAL_DATA_TAG     (0x41)

/// Param type for optiga pre master secret
#define CMD_RANDOM_PARAM_TYPE_PRE_MASTER_SECRET  (0x04)

/// Set object protected tag
#define CMD_SET_OBJECT_PROTECTED_TAG             (0x30)

#define OPTIGA_CMD_APDU_TL_LENGTH                (0x03)
#define OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT  (0x00)
// Function to set given bits
#define SET_DEV_ERROR_HANDLER_STATE(state){\
                                   (me->device_error_status) &= ~OPTIGA_CMD_ERROR_CODE_STATE_MASK;\
                                   (me->device_error_status) |= state & OPTIGA_CMD_ERROR_CODE_STATE_MASK;}

#define SET_DEV_ERROR_NOTIFICATION(bits_value){\
                                  (me->device_error_status) &= ~OPTIGA_CMD_ENTER_HANDLER_CALL_MASK;\
                                  (me->device_error_status) |= bits_value & OPTIGA_CMD_ENTER_HANDLER_CALL_MASK;}


#define EXIT_STATE_WITH_ERROR(ctx, exit_machine){\
                              ctx->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;\
                              ctx->exit_status = OPTIGA_CMD_ERROR;\
                              exit_machine = FALSE;}
// Scheduler definitions

// Optiga execution slot states
#define     OPTIGA_CMD_QUEUE_NOT_ASSIGNED           (0x00)
#define     OPTIGA_CMD_QUEUE_ASSIGNED               (0x01)
#define     OPTIGA_CMD_QUEUE_REQUEST                (0x02)
#define     OPTIGA_CMD_QUEUE_PROCESSING             (0x04)
#define     OPTIGA_CMD_QUEUE_RESUME                 (0x08)

// Optiga session state
#define     OPTIGA_CMD_SESSION_NOT_ASSIGNED         (0x00)
#define     OPTIGA_CMD_SESSION_ASSIGNED             (0x10)
//Session not assigned to optiga cmd instance
#define     OPTIGA_CMD_NO_SESSION_OID               (0x0000)
#define     OPTIGA_CMD_ALL_SESSION_ASSIGNED         (0x10101010)

// Optiga slot request types
#define     OPTIGA_CMD_QUEUE_REQUEST_LOCK           (0x21)
#define     OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK    (0x23)
#define     OPTIGA_CMD_QUEUE_REQUEST_SESSION        (0x22)
#define     OPTIGA_CMD_QUEUE_NO_REQUEST	            (0x00)

// Type of slot, Do not change the values
#define     OPTIGA_CMD_QUEUE_SLOT_STATE             (0x09)
// Type of lock, Do not change the values
#define     OPTIGA_CMD_QUEUE_SLOT_LOCK_TYPE         (0x08)
// Frequency of scheduler polling when no asynchronous requests are pending
#define     OPTIGA_CMD_SCHEDULER_IDLING_TIME_MS     (1000U)
// Frequency of scheduler polling when asynchronous requests is being processed
#define     OPTIGA_CMD_SCHEDULER_RUNNING_TIME_MS    (50U)

/** \brief The enum represents diffrent state of command handler */
typedef enum optiga_cmd_state
{
    OPTIGA_CMD_EXEC_COMMS_OPEN = 0,
    OPTIGA_CMD_EXEC_COMMS_OPEN_START,
    OPTIGA_CMD_EXEC_COMMS_OPEN_DONE,

    OPTIGA_CMD_EXEC_COMMS_CLOSE_START,
    OPTIGA_CMD_EXEC_COMMS_CLOSE_DONE,

    OPTIGA_CMD_EXEC_REQUEST_SESSION,
    OPTIGA_CMD_EXEC_REQUEST_LOCK,
    OPTIGA_CMD_EXEC_REQUEST_STRICT_LOCK,
    OPTIGA_CMD_EXEC_PREPARE_COMMAND,
    OPTIGA_CMD_EXEC_PROCESS_RESPONSE,
    OPTIGA_CMD_EXEC_GET_DEVICE_ERROR,
    OPTIGA_CMD_EXEC_RELEASE_LOCK,
    OPTIGA_CMD_STATE_EXIT

} optiga_cmd_state_t;

typedef optiga_lib_status_t (*optiga_cmd_handler_t)(optiga_cmd_t * me);

/** \brief The structure represents the slot in the execution queue */
typedef struct optiga_cmd_queue_slot
{
    // Registered context
    void * registered_ctx;
    // Arrival time of the optiga cmd instance in the execution queue
    uint32_t arrival_time;
    // Request for command lock or session
    uint8_t request_type;
    // state of the slot
    uint8_t state_of_entry;
}optiga_cmd_queue_slot_t;


/**
* \brief OPTIGA Context which holds the communication buffer, comms instance and other required.
*   This would be maintained and consumed by OPTIGA Cmd.
*/
struct optiga_context
{
    /// Context of OPTIGA Communication.
    optiga_comms_t * p_optiga_comms;
    /// comms tx size
    uint16_t comms_tx_size;
    /// comms rx size
    uint16_t comms_rx_size;
    /// Structure which maintains the session and contexts of requesters to acquire session.
    uint8_t sessions[OPTIGA_CMD_MAX_NUMBER_OF_SESSIONS];
    /// Indicates if instance is initialized
    uint8_t instance_init_state;
    /// Communication buffer to send/receive APDUs.
    uint8_t optiga_comms_buffer[OPTIGA_CMD_TOTAL_COMMS_BUFFER_SIZE];
    /// optiga execution queue
    optiga_cmd_queue_slot_t optiga_cmd_execution_queue[OPTIGA_CMD_MAX_REGISTRATIONS];
    /// pal os event instance/context
    pal_os_event_t * p_pal_os_event_ctx;
    /// Last processed cmd time stamp
    uint32_t last_time_stamp;
    /// optiga context handle buffer
    uint8_t optiga_context_handle_buffer[APP_CONTEXT_SIZE];
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    /// Protection level status flag
    uint8_t protection_level_state;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
};

// static instance of optiga
_STATIC_H optiga_context_t g_optiga = {0};
// List of optiga instances
_STATIC_H optiga_context_t * g_optiga_list[] = {&g_optiga};
// hibernate data store for each instance of optiga
_STATIC_H uint16_t g_hibernate_datastore_id_list[] = {OPTIGA_HIBERNATE_CONTEXT_ID};

const uint8_t g_optiga_unique_application_identifier[] =
{
    0xD2, 0x76, 0x00, 0x00, 0x04, 0x47, 0x65, 0x6E, 0x41, 0x75, 0x74, 0x68, 0x41, 0x70, 0x70, 0x6C,
};

/**
* \brief Command context
*/
struct optiga_cmd
{
    /// Pointer to OPTIGA context which holds the communication buffer, comms instance and other required.
    optiga_context_t * p_optiga;
    /// Internal handlers for prepare and process
    optiga_cmd_handler_t cmd_hdlrs;
    /// Command execution input parameters
    void * p_input;

    /// Caller context
    void* caller_context;
    /// Callback handler
    callback_handler_t handler;
    /// Holds a Session OID allotted to this instance.
    uint16_t session_oid;
    /// State of the command next execution state
    optiga_cmd_state_t cmd_next_execution_state;
    /// Chaining flag
    uint8_t chaining_ongoing;
    /// Param value for the respective command to be processed
    uint8_t cmd_param;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    /// To provide the encryption and decryption need for command and response
    uint8_t protection_level;
    /// To provide the presentation layer protocol version to be used
    uint8_t protocol_version;
    /// To provide the manage context options
    uint8_t manage_context_operation;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
    /// To provide error state
    uint8_t device_error_status;
    /// Assigned slot from execution queue
    uint8_t queue_id;
    /// Exit status value
    optiga_lib_status_t exit_status;
    /// Datastore ID for optiga context
    uint16_t optiga_context_datastore_id;
};

_STATIC_H optiga_lib_status_t optiga_cmd_get_error_code_handler(optiga_cmd_t * me);

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
//lint --e{714} suppress "This function is defined here but referred from other modules"
void optiga_cmd_set_shielded_connection_option(optiga_cmd_t * me,
                                               uint8_t value,
                                               uint8_t shielded_connection_option)
{
    switch (shielded_connection_option)
    {
        // Protection Level
        case OPTIGA_SET_PROTECTION_LEVEL:
        {
            me->protection_level = value;
        }
        break;
        // Protocol Version
        case OPTIGA_SET_PROTECTION_VERSION:
        {
            me->protocol_version = value;
        }
        break;
        // Manage Context
        case OPTIGA_SET_MANAGE_CONTEXT:
        {
            me->manage_context_operation = value;
        }
        break;
        default:
        break;
    }
}

#endif // OPTIGA_COMMS_SHIELDED_CONNECTION

// Cmd execute handler
_STATIC_H void optiga_cmd_execute(optiga_cmd_t * me,
                                  uint8_t cmd_param,
                                  optiga_cmd_handler_t cmd_hdlrs,
                                  optiga_cmd_state_t start_state,
                                  void * input);

// Cmd State machine handler
_STATIC_H void optiga_cmd_execute_handler(void * p_ctx,
                                          optiga_lib_status_t event);

//
_STATIC_H void optiga_cmd_prepare_apdu_header(uint8_t cmd, uint8_t param,
                                              uint16_t in_data_length,
                                              uint8_t * p_apdu_buffer)
{
    p_apdu_buffer [0] = cmd;
    p_apdu_buffer [1] = param;
    p_apdu_buffer [2] = (uint8_t)((in_data_length & 0xFF00) >> 8);
    p_apdu_buffer [3] = (uint8_t)(in_data_length & 0x00FF);
}


_STATIC_H void optiga_cmd_prepare_tag_header(uint8_t tag,
                                             uint16_t tag_length,
                                             uint8_t * buffer,
                                             uint16_t * position)
{
    uint16_t start_position = *position;

    buffer[start_position++] = tag;
    buffer[start_position++] = (uint8_t )(tag_length >> 8);
    buffer[start_position++] = (uint8_t )(tag_length);

    *position = start_position;
}


_STATIC_H void optiga_cmd_event_trigger_execute(void * p_ctx)
{
    optiga_cmd_execute_handler(p_ctx, OPTIGA_LIB_SUCCESS);
}

_STATIC_H void optiga_cmd_execute(optiga_cmd_t * me,
                                  uint8_t cmd_param,
                                  optiga_cmd_handler_t cmd_hdlrs,
                                  optiga_cmd_state_t start_state,
                                  void * input)
{
    me->p_input = input;
    me->cmd_next_execution_state = start_state;
    me->cmd_hdlrs = cmd_hdlrs;
    me->chaining_ongoing = FALSE;
    me->cmd_param = cmd_param;
    optiga_cmd_execute_handler(me, OPTIGA_LIB_SUCCESS);
}

/*
* Checks if optiga session is available or not
* Returns TRUE, if slot is available
* Returns FALSE, if slot is not available
*/
_STATIC_H bool_t optiga_cmd_session_available(const optiga_context_t * p_optiga)
{
    uint32_t status_check;
    // Consider the array as uin32_t value and check against 0x10101010
    // where 0x10 is value of OPTIGA_CMD_SESSION_ASSIGNED
    memcpy(&status_check, p_optiga->sessions, OPTIGA_CMD_MAX_NUMBER_OF_SESSIONS);
    return ((status_check < OPTIGA_CMD_ALL_SESSION_ASSIGNED)? (TRUE):(FALSE));
}

/*
* 1. If a optiga cmd instance does not have session, assigns an available session
*/
_STATIC_H void optiga_cmd_session_assign(optiga_cmd_t * me)
{
    uint8_t * p_optiga_sessions = me->p_optiga->sessions;
    uint8_t count;
    if (OPTIGA_CMD_NO_SESSION_OID == me->session_oid)
    {
        for (count = 0; count < OPTIGA_CMD_MAX_NUMBER_OF_SESSIONS; count++)
        {
            if (OPTIGA_CMD_SESSION_ASSIGNED != p_optiga_sessions[count])
            {
                me->session_oid = (OPTIGA_CMD_START_SESSION_OID | count);
                p_optiga_sessions[count] = OPTIGA_CMD_SESSION_ASSIGNED;
                break;
            }
        }
    }
}

/*
* Frees a session
*/
_STATIC_H void optiga_cmd_session_free(optiga_cmd_t * me)
{
    uint8_t * p_optiga_sessions = me->p_optiga->sessions;
    uint8_t count;
    if (OPTIGA_CMD_NO_SESSION_OID != me->session_oid)
    {
        count = me->session_oid & 0x0F;
        me->session_oid = OPTIGA_CMD_NO_SESSION_OID;
        p_optiga_sessions[count] = OPTIGA_CMD_SESSION_NOT_ASSIGNED;
    }
}

/*
*  Returns the requested info in the queue slot input cmd instance
*/
_STATIC_H  uint8_t optiga_cmd_queue_get_state_of(const optiga_cmd_t * me, uint8_t slot_member)
{
    uint8_t state = 0;
    switch (slot_member)
    {
        case OPTIGA_CMD_QUEUE_SLOT_LOCK_TYPE:
        {
            state = me->p_optiga->optiga_cmd_execution_queue[me->queue_id].request_type;
        }
        break;
        case OPTIGA_CMD_QUEUE_SLOT_STATE:
        {
            state = me->p_optiga->optiga_cmd_execution_queue[me->queue_id].state_of_entry;
        }
        break;
        default:
            break;
    }
    return (state);
}

/*
* Returns the count of number of slots with requested state
*/
_STATIC_H  uint8_t optiga_cmd_queue_get_count_of(const optiga_context_t * p_optiga, uint8_t slot_member, uint8_t state_to_check)
{
    uint8_t index;
    uint8_t count = 0;
    uint8_t slot_value = 0;
    for (index = 0; index < OPTIGA_CMD_MAX_REGISTRATIONS ; index++)
    {
        switch (slot_member)
        {
            case OPTIGA_CMD_QUEUE_SLOT_LOCK_TYPE:
            {
                slot_value = p_optiga->optiga_cmd_execution_queue[index].request_type;
            }
            break;
            case OPTIGA_CMD_QUEUE_SLOT_STATE:
            {
                slot_value = p_optiga->optiga_cmd_execution_queue[index].state_of_entry;
            }
            break;
            default:
                break;
        }
        if (state_to_check == slot_value)
        {
            count++;
        }
    }
    return (count);
}

/*
* Assigns an available slot to a optiga cmd instance and marks the slot as not available for another optiga cmd instance
*/
_STATIC_H void optiga_cmd_queue_assign_slot(const optiga_cmd_t * me, uint8_t * queue_index_store)
{
    uint8_t index;
    for (index = 0; index < OPTIGA_CMD_MAX_REGISTRATIONS ; index++)
    {
        if (OPTIGA_CMD_QUEUE_NOT_ASSIGNED == me->p_optiga->optiga_cmd_execution_queue[index].state_of_entry)
        {
            * queue_index_store =  index;
            me->p_optiga->optiga_cmd_execution_queue[index].state_of_entry = OPTIGA_CMD_QUEUE_ASSIGNED;
           break;
        }
    }
}


/*
* De-assigns a slot from a optiga cmd instance and makes the slot available for next optiga cmd instance
*/
_STATIC_H void optiga_cmd_queue_deassign_slot(optiga_cmd_t * me)
{
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].state_of_entry = OPTIGA_CMD_QUEUE_NOT_ASSIGNED;
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].request_type = OPTIGA_CMD_QUEUE_NO_REQUEST;
    me->queue_id = 0;
}


/*
* Select next optiga cmd instance from the execution queue based on a rule
* 1. A slot with OPTIGA_CMD_QUEUE_RESUME state should exist
* 2. Pick the slot which has acquired the strict slot
* 3. If no slot with OPTIGA_CMD_QUEUE_RESUME exists, slot must be in OPTIGA_CMD_QUEUE_REQUEST state
* 4. The arrival time must be the earliest provided
*     a. The request type is lock
*     b. If request type is session, either session is already assigned or atleast session is available for assignment
*/
_STATIC_H void optiga_cmd_queue_scheduler(void * p_optiga)
{
    uint32_t reference_time_stamp = 0xFFFFFFFF;
    optiga_cmd_queue_slot_t * p_queue_entry;
    uint8_t index;
    uint8_t prefered_index = 0xFF;
    uint8_t overflow_detected = FALSE;

    optiga_context_t * p_optiga_ctx = (optiga_context_t * )p_optiga;

    pal_os_event_t * my_os_event = p_optiga_ctx->p_pal_os_event_ctx;


    if (((0 == optiga_cmd_queue_get_count_of(p_optiga_ctx, OPTIGA_CMD_QUEUE_SLOT_STATE, OPTIGA_CMD_QUEUE_REQUEST)) &&
         (0 == optiga_cmd_queue_get_count_of(p_optiga_ctx, OPTIGA_CMD_QUEUE_SLOT_STATE, OPTIGA_CMD_QUEUE_RESUME))) ||
         ((1 == optiga_cmd_queue_get_count_of(p_optiga_ctx, OPTIGA_CMD_QUEUE_SLOT_STATE , OPTIGA_CMD_QUEUE_PROCESSING)) &&
         (0 < optiga_cmd_queue_get_count_of(p_optiga_ctx, OPTIGA_CMD_QUEUE_SLOT_LOCK_TYPE, OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK))))
    {
        // call self
        pal_os_event_register_callback_oneshot(my_os_event, optiga_cmd_queue_scheduler,
                                               p_optiga_ctx,OPTIGA_CMD_SCHEDULER_IDLING_TIME_MS);
    }
    else
    {
        pal_os_event_stop(my_os_event);
        // continue checking if no context selected and overflow detected
        do
        {
            //reset overflow detected flag and the last_time stamp
            if(overflow_detected == TRUE)
            {
                p_optiga_ctx->last_time_stamp = 0;
                overflow_detected = FALSE;
            }

            // Select optiga command based on rule
            for (index = 0; index < OPTIGA_CMD_MAX_REGISTRATIONS; index++)
            {
                p_queue_entry = &(p_optiga_ctx->optiga_cmd_execution_queue[index]);

                // if any slot has acquired strict lock, highest priority is given to it
                if (1 == optiga_cmd_queue_get_count_of(p_optiga_ctx, OPTIGA_CMD_QUEUE_SLOT_STATE , OPTIGA_CMD_QUEUE_RESUME))
                {
                    // Select the slot which has acquired strict lock
                    if ((OPTIGA_CMD_QUEUE_RESUME == p_queue_entry->state_of_entry) &&
                        (OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK == p_queue_entry->request_type))
                    {
                        reference_time_stamp = p_queue_entry->arrival_time;
                        prefered_index = index;
                    }

                }
                else
                {
                    // pick only requested queue slot and earliest arrival time
                    if (p_queue_entry->state_of_entry == OPTIGA_CMD_QUEUE_REQUEST)
                    {
                        // remember that overflow has occurred in one of the entry
                        if (p_queue_entry->arrival_time < p_optiga_ctx->last_time_stamp)
                        {
                            overflow_detected = TRUE;
                        }

                        // if lock request or session request and session available(either already assigned or available)
                        if (((p_queue_entry->arrival_time <= reference_time_stamp) && (p_queue_entry->arrival_time >= p_optiga_ctx->last_time_stamp)) &&
                            (((OPTIGA_CMD_QUEUE_REQUEST_SESSION == p_queue_entry->request_type) && (TRUE == optiga_cmd_session_available(p_optiga_ctx))) ||
                            ((OPTIGA_CMD_QUEUE_REQUEST_SESSION == p_queue_entry->request_type) && (OPTIGA_CMD_NO_SESSION_OID != ((optiga_cmd_t *)p_queue_entry->registered_ctx)->session_oid)) ||
                            (OPTIGA_CMD_QUEUE_REQUEST_LOCK == p_queue_entry->request_type) ||
                            (OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK == p_queue_entry->request_type)))
                        {
                            reference_time_stamp = p_queue_entry->arrival_time;
                            prefered_index = index;
                        }
                    }


                }
            }
        } while((0xFF == prefered_index) && (TRUE == overflow_detected));

        // Improve : check the index and max queue size check
        // If slot is identified then go further
        if (0xFF != prefered_index)
        {
            p_queue_entry = &(p_optiga_ctx->optiga_cmd_execution_queue[prefered_index]);
            // assign session
            if ((OPTIGA_CMD_QUEUE_REQUEST_SESSION == p_optiga_ctx->optiga_cmd_execution_queue[prefered_index].request_type) &&
                (OPTIGA_CMD_NO_SESSION_OID == ((optiga_cmd_t *)p_queue_entry->registered_ctx)->session_oid))
            {
                optiga_cmd_session_assign((optiga_cmd_t *)(p_optiga_ctx->optiga_cmd_execution_queue[prefered_index].registered_ctx));
                // Improve : Change the state of the type here. This will reduce 0x0000 check
            }

            // schedule with selected context
            my_os_event = ((optiga_cmd_t *)(p_optiga_ctx->optiga_cmd_execution_queue[prefered_index].registered_ctx))->p_optiga->p_pal_os_event_ctx;
            pal_os_event_register_callback_oneshot(my_os_event,
                                                   optiga_cmd_event_trigger_execute,
                                                   ((optiga_cmd_t *)(p_optiga_ctx->optiga_cmd_execution_queue[prefered_index].registered_ctx)),
                                                   OPTIGA_CMD_SCHEDULER_RUNNING_TIME_MS);
            p_optiga_ctx->optiga_cmd_execution_queue[prefered_index].state_of_entry = OPTIGA_CMD_QUEUE_PROCESSING;
            p_optiga_ctx->last_time_stamp = reference_time_stamp;
        }
        else
        {
            pal_os_event_register_callback_oneshot( my_os_event, optiga_cmd_queue_scheduler,
                                                    p_optiga_ctx,OPTIGA_CMD_SCHEDULER_IDLING_TIME_MS);
        }
    }
}

/*
* Updates a execution queue slot
*/
_STATIC_H void optiga_cmd_queue_update_slot(optiga_cmd_t * me, uint8_t request_type)
{
    if ((OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK != me->p_optiga->optiga_cmd_execution_queue[me->queue_id].request_type) ||
       ((OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK == me->p_optiga->optiga_cmd_execution_queue[me->queue_id].request_type) &&
       (OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK != request_type)))
    {
        //add time stamp
        me->p_optiga->optiga_cmd_execution_queue[me->queue_id].arrival_time = pal_os_timer_get_time_in_microseconds();
    }
    //add optiga_cmd ctx
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].registered_ctx = (void * )me;
    // set the state of slot to Requested state
    if ((OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK == me->p_optiga->optiga_cmd_execution_queue[me->queue_id].request_type) &&
        (OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK == request_type))
    {
        me->p_optiga->optiga_cmd_execution_queue[me->queue_id].state_of_entry = OPTIGA_CMD_QUEUE_RESUME;
    }
    else
    {
        me->p_optiga->optiga_cmd_execution_queue[me->queue_id].state_of_entry = OPTIGA_CMD_QUEUE_REQUEST;
    }
    //add request type
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].request_type = request_type;
}

/*
* Resets a execution slot
*/
_STATIC_H void optiga_cmd_queue_reset_slot(const optiga_cmd_t * me)
{
    // Reset the arrival time
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].arrival_time = 0xFFFFFFFF;
    //add optiga_cmd ctx
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].registered_ctx = NULL;
    //add request type
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].request_type = 0;
    // set the slot state to assigned
    me->p_optiga->optiga_cmd_execution_queue[me->queue_id].state_of_entry = OPTIGA_CMD_QUEUE_ASSIGNED;
    // start the event scheduler
    pal_os_event_start(me->p_optiga->p_pal_os_event_ctx, optiga_cmd_queue_scheduler, me->p_optiga);
}

optiga_lib_status_t optiga_cmd_request_session(optiga_cmd_t * me)
{
    optiga_cmd_queue_update_slot(me , OPTIGA_CMD_QUEUE_REQUEST_SESSION);
    return (OPTIGA_CMD_SUCCESS);
}

optiga_lib_status_t optiga_cmd_release_session(optiga_cmd_t * me)
{
    optiga_cmd_session_free(me);
    return (OPTIGA_CMD_SUCCESS);
}

optiga_lib_status_t optiga_cmd_request_lock(optiga_cmd_t * me, uint8_t lock_type)
{
    optiga_cmd_queue_update_slot(me , lock_type);
    return (OPTIGA_CMD_SUCCESS);
}

optiga_lib_status_t optiga_cmd_release_lock(const optiga_cmd_t * me)
{
    optiga_cmd_queue_reset_slot(me);
    return (OPTIGA_CMD_SUCCESS);
}

_STATIC_H optiga_lib_status_t optiga_cmd_restore_context(const optiga_cmd_t * me)
{
    uint16_t context_handle_length;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    do
    {
        if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != me->optiga_context_datastore_id)
        {
            context_handle_length = sizeof(me->p_optiga->optiga_context_handle_buffer);
            //Reading context handle secret from datastore
            return_status = pal_os_datastore_read(me->optiga_context_datastore_id,
                                                  me->p_optiga->optiga_context_handle_buffer,
                                                  &context_handle_length);
            if (PAL_STATUS_SUCCESS != return_status)
            {
                break;
            }
        }
        if (0 == optiga_common_get_uint32(me->p_optiga->optiga_context_handle_buffer) &&
            0 == optiga_common_get_uint32(&me->p_optiga->optiga_context_handle_buffer[4]))
        {
            return_status = OPTIGA_CMD_ERROR;
            break;
        }

        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    return (return_status);
}
_STATIC_H optiga_lib_status_t optiga_cmd_store_context(const optiga_cmd_t * me)
{
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    do
    {
        if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != me->optiga_context_datastore_id)
        {
            //Reading context handle secret from datastore
            return_status = pal_os_datastore_write(me->optiga_context_datastore_id,
                                                   me->p_optiga->optiga_context_handle_buffer,
                                                   sizeof(me->p_optiga->optiga_context_handle_buffer));
            if (PAL_STATUS_SUCCESS != return_status)
            {
                break;
            }
        }

        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    return (return_status);
}

_STATIC_H void optiga_cmd_execute_handler(void * p_ctx, optiga_lib_status_t event)
{
    optiga_context_t * p_optiga;
    uint8_t exit_loop = TRUE;
    optiga_cmd_t * me = (optiga_cmd_t *)p_ctx;
    optiga_cmd_handler_t optiga_cmd_handler = me->cmd_hdlrs;
    p_optiga = me->p_optiga;

    // in event of no success, release lock and exit
    if (OPTIGA_LIB_SUCCESS != event)
    {
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        /*In the case of hibernate if any low level error occurs while performing
          close application this section clears the saved application context.*/
        if ((OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT != me->cmd_param) &&
            (OPTIGA_COMMS_SESSION_CONTEXT_SAVE == me->manage_context_operation))
        {
            pal_os_memset(me->p_optiga->optiga_context_handle_buffer,0,
                            sizeof(me->p_optiga->optiga_context_handle_buffer));
            if ((OPTIGA_HIBERNATE_CONTEXT_ID == me->optiga_context_datastore_id) &&
                (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != me->optiga_context_datastore_id))
            {
                //Clearing context handle secret from datastore
                me->exit_status = pal_os_datastore_write(me->optiga_context_datastore_id,
                                                         me->p_optiga->optiga_context_handle_buffer,
                                                         sizeof(me->p_optiga->optiga_context_handle_buffer));
                if (PAL_STATUS_SUCCESS != me->exit_status)
                {
                    me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
                }
            }
        }
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
        me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
        me->exit_status = event;
    }

    do
    {
        switch (me->cmd_next_execution_state)
        {
            case OPTIGA_CMD_EXEC_COMMS_OPEN:
            {
                // add to queue and exit
                me->exit_status = optiga_cmd_request_lock(me, OPTIGA_CMD_QUEUE_REQUEST_LOCK);
                if (OPTIGA_LIB_SUCCESS != me->exit_status)
                {
                    EXIT_STATE_WITH_ERROR(me,exit_loop);
                    break;
                }

                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_COMMS_OPEN_START;
                break;
            }
            case OPTIGA_CMD_EXEC_COMMS_OPEN_START:
            {
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
                me->p_optiga->p_optiga_comms->protection_level =  me->protection_level;
                me->p_optiga->p_optiga_comms->protocol_version = me->protocol_version;
                me->p_optiga->p_optiga_comms->manage_context_operation = me->manage_context_operation;
                me->p_optiga->p_optiga_comms->p_pal_os_event_ctx = me->p_optiga->p_pal_os_event_ctx;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION

                (void)optiga_comms_set_callback_context(p_optiga->p_optiga_comms, me);
                me->exit_status = optiga_comms_open(me->p_optiga->p_optiga_comms);

                if (OPTIGA_LIB_SUCCESS != me->exit_status)
                {
                    EXIT_STATE_WITH_ERROR(me,exit_loop);
                    break;
                }

                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_COMMS_OPEN_DONE;
                break;
            }
            case OPTIGA_CMD_EXEC_COMMS_OPEN_DONE:
            {
                pal_os_event_register_callback_oneshot(me->p_optiga->p_pal_os_event_ctx,
                                                       (register_callback)optiga_cmd_event_trigger_execute,
                                                       me, OPTIGA_CMD_SCHEDULER_RUNNING_TIME_MS);
                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PREPARE_COMMAND;
                break;
            }
            case OPTIGA_CMD_EXEC_COMMS_CLOSE_START:
            {
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
                // In case PRL is enabled and save context is not invoked or is PRL is not enabled,
                //change state to OPTIGA_CMD_EXEC_COMMS_CLOSE_DONE, since it synchronous in nature
                if (OPTIGA_COMMS_SESSION_CONTEXT_SAVE != me->manage_context_operation)
                {
                    me->cmd_next_execution_state = OPTIGA_CMD_EXEC_COMMS_CLOSE_DONE;
                }
                me->p_optiga->p_optiga_comms->protection_level =  me->protection_level;
                me->p_optiga->p_optiga_comms->protocol_version = me->protocol_version;
                if (0 != me->p_optiga->protection_level_state)
                {
                    me->p_optiga->p_optiga_comms->manage_context_operation = me->manage_context_operation;
                }
                else
                {
                    me->p_optiga->p_optiga_comms->manage_context_operation = OPTIGA_COMMS_SESSION_CONTEXT_NONE;
                }
                me->p_optiga->protection_level_state = 0;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_COMMS_CLOSE_DONE;

                me->p_optiga->p_optiga_comms->p_pal_os_event_ctx = me->p_optiga->p_pal_os_event_ctx;

                (void)optiga_comms_set_callback_context(p_optiga->p_optiga_comms, me);
                me->exit_status = optiga_comms_close(me->p_optiga->p_optiga_comms);

                if (OPTIGA_LIB_SUCCESS != me->exit_status)
                {
                    EXIT_STATE_WITH_ERROR(me,exit_loop);
                    break;
                }

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
                // In case PRL is enabled and save context is requested,
                //change state to OPTIGA_CMD_EXEC_COMMS_CLOSE_DONE, since it asynchronous in nature
                if (OPTIGA_COMMS_SESSION_CONTEXT_SAVE == me->manage_context_operation)
                {
                    me->cmd_next_execution_state = OPTIGA_CMD_EXEC_COMMS_CLOSE_DONE;
                    exit_loop = TRUE;
                }
                // For synchronous behavior: After exit from optiga_comms_close, release lock
                else
                {
                    me->exit_status = optiga_cmd_release_lock(me);
                }
#else
                me->exit_status = optiga_cmd_release_lock(me);
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
                break;
            }
            case OPTIGA_CMD_EXEC_COMMS_CLOSE_DONE:
            {
                me->handler(me->caller_context, event);
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
                // For asynchronous behavior, change state to release the lock
                if (OPTIGA_COMMS_SESSION_CONTEXT_SAVE == me->manage_context_operation)
                {
                    me->exit_status = optiga_cmd_release_lock(me);
                }
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
                break;
            }
            case OPTIGA_CMD_EXEC_REQUEST_SESSION:
            case OPTIGA_CMD_EXEC_REQUEST_LOCK:
            {
                if (me->cmd_next_execution_state == OPTIGA_CMD_EXEC_REQUEST_SESSION)
                {
                    me->exit_status = optiga_cmd_request_session(me);
                }
                else
                {
                    me->exit_status = optiga_cmd_request_lock(me, OPTIGA_CMD_QUEUE_REQUEST_LOCK);
                }
                if (OPTIGA_LIB_SUCCESS != me->exit_status)
                {
                    EXIT_STATE_WITH_ERROR(me,exit_loop);
                    break;
                }

                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PREPARE_COMMAND;
                break;
            }
            case OPTIGA_CMD_EXEC_REQUEST_STRICT_LOCK:
            {
                me->exit_status = optiga_cmd_request_lock(me, OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK);
                if (OPTIGA_LIB_SUCCESS != me->exit_status)
                {
                    EXIT_STATE_WITH_ERROR(me,exit_loop);
                    break;
                }
                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PREPARE_COMMAND;
                break;
            }

            case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
            {
                exit_loop = TRUE;
                me->exit_status = optiga_cmd_handler(me);
                if (OPTIGA_LIB_SUCCESS != me->exit_status)
                {
                    me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
                    exit_loop = FALSE;
                    break;
                }
                me->p_optiga->comms_rx_size = OPTIGA_CMD_TOTAL_COMMS_BUFFER_SIZE;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
                me->p_optiga->p_optiga_comms->protection_level = me->protection_level;
                me->p_optiga->p_optiga_comms->protocol_version = me->protocol_version;
                me->p_optiga->protection_level_state |= me->protection_level;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
                (void)optiga_comms_set_callback_context(p_optiga->p_optiga_comms, me);
                me->exit_status = optiga_comms_transceive(p_optiga->p_optiga_comms,
                                                          p_optiga->optiga_comms_buffer,
                                                          p_optiga->comms_tx_size,
                                                          p_optiga->optiga_comms_buffer,
                                                          &p_optiga->comms_rx_size);

                if (OPTIGA_LIB_SUCCESS != me->exit_status)
                {
                    EXIT_STATE_WITH_ERROR(me,exit_loop);
                    break;
                }

                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PROCESS_RESPONSE;
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_ENTER_HANDLER_CALL);
                break;
            }
            case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
            {
                if (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE != (me->device_error_status & OPTIGA_CMD_ENTER_HANDLER_CALL_MASK))
                {
                    me->exit_status = optiga_cmd_handler(me);
                }
                else
                {
                    me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
                    exit_loop = FALSE;
                    break;
                }
                if (OPTIGA_LIB_SUCCESS == me->exit_status)
                {
                    if (FALSE == me->chaining_ongoing)
                    {
                        if (OPTIGA_CMD_STATE_EXIT != me->cmd_next_execution_state)
                        {
                            me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
                        }
                        exit_loop = FALSE;
                        break;
                    }
                    // for chaining, trigger preparing of next command
                    else if (TRUE == me->chaining_ongoing)
                    {
                        pal_os_event_register_callback_oneshot(me->p_optiga->p_pal_os_event_ctx,
                                                               (register_callback)optiga_cmd_event_trigger_execute,
                                                               (void*)me,
                                                               OPTIGA_CMD_SCHEDULER_IDLING_TIME_MS);
                        exit_loop = TRUE;

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
                        me->protection_level &= OPTIGA_PROTECTION_LEVEL_MASK;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION

                        me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PREPARE_COMMAND;
                    }
                    // After successful Close Application, change state to invoke optiga_comms_close
                    else
                    {
                        pal_os_event_register_callback_oneshot(me->p_optiga->p_pal_os_event_ctx,
                                                               (register_callback)optiga_cmd_event_trigger_execute,
                                                               me,
                                                               OPTIGA_CMD_SCHEDULER_IDLING_TIME_MS);
                        exit_loop=TRUE;
                        me->cmd_next_execution_state = OPTIGA_CMD_EXEC_COMMS_CLOSE_START;
                    }
                }
                else
                {
                    // After OPTIGA error is analyzed, invoke upper layer handler and release lock
                    if (OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT == me->exit_status)
                    {
                        me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
                        exit_loop = FALSE;
                        break;
                    }
                    // After OPTIGA responds with failure, invoke the next state to check which error occurred
                    else
                    {
                        me->cmd_next_execution_state = OPTIGA_CMD_EXEC_GET_DEVICE_ERROR;
                        SET_DEV_ERROR_HANDLER_STATE(OPTIGA_CMD_ERROR_CODE_PREPARE);
                        exit_loop=FALSE;
                    }
                }
                break;
            }
            case OPTIGA_CMD_EXEC_GET_DEVICE_ERROR:
            {
                exit_loop=TRUE;
                me->exit_status = optiga_cmd_get_error_code_handler(me);
                if (((OPTIGA_LIB_SUCCESS != me->exit_status) && !(OPTIGA_DEVICE_ERROR & me->exit_status)) ||
                    ((OPTIGA_DEVICE_ERROR == me->exit_status) &&
                    ((me->device_error_status & OPTIGA_CMD_ERROR_CODE_STATE_MASK) == OPTIGA_CMD_ERROR_CODE_RX)))
                {
                    exit_loop=FALSE;
                    me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
                }
                break;
            }
            case OPTIGA_CMD_EXEC_RELEASE_LOCK:
            {
                //lint --e{534} suppress "The return code is not checked because this is exit state."
                optiga_cmd_release_lock(me);
                me->cmd_next_execution_state = OPTIGA_CMD_STATE_EXIT;
                exit_loop=FALSE;
                break;
            }
            case OPTIGA_CMD_STATE_EXIT:
            {
                me->handler(me->caller_context, me->exit_status);
                exit_loop=TRUE;
                break;
            }
        }
    } while (FALSE == exit_loop);
}

optiga_cmd_t * optiga_cmd_create(uint8_t optiga_instance_id, callback_handler_t handler, void * caller_context)
{
    optiga_cmd_t * me = NULL;

    pal_os_lock_enter_critical_section();
    do
    {
        // Get number of free slots
        if (0 == optiga_cmd_queue_get_count_of(g_optiga_list[optiga_instance_id],
                                               OPTIGA_CMD_QUEUE_SLOT_STATE,
                                               OPTIGA_CMD_QUEUE_NOT_ASSIGNED))
        {
            break;
        }

        me = (optiga_cmd_t *)pal_os_calloc(1, sizeof(optiga_cmd_t));
        if (NULL == me)
        {
            break;
        }

        me->handler = handler;
        me->caller_context = caller_context;

        me->p_optiga = g_optiga_list[optiga_instance_id];
        me->optiga_context_datastore_id = g_hibernate_datastore_id_list[optiga_instance_id];

        if (FALSE == me->p_optiga->instance_init_state)
        {
            me->p_optiga->instance_init_state = TRUE;
            //create pal os event
            me->p_optiga->p_pal_os_event_ctx = pal_os_event_create(optiga_cmd_queue_scheduler, me->p_optiga);
            me->p_optiga->p_optiga_comms = optiga_comms_create(optiga_cmd_execute_handler, me);
            me->p_optiga->p_optiga_comms->p_pal_os_event_ctx = me->p_optiga->p_pal_os_event_ctx;
        }
        // attach optiga cmd queue entry
        optiga_cmd_queue_assign_slot(me, &(me->queue_id));
    } while (FALSE);

    pal_os_lock_exit_critical_section();
    return (me);
}

optiga_lib_status_t optiga_cmd_destroy(optiga_cmd_t * me)
{
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    do
    {
        if (NULL != me)
        {
            return_status = optiga_cmd_release_session(me);
            // attach optiga cmd queue entry
            optiga_cmd_queue_deassign_slot(me);
            pal_os_free(me);
            return_status = OPTIGA_LIB_SUCCESS;
        }
    } while (FALSE);
    return (return_status);
}


/*
* Last error code handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_get_error_code_handler(optiga_cmd_t * me)
{
    optiga_context_t * p_optiga;
    optiga_lib_status_t return_status = OPTIGA_DEVICE_ERROR;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;

#define OFFSET                     (0x0000)
#define BYTES_TO_READ              (0x0001)
#define GET_DATA_OBJECT_CMD        (0x01)
#define PARAM                      (0x00)

    p_optiga = me->p_optiga;

    switch (me->device_error_status & OPTIGA_CMD_ERROR_CODE_STATE_MASK)
    {
        case OPTIGA_CMD_ERROR_CODE_PREPARE:
        {
            optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data), OPTIGA_CMD_LAST_ERROR_CODE);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data), OFFSET);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data), BYTES_TO_READ);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            optiga_cmd_prepare_apdu_header(GET_DATA_OBJECT_CMD,
                                           PARAM,
                                           (uint16_t)(index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (uint16_t)(index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            SET_DEV_ERROR_HANDLER_STATE(OPTIGA_CMD_ERROR_CODE_TX);
            pal_os_event_register_callback_oneshot(me->p_optiga->p_pal_os_event_ctx,
                                                   (register_callback)optiga_cmd_event_trigger_execute,
                                                   me, OPTIGA_CMD_SCHEDULER_IDLING_TIME_MS);
        }
        break;
        case OPTIGA_CMD_ERROR_CODE_TX:
        {
            me->p_optiga->comms_rx_size = OPTIGA_CMD_TOTAL_COMMS_BUFFER_SIZE;

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
            me->p_optiga->p_optiga_comms->protection_level = me->protection_level;
            me->p_optiga->p_optiga_comms->protocol_version = me->protocol_version;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION

            (void)optiga_comms_set_callback_context(p_optiga->p_optiga_comms, (void*)me);
            return_status = optiga_comms_transceive(p_optiga->p_optiga_comms,
                                                    p_optiga->optiga_comms_buffer,
                                                    p_optiga->comms_tx_size,
                                                    p_optiga->optiga_comms_buffer,
                                                    &p_optiga->comms_rx_size);
            if (OPTIGA_COMMS_SUCCESS != return_status)
            {
                return_status = OPTIGA_CMD_ERROR;
                break;
            }
            SET_DEV_ERROR_HANDLER_STATE(OPTIGA_CMD_ERROR_CODE_RX);
        }
        break;
        case OPTIGA_CMD_ERROR_CODE_RX:
        {
            if (OPTIGA_CMD_APDU_FAILURE == me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                break;
            }
            return_status = me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET];
            return_status = return_status | OPTIGA_DEVICE_ERROR;
            me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PROCESS_RESPONSE;
            pal_os_event_register_callback_oneshot(me->p_optiga->p_pal_os_event_ctx,
                                                   (register_callback)optiga_cmd_event_trigger_execute,
                                                   me, OPTIGA_CMD_SCHEDULER_IDLING_TIME_MS);
        }
        break;
        default:
            break;
    }

#undef OFFSET
#undef BYTES_TO_READ
#undef GET_DATA_OBJECT_CMD
#undef PARAM
    return (return_status);
}

/*
* Open Application handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_open_application_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {

            OPTIGA_CMD_LOG_MESSAGE("Sending open app command...");

            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + sizeof(g_optiga_unique_application_identifier);
            total_apdu_length += ((OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT == me->cmd_param) ? (0) :
                                    (sizeof(me->p_optiga->optiga_context_handle_buffer)));
            //lint --e{774} suppress "If OPTIGA_MAX_COMMS_BUFFER_SIZE is set to lesser value it will fail"
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }
            if (OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT != me->cmd_param)
            {
                ///optiga context restore operation
                if (OPTIGA_LIB_SUCCESS != optiga_cmd_restore_context(me))
                {
                    break;
                }
            }
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_OPEN_APPLICATION,
                                           me->cmd_param,
                                           ((OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT == me->cmd_param) ?
                                           (sizeof(g_optiga_unique_application_identifier)) :
                                           (sizeof(g_optiga_unique_application_identifier) +
                                           sizeof(me->p_optiga->optiga_context_handle_buffer))),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            pal_os_memcpy(me->p_optiga->optiga_comms_buffer + OPTIGA_CMD_APDU_INDATA_OFFSET,
                          g_optiga_unique_application_identifier,
                          sizeof(g_optiga_unique_application_identifier));
            if (OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT != me->cmd_param)
            {
                pal_os_memcpy(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET +
                              sizeof(g_optiga_unique_application_identifier)],
                              me->p_optiga->optiga_context_handle_buffer,
                              sizeof(me->p_optiga->optiga_context_handle_buffer));
            }

            me->p_optiga->comms_tx_size = (OPTIGA_CMD_APDU_HEADER_SIZE +
                                           ((OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT == me->cmd_param) ?
                                           sizeof(g_optiga_unique_application_identifier):
                                           (sizeof(g_optiga_unique_application_identifier) +
                                           sizeof(me->p_optiga->optiga_context_handle_buffer))));

            pal_os_memset(me->p_optiga->optiga_context_handle_buffer,
                          0,
                          sizeof(me->p_optiga->optiga_context_handle_buffer));
            if (OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED != me->optiga_context_datastore_id)
            {
                //Clearing context handle secret from datastore
                return_status = pal_os_datastore_write(me->optiga_context_datastore_id,
                                                       me->p_optiga->optiga_context_handle_buffer,
                                                       sizeof(me->p_optiga->optiga_context_handle_buffer));
                if (PAL_STATUS_SUCCESS != return_status)
                {
                    return_status = OPTIGA_CMD_ERROR;
                    break;
                }
            }
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for open app command...");

            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing open app response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
            me->p_optiga->p_optiga_comms->manage_context_operation = OPTIGA_COMMS_SESSION_CONTEXT_NONE;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION
            OPTIGA_CMD_LOG_MESSAGE("Response of open app command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;

        }
        break;
        default:
            break;
    }

    return (return_status);
}


optiga_lib_status_t optiga_cmd_open_application(optiga_cmd_t * me, uint8_t cmd_param, void * params)
{
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    optiga_cmd_execute(me,
                       cmd_param,
                       optiga_cmd_open_application_handler,
                       OPTIGA_CMD_EXEC_COMMS_OPEN,
                       params);

    return (OPTIGA_LIB_SUCCESS);
}

/*
* Close Application handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_close_application_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {

            OPTIGA_CMD_LOG_MESSAGE("Sending close app command..");

            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE;
            //lint --e{774} suppress "If OPTIGA_MAX_COMMS_BUFFER_SIZE is set to lesser value it will fail"
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_CLOSE_APPLICATION,
                                           me->cmd_param,
                                           OPTIGA_CMD_ZERO_LENGTH_OR_VALUE,
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = OPTIGA_CMD_APDU_HEADER_SIZE;
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for close app command...");
            // check if the close app was successful
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing close app response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            if (OPTIGA_CMD_PARAM_INITIALIZE_APP_CONTEXT != me->cmd_param)
            {
                pal_os_memcpy(me->p_optiga->optiga_context_handle_buffer,
                              me->p_optiga->optiga_comms_buffer + OPTIGA_CMD_APDU_INDATA_OFFSET,
                              sizeof(me->p_optiga->optiga_context_handle_buffer));
                ///Optiga context store operation
                if (OPTIGA_LIB_SUCCESS != optiga_cmd_store_context(me))
                {
                    pal_os_memset(me->p_optiga->optiga_context_handle_buffer,0,
                                    sizeof(me->p_optiga->optiga_context_handle_buffer));
                    break;
                }
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of close app command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;

            // Close Application is successful, invoke optiga_comms_close next
            me->chaining_ongoing = START_OPTIGA_COMMS_CLOSE;
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_close_application(optiga_cmd_t * me, uint8_t cmd_param, void * params)
{
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    optiga_cmd_execute(me,
                       cmd_param,
                       optiga_cmd_close_application_handler,
                       OPTIGA_CMD_EXEC_REQUEST_LOCK,
                       params);

    return (OPTIGA_LIB_SUCCESS);
}

/*
* Get Data Object handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_get_data_object_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_get_data_object_params_t * p_optiga_read_data = (optiga_get_data_object_params_t *)me->p_input;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    uint16_t size_to_read, data_read;
    uint8_t cmd = OPTIGA_CMD_GET_DATA_OBJECT;
    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending read data command...");
            // APDU header size + oid 2bytes + offset 2 bytes + length 2 bytes
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                                    OPTIGA_CMD_UINT16_SIZE_IN_BYTES + OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            //lint --e{774} suppress "If OPTIGA_MAX_COMMS_BUFFER_SIZE is set to lesser value it will fail"
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                *(p_optiga_read_data->ref_bytes_to_read) = 0x00;
                break;
            }
            //oid
            optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                     p_optiga_read_data->oid);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            size_to_read = MIN((OPTIGA_MAX_COMMS_BUFFER_SIZE - OPTIGA_CMD_APDU_HEADER_SIZE),
                               (p_optiga_read_data->bytes_to_read - p_optiga_read_data->accumulated_size));

            if (CMD_READ_DATA == p_optiga_read_data->data_or_metadata)
            {
                //offset
                optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                           (p_optiga_read_data->offset + p_optiga_read_data->accumulated_size));
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

                //length
                optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                         size_to_read);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            }
            // If OID is for Last error code, don't clear the error code in read
            if (OPTIGA_CMD_LAST_ERROR_CODE == p_optiga_read_data->oid)
            {
                cmd = CMD_GET_DATA_OBJECT_NO_ERROR_CLEAR;
            }
            //prepare APDU header
            optiga_cmd_prepare_apdu_header(cmd,
                                           me->cmd_param,
                                           (index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           (me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET));

            me->p_optiga->comms_tx_size = index_for_data - OPTIGA_COMMS_DATA_OFFSET;
            p_optiga_read_data->last_read_size = size_to_read;
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for read data command...");
            me->chaining_ongoing = FALSE;
            // check if the read was successful
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing read data response...");
                //check if it is out of boundary issue
                if (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE != p_optiga_read_data->accumulated_size)
                {
                    SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_ENTER_HANDLER_CALL);
                }
                //flag used to setting the received buffer length to 0, in case unexpected error
                else
                {
                    SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                    *(p_optiga_read_data->ref_bytes_to_read) = 0x00;
                }
                return_status = OPTIGA_CMD_ERROR;
            }
            else if (me->exit_status == (optiga_lib_status_t)(OPTIGA_CMD_OUT_OF_BOUNDARY_ERROR | OPTIGA_DEVICE_ERROR))
            {
                *(p_optiga_read_data->ref_bytes_to_read) = p_optiga_read_data->accumulated_size;
                return_status = OPTIGA_LIB_SUCCESS;
            }
            else
            {
                //copy data from optiga comms buffer to user provided buffer
                data_read = me->p_optiga->comms_rx_size - OPTIGA_CMD_APDU_HEADER_SIZE;

                // check if the data received is greater than the size of read buffer
                if (p_optiga_read_data->bytes_to_read < data_read)
                {
                    OPTIGA_CMD_LOG_MESSAGE("Error in processing read data response...");
                    return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                    *(p_optiga_read_data->ref_bytes_to_read) = 0x00;
                    break;
                }

                //copy data from optiga comms buffer to user provided buffer
                pal_os_memcpy(p_optiga_read_data->buffer + p_optiga_read_data->accumulated_size,
                              me->p_optiga->optiga_comms_buffer + OPTIGA_CMD_APDU_INDATA_OFFSET,
                              data_read);

                p_optiga_read_data->accumulated_size += data_read;

                if ((p_optiga_read_data->last_read_size > data_read) ||
                    (p_optiga_read_data->accumulated_size == p_optiga_read_data->bytes_to_read))
                {
                    *(p_optiga_read_data->ref_bytes_to_read) = p_optiga_read_data->accumulated_size;
                    p_optiga_read_data->accumulated_size = 0;
                    p_optiga_read_data->last_read_size = 0;
                }
                else
                {
                    me->chaining_ongoing = TRUE;
                }
                OPTIGA_CMD_LOG_MESSAGE("Response of read data command is processed...");
                return_status = OPTIGA_LIB_SUCCESS;
            }
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_get_data_object(optiga_cmd_t * me, uint8_t cmd_param,
                                               optiga_get_data_object_params_t * params)
{
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    optiga_cmd_execute(me,
                       cmd_param,
                       optiga_cmd_get_data_object_handler,
                       OPTIGA_CMD_EXEC_REQUEST_LOCK,
                       params);

    return (OPTIGA_LIB_SUCCESS);
}

/*
* Set Data Object handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_set_data_object_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_set_data_object_params_t * p_optiga_write_data = (optiga_set_data_object_params_t *)me->p_input;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t size_to_send;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {

            OPTIGA_CMD_LOG_MESSAGE("Sending set data command...");
            me->chaining_ongoing = FALSE;
            //oid
            optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                     p_optiga_write_data->oid);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            //offset
            optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                     (p_optiga_write_data->offset + p_optiga_write_data->written_size));
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            // Check maximum size that can be written, based on optiga comms buffer size
            size_to_send = MIN((OPTIGA_MAX_COMMS_BUFFER_SIZE + OPTIGA_COMMS_DATA_OFFSET - index_for_data),
                               ((p_optiga_write_data->size) - p_optiga_write_data->written_size));

            // APDU header size + oid 2 bytes + offset 2 bytes + size of data to send
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                                    OPTIGA_CMD_UINT16_SIZE_IN_BYTES + size_to_send;
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }

            // prepare apdu
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_SET_DATA_OBJECT,
                                           me->cmd_param,
                                           (OPTIGA_CMD_APDU_HEADER_SIZE + size_to_send),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);
            //data to be written
            if (OPTIGA_UTIL_COUNT_DATA_OBJECT == me->cmd_param)
            {
                *(me->p_optiga->optiga_comms_buffer + index_for_data) = p_optiga_write_data->count;
            }
            else
            {
                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                              p_optiga_write_data->buffer + p_optiga_write_data->written_size,
                              size_to_send);
            }
            p_optiga_write_data->written_size += size_to_send;

            me->p_optiga->comms_tx_size = (index_for_data + size_to_send - OPTIGA_COMMS_DATA_OFFSET);

            // check if chaining is required based on size written and the user requested write
            if (p_optiga_write_data->written_size != p_optiga_write_data->size)
            {
                me->chaining_ongoing = TRUE;
                // For chaining, the 2nd loop will be Write only
                me->cmd_param = CMD_WRITE_ONLY;
            }
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for set data command...");
            // check if the write was successful
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing set data response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of set data command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }
    return (return_status);
}


optiga_lib_status_t optiga_cmd_set_data_object(optiga_cmd_t * me, uint8_t cmd_param,
                                               optiga_set_data_object_params_t * params)
{
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    optiga_cmd_execute(me,
                       cmd_param,
                       optiga_cmd_set_data_object_handler,
                       OPTIGA_CMD_EXEC_REQUEST_LOCK,
                       params);

    return (OPTIGA_LIB_SUCCESS);
}

#if defined (OPTIGA_CRYPT_RANDOM_ENABLED) || defined (OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED)
/*
* Get Random handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_get_random_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_get_random_params_t * p_random_params = (optiga_get_random_params_t *)me->p_input;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending get random command...");
            /// APDU header size + length of random
            /// OID size in case of param 0x04
            /// 0x41, Length and prepending optional data
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            total_apdu_length += ((CMD_RANDOM_PARAM_TYPE_PRE_MASTER_SECRET == (uint8_t)me->cmd_param) ?
                                (OPTIGA_CMD_UINT16_SIZE_IN_BYTES + OPTIGA_CMD_APDU_TL_LENGTH + p_random_params->optional_data_length) : 0x00);

            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }
            /// Copy the random data length
            optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                     p_random_params->random_data_length);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            if (CMD_RANDOM_PARAM_TYPE_PRE_MASTER_SECRET == (uint8_t)me->cmd_param)
            {
                /// Copy the data to buffer
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data),(me->session_oid));
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

                ///TLV formation for key usage
                optiga_cmd_prepare_tag_header(CMD_RSA_PRE_MASTER_OPTIONAL_DATA_TAG,
                                              ((NULL != p_random_params->optional_data) ?
                                                p_random_params->optional_data_length : 0x0000),
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                if ((NULL != p_random_params->optional_data))
                {
                    pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data, p_random_params->optional_data,
                                  p_random_params->optional_data_length);
                    index_for_data += p_random_params->optional_data_length;
                }
            }
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_GET_RANDOM,
                                           me->cmd_param,
                                           (index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = index_for_data - OPTIGA_COMMS_DATA_OFFSET;

            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for get random command...");
            // check if the random data retrieval app was successful
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing get random response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            if (CMD_RANDOM_PARAM_TYPE_PRE_MASTER_SECRET != (uint8_t)me->cmd_param)
            {
                //copy data from optiga comms buffer to user provided buffer
                pal_os_memcpy(p_random_params->random_data,
                              me->p_optiga->optiga_comms_buffer + OPTIGA_CMD_APDU_INDATA_OFFSET,
                              p_random_params->random_data_length);
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of get random command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }

    return (return_status);
}


optiga_lib_status_t optiga_cmd_get_random(optiga_cmd_t * me, uint8_t cmd_param, optiga_get_random_params_t * params)
{
    optiga_cmd_state_t cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_LOCK;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    if (CMD_RANDOM_PARAM_TYPE_PRE_MASTER_SECRET == cmd_param)
    {
        cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_SESSION;
    }

    optiga_cmd_execute(me,
                       cmd_param,
                       optiga_cmd_get_random_handler,
                       cmd_handler_state,
                       params);

    return (OPTIGA_LIB_SUCCESS);
}
#endif //OPTIGA_CRYPT_RANDOM_ENABLED

#if defined (OPTIGA_CRYPT_ECDSA_SIGN_ENABLED) || defined (OPTIGA_CRYPT_RSA_SIGN_ENABLED)
/*
* CalcSign handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_calc_sign_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_calc_sign_params_t * p_optiga_ecdsa_sign = (optiga_calc_sign_params_t *)me->p_input;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t private_key_oid;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            // APDU headed length + TLV of Digest + TLV of signature key OID
            OPTIGA_CMD_LOG_MESSAGE("Sending calculate sign command..");
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_APDU_TL_LENGTH + p_optiga_ecdsa_sign->digest_length +
                                    OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                *(p_optiga_ecdsa_sign->p_signature_length) = 0x00;
                break;
            }
            // Tag and length for digest
            optiga_cmd_prepare_tag_header(CMD_SIGN_DIGEST_TAG,
                                          p_optiga_ecdsa_sign->digest_length,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);
            //Digest data
            pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                          p_optiga_ecdsa_sign->p_digest,
                          p_optiga_ecdsa_sign->digest_length);

            index_for_data += p_optiga_ecdsa_sign->digest_length;

            // Tag and length for OID of signature key
            optiga_cmd_prepare_tag_header(CMD_SIGN_OID_TAG,
                                          CMD_SIGN_OID_LEN,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);
            //oid
            private_key_oid = me->session_oid;
            if (OPTIGA_KEY_ID_SESSION_BASED != p_optiga_ecdsa_sign->private_key_oid)
            {
                private_key_oid = (uint16_t)p_optiga_ecdsa_sign->private_key_oid;
            }
            optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                     private_key_oid);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            // prepare apdu
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_CALC_SIGN,
                                           me->cmd_param,
                                           (index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for calculate sign command...");
            // check if the calculate signature command was successful
            if (OPTIGA_CMD_APDU_SUCCESS == me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                // if the received signature length is greater than the user provided signature buffer length
                if ((*(p_optiga_ecdsa_sign->p_signature_length)) <
                    (me->p_optiga->comms_rx_size - OPTIGA_CMD_APDU_HEADER_SIZE))
                {
                    OPTIGA_CMD_LOG_MESSAGE("Error in processing calculate sign response...");
                    *(p_optiga_ecdsa_sign->p_signature_length) = 0x00;
                    return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                }
                else
                {
                    *(p_optiga_ecdsa_sign->p_signature_length) = me->p_optiga->comms_rx_size - OPTIGA_CMD_APDU_HEADER_SIZE;
                    //copy signed data from optiga comms buffer to user provided buffer

                    pal_os_memcpy(p_optiga_ecdsa_sign->p_signature,
                                  me->p_optiga->optiga_comms_buffer + OPTIGA_CMD_APDU_INDATA_OFFSET,
                                  *(p_optiga_ecdsa_sign->p_signature_length));
                    OPTIGA_CMD_LOG_MESSAGE("Response of calculate sign command is processed...");
                    return_status = OPTIGA_LIB_SUCCESS;
                }
            }
            else
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing calculate sign response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                *(p_optiga_ecdsa_sign->p_signature_length) = 0x00;
            }
        }
        break;
        default:
            break;
    }

    return (return_status);
}


optiga_lib_status_t optiga_cmd_calc_sign(optiga_cmd_t * me, uint8_t cmd_param, optiga_calc_sign_params_t * params)
{
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR_INVALID_INPUT;
    optiga_calc_sign_params_t * p_optiga_ecdsa_sign = (optiga_calc_sign_params_t*)params;
    optiga_cmd_state_t cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_LOCK;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    do
    {
        if (OPTIGA_KEY_ID_SESSION_BASED == p_optiga_ecdsa_sign->private_key_oid)
        {
            if (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE == me->session_oid)
            {
                break;
            }
            cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_SESSION;
        }


        optiga_cmd_execute(me,
                           cmd_param,
                           optiga_cmd_calc_sign_handler,
                           cmd_handler_state,
                           params);

        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    return (return_status);
}
#endif //(OPTIGA_CRYPT_ECDSA_SIGN_ENABLED) ||(OPTIGA_CRYPT_RSA_SIGN_ENABLED)

#if defined (OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED) || defined (OPTIGA_CRYPT_RSA_VERIFY_ENABLED)
/*
* VerifySign handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_verify_sign_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_verify_sign_params_t * p_optiga_verify_sign = (optiga_verify_sign_params_t *)me->p_input;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending verify sign command..");
            // APDU header length + TLV of digest + TLV of signature +
            // If public key from OID (TLV of public key OID)
            // If public key from host (TLV of algo ID + TLV of public key)
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_APDU_TL_LENGTH + p_optiga_verify_sign->digest_length +
                                    OPTIGA_CMD_APDU_TL_LENGTH + (p_optiga_verify_sign->signature_length) +
                                    (OPTIGA_CRYPT_OID_DATA == p_optiga_verify_sign->public_key_source_type ?
                                     (OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES):
                                    (OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES + OPTIGA_CMD_NO_OF_BYTES_IN_TAG + OPTIGA_CMD_APDU_TL_LENGTH +
                                     p_optiga_verify_sign->public_key->length));
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }
            //TLV formation for digest
            optiga_cmd_prepare_tag_header(CMD_VERIFY_SIGN_DIGEST_TAG,
                                          p_optiga_verify_sign->digest_length,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);

            pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                          p_optiga_verify_sign->p_digest,
                          p_optiga_verify_sign->digest_length);
            index_for_data += p_optiga_verify_sign->digest_length;

            //TLV formation for signature
            optiga_cmd_prepare_tag_header(CMD_VERIFY_SIGN_SIGNATURE_TAG,
                                          p_optiga_verify_sign->signature_length,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);

            pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                          p_optiga_verify_sign->p_signature,
                          p_optiga_verify_sign->signature_length);
            index_for_data += p_optiga_verify_sign->signature_length;

            if (OPTIGA_CRYPT_OID_DATA == p_optiga_verify_sign->public_key_source_type)
            {
                //TLV formation for public key certificate OID
                optiga_cmd_prepare_tag_header(CMD_VERIFY_SIGN_PUB_KEY_CERT_OID_TAG,
                                              CMD_VERIFY_SIGN_PUB_KEY_CERT_OID_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data),
                                         p_optiga_verify_sign->certificate_oid);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            }
            else
            {
                //TLV formation for public key from host
                optiga_cmd_prepare_tag_header(CMD_VERIFY_ALGO_ID_TAG,
                                              CMD_VERIFY_ALGO_ID_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                *(me->p_optiga->optiga_comms_buffer + index_for_data++) = (uint8_t)p_optiga_verify_sign->public_key->key_type;

                optiga_cmd_prepare_tag_header(CMD_VERIFY_PUBLIC_KEY_TAG,
                                              p_optiga_verify_sign->public_key->length,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                              p_optiga_verify_sign->public_key->public_key,
                              p_optiga_verify_sign->public_key->length);
                index_for_data += p_optiga_verify_sign->public_key->length;
            }

            //form apdu header
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_VERIFY_SIGN,
                                           me->cmd_param,
                                           (uint16_t)(index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (uint16_t)(index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PROCESS_RESPONSE;
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for verify sign command...");
            if (OPTIGA_CMD_APDU_FAILURE == me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing verify sign response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of veriy sign command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_verify_sign(optiga_cmd_t * me,
                                           uint8_t cmd_param,
                                           optiga_verify_sign_params_t * params)
{
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    optiga_cmd_execute(me,
                       cmd_param,
                       optiga_cmd_verify_sign_handler,
                       OPTIGA_CMD_EXEC_REQUEST_LOCK,
                       params);

    return (OPTIGA_LIB_SUCCESS);
}
#endif //OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED

#ifdef OPTIGA_CRYPT_ECDH_ENABLED
/*
* CalcSSec handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_calc_ssec_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    optiga_calc_ssec_params_t * p_optiga_ecdh = (optiga_calc_ssec_params_t *)me->p_input;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    uint16_t private_key_oid;
    
    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending calculate shared secret command..");
            // APDU header length + TLV of private key + TLV of algo id + TLV of public key
            // If shared secret option to export (TLV of export shared secret)
            // If shared secret store in OID (TLV of shared secret OID)
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                                    OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_NO_OF_BYTES_IN_TAG + OPTIGA_CMD_APDU_TL_LENGTH +
                                    p_optiga_ecdh->public_key->length +
                                    (TRUE == p_optiga_ecdh->export_to_host?(OPTIGA_CMD_APDU_TL_LENGTH):
                                    (OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES));
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }
            // Tag and length for private key OID
            optiga_cmd_prepare_tag_header(CMD_SSEC_PRIVATE_KEY_TAG,
                                          CMD_SSEC_PRIVATE_KEY_LEN,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);

            private_key_oid = (uint16_t)p_optiga_ecdh->private_key;
            if (OPTIGA_KEY_ID_SESSION_BASED == p_optiga_ecdh->private_key)
            {
                private_key_oid = me->session_oid;
            }

            optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data], private_key_oid);
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            // Tag and length for algorithm ID
            optiga_cmd_prepare_tag_header(CMD_SSEC_ALG_ID_TAG,
                                          CMD_SSEC_ALG_ID_LEN,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);

            me->p_optiga->optiga_comms_buffer[index_for_data++] = (uint8_t)p_optiga_ecdh->public_key->key_type;

            // Tag and length for public key
            optiga_cmd_prepare_tag_header(CMD_SSEC_PUB_KEY_TAG,
                                          p_optiga_ecdh->public_key->length,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);

            //Copy the public key to APDU
            pal_os_memcpy(&me->p_optiga->optiga_comms_buffer[index_for_data],
                          p_optiga_ecdh->public_key->public_key,
                          p_optiga_ecdh->public_key->length);

            index_for_data += p_optiga_ecdh->public_key->length;

            // check if shared secret needs to be exported or stored
            if (FALSE == p_optiga_ecdh->export_to_host)
            {
                // Tag and length for storing shared secret in session OID
                optiga_cmd_prepare_tag_header(CMD_SSEC_STORE_SESSION_TAG,
                                              CMD_SSEC_STORE_SESSION_LEN,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);
                // store session secret oid
                optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data], me->session_oid);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            }
            else
            {
                // Tag and length for exporting shared secret
                optiga_cmd_prepare_tag_header(CMD_SSEC_EXPORT_TAG,
                                              CMD_SSEC_EXPORT_LEN,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);
            }

            // prepare apdu
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_CALC_SSEC,
                                           me->cmd_param,
                                           (index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for calculate shared secret command...");
            // check if the shared secret transceive was successful
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing calculate shared secret response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            if (FALSE != p_optiga_ecdh->export_to_host)
            {
                //copy shared secret from optiga comms buffer to user provided buffer
                pal_os_memcpy(p_optiga_ecdh->shared_secret,
                              me->p_optiga->optiga_comms_buffer + OPTIGA_CMD_APDU_INDATA_OFFSET,
                              (me->p_optiga->comms_rx_size - OPTIGA_CMD_APDU_HEADER_SIZE));
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of calculate shared secret command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_calc_ssec(optiga_cmd_t * me, uint8_t cmd_param, optiga_calc_ssec_params_t * params)
{
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR_INVALID_INPUT;
    optiga_calc_ssec_params_t * p_optiga_ecdh = (optiga_calc_ssec_params_t *)params;
    optiga_cmd_state_t cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_SESSION;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    
    do
    {
        if ((OPTIGA_KEY_ID_SESSION_BASED == p_optiga_ecdh->private_key) &&
            (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE == me->session_oid))
        {
            break;
        }
        if ((OPTIGA_KEY_ID_SESSION_BASED != p_optiga_ecdh->private_key) &&
            (FALSE != p_optiga_ecdh->export_to_host))
        {
            cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_LOCK;
        }

        optiga_cmd_execute(me,
                           cmd_param,
                           optiga_cmd_calc_ssec_handler,
                           cmd_handler_state,
                           params);
        return_status = OPTIGA_LIB_SUCCESS;

    } while (FALSE);

    return (return_status);
}
#endif //OPTIGA_CRYPT_ECDH_ENABLED

#ifdef OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED
/*
* DeriveKey
*/
_STATIC_H optiga_lib_status_t optiga_cmd_derive_key_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    optiga_derive_key_params_t * p_optiga_tls_prf_sha256 = (optiga_derive_key_params_t *)me->p_input;
    uint16_t actual_secret_length = 0;
    uint16_t derive_key_length_sent;
    uint16_t secret_oid;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending derive key command..");
            // APDU header length + TLV of OID shared secret + TLV of secret derivation data +
            // TLV of length of the key to be derived
            // If derive key option to export (TLV of export derive key)
            // If derive key store in OID (TLV of derive key OID)
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                                    OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                                    (NULL != p_optiga_tls_prf_sha256->derived_key?(OPTIGA_CMD_APDU_TL_LENGTH):
                                    (OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES));
            if ((NULL != p_optiga_tls_prf_sha256->label) && (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE != p_optiga_tls_prf_sha256->label_length))
            {
                total_apdu_length += p_optiga_tls_prf_sha256->label_length;
            }
            //copy seed(secret) if not NULL
            if ((NULL != p_optiga_tls_prf_sha256->seed) && (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE != p_optiga_tls_prf_sha256->seed_length))
            {
                total_apdu_length += p_optiga_tls_prf_sha256->seed_length;
            }
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }

            // OID of shared secret to derive new secret from
            optiga_cmd_prepare_tag_header(CMD_DERIVE_KEY_SEC_OID_TAG,
                                          CMD_DERIVE_KEY_SEC_OID_TAG_LENGTH,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);
            secret_oid = (p_optiga_tls_prf_sha256->input_shared_secret_oid == 0x00) ?
                          me->session_oid : p_optiga_tls_prf_sha256->input_shared_secret_oid;
            optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data), secret_oid);
            index_for_data += 2;

            // Length of key to be derived
            optiga_cmd_prepare_tag_header(CMD_DERIVE_KEY_KEY_LEN_TAG,
                                          CMD_DERIVE_KEY_KEY_LEN_TAG_LENGTH,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);
            derive_key_length_sent = p_optiga_tls_prf_sha256->derived_key_length;
            if ((derive_key_length_sent > OPTIGA_CMD_ZERO_LENGTH_OR_VALUE) &&
                (derive_key_length_sent < CMD_DERIVE_KEY_DERIVE_KEY_LEN_MIN))
            {
                derive_key_length_sent = CMD_DERIVE_KEY_DERIVE_KEY_LEN_MIN;
            }
            optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data), derive_key_length_sent);
            index_for_data += 2;

            //copy label(secret) if present
            *(me->p_optiga->optiga_comms_buffer + index_for_data++) = CMD_DERIVE_KEY_DERIVATION_DATA_TAG;
            index_for_data += 2;

            if ((NULL != p_optiga_tls_prf_sha256->label) && (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE != p_optiga_tls_prf_sha256->label_length))
            {
                // pre increase index
                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                              p_optiga_tls_prf_sha256->label,
                              p_optiga_tls_prf_sha256->label_length);

                actual_secret_length = p_optiga_tls_prf_sha256->label_length;
            }
            //copy seed(secret) if not NULL
            if ((NULL != p_optiga_tls_prf_sha256->seed) && (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE != p_optiga_tls_prf_sha256->seed_length))
            {
                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data+actual_secret_length,
                              p_optiga_tls_prf_sha256->seed,
                              p_optiga_tls_prf_sha256->seed_length);

                actual_secret_length += p_optiga_tls_prf_sha256->seed_length;
            }

            //Add secret length
            optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data - 2), (actual_secret_length));
            index_for_data += actual_secret_length;

            // Alternate output options
            if (NULL == p_optiga_tls_prf_sha256->derived_key)
            {
                //store internal
                optiga_cmd_prepare_tag_header(CMD_DERIVE_KEY_STORE_TAG,
                                              CMD_DERIVE_KEY_STORE_TAG_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data), (me->session_oid));
                index_for_data += 2;
            }
            else
            {
                //export
                optiga_cmd_prepare_tag_header(CMD_DERIVE_KEY_EXPORT_TAG,
                                              OPTIGA_CMD_ZERO_LENGTH_OR_VALUE,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);
            }

            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_DERIVE_KEY,
                                           me->cmd_param,
                                          (index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PROCESS_RESPONSE;
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for derive key command...");
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing derive key response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of derive key command is processed...");
            // session release
            return_status = OPTIGA_LIB_SUCCESS;
            if (NULL != p_optiga_tls_prf_sha256->derived_key)
            {
                pal_os_memcpy(p_optiga_tls_prf_sha256->derived_key,
                              me->p_optiga->optiga_comms_buffer + OPTIGA_CMD_APDU_INDATA_OFFSET,
                              p_optiga_tls_prf_sha256->derived_key_length);
            }
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_derive_key(optiga_cmd_t * me, uint8_t cmd_param, optiga_derive_key_params_t * params)
{
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR_INVALID_INPUT;
    optiga_derive_key_params_t * p_optiga_tls_prf_sha256 = (optiga_derive_key_params_t*)params;
    optiga_cmd_state_t initial_state;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    
    do
    {
        if ((OPTIGA_KEY_ID_SESSION_BASED == (optiga_key_id_t)p_optiga_tls_prf_sha256->input_shared_secret_oid) &&
            (OPTIGA_CMD_ZERO_LENGTH_OR_VALUE == me->session_oid))
        {
            break;
        }

        me->chaining_ongoing = FALSE;
        me->cmd_param = cmd_param;
        initial_state = OPTIGA_CMD_EXEC_REQUEST_LOCK;

        if ((NULL == p_optiga_tls_prf_sha256->derived_key) ||
            (0x00 == p_optiga_tls_prf_sha256->input_shared_secret_oid))
        {
            initial_state = OPTIGA_CMD_EXEC_REQUEST_SESSION;
        }

        optiga_cmd_execute(me,
                           me->cmd_param,
                           optiga_cmd_derive_key_handler,
                           initial_state,
                           params);

        return_status = OPTIGA_LIB_SUCCESS;

    } while (FALSE);
    return (return_status);
}
#endif //OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED

#if defined (OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED) || defined (OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED)


#define ALGORITHM_PRIVATE_KEY_LOOKUP_TABLE_SIZE 4
static uint16_t algorithm_private_key_lookup_table[ALGORITHM_PRIVATE_KEY_LOOKUP_TABLE_SIZE][2] = 
{
    // ECC Keys
    {OPTIGA_ECC_CURVE_NIST_P_256, 0x22},
    {OPTIGA_ECC_CURVE_NIST_P_384, 0x32},
    // RSA Keys
    {OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL, 0x83},
    {OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL, 0x104},
    
};    

_STATIC_H void optiga_cmd_get_private_key_length(uint8_t algorithm, uint16_t * private_key_length)
{
    int i;
    
    do
    {
        if (private_key_length == NULL)
        {
            break;
        }
        for (i = 0; i < ALGORITHM_PRIVATE_KEY_LOOKUP_TABLE_SIZE); i++)
        {
            if (key_lookup_table[i][0] == algorithm)
            {
                *private_key_length = key_lookup_table[i][1];
                break;
            }
        }
        //There is no such algorithm, thus length to 0
        *private_key_length = 0;
        
    } while (FALSE);
    return ;
}

/*
* GenKeyPair handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_gen_keypair_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_gen_keypair_params_t * p_optiga_ecc_gen_keypair = (optiga_gen_keypair_params_t *)me->p_input;
    uint16_t header_offset;
	uint16_t golden_private_key_length;
    uint16_t private_key_length;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    uint16_t out_data_size;
    uint16_t public_key_length;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending generate keypair command..");
            // APDU header length +
            // If private key option to store in OID (TLV of private key + TLV of key usages)
            // If private key option to export (TLV of export key pair)
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE +
                                    (FALSE == p_optiga_ecc_gen_keypair->export_private_key ?
                                    (OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES + OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_NO_OF_BYTES_IN_TAG):
                                    (OPTIGA_CMD_APDU_TL_LENGTH ));
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                *p_optiga_ecc_gen_keypair->public_key_length = 0;
                break;
            }
            if (FALSE == p_optiga_ecc_gen_keypair->export_private_key)
            {
                ///TLV formation for private key OID
                optiga_cmd_prepare_tag_header (CMD_GEN_KEY_PAIR_PRIVATE_KEY_OID_TAG,
                                               CMD_GEN_KEY_PAIR_PRIVATE_KEY_OID_LENGTH,
                                               me->p_optiga->optiga_comms_buffer,
                                               &index_for_data);
                ///Update OID based on input param
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data),
                                         ((OPTIGA_KEY_ID_SESSION_BASED == p_optiga_ecc_gen_keypair->private_key_oid)?
                                         me->session_oid:(uint16_t)p_optiga_ecc_gen_keypair->private_key_oid));

                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
                ///TLV formation for key usage
                optiga_cmd_prepare_tag_header (CMD_GEN_KEY_PAIR_KEY_USAGE_TAG,
                                               CMD_GEN_KEY_PAIR_KEY_USAGE_LENGTH,
                                               me->p_optiga->optiga_comms_buffer,
                                               &index_for_data);

                *(me->p_optiga->optiga_comms_buffer + index_for_data++) = p_optiga_ecc_gen_keypair->key_usage;
            }
            else
            {
                ///TLV formation for export key
                optiga_cmd_prepare_tag_header (CMD_GEN_KEY_PAIR_EXPORT_KEY_TAG,
                                               CMD_GEN_KEY_PAIR_EXPORT_KEY_LENGTH,
                                               me->p_optiga->optiga_comms_buffer,
                                               &index_for_data);
            }

            //form apdu header
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_GEN_KEYPAIR, me->cmd_param,
                                            (uint16_t)(index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
            me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (uint16_t)(index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PROCESS_RESPONSE;
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for generate keypair command...");
            // check if the write was successful
            if (OPTIGA_CMD_APDU_FAILURE == me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing generate keypair response...");
                *p_optiga_ecc_gen_keypair->public_key_length = 0;
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            header_offset = 0;
            optiga_common_get_uint16(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET - OPTIGA_CMD_UINT16_SIZE_IN_BYTES],
                                     &out_data_size);

            while (out_data_size > header_offset)
            {
                if (CMD_GEN_KEY_PAIR_PUBLIC_KEY_TAG ==
                    me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + header_offset])
                {
                    optiga_common_get_uint16(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET +
                                                header_offset + OPTIGA_CMD_NO_OF_BYTES_IN_TAG], &public_key_length);
                    if (public_key_length > *p_optiga_ecc_gen_keypair->public_key_length)
                    {
                        OPTIGA_CMD_LOG_MESSAGE("Error in processing generate keypair response...");
                        return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                        *p_optiga_ecc_gen_keypair->public_key_length = 0;
                        break;
                    }
                    *p_optiga_ecc_gen_keypair->public_key_length = public_key_length;
                    pal_os_memcpy(p_optiga_ecc_gen_keypair->public_key,
                                  &me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + header_offset +
                                   OPTIGA_CMD_UINT16_SIZE_IN_BYTES + OPTIGA_CMD_NO_OF_BYTES_IN_TAG],
                                   *p_optiga_ecc_gen_keypair->public_key_length);
                    header_offset +=  (uint16_t)OPTIGA_CMD_UINT16_SIZE_IN_BYTES+ OPTIGA_CMD_NO_OF_BYTES_IN_TAG +
                                        *p_optiga_ecc_gen_keypair->public_key_length;
                    return_status = OPTIGA_LIB_SUCCESS;
                }
                else if (CMD_GEN_KEY_PAIR_PRIVATE_KEY_TAG == me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET +
                                                             header_offset])
                {
                    optiga_common_get_uint16(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + header_offset
                                             + OPTIGA_CMD_NO_OF_BYTES_IN_TAG], &private_key_length);

                    // check if the returned length of the key isn't longer than expected
                    optiga_cmd_get_private_key_length(me->cmd_param, &golden_private_key_length); 
                    if(golden_private_key_length != private_key_length)
                    {
                        OPTIGA_CMD_LOG_MESSAGE("Error in processing generate keypair response...");
                        return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                        *p_optiga_ecc_gen_keypair->public_key_length = 0;
                        break;
                    }

                    pal_os_memcpy(p_optiga_ecc_gen_keypair->private_key,
                                  &me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + header_offset +
                                  OPTIGA_CMD_UINT16_SIZE_IN_BYTES+ OPTIGA_CMD_NO_OF_BYTES_IN_TAG], private_key_length);
                    header_offset +=  OPTIGA_CMD_UINT16_SIZE_IN_BYTES+ OPTIGA_CMD_NO_OF_BYTES_IN_TAG + private_key_length;
                    return_status = OPTIGA_LIB_SUCCESS;
                }
                else
                {
                    break;
                }
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of generate keypair command is processed...");
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_gen_keypair(optiga_cmd_t * me, uint8_t cmd_param, optiga_gen_keypair_params_t * params)
{
    optiga_gen_keypair_params_t * p_optiga_ecc_gen_keypair = (optiga_gen_keypair_params_t*)params;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    
    if ((OPTIGA_KEY_ID_SESSION_BASED == p_optiga_ecc_gen_keypair->private_key_oid)
        && (FALSE == p_optiga_ecc_gen_keypair->export_private_key))
    {
        optiga_cmd_execute(me, cmd_param, optiga_cmd_gen_keypair_handler, OPTIGA_CMD_EXEC_REQUEST_SESSION, params);
    }
    else
    {
        optiga_cmd_execute(me, cmd_param, optiga_cmd_gen_keypair_handler, OPTIGA_CMD_EXEC_REQUEST_LOCK, params);
    }

    return (OPTIGA_LIB_SUCCESS);
}
#endif //OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

/*
* EncryptAsym handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_encrypt_asym_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_encrypt_asym_params_t * p_optiga_encrypt_asym = (optiga_encrypt_asym_params_t *)me->p_input;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t out_data_size;

#define PUBLIC_KEY_HOST ((public_key_from_host_t *)(p_optiga_encrypt_asym->key))

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending asymmetric encryption command..");
            // APDU header length + default TLV of OID of session + default TLV of OID of public key
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + (OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES) + (OPTIGA_CMD_APDU_TL_LENGTH +
                                OPTIGA_CMD_UINT16_SIZE_IN_BYTES);
            if (NULL != p_optiga_encrypt_asym->message)
            {
                // Add TLV of message and reduce 2 it is already added by default
                total_apdu_length += (p_optiga_encrypt_asym->message_length - OPTIGA_CMD_UINT16_SIZE_IN_BYTES);
            }
            if (p_optiga_encrypt_asym->public_key_source_type)
            {
                // Add TLV of public key and reduce 1 because alg id is only 1 byte
                total_apdu_length += (OPTIGA_CMD_APDU_TL_LENGTH +(PUBLIC_KEY_HOST->length));
                total_apdu_length--;
            }
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                *(p_optiga_encrypt_asym->processed_message_length) = 0;
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }

            // message for encryption
            if ((NULL == p_optiga_encrypt_asym->message) && (0 == p_optiga_encrypt_asym->message_length))
            {
                //TLV formation for session
                optiga_cmd_prepare_tag_header(CMD_ENCRYPT_ASYM_SESSION_OID_TAG,
                                              CMD_ENCRYPT_ASYM_SESSION_OID_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                //Add session OID
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data ), (me->session_oid));
                index_for_data += 2;
            }
            else
            {
                //TLV formation for message
                optiga_cmd_prepare_tag_header(CMD_ENCRYPT_ASYM_MESSAGE_TAG,
                                              p_optiga_encrypt_asym->message_length,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                                p_optiga_encrypt_asym->message,
                                p_optiga_encrypt_asym->message_length);
                index_for_data += p_optiga_encrypt_asym->message_length;

            }

            // key for encryption

            if (p_optiga_encrypt_asym->public_key_source_type)
            {

                //TLV formation for public key from host
                optiga_cmd_prepare_tag_header(CMD_ENCRYPT_ASYM_ALGO_ID_TAG,
                                              CMD_ENCRYPT_ASYM_ALGO_ID_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                *(me->p_optiga->optiga_comms_buffer + index_for_data++) = (uint8_t)PUBLIC_KEY_HOST->key_type;

                optiga_cmd_prepare_tag_header(CMD_ENCRYPT_ASYM_PUBLIC_KEY_TAG,
                                              PUBLIC_KEY_HOST->length,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                              PUBLIC_KEY_HOST->public_key,
                              PUBLIC_KEY_HOST->length);

                index_for_data += PUBLIC_KEY_HOST->length;
#undef PUBLIC_KEY_HOST
            }
            else
            {
                //TLV for public key from OID
                optiga_cmd_prepare_tag_header(CMD_ENCRYPT_ASYM_PUB_KEY_CERT_OID_TAG,
                                              CMD_ENCRYPT_ASYM_PUB_KEY_CERT_OID_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);

                //Add session OID
                optiga_common_set_uint16(me->p_optiga->optiga_comms_buffer + index_for_data,
                                        *((uint16_t * )(p_optiga_encrypt_asym->key)));
                index_for_data += CMD_ENCRYPT_ASYM_PUB_KEY_CERT_OID_LENGTH;
            }


            //form apdu header
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_ENCRYPT_ASYM,
                                           me->cmd_param,
                                           (uint16_t)(index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (uint16_t)(index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PROCESS_RESPONSE;
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for asymmetric encryption command...");
            // check if the random data retrieval app was successful
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing asymmetric encryption response...");
                *(p_optiga_encrypt_asym->processed_message_length) = 0;
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            optiga_common_get_uint16(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + OPTIGA_CMD_NO_OF_BYTES_IN_TAG],
                                     &out_data_size);
            if ((*(p_optiga_encrypt_asym->processed_message_length )) < out_data_size)
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing asymmetric encryption response...");
                *(p_optiga_encrypt_asym->processed_message_length) = 0;
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }
            //copy data from optiga comms buffer to user provided buffer
            pal_os_memcpy(p_optiga_encrypt_asym->processed_message,
                          &me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                          OPTIGA_CMD_NO_OF_BYTES_IN_TAG], out_data_size);
            *(p_optiga_encrypt_asym->processed_message_length) = out_data_size ;
            OPTIGA_CMD_LOG_MESSAGE("Response of asymmetric encryption command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_encrypt_asym(optiga_cmd_t * me, uint8_t cmd_param, optiga_encrypt_asym_params_t * params)
{
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR_INVALID_INPUT;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    
    // for encrypting session data, instance must have session.
    if (((NULL == params->message) && (0 == params->message_length) && (0x0000 != me->session_oid)) ||
        (NULL != params->message))
    {
        optiga_cmd_execute(me, cmd_param, optiga_cmd_encrypt_asym_handler, OPTIGA_CMD_EXEC_REQUEST_LOCK, params);
        return_status = OPTIGA_LIB_SUCCESS;
    }
    return (return_status);
}
#endif // OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

#ifdef OPTIGA_CRYPT_RSA_DECRYPT_ENABLED

/*
* EncryptAsym handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_decrypt_asym_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_decrypt_asym_params_t * p_optiga_decrypt_asym = (optiga_decrypt_asym_params_t *)me->p_input;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    uint16_t out_data_size;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending asymmetric decryption command..");
            // APDU headed length + TLV of encrypt message + TLV of decrypt key OID + (optional)TLV of session key OID
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_APDU_TL_LENGTH + p_optiga_decrypt_asym->message_length +
                                OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                                ((NULL != p_optiga_decrypt_asym->processed_message) &&
                                (NULL != p_optiga_decrypt_asym->processed_message_length)
                                ? 0 : OPTIGA_CMD_APDU_TL_LENGTH + OPTIGA_CMD_UINT16_SIZE_IN_BYTES);
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                if (NULL != p_optiga_decrypt_asym->processed_message_length)
                {
                    *(p_optiga_decrypt_asym->processed_message_length) = 0;
                }
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }
            // Tag and length for encrypt message
            optiga_cmd_prepare_tag_header(CMD_DECRYPT_ASYM_ENCRYPT_MESSAGE_TAG,
                                          p_optiga_decrypt_asym->message_length,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);
            //Encrypt data
            pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                          p_optiga_decrypt_asym->message,
                          p_optiga_decrypt_asym->message_length);

            index_for_data += p_optiga_decrypt_asym->message_length;

            // Tag and length for OID of decrypt key
            optiga_cmd_prepare_tag_header(CMD_DECRYPT_ASYM_DECRYPT_KEY_TAG,
                                          CMD_DECRYPT_ASYM_DECRYPT_KEY_LENGTH,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);
            //OID of decrypt key
            optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                     (uint16_t)*((optiga_key_id_t*)p_optiga_decrypt_asym->key));
            index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

            if ((NULL == p_optiga_decrypt_asym->processed_message) &&
                (NULL == p_optiga_decrypt_asym->processed_message_length))
            {
                // Tag and length for session OID
                optiga_cmd_prepare_tag_header(CMD_DECRYPT_ASYM_SESSION_OID_TAG,
                                              CMD_DECRYPT_ASYM_SESSION_OID_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              &index_for_data);
                //OID of decrypt key
                optiga_common_set_uint16(&me->p_optiga->optiga_comms_buffer[index_for_data],
                                         me->session_oid);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            }

            // prepare apdu
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_DECRYPT_ASYM,
                                           me->cmd_param,
                                           (index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for asymmetric decryption command...");
            // check if the write was successful
            if (OPTIGA_CMD_APDU_FAILURE == me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                if (NULL != p_optiga_decrypt_asym->processed_message_length)
                {
                    *p_optiga_decrypt_asym->processed_message_length = 0;
                }
                OPTIGA_CMD_LOG_MESSAGE("Error in processing asymmetric decryption response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            if ((NULL != p_optiga_decrypt_asym->processed_message) &&
                (NULL != p_optiga_decrypt_asym->processed_message_length))
            {
                optiga_common_get_uint16(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET +
                                         OPTIGA_CMD_NO_OF_BYTES_IN_TAG], &out_data_size);
                // if the received decrypted length is greater than the user provided decrypt buffer length
                if ((*(p_optiga_decrypt_asym->processed_message_length )) < out_data_size)
                {
                    OPTIGA_CMD_LOG_MESSAGE("Error in processing asymmetric decryption response...");
                    *(p_optiga_decrypt_asym->processed_message_length) = 0;
                    return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                    break;
                }

                pal_os_memcpy(p_optiga_decrypt_asym->processed_message,
                              &me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + OPTIGA_CMD_APDU_TL_LENGTH],
                              out_data_size);
                *p_optiga_decrypt_asym->processed_message_length = out_data_size;
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of asymmetric decryption command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_decrypt_asym(optiga_cmd_t * me, uint8_t cmd_param, optiga_decrypt_asym_params_t * params)
{
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    optiga_decrypt_asym_params_t * p_optiga_decrypt_asym = (optiga_decrypt_asym_params_t*)params;
    optiga_cmd_state_t cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_LOCK;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    do
    {
        if ((NULL == p_optiga_decrypt_asym->processed_message) &&
            (NULL == p_optiga_decrypt_asym->processed_message_length))
        {
            cmd_handler_state = OPTIGA_CMD_EXEC_REQUEST_SESSION;
        }
        optiga_cmd_execute(me,
                           cmd_param,
                           optiga_cmd_decrypt_asym_handler,
                           cmd_handler_state,
                           params);

        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    return (return_status);
}
#endif // OPTIGA_CRYPT_RSA_DECRYPT_ENABLED

#ifdef OPTIGA_CRYPT_HASH_ENABLED
/*
* CalCHash handler
*/
_STATIC_H optiga_lib_status_t optiga_cmd_calc_hash_handler(optiga_cmd_t * me)
{
    optiga_calc_hash_params_t * p_optiga_calc_hash = (optiga_calc_hash_params_t *)me->p_input;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;
    uint16_t out_data_size;
    uint16_t length_to_hash;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending calculate hash command..");
            //Hash Input
            // tag setting
            me->chaining_ongoing = FALSE;
            p_optiga_calc_hash->chaining_status = FALSE;


            // add data if available
            if (NULL != p_optiga_calc_hash->p_hash_data)
            {
                //continue
                 *(me->p_optiga->optiga_comms_buffer + index_for_data++) = p_optiga_calc_hash->hash_sequence;
                 //lint --e{734} suppress "length_to_hash parameter is of uint16 type, while the arguments used for
                 //calculating are of uint32 type. The final value calculated never crosses uint16 max value and for
                 // future use case, explicit type-casting is not done "
                length_to_hash = MIN(((OPTIGA_MAX_COMMS_BUFFER_SIZE - (index_for_data + OPTIGA_CMD_HASH_HEADER_SIZE + OPTIGA_CMD_INTERMEDIATE_CONTEX_HEADER)) - 
                                       p_optiga_calc_hash->apparent_context_size),(p_optiga_calc_hash->p_hash_data->length - p_optiga_calc_hash->data_sent));

                // add length
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data), length_to_hash);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                              p_optiga_calc_hash->p_hash_data->buffer + p_optiga_calc_hash->data_sent,
                              length_to_hash);
                index_for_data += length_to_hash;

                p_optiga_calc_hash->data_sent += length_to_hash;
                if (p_optiga_calc_hash->data_sent != p_optiga_calc_hash->p_hash_data->length)
                {
                    me->chaining_ongoing = TRUE;
                    p_optiga_calc_hash->chaining_status = TRUE;
                }
            }
            else if (NULL != p_optiga_calc_hash->p_hash_oid)
            {
                //OID data
                optiga_cmd_prepare_tag_header(p_optiga_calc_hash->hash_sequence,
                                              OPTIGA_CMD_OID_DATA_LENGTH,
                                              me->p_optiga->optiga_comms_buffer,
                                              (uint16_t*)&index_for_data);

                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data),
                                          p_optiga_calc_hash->p_hash_oid->oid);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data),
                                          p_optiga_calc_hash->p_hash_oid->offset);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data),
                                          p_optiga_calc_hash->p_hash_oid->length);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;
            }
            else
            {
                optiga_cmd_prepare_tag_header (p_optiga_calc_hash->hash_sequence,
                                               OPTIGA_CMD_ZERO_LENGTH_OR_VALUE,
                                               me->p_optiga->optiga_comms_buffer,
                                               (uint16_t*)&index_for_data);
            }
            //Hash Context
            if ((NULL != p_optiga_calc_hash->p_hash_context) &&
                (0 != p_optiga_calc_hash->apparent_context_size))
            {
                *(me->p_optiga->optiga_comms_buffer + index_for_data++) = OPTIGA_CRYPT_INTERMEDIATE;
                optiga_common_set_uint16((me->p_optiga->optiga_comms_buffer + index_for_data),
                                          p_optiga_calc_hash->p_hash_context->context_buffer_length);
                index_for_data += OPTIGA_CMD_UINT16_SIZE_IN_BYTES;

                pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                              p_optiga_calc_hash->p_hash_context->context_buffer,
                              p_optiga_calc_hash->p_hash_context->context_buffer_length);

                index_for_data += p_optiga_calc_hash->p_hash_context->context_buffer_length;
                p_optiga_calc_hash->apparent_context_size = 0;
            }

            //Export Option
            if ((OPTIGA_CRYPT_HASH_FINAL != p_optiga_calc_hash->hash_sequence)&&
                (FALSE == me->chaining_ongoing))
            {
                optiga_cmd_prepare_tag_header(OPTIGA_CRYPT_HASH_CONTX_OUT,
                                              OPTIGA_CMD_ZERO_LENGTH_OR_VALUE,
                                              me->p_optiga->optiga_comms_buffer,
                                              (uint16_t*)&index_for_data);
            }

            //form apdu header
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_CALC_HASH,
                                           me->cmd_param,
                                           (uint16_t)(index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (uint16_t)(index_for_data - OPTIGA_COMMS_DATA_OFFSET);
            me->cmd_next_execution_state = OPTIGA_CMD_EXEC_PROCESS_RESPONSE;
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for calculate hash command...");
            // check if the write was successful
            if (OPTIGA_CMD_APDU_FAILURE == me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing calculate hash response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            if (OPTIGA_CRYPT_HASH_FINAL == p_optiga_calc_hash->hash_sequence)
            {
                if (OPTIGA_CRYPT_HASH_DIGEST_OUT != me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET])
                {
                   break;
                }
                optiga_common_get_uint16(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET +
                                         OPTIGA_CMD_NO_OF_BYTES_IN_TAG], &out_data_size);

                pal_os_memcpy(p_optiga_calc_hash->p_out_digest,
                              &me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                              OPTIGA_CMD_NO_OF_BYTES_IN_TAG], out_data_size);
            }
            else if ((OPTIGA_CRYPT_HASH_FINAL != p_optiga_calc_hash->hash_sequence) &&
                    (FALSE == p_optiga_calc_hash->chaining_status))
            {
                if (OPTIGA_CRYPT_INTERMEDIATE != me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET])
                {
                    break;
                }

                optiga_common_get_uint16(&me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET +
                                         OPTIGA_CMD_NO_OF_BYTES_IN_TAG], &out_data_size);
                if (p_optiga_calc_hash->p_hash_context->context_buffer_length < out_data_size)
                {
                    OPTIGA_CMD_LOG_MESSAGE("Error in processing calculate hash response...");
                    return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                    break;
                }

                pal_os_memcpy(p_optiga_calc_hash->p_hash_context->context_buffer,
                              &me->p_optiga->optiga_comms_buffer[OPTIGA_CMD_APDU_INDATA_OFFSET + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                              OPTIGA_CMD_NO_OF_BYTES_IN_TAG], out_data_size);
                p_optiga_calc_hash->p_hash_context->context_buffer_length = out_data_size;
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of calculate hash command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }

    return (return_status);
}

optiga_lib_status_t optiga_cmd_calc_hash(optiga_cmd_t * me,
                                         uint8_t cmd_param,
                                         optiga_calc_hash_params_t * params)
{
    optiga_calc_hash_params_t * p_optiga_calc_hash = (optiga_calc_hash_params_t*)params;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    
    p_optiga_calc_hash->data_sent = 0;

    optiga_cmd_execute(me,
                       cmd_param,
                       optiga_cmd_calc_hash_handler,
                       OPTIGA_CMD_EXEC_REQUEST_LOCK,
                       params);

    return (OPTIGA_LIB_SUCCESS);
}
#endif //OPTIGA_CRYPT_HASH_ENABLED



/*
* Set Data Object handler for protected update
*/
_STATIC_H optiga_lib_status_t optiga_cmd_set_object_protected_handler(optiga_cmd_t * me)
{
    uint16_t total_apdu_length;
    optiga_set_object_protected_params_t * p_optiga_write_protected_data = (optiga_set_object_protected_params_t *)me->p_input;
    optiga_lib_status_t return_status = OPTIGA_CMD_ERROR;
    uint16_t index_for_data = OPTIGA_CMD_APDU_INDATA_OFFSET;

    switch ((uint8_t)me->cmd_next_execution_state)
    {
        case OPTIGA_CMD_EXEC_PREPARE_COMMAND:
        {
            OPTIGA_CMD_LOG_MESSAGE("Sending set data object command..");
            me->chaining_ongoing = FALSE;

            // APDU header size + Set Object protected tag 1 bytes + length of buffer 2 bytes + size of data to send
            total_apdu_length = OPTIGA_CMD_APDU_HEADER_SIZE + OPTIGA_CMD_NO_OF_BYTES_IN_TAG + OPTIGA_CMD_UINT16_SIZE_IN_BYTES +
                                p_optiga_write_protected_data->p_protected_update_buffer_length;
            if (OPTIGA_MAX_COMMS_BUFFER_SIZE < total_apdu_length)
            {
                return_status = OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT;
                break;
            }

            // Copy the tag and protected length
            optiga_cmd_prepare_tag_header((CMD_SET_OBJECT_PROTECTED_TAG |
                                           (uint8_t)p_optiga_write_protected_data->set_obj_protected_tag),
                                          p_optiga_write_protected_data->p_protected_update_buffer_length,
                                          me->p_optiga->optiga_comms_buffer,
                                          &index_for_data);
            // data to be written
            pal_os_memcpy(me->p_optiga->optiga_comms_buffer + index_for_data,
                          p_optiga_write_protected_data->p_protected_update_buffer,
                          p_optiga_write_protected_data->p_protected_update_buffer_length);

            index_for_data += p_optiga_write_protected_data->p_protected_update_buffer_length;
            // prepare apdu
            optiga_cmd_prepare_apdu_header(OPTIGA_CMD_SET_OBJECT_PROTECTED,
                                           me->cmd_param,
                                           (index_for_data - OPTIGA_CMD_APDU_INDATA_OFFSET),
                                           me->p_optiga->optiga_comms_buffer + OPTIGA_COMMS_DATA_OFFSET);

            me->p_optiga->comms_tx_size = (index_for_data - OPTIGA_COMMS_DATA_OFFSET);

            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;

        case OPTIGA_CMD_EXEC_PROCESS_RESPONSE:
        {
            OPTIGA_CMD_LOG_MESSAGE("Processing response for set data object data command...");
            // check if the write was successful
            if (OPTIGA_CMD_APDU_SUCCESS != me->p_optiga->optiga_comms_buffer[OPTIGA_COMMS_DATA_OFFSET])
            {
                OPTIGA_CMD_LOG_MESSAGE("Error in processing set data object response...");
                SET_DEV_ERROR_NOTIFICATION(OPTIGA_CMD_EXIT_HANDLER_CALL);
                break;
            }
            if (OPTIGA_SET_PROTECTED_UPDATE_FINAL == p_optiga_write_protected_data->set_obj_protected_tag)
            {
                me->cmd_next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
            }
            else
            {
                me->cmd_next_execution_state = OPTIGA_CMD_STATE_EXIT;
                pal_os_event_start(me->p_optiga->p_pal_os_event_ctx, optiga_cmd_queue_scheduler, me->p_optiga);
            }
            OPTIGA_CMD_LOG_MESSAGE("Response of set data object command is processed...");
            return_status = OPTIGA_LIB_SUCCESS;
        }
        break;
        default:
            break;
    }
    return (return_status);
}

optiga_lib_status_t optiga_cmd_set_object_protected(optiga_cmd_t * me, uint8_t cmd_param,
                                                    optiga_set_object_protected_params_t * params)
{
    optiga_cmd_state_t next_execution_state = OPTIGA_CMD_EXEC_REQUEST_STRICT_LOCK;
    optiga_lib_status_t return_status = OPTIGA_LIB_SUCCESS;
    OPTIGA_CMD_LOG_MESSAGE(__FUNCTION__);
    
    if (((OPTIGA_SET_PROTECTED_UPDATE_FINAL == params->set_obj_protected_tag) ||
       (OPTIGA_SET_PROTECTED_UPDATE_CONTINUE == params->set_obj_protected_tag)) &&
       ((OPTIGA_CMD_QUEUE_REQUEST_STRICT_LOCK != optiga_cmd_queue_get_state_of(me, OPTIGA_CMD_QUEUE_SLOT_LOCK_TYPE)) ||
       (OPTIGA_CMD_QUEUE_PROCESSING != optiga_cmd_queue_get_state_of(me, OPTIGA_CMD_QUEUE_SLOT_STATE))))
    {
        return_status = OPTIGA_CMD_ERROR_INVALID_INPUT;
    }
    else
    {
        if (( NULL == params->p_protected_update_buffer ) &&
            (OPTIGA_SET_PROTECTED_UPDATE_FINAL == params->set_obj_protected_tag))
        {
            /// Release the strict sequence
            next_execution_state = OPTIGA_CMD_EXEC_RELEASE_LOCK;
        }
        optiga_cmd_execute(me,
                           cmd_param,
                           optiga_cmd_set_object_protected_handler,
                           next_execution_state,
                           params);
    }

    return (return_status);
}


/**
* @}
*/
