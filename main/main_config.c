
#include "main_config.h"

//-----------------------------------------------------rx_i2s_std_config-------------------------------------------------------

// Aplicar las configuraciones estandar del microfono
i2s_std_config_t rx_i2s_std_config =
{
    .clk_cfg = 
    {
        .sample_rate_hz = SAMPLE_RATE,              // Tasa de muestreo
        .clk_src        = I2S_CLK_SRC_DEFAULT,      // Predeterminado
        .mclk_multiple  = I2S_MCLK_MULTIPLE_256     // Predeterminado
    },
    .slot_cfg = 
    {
        .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT, // 16 bits por muestra
        .slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT, // Predeterminado
        .slot_mode = I2S_SLOT_MODE_MONO,            // Solo un canal
        .slot_mask = I2S_STD_SLOT_LEFT,             // El canal izquierdo
        .ws_width = I2S_SLOT_BIT_WIDTH_16BIT,       // Predeterminado
        .ws_pol = false,                            // Predeterminado   
        .bit_shift = true,                          // Predeterminado
        .left_align = true,                         // Predeterminado
        .big_endian = false,                        // Predeterminado
        .bit_order_lsb = false                      // Predeterminado
    },
    .gpio_cfg = 
    {
        .mclk = GPIO_NUM_NC,                        // No se usa
        .bclk = MIC_SCK_PIN,                        // Pin de la señal del reloj
        .ws = MIC_WS_PIN,                           // Pin del la señal world select
        .dout = GPIO_NUM_NC,                        // No se usa
        .din = MIC_SD_PIN,                          // Pin de la señal de datos de entrada
        .invert_flags = 
        {
            .mclk_inv = false,                      // Predeterminado
            .bclk_inv = false,                      // Predeterminado
            .ws_inv = false                         // Predeterminado
        }
    }
};

//-----------------------------------------------------tx_i2s_std_config-------------------------------------------------------

// Aplicar las configuraciones estandar del altavoz
i2s_std_config_t tx_i2s_std_config =
{
    .clk_cfg = 
    {
        .sample_rate_hz = (SAMPLE_RATE / 2),        // Tasa de muestreo dividido entre dos por problemas del mic
        .clk_src        = I2S_CLK_SRC_DEFAULT,      // Predeterminado
        .mclk_multiple  = I2S_MCLK_MULTIPLE_256     // Predeterminado
    },
    .slot_cfg = 
    {
        .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT, // 16 bits por muestra
        .slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT, // Predeterminado
        .slot_mode = I2S_SLOT_MODE_STEREO,          // Los dos canales
        .slot_mask = I2S_STD_SLOT_BOTH,             // Los dos canales
        .ws_width = I2S_SLOT_BIT_WIDTH_16BIT,       // Predeterminado
        .ws_pol = false,                            // Predeterminado   
        .bit_shift = true,                          // Predeterminado
        .left_align = true,                         // Predeterminado
        .big_endian = false,                        // Predeterminado
        .bit_order_lsb = false                      // Predeterminado
    },
    .gpio_cfg = 
    {
        .mclk = GPIO_NUM_NC,                        // No se usa
        .bclk = SPK_BCLK_PIN,                       // Pin de la señal del reloj
        .ws = SPK_LRC_PIN,                          // Pin del la señal world select
        .dout = SPK_DIN_PIN,                        // Pin de la señal de datos de salida
        .din = GPIO_NUM_NC,                         // No se usa
        .invert_flags = 
        {
            .mclk_inv = false,                      // Predeterminado
            .bclk_inv = false,                      // Predeterminado
            .ws_inv = false                         // Predeterminado
        }
    }
};

//-------------------------------------------------inicializar el pin ppt------------------------------------------------------

void ppt_button_init(gpio_num_t ppt_button_pin)
{
    // Configurar el boton ppt y iniciarlo
    gpio_config_t ppt_pin_config = 
    {
        .pin_bit_mask = (1ULL << ppt_button_pin ),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&ppt_pin_config));
}