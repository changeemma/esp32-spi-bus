idf_component_register(SRCS "spi_lowlevel.c" "spi_payload.c" "spi_internal.c" "spi_netif.c" "spi.c"
    INCLUDE_DIRS "include"
    REQUIRES driver device
)