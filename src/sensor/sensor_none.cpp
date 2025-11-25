#include "../../config/config.h"
#include "LoRaBoards.h"
#ifdef ENABLE_SENSOR_NONE
// Implementación sin sensor (solo batería)

/**
 * @brief Inicializa (nada que inicializar)
 */
bool sensor_none_init(void) {
    Serial.println("Sensor: Ninguno (solo batería)");
    return true;
}

/**
 * @brief Verifica si el sensor está disponible
 */
bool sensor_none_is_available(void) {
    return true;
}

/**
 * @brief Intenta reinicializar (siempre ok)
 */
bool sensor_none_retry_init(void) {
    return true;
}

/**
 * @brief Lee datos (solo batería)
 */
bool sensor_none_read_all(sensor_data_t* data) {
    if (!data) return false;

    data->temperature = SENSOR_ERROR_TEMPERATURE;
    data->humidity = SENSOR_ERROR_HUMIDITY;
    data->pressure = SENSOR_ERROR_PRESSURE;
    data->distance = SENSOR_ERROR_DISTANCE;
    data->battery = readBatteryVoltage();
    data->valid = true;

    Serial.printf("Sensor: Solo batería - %.2fV\n", data->battery);
    return true;
}

/**
 * @brief Obtiene el payload (solo batería)
 */
uint8_t sensor_none_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) return 0;

    sensor_data_t data;
    sensor_none_read_all(&data);

    uint8_t offset = 0;
    // Temperatura error
    config->buffer[offset++] = 0x80;
    config->buffer[offset++] = 0x00;

    // Humedad error
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Presión error
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Distancia error
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Batería
    uint16_t batt_int = (uint16_t)(data.battery * 100);
    config->buffer[offset++] = batt_int >> 8;
    config->buffer[offset++] = batt_int & 0xFF;

    config->written = offset;
    return offset;
}

/**
 * @brief Obtiene el nombre del sensor
 */
const char* sensor_none_get_name(void) {
    return "Ninguno";
}

/**
 * @brief Fuerza el estado (no aplica)
 */
void sensor_none_set_available_for_testing(bool available) {
    // No hace nada
}

#endif // ENABLE_SENSOR_NONE
