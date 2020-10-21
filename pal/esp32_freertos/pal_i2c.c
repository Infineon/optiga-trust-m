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
* \file pal_i2c.c
*
* \brief   This file implements the platform abstraction layer(pal) APIs for I2C.
*
* \ingroup  grPAL
*
* @{
*/


#include "optiga/pal/pal_i2c.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define PAL_I2C_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define PAL_I2C_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */

#define CONFIG_PAL_I2C_INIT_ENABLE 1

#define WRITE_BIT                       I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                        I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                    0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                   0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                         0x0              /*!< I2C ack value */
#define NACK_VAL                        0x1              /*!< I2C nack value */

/// @cond hidden
typedef struct esp32_i2c_ctx {
	uint8_t  port;
	uint8_t	 scl_io;
	uint8_t	 sda_io;
	uint32_t bitrate;
}esp32_i2c_ctx_t;

/* Varibale to indicate the re-entrant count of the i2c bus acquire function*/
static volatile uint32_t g_entry_count = 0;

/* Pointer to the current pal i2c context*/
static pal_i2c_t * gp_pal_i2c_current_ctx;

/// @endcond


pal_status_t pal_i2c_init(const pal_i2c_t* p_i2c_context)
{
	int master_port;
	esp32_i2c_ctx_t* master_ctx;
	i2c_config_t conf;
	
#if 1 //def CONFIG_PAL_I2C_INIT_ENABLE	
	ESP_LOGI("pal_i2c", "Initialize pal_i2c_init  ");

	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return PAL_STATUS_FAILURE;
	
	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	master_port = master_ctx->port;
	
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = master_ctx->sda_io;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = master_ctx->scl_io;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = master_ctx->bitrate;
    i2c_param_config(master_port, &conf);
    i2c_driver_install(master_port, conf.mode,
                       PAL_I2C_MASTER_TX_BUF_DISABLE,
                       PAL_I2C_MASTER_RX_BUF_DISABLE, 0);
	
    ESP_LOGI("pal_i2c", "init successful");
#endif

    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_deinit(const pal_i2c_t* p_i2c_context)
{
	esp32_i2c_ctx_t* master_ctx;

#if 1    //def CONFIG_PAL_I2C_INIT_ENABLE			
	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return PAL_STATUS_FAILURE;
	
	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	
	i2c_driver_delete(master_ctx->port);
#endif
	
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_write(const pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;
	upper_layer_callback_t upper_layer_handler;
	int i2c_master_port;
	esp32_i2c_ctx_t* master_ctx;

	gp_pal_i2c_current_ctx = p_i2c_context;

	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return status;

	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	i2c_master_port = master_ctx->port;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (p_i2c_context->slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, p_data, length, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

	upper_layer_handler = (upper_layer_callback_t)p_i2c_context->upper_layer_event_handler;

	if (ret == ESP_OK) {
        //ESP_LOG_BUFFER_HEX("pal_i2c : W:", p_data, length);
		upper_layer_handler(p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_SUCCESS);
		status = PAL_STATUS_SUCCESS;
	} else {
        //ESP_LOGI("pal_i2c", "W-Failure");
        upper_layer_handler(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
	}

    return status;
}

pal_status_t pal_i2c_read(const pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;
    upper_layer_callback_t upper_layer_handler;
	int i2c_master_port;
	esp32_i2c_ctx_t* master_ctx;

	gp_pal_i2c_current_ctx = p_i2c_context;

	if (length == 0)
        return status;
	
	if ((p_i2c_context == NULL) || (p_i2c_context->p_i2c_hw_config == NULL))
		return status;

	master_ctx = (esp32_i2c_ctx_t*)p_i2c_context->p_i2c_hw_config;
	i2c_master_port = master_ctx->port;
	
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( p_i2c_context->slave_address << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (length > 1) {
        i2c_master_read(cmd, p_data, length - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, p_data + length - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
	
    upper_layer_handler = (upper_layer_callback_t)p_i2c_context->upper_layer_event_handler;
	
	if (ret == ESP_OK) {
        //ESP_LOG_BUFFER_HEX("pal_i2c : R:", p_data, length);
		upper_layer_handler(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
		status = PAL_STATUS_SUCCESS;
	} else {
        //ESP_LOGI("pal_i2c", "R-Failure");
        upper_layer_handler(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
	}

    return status;
}

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t * p_i2c_context, uint16_t bitrate)
{
    return PAL_STATUS_SUCCESS;
}

/**
* @}
*/
