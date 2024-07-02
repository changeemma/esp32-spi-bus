#pragma once

#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

bool broadcast_to_siblings(const void *msg, uint16_t len);

esp_err_t spi_forward(spi_payload_t *p);

esp_err_t spi_internal_handler(spi_payload_t *p);

#ifdef __cplusplus
}
#endif

