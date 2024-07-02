#include "spi_netif.h"


static esp_err_t spi_netif_process(spi_payload_t *p)
{
    printf("esp_netif_receive(netif, %s, %i, NULL)\n", p->buffer, p->len);
    return ESP_OK;
}

esp_err_t spi_netif_handler(spi_payload_t *p)
{
    if (p->src_device_id == device_config_get_id())  // bounced package
    {
        printf("Discard payload\n");
        return ESP_OK;
    }
    else if ((p->dst_device_id == device_config_get_id()) \
        || (p->dst_device_id == DEVICE_ID_ANY))  // payload for me
    {
        return spi_netif_process(p);
    }
    else  // not for me, forwarding
    {
        return spi_forward(p);        
    }
}