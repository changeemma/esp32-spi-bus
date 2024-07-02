#include "spi_internal.h"

static const char* TAG = "==> spi-internal";

esp_err_t spi_forward(spi_payload_t *p)
{
    if (p->ttl <= 0) {
        ESP_LOGW(TAG, "Discarding packet id '%i' ('%s') due to TTL. Won't forward.", p->id, p->buffer);
        return ESP_OK;
    }
    p->ttl--;
    ESP_LOGI(TAG, "Forwarding packet id '%i' with dest '%i' (TTL=%i).", p->id, p->dst_device_id, p->ttl);
    return spi_transmit(p, sizeof(spi_payload_t));
}

static esp_err_t spi_internal_process(spi_payload_t *p)
{
    printf("call on_sibling_message(%s, %i)\n", p->buffer, p->len);
    return ESP_OK;
}

static esp_err_t spi_internal_broadcast(const void *buffer, uint16_t len){
    spi_payload_t p = {
        .id = 0,
        .ttl = SPI_PAYLOAD_TTL,
        .src_device_id = device_config_get_id(),
        .dst_device_id = DEVICE_ID_ALL,
        .buffer_type = SPI_PAYLOAD_TYPE_INTERNAL,
        .len = len,
    };
    memccpy(p.buffer, buffer, len, SPI_PAYLOAD_BUFFER_SIZE);
    ESP_ERROR_CHECK(spi_transmit(&p, sizeof(spi_payload_t)));

    // wait for reception
    return ESP_OK;
}

bool broadcast_to_siblings(const void *msg, uint16_t len)
{
    esp_err_t rc = spi_internal_broadcast(msg, len);
    return rc == ESP_OK? true : false;
}

static esp_err_t spi_internal_broadcast_handler(spi_payload_t *p)
{
    // broadcast origin
    if (p->src_device_id == device_config_get_id())
    {
        ESP_LOGI(TAG, "Broadcast complete for packet id '%i'.", p->id);
        // notify broadcast complete
        return ESP_OK;
    }
    else
    {
        ESP_ERROR_CHECK(spi_internal_process(p));
        return spi_forward(p);
    }
}

esp_err_t spi_internal_handler(spi_payload_t *p)
{
    if (p->dst_device_id == DEVICE_ID_ALL)  // broadcast
    {
        return spi_internal_broadcast_handler(p);
    }
    else if ((p->dst_device_id == device_config_get_id()) \
        || (p->dst_device_id == DEVICE_ID_ANY))  // payload for me
    {
        return spi_internal_process(p);
    }
    else  // not for me, forwarding
    {
        return spi_forward(p);        
    }
}
