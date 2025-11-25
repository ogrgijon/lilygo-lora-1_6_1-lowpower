#include "../../config/config.h"

#ifdef ENABLE_SENSOR_HCSR04
// Implementación HC-SR04 (distancia ultrasónica)
#include "sensor_interface.h"
#include "LoRaBoards.h"

// Estado del sensor
static bool sensor_available = false;

/**
 * @brief Inicializa el sensor HC-SR04
 */
bool sensor_hcsr04_init(void) {
    pinMode(HCSR04_TRIG_PIN, OUTPUT);
    pinMode(HCSR04_ECHO_PIN, INPUT);
    digitalWrite(HCSR04_TRIG_PIN, LOW);
    Serial.println("Sensor HC-SR04 inicializado.");
    sensor_available = true;
    return true;
}

/**
 * @brief Verifica si el sensor está disponible
 */
bool sensor_hcsr04_is_available(void) {
    return sensor_available;
}

/**
 * @brief Intenta reinicializar el sensor
 */
bool sensor_hcsr04_retry_init(void) {
    if (sensor_available) return true;
    Serial.println("Reintentando inicialización del sensor HC-SR04...");
    return sensor_hcsr04_init();
}

/**
 * @brief Mide la distancia usando HC-SR04
 */
static float measure_distance(void) {
    // Enviar pulso de trigger
    digitalWrite(HCSR04_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(HCSR04_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCSR04_TRIG_PIN, LOW);

    // Medir tiempo de echo
    unsigned long duration = pulseIn(HCSR04_ECHO_PIN, HIGH, HCSR04_TIMEOUT_US);

    if (duration == 0) {
        return SENSOR_ERROR_DISTANCE; // Timeout
    }

    // Calcular distancia: tiempo(us) * velocidad_sonido(cm/us) / 2
    // Velocidad sonido ~343 m/s = 0.0343 cm/us
    float distance = (duration * 0.0343) / 2.0;

    // Validar rango
    if (distance < DISTANCE_MIN || distance > DISTANCE_MAX) {
        return SENSOR_ERROR_DISTANCE;
    }

    return distance;
}

/**
 * @brief Lee todos los datos del sensor HC-SR04
 */
bool sensor_hcsr04_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    // Intentar lecturas múltiples para mayor precisión
    float distances[HCSR04_READ_ATTEMPTS];
    int valid_readings = 0;
    float sum = 0.0;

    for (int i = 0; i < HCSR04_READ_ATTEMPTS; i++) {
        float dist = measure_distance();
        if (dist > 0) {
            distances[valid_readings] = dist;
            sum += dist;
            valid_readings++;
        }
        if (i < HCSR04_READ_ATTEMPTS - 1) {
            delay(HCSR04_READ_DELAY_MS);
        }
    }

    if (valid_readings == 0) {
        Serial.println("HC-SR04: No se pudieron obtener lecturas válidas");
        data->distance = SENSOR_ERROR_DISTANCE;
        data->valid = false;
        return false;
    }

    // Promedio de lecturas válidas
    data->distance = sum / valid_readings;
    data->temperature = SENSOR_ERROR_TEMPERATURE;  // No mide temperatura
    data->humidity = SENSOR_ERROR_HUMIDITY;       // No mide humedad
    data->pressure = SENSOR_ERROR_PRESSURE;       // No mide presión
    data->battery = readBatteryVoltage();
    data->valid = true;

    Serial.printf("HC-SR04: Lectura exitosa - Dist: %.1f cm (promedio de %d lecturas)\n",
                  data->distance, valid_readings);
    return true;
}

/**
 * @brief Obtiene el payload empaquetado para HC-SR04
 */
uint8_t sensor_hcsr04_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) return 0;

    sensor_data_t data;
    bool sensor_ok = sensor_hcsr04_read_all(&data);

    if (!sensor_ok) {
        data.distance = SENSOR_ERROR_DISTANCE;
        sensor_hcsr04_retry_init();
    }

    uint8_t offset = 0;

    // Temperatura (error)
    config->buffer[offset++] = 0x80;
    config->buffer[offset++] = 0x00;

    // Humedad (error)
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Presión (error)
    config->buffer[offset++] = 0xFF;
    config->buffer[offset++] = 0xFF;

    // Distancia
    uint16_t dist_int = (uint16_t)(data.distance * 100);
    config->buffer[offset++] = dist_int >> 8;
    config->buffer[offset++] = dist_int & 0xFF;

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
const char* sensor_hcsr04_get_name(void) {
    return "HC-SR04";
}

/**
 * @brief Fuerza el estado del sensor para testing
 */
void sensor_hcsr04_set_available_for_testing(bool available) {
    sensor_available = available;
    Serial.printf("TESTING: Sensor HC-SR04 forzado a %s\n", available ? "disponible" : "no disponible");
}

#endif // ENABLE_SENSOR_HCSR04
