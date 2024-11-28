#pragma once
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define ESP_NOW_CHANNEL         1

void init_radio_esp_now(size_t ring_buffer_size);
void esp_now_send_data(uint8_t *data, size_t len);

uint8_t *dump_data_get_ptr(size_t *read_data_size, int timeout_ms);
void return_item_to_ringbuff(uint8_t *to_return_buffer_ptr);