/*
    MIT License

    Copyright (c) 2024 Linkensin

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

// ------------------------------------------------------Incluir librerias------------------------------------------------------------

// Librerias estandar de C
#include <stdio.h>
#include <string.h>

// Librerias de Espressif
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

// Mis propias librerias
#include "main_config.h"
#include "EspNowDriver.h"
#include "I2SMic.h"
#include "I2SSpk.h"

// ------------------------------------------------------Variables globales-----------------------------------------------------------

// El tag para esp_log
static const char *TAG = "MAIN";

// -----------------------------------------------------------APP MAIN-----------------------------------------------------------------

void app_main(void)
{
    // Crear el buffer donde se guardan los datos leidos del mic
    uint8_t *mic_read_buffer = (uint8_t *) heap_caps_malloc(READ_BUFFER_SIZE, MALLOC_CAP_DMA);
    assert(mic_read_buffer);

    // Inicializar ESP-NOW
    init_radio_esp_now(OUTPUT_RINGBUF_SIZE);

    // Inicializar el microfono
    i2s_mic_init(&rx_i2s_std_config);

    // Inicializar el altavoz
    i2s_spk_init(&tx_i2s_std_config, SPK_SD_PIN);

    // Inicializar el boton PPT
    ppt_button_init(PPT_BUTTON_PIN);

    while(1)
    {
        if(gpio_get_level(PPT_BUTTON_PIN))
        {
            // Como el boton esta oprimido entonces entramos en modo transmission, encender el microfono
            i2s_mic_enable();

            // Conseguir el tiempo incial cuando se oprime el boton ppt
            int64_t start_time = esp_timer_get_time();
            // Si no ha passado 1s o el boton sigue oprimido entonces seguir dentro del bucle
            while((esp_timer_get_time() - start_time) < 1000000 || gpio_get_level(PPT_BUTTON_PIN))
            {
                // Leer datos del mic y guardarlos al buffer
                i2s_mic_read_samples(mic_read_buffer, READ_SAMPLES_COUNT);

                // Enviar los datos por esp-now
                esp_now_send_data(mic_read_buffer, READ_SAMPLES_COUNT);

                // Si no haces un delay aqui entonces explota el programa, y voy en serio
                vTaskDelay( 10 / portTICK_PERIOD_MS );
            }

            // Apagar el microfono
            i2s_mic_disable();
        }
        else
        {
            // Como el boton no esta oprimido, entonces entramos en modo receptor y encendemos el alatvoz
            i2s_spk_enable();

            // Si el boton sigue sin ser oprimido pues seguir leiendo del ring buffer y mandar por le mic
            while(!gpio_get_level(PPT_BUTTON_PIN))
            {
                size_t ringbuf_received_data_length = 0;

                uint8_t *data_buffer_ptr = dump_data_get_ptr(&ringbuf_received_data_length, 10);
                
                if(data_buffer_ptr != NULL)
                {
                    i2s_spk_write_samples(data_buffer_ptr, ringbuf_received_data_length);

                    return_item_to_ringbuff(data_buffer_ptr);
                }
            }

            // Apagar el altavoz
            i2s_spk_disable();
        }
        
        vTaskDelay( 10 / portTICK_PERIOD_MS );
    }
}