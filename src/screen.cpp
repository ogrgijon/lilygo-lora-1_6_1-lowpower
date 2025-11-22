/**
 * @file      screen.cpp
 * @brief     Implementación de funciones de pantalla OLED
 *
 * Este archivo contiene las funciones para gestionar la pantalla OLED,
 * mostrar mensajes de estado y datos del sensor.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#include "screen.h"
#include "LoRaBoards.h"

// Configuración de la cola de mensajes
#define MAX_SCREEN_MESSAGES 10
static ScreenMessage messageQueue[MAX_SCREEN_MESSAGES];
static int messageCount = 0;
static int currentMessageIndex = -1;
static uint32_t lastUpdateTime = 0;
static bool displayActive = false;

// Variables para controlar el estado de la pantalla
static uint32_t currentMessageStartTime = 0;
static bool screenNeedsUpdate = false;

/**
 * @brief Inicializa la pantalla OLED
 *
 * @return true si la inicialización es exitosa, false en caso contrario
 */
bool initDisplay() {
    if (!u8g2) {
        Serial.println("Display no disponible");
        return false;
    }
    u8g2->begin();
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_ncenB08_tr);
    u8g2->drawStr(0, 20, "MediaLab LoRaWAN");
    u8g2->drawStr(0, 40, "Bajo Consumo V.1.0");
    u8g2->sendBuffer();
    delay(2000);

    displayActive = true;
    screenNeedsUpdate = true;

    return true;
}

/**
 * @brief Agrega un mensaje a la cola
 *
 * @param message Mensaje a agregar
 * @return true si se agregó correctamente, false si la cola está llena
 */
static bool addMessageToQueue(const ScreenMessage& message) {
    if (messageCount >= MAX_SCREEN_MESSAGES) {
        // Cola llena, eliminar el mensaje más antiguo
        for (int i = 1; i < messageCount; i++) {
            messageQueue[i-1] = messageQueue[i];
        }
        messageCount--;
    }

    messageQueue[messageCount] = message;
    messageQueue[messageCount].timestamp = millis();
    messageQueue[messageCount].displayed = false;
    messageCount++;

    screenNeedsUpdate = true;
    return true;
}

/**
 * @brief Obtiene el siguiente mensaje a mostrar
 *
 * @return Índice del mensaje a mostrar, o -1 si no hay mensajes
 */
static int getNextMessageIndex() {
    if (messageCount == 0) return -1;

    uint32_t currentTime = millis();

    // Si hay un mensaje actual y no ha expirado, mantenerlo
    if (currentMessageIndex >= 0 && currentMessageIndex < messageCount) {
        ScreenMessage& currentMsg = messageQueue[currentMessageIndex];
        if (currentMsg.duration == 0 ||
            (currentTime - currentMsg.timestamp) < currentMsg.duration) {
            return currentMessageIndex;
        }
    }

    // Buscar el siguiente mensaje no mostrado
    for (int i = 0; i < messageCount; i++) {
        if (!messageQueue[i].displayed) {
            return i;
        }
    }

    // Si todos los mensajes se mostraron, mostrar el último mensaje permanente
    for (int i = messageCount - 1; i >= 0; i--) {
        if (messageQueue[i].duration == 0) {
            return i;
        }
    }

    return -1; // No hay mensajes para mostrar
}

/**
 * @brief Renderiza un mensaje en la pantalla según su tipo
 *
 * @param message Mensaje a renderizar
 */
static void renderMessage(const ScreenMessage& message) {
    if (!u8g2) return;

    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_ncenB08_tr);

    // Mostrar tipo de mensaje en la parte superior
    const char* typeStr = "";
    switch (message.type) {
        case MSG_INFO: typeStr = "[INFO]"; break;
        case MSG_WARNING: typeStr = "[WARN]"; break;
        case MSG_ERROR: typeStr = "[ERROR]"; break;
        case MSG_SUCCESS: typeStr = "[OK]"; break;
        case MSG_SENSOR_DATA: typeStr = "[SENSOR]"; break;
        case MSG_STATUS: typeStr = "[STATUS]"; break;
    }

    u8g2->drawStr(0, 10, typeStr);

    // Mostrar el texto del mensaje
    // Dividir en líneas si es necesario
    String text = message.text;
    int maxCharsPerLine = 16; // Aproximadamente para fuente ncenB08
    int yPos = 25;

    while (text.length() > 0 && yPos < 60) {
        String line;
        if (text.length() <= maxCharsPerLine) {
            line = text;
            text = "";
        } else {
            // Buscar un espacio para cortar la línea
            int cutPos = maxCharsPerLine;
            while (cutPos > 0 && text.charAt(cutPos) != ' ') {
                cutPos--;
            }
            if (cutPos == 0) cutPos = maxCharsPerLine;

            line = text.substring(0, cutPos);
            text = text.substring(cutPos);
            text.trim(); // Remover espacio inicial
        }

        u8g2->drawStr(0, yPos, line.c_str());
        yPos += 12;
    }

    u8g2->sendBuffer();
}

/**
 * @brief Actualiza y gestiona la pantalla (llamar en loop principal)
 */
void updateDisplay() {
    if (!u8g2 || !displayActive) return;

    uint32_t currentTime = millis();

    // No actualizar demasiado frecuentemente para evitar flickering
    if (currentTime - lastUpdateTime < 100) return;
    lastUpdateTime = currentTime;

    int nextMessageIndex = getNextMessageIndex();

    // Si hay un cambio de mensaje o se necesita actualización
    if (nextMessageIndex != currentMessageIndex || screenNeedsUpdate) {
        if (nextMessageIndex >= 0) {
            currentMessageIndex = nextMessageIndex;
            ScreenMessage& message = messageQueue[currentMessageIndex];
            renderMessage(message);
            message.displayed = true;
            currentMessageStartTime = currentTime;
        } else {
            // No hay mensajes para mostrar, apagar pantalla
            turnOffDisplay();
        }
        screenNeedsUpdate = false;
    }

    // Verificar si el mensaje actual ha expirado
    if (currentMessageIndex >= 0 && currentMessageIndex < messageCount) {
        ScreenMessage& currentMsg = messageQueue[currentMessageIndex];
        if (currentMsg.duration > 0 &&
            (currentTime - currentMsg.timestamp) >= currentMsg.duration) {
            // Mensaje expirado, buscar siguiente
            screenNeedsUpdate = true;
        }
    }
}

/**
 * @brief Envía un mensaje a la pantalla con duración específica
 */
void sendScreenMessage(ScreenMessageType type, const String& text, uint32_t duration) {
    ScreenMessage message;
    message.type = type;
    message.text = text;
    message.duration = duration;

    addMessageToQueue(message);
    
    // Asegurar que la pantalla esté activa para mostrar el mensaje
    if (!displayActive) {
        displayActive = true;
        if (u8g2) {
            u8g2->setPowerSave(0);
        }
    }
}

/**
 * @brief Envía un mensaje de información
 */
void sendInfoMessage(const String& text, uint32_t duration) {
    sendScreenMessage(MSG_INFO, text, duration);
}

/**
 * @brief Envía un mensaje de advertencia
 */
void sendWarningMessage(const String& text, uint32_t duration) {
    sendScreenMessage(MSG_WARNING, text, duration);
}

/**
 * @brief Envía un mensaje de error
 */
void sendErrorMessage(const String& text, uint32_t duration) {
    sendScreenMessage(MSG_ERROR, text, duration);
}

/**
 * @brief Envía un mensaje de éxito
 */
void sendSuccessMessage(const String& text, uint32_t duration) {
    sendScreenMessage(MSG_SUCCESS, text, duration);
}

/**
 * @brief Muestra datos del sensor en la pantalla
 */
void displaySensorData(float temp, float hum, float battery, uint32_t duration) {
    char buffer[64];
    if (temp == -999.0f && hum == -1.0f) {
        // Datos de error del sensor
        sendScreenMessage(MSG_ERROR, "Sensor ERROR!", duration);
    } else {
        // Datos válidos del sensor
        sprintf(buffer, "T:%.1fC H:%.1f%% B:%.2fV", temp, hum, battery);
        sendScreenMessage(MSG_SENSOR_DATA, buffer, duration);
    }
}

/**
 * @brief Fuerza la actualización inmediata de la pantalla
 */
void forceDisplayUpdate() {
    screenNeedsUpdate = true;
    updateDisplay();
}

/**
 * @brief Limpia todos los mensajes pendientes
 */
void clearScreenMessages() {
    messageCount = 0;
    currentMessageIndex = -1;
    screenNeedsUpdate = true;
}

/**
 * @brief Obtiene el número de mensajes en cola
 */
int getPendingMessageCount() {
    return messageCount;
}

/**
 * @brief Apaga la pantalla para ahorrar energía
 */
void turnOffDisplay() {
    if (u8g2) {
        u8g2->setPowerSave(1);
    }
    displayActive = false;
    currentMessageIndex = -1;
}
