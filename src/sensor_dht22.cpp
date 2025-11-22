/**
 * @file      sensor_dht22.cpp
 * @brief     Implementación del sensor DHT22
 *
 * Implementación específica para el sensor DHT22 de Adafruit
 * que mide temperatura y humedad.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#include <DHT.h>
#include "sensor_interface.h"
#include "sensor_config.h"
#include "LoRaBoards.h"

// Objeto global del sensor DHT
static DHT dht(DHT_PIN, DHT_TYPE);

// Estado del sensor
static bool sensor_available = false;

/**
 * @brief Inicializa el sensor DHT22
 */
bool sensor_init(void) {
    dht.begin();
    Serial.println("Sensor DHT22 inicializado.");

    // Verificar que el sensor responde
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
        Serial.println("Error: DHT22 no responde correctamente");
        sensor_available = false;
        return false;
    }

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

    Serial.println("Reintentando inicialización del sensor DHT22...");
    return sensor_init();
}

/**
 * @brief Lee todos los datos del sensor DHT22
 */
bool sensor_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) {
        return false;
    }

    // Leer datos del sensor
    data->temperature = dht.readTemperature();
    data->humidity = dht.readHumidity();
    data->pressure = SENSOR_ERROR_PRESSURE;  // DHT22 no mide presión
    data->battery = readBatteryVoltage();
    data->valid = true;

    // Verificar si las lecturas son válidas
    if (isnan(data->temperature) || isnan(data->humidity)) {
        data->temperature = SENSOR_ERROR_TEMPERATURE;
        data->humidity = SENSOR_ERROR_HUMIDITY;
        data->valid = false;
        return false;
    }

    return true;
}

/**
 * @brief Obtiene el payload empaquetado para DHT22
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
        Serial.println("Enviando datos de error del sensor DHT22");

        // Intentar reinicializar el sensor para el próximo ciclo
        sensor_retry_init();
    }

    // Empaquetado de datos (big-endian)
    uint8_t offset = 0;

    // Temperatura (siempre disponible en DHT22)
    int16_t temp_int = (int16_t)(data.temperature * 100);
    config->buffer[offset++] = temp_int >> 8;
    config->buffer[offset++] = temp_int & 0xFF;

    // Humedad (siempre disponible en DHT22)
    uint16_t hum_int = (uint16_t)(data.humidity * 100);
    config->buffer[offset++] = hum_int >> 8;
    config->buffer[offset++] = hum_int & 0xFF;

    // Presión (no disponible en DHT22 - se omite)

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
        Serial.println("TESTING: Sensor DHT22 forzado a disponible");
    } else {
        Serial.println("TESTING: Sensor DHT22 forzado a no disponible");
    }
}