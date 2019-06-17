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
* \file ifx_i2c_config.h
*
* \brief   This file defines the structures and macros for the Infineon I2C Protocol.
*
* \ingroup  grIFXI2C
*
* @{
*/

#ifndef _IFX_I2C_CONFIG_H_
#define _IFX_I2C_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IFX_I2C_CONFIG_EXTERNAL
    #include "ifx_i2c_config_external.h"   
#else
// Protocol Stack Includes
#include "optiga/pal/pal_i2c.h"
#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_os_timer.h"
#include "optiga/pal/pal_os_datastore.h"
#include "optiga/optiga_lib_config.h"

/** @brief I2C slave address of the Infineon device */
#define IFX_I2C_BASE_ADDR           (0x30)

/** @brief Physical Layer: polling interval in microseconds */
#define PL_POLLING_INVERVAL_US      (1000U)
/** @brief Physical layer: maximal attempts */
#define PL_POLLING_MAX_CNT          (200U)
/** @brief Physical Layer: data register polling interval in microseconds */
#define PL_DATA_POLLING_INVERVAL_US (5000U)
/** @brief Physical Layer: guard time interval in microseconds */
#define PL_GUARD_TIME_INTERVAL_US   (50U)

/** @brief Data link layer: maximum frame size */
#ifndef DL_MAX_FRAME_SIZE
#define DL_MAX_FRAME_SIZE           (300U)
#endif

/** @brief Transport Layer: header size */
#define TL_HEADER_SIZE              (1U)
/** @brief Data link layer: header size */
#define DL_HEADER_SIZE              (5U)
/** @brief Data link layer: maximum number of retries in case of transmission error */
#define DL_TRANS_REPEAT             (3U)
/** @brief Data link layer: Trans timeout in milliseconds*/
#define PL_TRANS_TIMEOUT_MS         (10U)

/** @brief Transport layer: Maximum exit timeout in seconds */
#define TL_MAX_EXIT_TIMEOUT         (180U)

/** @brief Reset low time for GPIO pin toggling */
#define RESET_LOW_TIME_MSEC         (2000U)
/** @brief Start up time */
#define STARTUP_TIME_MSEC           (12000U)

/** @brief Protocol Stack: Status codes for success */
#define IFX_I2C_STACK_SUCCESS       (0x0000)
/** @brief Protocol Stack: Status codes busy */
#define IFX_I2C_STACK_BUSY          (0x0001)
/** @brief Protocol Stack: Status codes for error */
#define IFX_I2C_STACK_ERROR         (0x0102)
/** @brief Protocol Stack: Memory insufficient */
#define IFX_I2C_STACK_MEM_ERROR     (0x0104)
/** @brief Protocol Stack: Fatal error. Used internal to IFX I2C Stack */
#define IFX_I2C_FATAL_ERROR         (0x0106)
/** @brief Protocol Stack: handshake error */
#define IFX_I2C_HANDSHAKE_ERROR     (0x0107)
/** @brief Protocol Stack: session error */
#define IFX_I2C_SESSION_ERROR       (0x0108)

/** @brief Offset of Datalink header in tx_frame_buffer */
#define IFX_I2C_DL_HEADER_OFFSET    (0U)
/** @brief Offset of Transport header in tx_frame_buffer */
#define IFX_I2C_TL_HEADER_OFFSET    (IFX_I2C_DL_HEADER_OFFSET + 3)
/** @brief Protocol Stack debug switch for physical layer (set to 0 or 1) */
#define IFX_I2C_LOG_PL              (0U)
/** @brief Protocol Stack debug switch for data link layer (set to 0 or 1) */
#define IFX_I2C_LOG_DL              (0U)
/** @brief Protocol Stack debug switch for transport layer (set to 0 or 1) */
#define IFX_I2C_LOG_TL              (0U)
/** @brief Protocol Stack debug switch for presentation layer (set to 0 or 1) */
#define IFX_I2C_LOG_PRL             (0U)

/** @brief Log ID number for physical layer */
#define IFX_I2C_LOG_ID_PL           (0x00)
/** @brief Log ID number for data link layer */
#define IFX_I2C_LOG_ID_DL           (0x01)
/** @brief Log ID number for transport layer */
#define IFX_I2C_LOG_ID_TL           (0x02)
/** @brief Log ID number for presentation layer */
#define IFX_I2C_LOG_ID_PRL          (0x03)
/** @brief Log ID number for platform abstraction layer */
#define IFX_I2C_LOG_ID_PAL          (0x04)


#if defined IFX_I2C_PRESENTATION_LAYER_TRANSPARENT
    #define IFX_I2C_TL_ENABLE              (1U)

    #define IFX_I2C_PRESENCE_BIT           (0x08)
    #define IFX_I2C_PRL_MAC_SIZE           (0x08)
    #define IFX_I2C_PRL_HEADER_SIZE        (0x05)
    /// Overhead buffer size for user buffer
    #define IFX_I2C_PRL_OVERHEAD_SIZE      (IFX_I2C_PRL_HEADER_SIZE + IFX_I2C_PRL_MAC_SIZE)
    /// Offset for data
    #define IFX_I2C_DATA_OFFSET            (IFX_I2C_PRL_HEADER_SIZE)
    #define IFX_I2C_PRESENCE_BIT_CHECK     (0x08)
#endif

#if defined OPTIGA_COMMS_SHIELDED_CONNECTION
    #define IFX_I2C_PRL_ENABLED            (1U)
    #define IFX_I2C_TL_ENABLE              (1U)

    #define IFX_I2C_PRESENCE_BIT           (0x08)
    #define IFX_I2C_PRL_MAC_SIZE           (0x08)
    #define IFX_I2C_PRL_HEADER_SIZE        (0x05)
    /// Overhead buffer size for user buffer
    #define IFX_I2C_PRL_OVERHEAD_SIZE      (IFX_I2C_PRL_HEADER_SIZE + IFX_I2C_PRL_MAC_SIZE)
    /// Offset for data
    #define IFX_I2C_DATA_OFFSET            (IFX_I2C_PRL_HEADER_SIZE)
    #define IFX_I2C_PRESENCE_BIT_CHECK     (0x08)
#else
    #define IFX_I2C_PRESENCE_BIT           (0x00)
    #define IFX_I2C_PRL_MAC_SIZE           (0x00)
    #define IFX_I2C_PRL_HEADER_SIZE        (0x00)
    /// Overhead buffer size for user buffer
    #define IFX_I2C_PRL_OVERHEAD_SIZE      (IFX_I2C_PRL_HEADER_SIZE + IFX_I2C_PRL_MAC_SIZE)
    /// Offset for data
    #define IFX_I2C_DATA_OFFSET            (IFX_I2C_PRL_HEADER_SIZE)
    #define IFX_I2C_PRESENCE_BIT_CHECK     (0x00)
#endif

/** @brief Pre shared secret protocol version*/
#define PROTOCOL_VERSION_PRE_SHARED_SECRET   (0x01)
/** @brief To restore saved secure session */
#define IFX_I2C_SESSION_CONTEXT_RESTORE    (0x11)
/** @brief To store  active secure session */
#define IFX_I2C_SESSION_CONTEXT_SAVE       (0x22)
/** @brief For no manage context operation */
#define IFX_I2C_SESSION_CONTEXT_NONE       (0x33)

/** @brief When data from master and slave is unprotected */
#define NO_PROTECTION               (0x00)
/** @brief When data from master is protected and data from slave is unprotected */
#define MASTER_PROTECTION           (0x01)
/** @brief When data from master is unprotected and data from is slave protected */
#define SLAVE_PROTECTION            (0x02)
/** @brief When data from master and slave protected */
#define FULL_PROTECTION             (0x03)
/** @brief To re-establish secure channel */
#define RE_ESTABLISH                (0x80)


typedef struct ifx_i2c_context ifx_i2c_context_t;

/** @brief Event handler function prototype */
typedef void ( * ifx_i2c_event_handler_t)(struct ifx_i2c_context * p_ctx,
                                          optiga_lib_status_t event,
                                          const uint8_t * data,
                                          uint16_t data_len);

/** @brief Physical layer structure */
typedef struct ifx_i2c_pl
{
    // Physical Layer low level interface variables

    /// Physical layer buffer
    uint8_t buffer[DL_MAX_FRAME_SIZE + 1];
    /// Tx length
    uint16_t buffer_tx_len;
    /// Rx length
    uint16_t buffer_rx_len;
    /// Action on register, read/write
    uint8_t  register_action;
    /// i2c read/i2c write
    uint8_t  i2c_cmd;
    /// Retry counter
    uint16_t retry_counter;

    // Physical Layer high level interface variables

    /// Action of frame. Tx/Rx
    uint8_t   frame_action;
    /// Frame state
    uint8_t   frame_state ;
    /// Pointer to data to be sent
    uint8_t * p_tx_frame;
    /// Length of data to be sent
    uint16_t  tx_frame_len;
    // Upper layer handler
    ifx_i2c_event_handler_t upper_layer_event_handler;

    // Physical Layer negotiation/soft reset variables

    /// Negotiation state
    uint8_t   negotiate_state;
    /// Soft reset requested
    uint8_t   request_soft_reset;
} ifx_i2c_pl_t;

/** @brief Datalink layer structure */
typedef struct ifx_i2c_dl
{
    // Data Link layer internal state variables

    /// Datalink layer state
    uint8_t state;
    /// Tx sequence number
    uint8_t tx_seq_nr;
    // Rx sequence number
    uint8_t rx_seq_nr;
    /// Indicate only Rx required
    uint8_t action_rx_only;
    /// Retransmit counter
    uint8_t retransmit_counter;
    /// Error occured
    uint8_t error;
    /// Resynced
    uint8_t resynced;
    /// Timeout value
    uint32_t data_poll_timeout;
    /// Transmit buffer size
    uint16_t tx_buffer_size;
    /// Receive buffer size
    uint16_t rx_buffer_size;
    /// Pointer to main transmit buffers
    uint8_t * p_tx_frame_buffer;
    /// Pointer to main receive buffers
    uint8_t * p_rx_frame_buffer;
    ///Start time of sending frame
    uint32_t frame_start_time;
    // Upper layer Event handler
    ifx_i2c_event_handler_t upper_layer_event_handler;
} ifx_i2c_dl_t;

/** @brief Transport layer structure */
typedef struct ifx_i2c_tl
{
    // Transport Layer state and buffer

    /// Transport layer state
    uint8_t  state;
    /// Pointer to packet provided by user
    uint8_t * p_actual_packet;
    /// Total received data
    uint16_t total_recv_length;
    /// Actual length of user provided packet
    uint16_t actual_packet_length;
    /// Offset till which data is sent from p_actual_packet
    uint16_t packet_offset;
    /// Maximum length of packet at transport layer
    uint16_t max_packet_length;
    /// Pointer to user provided receive buffer
    uint8_t * p_recv_packet_buffer;
    /// Length of receive buffer
    uint16_t * p_recv_packet_buffer_length;
    /// Start time of the transport layer API
    uint32_t api_start_time;
    ///Chaining error coutn from slave
    uint8_t chaining_error_count;
    ///Chaining error count for master
    uint8_t master_chaining_error_count;
    ///State to check last chaining state
    uint8_t previous_chaining;
    /// transmission done
    uint8_t transmission_completed;
    /// Error event state
    optiga_lib_status_t error_event;
    ///Tl rx payload copy offset
    uint8_t payload_offset;
    ///Tl tx payload copy offset
    uint8_t tx_payload_offset;
    ///Initial state check
    uint8_t initialization_state;

    /// Upper layer event handler
    ifx_i2c_event_handler_t upper_layer_event_handler;
} ifx_i2c_tl_t;

#if defined OPTIGA_COMMS_SHIELDED_CONNECTION

/** @brief Presentation layer manage context structure */
typedef struct ifx_i2c_prl_manage_context
{
    ///Buffer to store session key
    uint8_t session_key[40];
    /// Master retransmit counter
    uint8_t decryption_failure_counter;
    /// Slave retransmit counter
    uint8_t data_retransmit_counter;
    /// Negotiation state
    uint8_t negotiation_state;
    ///Stored context flag
    uint8_t stored_context_flag;
   /// Master sequence number
    uint32_t master_sequence_number;
    ///Save slave sequence number
    uint32_t save_slave_sequence_number;
}ifx_i2c_prl_manage_context_t;

/** @brief Data store configuration structure */
typedef struct ifx_i2c_datastore_config
{
    /// Protocol version
    uint8_t  protocol_version;
    /// ID to read and write the shared secret
    uint16_t datastore_shared_secret_id;
    /// ID to read and write the shielded connection context data
    uint16_t datastore_manage_context_id;
    /// Length of shared secret
    uint16_t shared_secret_length;
} ifx_i2c_datastore_config_t;


/** @brief Presentation layer structure */
typedef struct ifx_i2c_prl
{
    // Presentation layer state
    uint8_t state;
    // Handshake state
    uint8_t hs_state;
    /// Handshake negotiation state
    uint8_t negotiation_state;
    ///Manage context state
    uint8_t mc_state;
    /// Master sequence number
    uint32_t master_sequence_number;
    /// Slave sequence number
    uint32_t slave_sequence_number;
    ///Save slave sequence number
    uint32_t save_slave_sequence_number;
    /// Pointer to packet provided by user
    uint8_t * p_actual_payload;
    /// Total received data
    uint16_t actual_payload_length;
    /// Pointer to user provided receive buffer
    uint8_t * p_recv_payload_buffer;
    /// Length of receive buffer
    uint16_t * p_recv_payload_buffer_length;
    ///SCTR status byte
    uint8_t sctr;
    ///Saved SCTR status byte
    uint8_t saved_sctr;
    ///Alert type
    uint8_t alert_type;
    ///Presentation header offset
    uint8_t prl_header_offset;
    ///Buffer to store prf
    uint8_t session_key[40];
    /// Randon data
    uint8_t random[32];
    /// Receive buffer
    uint8_t prl_txrx_buffer[58];
    /// Receive txrx buffer length
    uint16_t prl_txrx_receive_length;
    /// Associate data buffer
    uint8_t associate_data[8];
    /// Receive buffer length
    uint16_t prl_receive_length;
    /// Master retransmit counter
    uint8_t decryption_failure_counter;
    /// Slave retransmit counter
    uint8_t data_retransmit_counter;
    /// Return status
    optiga_lib_status_t return_status;
    ///Restore context flag
    uint8_t restore_context_flag;
    //Context to be stored
    ifx_i2c_prl_manage_context_t prl_saved_ctx;
    // Upper layer Event handler
    ifx_i2c_event_handler_t upper_layer_event_handler;
    // Trans repeat status 
    uint8_t trans_repeat_status;
}ifx_i2c_prl_t;
#endif

/** @brief IFX I2C context structure */
typedef struct ifx_i2c_context
{
    /// I2C Slave address
    uint8_t slave_address;
    /// Frequency of i2c master
    uint16_t frequency;
    /// Data link layer frame size
    uint16_t frame_size;
    /// Pointer to pal gpio context for vdd
    pal_gpio_t * p_slave_vdd_pin;
    /// Pointer to pal gpio context for reset
    pal_gpio_t * p_slave_reset_pin;
    /// Pointer to pal i2c context
    pal_i2c_t * p_pal_i2c_ctx;
#if defined OPTIGA_COMMS_SHIELDED_CONNECTION    
    /// Datastore configuration instance for prl
    ifx_i2c_datastore_config_t * ifx_i2c_datastore_config;
#endif    
    /// Upper layer event handler
    upper_layer_callback_t upper_layer_event_handler;
    /// Upper layer context
    void * p_upper_layer_ctx;
    /// Pointer to upper layer rx buffer
    uint8_t * p_upper_layer_rx_buffer;
    /// Pointer to length of upper layer rx buffer
    uint16_t * p_upper_layer_rx_buffer_len;

    /// Protocol variables
    /// ifx i2c wrapper apis state
    uint8_t state;
    /// ifx i2c wrapper api status
    uint8_t status;
    /// reset states
    uint8_t reset_state;
    /// Close states
    optiga_lib_status_t close_state;
    /// type of reset
    uint8_t reset_type;
    /// init pal
    uint8_t do_pal_init;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    // protection level:
    //0(master unprotected and slave unprotected),1(master protected and slave unprotected),
    //2(master unprotected and slave protected),3(master protected and slave protected),
    //255(re-negotiate)
    uint8_t protection_level;
    ///Supported presentation layer protocol version
    uint8_t protocol_version;
    ///Variable to indicate manage context operation
    uint8_t manage_context_operation;
#endif
    /// Transport layer context
    ifx_i2c_tl_t tl;
    /// Datalink layer context
    ifx_i2c_dl_t dl;
    /// Physical layer context
    ifx_i2c_pl_t pl;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    /// Presentation layer context
    ifx_i2c_prl_t prl;
#endif
    /// IFX I2C tx frame of max length
    uint8_t tx_frame_buffer[DL_MAX_FRAME_SIZE];
    /// IFX I2C rx frame of max length
    uint8_t rx_frame_buffer[DL_MAX_FRAME_SIZE];
    void * pal_os_event_ctx;

} ifx_i2c_context_t;

/** @brief IFX I2C Instance */
extern ifx_i2c_context_t ifx_i2c_context_0;

#endif

#ifdef __cplusplus
}
#endif
#endif /* _IFX_I2C_CONFIG_H_ */

/**
 * @}
 **/
