/**
 * @file      ttn_decoder_generator.cpp
 * @brief     Generador dinámico de decoders TTN según configuración de sensores
 *
 * Este archivo genera automáticamente el código JavaScript del decoder
 * para The Things Network (TTN) basado en los sensores configurados.
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#include "../config/config.h"
#include <Arduino.h>

// =============================================================================
// CONFIGURACIÓN DEL GENERADOR DE DECODERS TTN
// =============================================================================

// Define esta constante en config.h para habilitar la impresión del decoder
#ifndef SHOW_TTN_DECODER
#define SHOW_TTN_DECODER 0  // Cambia a 1 para mostrar el decoder por Serial
#endif

// =============================================================================
// FUNCIONES PARA GENERAR EL DECODER TTN
// =============================================================================

/**
 * @brief Imprime el header del decoder TTN
 */
static void print_decoder_header() {
    Serial.println(F(""));
    Serial.println(F("==================== TTN PAYLOAD DECODER ===================="));
    Serial.println(F(""));
    Serial.println(F("// COPIA Y PEGA ESTE CÓDIGO EN TTN CONSOLE -> APPLICATIONS -> PAYLOAD FORMATTERS"));
    Serial.println(F("// 1. Ve a tu aplicación en TTN Console"));
    Serial.println(F("// 2. Ve a Payload formatters -> Uplink"));
    Serial.println(F("// 3. Selecciona 'JavaScript' como formatter type"));
    Serial.println(F("// 4. Pega el código siguiente en el campo 'Formatter code'"));
    Serial.println(F("// 5. Haz clic en 'Save changes'"));
    Serial.println(F(""));
    Serial.println(F("function decodeUplink(input) {"));
    Serial.println(F("  var data = {};"));
    Serial.println(F("  var bytes = input.bytes;"));
    Serial.println(F("  var offset = 0;"));
    Serial.println(F(""));
}

/**
 * @brief Imprime el código para decodificar temperatura
 */
static void print_temperature_decoder() {
    Serial.println(F("  // Temperatura (°C * 100)"));
    Serial.println(F("  data.temperature = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;"));
}

/**
 * @brief Imprime el código para decodificar humedad
 */
static void print_humidity_decoder() {
    Serial.println(F("  // Humedad (% * 100)"));
    Serial.println(F("  data.humidity = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;"));
}

/**
 * @brief Imprime el código para decodificar presión
 */
static void print_pressure_decoder() {
    Serial.println(F("  // Presión atmosférica (hPa * 10)"));
    Serial.println(F("  data.pressure = ((bytes[offset++] << 8) | bytes[offset++]) / 10.0;"));
}

/**
 * @brief Imprime el código para decodificar distancia
 */
static void print_distance_decoder() {
    Serial.println(F("  // Distancia (cm * 100)"));
    Serial.println(F("  data.distance = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;"));
}

/**
 * @brief Imprime el código para decodificar batería
 */
static void print_battery_decoder() {
    Serial.println(F(""));
    Serial.println(F("  // Batería (V * 100) - siempre presente"));
    Serial.println(F("  var batteryIndex = bytes.length - 2;"));
    Serial.println(F("  data.battery_voltage = ((bytes[batteryIndex] << 8) | bytes[batteryIndex + 1]) / 100.0;"));
    Serial.println(F(""));
}

/**
 * @brief Imprime el footer del decoder TTN
 */
static void print_decoder_footer() {
    Serial.println(F("  return { data: data };"));
    Serial.println(F("}"));
    Serial.println(F(""));
    Serial.println(F("==================== COPIA EL CÓDIGO ARRIBA ===================="));
    Serial.println(F(""));
}

/**
 * @brief Imprime información sobre la configuración actual
 */
static void print_configuration_info() {
    Serial.println(F("=== CONFIGURACIÓN ACTUAL DE SENSORES ==="));

    Serial.print(F("Sensores activos: "));

    bool first = true;
#ifdef ENABLE_SENSOR_DHT22
    if (!first) Serial.print(F(", "));
    Serial.print(F("DHT22"));
    first = false;
#endif
#ifdef ENABLE_SENSOR_DHT11
    if (!first) Serial.print(F(", "));
    Serial.print(F("DHT11"));
    first = false;
#endif
#ifdef ENABLE_SENSOR_DS18B20
    if (!first) Serial.print(F(", "));
    Serial.print(F("DS18B20"));
    first = false;
#endif
#ifdef ENABLE_SENSOR_BMP280
    if (!first) Serial.print(F(", "));
    Serial.print(F("BMP280"));
    first = false;
#endif
#ifdef ENABLE_SENSOR_HCSR04
    if (!first) Serial.print(F(", "));
    Serial.print(F("HC-SR04"));
    first = false;
#endif
#ifdef ENABLE_SENSOR_NONE
    if (!first) Serial.print(F(", "));
    Serial.print(F("NONE"));
    first = false;
#endif

    if (first) {
        Serial.print(F("NINGUNO (solo batería)"));
    }

    Serial.println(F(""));

    // Calcular tamaño del payload
    uint8_t payload_size = PAYLOAD_SIZE_BYTES;
    Serial.printf("Tamaño del payload: %d bytes\r\n", payload_size);

    // Información sobre qué campos están incluidos
    Serial.println(F("Campos incluidos en el payload:"));
    if (SYSTEM_HAS_TEMPERATURE) Serial.println(F("  ✓ Temperatura"));
    if (SYSTEM_HAS_HUMIDITY) Serial.println(F("  ✓ Humedad"));
    if (SYSTEM_HAS_PRESSURE) Serial.println(F("  ✓ Presión atmosférica"));
    if (SYSTEM_HAS_DISTANCE) Serial.println(F("  ✓ Distancia"));
    Serial.println(F("  ✓ Batería"));

    Serial.println(F(""));
}

/**
 * @brief Genera e imprime el decoder TTN completo por Serial
 *
 * Esta función analiza la configuración actual de sensores y genera
 * automáticamente el código JavaScript necesario para decodificar
 * el payload en The Things Network Console.
 */
void generate_and_print_ttn_decoder() {
    if (!SHOW_TTN_DECODER) {
        return; // No mostrar si no está habilitado
    }

    print_configuration_info();
    print_decoder_header();

    // Generar el código de decodificación según los sensores activos
    if (SYSTEM_HAS_TEMPERATURE) {
        print_temperature_decoder();
    }

    if (SYSTEM_HAS_HUMIDITY) {
        print_humidity_decoder();
    }

    if (SYSTEM_HAS_PRESSURE) {
        print_pressure_decoder();
    }

    if (SYSTEM_HAS_DISTANCE) {
        print_distance_decoder();
    }

    print_battery_decoder();
    print_decoder_footer();
}

/**
 * @brief Genera el decoder TTN y lo devuelve como string (para uso futuro)
 *
 * @param buffer Buffer donde almacenar el código generado
 * @param max_size Tamaño máximo del buffer
 * @return Número de caracteres escritos
 */
uint16_t generate_ttn_decoder_string(char* buffer, uint16_t max_size) {
    if (!buffer || max_size < 100) return 0;

    uint16_t offset = 0;

    // Header básico
    offset += snprintf(buffer + offset, max_size - offset,
        "function decodeUplink(input) {\n"
        "  var data = {};\n"
        "  var bytes = input.bytes;\n"
        "  var offset = 0;\n\n");

    // Campos de sensores
    if (SYSTEM_HAS_TEMPERATURE) {
        offset += snprintf(buffer + offset, max_size - offset,
            "  data.temperature = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;\n");
    }

    if (SYSTEM_HAS_HUMIDITY) {
        offset += snprintf(buffer + offset, max_size - offset,
            "  data.humidity = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;\n");
    }

    if (SYSTEM_HAS_PRESSURE) {
        offset += snprintf(buffer + offset, max_size - offset,
            "  data.pressure = ((bytes[offset++] << 8) | bytes[offset++]) / 10.0;\n");
    }

    if (SYSTEM_HAS_DISTANCE) {
        offset += snprintf(buffer + offset, max_size - offset,
            "  data.distance = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;\n");
    }

    // Batería
    offset += snprintf(buffer + offset, max_size - offset,
        "\n"
        "  var batteryIndex = bytes.length - 2;\n"
        "  data.battery_voltage = ((bytes[batteryIndex] << 8) | bytes[batteryIndex + 1]) / 100.0;\n"
        "\n"
        "  return { data: data };\n"
        "}\n");

    return offset;
}