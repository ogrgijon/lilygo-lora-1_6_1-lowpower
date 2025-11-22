/**
 * @file      sensor_interface.h
 * @brief     Interfaz genérica para sensores
 *
 * Esta interfaz define las funciones que debe implementar cada sensor,
 * permitiendo cambiar de sensor sin modificar el código principal.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// ESTRUCTURAS DE DATOS
// ============================================================================

/**
 * @brief Estructura que contiene todas las lecturas del sensor
 */
typedef struct {
    float temperature;    /**< Temperatura en °C */
    float humidity;       /**< Humedad relativa en % */
    float pressure;       /**< Presión atmosférica en hPa */
    float battery;        /**< Voltaje de batería en V */
    bool valid;           /**< true si todas las lecturas son válidas */
} sensor_data_t;

/**
 * @brief Estructura para configuración del payload
 */
typedef struct {
    uint8_t* buffer;      /**< Buffer donde escribir el payload */
    uint8_t max_size;     /**< Tamaño máximo del buffer */
    uint8_t written;      /**< Bytes realmente escritos */
} payload_config_t;

// ============================================================================
// FUNCIONES DE LA INTERFAZ DEL SENSOR
// ============================================================================

/**
 * @brief Inicializa el sensor seleccionado
 *
 * @return true si la inicialización es exitosa, false en caso contrario
 */
bool sensor_init(void);

/**
 * @brief Verifica si el sensor está disponible y funcionando
 *
 * @return true si el sensor está operativo, false en caso contrario
 */
bool sensor_is_available(void);

/**
 * @brief Intenta reinicializar el sensor
 *
 * @return true si la reinicialización es exitosa, false en caso contrario
 */
bool sensor_retry_init(void);

/**
 * @brief Lee todos los datos del sensor
 *
 * @param data Puntero a estructura donde almacenar los datos
 * @return true si la lectura es exitosa, false en caso contrario
 */
bool sensor_read_all(sensor_data_t* data);

/**
 * @brief Obtiene el payload empaquetado para envío LoRaWAN
 *
 * @param config Configuración del payload (buffer, tamaño, etc.)
 * @return Número de bytes escritos, o 0 si hay error
 */
uint8_t sensor_get_payload(payload_config_t* config);

/**
 * @brief Obtiene el nombre del sensor actual
 *
 * @return Cadena con el nombre del sensor
 */
const char* sensor_get_name(void);

/**
 * @brief Fuerza el estado del sensor para testing (solo para depuración)
 *
 * @param available true para simular sensor disponible, false para simular fallo
 * @note Esta función es solo para testing y depuración
 */
void sensor_set_available_for_testing(bool available);

#endif // SENSOR_INTERFACE_H