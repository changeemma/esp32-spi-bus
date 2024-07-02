// C libraries
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ESP32 libraries
#include "esp_event.h"
#include "esp_check.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "config.h"
#include "spi.h"
#include "spi_internal.h"
#include "spi_netif.h"


static const char *TAG = "==> main";

esp_err_t init_device(void){
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "nvs initialized");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "default event loop created");
    device_config_setup();
    device_config_print();
    ESP_ERROR_CHECK(spi_init());
    return ESP_OK;
}


static void spi_receive_task( void *pvParameters )
{
    spi_payload_t p;
    while (true) {
        memset(&p, 0, sizeof(spi_payload_t));
        ESP_ERROR_CHECK(spi_receive(&p, sizeof(spi_payload_t)));
        switch (p.buffer_type)
        {
        case SPI_PAYLOAD_TYPE_INTERNAL:
            ESP_ERROR_CHECK_WITHOUT_ABORT(spi_internal_handler(&p));
            break;
        case SPI_PAYLOAD_TYPE_ESP_NETIF:
            ESP_ERROR_CHECK_WITHOUT_ABORT(spi_netif_handler(&p));
            break;
        default:
            ESP_LOGE(TAG, "Unknown SPI_PAYLOAD_TYPE '%i'", p.buffer_type);
            break;
        }
    }
    vTaskDelete(NULL);
}

static void test_spi_internal_message_any_sibling( void )
{
    spi_payload_t p = {
        .id = 1,
        .src_device_id = device_config_get_id(),
        .dst_device_id = DEVICE_ID_ANY,
        .buffer_type = SPI_PAYLOAD_TYPE_INTERNAL,
        .buffer = "Hi neighbor!",
        .ttl = SPI_PAYLOAD_TTL,
        .len = SPI_PAYLOAD_BUFFER_SIZE,
    };
    printf("Testing spi_transmit to any sibling(dst_device_id=%i, TTL=%i, type: INTERNAL)\n", \
            p.dst_device_id, p.ttl);
    assert(spi_transmit(&p, sizeof(spi_payload_t)) == ESP_OK);
    printf("---------------------------\n\n");
}
static void test_spi_internal_wrong_sibling( void )
{
    spi_payload_t p = {
        .id = 2,
        .src_device_id = device_config_get_id(),
        .dst_device_id = DEVICE_ID_NONE,
        .buffer_type = SPI_PAYLOAD_TYPE_INTERNAL,
        .buffer = "where is Waldo?",
        .ttl = SPI_PAYLOAD_TTL,
        .len = SPI_PAYLOAD_BUFFER_SIZE,
    };
    printf("Testing spi_transmit to wrong sibling(dst_device_id=%i, TTL=%i, type: INTERNAL)\n", \
            DEVICE_ID_NONE, SPI_PAYLOAD_TTL);
    assert(spi_transmit(&p, sizeof(spi_payload_t)) == ESP_OK);
    printf("---------------------------\n\n");
}

static void test_spi_internal_broadcast_to_siblings( void )
{
    const char msg[] = "hello world, this is a broadcast.";
    printf("Testing broadcast_to_siblings(\"%s\", %i)\n", msg, sizeof(msg));
    assert(broadcast_to_siblings(msg, sizeof(msg)) == true);
    printf("---------------------------\n\n");
}

static void test_spi_netif_message_any_sibling( void )
{
    spi_payload_t p = {
        .id = 3,
        .src_device_id = device_config_get_id(),
        .dst_device_id = DEVICE_ID_ANY,
        .buffer_type = SPI_PAYLOAD_TYPE_ESP_NETIF,
        .buffer = "this is a netif payload",
        .ttl = SPI_PAYLOAD_TTL,
        .len = SPI_PAYLOAD_BUFFER_SIZE,
    };
    printf("Testing spi_transmit to any sibling(dst_device_id=%i, TTL=%i, type: ESP_NETIF)\n", \
            DEVICE_ID_NONE, SPI_PAYLOAD_TTL);
    assert(spi_transmit(&p, sizeof(spi_payload_t)) == ESP_OK);
    printf("---------------------------\n\n");
}

static void test_spi_netif_bouncing_message( void )
{
    spi_payload_t p = {
        .id = 4,
        .src_device_id = device_config_get_id(),
        .dst_device_id = device_config_get_id(),
        .buffer_type = SPI_PAYLOAD_TYPE_ESP_NETIF,
        .buffer = "hi, i am back!",
        .ttl = SPI_PAYLOAD_TTL,
        .len = SPI_PAYLOAD_BUFFER_SIZE,
    };
    printf("Testing spi_transmit to myself (dst_device_id=%i, TTL=%i, type: ESP_NETIF)\n", \
            DEVICE_ID_NONE, SPI_PAYLOAD_TTL);
    assert(spi_transmit(&p, sizeof(spi_payload_t)) == ESP_OK);
    printf("---------------------------\n\n");
}

void app_main(void)
{
    ESP_ERROR_CHECK(init_device());
    xTaskCreate(spi_receive_task, "spi_receive_task", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    test_spi_internal_message_any_sibling();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    test_spi_internal_wrong_sibling();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    test_spi_internal_broadcast_to_siblings();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    test_spi_netif_message_any_sibling();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    test_spi_netif_bouncing_message();
}
