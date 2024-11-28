
#include <stdio.h>
#include "I2SSpk.h"

// Crear el handle del canal tx del bus i2s
static i2s_chan_handle_t tx_handle;

// Crear la variable donde se guarda el pin SD para que sea modificado
static gpio_num_t sd_pin_num = GPIO_NUM_NC;

// Configuracion del canal receptor (microfono)
i2s_chan_config_t tx_chan_config = 
{
    .id             = I2S_NUM_1,        // Canal emisor en el controlador 1
    .role           = I2S_ROLE_MASTER,  // El MCU sera quien proporcionara la señal del reloj
    .dma_desc_num   = 24,               // 24 buffers DMA
    .dma_frame_num  = 500,              // Cada buffer seran 500 muestras
    .auto_clear_before_cb = true        // No se que hace pero suena bien
};

// Para inicializar el altavoz
void i2s_spk_init(i2s_std_config_t* spk_i2s_std_config, gpio_num_t sd_pin)
{
    // Crear el bus i2s para el altavoz
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_config, &tx_handle, NULL));
    // Inicializar el canal, sin encender
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, spk_i2s_std_config));

    // Configurar el pin SD (que sirve para encender y apagar el MAX98357A) y iniciarlo
    gpio_config_t sd_pin_config = 
    {
        .pin_bit_mask = (1ULL << sd_pin ),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&sd_pin_config));

    // Guardar estaticamente el numero de este pin para que pueda usarlo en otras funciones
    sd_pin_num = sd_pin;
}

// Despertar el canal del altavoz
void i2s_spk_enable()
{
    // Encender el bus i2s del altavoz
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));

    // Encender el altavoz
    gpio_set_level(sd_pin_num, 1);
}

// Poner a dormir el canal del altavoz
void i2s_spk_disable()
{
    // Apagar el bus i2s del altavoz
    ESP_ERROR_CHECK(i2s_channel_disable(tx_handle));

    // Apagat el altavoz
    gpio_set_level(sd_pin_num, 0);
}

// Borrar el canal del altavoz
void i2s_spk_delete()
{
    // Borrar el microfono
    ESP_ERROR_CHECK(i2s_del_channel(tx_handle));
    sd_pin_num = GPIO_NUM_NC;
}

void i2s_spk_write_samples(uint8_t *raw_data_buffer, size_t count)
{
    // Buffer donde se guarda los datos que van a ser enviados
    int16_t to_send_buffer[250 * 2] = {0};

    // Este es el maximo que se puede enviar de golpe
    // Esto lo he definido yo por conveniencia propia y compatibilidad
    const size_t max_samples_to_send_in_one_time = 250;

    // Los bytes que se han enviado correctamente
    size_t bytes_written = 0;

    // Las muestras que aun se deben de enviar
    size_t samples_still_to_send = count;

    // El arreglo del desplazamiento
    size_t offset = 0;

    // Este es otro arreglo pero le he puesto el nombre como itinerador porque si, algun problema?
    size_t itinerator = 0;

    while (samples_still_to_send > 0)
    {
        // Si aun hay mas de 250 muestras que enviar entonces enviar 250, si hay menos entonces las que queden
        itinerator = (samples_still_to_send > max_samples_to_send_in_one_time) ?
                max_samples_to_send_in_one_time : samples_still_to_send;
        
        for (int i = 0; i < itinerator; i++)
        {
            // Tranformar de vuelta los datos a int16_t
            int16_t temp = (int16_t)(raw_data_buffer[i + offset] << 8) - 32768;
            
            // Mencionado anteriormente, esto es porque el max98357a hace un mixeo stereo de los datos que se envian
            to_send_buffer[i * 2] =     temp; // Canal izquierdo
            to_send_buffer[i * 2 + 1] = temp; // Canal derecho
        }
        
        // Enviar por el altavoz
        i2s_channel_write(tx_handle, to_send_buffer, itinerator * sizeof(int16_t) * 2, &bytes_written, 1000);

        // Actualizar el desplazamiento y las muestras que faltan por enviar
        offset += itinerator;
        samples_still_to_send -= itinerator;
    } 
}