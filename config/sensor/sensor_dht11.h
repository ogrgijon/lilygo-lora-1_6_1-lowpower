#ifndef SENSOR_CONFIG_DHT11_H
#define SENSOR_CONFIG_DHT11_H

// Nombre del sensor
#define SENSOR_NAME "DHT11"

// ¿Qué mide?
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY true
#define SENSOR_HAS_PRESSURE false
#define SENSOR_HAS_DISTANCE false

// Configuración hardware
#define DHT_PIN 13
#define DHT_TYPE DHT11
#define DHT_POWER_PIN 12
#define DHT_POWER_ON_DELAY_MS 2000

// Rangos válidos
#define TEMPERATURE_MIN 0.0f
#define TEMPERATURE_MAX 50.0f
#define HUMIDITY_MIN 20.0f
#define HUMIDITY_MAX 90.0f

// Configuración de lecturas
#define DHT_READ_ATTEMPTS 3
#define DHT_READ_DELAY_MS 100

#endif // SENSOR_CONFIG_DHT11_H