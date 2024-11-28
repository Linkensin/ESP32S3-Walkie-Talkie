#pragma once
#include <stdio.h>
#include "driver/i2s_std.h"
#include "driver/gpio.h"

//-------------------------------------------------Seccion para escojer y configurar---------------------------------------------

/**
 * La tasa de muestreo, hay que tener en cuenta que como estamos leiendo un INMP441 y la nueva libreria tiene
 * un problema con ese microfono ya que segun la documentacion del inmp441 se necessita 64 clock cycles para poder
 * leer 1 muestra, pero de predeterminado el esp32 solo manda 32 clock cicles, eso hace que leamos la mitad del+
 * sample rate (obviamente hay que luego dividir por 2 este sample rate en la config del altavoz, pero eso ya esta
 * hecho)
 * Por esto, en realidad en sample rate real es este numero dividido entre dos
 */
#define SAMPLE_RATE     32000

/**
 * Es muy raro pero definiendo 32000 para sample rate hay un delay de 1 segundo algo asi por la transmission, pero
 * si defines el sample rate a 48000 la transmission es super a tiempo real y con una fidelidad de audio musical
 * increible, aunque para voces como es tan sensible entonces ya se vuelve malo, por eso la mejor opcion sigue
 * siendo usar de sample rate 32000 (16000)
 */
//#define SAMPLE_RATE 48000

/**0
 * Los pines del microfono, estos nombres son los que estan escritas en las placas
 * El pin L/R se conecta al GND
 */
#define MIC_SD_PIN      GPIO_NUM_4
#define MIC_WS_PIN      GPIO_NUM_5
#define MIC_SCK_PIN     GPIO_NUM_6

/**
 * Los pines del atltavoz, estos nombres son los que estan escritas en las placas
 */
#define SPK_SD_PIN      GPIO_NUM_7
#define SPK_LRC_PIN     GPIO_NUM_15
#define SPK_BCLK_PIN    GPIO_NUM_16
#define SPK_DIN_PIN     GPIO_NUM_17

/**
 * El pin donde esta conectado el boton PPT, este boton tiene que ser un boton cerrado que corte la 
 * corriente, asi es porque en el programa define ese pin con una resistencia pulldown y se acciona
 * al recibir la corriente
 */
#define PPT_BUTTON_PIN  GPIO_NUM_8

//--------------------------------------------------Constantes y definiciones----------------------------------------------------

/**
 * Configuraciones del buffer de lectura del microfono
 */
#define READ_SAMPLES_COUNT  250
#define READ_SAMPLES_SIZE   1
#define READ_BUFFER_SIZE    READ_SAMPLES_COUNT * READ_SAMPLES_SIZE

/**
 * El tamaño del ring buffer, tendria que ser multiple de READ_SAMPLES_COUNT (250)
 */
#define OUTPUT_RINGBUF_SIZE 5000

//------------------------------------------------------Externs y funciones------------------------------------------------------

/**
 * Esta es la configuracion del microfono 
 */
extern i2s_std_config_t rx_i2s_std_config; 

/**
 * Esta es la configuracion del altavoz, este canal esta configurado para ser both slot aunque solo tengamos
 * un solo altavoz, esto se debe a que el modulo MAX98357A que se ha escojido hace una suma de (L+R)/2 para
 * sacar la intensidad final, es muy trol ya que esto me obliga a copiar dos veces los datos para que sean
 * enviado correctamente
 */
extern i2s_std_config_t tx_i2s_std_config;

/**
 * Funcion para inicializar el boton PPT
 */
void ppt_button_init(gpio_num_t ppt_button_pin);