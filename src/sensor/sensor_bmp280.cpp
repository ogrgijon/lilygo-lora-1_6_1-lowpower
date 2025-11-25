#include "../../config/config.h"

#ifdef ENABLE_SENSOR_BMP280
// Implementación BMP280 (presión y temperatura)
#include <Adafruit_BMP280.h>
#include "sensor_interface.h"
#include "LoRaBoards.h"

// Objeto global del sensor
static Adafruit_BMP280 bmp;

// Estado del sensor
static bool sensor_available = false;

/**
 * @brief Inicializa el sensor BMP280
 */
bool sensor_bmp280_init(void) {
    if (!bmp.begin(BMP280_ADDRESS)) {
        Serial.println("BMP280: No encontrado");
        sensor_available = false;
        return false;
    }
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
    Serial.println("Sensor BMP280 inicializado.");
    sensor_available = true;
    return true;
}

/**
 * @brief Verifica si el sensor está disponible
 */
bool sensor_bmp280_is_available(void) {
    return sensor_available;
}

/**
 * @brief Intenta reinicializar el sensor
 */
bool sensor_bmp280_retry_init(void) {
    if (sensor_available) return true;
    Serial.println("Reintentando inicialización del sensor BMP280...");
    return sensor_bmp280_init();
}

/**
 * @brief Lee todos los datos del sensor BMP280
 */
bool sensor_bmp280_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    data->temperature = bmp.readTemperature();
    data->humidity = SENSOR_ERROR_HUMIDITY;  // No mide humedad
    data->pressure = bmp.readPressure() / 100.0F;  // Convertir a hPa
    data->distance = SENSOR_ERROR_DISTANCE;  // No mide distancia
    data->battery = readBatteryVoltage();
    data->valid = true;

    if (isnan(data->temperature) || isnan(data->pressure)) {
        Serial.println("BMP280: Error en lectura");
        data->temperature = SENSOR_ERROR_TEMPERATURE;
        data->pressure = SENSOR_ERROR_PRESSURE;
        data->valid = false;
        return false;
    }

    Serial.printf("BMP280: Lectura exitosa - Temp: %.1f°C, Pres: %.1f hPa\n",
                  data->temperature, data->pressure);
    return true;
}

/**
 * @brief Obtiene el payload empaquetado para BMP280
 */
uint8_t sensor_bmp280_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) return 0;

    sensor_data_t data;
    bool sensor_ok = sensor_bmp280_read_all(&data);

    if (!sensor_ok) {
        data.temperature = SENSOR_ERROR_TEMPERATURE;
        data.pressure = SENSOR_ERROR_PRESSURE;
        sensor_bmp280_retry_init();
    }

    uint8_t offset = 0;
    int16_t temp_int = (int16_t)(data.temperature * 100);
    config->buffer[offset++] = temp_int >> 8;
    config->buffer[offset++] = temp_int & 0xFF;

    // Humedad no disponible
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Presión
    uint16_t pres_int = (uint16_t)(data.pressure * 10);
    config->buffer[offset++] = pres_int >> 8;
    config->buffer[offset++] = pres_int & 0xFF;

    // Distancia (no disponible)
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
const char* sensor_bmp280_get_name(void) {
    return "BMP280";
}

/**
 * @brief Fuerza el estado del sensor para testing
 */
void sensor_bmp280_set_available_for_testing(bool available) {
    sensor_available = available;
    Serial.printf("TESTING: Sensor BMP280 forzado a %s\n", available ? "disponible" : "no disponible");
}

#endif // ENABLE_SENSOR_BMP280
