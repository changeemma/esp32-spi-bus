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

#include "test_spi.h"

static const char *TAG = "==> main";

static esp_err_t init_device(void){
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

void app_main(void)
{
    ESP_ERROR_CHECK(init_device());
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
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    printf("\n\n\n\n\n\n\n\n\n\n\n");
}
