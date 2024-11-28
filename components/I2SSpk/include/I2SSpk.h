#pragma once
// Incluir las librerias para que funcione
#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "esp_err.h"
#include "esp_log.h"

// Declarar las funciones 

// Inicializar el altavoz, hay que iniciarlo con "i2s_spk_enable()" antes de usar
void i2s_spk_init(i2s_std_config_t* spk_i2s_std_config, gpio_num_t sd_pin);

// Despertar el canal del altavoz
void i2s_spk_enable();

// Poner a dormir el canal del altavoz
void i2s_spk_disable();

// Borrar el canal del altavoz
void i2s_spk_delete();

/**
 * En esta funcion se recibe datos uint8_t, los tranforma en int16_t y los envia por el bus i2s
 */
void i2s_spk_write_samples(uint8_t *raw_data_buffer, size_t count);