#include "spi.h"


const char *SPI_TAG = "==> spi-phy";

static SemaphoreHandle_t xTxSemaphore = NULL;
static spi_device_handle_t xTxHandle = {0};


esp_err_t xSPIRxInit() {
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=RECEIVER_GPIO_MOSI,
        .miso_io_num=-1,
        .sclk_io_num=RECEIVER_GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=RECEIVER_GPIO_CS,
        .queue_size=SPI_QUEUE_SIZE,
        .flags=0,
        //.post_setup_cb=spi_post_setup_cb,
        //.post_trans_cb=spi_post_trans_cb
    };

    //Initialize SPI slave interface
    ESP_ERROR_CHECK(spi_slave_initialize(SPI_RECEIVER_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
    return ESP_OK;
}

esp_err_t xSPITxInit() {
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=SENDER_GPIO_MOSI,
        .miso_io_num=-1,
        .sclk_io_num=SENDER_GPIO_SCLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=SPI_MASTER_FREQ_8M,
        .duty_cycle_pos=128,        //50% duty cycle
        .mode=0,
        .spics_io_num=SENDER_GPIO_CS,
        .cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size=SPI_QUEUE_SIZE
    };

    //Initialize the SPI bus and add the device we want to send stuff to.
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_SENDER_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_SENDER_HOST, &devcfg, &xTxHandle));
    xTxSemaphore = xSemaphoreCreateMutex();
    return ESP_OK;
}

esp_err_t xSPIInit(void) {
    ESP_ERROR_CHECK(xSPIRxInit());
    ESP_ERROR_CHECK(xSPITxInit());
    return ESP_OK;
}

esp_err_t xSPITransmit(SPIPayload_t *p) {
    spi_transaction_t t = {
        .length = (sizeof(SPIPayload_t)) * 8,
        .tx_buffer = (void *) p,
    };
    if( xSemaphoreTake( xTxSemaphore, ( TickType_t ) 10 ) == pdTRUE )
    {
        ESP_ERROR_CHECK(spi_device_transmit(xTxHandle, &t));
        xSemaphoreGive( xTxSemaphore );
        return ESP_OK;
    }
    ESP_LOGW(SPI_TAG, "Could not adquire Mutex. Will retry...");
    return ESP_FAIL;
}

esp_err_t xSPIReceive(SPIPayload_t *p)
{
    spi_slave_transaction_t t = {
        .rx_buffer = (void *) p,
        .length = (sizeof(SPIPayload_t)) * 8,
    };
    ESP_ERROR_CHECK(spi_slave_transmit(SPI_RECEIVER_HOST, &t, portMAX_DELAY));
    return ESP_OK;
}
