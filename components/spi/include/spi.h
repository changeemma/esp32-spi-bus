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
#define SPI_TX_RETRY_COUNT 3

typedef enum {
    SPI_PAYLOAD_TYPE_INTERNAL = 0,
    SPI_PAYLOAD_TYPE_ESP_NETIF = 1,
} SPIPayloadType_t;

typedef struct
{
    ConfigBoardID_t ucSrcBoardID;
    ConfigBoardID_t ucDstBoardID;
    SPIPayloadType_t ePayloadType;
    uint8_t ucPayloadID;
    char pcPayload[ 20 ];
} SPIPayload_t;


esp_err_t xSPIInit(void);
esp_err_t xSPITransmit(SPIPayload_t *p);
esp_err_t xSPIReceive(SPIPayload_t *p);

esp_err_t xSPIBroadcast(SPIPayload_t *p);