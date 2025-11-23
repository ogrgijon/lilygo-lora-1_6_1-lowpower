/**
 * @file      main_otta.ino
 * @brief     Archivo principal del proyecto de Sensor LoRaWAN OTAA de Bajo Consumo
 *
 * Este archivo es el punto de entrada de Arduino para el proyecto.
 * Inicializa el hardware y la comunicación LoRaWAN, luego entra en el bucle principal.
 *
 * Características:
 * - Integración con sensor DHT22 para temperatura y humedad
 * - Envío periódico de datos vía LoRaWAN OTAA
 * - Gestión de energía: pantalla activa solo en fases críticas
 * - Sueño profundo para minimizar consumo
 * - Watchdog Timer: Reinicio automático si no hay actividad por 5 minutos
 *
 * Watchdog Timer:
 * - Timeout: 5 minutos (300 segundos)
 * - Función: Detecta cuelgues del sistema y fuerza reinicio
 * - Alimentación: Se resetea en cada iteración del bucle principal
 * - Modo: Panic (reinicio inmediato al expirar)
 *
 * @note      Basado en el proyecto base de LilyGo LoRa Series
 * @author    Desarrollado para proyectos IoT de bajo consumo
 * @version   1.1 - Con watchdog timer
 * @date      2025
 */

#include "loramac.h"      // Funciones LoRaWAN y sensor
#include "LoRaBoards.h"   // Configuración de hardware y pines
#include "screen.h"       // Gestión de pantalla
#include <esp_task_wdt.h> // Watchdog timer para protección contra cuelgues

/**
 * @brief     Función de configuración inicial de Arduino
 *
 * Inicializa el hardware de la placa, espera un retraso para estabilización
 * y configura la comunicación LoRaWAN.
 * También configura el Watchdog Timer para protección contra cuelgues.
 */
void setup()
{
    setupBoards(false);  // Configura pines y periféricos, mantiene display activo para gestión
    // Retraso necesario para estabilización de alimentación al encender
    delay(1500);
    Serial.println("Proyecto de Sensor LoRaWAN de Bajo Consumo Iniciando...");
    setupLMIC();    // Inicializa LMIC y sensor DHT22

    // Inicializar watchdog timer (5 minutos)
    esp_task_wdt_init(300, true); // 300 segundos = 5 minutos, panic on timeout
    esp_task_wdt_add(NULL);       // Agregar tarea actual al WDT

    // Inicializar sistema de pantalla
    initDisplay();
    sendInfoMessage("Sistema Iniciado", 3000);
}

/**
 * @brief     Bucle principal de Arduino
 *
 * Maneja el ciclo de vida de LoRaWAN: join, envío de datos, eventos y sueño.
 * Se ejecuta continuamente, pero el dispositivo entra en sueño profundo entre transmisiones.
 * Alimenta el watchdog timer en cada iteración para indicar actividad normal.
 */
void loop()
{
    loopLMIC();     // Procesa eventos LoRaWAN y gestiona el ciclo de bajo consumo
    updateDisplay(); // Gestiona la pantalla y mensajes
    esp_task_wdt_reset(); // Alimentar watchdog para indicar actividad
}

