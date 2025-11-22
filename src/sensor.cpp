/**
 * @file      sensor.cpp
 * @brief     Implementación de funciones del sensor BME280 y batería
 *
 * Este archivo contiene las funciones para inicializar y leer
 * datos del sensor BME280 (temperatura, humedad, presión) y batería.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#include <Adafruit_BME280.h>
#include "sensor.h"
#include "LoRaBoards.h"

// Objeto global del sensor BME280
Adafruit_BME280 bme;

// Estado del sensor
static bool sensorAvailable = false;

/**
 * @brief Inicializa el sensor BME280
 *
 * @return true si la inicialización es exitosa, false en caso contrario
 */
bool initSensor() {
    if (!bme.begin(0x76)) {
        Serial.println("No se pudo encontrar un sensor BME280 válido, verificar cableado!");
        sensorAvailable = false;
        return false;
    }
    Serial.println("Sensor BME280 inicializado.");
    sensorAvailable = true;
    return true;
}

/**
 * @brief Verifica si el sensor está disponible y funcionando
 *
 * @return true si el sensor está operativo, false en caso contrario
 */
bool isSensorAvailable() {
    return sensorAvailable;
}

/**
 * @brief Intenta reinicializar el sensor BME280
 *
 * @return true si la reinicialización es exitosa, false en caso contrario
 */
bool retrySensorInit() {
    if (sensorAvailable) {
        return true; // Ya está inicializado
    }

    Serial.println("Reintentando inicialización del sensor BME280...");
    return initSensor();
}

/**
 * @brief Lee la temperatura del sensor BME280
 *
 * @return Temperatura en grados Celsius, o -999.0 si hay error
 */
float readTemperature() {
    if (!sensorAvailable) {
        return -999.0f; // Valor de error
    }
    return bme.readTemperature();
}

/**
 * @brief Lee la humedad del sensor BME280
 *
 * @return Humedad relativa en porcentaje, o -1.0 si hay error
 */
float readHumidity() {
    if (!sensorAvailable) {
        return -1.0f; // Valor de error
    }
    return bme.readHumidity();
}

/**
 * @brief Lee la presión del sensor BME280
 *
 * @return Presión atmosférica en hPa, o -1.0 si hay error
 */
float readPressure() {
    if (!sensorAvailable) {
        return -1.0f; // Valor de error
    }
    return bme.readPressure() / 100.0F;  // Convertir Pa a hPa
}

/**
 * @brief Fuerza el estado del sensor para testing (solo para depuración)
 *
 * @param available true para simular sensor disponible, false para simular fallo
 * @note Esta función es solo para testing y depuración
 */
void setSensorAvailableForTesting(bool available) {
    sensorAvailable = available;
    if (available) {
        Serial.println("TESTING: Sensor forzado a disponible");
    } else {
        Serial.println("TESTING: Sensor forzado a no disponible");
    }
}

/**
 * @brief Obtiene todos los datos del sensor empaquetados para envío LoRaWAN
 *
 * Lee temperatura, humedad, presión y voltaje de batería, los valida
 * y los empaqueta en formato binario optimizado.
 *
 * Formato de datos (8 bytes):
 * - Bytes 0-1: Temperatura (°C * 100, int16 big-endian)
 * - Bytes 2-3: Humedad (% * 100, uint16 big-endian)
 * - Bytes 4-5: Presión (hPa * 100, uint16 big-endian)
 * - Bytes 6-7: Batería (V * 100, uint16 big-endian)
 *
 * @param payload Buffer donde almacenar los datos empaquetados (mínimo 8 bytes)
 * @param maxSize Tamaño máximo del buffer
 * @return Número de bytes escritos en el payload, o 0 si hay error
 */
uint8_t getSensorPayload(uint8_t* payload, uint8_t maxSize) {
    if (maxSize < 8) {
        return 0; // Buffer demasiado pequeño
    }

    // ==================== LECTURA DE TODOS LOS SENSORES ====================
    float temperatura, humedad, presion, bateria;
    bool sensorOk = isSensorAvailable();

    if (sensorOk) {
        // Intentar leer datos del sensor BME280
        temperatura = readTemperature();
        humedad = readHumidity();
        presion = readPressure();

        // Verificar si las lecturas son válidas
        if (temperatura == -999.0f || humedad == -1.0f || presion == -1.0f) {
            sensorOk = false; // Lectura fallida
            Serial.println("Error en lectura del sensor BME280, enviando datos de error");
        }
    }

    if (!sensorOk) {
        // Sensor no disponible o lectura fallida - enviar datos de error
        temperatura = -999.0f;  // Código de error para temperatura
        humedad = -1.0f;        // Código de error para humedad
        presion = -1.0f;        // Código de error para presión

        Serial.println("Enviando datos de error del sensor BME280");

        // Intentar reinicializar el sensor para el próximo ciclo
        retrySensorInit();
    }

    // Leer voltaje de batería (siempre disponible)
    bateria = readBatteryVoltage();

    // ==================== EMPAQUETADO DE DATOS ====================
    // Formato optimizado: 8 bytes total para 4 valores
    int16_t temp_int = (int16_t)(temperatura * 100);  // Escalar para mantener precisión decimal
    uint16_t hum_int = (uint16_t)(humedad * 100);
    uint16_t pres_int = (uint16_t)(presion * 100);
    uint16_t batt_int = (uint16_t)(bateria * 100);

    // Big-endian encoding
    payload[0] = temp_int >> 8;    payload[1] = temp_int & 0xFF;
    payload[2] = hum_int >> 8;     payload[3] = hum_int & 0xFF;
    payload[4] = pres_int >> 8;    payload[5] = pres_int & 0xFF;
    payload[6] = batt_int >> 8;    payload[7] = batt_int & 0xFF;

    return 8; // Bytes escritos
}

/**
 * @brief Obtiene el estado de los sensores para mostrar en pantalla
 *
 * @param temp Referencia donde almacenar la temperatura
 * @param hum Referencia donde almacenar la humedad
 * @param pres Referencia donde almacenar la presión
 * @param battery Referencia donde almacenar el voltaje de batería
 * @return true si los datos son válidos, false si hay error
 */
bool getSensorDataForDisplay(float& temp, float& hum, float& pres, float& battery) {
    bool sensorOk = isSensorAvailable();

    if (sensorOk) {
        // Leer datos del sensor
        temp = readTemperature();
        hum = readHumidity();
        pres = readPressure();

        // Verificar si las lecturas son válidas
        if (temp == -999.0f || hum == -1.0f || pres == -1.0f) {
            sensorOk = false;
        }
    }

    if (!sensorOk) {
        // Datos de error
        temp = -999.0f;
        hum = -1.0f;
        pres = -1.0f;
    }

    // Leer batería siempre
    battery = readBatteryVoltage();

    return sensorOk;
}
