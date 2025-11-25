#include "../../config/config.h"

#ifdef ENABLE_SENSOR_DHT11
// Implementación DHT11 (similar a DHT22 pero menos preciso)
#include <DHT.h>
#include "sensor_interface.h"
#include "LoRaBoards.h"

// Objeto global del sensor DHT
static DHT dht(DHT_PIN, DHT11);

// Estado del sensor
static bool sensor_available = false;

/**
 * @brief Controla la alimentación del sensor DHT11
 * @param power_on true para encender, false para apagar
 */
static void dht_power_control(bool power_on) {
    if (power_on) {
        pinMode(DHT_POWER_PIN, OUTPUT);
        digitalWrite(DHT_POWER_PIN, HIGH);
        Serial.println("DHT11: Alimentación ON");
    } else {
        digitalWrite(DHT_POWER_PIN, LOW);
        pinMode(DHT_POWER_PIN, INPUT);
        Serial.println("DHT11: Alimentación OFF");
    }
}

/**
 * @brief Inicializa el sensor DHT11
 */
bool sensor_dht11_init(void) {
    pinMode(DHT_POWER_PIN, OUTPUT);
    digitalWrite(DHT_POWER_PIN, LOW);
    dht.begin();
    Serial.println("Sensor DHT11 inicializado.");
    sensor_available = true;
    return true;
}

/**
 * @brief Verifica si el sensor está disponible
 */
bool sensor_dht11_is_available(void) {
    return sensor_available;
}

/**
 * @brief Intenta reinicializar el sensor
 */
bool sensor_dht11_retry_init(void) {
    if (sensor_available) return true;
    Serial.println("Reintentando inicialización del sensor DHT11...");
    return sensor_dht11_init();
}

/**
 * @brief Lee todos los datos del sensor DHT11
 */
bool sensor_dht11_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    dht_power_control(true);
    delay(DHT_POWER_ON_DELAY_MS);

    data->temperature = dht.readTemperature();
    data->humidity = dht.readHumidity();
    data->pressure = SENSOR_ERROR_PRESSURE;
    data->distance = SENSOR_ERROR_DISTANCE;
    data->battery = readBatteryVoltage();
    data->valid = true;

    dht_power_control(false);

    if (isnan(data->temperature) || isnan(data->humidity)) {
        Serial.println("DHT11: Error en lectura");
        data->temperature = SENSOR_ERROR_TEMPERATURE;
        data->humidity = SENSOR_ERROR_HUMIDITY;
        data->valid = false;
        return false;
    }

    Serial.printf("DHT11: Lectura exitosa - Temp: %.1f°C, Hum: %.1f%%\n",
                  data->temperature, data->humidity);
    return true;
}

/**
 * @brief Obtiene el payload empaquetado para DHT11
 */
uint8_t sensor_dht11_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) return 0;

    sensor_data_t data;
    bool sensor_ok = sensor_dht11_read_all(&data);

    if (!sensor_ok) {
        data.temperature = SENSOR_ERROR_TEMPERATURE;
        data.humidity = SENSOR_ERROR_HUMIDITY;
        sensor_dht11_retry_init();
    }

    uint8_t offset = 0;
    int16_t temp_int = (int16_t)(data.temperature * 100);
    config->buffer[offset++] = temp_int >> 8;
    config->buffer[offset++] = temp_int & 0xFF;

    uint16_t hum_int = (uint16_t)(data.humidity * 100);
    config->buffer[offset++] = hum_int >> 8;
    config->buffer[offset++] = hum_int & 0xFF;

    // Presión (no disponible)
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

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
const char* sensor_dht11_get_name(void) {
    return "DHT11";
}

/**
 * @brief Fuerza el estado del sensor para testing
 */
void sensor_dht11_set_available_for_testing(bool available) {
    sensor_available = available;
    Serial.printf("TESTING: Sensor DHT11 forzado a %s\n", available ? "disponible" : "no disponible");
}

#endif // ENABLE_SENSOR_DHT11
