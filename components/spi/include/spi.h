#pragma once

// C libraries
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ESP32 libraries
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "driver/spi_master.h"
#include "driver/spi_slave.h"

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_QUEUE_SIZE 3

#define SENDER_GPIO_MOSI 23
#define SENDER_GPIO_SCLK 18
#define SENDER_GPIO_CS 5

#define RECEIVER_GPIO_MOSI 13
#define RECEIVER_GPIO_SCLK 14
#define RECEIVER_GPIO_CS 15

#define SPI_SENDER_HOST VSPI_HOST
#define SPI_RECEIVER_HOST HSPI_HOST

#define SPI_PAYLOAD_BUFFER_SIZE 40
#define SPI_PAYLOAD_TTL 4

typedef enum {
    SPI_PAYLOAD_TYPE_INTERNAL = 0,
    SPI_PAYLOAD_TYPE_ESP_NETIF = 1,
} spi_buffer_type_t;

typedef struct
{
    uint8_t id;
    uint8_t ttl;
    device_id_t src_device_id;
    device_id_t dst_device_id;
    spi_buffer_type_t buffer_type;
    char buffer[ SPI_PAYLOAD_BUFFER_SIZE ];
    uint8_t len;
} spi_payload_t;


esp_err_t spi_init(void);

esp_err_t spi_transmit(void *p, size_t len);

esp_err_t spi_receive(void *p, size_t len);

#ifdef __cplusplus
}
#endif

