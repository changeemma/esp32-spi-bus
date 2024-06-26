#ifndef __CONFIG_H
#define __CONFIG_H

#include "esp_log.h"
#include "driver/gpio.h"

#define CONFIG_GPIO_PIN_0 22 // the lowest bit
#define CONFIG_GPIO_PIN_1 21
#define CONFIG_GPIO_PIN_2 16 // the highest bit

typedef enum {
    CONFIG_ORIENTATION_NORTH = 0, // 000
    CONFIG_ORIENTATION_SOUTH = 1, // 001
    CONFIG_ORIENTATION_EAST = 2, // 010
    CONFIG_ORIENTATION_WEST = 3, // 011
    CONFIG_ORIENTATION_NONE = 9
} ConfigOrientation_t;

typedef enum {
    CONFIG_BOARD_MODE_PEER_LINK = 0,
    CONFIG_BOARD_MODE_ACCESS_POINT = 4, // 100 - Wi-Fi AccessPoint
    CONFIG_BOARD_MODE_ROOT = 5, // 101
    CONFIG_BOARD_MODE_NONE = 9
} ConfigBoardMode_t;

typedef uint8_t ConfigBoardID_t;

typedef struct {
    ConfigBoardID_t uConfigBoardID;
    ConfigBoardMode_t eConfigBoardMode;
    ConfigOrientation_t eConfigOrientation;
} Config_t;

void vReadConfig(void);
void vPrintConfig(void);
ConfigBoardID_t xGetConfigBoardID(void);
ConfigOrientation_t xGetConfigOrientation(void);
ConfigBoardMode_t xGetConfigBoardMode(void);

#endif
