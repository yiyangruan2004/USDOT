#pragma once
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "udp.h"

bool spi_init();
bool spi_trans(uint8_t *buffer, size_t len);
