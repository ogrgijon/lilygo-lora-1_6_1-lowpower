/**
 * @file      main_otta.ino
 * @brief     Archivo principal del proyecto de Sensor LoRaWAN OTAA de Bajo Consumo
 *
 * Este archivo es el punto de entrada de Arduino para el proyecto.
 * Inicializa el hardware y la comunicación LoRaWAN, luego entra en el bucle principal.
 *
 * Características:
 * - Integración con sensor BME280 para temperatura, humedad y presión
 * - Envío periódico de datos vía LoRaWAN OTAA
 * - Gestión de energía: pantalla activa solo en fases críticas
 * - Sueño profundo para minimizar consumo
 *
 * @note      Basado en el proyecto base de LilyGo LoRa Series
 * @author    Desarrollado para proyectos IoT de bajo consumo
 * @version   1.0
 * @date      2025
 */

#include "loramac.h"      // Funciones LoRaWAN y sensor
#include "LoRaBoards.h"   // Configuración de hardware y pines
#include "screen.h"       // Gestión de pantalla

/**
 * @brief     Función de configuración inicial de Arduino
 *
 * Inicializa el hardware de la placa, espera un retraso para estabilización
 * y configura la comunicación LoRaWAN.
 */
void setup()
{
    setupBoards(false);  // Configura pines y periféricos, mantiene display activo para gestión
    // Retraso necesario para estabilización de alimentación al encender
    delay(1500);
    Serial.println("Proyecto de Sensor LoRaWAN de Bajo Consumo Iniciando...");
    setupLMIC();    // Inicializa LMIC y sensor BME280

    // Inicializar sistema de pantalla
    initDisplay();
    sendInfoMessage("Sistema Iniciado", 3000);
}

/**
 * @brief     Bucle principal de Arduino
 *
 * Maneja el ciclo de vida de LoRaWAN: join, envío de datos, eventos y sueño.
 * Se ejecuta continuamente, pero el dispositivo entra en sueño profundo entre transmisiones.
 */
void loop()
{
    loopLMIC();     // Procesa eventos LoRaWAN y gestiona el ciclo de bajo consumo
    updateDisplay(); // Gestiona la pantalla y mensajes
}

