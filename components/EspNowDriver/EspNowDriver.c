
// -------------------------------------------------------Incluir librerias----------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "EspNowDriver.h"

// -------------------------------------------------Variables locales (globales)-----------------------------------------------------

static const char *TAG = "ESP-NOW";

static RingbufHandle_t private_ringbuf_handle = NULL;

uint8_t broadcast_adress[ESP_NOW_ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// -------------------------------------------------Inicializar el ringbuffer---------------------------------------------------------

static void init_ring_buffer(size_t ring_buffer_size)
{
    // Crear el ring buffer y inicializarlo
    private_ringbuf_handle = xRingbufferCreate(ring_buffer_size, RINGBUF_TYPE_BYTEBUF);
    assert(private_ringbuf_handle);
}

// ------------------------------------------------------Inicializar el wifi---------------------------------------------------------

static void init_wifi()
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(nvs_flash_init());
    
    wifi_init_config_t esp_now_wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&esp_now_wifi_config));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wifi inicializado correctamente");
}

// -------------------------------------------Funcion de callback al recibir datos por ESP-NOW---------------------------------------

void esp_now_receive_callback(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len)
{
    BaseType_t state = xRingbufferSendFromISR(private_ringbuf_handle, data, data_len, NULL);
    if (state != pdTRUE)
    {
        ESP_LOGW(TAG, "Error, no hay suficiente memoria en el ring buffer para que los datos sean guardados");
    }
}

// -------------------------------------------Funcion de callback al enviar datos por ESP-NOW----------------------------------------

void esp_now_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    /*if (status == ESP_NOW_SEND_SUCCESS)
    {
        ESP_LOGI(TAG, "Dato enviado por ESP-NOW correctamente");
    }
    else
    {
        ESP_LOGI(TAG, "Error enviando el dato por ESP-NOW");
    }*/
    ; // En teoria no tenemos que hacer nada aqui, ya que como transmitimos con broadcast siempre sera true
}

// ------------------------------------------------------Inicialziar ESP-NOW----------------------------------------------------------

static void init_esp_now(void)
{
    ESP_ERROR_CHECK(esp_now_init());
    
    ESP_ERROR_CHECK(esp_now_register_recv_cb(esp_now_receive_callback));

    ESP_ERROR_CHECK(esp_now_register_send_cb(esp_now_send_callback));

    ESP_LOGI(TAG, "ESP-NOW inicializado correctamente");
}

// ---------------------------------Agregar la mac de los dispositivos a las que se van a conectar------------------------------------

static void esp_now_register_peer(uint8_t *peer_adress)
{
    esp_now_peer_info_t esp_now_peer_info = {};
    memcpy(esp_now_peer_info.peer_addr, peer_adress, ESP_NOW_ETH_ALEN);
    esp_now_peer_info.channel = ESP_NOW_CHANNEL;
    esp_now_peer_info.ifidx = ESP_IF_WIFI_STA;

    ESP_ERROR_CHECK(esp_now_add_peer(&esp_now_peer_info));

    ESP_LOGI(TAG, "Correctamente registrado la direccion MAC: " MACSTR, MAC2STR(peer_adress));
}

// ----------------------------------------------------Enviar datos por ESP-NOW-------------------------------------------------------

void esp_now_send_data(uint8_t *data, size_t len)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_now_send(broadcast_adress, data, len));
}

// -----------------------------------Conseguir el puntero donde estan los datos guardados-------------------------------------------

uint8_t *dump_data_get_ptr(size_t *read_data_size, int timeout_ms)
{
    return (uint8_t *) xRingbufferReceive(private_ringbuf_handle, read_data_size, pdMS_TO_TICKS(timeout_ms));
}

// ------------------------------------Devolver los datos que ya se han mandado al altavoz-------------------------------------------

void return_item_to_ringbuff(uint8_t *to_return_buffer_ptr)
{
    vRingbufferReturnItem(private_ringbuf_handle, to_return_buffer_ptr);
}

// ---------------------------------------------------Inicializar el ESP-NOW---------------------------------------------------------

void init_radio_esp_now(size_t ring_buffer_size)
{
    init_ring_buffer(ring_buffer_size);
    
    init_wifi();
    init_esp_now();
    esp_now_register_peer(broadcast_adress);

    ESP_LOGI(TAG, "ESP-NOW Iniciado totalmente");
}