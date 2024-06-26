idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES spi config esp_common esp_event nvs_flash
)
