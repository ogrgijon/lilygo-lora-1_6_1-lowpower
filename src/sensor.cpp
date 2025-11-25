/**
 * @file      sensor.cpp
 * @brief     Implementación modular de sensores
 *
 * Este archivo implementa el sistema modular de sensores,
 * permitiendo cambiar entre diferentes sensores mediante configuración.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#include "../config/config.h"  // Configuración unificada del proyecto
#include "sensor_interface.h"  // Interfaz genérica de sensores
#include "LoRaBoards.h"  // Para readBatteryVoltage

// Declaración externa para funciones de carga solar
extern bool isSolarChargingBattery();

// ============================================================================
// SISTEMA MULTISENSOR - FUNCIONES PARA GESTIONAR MÚLTIPLES SENSORES
// ============================================================================

/**
 * @brief Inicializa todos los sensores habilitados
 * @return true si al menos un sensor se inicializó correctamente
 */
bool sensors_init_all(void) {
    bool any_success = false;

#ifdef ENABLE_SENSOR_DHT22
    if (sensor_dht22_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_DHT11
    if (sensor_dht11_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_DS18B20
    if (sensor_ds18b20_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_BMP280
    if (sensor_bmp280_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_HCSR04
    if (sensor_hcsr04_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_NONE
    if (sensor_none_init()) any_success = true;
#endif

    return any_success;
}

/**
 * @brief Verifica si al menos un sensor está disponible
 * @return true si algún sensor está operativo
 */
bool sensors_is_any_available(void) {
    bool any_available = false;

#ifdef ENABLE_SENSOR_DHT22
    if (sensor_dht22_is_available()) any_available = true;
#endif
#ifdef ENABLE_SENSOR_DHT11
    if (sensor_dht11_is_available()) any_available = true;
#endif
#ifdef ENABLE_SENSOR_DS18B20
    if (sensor_ds18b20_is_available()) any_available = true;
#endif
#ifdef ENABLE_SENSOR_BMP280
    if (sensor_bmp280_is_available()) any_available = true;
#endif
#ifdef ENABLE_SENSOR_HCSR04
    if (sensor_hcsr04_is_available()) any_available = true;
#endif
#ifdef ENABLE_SENSOR_NONE
    if (sensor_none_is_available()) any_available = true;
#endif

    return any_available;
}

/**
 * @brief Intenta reinicializar todos los sensores
 * @return true si al menos un sensor se reinicializó correctamente
 */
bool sensors_retry_init_all(void) {
    bool any_success = false;

#ifdef ENABLE_SENSOR_DHT22
    if (sensor_dht22_retry_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_DHT11
    if (sensor_dht11_retry_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_DS18B20
    if (sensor_ds18b20_retry_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_BMP280
    if (sensor_bmp280_retry_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_HCSR04
    if (sensor_hcsr04_retry_init()) any_success = true;
#endif
#ifdef ENABLE_SENSOR_NONE
    if (sensor_none_retry_init()) any_success = true;
#endif

    return any_success;
}

/**
 * @brief Lee datos de todos los sensores habilitados y los combina
 * @param data Puntero a estructura donde almacenar los datos combinados
 * @return true si se pudieron leer datos de al menos un sensor
 */
bool sensors_read_all(sensor_data_t* data) {
    if (!data) return false;

    // Inicializar con valores de error
    data->temperature = SENSOR_ERROR_TEMPERATURE;
    data->humidity = SENSOR_ERROR_HUMIDITY;
    data->pressure = SENSOR_ERROR_PRESSURE;
    data->distance = SENSOR_ERROR_DISTANCE;
    data->battery = readBatteryVoltage();
    data->valid = false;

    bool any_data = false;

    // Leer de cada sensor habilitado y combinar datos
#ifdef ENABLE_SENSOR_DHT22
    {
        sensor_data_t dht_data;
        if (sensor_dht22_read_all(&dht_data)) {
            if (dht_data.temperature != SENSOR_ERROR_TEMPERATURE) {
                data->temperature = dht_data.temperature;
                any_data = true;
            }
            if (dht_data.humidity != SENSOR_ERROR_HUMIDITY) {
                data->humidity = dht_data.humidity;
                any_data = true;
            }
            // Si la batería leída por DHT22 es válida, usarla
            if (dht_data.battery > 2.5f && dht_data.battery < 4.5f) {
                data->battery = dht_data.battery;
            }
        }
    }
#endif
#ifdef ENABLE_SENSOR_DHT11
    {
        sensor_data_t dht_data;
        if (sensor_dht11_read_all(&dht_data)) {
            if (dht_data.temperature != SENSOR_ERROR_TEMPERATURE) {
                data->temperature = dht_data.temperature;
                any_data = true;
            }
            if (dht_data.humidity != SENSOR_ERROR_HUMIDITY) {
                data->humidity = dht_data.humidity;
                any_data = true;
            }
        }
    }
#endif
#ifdef ENABLE_SENSOR_DS18B20
    {
        sensor_data_t ds_data;
        if (sensor_ds18b20_read_all(&ds_data)) {
            if (ds_data.temperature != SENSOR_ERROR_TEMPERATURE) {
                data->temperature = ds_data.temperature;
                any_data = true;
            }
        }
    }
#endif
#ifdef ENABLE_SENSOR_BMP280
    {
        sensor_data_t bmp_data;
        if (sensor_bmp280_read_all(&bmp_data)) {
            if (bmp_data.temperature != SENSOR_ERROR_TEMPERATURE) {
                data->temperature = bmp_data.temperature;
                any_data = true;
            }
            if (bmp_data.pressure != SENSOR_ERROR_PRESSURE) {
                data->pressure = bmp_data.pressure;
                any_data = true;
            }
        }
    }
#endif
#ifdef ENABLE_SENSOR_HCSR04
    {
        sensor_data_t hcsr_data;
        if (sensor_hcsr04_read_all(&hcsr_data)) {
            if (hcsr_data.distance != SENSOR_ERROR_DISTANCE) {
                data->distance = hcsr_data.distance;
                any_data = true;
            }
        }
    }
#endif
#ifdef ENABLE_SENSOR_NONE
    {
        sensor_data_t none_data;
        if (sensor_none_read_all(&none_data)) {
            any_data = true;
        }
    }
#endif

    data->valid = any_data;
    return any_data;
}

/**
 * @brief Construye el payload con datos de todos los sensores
 * @param config Configuración del payload
 * @return Número de bytes escritos
 */
uint8_t sensors_get_payload(payload_config_t* config) {
    if (!config || config->max_size < PAYLOAD_SIZE_BYTES) return 0;

    sensor_data_t data;
    bool sensor_ok = sensors_read_all(&data);

    if (!sensor_ok) {
        // Si no hay datos válidos, intentar reinicializar
        sensors_retry_init_all();
        // Usar datos de error
        data.temperature = SENSOR_ERROR_TEMPERATURE;
        data.humidity = SENSOR_ERROR_HUMIDITY;
        data.pressure = SENSOR_ERROR_PRESSURE;
        data.distance = SENSOR_ERROR_DISTANCE;
    }

    uint8_t offset = 0;

    // Temperatura (si está disponible en el sistema)
    if (SYSTEM_HAS_TEMPERATURE) {
        int16_t temp_int = (int16_t)(data.temperature * 100);
        config->buffer[offset++] = temp_int >> 8;
        config->buffer[offset++] = temp_int & 0xFF;
    }

    // Humedad (si está disponible en el sistema)
    if (SYSTEM_HAS_HUMIDITY) {
        int16_t hum_int = (int16_t)(data.humidity * 100);
        config->buffer[offset++] = hum_int >> 8;
        config->buffer[offset++] = hum_int & 0xFF;
    }

    // Presión (si está disponible en el sistema)
    if (SYSTEM_HAS_PRESSURE) {
        uint16_t pres_int = (uint16_t)(data.pressure * 10);
        config->buffer[offset++] = pres_int >> 8;
        config->buffer[offset++] = pres_int & 0xFF;
    }

    // Distancia (si está disponible en el sistema)
    if (SYSTEM_HAS_DISTANCE) {
        uint16_t dist_int = (uint16_t)(data.distance * 100);
        config->buffer[offset++] = dist_int >> 8;
        config->buffer[offset++] = dist_int & 0xFF;
    }

    // Batería (siempre incluida)
        uint16_t batt_int = (uint16_t)(data.battery * 100);
        Serial.printf("DEBUG: Packing battery voltage %.2f V as %u (0x%04X)\n", data.battery, batt_int, batt_int);
        config->buffer[offset++] = batt_int >> 8;
        config->buffer[offset++] = batt_int & 0xFF;

    // Estado solar (siempre incluido)
    config->buffer[offset++] = isSolarChargingBattery() ? 1 : 0;

    config->written = offset;
    return offset;
}

/**
 * @brief Obtiene el nombre de los sensores activos
 * @return Cadena con los nombres de sensores separados por coma
 */
const char* sensors_get_name(void) {
    static char name_buffer[64];
    name_buffer[0] = '\0';

#ifdef ENABLE_SENSOR_DHT22
    strcat(name_buffer, "DHT22 ");
#endif
#ifdef ENABLE_SENSOR_DHT11
    strcat(name_buffer, "DHT11 ");
#endif
#ifdef ENABLE_SENSOR_DS18B20
    strcat(name_buffer, "DS18B20 ");
#endif
#ifdef ENABLE_SENSOR_BMP280
    strcat(name_buffer, "BMP280 ");
#endif
#ifdef ENABLE_SENSOR_HCSR04
    strcat(name_buffer, "HC-SR04 ");
#endif
#ifdef ENABLE_SENSOR_NONE
    strcat(name_buffer, "NONE ");
#endif

    // Remover espacio final si existe
    size_t len = strlen(name_buffer);
    if (len > 0 && name_buffer[len-1] == ' ') {
        name_buffer[len-1] = '\0';
    }

    return name_buffer;
}

/**
 * @brief Fuerza el estado de todos los sensores para testing
 * @param available true para simular disponibles, false para simular fallos
 */
void sensors_set_available_for_testing(bool available) {
#ifdef ENABLE_SENSOR_DHT22
    sensor_dht22_set_available_for_testing(available);
#endif
#ifdef ENABLE_SENSOR_DHT11
    sensor_dht11_set_available_for_testing(available);
#endif
#ifdef ENABLE_SENSOR_DS18B20
    sensor_ds18b20_set_available_for_testing(available);
#endif
#ifdef ENABLE_SENSOR_BMP280
    sensor_bmp280_set_available_for_testing(available);
#endif
#ifdef ENABLE_SENSOR_HCSR04
    sensor_hcsr04_set_available_for_testing(available);
#endif
#ifdef ENABLE_SENSOR_NONE
    sensor_none_set_available_for_testing(available);
#endif
}

// ============================================================================
// FUNCIONES DE COMPATIBILIDAD HACIA ATRÁS
// ============================================================================

/**
 * Funciones legacy para mantener compatibilidad con el código existente
 * Estas funciones llaman a la nueva interfaz multisensor
 */

bool initSensor() {
    return sensors_init_all();
}

bool isSensorAvailable() {
    return sensors_is_any_available();
}

bool retrySensorInit() {
    return sensors_retry_init_all();
}

float readTemperature() {
    sensor_data_t data;
    if (sensors_read_all(&data) && data.valid) {
        return data.temperature;
    }
    return SENSOR_ERROR_TEMPERATURE;
}

float readHumidity() {
    sensor_data_t data;
    if (sensors_read_all(&data) && data.valid) {
        return data.humidity;
    }
    return SENSOR_ERROR_HUMIDITY;
}

float readPressure() {
    sensor_data_t data;
    if (sensors_read_all(&data) && data.valid) {
        return data.pressure;
    }
    return SENSOR_ERROR_PRESSURE;
}

void setSensorAvailableForTesting(bool available) {
    sensors_set_available_for_testing(available);
}

uint8_t getSensorPayload(uint8_t* payload, uint8_t maxSize) {
    payload_config_t config = {payload, maxSize, 0};
    return sensors_get_payload(&config);
}

bool getSensorDataForDisplay(float& temp, float& hum, float& pres, float& battery) {
    sensor_data_t data;
    bool ok = sensors_read_all(&data);

    temp = data.temperature;
    hum = data.humidity;
    pres = data.pressure;
    battery = data.battery;

    return ok && data.valid;
}