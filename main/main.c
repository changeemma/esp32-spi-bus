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

#include "spi.h"
#include "config.h"


static const char *TAG = "==> main";

esp_err_t xInitBoard(void){
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
    vReadConfig();
    vPrintConfig();
    return ESP_OK;
}

static void vSPIInternalTxTask( void *pvParameters )
{
    SPIPayload_t p = {
        .ucSrcBoardID = xGetConfigBoardID(),
        .ucPayloadID = 0,
        .ePayloadType = SPI_PAYLOAD_TYPE_INTERNAL,
        .pcPayload = "this is a payload",
    };
    while (true) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        printf("INTERNAL - sending src: %i, id: %i, data: '%s'\n", p.ucSrcBoardID, p.ucPayloadID, p.pcPayload);
        ESP_ERROR_CHECK(xSPITransmit(&p));
        p.ucPayloadID ++;
    }
    vTaskDelete(NULL);
}

static void vSPINetifTxTask( void *pvParameters )
{
    vTaskDelay(2500 / portTICK_PERIOD_MS);

    SPIPayload_t p = {
        .ucSrcBoardID = xGetConfigBoardID(),
        .ucPayloadID = 0,
        .ePayloadType = SPI_PAYLOAD_TYPE_ESP_NETIF,
        .pcPayload = "netif payload",
    };
    while (true) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        printf("NETIF - sending src: %i, id: %i, data: '%s'\n", p.ucSrcBoardID, p.ucPayloadID, p.pcPayload);
        ESP_ERROR_CHECK(xSPITransmit(&p));
        p.ucPayloadID ++;
    }
    vTaskDelete(NULL);
}

esp_err_t xHandleSPIInternal(SPIPayload_t *p)
{
    if (p->ucSrcBoardID == xGetConfigBoardID()) 
    {
        // payload returned
        printf("Do not forward - %i -> %i, id: %i, data: '%s'\n", p->ucSrcBoardID, p->ucDstBoardID, p->ucPayloadID, p->pcPayload);
        return ESP_OK;
    }
    else if (p->ucDstBoardID == xGetConfigBoardID())
    {
        // process
        printf("Processing payload - %i -> %i, id: %i, data: '%s'\n", p->ucSrcBoardID, p->ucDstBoardID, p->ucPayloadID, p->pcPayload);
        return ESP_OK;
    }
    else
    {
        // not for me, forwarding
        printf("Forwarding - %i -> %i, id: %i, data: '%s'\n", p->ucSrcBoardID, p->ucDstBoardID, p->ucPayloadID, p->pcPayload);
        return xSPITransmit(p);
    }
}

esp_err_t xHandleSPINetif(SPIPayload_t *p)
{
    if (p->ucSrcBoardID == xGetConfigBoardID()) 
    {
        printf("Do not process - %i -> %i, id: %i, data: '%s'\n", p->ucSrcBoardID, p->ucDstBoardID, p->ucPayloadID, p->pcPayload);
        return ESP_OK;
    }
    // process
    printf("Processing payload - %i -> %i, id: %i, data: '%s'\n", p->ucSrcBoardID, p->ucDstBoardID, p->ucPayloadID, p->pcPayload);
    return ESP_OK;
}



static void vRxTask( void *pvParameters )
{
    SPIPayload_t p;
    while (true) {
        memset(&p, 0, sizeof(SPIPayload_t));
        ESP_ERROR_CHECK(xSPIReceive(&p));
        switch (p.ePayloadType)
        {
        case SPI_PAYLOAD_TYPE_INTERNAL:
            printf("INTERNAL - receiving src: %i, id: %i, data: '%s'\n", p.ucSrcBoardID, p.ucPayloadID, p.pcPayload);
            ESP_ERROR_CHECK_WITHOUT_ABORT(xHandleSPIInternal(&p));
            break;
        case SPI_PAYLOAD_TYPE_ESP_NETIF:
            printf("ESP-NETIF - src: %i, id: %i, data: '%s'\n", p.ucSrcBoardID, p.ucPayloadID, p.pcPayload);
            ESP_ERROR_CHECK_WITHOUT_ABORT(xHandleSPINetif(&p));
            break;
        default:
            ESP_LOGE(TAG, "Unknown SPI_PAYLOAD_TYPE '%i'", p.ePayloadType);
            break;
        }
    }
    vTaskDelete(NULL);
}



void app_main(void)
{
    ESP_ERROR_CHECK(xInitBoard());
    ESP_ERROR_CHECK(xSPIInit());
    xTaskCreate(vRxTask, "rx_task", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
    xTaskCreate(vSPIInternalTxTask, "tx_internal_task", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
    xTaskCreate(vSPINetifTxTask, "tx_netif_task", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
}
