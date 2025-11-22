/**
 * @file      sensor_dispatcher.cpp
 * @brief     Dispatcher para seleccionar implementación de sensor
 *
 * Este archivo incluye la implementación correcta del sensor
 * basada en la configuración definida en sensor_config.h
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#include "sensor_config.h"

// ============================================================================
// INCLUSIÓN CONDICIONAL DE IMPLEMENTACIONES
// ============================================================================

// Incluir la implementación correspondiente al sensor seleccionado
#ifdef USE_SENSOR_BME280
#include "sensor_bme280.cpp"
#endif

#ifdef USE_SENSOR_DHT22
#include "sensor_dht22.cpp"
#endif

#ifdef USE_SENSOR_SHT30
// TODO: Implementar sensor_sht30.cpp
#error "Sensor SHT30 no implementado aún"
#endif

#ifdef USE_SENSOR_DS18B20
// TODO: Implementar sensor_ds18b20.cpp
#error "Sensor DS18B20 no implementado aún"
#endif

#ifdef USE_SENSOR_MOCK
#include "sensor_mock.cpp"
#endif

// ============================================================================
// FUNCIONES DE COMPATIBILIDAD HACIA ATRÁS
// ============================================================================

/**
 * Funciones legacy para mantener compatibilidad con el código existente
 * Estas funciones llaman a la nueva interfaz genérica
 */

#include "sensor.h"  // Para las declaraciones legacy

bool initSensor() {
    return sensor_init();
}

bool isSensorAvailable() {
    return sensor_is_available();
}

bool retrySensorInit() {
    return sensor_retry_init();
}

float readTemperature() {
    sensor_data_t data;
    if (sensor_read_all(&data) && data.valid) {
        return data.temperature;
    }
    return SENSOR_ERROR_TEMPERATURE;
}

float readHumidity() {
    sensor_data_t data;
    if (sensor_read_all(&data) && data.valid) {
        return data.humidity;
    }
    return SENSOR_ERROR_HUMIDITY;
}

float readPressure() {
    sensor_data_t data;
    if (sensor_read_all(&data) && data.valid) {
        return data.pressure;
    }
    return SENSOR_ERROR_PRESSURE;
}

void setSensorAvailableForTesting(bool available) {
    sensor_set_available_for_testing(available);
}

uint8_t getSensorPayload(uint8_t* payload, uint8_t maxSize) {
    payload_config_t config = {payload, maxSize, 0};
    return sensor_get_payload(&config);
}

bool getSensorDataForDisplay(float& temp, float& hum, float& pres, float& battery) {
    sensor_data_t data;
    bool ok = sensor_read_all(&data);

    temp = data.temperature;
    hum = data.humidity;
    pres = data.pressure;
    battery = data.battery;

    return ok && data.valid;
}