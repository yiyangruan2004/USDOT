#pragma once
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "errno.h"

#include "spi.h"

#define CH_CNT  2

#define LOG(msg, ...) \
    ESP_LOGI("", "\x1b[32m" msg "\x1b[0m", ##__VA_ARGS__)

bool wifi_init();
void udp_task(void *arg);