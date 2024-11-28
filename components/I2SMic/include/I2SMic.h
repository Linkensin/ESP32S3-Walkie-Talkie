#pragma once
// Incluir las librerias para que funcione
#include "driver/i2s_std.h"
#include "esp_err.h"
#include "esp_log.h"

// Declarar las funciones 

// Inicializar el microfono, hay que iniciarlo con "i2s_mic_enable()" antes de usar
void i2s_mic_init(i2s_std_config_t* mic_i2s_std_config);

// Despertar el canal del microfono
void i2s_mic_enable();

// Poner a dormir el canal del microfono
void i2s_mic_disable();

// Borrar el canal del microfono
void i2s_mic_delete();

// Leer datos del canal del microfono
void i2s_mic_read_samples(uint8_t *store_buffer, size_t count);
