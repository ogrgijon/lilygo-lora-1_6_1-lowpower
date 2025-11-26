#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// CONFIGURACIÓN DE HARDWARE (PLACA)
// =============================================================================

// Selección de placa LilyGo LoRa
// Descomenta solo UNA de las siguientes líneas según tu placa

// #define T3_V1_3_SX1276    // T3 V1.3 con SX1276
// #define T3_V1_3_SX1278    // T3 V1.3 con SX1278
#define T3_V1_6_SX1276    // T3 V1.6 con SX1276
// #define T3_V1_6_SX1278    // T3 V1.6 con SX1278
// #define T_BEAM_SX1276     // T-Beam con SX1276
// Otras placas disponibles en hardware_config.h

#include <Arduino.h>
#include "hardware_config.h"  // Configuración específica del hardware

// Incluir configuración LoRaWAN (con las claves)
#include <lmic.h>
#include "lorawan_config.h"

// =============================================================================
// CONFIGURACIÓN DE SENSORES - SISTEMA MULTISENSOR
// =============================================================================
// Ahora puedes habilitar múltiples sensores al mismo tiempo.
// Cada sensor puede estar conectado independientemente.

// HABILITACIÓN DE SENSORES - ¡DESCOMENTA LOS QUE QUIERAS USAR!
#define ENABLE_SENSOR_DHT22      // DHT22 (Temperatura + Humedad)
//#define ENABLE_SENSOR_DHT11      // DHT11 (Temperatura + Humedad)
//#define ENABLE_SENSOR_DS18B20    // DS18B20 (Temperatura)
//#define ENABLE_SENSOR_BMP280     // BMP280 (Presión + Temperatura)
//#define ENABLE_SENSOR_HCSR04     // HC-SR04 (Distancia)
// #define ENABLE_SENSOR_NONE     // Sin sensores adicionales (solo batería)

// =============================================================================
// INCLUSIÓN AUTOMÁTICA DE LOS SENSORES HABILITADOS
// =============================================================================

// Incluir headers de sensores habilitados
#ifdef ENABLE_SENSOR_DHT22
#include "sensor/sensor_dht22.h"
#endif
#ifdef ENABLE_SENSOR_DHT11
#include "sensor/sensor_dht11.h"
#endif
#ifdef ENABLE_SENSOR_DS18B20
#include "sensor/sensor_ds18b20.h"
#endif
#ifdef ENABLE_SENSOR_BMP280
#include "sensor/sensor_bmp280.h"
#endif
#ifdef ENABLE_SENSOR_HCSR04
#include "sensor/sensor_hcsr04.h"
#endif
#ifdef ENABLE_SENSOR_NONE
#include "sensor/sensor_none.h"
#endif

// =============================================================================
// CONFIGURACIÓN DE PANTALLA OLED
// =============================================================================

#define ENABLE_DISPLAY true          // true: pantalla activa, false: apagada para máximo ahorro
#define SHOW_ACTIVITY_INDICATORS true // true: mostrar indicadores permanentes, false: solo mensajes

// Duraciones de mensajes (ms)
#define MESSAGE_DURATION_SUCCESS 5000
#define MESSAGE_DURATION_ERROR 3000
#define MESSAGE_DURATION_WARNING 4000
#define MESSAGE_DURATION_INFO 3000
#define MESSAGE_DURATION_SLEEP 3000

// =============================================================================
// CONFIGURACIÓN LoRaWAN
// =============================================================================

// Región LoRaWAN
#define LORAWAN_REGION LMIC_region_t::LMIC_REGION_eu868

// Parámetros de transmisión
#define TX_POWER_DBM 17              // Potencia de transmisión (máx 17 para evitar interferencias)
#define BACKOFF_INITIAL_SECONDS 300  // Backoff inicial exponencial

// =============================================================================
// CLAVES LoRaWAN OTAA (¡MODIFICA EN lorawan_config.h!)
// =============================================================================
// Las claves LoRaWAN se definen en lorawan_config.h para mantenerlas separadas
// de la configuración general (por seguridad y organización)

// =============================================================================
// CONFIGURACIÓN DE TIMING Y ENERGÍA
// =============================================================================

#define SEND_INTERVAL_SECONDS 300    // Intervalo entre envíos (mínimo 60s para evitar sobrecarga)
#define WATCHDOG_TIMEOUT_MINUTES 5   // Timeout del watchdog en minutos

// Energía y batería
#define ENABLE_SOLAR_CHARGING true   // Habilitar carga solar
#define BATTERY_LOW_THRESHOLD 20     // Umbral de batería baja (%)

// =============================================================================
// CONFIGURACIÓN DE DEPURACIÓN Y LOGGING
// =============================================================================

#define ENABLE_SERIAL_LOGS true      // Habilitar logs por Serial
#define LOG_LEVEL 1                  // 0: ninguno, 1: básico, 2: detallado
#define SHOW_TTN_DECODER true  // true: mostrar decoder TTN por Serial al iniciar

// =============================================================================
// CONFIGURACIÓN DE PAYLOAD Y DATOS
// =============================================================================

// Determinación automática de capacidades del sistema multisensor
#if (defined(ENABLE_SENSOR_DHT22) && SENSOR_HAS_TEMPERATURE) || \
    (defined(ENABLE_SENSOR_DHT11) && SENSOR_HAS_TEMPERATURE) || \
    (defined(ENABLE_SENSOR_DS18B20) && SENSOR_HAS_TEMPERATURE) || \
    (defined(ENABLE_SENSOR_BMP280) && SENSOR_HAS_TEMPERATURE)
#define SYSTEM_HAS_TEMPERATURE 1
#else
#define SYSTEM_HAS_TEMPERATURE 0
#endif

#if (defined(ENABLE_SENSOR_DHT22) && SENSOR_HAS_HUMIDITY) || \
    (defined(ENABLE_SENSOR_DHT11) && SENSOR_HAS_HUMIDITY)
#define SYSTEM_HAS_HUMIDITY 1
#else
#define SYSTEM_HAS_HUMIDITY 0
#endif

#if defined(ENABLE_SENSOR_BMP280) && SENSOR_HAS_PRESSURE
#define SYSTEM_HAS_PRESSURE 1
#else
#define SYSTEM_HAS_PRESSURE 0
#endif

#if defined(ENABLE_SENSOR_HCSR04) && SENSOR_HAS_DISTANCE
#define SYSTEM_HAS_DISTANCE 1
#else
#define SYSTEM_HAS_DISTANCE 0
#endif

// Cálculo del tamaño del payload usando directivas del preprocesador
#if defined(ENABLE_SENSOR_DHT22) || defined(ENABLE_SENSOR_DHT11) || defined(ENABLE_SENSOR_DS18B20) || defined(ENABLE_SENSOR_BMP280)
#define PAYLOAD_SIZE_TEMPERATURE 2
#else
#define PAYLOAD_SIZE_TEMPERATURE 0
#endif

#if defined(ENABLE_SENSOR_DHT22) || defined(ENABLE_SENSOR_DHT11)
#define PAYLOAD_SIZE_HUMIDITY 2
#else
#define PAYLOAD_SIZE_HUMIDITY 0
#endif

#if defined(ENABLE_SENSOR_BMP280)
#define PAYLOAD_SIZE_PRESSURE 2
#else
#define PAYLOAD_SIZE_PRESSURE 0
#endif

#if defined(ENABLE_SENSOR_HCSR04)
#define PAYLOAD_SIZE_DISTANCE 2
#else
#define PAYLOAD_SIZE_DISTANCE 0
#endif

#define PAYLOAD_SIZE_BYTES ( \
    PAYLOAD_SIZE_TEMPERATURE + \
    PAYLOAD_SIZE_HUMIDITY + \
    PAYLOAD_SIZE_PRESSURE + \
    PAYLOAD_SIZE_DISTANCE + \
    2 /* batería */ \
)

// Valores de error para lecturas fallidas
#define SENSOR_ERROR_TEMPERATURE -999.0f
#define SENSOR_ERROR_HUMIDITY -1.0f
#define SENSOR_ERROR_PRESSURE -1.0f
#define SENSOR_ERROR_DISTANCE -1.0f
#define SENSOR_ERROR_BATTERY -1.0f

// =============================================================================
// ESTRUCTURAS DE DATOS PARA SENSORES (MODIFICABLES POR EL USUARIO)
// =============================================================================
// Estas estructuras pueden ser modificadas al añadir nuevos sensores

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Estructura que contiene todas las lecturas del sensor
 * @note MODIFICA esta estructura al añadir nuevos tipos de datos de sensores
 */
typedef struct {
    float temperature;    /**< Temperatura en °C */
    float humidity;       /**< Humedad relativa en % */
    float pressure;       /**< Presión atmosférica en hPa */
    float distance;       /**< Distancia en cm */
    float battery;        /**< Voltaje de batería en V */
    bool valid;           /**< true si todas las lecturas son válidas */
    // Añade aquí nuevos campos para sensores personalizados
    // float custom_field1;
    // int custom_field2;
} sensor_data_t;

/**
 * @brief Estructura para configuración del payload LoRaWAN
 * @note MODIFICA esta estructura si necesitas cambiar el formato del payload
 */
typedef struct {
    uint8_t* buffer;      /**< Buffer donde escribir el payload */
    uint8_t max_size;     /**< Tamaño máximo del buffer */
    uint8_t written;      /**< Bytes realmente escritos */
    // Añade aquí campos adicionales para configuración de payload
    // bool include_timestamp;
    // uint8_t compression_level;
} payload_config_t;

#endif // CONFIG_H