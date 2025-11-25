#ifndef SENSOR_CONFIG_BMP280_H
#define SENSOR_CONFIG_BMP280_H

// Nombre del sensor
#define SENSOR_NAME "BMP280"

// ¿Qué mide?
#define SENSOR_HAS_TEMPERATURE true
#define SENSOR_HAS_HUMIDITY false
#define SENSOR_HAS_PRESSURE true
#define SENSOR_HAS_DISTANCE false

// Configuración hardware
#define BMP280_I2C_ADDR 0x76
#define BMP280_POWER_PIN 12
#define BMP280_POWER_ON_DELAY_MS 100

// Configuración del sensor
#define BMP280_SEA_LEVEL_PRESSURE 1013.25f
#define BMP280_MODE 3
#define BMP280_STANDBY 5
#define BMP280_FILTER 4
#define BMP280_SPI3W_EN 0

// Rangos válidos
#define TEMPERATURE_MIN -40.0f
#define TEMPERATURE_MAX 85.0f
#define PRESSURE_MIN 300.0f
#define PRESSURE_MAX 1100.0f

// Configuración de lecturas
#define BMP280_READ_ATTEMPTS 3
#define BMP280_READ_DELAY_MS 100

#endif // SENSOR_CONFIG_BMP280_H