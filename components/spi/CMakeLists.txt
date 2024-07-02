idf_component_register(SRCS "spi.c" "spi_internal.c" "spi_netif.c"
    INCLUDE_DIRS "include"
    REQUIRES driver device
)