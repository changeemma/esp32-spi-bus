idf_component_register(
    SRCS "main.c" "test_spi.c"
    INCLUDE_DIRS "."
    REQUIRES spi device esp_common esp_event nvs_flash
)
