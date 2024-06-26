#include "config.h"

const char *CONFIG_TAG = "==> config";

Config_t config = {
    .eConfigBoardMode = CONFIG_BOARD_MODE_NONE,
    .eConfigOrientation = CONFIG_ORIENTATION_NONE,
};

bool uxReadPin(uint8_t pin){
    int pin_state;

    // enable gpio pin
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << pin),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    gpio_config(&io_conf);

    // get pin state
    pin_state = gpio_get_level(pin);

    // disable gpio pin
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);
    return pin_state == 0? false : true;
}

void vReadConfig(void){
    // Initialize config_bits with all zeros
    char config_bits = 0b0;

    // Set the corresponding bits in config_bits based on GPIO pin values
    config_bits |= (uxReadPin(CONFIG_GPIO_PIN_0) << 0);
    config_bits |= (uxReadPin(CONFIG_GPIO_PIN_1) << 1);
    config_bits |= (uxReadPin(CONFIG_GPIO_PIN_2) << 2);

    // Set the mode and orientation based on config_bits
    config.uConfigBoardID = (ConfigBoardID_t) config_bits;
    config.eConfigBoardMode = config_bits >> 2 == 0? CONFIG_BOARD_MODE_PEER_LINK : (ConfigBoardMode_t) config_bits;
    config.eConfigOrientation = config_bits >> 2 == 0? (ConfigOrientation_t) config_bits : CONFIG_ORIENTATION_NONE;
}

ConfigBoardID_t xGetConfigBoardID(void){
    return config.uConfigBoardID;
}

ConfigBoardMode_t xGetConfigBoardMode(void) {
    return config.eConfigBoardMode;
}

ConfigOrientation_t xGetConfigOrientation(void){
    return config.eConfigOrientation;
}

void vPrintConfigBoardMode(void) {
    switch(xGetConfigBoardMode()) {
        case CONFIG_BOARD_MODE_PEER_LINK:
            ESP_LOGI(CONFIG_TAG, "CONFIG_BOARD_MODE_PEER_LINK");
            break;
        case CONFIG_BOARD_MODE_ACCESS_POINT:
            ESP_LOGI(CONFIG_TAG, "CONFIG_BOARD_MODE_ACCESS_POINT (100)");
            break;
        case CONFIG_BOARD_MODE_ROOT:
            ESP_LOGI(CONFIG_TAG, "CONFIG_BOARD_MODE_ROOT (101)");
            break;
        case CONFIG_BOARD_MODE_NONE:
            ESP_LOGI(CONFIG_TAG, "CONFIG_BOARD_MODE_NONE");
            break;
        default:
            ESP_LOGW(CONFIG_TAG, "Invalid Mode (%i)", xGetConfigBoardMode());
            break;
    }
}

void vPrintConfigOrientation(void) {
    switch(xGetConfigOrientation()) {
        case CONFIG_ORIENTATION_NORTH:
            ESP_LOGI(CONFIG_TAG, "CONFIG_ORIENTATION_NORTH (000)");
            break;
        case CONFIG_ORIENTATION_SOUTH:
            ESP_LOGI(CONFIG_TAG, "CONFIG_ORIENTATION_SOUTH (001)");
            break;
        case CONFIG_ORIENTATION_EAST:
            ESP_LOGI(CONFIG_TAG, "CONFIG_ORIENTATION_EAST (010)");
            break;
        case CONFIG_ORIENTATION_WEST:
            ESP_LOGI(CONFIG_TAG, "CONFIG_ORIENTATION_WEST (011)");
            break;
        case CONFIG_ORIENTATION_NONE:
            ESP_LOGI(CONFIG_TAG, "CONFIG_ORIENTATION_NONE");
            break;
        default:
            ESP_LOGW(CONFIG_TAG, "Invalid Orientation (%i)", xGetConfigOrientation());
            break;
    }
}

void vPrintConfig(void){
    ESP_LOGI(CONFIG_TAG, "Board ID: '%i'", xGetConfigBoardID());
    vPrintConfigBoardMode();
    vPrintConfigOrientation();
}