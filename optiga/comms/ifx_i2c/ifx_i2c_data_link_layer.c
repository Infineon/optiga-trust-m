/**
* \copyright
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
* \endcopyright
*
* \author Infineon Technologies AG
*
* \file ifx_i2c_data_link_layer.c
*
* \brief   This file implements the APIs for data link layer of the Infineon I2C Protocol Stack library.
*
* \ingroup  grIFXI2C
*
* @{
*/

#include "optiga/ifx_i2c/ifx_i2c_data_link_layer.h"
#include "optiga/ifx_i2c/ifx_i2c_physical_layer.h"

/// @cond hidden

// Data Link layer internal states
#define DL_STATE_UNINIT                 (0x00)
#define DL_STATE_IDLE                   (0x01)
#define DL_STATE_TX                     (0x02)
#define DL_STATE_RX                     (0x03)
#define DL_STATE_ACK                    (0x04)
#define DL_STATE_RESEND                 (0x05)
#define DL_STATE_NACK                   (0x06)
#define DL_STATE_ERROR                  (0x08)
#define DL_STATE_DISCARD                (0x09)
#define DL_STATE_RX_DF                  (0x0A)
#define DL_STATE_RX_CF                  (0x0B)

// Data Link Layer Frame Control Constants
#define DL_FCTR_FTYPE_MASK              (0x80)
#define DL_FCTR_FTYPE_OFFSET            (7U)
#define DL_FCTR_VALUE_CONTROL_FRAME     (0x01)

#define DL_FCTR_SEQCTR_MASK             (0x60)
#define DL_FCTR_SEQCTR_OFFSET           (5U)
#define DL_FCTR_SEQCTR_VALUE_ACK        (0x00)
#define DL_FCTR_SEQCTR_VALUE_NACK       (0x01)
#define DL_FCTR_SEQCTR_VALUE_RESYNC     (0x02)
#define DL_FCTR_SEQCTR_VALUE_RFU        (0x03)

#define DL_FCTR_FRNR_MASK               (0x0C)
#define DL_FCTR_FRNR_OFFSET             (2U)

#define DL_FCTR_ACKNR_MASK              (0x03)
#define DL_FCTR_ACKNR_OFFSET            (0U)

// Data Link Layer frame counter max value
#define DL_MAX_FRAME_NUM                (0x03)

// Data link layer length
#define DL_CONTROL_FRAME_LENGTH         (5U)

// Seconds to milliseconds
#define DL_SEC_TO_MSECS                 (1000U)

#if defined (OPTIGA_LIB_ENABLE_LOGGING) && defined (OPTIGA_LIB_ENABLE_COMMS_LOGGING)

// Logs the message provided from OPTIGA Comms layer
#define OPTIGA_COMMS_LOG_MESSAGE(msg) \
{\
    optiga_lib_print_message(msg,OPTIGA_COMMUNICATION_LAYER,OPTIGA_COMMUNICATION_LAYER_COLOR);\
}

// Logs the status info provided from OPTIGA Comms layer
//lint --e{750} suppress "The unused OPTIGA_COMMS_LOG_STATUS macro is kept for future enhancements"
#define OPTIGA_COMMS_LOG_STATUS(return_value)                                 \
{                                                       \
    if (OPTIGA_LIB_SUCCESS != return_value)                                 \
    {                                                     \
        optiga_lib_print_status(OPTIGA_COMMUNICATION_LAYER,OPTIGA_ERROR_COLOR,return_value);        \
    }                                                     \
    else                                                  \
    {                                                     \
        optiga_lib_print_status(OPTIGA_COMMUNICATION_LAYER,OPTIGA_COMMUNICATION_LAYER_COLOR,return_value);  \
    }                                                     \
}

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION

// Logs the transmitted data to OPTIGA from IFXI2C layer in hexadecimal format
#define OPTIGA_IFXI2C_LOG_TRANSMIT_HEX_DATA(array,array_len,p_ifx_i2c_ctx)              \
{                                                 \
  if (0 != frame_len)                                       \
  {                                               \
    if (((((ifx_i2c_context_t * )p_ifx_i2c_ctx)->protection_level & 0x0F) == 0x01) ||       \
      ((((ifx_i2c_context_t * )p_ifx_i2c_ctx)->protection_level & 0x0F) == 0x03))       \
      {                                             \
        optiga_lib_print_array_hex_format(array,array_len,OPTIGA_PROTECTED_DATA_COLOR);  \
      }                                             \
      else                                          \
      {                                             \
        optiga_lib_print_array_hex_format(array,array_len,OPTIGA_UNPROTECTED_DATA_COLOR);\
      }                                             \
  }                                               \
}

// Logs the received data from OPTIGA in hexadecimal format
#define OPTIGA_IFXI2C_LOG_RECEIVE_HEX_DATA(array,array_len,p_ifx_i2c_ctx) \
{\
    if (((((ifx_i2c_context_t * )p_ifx_i2c_ctx)->protection_level & 0x0F) == 0x02) || \
        ((((ifx_i2c_context_t * )p_ifx_i2c_ctx)->protection_level & 0x0F) == 0x03)) \
        { \
            optiga_lib_print_array_hex_format(array,array_len,OPTIGA_PROTECTED_DATA_COLOR); \
        } \
        else \
        { \
            optiga_lib_print_array_hex_format(array,array_len,OPTIGA_UNPROTECTED_DATA_COLOR); \
        } \
}
#else

// Logs the transmitted data to OPTIGA from IFXI2C layer in hexadecimal format
#define OPTIGA_IFXI2C_LOG_TRANSMIT_HEX_DATA(array,array_len,p_ifx_i2c_ctx) \
{\
    OPTIGA_COMMS_LOG_MESSAGE("");\
    optiga_lib_print_array_hex_format(array,array_len,OPTIGA_UNPROTECTED_DATA_COLOR);\
}

// Logs the received data from OPTIGA in hexadecimal format
#define OPTIGA_IFXI2C_LOG_RECEIVE_HEX_DATA(array,array_len,p_ifx_i2c_ctx) \
{\
    OPTIGA_COMMS_LOG_MESSAGE("");\
    optiga_lib_print_array_hex_format(array,array_len,OPTIGA_UNPROTECTED_DATA_COLOR);\
}

#endif

#else

#define OPTIGA_IFXI2C_LOG_TRANSMIT_HEX_DATA(array,array_len,p_ifx_i2c_ctx) {}
#define OPTIGA_IFXI2C_LOG_RECEIVE_HEX_DATA(array,array_len,p_ifx_i2c_ctx) {}
#define OPTIGA_COMMS_LOG_MESSAGE(msg) {}
//lint --e{750} suppress "The unused OPTIGA_COMMS_LOG_STATUS macro is kept for future enhancements"
#define OPTIGA_COMMS_LOG_STATUS(return_value) {}
#endif

// Setup debug log statements
#if IFX_I2C_LOG_DL == 1
#define LOG_DL IFX_I2C_LOG
#else
#define LOG_DL(...) //printf(__VA_ARGS__)
#endif

/// Helper function to calculate CRC of a byte
_STATIC_H uint16_t ifx_i2c_dl_calc_crc_byte(uint16_t seed,
                                                       uint8_t byte);
/// Helper function to calculate CRC of a frame
_STATIC_H uint16_t ifx_i2c_dl_calc_crc(const uint8_t * p_data,
                                                  uint16_t data_len);
/// Internal function to send frame
_STATIC_H optiga_lib_status_t ifx_i2c_dl_send_frame_internal(ifx_i2c_context_t * p_ctx,
                                                             uint16_t frame_len,
                                                             uint8_t seqctr_value,
                                                             uint8_t resend);
/// Helper function to send resync
_STATIC_H optiga_lib_status_t ifx_i2c_dl_resync(ifx_i2c_context_t * p_ctx);
/// Helper function to resend frame
_STATIC_H void ifx_i2c_dl_resend_frame(ifx_i2c_context_t * p_ctx,
                                       uint8_t seqctr_value);
/// Data Link Layer state machine
_STATIC_H void ifx_i2c_pl_event_handler(ifx_i2c_context_t * p_ctx,
                                        optiga_lib_status_t event,
                                        const uint8_t * p_data,
                                        uint16_t data_len);

/// @endcond

optiga_lib_status_t ifx_i2c_dl_init(ifx_i2c_context_t * p_ctx, ifx_i2c_event_handler_t handler)
{
    LOG_DL("[IFX-DL]: Init\n");

    p_ctx->dl.state = DL_STATE_UNINIT;
    // Initialize Physical Layer (and register event handler)
    if (IFX_I2C_STACK_SUCCESS != ifx_i2c_pl_init(p_ctx, ifx_i2c_pl_event_handler))
    {
        return (IFX_I2C_STACK_ERROR);
    }

    // Initialize internal variables
    p_ctx->dl.upper_layer_event_handler = handler;
    p_ctx->dl.state = DL_STATE_IDLE;
    p_ctx->dl.tx_seq_nr = DL_MAX_FRAME_NUM;
    p_ctx->dl.rx_seq_nr = DL_MAX_FRAME_NUM;
    p_ctx->dl.resynced = 0;
    p_ctx->dl.error = 0;
    p_ctx->dl.p_tx_frame_buffer = p_ctx->tx_frame_buffer;
    p_ctx->dl.p_rx_frame_buffer = p_ctx->rx_frame_buffer;

    return IFX_I2C_STACK_SUCCESS;
}

optiga_lib_status_t ifx_i2c_dl_send_frame(ifx_i2c_context_t * p_ctx, uint16_t frame_len)
{
    LOG_DL("[IFX-DL]: Start TX Frame\n");
    // State must be idle and payload available
    if (p_ctx->dl.state != DL_STATE_IDLE || (0 == frame_len))
    {
        return (IFX_I2C_STACK_ERROR);
    }

    p_ctx->dl.state = DL_STATE_TX;
    p_ctx->dl.retransmit_counter = 0;
    p_ctx->dl.action_rx_only = 0;
    p_ctx->dl.tx_buffer_size = frame_len;
    p_ctx->dl.data_poll_timeout = PL_TRANS_TIMEOUT_MS;

    return (ifx_i2c_dl_send_frame_internal(p_ctx, frame_len, DL_FCTR_SEQCTR_VALUE_ACK, 0));
}

optiga_lib_status_t ifx_i2c_dl_receive_frame(ifx_i2c_context_t * p_ctx)
{
    LOG_DL("[IFX-DL]: Start RX Frame\n");

    if (DL_STATE_IDLE != p_ctx->dl.state)
    {
        return (IFX_I2C_STACK_ERROR);
    }

    // Set internal state
    p_ctx->dl.state = DL_STATE_RX;
    p_ctx->dl.retransmit_counter = 0;
    p_ctx->dl.action_rx_only = 1;
    p_ctx->dl.frame_start_time = pal_os_timer_get_time_in_milliseconds();
    p_ctx->dl.data_poll_timeout = TL_MAX_EXIT_TIMEOUT * DL_SEC_TO_MSECS;

    return (ifx_i2c_pl_receive_frame(p_ctx));
}

_STATIC_H uint16_t ifx_i2c_dl_calc_crc_byte(uint16_t seed, uint8_t byte)
{
    uint16_t h1;
    uint16_t h2;
    uint16_t h3;
    uint16_t h4;

    h1 = (seed ^ byte) & 0xFF;
    h2 = h1 & 0x0F;
    h3 = ((uint16_t)(h2 << 4)) ^ h1;
    h4 = h3 >> 4;

    return ((uint16_t)((((uint16_t)((((uint16_t)(h3 << 1)) ^ h4) << 4)) ^ h2) << 3)) ^ h4 ^ (seed >> 8);
}

_STATIC_H uint16_t ifx_i2c_dl_calc_crc(const uint8_t * p_data, uint16_t data_len)
{
    uint16_t i;
    uint16_t crc = 0;

    for (i = 0; i < data_len; i++)
    {
        crc = ifx_i2c_dl_calc_crc_byte(crc, p_data[i]);
    }

    return (crc);
}

_STATIC_H optiga_lib_status_t ifx_i2c_dl_send_frame_internal(ifx_i2c_context_t * p_ctx,
                                                             uint16_t frame_len,
                                                             uint8_t seqctr_value,
                                                             uint8_t resend)
{
    uint16_t crc;
    uint16_t ack_nr = p_ctx->dl.rx_seq_nr;
    uint8_t * p_buffer;

    LOG_DL("[IFX-DL]: TX Frame len %d\n", frame_len);
    // In case of sending a NACK the next frame is referenced
    if (DL_FCTR_SEQCTR_VALUE_NACK == seqctr_value)
    {
        ack_nr = (p_ctx->dl.rx_seq_nr + 1) & DL_MAX_FRAME_NUM;
    }
    p_buffer = p_ctx->dl.p_tx_frame_buffer;
    if (DL_FCTR_SEQCTR_VALUE_RESYNC == seqctr_value)
    {
        ack_nr = 0;
        // Use rx buffer to send resync
        p_buffer = p_ctx->dl.p_rx_frame_buffer;
    }
    // Use tx buffer when discarding a frame and send nakc for later received frame
    if ((DL_FCTR_SEQCTR_VALUE_ACK == seqctr_value) && (DL_STATE_DISCARD == p_ctx->dl.state))
    {
        p_buffer = p_ctx->dl.p_rx_frame_buffer;
    }

    // Set sequence control value (ACK or NACK) and referenced frame number
    //lint --e{835} suppress "DL_FCTR_ACKNR_OFFSET macro is defined as 0x00 and is kept for future enhancements"
    p_buffer[0] = (uint8_t)(ack_nr << DL_FCTR_ACKNR_OFFSET);
    p_buffer[0] |= (uint8_t)(seqctr_value << DL_FCTR_SEQCTR_OFFSET);

    if (0 != frame_len) // Data frame
    {
        // Increment and set frame transmit sequence number
        if ((0 == resend) || (0 != p_ctx->dl.resynced))
        {
            p_ctx->dl.tx_seq_nr = (p_ctx->dl.tx_seq_nr + 1) & DL_MAX_FRAME_NUM;
        }
        p_buffer[0] |= (uint8_t)(p_ctx->dl.tx_seq_nr << DL_FCTR_FRNR_OFFSET);
        // Reset resync received
        p_ctx->dl.resynced = 0;
        OPTIGA_COMMS_LOG_MESSAGE(">>>>");
    }
    else // Control frame
    {
        p_buffer[0] |= DL_FCTR_FTYPE_MASK;
    }

    // Set frame length
    p_buffer[1] = (uint8_t)(frame_len >> 8);
    p_buffer[2] = (uint8_t)frame_len;

    // Calculate frame CRC
    crc = ifx_i2c_dl_calc_crc(p_buffer, 3 + frame_len);
    p_buffer[3 + frame_len] = (uint8_t) (crc >> 8);
    p_buffer[4 + frame_len] = (uint8_t)crc;

    // Transmit frame
    OPTIGA_IFXI2C_LOG_TRANSMIT_HEX_DATA(p_buffer,DL_HEADER_SIZE + frame_len,p_ctx)
    return (ifx_i2c_pl_send_frame(p_ctx, p_buffer, DL_HEADER_SIZE + frame_len));
}

_STATIC_H optiga_lib_status_t ifx_i2c_dl_resync(ifx_i2c_context_t * p_ctx)
{
    optiga_lib_status_t api_status;
    // Reset tx and rx counters
    p_ctx->dl.tx_seq_nr = DL_MAX_FRAME_NUM;
    p_ctx->dl.rx_seq_nr = DL_MAX_FRAME_NUM;
    p_ctx->dl.resynced = 1;
    LOG_DL("[IFX-DL]: Send Re-Sync Frame\n");
    p_ctx->dl.state = DL_STATE_RESEND;
    api_status = ifx_i2c_dl_send_frame_internal(p_ctx, 0, DL_FCTR_SEQCTR_VALUE_RESYNC, 0);
    return (api_status);
}

_STATIC_H void ifx_i2c_dl_resend_frame(ifx_i2c_context_t * p_ctx, uint8_t seqctr_value)
{
    optiga_lib_status_t status;
    // If exit timeout not violated
    uint32_t current_time_stamp = pal_os_timer_get_time_in_milliseconds();
    uint32_t time_stamp_diff = current_time_stamp - p_ctx->tl.api_start_time;

    if (p_ctx->tl.api_start_time > current_time_stamp)
    {
        time_stamp_diff = (0xFFFFFFFF + (current_time_stamp - p_ctx->tl.api_start_time)) + 0x01;
    }
    if (time_stamp_diff < (TL_MAX_EXIT_TIMEOUT * DL_SEC_TO_MSECS))
    {
        if (DL_TRANS_REPEAT == p_ctx->dl.retransmit_counter)
        {
            LOG_DL("[IFX-DL]: Re-Sync counters\n");
            p_ctx->dl.retransmit_counter = 0;
            status = ifx_i2c_dl_resync(p_ctx);
        }
        else
        {
            LOG_DL("[IFX-DL]: Re-TX Frame\n");
            p_ctx->dl.retransmit_counter++;
            p_ctx->dl.state = DL_STATE_TX;
            status = ifx_i2c_dl_send_frame_internal(p_ctx, p_ctx->dl.tx_buffer_size, seqctr_value, 1);
        }
        // Handle error in above case by sending NACK
        if (IFX_I2C_STACK_SUCCESS != status)
        {
            p_ctx->dl.state  = DL_STATE_NACK;
        }
    }
    else
    {
        p_ctx->dl.state = DL_STATE_ERROR;
    }
}

_STATIC_H void ifx_i2c_pl_event_handler(ifx_i2c_context_t * p_ctx,
                                        optiga_lib_status_t event,
                                        const uint8_t * p_data,
                                        uint16_t data_len)
{
    uint8_t fctr = 0;
    uint8_t fr_nr = 0;
    uint8_t ack_nr = 0;
    uint8_t seqctr = 0;
    uint8_t current_event;
    uint8_t ftype = 0;
    uint8_t continue_state_machine = TRUE;
    uint16_t packet_len = 0;
    uint16_t crc_received = 0;
    uint16_t crc_calculated = 0;
    LOG_DL("[IFX-DL]: #Enter DL Handler\n");
    do
    {
        if ((IFX_I2C_FATAL_ERROR == event) && (DL_STATE_IDLE != p_ctx->dl.state))
        {    // Exit in case of fatal error
            LOG_DL("[IFX-DL]: Fatal error received\n");
            p_ctx->dl.state = DL_STATE_ERROR;
        }
        switch (p_ctx->dl.state)
        {
            case DL_STATE_IDLE:
            {
                current_event = (event != IFX_I2C_STACK_SUCCESS) ? IFX_I2C_DL_EVENT_ERROR : IFX_I2C_DL_EVENT_TX_SUCCESS;
                continue_state_machine = FALSE;
                p_ctx->dl.upper_layer_event_handler(p_ctx,current_event, 0, 0);
            }
            break;
            case DL_STATE_TX:
            {
                // If writing a frame failed retry sending
                if (IFX_I2C_STACK_ERROR == event)
                {
                    p_ctx->dl.state = DL_STATE_RESEND;
                    break;
                }
                LOG_DL("[IFX-DL]: Frame Sent\n");
                // Transmission successful, start receiving frame
                p_ctx->dl.frame_start_time = pal_os_timer_get_time_in_milliseconds();
                p_ctx->dl.state = DL_STATE_RX;
                if (0 != ifx_i2c_pl_receive_frame(p_ctx))
                {
                    p_ctx->dl.state  = DL_STATE_NACK;
                }
                else
                {
                    continue_state_machine = FALSE;
                }
            }
            break;
            case DL_STATE_RX:
            {
                if (IFX_I2C_STACK_ERROR == event)
                {    // If no frame was received retry sending
                    p_ctx->dl.state = DL_STATE_RESEND;
                    break;
                }
                // Received frame from device, start analyzing
                LOG_DL("[IFX-DL]: Received Frame of length %d\n",data_len);

                if (data_len < DL_HEADER_SIZE)
                {    // Received length is less than minimum size
                    LOG_DL("[IFX-DL]: received data_len < DL_HEADER_SIZE\n");
                    p_ctx->dl.state  = DL_STATE_NACK;
                    break;
                }
                // Check transmit frame sequence number
                fctr = p_data[0];
                ftype = (fctr & DL_FCTR_FTYPE_MASK) >> DL_FCTR_FTYPE_OFFSET;
                seqctr = (fctr & DL_FCTR_SEQCTR_MASK) >> DL_FCTR_SEQCTR_OFFSET;
                //lint --e{835} suppress "DL_FCTR_ACKNR_OFFSET is defined as 0x00 and is kept for future enhancements"
                ack_nr = (fctr & DL_FCTR_ACKNR_MASK) >> DL_FCTR_ACKNR_OFFSET;
                fr_nr = (fctr & DL_FCTR_FRNR_MASK) >> DL_FCTR_FRNR_OFFSET;
                packet_len = (p_data[1] << 8) | p_data[2];

                // Check frame CRC value
                crc_received = (p_data[data_len - 2] << 8) | p_data[data_len - 1];
                crc_calculated = ifx_i2c_dl_calc_crc(p_data, data_len - 2);
                p_ctx->dl.state = (ftype == DL_FCTR_VALUE_CONTROL_FRAME) ? DL_STATE_RX_CF : DL_STATE_RX_DF;
            }
            break;
            case DL_STATE_RX_DF:
            {
                LOG_DL("[IFX-DL]: Data Frame Received\n");
                if ((crc_received != crc_calculated) || (0 == packet_len) ||
                    (data_len != (DL_HEADER_SIZE + packet_len)) || (DL_FCTR_SEQCTR_VALUE_RFU == seqctr) ||
                    (DL_FCTR_SEQCTR_VALUE_RESYNC == seqctr))
                {
                    // CRC,Length of data frame is 0/ SEQCTR has RFU/Re-sync in Data frame
                    LOG_DL("[IFX-DL]: NACK for CRC error,Data frame length is not correct,RFU in SEQCTR\n");
                    p_ctx->dl.state  = DL_STATE_NACK;
                    break;
                }
                if (fr_nr != ((p_ctx->dl.rx_seq_nr + 1) & DL_MAX_FRAME_NUM))
                {
                    LOG_DL("[IFX-DL]: Data frame number not expected\n");
                    p_ctx->dl.state  = DL_STATE_DISCARD;
                    continue_state_machine = FALSE;
                    //lint --e{534} suppress "Error handling is not required so return value is not checked"
                    ifx_i2c_dl_send_frame_internal(p_ctx, 0, DL_FCTR_SEQCTR_VALUE_ACK, 0);
                    break;
                }
                if (ack_nr != p_ctx->dl.tx_seq_nr)
                {
                    // ack number error
                    LOG_DL("[IFX-DL]: Error in ack number\n");
                    //lint --e{534} suppress "Error handling is not required so return value is not checked"
                    p_ctx->dl.state = DL_STATE_DISCARD;
                    break;
                }
                if (DL_FCTR_SEQCTR_VALUE_NACK == seqctr)
                {
                    // NACK for transmitted frame
                    LOG_DL("[IFX-DL]: NACK received in data frame\n");
                    p_ctx->dl.state = DL_STATE_RESEND;
                    break;
                }
                p_ctx->dl.rx_seq_nr = (p_ctx->dl.rx_seq_nr + 1) & DL_MAX_FRAME_NUM;
                memcpy(p_ctx->dl.p_rx_frame_buffer, p_data, data_len);
                p_ctx->dl.rx_buffer_size = data_len;

                // Send control frame to acknowledge reception of this data frame
                LOG_DL("[IFX-DL]: Read Data Frame -> Send ACK\n");
                p_ctx->dl.retransmit_counter = 0;
                p_ctx->dl.state = DL_STATE_ACK;
                continue_state_machine = FALSE;

                OPTIGA_COMMS_LOG_MESSAGE("<<<<");\
                OPTIGA_IFXI2C_LOG_RECEIVE_HEX_DATA(p_data,data_len,p_ctx);
                //lint --e{534} suppress "Error handling is not required so return value is not checked"
                ifx_i2c_dl_send_frame_internal(p_ctx, 0, DL_FCTR_SEQCTR_VALUE_ACK, 0);
            }
            break;
            case DL_STATE_RX_CF:
            {
                LOG_DL("[IFX-DL]: Control Frame Received\n");
                // Except Re-Sync, Discard Control frame when in receiver mode
                // Discard Re-Sync in transmission mode
                //lint --e{514} suppress "Refer the above comment for the explaination of this check"
                if ((p_ctx->dl.action_rx_only) ^ (DL_FCTR_SEQCTR_VALUE_RESYNC == seqctr))
                {
                    //If control frame already received for data frame, ignore any received control frame
                    LOG_DL("[IFX-DL]: CF in receiver mode,Discard\n");
                    p_ctx->dl.state = DL_STATE_DISCARD;
                    break;
                }
                if (crc_received != crc_calculated)
                {
                    // Re-Transmit frame in case of CF CRC error
                    LOG_DL("[IFX-DL]: Retransmit frame for CF CRC error\n");
                    p_ctx->dl.state = DL_STATE_RESEND;
                    break;
                }
                if ((data_len > DL_CONTROL_FRAME_LENGTH) || (0 != packet_len))
                {
                    // Control frame is more than 5/Control frame with non-zero FRNR/packet len is not 0
                    LOG_DL("[IFX-DL]: Errors in control frame\n");
                    p_ctx->dl.state = DL_STATE_DISCARD;
                    break;
                }
                if (DL_FCTR_SEQCTR_VALUE_RESYNC == seqctr)
                {    // Re-sync received
                    LOG_DL("[IFX-DL]: Re-Sync received\n");
                    p_ctx->dl.state = DL_STATE_DISCARD;
                    p_ctx->dl.resynced = 1;
                    p_ctx->dl.tx_seq_nr = DL_MAX_FRAME_NUM;
                    p_ctx->dl.rx_seq_nr = DL_MAX_FRAME_NUM;
                    break;
                }
                if ((0 != fr_nr) || (DL_FCTR_SEQCTR_VALUE_RFU == seqctr) || (ack_nr != p_ctx->dl.tx_seq_nr))
                {
                    // Control frame with non-zero FRNR/ ACK not received/ ack number != tx number
                    LOG_DL("[IFX-DL]: Errors in control frame\n");
                    p_ctx->dl.state = DL_STATE_DISCARD;
                    break;
                }
                if (DL_FCTR_SEQCTR_VALUE_NACK == seqctr)
                {
                    // NACK for transmitted frame
                    LOG_DL("[IFX-DL]: NACK received\n");
                    p_ctx->dl.state = DL_STATE_RESEND;
                    break;
                }

                LOG_DL("[IFX-DL]: ACK received\n");
                // Report frame reception to upper layer and go in idle state
                p_ctx->dl.state = DL_STATE_IDLE;
                continue_state_machine = FALSE;
                p_ctx->dl.upper_layer_event_handler(p_ctx, IFX_I2C_DL_EVENT_TX_SUCCESS, 0, 0);
            }
            break;
            case DL_STATE_DISCARD:
            {
                LOG_DL("[IFX-DL]: Discard frame\n");
                p_ctx->dl.state = DL_STATE_RX;
                continue_state_machine = FALSE;
                //lint --e{534} suppress "Error handling is not required so return value is not checked"
                ifx_i2c_pl_receive_frame(p_ctx);
            }
            break;
            case DL_STATE_ACK:
            {
                LOG_DL("[IFX-DL]: ACK sent\n");
                if (IFX_I2C_STACK_ERROR == event)
                {
                    // If writing the ACK frame failed, Re-Send
                    LOG_DL("[IFX-DL]: Physical Layer error -> Resend ACK\n");
                    p_ctx->dl.state = DL_STATE_RESEND;
                    break;
                }
                // Control frame successful transmitted
                p_ctx->dl.state = DL_STATE_IDLE;
                continue_state_machine = FALSE;
                if (0 != p_ctx->dl.action_rx_only)
                {
                    p_ctx->dl.upper_layer_event_handler(p_ctx,
                                                        IFX_I2C_DL_EVENT_RX_SUCCESS,
                                                        p_ctx->dl.p_rx_frame_buffer + 3,
                                                        p_ctx->dl.rx_buffer_size - DL_HEADER_SIZE);
                }
                else
                {
                    p_ctx->dl.upper_layer_event_handler(p_ctx,
                                                        IFX_I2C_DL_EVENT_TX_SUCCESS | IFX_I2C_DL_EVENT_RX_SUCCESS,
                                                        p_ctx->dl.p_rx_frame_buffer + 3,
                                                        p_ctx->dl.rx_buffer_size - DL_HEADER_SIZE);
                }
            }
            break;
            case DL_STATE_NACK:
            {
                // Sending NACK
                LOG_DL("[IFX-DL]: Sending NACK\n");
                p_ctx->dl.state = DL_STATE_TX;
                continue_state_machine = FALSE;
                //lint --e{534} suppress "Error handling is not required so return value is not checked"
                ifx_i2c_dl_send_frame_internal(p_ctx, 0, DL_FCTR_SEQCTR_VALUE_NACK, 0);
            }
            break;
            case DL_STATE_RESEND:
            {
                //Resend frame
                ifx_i2c_dl_resend_frame(p_ctx, DL_FCTR_SEQCTR_VALUE_ACK);
                if (DL_STATE_ERROR != p_ctx->dl.state)
                {
                    continue_state_machine = FALSE;
                }
            }
            break;
            case DL_STATE_ERROR:
            {
                if (0 == p_ctx->dl.resynced)
                {
                    p_ctx->dl.error = 1;
                }
                if (0 == p_ctx->dl.error)
                {
                    LOG_DL("[IFX-DL]: Exit error after fatal error\n");
                    //After sending resync, inform upper layer
                    p_ctx->dl.state = DL_STATE_IDLE;
                    p_ctx->dl.upper_layer_event_handler(p_ctx, IFX_I2C_DL_EVENT_ERROR, 0, 0);
                }
                else
                {
                    LOG_DL("[IFX-DL]: Sending re-sync after fatal error\n");
                    // Send re-sync to slave on error
                    //lint --e{534} suppress "As this is last step, no effect of checking return code"
                    ifx_i2c_dl_resync(p_ctx);
                    p_ctx->dl.state = DL_STATE_ERROR;
                    p_ctx->dl.error = 0;
                }
                continue_state_machine = FALSE;
            }
            break;
            default:
                LOG_DL("[IFX-DL]: Default condition occurred. Exiting with error\n");
                p_ctx->dl.state = DL_STATE_IDLE;
                p_ctx->dl.upper_layer_event_handler(p_ctx, IFX_I2C_DL_EVENT_ERROR, 0, 0);
                continue_state_machine = FALSE;
                break;
        }
    } while (TRUE == continue_state_machine);
    LOG_DL("[IFX-DL]: #Exiting DL Handler\n");
}

/**
* @}
*/
