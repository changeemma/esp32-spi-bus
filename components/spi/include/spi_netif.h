#pragma once

#include "spi_payload.h"
#include "spi_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t spi_netif_handler(spi_payload_t *p);

#ifdef __cplusplus
}
#endif

