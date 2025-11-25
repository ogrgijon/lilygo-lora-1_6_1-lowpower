#ifndef SENSOR_CONFIG_DHT22_H
#define SENSOR_CONFIG_DHT22_H

// =============================================================================
// CONFIGURACIÓN DEL SENSOR DHT22
// =============================================================================
// Copia este archivo para crear un nuevo sensor y modifica los valores

// Nombre del sensor (para display y logs)
#define SENSOR_NAME "DHT22"

// ¿Qué mide este sensor? (true/false)
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY true
#define SENSOR_HAS_PRESSURE false
#define SENSOR_HAS_DISTANCE false

// Configuración específica del hardware DHT22
#define DHT_PIN 13
#define DHT_TYPE DHT22
#define DHT_POWER_PIN 12
#define DHT_POWER_ON_DELAY_MS 2000

// Rangos válidos para validación
#define TEMPERATURE_MIN -40.0f
#define TEMPERATURE_MAX 80.0f
#define HUMIDITY_MIN 0.0f
#define HUMIDITY_MAX 100.0f

// Configuración de lecturas
#define DHT_READ_ATTEMPTS 3
#define DHT_READ_DELAY_MS 100

#endif // SENSOR_CONFIG_DHT22_H
