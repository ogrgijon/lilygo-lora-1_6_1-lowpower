#include "../../config/config.h"

#ifdef ENABLE_SENSOR_DS18B20
// Implementación DS18B20 (temperatura)
#include <OneWire.h>
#include <DallasTemperature.h>
#include "sensor_interface.h"
#include "LoRaBoards.h"

// Objeto global del sensor
static OneWire oneWire(ONE_WIRE_BUS);
static DallasTemperature sensors(&oneWire);

// Estado del sensor
static bool sensor_available = false;

/**
 * @brief Inicializa el sensor DS18B20
 */
bool sensor_ds18b20_init(void) {
    sensors.begin();
    Serial.println("Sensor DS18B20 inicializado.");
    sensor_available = true;
    return true;
}

/**
 * @brief Verifica si el sensor está disponible
 */
bool sensor_ds18b20_is_available(void) {
    return sensor_available;
}

/**
 * @brief Intenta reinicializar el sensor
 */
bool sensor_ds18b20_retry_init(void) {
    if (sensor_available) return true;
    Serial.println("Reintentando inicialización del sensor DS18B20...");
    return sensor_ds18b20_init();
}

/**
 * @brief Lee todos los datos del sensor DS18B20
 */
bool sensor_ds18b20_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    sensors.requestTemperatures();
    data->temperature = sensors.getTempCByIndex(0);
    data->humidity = SENSOR_ERROR_HUMIDITY;  // No mide humedad
    data->pressure = SENSOR_ERROR_PRESSURE;  // No mide presión
    data->distance = SENSOR_ERROR_DISTANCE;  // No mide distancia
    data->battery = readBatteryVoltage();
    data->valid = true;

    if (data->temperature == DEVICE_DISCONNECTED_C) {
        Serial.println("DS18B20: Error en lectura");
        data->temperature = SENSOR_ERROR_TEMPERATURE;
        data->valid = false;
        return false;
    }

    Serial.printf("DS18B20: Lectura exitosa - Temp: %.1f°C\n", data->temperature);
    return true;
}

/**
 * @brief Obtiene el payload empaquetado para DS18B20
 */
uint8_t sensor_ds18b20_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) return 0;

    sensor_data_t data;
    bool sensor_ok = sensor_ds18b20_read_all(&data);

    if (!sensor_ok) {
        data.temperature = SENSOR_ERROR_TEMPERATURE;
        sensor_ds18b20_retry_init();
    }

    uint8_t offset = 0;
    int16_t temp_int = (int16_t)(data.temperature * 100);
    config->buffer[offset++] = temp_int >> 8;
    config->buffer[offset++] = temp_int & 0xFF;

    // Humedad no disponible
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Presión no disponible
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Distancia no disponible
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    uint16_t batt_int = (uint16_t)(data.battery * 100);
    config->buffer[offset++] = batt_int >> 8;
    config->buffer[offset++] = batt_int & 0xFF;

    config->written = offset;
    return offset;
}

/**
 * @brief Obtiene el nombre del sensor
 */
const char* sensor_ds18b20_get_name(void) {
    return "DS18B20";
}

/**
 * @brief Fuerza el estado del sensor para testing
 */
void sensor_ds18b20_set_available_for_testing(bool available) {
    sensor_available = available;
    Serial.printf("TESTING: Sensor DS18B20 forzado a %s\n", available ? "disponible" : "no disponible");
}

#endif // ENABLE_SENSOR_DS18B20
