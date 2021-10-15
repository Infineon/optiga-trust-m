/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
* \file ifx_i2c_transport_layer.c
*
* \brief   This file implements the APIs for transport layer of the Infineon I2C Protocol Stack library.
*
* \ingroup  grIFXI2C
*
* @{
*/

#include "optiga/ifx_i2c/ifx_i2c_transport_layer.h"
#include "optiga/ifx_i2c/ifx_i2c_data_link_layer.h" // include lower layer header

/// @cond hidden

// Transport Layer states
#define TL_STATE_UNINIT                     (0x00)
#define TL_STATE_IDLE                       (0x01)
#define TL_STATE_TX                         (0x02)
#define TL_STATE_RX                         (0x04)
#define TL_STATE_CHAINING                   (0x05)
#define TL_STATE_ERROR                      (0x06)
#define TL_STATE_CHAINING_ERROR             (0x07)
#define TL_STATE_RESEND                     (0x08)


// Transport Layer chaining values
#define TL_CHAINING_NO                      (0x00)
#define TL_CHAINING_FIRST                   (0x01)
#define TL_CHAINING_INTERMEDIATE            (0x02)
#define TL_CHAINING_LAST                    (0x04)
#define TL_CHAINING_ERROR                   (0x07)

#define TL_PCTR_CHANNEL_MASK                (0xF8)
#define TL_PCTR_CHAIN_MASK                  (0x07)
#define TL_PCTR_INVALID                     (0xFF)
#define TL_PREVIOUS_CHAINING_STATUS         (0x02)
#define TL_CURRENT_CHAINING_STATUS          (0x08)
// Setup debug log statements
#if IFX_I2C_LOG_TL == 1
#define LOG_TL IFX_I2C_LOG
#else
#define LOG_TL(...)    //printf(__VA_ARGS__);
#endif

static const uint8_t g_pctr_states_table[TL_CURRENT_CHAINING_STATUS][TL_PREVIOUS_CHAINING_STATUS] = {
                                                                                              {TL_CHAINING_NO, TL_CHAINING_LAST},
                                                                                              {TL_CHAINING_NO, TL_CHAINING_LAST},
                                                                                              {TL_CHAINING_FIRST, TL_CHAINING_INTERMEDIATE},
                                                                                              {TL_PCTR_INVALID, TL_PCTR_INVALID},
                                                                                              {TL_CHAINING_FIRST, TL_CHAINING_INTERMEDIATE},
                                                                                              {TL_PCTR_INVALID, TL_PCTR_INVALID},
                                                                                              {TL_PCTR_INVALID, TL_PCTR_INVALID},
                                                                                              {TL_CHAINING_ERROR, TL_CHAINING_ERROR},
                                                                                              };

_STATIC_H optiga_lib_status_t ifx_i2c_tl_send_next_fragment(ifx_i2c_context_t * p_ctx);
_STATIC_H void ifx_i2c_dl_event_handler(ifx_i2c_context_t * p_ctx,
                                        optiga_lib_status_t event,
                                        const uint8_t * p_data,
                                        uint16_t data_len);
_STATIC_H optiga_lib_status_t ifx_i2c_tl_resend_packets(ifx_i2c_context_t * p_ctx);
_STATIC_H optiga_lib_status_t ifx_i2c_tl_send_chaining_error(ifx_i2c_context_t * p_ctx);
_STATIC_H uint8_t ifx_i2c_tl_calculate_pctr(const ifx_i2c_context_t * p_ctx);
_STATIC_H optiga_lib_status_t ifx_i2c_tl_check_chaining_error(uint8_t current_chaning, uint8_t previous_chaining);
/// @endcond

optiga_lib_status_t ifx_i2c_tl_init(ifx_i2c_context_t * p_ctx, ifx_i2c_event_handler_t handler)
{
    LOG_TL("[IFX-TL]: Init\n");

    p_ctx->tl.state = TL_STATE_UNINIT;

    // Initialize Data Link layer (and register event handler)
    if (IFX_I2C_STACK_SUCCESS != ifx_i2c_dl_init(p_ctx, ifx_i2c_dl_event_handler))
    {
        return (IFX_I2C_STACK_ERROR);
    }
    p_ctx->tl.initialization_state = TRUE;
    p_ctx->tl.upper_layer_event_handler = handler;
    p_ctx->tl.state = TL_STATE_IDLE;
    p_ctx->tl.max_packet_length = p_ctx->frame_size - (DL_HEADER_SIZE + TL_HEADER_SIZE);

    return (IFX_I2C_STACK_SUCCESS);
}


optiga_lib_status_t ifx_i2c_tl_transceive(ifx_i2c_context_t * p_ctx,
                                          uint8_t * p_packet,
                                          uint16_t packet_len,
                                          uint8_t * p_recv_packet,
                                          uint16_t * p_recv_packet_len)
{
    optiga_lib_status_t status = IFX_I2C_STACK_ERROR;
    LOG_TL("[IFX-TL]: Transceive txlen %d\n", packet_len);

    do
    {
        // Check function arguments
        if ((NULL == p_packet) || (0 == packet_len))
        {
            break;
        }
        // Transport Layer must be idle
        if (TL_STATE_IDLE != p_ctx->tl.state)
        {
            break;
        }
        p_ctx->tl.state = TL_STATE_TX;
        p_ctx->tl.api_start_time = pal_os_timer_get_time_in_milliseconds();
        p_ctx->tl.p_actual_packet = p_packet;
        p_ctx->tl.actual_packet_length = packet_len;
        p_ctx->tl.packet_offset = 0;
        p_ctx->tl.p_recv_packet_buffer = p_recv_packet;
        p_ctx->tl.p_recv_packet_buffer_length = p_recv_packet_len;
        p_ctx->tl.total_recv_length = 0;
        p_ctx->tl.chaining_error_count = 0;
        p_ctx->tl.master_chaining_error_count = 0;
        p_ctx->tl.transmission_completed = 0;
        p_ctx->tl.error_event = IFX_I2C_STACK_ERROR;
        status = ifx_i2c_tl_send_next_fragment(p_ctx);
    } while (FALSE);
    return (status);
}


_STATIC_H optiga_lib_status_t ifx_i2c_tl_resend_packets(ifx_i2c_context_t * p_ctx)
{
    // Transport Layer must be idle
    if (TL_STATE_IDLE != p_ctx->tl.state)
    {
        return (IFX_I2C_STACK_ERROR);
    }

    p_ctx->tl.packet_offset = 0;
    p_ctx->tl.total_recv_length = 0;
    p_ctx->tl.state = TL_STATE_TX;
    return (ifx_i2c_tl_send_next_fragment(p_ctx));
}

_STATIC_H uint8_t ifx_i2c_tl_calculate_pctr(const ifx_i2c_context_t * p_ctx)
{
    uint8_t pctr;
    uint16_t fragment_size = p_ctx->tl.max_packet_length;
    uint16_t remaining_data = p_ctx->tl.actual_packet_length - p_ctx->tl.packet_offset;
    // No chain
    if ((0 == p_ctx->tl.packet_offset) && (remaining_data <= fragment_size))
    {
        pctr = TL_CHAINING_NO;
    }
    // First chain
    else if ((0 == p_ctx->tl.packet_offset) && (remaining_data > fragment_size))
    {
        pctr = TL_CHAINING_FIRST;
    }
    // Intermediate chain
    else if ((0 != p_ctx->tl.packet_offset) && (remaining_data > fragment_size))
    {
        pctr = TL_CHAINING_INTERMEDIATE;
    }
    // Last chain
    else
    {
        pctr = TL_CHAINING_LAST;
    }

    return (pctr);
}
_STATIC_H optiga_lib_status_t ifx_i2c_tl_send_next_fragment(ifx_i2c_context_t * p_ctx)
{
    uint8_t pctr;
    // Calculate size of fragment (last one might be shorter)
    uint16_t tl_fragment_size = p_ctx->tl.max_packet_length;
    pctr = ifx_i2c_tl_calculate_pctr(p_ctx);
    if ((p_ctx->tl.actual_packet_length - p_ctx->tl.packet_offset) < tl_fragment_size)
    {
        tl_fragment_size = p_ctx->tl.actual_packet_length - p_ctx->tl.packet_offset;
    }
    // Assign the pctr
    //lint --e{835} suppress "IFX_I2C_DL_HEADER_OFFSET macro is defined as 0x00 and is kept for future enhancements"
    p_ctx->tx_frame_buffer[IFX_I2C_TL_HEADER_OFFSET] = (pctr | IFX_I2C_PRESENCE_BIT);
    //copy the data
    //lint --e{835} suppress "IFX_I2C_DL_HEADER_OFFSET macro is defined as 0x00 and is kept for future enhancements"
    memcpy(p_ctx->tx_frame_buffer+IFX_I2C_TL_HEADER_OFFSET + 1,
           p_ctx->tl.p_actual_packet + p_ctx->tl.packet_offset,
           tl_fragment_size);
    p_ctx->tl.packet_offset += tl_fragment_size;
    //send the fragment to dl layer
    return (ifx_i2c_dl_send_frame(p_ctx,tl_fragment_size + 1));
}

_STATIC_H optiga_lib_status_t ifx_i2c_tl_send_chaining_error(ifx_i2c_context_t * p_ctx)
{
    uint16_t tl_fragment_size = 1;
    //lint --e{835} suppress "IFX_I2C_DL_HEADER_OFFSET macro is defined as 0x00 and is kept for future enhancements"
    p_ctx->tx_frame_buffer[IFX_I2C_TL_HEADER_OFFSET] = 0x07;
    p_ctx->tl.total_recv_length = 0;
    //send the fragment to dl layer
    return (ifx_i2c_dl_send_frame(p_ctx,tl_fragment_size));
}

_STATIC_H optiga_lib_status_t ifx_i2c_tl_check_chaining_error(uint8_t current_chaning,
                                                              uint8_t previous_chaining)
{
    optiga_lib_status_t status = IFX_I2C_STACK_ERROR;
    if (((TL_CHAINING_ERROR == current_chaning) || (TL_CHAINING_NO == current_chaning) ||
        (TL_CHAINING_LAST == current_chaning) || (TL_CHAINING_INTERMEDIATE == current_chaning) ||
        (TL_CHAINING_FIRST == current_chaning)))
    {
        if ((g_pctr_states_table[current_chaning][0] == previous_chaining) ||
            (g_pctr_states_table[current_chaning][1] == previous_chaining))
        {
            status =  IFX_I2C_STACK_SUCCESS;
        }
    }

    return (status);
}
// This functionality checks for the present bit in the pctr register
_STATIC_H  optiga_lib_status_t presence_bit_check(const ifx_i2c_context_t * p_ctx,
                                                  uint8_t pctr)
{
    optiga_lib_status_t return_status= IFX_I2C_STACK_SUCCESS;
    uint8_t pctr_value;

    pctr_value = (pctr & TL_PCTR_CHANNEL_MASK);
    //lint --e{774} suppress "This value changes based on presentation layer macro"
    if (0 != IFX_I2C_PRESENCE_BIT_CHECK)
    {
        if ((FALSE == p_ctx->tl.initialization_state) && ((TL_CHAINING_NO == (pctr & TL_PCTR_CHAIN_MASK))
            || (TL_CHAINING_FIRST == (pctr & TL_PCTR_CHAIN_MASK))))
        {
            if (IFX_I2C_PRESENCE_BIT_CHECK != pctr_value)
            {
                return_status = IFX_I2C_STACK_ERROR;
            }
        }
    }
    else 
    {
        if ((FALSE == p_ctx->tl.initialization_state))
        {
            if (IFX_I2C_PRESENCE_BIT_CHECK != pctr_value)
            {
                return_status = IFX_I2C_STACK_ERROR;
            }
        }
    }
    return (return_status);
}
_STATIC_H void ifx_i2c_dl_event_handler(ifx_i2c_context_t * p_ctx,
                                        optiga_lib_status_t event,
                                        const uint8_t * p_data,
                                        uint16_t data_len)
{
    uint8_t pctr = 0;
    uint8_t chaining = 0;
    uint8_t exit_machine = TRUE;
    do
    {
        if (NULL != p_data)
        {
            pctr = p_data[0];
            chaining = pctr & TL_PCTR_CHAIN_MASK;
        }
        // Propagate errors to upper layer
        if (0 != (event & IFX_I2C_DL_EVENT_ERROR))
        {
            p_ctx->tl.state = TL_STATE_ERROR;
            p_ctx->tl.error_event = IFX_I2C_STACK_ERROR;
        }
        if (0 != data_len)
        {
            if (0 != (presence_bit_check(p_ctx,pctr)))
            {
                p_ctx->tl.state = TL_STATE_ERROR;
                p_ctx->tl.error_event = IFX_I2C_STACK_ERROR;
            }
        }
        p_ctx->tl.initialization_state = FALSE;
        switch (p_ctx->tl.state)
        {
            case TL_STATE_IDLE:
            {
                exit_machine = FALSE;
                p_ctx->tl.upper_layer_event_handler(p_ctx, IFX_I2C_STACK_SUCCESS, 0, 0);
            }
            break;
            case TL_STATE_TX:
            {
                // Frame transmission in Data Link layer complete, start receiving frames
                if (0 != (event & IFX_I2C_DL_EVENT_TX_SUCCESS))
                {
                    if (p_ctx->tl.packet_offset < p_ctx->tl.actual_packet_length)
                    {
                        // Transmission of one fragment complete, send next fragment
                        LOG_TL("[IFX-TL]: Tx:Fragment sent,now send next\n");
                        // Chaining error from slave
                        if (TL_CHAINING_ERROR == chaining)
                        {
                            LOG_TL("[IFX-TL]: Tx:Chaining error received while Tx\n");
                            p_ctx->tl.state = TL_STATE_RESEND;
                            break;
                        }
                        // Any fragment received before complete transmission is error
                        if (0 != data_len)
                        {
                            LOG_TL("[IFX-TL]: Tx:Data received while Tx\n");
                            p_ctx->tl.state = TL_STATE_ERROR;
                            break;
                        }
                        exit_machine = FALSE;
                        //lint --e{534} suppress "Error handling is not required so return value is not checked"
                        ifx_i2c_tl_send_next_fragment(p_ctx);
                    }
                    else
                    {
                        // Transmission of all fragments complete, start receiving fragments
                        LOG_TL("[IFX-TL]: Tx:All fragment sent\n");
                        p_ctx->tl.state = TL_STATE_RX;
                        p_ctx->tl.total_recv_length = 0;
                        p_ctx->tl.previous_chaining = TL_CHAINING_NO;
                        p_ctx->tl.transmission_completed = 1;
                        // if data is received after sending last frame
                        if (!(event & IFX_I2C_DL_EVENT_RX_SUCCESS))
                        {
                            LOG_TL("[IFX-TL]: Tx:Data already received after Tx\n");
                            // Received CTRL frame, trigger reception in Data Link layer
                            if (0 != ifx_i2c_dl_receive_frame(p_ctx))
                            {
                                LOG_TL("[IFX-TL]: Tx:RX Received CTRL frame fail -> Inform UL\n");
                                p_ctx->tl.state = TL_STATE_ERROR;
                            }
                            exit_machine = FALSE;
                        }
                    }
                }
                else
                {
                    LOG_TL("[IFX-TL]: Tx:IFX_I2C_DL_EVENT_TX_SUCCESS is not satisfied Tx\n");
                    p_ctx->tl.state = TL_STATE_ERROR;
                    break;
                }
            }
            break;
            case TL_STATE_RX:
            {
                // Reception of frame from Data Link layer
                if (0 != (event & IFX_I2C_DL_EVENT_RX_SUCCESS))
                {
                    // Message must contain at least the transport layer header
                    if (data_len < TL_HEADER_SIZE)
                    {
                        LOG_TL("[IFX-TL]: Rx : Data received is more than header len\n");
                        p_ctx->tl.state = TL_STATE_ERROR;
                        break;
                    }

                    if (IFX_I2C_STACK_MEM_ERROR == p_ctx->tl.error_event)
                    {
                        if ((TL_CHAINING_LAST == chaining) || (0 != ifx_i2c_dl_receive_frame(p_ctx)))
                        {
                            p_ctx->tl.state = TL_STATE_ERROR;
                            break;
                        }
                        p_ctx->tl.state = TL_STATE_RX;
                        exit_machine = FALSE;
                        break;
                    }

                    // If chaining error detected
                    if (IFX_I2C_STACK_SUCCESS != ifx_i2c_tl_check_chaining_error(chaining,p_ctx->tl.previous_chaining))
                    {
                        LOG_TL("[IFX-TL]: Rx : Chaining state is not correct\n");
                        p_ctx->tl.state = TL_STATE_RESEND;
                        break;
                    }

                    p_ctx->tl.previous_chaining = chaining & 0x07;
                    if (NULL == p_data)
                    {
                        p_ctx->tl.state = TL_STATE_ERROR;
                        break;
                    }

                    // No chaining and Last
                    if ((TL_CHAINING_NO == chaining) || (TL_CHAINING_LAST == chaining))
                    {
                        LOG_TL("[IFX-TL]: Rx : No chain/Last chain received, Inform UL\n");
                        // Check for possible receive buffer overflow
                        if ((p_ctx->tl.total_recv_length + data_len - 1) > (*p_ctx->tl.p_recv_packet_buffer_length))
                        {
                            LOG_TL("[IFX-TL]: Chain : Buffer overflow\n");
                            p_ctx->tl.error_event = IFX_I2C_STACK_MEM_ERROR;
                            p_ctx->tl.state = TL_STATE_ERROR;
                            break;
                        }
                        exit_machine = FALSE;
                        // Copy frame payload to transport layer receive buffer
                        memcpy(p_ctx->tl.p_recv_packet_buffer + p_ctx->tl.total_recv_length, p_data + 1, data_len - 1);
                        p_ctx->tl.total_recv_length += (data_len - 1);
                        // Inform upper layer that a packet has arrived
                        p_ctx->tl.state = TL_STATE_IDLE;
                        *p_ctx->tl.p_recv_packet_buffer_length = p_ctx->tl.total_recv_length;
                        p_ctx->tl.upper_layer_event_handler(p_ctx,IFX_I2C_STACK_SUCCESS,
                                                            p_ctx->tl.p_recv_packet_buffer,
                                                            *p_ctx->tl.p_recv_packet_buffer_length);
                    }
                    else
                    {
                        p_ctx->tl.state = TL_STATE_CHAINING;
                    }
                }
                else
                {
                    LOG_TL("[IFX-TL]: Tx:IFX_I2C_DL_EVENT_TX_SUCCESS is not satisfied Tx\n");
                    p_ctx->tl.state = TL_STATE_ERROR;
                    break;
                }
            }
            break;
            case TL_STATE_CHAINING:
            {
                LOG_TL("[IFX-TL]: Chain : Chaining mode entered\n");
                // When receiving a starting fragment, fragment length must be max frame size for intermediate and last 
                // frame the buffer should not be empty
                if (data_len != (p_ctx->tl.max_packet_length + 1))
                {
                    LOG_TL("[IFX-TL]: Chain : Data len not equal to max frame size\n");
                    p_ctx->tl.state = TL_STATE_CHAINING_ERROR;
                    break;
                }
                // Check for possible receive buffer overflow
                if ((p_ctx->tl.total_recv_length + data_len - 1) > (*p_ctx->tl.p_recv_packet_buffer_length))
                {
                    LOG_TL("[IFX-TL]: Chain : Buffer overflow\n");
                    p_ctx->tl.error_event = IFX_I2C_STACK_MEM_ERROR;
                    p_ctx->tl.state = TL_STATE_RX;
                    break;
                }
                if (NULL == p_data)
                {
                    p_ctx->tl.state = TL_STATE_ERROR;
                    break;
                }
                // Copy frame payload to transport layer receive buffer
                memcpy(p_ctx->tl.p_recv_packet_buffer + p_ctx->tl.total_recv_length, p_data + 1, data_len - 1);
                p_ctx->tl.total_recv_length += (data_len - 1);

                p_ctx->tl.previous_chaining = pctr & 0x07;
                LOG_TL("[IFX-TL]: Chain : Continue  in receive mode\n");
                p_ctx->tl.state = TL_STATE_RX;
                // Continue receiving frames until packet is complete
                if (0 != ifx_i2c_dl_receive_frame(p_ctx))
                {
                    p_ctx->tl.state = TL_STATE_ERROR;
                }
                exit_machine = FALSE;
            }
            break;
            case TL_STATE_RESEND:
            {
                LOG_TL("[IFX-TL]: Resend Enter\n");
                // In received mode , for wrong pctr with data
                if ((data_len > 1) && (p_ctx->tl.transmission_completed == 1))
                {
                    LOG_TL("[IFX-TL]: Resend : Send chaining error\n");
                    p_ctx->tl.state = TL_STATE_CHAINING_ERROR;
                    break;
                }
                // Master Resend the packets,Resend only once, otherwise exit with error
                if (0 == (p_ctx->tl.chaining_error_count++))
                {
                    LOG_TL("[IFX-TL]: Resend : Resending\n");
                    p_ctx->tl.state = TL_STATE_IDLE;
                    if (0 != ifx_i2c_tl_resend_packets(p_ctx))
                    {
                        p_ctx->tl.state = TL_STATE_ERROR;
                    }
                    else
                    {
                        exit_machine = FALSE;
                    }
                }
                else
                {
                    LOG_TL("[IFX-TL]: Resend : chaining_error_count exceeded\n");
                    p_ctx->tl.state = TL_STATE_ERROR;
                }
            }
            break;
            case TL_STATE_CHAINING_ERROR:
            {
                // Send chaining error to slave
                p_ctx->tl.state = TL_STATE_TX;
                if (0 == (p_ctx->tl.master_chaining_error_count++))
                {
                    LOG_TL("[IFX-TL]: Chain error : Sending chain error\n");
                    // Send chaining error only once
                    if (0 != ifx_i2c_tl_send_chaining_error(p_ctx))
                    {
                        p_ctx->tl.state = TL_STATE_ERROR;
                    }
                    else
                    {
                        exit_machine = FALSE;
                    }
                }
                else
                {
                    LOG_TL("[IFX-TL]: Chain error : master_chaining_error_count exceeded\n");
                    p_ctx->tl.state = TL_STATE_ERROR;
                }
            }
            break;
            case TL_STATE_ERROR:
            {
                LOG_TL("[IFX-TL]: Error\n");
                exit_machine = FALSE;
                if ((0 != (event & IFX_I2C_DL_EVENT_ERROR)) || (0 != data_len))
                {
                    p_ctx->tl.state = TL_STATE_IDLE;
                }
                p_ctx->tl.upper_layer_event_handler(p_ctx, p_ctx->tl.error_event, 0u, 0u);
            }
            break;
            default:
            {
                LOG_TL("[IFX-TL]: Exit from default case\n");
                p_ctx->tl.state = TL_STATE_IDLE;
                exit_machine = FALSE;
                p_ctx->tl.upper_layer_event_handler(p_ctx, p_ctx->tl.error_event, 0u, 0u);
            }
            break;
        }
    } while (TRUE == exit_machine);
}


/**
* @}
*/
