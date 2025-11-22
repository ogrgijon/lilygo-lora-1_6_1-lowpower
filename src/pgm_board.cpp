/**
 * @file      loramac.cpp
 * @brief     Implementación del módulo LoRaWAN con sensor BME280 y bajo consumo
 *
 * Este archivo contiene la lógica principal para:
 * - Comunicación LoRaWAN usando LMIC (solo SX1276)
 * - Integración del sensor BME280 (temperatura, humedad, presión)
 * - Gestión de eventos LoRaWAN (join, envío, recepción)
 * - Control de bajo consumo con sueño profundo ESP32
 * - Interfaz con pantalla OLED para feedback visual
 * - ROBUSTEZ: Continúa funcionando aunque el sensor falle
 *
 * Arquitectura modular para fácil extensión:
 * - Agregar sensores: modificar setupSensor() y do_send()
 * - Cambiar período: ajustar TIEMPO_SUENO_SEGUNDOS
 * - Personalizar datos: modificar empaquetado en do_send()
 *
 * Gestión de errores del sensor:
 * - Si el sensor falla al inicializar, el dispositivo continúa funcionando
 * - Envía datos de error (-999 para temperatura, -1 para humedad/presión)
 * - Intenta reinicializar el sensor en cada ciclo
 * - Muestra "Sensor ERROR!" en pantalla cuando hay problemas
 *
 * @note      Compatible con placas LilyGo T3-S3 y similares
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.1 - Con manejo robusto de errores del sensor
 * @date      2025
 */

#include <Arduino.h>
#include <lmic.h>           // Biblioteca LMIC para LoRaWAN
#include <hal/hal.h>        // HAL para LMIC
#include <Wire.h>           // Comunicación I2C para sensor
#include <esp_sleep.h>      // Funciones de sueño ESP32
#include "LoRaBoards.h"     // Configuración de hardware
#include "sensor.h"         // Funciones del sensor
#include "screen.h"         // Funciones de pantalla

// Objeto global del sensor BME280
// Ahora definido en sensor.cpp

// Prototipos de funciones privadas
void enterDeepSleep();

// ==================== CONFIGURACIÓN LoRaWAN ====================
// Claves de activación OTAA - ¡ACTUALIZAR CON TUS VALORES DE TTN!
// Formato: LSB para APPEUI/DEVEUI, MSB para APPKEY
// Obtén estos valores de la consola de TTN en tu aplicación
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // LSB
static const u1_t PROGMEM DEVEUI[8] = {0x2B, 0x43, 0x07, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; // LSB
static const u1_t PROGMEM APPKEY[16] = {0x36, 0xA5, 0x8D, 0x12, 0x03, 0x93, 0xE7, 0xAF, 0xDC, 0xE8, 0x6E, 0x27, 0xF2, 0x91, 0x7F, 0x4B}; // MSB

// ==================== MAPEO DE PINES LMIC ====================
// Configuración de pines para diferentes plataformas
#ifdef STM32L073xx
const lmic_pinmap lmic_pins = {
    .nss =  RADIO_CS_PIN,
    .rxtx = RADIO_SWITCH_PIN,
    .rst =  RADIO_RST_PIN,
    .dio = {RADIO_DIO0_PIN, RADIO_DIO1_PIN, RADIO_DIO2_PIN},
    .rx_level = HIGH
};
#else
// Configuración para ESP32 (LilyGo T3-S3)
const lmic_pinmap lmic_pins = {
    .nss =  RADIO_CS_PIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst =  RADIO_RST_PIN,
    .dio = {RADIO_DIO0_PIN, RADIO_DIO1_PIN, RADIO_DIO2_PIN}
};
#endif

// Variables globales para LMIC
static osjob_t sendjob;
static int spreadFactor = DR_SF7;
static int joinStatus = EV_JOINING;
static const unsigned TX_INTERVAL = 30;  // No usado en bajo consumo, pero mantener para compatibilidad
#define SLEEP_TIME_SECONDS 60  // Periodo entre transmisiones
#define uS_TO_S_FACTOR 1000000ULL
static String lora_msg = "";

// Funciones callback de LMIC
void os_getArtEui (u1_t *buf)
{
    memcpy_P(buf, APPEUI, 8);
}

void os_getDevEui (u1_t *buf)
{
    memcpy_P(buf, DEVEUI, 8);
}

void os_getDevKey (u1_t *buf)
{
    memcpy_P(buf, APPKEY, 16);
}


// ==================== FUNCIONES DE CALLBACK Y UTILIDAD ====================

/**
 * @brief     Función callback para envío de datos del sensor
 *
 * Obtiene el payload completo de sensores vía la función getSensorPayload(),
 * que incluye temperatura, humedad, presión y voltaje de batería.
 * Envía los datos vía LoRaWAN y maneja la interfaz de usuario en pantalla.
 *
 * Formato de datos (8 bytes):
 * - Bytes 0-1: Temperatura (°C * 100, int16 big-endian)
 * - Bytes 2-3: Humedad (% * 100, uint16 big-endian)
 * - Bytes 4-5: Presión (hPa * 100, uint16 big-endian)
 * - Bytes 6-7: Batería (V * 100, uint16 big-endian)
 *
 * @param j  Puntero al trabajo OS (no usado directamente)
 *
 * @note      Se llama automáticamente por LMIC cuando es momento de enviar
 * @warning   Asegúrate de que el sensor esté inicializado antes de llamar
 */
void do_send(osjob_t *j)
{
    // Verificar estado de join
    if (joinStatus == EV_JOINING) {
        Serial.println(F("Aún no unido a la red"));
        // Reprogramar envío para más tarde
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
        return;
    }

    // Verificar si hay una transmisión/recepción pendiente
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("Transmisión pendiente, esperando..."));
        return;
    }

    Serial.println(F("Preparando datos del sensor para envío..."));

    // ==================== OBTENER PAYLOAD COMPLETO ====================
    uint8_t payload[8];
    uint8_t payloadSize = getSensorPayload(payload, sizeof(payload));

    if (payloadSize == 0) {
        Serial.println("Error al obtener payload del sensor");
        sendErrorMessage("Error payload", 3000);
        // Programar siguiente intento en 10 segundos
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(10), do_send);
        return;
    }

    // ==================== OBTENER DATOS PARA DISPLAY ====================
    float temperatura, humedad, presion, bateria;
    bool sensorOk = getSensorDataForDisplay(temperatura, humedad, presion, bateria);

    // ==================== INTERFAZ DE USUARIO ====================
    // Mostrar datos en pantalla OLED durante el envío
    if (sensorOk) {
        displaySensorData(temperatura, humedad, presion, bateria, 5000);
    } else {
        // Mostrar solo batería cuando no hay sensor
        sendWarningMessage("Solo bateria", 3000);
    }

    // ==================== ENVÍO LoRaWAN ====================
    LMIC_setTxData2(1, payload, payloadSize, 0);

    if (sensorOk) {
        Serial.printf("Enviando: Temp=%.2f C, Hum=%.2f %%, Pres=%.2f hPa, Batt=%.2f V\n",
                     temperatura, humedad, presion, bateria);
    } else {
        Serial.printf("Enviando datos limitados: Temp=ERROR, Hum=ERROR, Pres=ERROR, Batt=%.2f V\n", bateria);
    }

    // Nota: No se programa el siguiente envío aquí - se hará después del TX completo en onEvent
}

/**
 * @brief     Callback de eventos LoRaWAN
 *
 * Maneja todos los eventos del ciclo de vida LoRaWAN:
 * - Join: Unión a la red
 * - TX Complete: Envío exitoso, transición a sueño
 * - Errores: Reintentos de join
 *
 * @param ev  Código del evento (EV_JOINED, EV_TXCOMPLETE, etc.)
 *
 * @note      Función crítica para el flujo de bajo consumo
 */
void onEvent (ev_t ev)
{
    Serial.print(os_getTime());
    Serial.print(": ");

    switch (ev) {
        case EV_TXCOMPLETE:
            Serial.println(F("Transmisión completada (incluyendo RX windows)"));

            // Verificar si se recibió ACK
            if (LMIC.txrxFlags & TXRX_ACK) {
                Serial.println(F("ACK recibido de gateway"));
                lora_msg = "ACK recibido.";
            }

            // Mostrar métricas de enlace
            lora_msg = "rssi:" + String(LMIC.rssi) + " snr: " + String(LMIC.snr);

            // Verificar datos downlink
            if (LMIC.dataLen) {
                Serial.print(F("Datos recibidos: "));
                Serial.print(LMIC.dataLen);
                Serial.println(F(" bytes"));
                // Aquí se podrían procesar comandos downlink
            }

            // Feedback visual de éxito
            sendSuccessMessage("Datos enviados!", 2000);

            // ==================== TRANSICIÓN A SUEÑO PROFUNDO ====================
            enterDeepSleep();
            break;

        case EV_JOINING:
            Serial.println(F("Iniciando proceso de join..."));
            lora_msg = "Uniéndose OTAA....";
            joinStatus = EV_JOINING;

            // Mostrar estado en pantalla
            sendInfoMessage("Uniéndose OTAA....", 0); // Mostrar hasta que se complete
            break;

        case EV_JOIN_FAILED:
            Serial.println(F("Join fallido - reintentando..."));
            lora_msg = "Unión OTAA fallida";

            // Mostrar error en pantalla
            sendWarningMessage("Unión fallida, reintentando...", 3000);

            // Programar reintento en 10 segundos
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(10), do_send);
            break;

        case EV_JOINED:
            Serial.println(F("Unión exitosa a la red LoRaWAN"));
            lora_msg = "Unido!";
            joinStatus = EV_JOINED;

            // Celebrar éxito en pantalla
            sendSuccessMessage("Unido a TTN!", 3000);
            delay(2000);  // Mostrar por 2 segundos
            turnOffDisplay();  // Apagar pantalla para ahorrar energía

            // Deshabilitar link check para simplificar
            LMIC_setLinkCheckMode(0);
            break;

        case EV_RXCOMPLETE:
            Serial.println(F("Recepción completada"));
            break;

        case EV_LINK_DEAD:
            Serial.println(F("Enlace perdido"));
            break;

        case EV_LINK_ALIVE:
            Serial.println(F("Enlace recuperado"));
            break;

        default:
            Serial.println(F("Evento desconocido"));
            break;
    }
}

/**
 * @brief     Entrada en modo sueño profundo
 *
 * Configura el temporizador ESP32 para despertar después del intervalo definido
 * y apaga la pantalla para maximizar el ahorro de energía.
 *
 * @note      El dispositivo se reiniciará completamente al despertar
 * @warning   Toda la memoria RAM se pierde durante el sueño profundo
 */
void enterDeepSleep() {
    Serial.println("Entrando en sueño profundo por " + String(SLEEP_TIME_SECONDS) + " segundos...");

    // Apagar pantalla para ahorrar energía
    turnOffDisplay();

    // Configurar despertar por temporizador
    esp_sleep_enable_timer_wakeup(SLEEP_TIME_SECONDS * uS_TO_S_FACTOR);

    // Entrar en sueño profundo (reinicio completo al despertar)
    esp_deep_sleep_start();
}

// ==================== FUNCIONES PÚBLICAS ====================

/**
 * @brief     Inicializa LMIC, sensor BME280 y configura LoRaWAN OTAA
 *
 * Esta función configura:
 * - Pines del módulo LoRa (TCXO si aplica)
 * - Sistema operativo LMIC
 * - Sensor BME280 con verificación de conexión
 * - Sesión LoRaWAN con claves OTAA
 * - Canales TTN para Europa (868MHz)
 * - Parámetros de enlace y tasa de datos
 * - Inicia el proceso de join
 *
 * @note      Debe llamarse una vez en setup() de Arduino
 * @warning   Asegúrate de actualizar las claves LoRaWAN antes de usar
 */
void setupLMIC(void)
{
    // Habilitar TCXO si el hardware lo requiere (para estabilidad de frecuencia)
#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    // Inicializar el sistema operativo de LMIC
    os_init();

    // ==================== CONFIGURACIÓN DEL SENSOR ====================
    // Inicializar sensor BME280 en dirección I2C 0x76 (por defecto)
    if (!initSensor()) {
        Serial.println("ADVERTENCIA: Sensor no disponible, el dispositivo continuará funcionando y enviará datos de error");
        sendWarningMessage("Sensor no disponible", 5000);
        // No entramos en bucle infinito - el dispositivo debe continuar funcionando
    }

    // ==================== CONFIGURACIÓN LoRaWAN ====================
    // Reiniciar estado MAC - descarta sesiones y transferencias pendientes
    LMIC_reset();

    // Configurar tolerancia de error de reloj (1% máximo)
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    // Configurar canales TTN Europa (868MHz) - habilita todos los canales disponibles
    // Esto evita sobrecargar los 3 canales base de LoRaWAN
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band

    // Deshabilitar validación de enlace (link check) para simplificar
    LMIC_setLinkCheckMode(0);

    // Configurar downlink RX2 con SF9 (estándar TTN)
    LMIC.dn2Dr = DR_SF9;

    // Configurar spread factor y potencia de transmisión
    LMIC_setDrTxpow(spreadFactor, 14);

    Serial.println("Iniciando proceso de join LoRaWAN...");
    // Iniciar el proceso de joining a la red
    LMIC_startJoining();

    // Programar primer envío (también inicia join si no está hecho)
    do_send(&sendjob);
}

/**
 * @brief     Bucle principal para procesar eventos LoRaWAN
 *
 * Ejecuta una iteración del bucle de eventos de LMIC.
 * Maneja automáticamente el join, envíos y transiciones a sueño profundo.
 *
 * @note      Debe llamarse continuamente en loop() de Arduino
 */
void loopLMIC(void)
{
    os_runloop_once();  // Procesar eventos LMIC pendientes
}

// Función de utilidad para leer registro (si es necesario)
u1_t readReg (u1_t addr)
{
    hal_pin_nss(0);
    hal_spi(addr & 0x7F);
    u1_t val = hal_spi(0x00);
    hal_pin_nss(1);
    return val;
}

