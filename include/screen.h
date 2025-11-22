/**
 * @file      screen.h
 * @brief     Cabeceras para el módulo de pantalla OLED
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#pragma once

#include <Arduino.h>

// Tipos de mensajes de pantalla
enum ScreenMessageType {
    MSG_INFO,           // Información general
    MSG_WARNING,        // Advertencia
    MSG_ERROR,          // Error
    MSG_SUCCESS,        // Éxito
    MSG_SENSOR_DATA,    // Datos del sensor
    MSG_STATUS          // Estado del sistema
};

// Estructura para mensajes de pantalla
struct ScreenMessage {
    ScreenMessageType type;
    String text;
    uint32_t duration;      // Duración en milisegundos (0 = mostrar hasta que llegue otro mensaje)
    uint32_t timestamp;     // Cuando se creó el mensaje
    bool displayed;         // Si ya se mostró
};

/**
 * @brief Inicializa la pantalla OLED
 *
 * @return true si la inicialización es exitosa, false en caso contrario
 */
bool initDisplay();

/**
 * @brief Actualiza y gestiona la pantalla (llamar en loop principal)
 * Maneja la cola de mensajes y controla cuándo mostrar/ocultar información
 */
void updateDisplay();

/**
 * @brief Envía un mensaje a la pantalla con duración específica
 *
 * @param type Tipo de mensaje
 * @param text Texto del mensaje
 * @param duration Duración en milisegundos (0 = mostrar hasta que llegue otro mensaje)
 */
void sendScreenMessage(ScreenMessageType type, const String& text, uint32_t duration = 3000);

/**
 * @brief Envía un mensaje de información
 *
 * @param text Texto del mensaje
 * @param duration Duración en milisegundos (por defecto 3000ms)
 */
void sendInfoMessage(const String& text, uint32_t duration = 3000);

/**
 * @brief Envía un mensaje de advertencia
 *
 * @param text Texto del mensaje
 * @param duration Duración en milisegundos (por defecto 4000ms)
 */
void sendWarningMessage(const String& text, uint32_t duration = 4000);

/**
 * @brief Envía un mensaje de error
 *
 * @param text Texto del mensaje
 * @param duration Duración en milisegundos (por defecto 5000ms)
 */
void sendErrorMessage(const String& text, uint32_t duration = 5000);

/**
 * @brief Envía un mensaje de éxito
 *
 * @param text Texto del mensaje
 * @param duration Duración en milisegundos (por defecto 2000ms)
 */
void sendSuccessMessage(const String& text, uint32_t duration = 2000);

/**
 * @brief Muestra datos del sensor en la pantalla
 *
 * @param temp Temperatura en °C
 * @param hum Humedad en %
 * @param pres Presión en hPa
 * @param battery Voltaje de batería en V
 * @param duration Duración en milisegundos (por defecto 5000ms)
 */
void displaySensorData(float temp, float hum, float pres, float battery, uint32_t duration = 5000);

/**
 * @brief Fuerza la actualización inmediata de la pantalla
 */
void forceDisplayUpdate();

/**
 * @brief Limpia todos los mensajes pendientes
 */
void clearScreenMessages();

/**
 * @brief Obtiene el número de mensajes en cola
 *
 * @return Número de mensajes pendientes
 */
int getPendingMessageCount();

/**
 * @brief Apaga la pantalla para ahorrar energía
 */
void turnOffDisplay();