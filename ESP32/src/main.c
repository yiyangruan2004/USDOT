#include "udp.h"
#include "spi.h"

void app_main() {
    //xTaskCreate(task_func, "task_name", stack_size, var_ptr, priority, task_handle); 
    spi_init();
    wifi_init();
    xTaskCreate(udp_task, "udp_task", 2048, NULL, 4, NULL); 
    //xTaskCreate(spi_task, "spi", 2048, NULL, 0, NULL); 
}

