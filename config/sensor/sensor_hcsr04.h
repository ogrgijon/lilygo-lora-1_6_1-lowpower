#ifndef SENSOR_HCSR04_H
#define SENSOR_HCSR04_H

// =============================================================================
// CONFIGURACIÓN DEL SENSOR HC-SR04 - MEDIDOR DE DISTANCIA ULTRASÓNICA
// =============================================================================

// Nombre del sensor (para display y logs)
#define SENSOR_NAME "HC-SR04"

// ¿Qué mide este sensor? (true/false)
#define SENSOR_HAS_TEMPERATURE false
#define SENSOR_HAS_HUMIDITY false
#define SENSOR_HAS_PRESSURE false
#define SENSOR_HAS_DISTANCE true

// Configuración específica del hardware
#define HCSR04_TRIG_PIN 13          // Pin de trigger
#define HCSR04_ECHO_PIN 12          // Pin de echo
#define HCSR04_MAX_DISTANCE_CM 400  // Distancia máxima detectable (cm)
#define HCSR04_TIMEOUT_US 23200     // Timeout para 4m (velocidad sonido ~343m/s)

// Rangos válidos para validación
#define DISTANCE_MIN 2.0f           // Distancia mínima detectable (cm)
#define DISTANCE_MAX 400.0f         // Distancia máxima detectable (cm)

// Configuración de lecturas
#define HCSR04_READ_ATTEMPTS 5      // Número de intentos de lectura
#define HCSR04_READ_DELAY_MS 50     // Delay entre intentos (ms)

#endif // SENSOR_HCSR04_H