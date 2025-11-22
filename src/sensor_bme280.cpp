/**
 * @file      sensor_bme280.cpp
 * @brief     Implementación del sensor BME280
 *
 * Implementación específica para el sensor Bosch BME280
 * que mide temperatura, humedad y presión.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#include <Adafruit_BME280.h>
#include "sensor_interface.h"
#include "sensor_config.h"
#include "LoRaBoards.h"

// Objeto global del sensor BME280
static Adafruit_BME280 bme;

// Estado del sensor
static bool sensor_available = false;

/**
 * @brief Inicializa el sensor BME280
 */
bool sensor_init(void) {
    if (!bme.begin(SENSOR_I2C_ADDRESS)) {
        Serial.println("No se pudo encontrar un sensor BME280 válido, verificar cableado!");
        sensor_available = false;
        return false;
    }
    Serial.println("Sensor BME280 inicializado.");
    sensor_available = true;
    return true;
}

/**
 * @brief Verifica si el sensor está disponible
 */
bool sensor_is_available(void) {
    return sensor_available;
}

/**
 * @brief Intenta reinicializar el sensor
 */
bool sensor_retry_init(void) {
    if (sensor_available) {
        return true; // Ya está inicializado
    }

    Serial.println("Reintentando inicialización del sensor BME280...");
    return sensor_init();
}

/**
 * @brief Lee todos los datos del sensor BME280
 */
bool sensor_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) {
        return false;
    }

    // Leer datos del sensor
    data->temperature = bme.readTemperature();
    data->humidity = bme.readHumidity();
    data->pressure = bme.readPressure() / 100.0F;  // Convertir Pa a hPa
    data->battery = readBatteryVoltage();
    data->valid = true;

    // Verificar si las lecturas son válidas
    if (data->temperature == SENSOR_ERROR_TEMPERATURE ||
        data->humidity == SENSOR_ERROR_HUMIDITY ||
        data->pressure == SENSOR_ERROR_PRESSURE) {
        data->valid = false;
        return false;
    }

    return true;
}

/**
 * @brief Obtiene el payload empaquetado para BME280
 */
uint8_t sensor_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) {
        return 0; // Buffer demasiado pequeño
    }

    sensor_data_t data;
    bool sensor_ok = sensor_read_all(&data);

    if (!sensor_ok) {
        // Sensor no disponible - enviar datos de error
        data.temperature = SENSOR_ERROR_TEMPERATURE;
        data.humidity = SENSOR_ERROR_HUMIDITY;
        data.pressure = SENSOR_ERROR_PRESSURE;
        Serial.println("Enviando datos de error del sensor BME280");

        // Intentar reinicializar el sensor para el próximo ciclo
        sensor_retry_init();
    }

    // Empaquetado de datos (big-endian)
    uint8_t offset = 0;

    // Temperatura (si disponible)
    if (SENSOR_HAS_TEMPERATURE) {
        int16_t temp_int = (int16_t)(data.temperature * 100);
        config->buffer[offset++] = temp_int >> 8;
        config->buffer[offset++] = temp_int & 0xFF;
    }

    // Humedad (si disponible)
    if (SENSOR_HAS_HUMIDITY) {
        uint16_t hum_int = (uint16_t)(data.humidity * 100);
        config->buffer[offset++] = hum_int >> 8;
        config->buffer[offset++] = hum_int & 0xFF;
    }

    // Presión (si disponible)
    if (SENSOR_HAS_PRESSURE) {
        uint16_t pres_int = (uint16_t)(data.pressure * 100);
        config->buffer[offset++] = pres_int >> 8;
        config->buffer[offset++] = pres_int & 0xFF;
    }

    // Batería (siempre)
    uint16_t batt_int = (uint16_t)(data.battery * 100);
    config->buffer[offset++] = batt_int >> 8;
    config->buffer[offset++] = batt_int & 0xFF;

    config->written = offset;
    return offset;
}

/**
 * @brief Obtiene el nombre del sensor
 */
const char* sensor_get_name(void) {
    return SENSOR_NAME;
}

/**
 * @brief Fuerza el estado del sensor para testing
 */
void sensor_set_available_for_testing(bool available) {
    sensor_available = available;
    if (available) {
        Serial.println("TESTING: Sensor BME280 forzado a disponible");
    } else {
        Serial.println("TESTING: Sensor BME280 forzado a no disponible");
    }
}