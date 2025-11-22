/**
 * @file      sensor_config.h
 * @brief     Configuración de sensores para diferentes configuraciones
 *
 * Este archivo define las configuraciones de sensores disponibles
 * y permite seleccionar cuál usar en tiempo de compilación.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

// ============================================================================
// CONFIGURACIÓN DE SENSOR - MODIFICAR ESTA SECCIÓN PARA CAMBIAR DE SENSOR
// ============================================================================

/**
 * Selección del tipo de sensor a usar
 * Descomentar solo UNA de las siguientes líneas:
 */
// #define USE_SENSOR_BME280        // Bosch BME280 (Temp, Hum, Pres)
#define USE_SENSOR_DHT22         // DHT22 (Temp, Hum)
// #define USE_SENSOR_SHT30         // Sensirion SHT30 (Temp, Hum)
// #define USE_SENSOR_DS18B20       // Dallas DS18B20 (Temp only)
// #define USE_SENSOR_MOCK          // Sensor simulado para testing

// Por defecto usar BME280 si no se especifica ninguno
#if !defined(USE_SENSOR_BME280) && !defined(USE_SENSOR_DHT22) && \
    !defined(USE_SENSOR_SHT30) && !defined(USE_SENSOR_DS18B20) && \
    !defined(USE_SENSOR_MOCK)
#define USE_SENSOR_BME280
#endif

// ============================================================================
// CONFIGURACIONES ESPECÍFICAS POR SENSOR
// ============================================================================

// Configuración BME280
#ifdef USE_SENSOR_BME280
#define SENSOR_NAME "BME280"
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY true
#define SENSOR_HAS_PRESSURE true
#define SENSOR_I2C_ADDRESS 0x76
#endif

// Configuración DHT22
#ifdef USE_SENSOR_DHT22
#define SENSOR_NAME "DHT22"
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY true
#define SENSOR_HAS_PRESSURE false
#define DHT_PIN 13  // Pin GPIO para datos DHT22
#define DHT_TYPE DHT22
#define DHT_POWER_PIN 12  // Pin GPIO para controlar alimentación del DHT22
#define DHT_POWER_ON_DELAY_MS 2000  // Tiempo de estabilización después de encender (ms)
#endif

// Configuración SHT30
#ifdef USE_SENSOR_SHT30
#define SENSOR_NAME "SHT30"
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY true
#define SENSOR_HAS_PRESSURE false
#define SHT30_I2C_ADDRESS 0x44
#endif

// Configuración DS18B20
#ifdef USE_SENSOR_DS18B20
#define SENSOR_NAME "DS18B20"
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY false
#define SENSOR_HAS_PRESSURE false
#define ONE_WIRE_BUS 4  // Pin GPIO para OneWire
#endif

// Configuración Mock (para testing)
#ifdef USE_SENSOR_MOCK
#define SENSOR_NAME "MOCK"
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY true
#define SENSOR_HAS_PRESSURE true
#endif

// ============================================================================
// CONFIGURACIÓN DE PAYLOAD
// ============================================================================

/**
 * Tamaño del payload en bytes basado en los sensores disponibles
 * Cada sensor contribuye con 2 bytes (valor escalado)
 */
#define PAYLOAD_SIZE_BYTES ( \
    (SENSOR_HAS_TEMPERATURE ? 2 : 0) + \
    (SENSOR_HAS_HUMIDITY ? 2 : 0) + \
    (SENSOR_HAS_PRESSURE ? 2 : 0) + \
    2 /* batería siempre */ \
)

// ============================================================================
// VALORES DE ERROR
// ============================================================================

#define SENSOR_ERROR_TEMPERATURE -999.0f
#define SENSOR_ERROR_HUMIDITY -1.0f
#define SENSOR_ERROR_PRESSURE -1.0f

#endif // SENSOR_CONFIG_H