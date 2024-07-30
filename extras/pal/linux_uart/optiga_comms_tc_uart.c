// SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
// SPDX-License-Identifier: MIT

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "optiga_comms.h"

#define MAX_TRANSMIT_FRAME_SIZE (2000 - 2)
#define CONFIG_FILE_NAME "optiga_comms.ini"

typedef struct com_context {
    int fd;
    char *com_port;
} com_context_t;

com_context_t com_context = {0};

static optiga_lib_status_t _optiga_comms_get_params(com_context_t *p_com_ctx, char *path) {
    optiga_lib_status_t return_status = OPTIGA_COMMS_ERROR;
    FILE *pConfigFile = NULL;
    uint8_t string_length;
    char file_path[1000];
    char szConfig[50];
    char *file_name = CONFIG_FILE_NAME;
    char comport[20];

    do {
        // read xml file from current location
        strcpy(file_path, path);
        strcat(file_path, file_name);
        // Read configuration file
        pConfigFile = fopen(file_path, "r");
        if (NULL == pConfigFile) {
            printf("\n!!!Unable to open %s\n", CONFIG_FILE_NAME);
            break;
        }

        if (NULL == fgets(szConfig, sizeof(szConfig), pConfigFile)) {
            printf("\n!!!Unable to read %s\n", CONFIG_FILE_NAME);
            break;
        }
        string_length = strlen(szConfig);
        // printf ("\nData read from %s", szConfig);
        strcpy((char *)p_com_ctx->com_port, szConfig);
        return_status = OPTIGA_COMMS_SUCCESS;
    } while (0);

    if (NULL != pConfigFile) {
        fclose(pConfigFile);
    }
}

optiga_lib_status_t
optiga_comms_set_callback_handler(optiga_comms_t *p_optiga_comms, callback_handler_t handler) {
    p_optiga_comms->upper_layer_handler = handler;
    return (0);
}

optiga_lib_status_t
optiga_comms_set_callback_context(optiga_comms_t *p_optiga_comms, void *context) {
    p_optiga_comms->p_upper_layer_ctx = context;
    return (0);
}

optiga_comms_t *optiga_comms_create(callback_handler_t callback, void *context) {
    optiga_comms_t *p_optiga_comms = (optiga_comms_t *)calloc(sizeof(optiga_comms_t), 1);
    p_optiga_comms->p_comms_ctx = &com_context;
    return (p_optiga_comms);
}

void optiga_comms_destroy(optiga_comms_t *p_ctx) {
#define COMM_CTX ((com_context_t *)(p_ctx->p_comms_ctx))
    if (NULL != p_ctx) {
        free(p_ctx);
    }
#undef COMM_CTX
}

// Opens the specified serial port, sets it up for binary communication,
// configures its read timeouts, and sets its baud rate.
// Returns a non-negative file descriptor on success, or -1 on failure.
static int open_serial_port(const char *device, uint32_t baud_rate) {
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror(device);
        return -1;
    }

    // Flush away any bytes previously read or written.
    int result = tcflush(fd, TCIOFLUSH);
    if (result) {
        perror("tcflush failed");  // just a warning, not a fatal error
    }

    // Get the current configuration of the serial port.
    struct termios options;
    result = tcgetattr(fd, &options);
    if (result) {
        perror("tcgetattr failed");
        close(fd);
        return -1;
    }

    // Turn off any options that might interfere with our ability to send and
    // receive raw binary bytes.
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    // Set up timeouts: Calls to read() will return as soon as there is
    // at least one byte available or when 100 ms has passed.
    options.c_cc[VTIME] = 10;
    options.c_cc[VMIN] = 255;

    // This code only supports certain standard baud rates. Supporting
    // non-standard baud rates should be possible but takes more work.
    switch (baud_rate) {
        case 4800:
            cfsetospeed(&options, B4800);
            break;
        case 9600:
            cfsetospeed(&options, B9600);
            break;
        case 19200:
            cfsetospeed(&options, B19200);
            break;
        case 38400:
            cfsetospeed(&options, B38400);
            break;
        case 115200:
            cfsetospeed(&options, B115200);
            break;
        default:
            fprintf(stderr, "warning: baud rate %u is not supported, using 9600.\n", baud_rate);
            cfsetospeed(&options, B9600);
            break;
    }
    cfsetispeed(&options, cfgetospeed(&options));

    result = tcsetattr(fd, TCSANOW, &options);
    if (result) {
        perror("tcsetattr failed");
        close(fd);
        return -1;
    }

    return fd;
}

uint16_t calc_crc16_byte(uint16_t seed, uint8_t byte) {
    uint16_t h1;
    uint16_t h2;
    uint16_t h3;
    uint16_t h4;

    h1 = (seed ^ byte) & 0xFF;
    h2 = h1 & 0x0F;
    h3 = ((uint16_t)(h2 << 4)) ^ h1;
    h4 = h3 >> 4;

    return ((uint16_t)((((uint16_t)((((uint16_t)(h3 << 1)) ^ h4) << 4)) ^ h2) << 3)) ^ h4
           ^ (seed >> 8);
}

uint16_t calc_crc16(const uint8_t *p_data, uint16_t data_len) {
    uint16_t i;
    uint16_t crc = 0;

    for (i = 0; i < data_len; i++) {
        crc = calc_crc16_byte(crc, p_data[i]);
    }

    return (crc);
}

// Writes bytes to the serial port, returning 0 on success and -1 on failure.
static int write_port(int fd, uint8_t *buffer, size_t size) {
    ssize_t result = write(fd, buffer, size);
    if (result != (ssize_t)size) {
        perror("failed to write to port");
        return -1;
    }
    return size;
}

// Reads bytes from the serial port.
// Returns after all the desired bytes have been read, or if there is a
// timeout or other error.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
static ssize_t read_port(int fd, uint8_t *buffer, size_t size) {
    size_t received = 0;
    while (received < size) {
        ssize_t r = read(fd, buffer + received, size - received);
        // printf("Recv %d\n", r);
        if (r == 0) {
            // Timeout
            break;
        }
        received += r;
    }
    return received;
}

/**
 *  \brief This API open the communication channel.
 *
 * \details
 *  - This api opens the COM port based on the input provided in optiga_comms.ini file
 * \param[in,out] p_ctx        Pointer to #optiga_comms_t
 *
 * \retval  #OPTIGA_COMMS_SUCCESS
 * \retval  #OPTIGA_COMMS_ERROR
 */
optiga_lib_status_t optiga_comms_open(optiga_comms_t *p_ctx) {
    com_context_t *p_comms_context = (com_context_t *)(p_ctx->p_comms_ctx);
    optiga_lib_status_t api_status = OPTIGA_COMMS_ERROR;

    p_comms_context->com_port = (char *)calloc(15, 1);
    _optiga_comms_get_params((com_context_t *)p_ctx->p_comms_ctx, "");

    ((com_context_t *)p_ctx->p_comms_ctx)->fd =
        open_serial_port(((com_context_t *)p_ctx->p_comms_ctx)->com_port, 115200);
    if (((com_context_t *)p_ctx->p_comms_ctx)->fd < 0)
        return OPTIGA_COMMS_ERROR;

    return OPTIGA_COMMS_SUCCESS;
}

/**
 *  \brief This API resets the OPTIGA.
 *
 * \param[in,out] p_ctx        Pointer to #optiga_comms_t
 * \param[in,out] reset_type   type of reset
 *
 * \retval  #OPTIGA_COMMS_SUCCESS
 * \retval  #OPTIGA_COMMS_ERROR
 */
optiga_lib_status_t optiga_comms_reset(optiga_comms_t *p_ctx, uint8_t reset_type) {
    optiga_lib_status_t api_status = OPTIGA_COMMS_ERROR;

    return api_status;
}

/**
 * \brief   This API sends a command and receives a response.
 *
 * \details
 *  - Total number of bytes transmit as MAX_TRANSMIT_FRAME_SIZE - 2.
 *  - If the data size is less then (MAX_TRANSMIT_FRAME_SIZE - 2) its prepend garbage value and actual length is captured in first 2 bytes
 *  - While receiving data its check for the first 2 bytes for length o actual data to be processes.
 *  - Received data is 2 byte and the value is 0xFF 0xFF the api returns #OPTIGA_COMMS_ERROR.
 *
 * \param[in,out] p_ctx              Pointer to #optiga_comms_t
 * \param[in]     p_tx_data          Pointer to the write data buffer
 * \param[in]     tx_data_length     Pointer to the length of the write data buffer
 * \param[in,out] p_rx_data          Pointer to the receive data buffer
 * \param[in,out] p_rx_data_len      Pointer to the length of the receive data buffer
 *
 * \retval  #OPTIGA_COMMS_SUCCESS
 * \retval  #OPTIGA_COMMS_ERROR
 */
optiga_lib_status_t optiga_comms_transceive(
    optiga_comms_t *p_ctx,
    const uint8_t *p_tx_data,
    uint16_t tx_data_length,
    uint8_t *p_rx_data,
    uint16_t *p_rx_data_len
) {
    optiga_lib_status_t api_status = OPTIGA_COMMS_ERROR;
    uint32_t number_of_bytes_written = 0;
    uint8_t byte_of_data[MAX_TRANSMIT_FRAME_SIZE] = {0};
    uint32_t NoBytesRead;
    uint32_t index = 0;
    uint8_t failure_status[] = {0xff, 0xff};
    uint8_t max_transmit_frame[MAX_TRANSMIT_FRAME_SIZE];
    uint16_t crc16 = 0x0000;
    ;
    uint8_t start_seq[] = {0xbe, 0xef, 0xde, 0xad};

    do {
        // Prepare the Start sequence
        max_transmit_frame[0] = 0xbe;
        max_transmit_frame[1] = 0xef;
        max_transmit_frame[2] = 0xde;
        max_transmit_frame[3] = 0xad;
        // Prepare the length
        max_transmit_frame[4] = (uint8_t)(tx_data_length >> 8);
        max_transmit_frame[5] = (uint8_t)(tx_data_length);
        // Prepare the actual chip response
        memcpy(&max_transmit_frame[6], p_tx_data, tx_data_length);
        // Calculate the CRC and add it to the message
        crc16 = 0x0000;
        crc16 = calc_crc16(max_transmit_frame, tx_data_length + 6);
        // Send the message to the recepient
        max_transmit_frame[6 + tx_data_length] = (uint8_t)(crc16 >> 8);
        max_transmit_frame[7 + tx_data_length] = (uint8_t)(crc16 & 0x00ff);
        // Write Data to the peer
        number_of_bytes_written = write_port(
            ((com_context_t *)p_ctx->p_comms_ctx)->fd,
            max_transmit_frame,
            tx_data_length + 8
        );
        if (number_of_bytes_written != (tx_data_length + 8)) {
            printf("COM port write failed\n");
            printf("Error is %d\n", number_of_bytes_written);
            break;
        }

        number_of_bytes_written =
            read_port(((com_context_t *)p_ctx->p_comms_ctx)->fd, byte_of_data, 6);
        if (number_of_bytes_written != 6) {
            printf("COM port read 1 failed\n");
            fprintf(stderr, "%s\n", strerror(errno));
            break;
        }

        // Compare whether the start sequence is ok
        if (memcmp(start_seq, byte_of_data, 4) == 0) {
            // If ok, calculate the expected packet
            *p_rx_data_len = (uint16_t)((byte_of_data[4] << 8) | (byte_of_data[5]));
        } else {
            printf("No Start Sequence found\n");
            break;
        }

        // Unpack data and return
        if (*p_rx_data_len == 0xffff) {
            printf("Receive error\n");
            api_status = OPTIGA_COMMS_ERROR;
        } else {
            if (*p_rx_data_len > (MAX_TRANSMIT_FRAME_SIZE - 8)) {
                printf("Receive error. Frame too big %02X\n", *p_rx_data_len);
                break;
            }

            // receive the rest of the message using the calculated packet size + 2 bytes for the crc16
            number_of_bytes_written = read_port(
                ((com_context_t *)p_ctx->p_comms_ctx)->fd,
                &byte_of_data[6],
                *p_rx_data_len + 2
            );
            if (number_of_bytes_written != (*p_rx_data_len + 2)) {
                printf("COM port read 2 failed\n");
                fprintf(stderr, "%s\n", strerror(errno));
                break;
            }

            memcpy(p_rx_data, &byte_of_data[6], *p_rx_data_len);
            // Calculate the CRC16
            crc16 = 0x0000;
            crc16 = (uint16_t
            )((byte_of_data[6 + *p_rx_data_len] << 8) | (byte_of_data[7 + *p_rx_data_len]));
            // Check whether the CRC is correct
            if (crc16 != calc_crc16(byte_of_data, *p_rx_data_len + 6)) {
                printf("Receive error. Invalid CRC16\n");
                break;
            }

            api_status = OPTIGA_COMMS_SUCCESS;
        }
    } while (0);

    return api_status;
}

/**
 *  \brief This API closes the communication.
 *
 * \param[in,out] p_ctx             Pointer to #optiga_comms_t
 *
 * \retval  #OPTIGA_COMMS_SUCCESS
 * \retval  #OPTIGA_COMMS_ERROR
 */
optiga_lib_status_t optiga_comms_close(optiga_comms_t *p_ctx) {
    optiga_lib_status_t return_status = OPTIGA_COMMS_ERROR;
    com_context_t *p_comm_context = (com_context_t *)(p_ctx->p_comms_ctx);

    do {
        if (NULL == p_ctx) {
            printf("\n!!!optiga_comms_close invoked with NULL Pointer");
            break;
        }

        printf("\nClose the %s port done", p_comm_context->com_port);
        close(p_comm_context->fd);

        if (p_comm_context->com_port) {
            free((void *)p_comm_context->com_port);
            p_comm_context->com_port = NULL;
        }

        return_status = OPTIGA_COMMS_SUCCESS;
    } while (0);
    return return_status;
}
