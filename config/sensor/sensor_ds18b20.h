#ifndef SENSOR_CONFIG_DS18B20_H
#define SENSOR_CONFIG_DS18B20_H

// Nombre del sensor
#define SENSOR_NAME "DS18B20"

// ¿Qué mide?
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY false
#define SENSOR_HAS_PRESSURE false
#define SENSOR_HAS_DISTANCE false

// Configuración hardware
#define ONE_WIRE_BUS 13
#define DS18B20_RESOLUTION 12
#define DS18B20_POWER_PIN 12
#define DS18B20_POWER_ON_DELAY_MS 100

// Rangos válidos
#define TEMPERATURE_MIN -55.0f
#define TEMPERATURE_MAX 125.0f

// Configuración de lecturas
#define DS18B20_READ_ATTEMPTS 3
#define DS18B20_READ_DELAY_MS 100
#define DS18B20_CONVERSION_TIMEOUT_MS 1000

#endif // SENSOR_CONFIG_DS18B20_H