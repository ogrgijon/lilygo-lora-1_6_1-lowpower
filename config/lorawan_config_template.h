/**
 * @file      lorawan_config_template.h
 * @brief     Plantilla de configuración de claves LoRaWAN
 *
 * Este archivo es una PLANTILLA para configurar las claves LoRaWAN.
 * Para usar:
 * 1. Copia este archivo como lorawan_config.h
 * 2. Actualiza las claves con tus valores reales de TTN
 * 3. El archivo lorawan_config.h será ignorado por git (está en .gitignore)
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#ifndef LORAWAN_CONFIG_H
#define LORAWAN_CONFIG_H

#include <lmic.h>
#include <Arduino.h> // For PROGMEM and uint8_t/u1_t

// ==================== CLAVES LoRaWAN OTAA ====================
// ¡ACTUALIZAR CON TUS VALORES DE TTN!
// Formato: LSB para APPEUI/DEVEUI, MSB para APPKEY
// Obtén estos valores de la consola de TTN en tu aplicación

// Application EUI (AppEUI) - LSB format
// Ejemplo: {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Device EUI (DevEUI) - LSB format
// Ejemplo: {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22}
static const u1_t PROGMEM DEVEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Application Key (AppKey) - MSB format
// Ejemplo: {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}
static const u1_t PROGMEM APPKEY[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#endif // LORAWAN_CONFIG_H