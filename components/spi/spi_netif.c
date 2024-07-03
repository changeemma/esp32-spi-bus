#include "spi_netif.h"

static const char* TAG = "==> spi-netif";

static esp_err_t spi_netif_process(spi_payload_t *p)
{
    printf("call esp_netif_receive(netif, %s, %i, NULL)\n", p->buffer, p->len);
    return ESP_OK;
}

esp_err_t spi_netif_handler(spi_payload_t *p)
{
    if (spi_payload_is_from_device(p))  // bounced package
    {
        ESP_LOGW(TAG, "Discarding bounced packet. id '%i' ('%s').", p->id, p->buffer);
        return ESP_OK;
    }
    else if (spi_payload_is_for_device(p))  // payload for me
    {
        return spi_netif_process(p);
    }
    else  // not for me, forwarding
    {
        return spi_payload_forward(p);        
    }
}