#ifndef SENSOR_CONFIG_TEMPLATE_H
#define SENSOR_CONFIG_TEMPLATE_H

// =============================================================================
// PLANTILLA PARA NUEVO SENSOR - SISTEMA SIMPLE
// =============================================================================
// INSTRUCCIONES PARA CREAR UN NUEVO SENSOR:
//
// 1. Copia este archivo como sensor_tu_sensor.h
// 2. Cambia el nombre en las guardas #ifndef/#define/#endif
// 3. Modifica los valores marcados con "MODIFICA"
// 4. Añade el sensor a config.h
// 5. Implementa las funciones en src/sensor.cpp
//
// EJEMPLO PARA UN SENSOR DE ULTRASONIDOS HC-SR04:
// - Copia este archivo como sensor_hcsr04.h
// - Cambia SENSOR_NAME a "HC-SR04"
// - Pon SENSOR_HAS_DISTANCE true (y añade el campo a la estructura)
// - Define los pines y configuraciones específicas

// MODIFICA: Nombre del sensor (para display y logs)
#define SENSOR_NAME "TU_SENSOR"

// MODIFICA: ¿Qué mide este sensor? (true/false)
#define SENSOR_HAS_TEMPERATURE false
#define SENSOR_HAS_HUMIDITY false
#define SENSOR_HAS_PRESSURE false
// Añade aquí nuevas capacidades si necesitas:
// #define SENSOR_HAS_DISTANCE false
// #define SENSOR_HAS_LIGHT false

// MODIFICA: Configuración específica del hardware
// #define TU_SENSOR_PIN 13
// #define TU_SENSOR_I2C_ADDR 0x40
// #define TU_SENSOR_POWER_PIN 12
// #define TU_SENSOR_POWER_ON_DELAY_MS 100

// MODIFICA: Rangos válidos para validación
// #define TEMPERATURE_MIN -40.0f
// #define TEMPERATURE_MAX 80.0f
// #define DISTANCE_MIN 2.0f
// #define DISTANCE_MAX 400.0f

// MODIFICA: Configuración de lecturas
// #define TU_SENSOR_READ_ATTEMPTS 3
// #define TU_SENSOR_READ_DELAY_MS 100

#endif // SENSOR_CONFIG_TEMPLATE_H
