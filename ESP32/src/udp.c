#include "udp.h"


#define ESP_PORT    12345
static int sock;
bool wifi_init() {
    // Hardware config
    ESP_ERROR_CHECK(nvs_flash_init());                                  // Non-volatile flash 
    ESP_ERROR_CHECK(esp_netif_init());                                  // First layer above hardware
    ESP_ERROR_CHECK(esp_event_loop_create_default());                   // Another layer
    esp_netif_create_default_wifi_ap();                                 // ESP behaves as AP access point similar to router
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();   
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));          
    // Software config
    wifi_config_t wifi_config = {                           
        .ap = {
            .ssid = "USDOT",
            .ssid_len = strlen("USDOT"),
            .password = "",
            .max_connection = 2,
            .authmode = WIFI_AUTH_OPEN,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    LOG("udp_init");

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        LOG("Unable to create socket: errno %d", errno);
        return false;
    }
    struct sockaddr_in esp_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(ESP_PORT)
    };
    int err = bind(sock, (struct sockaddr *)&esp_addr, sizeof(esp_addr));
    if (err < 0) {
        LOG("Socket unable to bind: errno %d", errno);
        return false;
    }
    LOG("Socket bound to port %d", ESP_PORT);
    return true;
}

static struct sockaddr_in src_addr;
static socklen_t src_len;
bool udp_tx(uint8_t *buffer, ssize_t len){
    if (sendto(sock, buffer, len, 0, (struct sockaddr *)&src_addr, src_len) < 0) {
        LOG("udp_tx failed: errno %d", errno);
        return false;
    }
    LOG("udp_tx");
    return true;
}

static DMA_ATTR uint8_t buffer[1024];
void udp_task(void *arg){
    while (1) {
        src_len = sizeof(src_addr);
        ssize_t len = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&src_addr, &src_len);
        if (len > 0){ 
            if(buffer[0] == 'R'){
                spi_trans(buffer, len);
                if(buffer[1] != 'R'){    
                    buffer[0] = 'N';
                }
                udp_tx(buffer, len);
            }else{
                spi_trans(buffer, len);  
                udp_tx(buffer, len);
            }
        }
    }
}

