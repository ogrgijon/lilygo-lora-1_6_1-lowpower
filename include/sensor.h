/**
 * @file      sensor.h
 * @brief     Cabeceras para el módulo de sensor BME280
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#pragma once

#include <Arduino.h>

/**
 * @brief Inicializa el sensor BME280
 *
 * @return true si la inicialización es exitosa, false en caso contrario
 */
bool initSensor();

/**
 * @brief Verifica si el sensor está disponible y funcionando
 *
 * @return true si el sensor está operativo, false en caso contrario
 */
bool isSensorAvailable();

/**
 * @brief Intenta reinicializar el sensor BME280
 *
 * @return true si la reinicialización es exitosa, false en caso contrario
 */
bool retrySensorInit();

/**
 * @brief Lee la temperatura del sensor BME280
 *
 * @return Temperatura en grados Celsius, o -999.0 si hay error
 */
float readTemperature();

/**
 * @brief Lee la humedad del sensor BME280
 *
 * @return Humedad relativa en porcentaje, o -1.0 si hay error
 */
float readHumidity();

/**
 * @brief Lee la presión del sensor BME280
 *
 * @return Presión atmosférica en hPa, o -1.0 si hay error
 */
float readPressure();

/**
 * @brief Fuerza el estado del sensor para testing (solo para depuración)
 *
 * @param available true para simular sensor disponible, false para simular fallo
 * @note Esta función es solo para testing y depuración
 */
void setSensorAvailableForTesting(bool available);

/**
 * @brief Obtiene todos los datos del sensor empaquetados para envío LoRaWAN
 *
 * Lee temperatura, humedad y voltaje de batería, los valida
 * y los empaqueta en formato binario optimizado.
 *
 * Formato de datos (7 bytes):
 * - Bytes 0-1: Temperatura (°C * 100, int16 big-endian)
 * - Bytes 2-3: Humedad (% * 100, uint16 big-endian)
 * - Bytes 4-5: Batería (V * 100, uint16 big-endian)
 * - Byte 6: Estado de carga solar (0=no, 1=sí)
 *
 * @param payload Buffer donde almacenar los datos empaquetados (mínimo 7 bytes)
 * @param maxSize Tamaño máximo del buffer
 * @return Número de bytes escritos en el payload, o 0 si hay error
 */
uint8_t getSensorPayload(uint8_t* payload, uint8_t maxSize);

/**
 * @brief Obtiene el estado de los sensores para mostrar en pantalla
 *
 * @param temp Referencia donde almacenar la temperatura
 * @param hum Referencia donde almacenar la humedad
 * @param pres Referencia donde almacenar la presión
 * @param battery Referencia donde almacenar el voltaje de batería
 * @return true si los datos son válidos, false si hay error
 */
bool getSensorDataForDisplay(float& temp, float& hum, float& pres, float& battery);