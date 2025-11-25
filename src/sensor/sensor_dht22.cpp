#include "../../config/config.h"

#ifdef ENABLE_SENSOR_DHT22
// Implementación DHT22
#include <DHT.h>
#include "sensor_interface.h"
#include "LoRaBoards.h"

// Objeto global del sensor DHT
static DHT dht(DHT_PIN, DHT_TYPE);

// Estado del sensor
static bool sensor_available = false;

/**
 * @brief Controla la alimentación del sensor DHT22
 * @param power_on true para encender, false para apagar
 */
static void dht_power_control(bool power_on) {
    if (power_on) {
        // Encender sensor
        pinMode(DHT_POWER_PIN, OUTPUT);
        digitalWrite(DHT_POWER_PIN, HIGH);
        Serial.println("DHT22: Alimentación ON");
    } else {
        // Apagar sensor
        digitalWrite(DHT_POWER_PIN, LOW);
        pinMode(DHT_POWER_PIN, INPUT);  // Alta impedancia para minimizar consumo
        Serial.println("DHT22: Alimentación OFF");
    }
}

/**
 * @brief Inicializa el sensor DHT22
 */
bool sensor_dht22_init(void) {
    // Configurar pin de alimentación
    pinMode(DHT_POWER_PIN, OUTPUT);
    digitalWrite(DHT_POWER_PIN, LOW);  // Empezar apagado

    // Inicializar objeto DHT (no requiere alimentación aún)
    dht.begin();

    Serial.println("Sensor DHT22 inicializado (alimentación controlada).");
    sensor_available = true;
    return true;
}

/**
 * @brief Verifica si el sensor está disponible
 */
bool sensor_dht22_is_available(void) {
    return sensor_available;
}

/**
 * @brief Intenta reinicializar el sensor
 */
bool sensor_dht22_retry_init(void) {
    if (sensor_available) {
        return true; // Ya está inicializado
    }

    Serial.println("Reintentando inicialización del sensor DHT22...");
    return sensor_dht22_init();
}

/**
 * @brief Lee todos los datos del sensor DHT22 con gestión de energía
 */
bool sensor_dht22_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) {
        return false;
    }

    // Encender el sensor
    dht_power_control(true);

    // Esperar a que se estabilice
    Serial.printf("DHT22: Esperando %d ms para estabilización...\n", DHT_POWER_ON_DELAY_MS);
    delay(DHT_POWER_ON_DELAY_MS);

    // Leer datos del sensor
    data->temperature = dht.readTemperature();
    data->humidity = dht.readHumidity();
    data->pressure = SENSOR_ERROR_PRESSURE;  // DHT22 no mide presión
    data->distance = SENSOR_ERROR_DISTANCE;  // DHT22 no mide distancia
    data->battery = readBatteryVoltage();
    data->valid = true;

    // Apagar el sensor inmediatamente después de leer
    dht_power_control(false);

    // Verificar si las lecturas son válidas
    if (isnan(data->temperature) || isnan(data->humidity)) {
        Serial.println("DHT22: Error en lectura - valores NaN");
        data->temperature = SENSOR_ERROR_TEMPERATURE;
        data->humidity = SENSOR_ERROR_HUMIDITY;
        data->valid = false;
        return false;
    }

    Serial.printf("DHT22: Lectura exitosa - Temp: %.1f°C, Hum: %.1f%%\n",
                  data->temperature, data->humidity);
    return true;
}

/**
 * @brief Obtiene el payload empaquetado para DHT22
 */
uint8_t sensor_dht22_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) {
        return 0; // Buffer demasiado pequeño
    }

    sensor_data_t data;
    bool sensor_ok = sensor_dht22_read_all(&data);

    if (!sensor_ok) {
        // Sensor no disponible - enviar datos de error
        data.temperature = SENSOR_ERROR_TEMPERATURE;
        data.humidity = SENSOR_ERROR_HUMIDITY;
        Serial.println("Enviando datos de error del sensor DHT22");

        // Intentar reinicializar el sensor para el próximo ciclo
        sensor_dht22_retry_init();
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

    // Presión (no disponible en DHT22)
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Distancia (no disponible en DHT22)
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

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
const char* sensor_dht22_get_name(void) {
    return SENSOR_NAME;
}

/**
 * @brief Fuerza el estado del sensor para testing
 */
void sensor_dht22_set_available_for_testing(bool available) {
    sensor_available = available;
    if (available) {
        Serial.println("TESTING: Sensor DHT22 forzado a disponible");
    } else {
        Serial.println("TESTING: Sensor DHT22 forzado a no disponible");
    }
}

#endif // ENABLE_SENSOR_DHT22