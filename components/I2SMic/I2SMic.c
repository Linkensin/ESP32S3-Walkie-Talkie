
#include <stdio.h>
#include "I2SMic.h"

// Crear el tag de este componente
static const char *TAG = "I2S_Mic";

// Crear el handle del canal rx del bus i2s
static i2s_chan_handle_t rx_handle;

// Configuracion del canal receptor (microfono)
i2s_chan_config_t rx_chan_config = 
{
    .id             = I2S_NUM_0,        // Canal receptor en el controlador 0
    .role           = I2S_ROLE_MASTER,  // El MCU sera quien proporcionara la señal del reloj
    .dma_desc_num   = 24,               // 24 buffers DMA
    .dma_frame_num  = 250,              // Cada buffer seran 250 muestras
    .auto_clear     = true              // No importa
};

// Para inicializar el microfono
void i2s_mic_init(i2s_std_config_t* mic_i2s_std_config)
{
    // Crear el bus i2s para el microfono
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_config, NULL, &rx_handle));
    // Inicializar el canal, sin encender
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, mic_i2s_std_config));
}

// Para encender el microfono
void i2s_mic_enable()
{
    // Encender el microfono
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
}

// Para apagar el microfono
void i2s_mic_disable()
{
    // Apagar el microfono
    ESP_ERROR_CHECK(i2s_channel_disable(rx_handle));
}

// Para borrar totalmente el bus del microfono
void i2s_mic_delete()
{
    // Borrar el microfono
    ESP_ERROR_CHECK(i2s_del_channel(rx_handle));
}

// Leer datos del microfono y guardarlos al buffer del puntero
void i2s_mic_read_samples(uint8_t *store_buffer, size_t count)
{
    // Buffer donde se guardaran los datos del microfono
    int32_t mic_raw_data_buffer[250] = {0};

    // Cuantas muestras se tienen que leer?
    size_t samples_to_read = count;

    // Assegurarnos que tengamos suficiente espacio en el buffer alocado donde guardar datos leidos
    if (samples_to_read > 250)
    {
        samples_to_read = 250;
        ESP_LOGV(TAG, "Buffer demasiado pequeño para contener todos los datos que se quieren leer");
    }

    // Los bytes que lean
    size_t bytes_read = 0;
    
    // Leer los datos
    ESP_ERROR_CHECK(i2s_channel_read(rx_handle, mic_raw_data_buffer, samples_to_read * sizeof(int32_t), &bytes_read, 1000));

    // Calcular cuantas muestras se han leido realmente
    size_t samples_read = bytes_read / sizeof(int32_t);

    // Processar los datos leidos
    for(int i = 0; i < samples_read; i++)
    {
        //Aqui se transforma primero el dato a uint16_t y luego se le truncan 8 bits para transformar en uint8_t
        store_buffer[i] = (uint8_t)((mic_raw_data_buffer[i] + 32767) >> 8);
    }

    /*int16_t output_try[250] = {0};
    for (int k = 0; k < samples_read; k++)
    {
        output_try[k] = (int16_t)(store_buffer[k] << 8) - 32768;
    }

    // Print the values to serial plotter (Use arduino serial plotter)
    for(int j = 0; j < samples_read; j++)
    {
        //printf("%d %d %d\n", 0, store_buffer[j], 255);
        printf("%d %d %d\n", 32767, output_try[j], -32767);
    }*/
}