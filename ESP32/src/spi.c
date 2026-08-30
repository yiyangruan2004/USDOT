#include "spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//  #define INT_GPIO       14
// static TaskHandle_t spi_task_handle = NULL;

// static void IRAM_ATTR gpio_isr_handler(void *arg){
//     (void)arg;

//     if (spi_task_handle == NULL) {
//         return;
//     }

//     BaseType_t higher_priority_task_woken = pdFALSE;
//     vTaskNotifyGiveFromISR(spi_task_handle, &higher_priority_task_woken);

//     if (higher_priority_task_woken) {
//         portYIELD_FROM_ISR();
//     }
// }

#define CS          10
#define MOSI        11
#define SCLK        12
#define MISO        13
#define SPI_FREQ    2*1000*1000        
static spi_device_handle_t device_handle;
bool spi_init() {        
    spi_bus_config_t bus_config = {
        .mosi_io_num = MOSI,   
        .miso_io_num = MISO,  
        .sclk_io_num = SCLK,  
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };  
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST,&bus_config,SPI_DMA_CH_AUTO));
    spi_device_interface_config_t device_config = {
        .clock_speed_hz = SPI_FREQ,         
        .mode = 0,                          
        .queue_size = 4,  
        .spics_io_num = CS,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &device_config, &device_handle));
    LOG("spi_init");

    // gpio_config_t io_conf = {
    //     .pin_bit_mask = (1ULL << INT_GPIO),
    //     .mode = GPIO_MODE_INPUT,
    //     .pull_up_en = 1,
    //     .intr_type = GPIO_INTR_NEGEDGE,
    // };
    // ESP_ERROR_CHECK(gpio_config(&io_conf));
    // ESP_ERROR_CHECK(gpio_install_isr_service(0));
    // ESP_ERROR_CHECK(gpio_isr_handler_add(INT_GPIO, gpio_isr_handler, NULL));
    LOG("int_init");
    return true;
}    

/*
 * - Mode 0, MSB-first, 4 bytes
 * - TX byte 0 = cmd, others are dummy
 * - RX byte 0 is dummy, others are response.
 * ESP32 is little-endian, so the command must be in the low byte of cmd.
 */

bool spi_trans(uint8_t *buffer, size_t len){
    if (buffer == NULL || len == 0) return false;
    LOG("spi_tx");
    spi_transaction_t trans = {
        .flags = 0,
        .length = len * 8,     // bits
        .tx_buffer = buffer,   // send original data
        .rx_buffer = buffer,   // overwrite with received data
    };
    ESP_ERROR_CHECK(spi_device_transmit(device_handle, &trans));
    LOG("spi_rx" );
    return true;
}


// void spi_task(void *arg){
//     static uint8_t rx[1024];
//     static uint8_t tx = 0xF0;   //read command
//     spi_task_handle = xTaskGetCurrentTaskHandle();

//     while (1){
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//         spi_transaction_t trans = {
//             .length = 1024 * 8,
//             .tx_buffer = &tx,
//             .rx_buffer = rx,
//         };
//         ESP_ERROR_CHECK(spi_device_transmit(device_handle, &trans));
//         udp_tx(rx, 1023);
//     }
// }

