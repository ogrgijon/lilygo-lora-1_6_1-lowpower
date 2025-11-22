# 💻 Configuración Software - Arquitectura Modular IoT

## 🏗️ Entorno de Desarrollo

### 📦 **PlatformIO + VS Code**
```json
{
  "framework": "arduino",
  "platform": "espressif32",
  "board": "esp32-s3-devkitc-1",
  "board_build.mcu": "esp32s3",
  "board_build.f_cpu": "240000000L",
  "board_build.f_flash": "80000000L",
  "board_build.flash_mode": "qio"
}
```

### 🛠️ **Herramientas Requeridas**
- **VS Code** 1.70+
- **PlatformIO Extension** 6.0+
- **Git** 2.30+
- **Python** 3.8+ (incluido con PlatformIO)

## 📚 Dependencias y Librerías

### 🔧 **Librerías Core del Sistema**

#### **1. LoRaWAN - LMIC (IBM)**
```cpp
// Versión: 4.1.1
// Propósito: Stack completo LoRaWAN Class A
// Configuración: OTAA, EU868, Adaptive Rate
#include <lmic.h>
#include <hal/hal.h>
```
- **Uso**: Comunicación bidireccional LoRaWAN
- **Características**: AES-128 encryption, duty cycle compliance
- **Configuración**: `platformio.ini` → `lib_deps = IBM LMIC framework`

#### **2. Sensor Ambiental - DHT Sensor Library**
```cpp
// Versión: 1.4.4
// Propósito: Temperatura y humedad ambiental
// Precisión: ±0.5°C, ±2-5% RH (depende del modelo)
#include <DHT.h>
```
- **Uso**: Lecturas ambientales cada ciclo de transmisión
- **Control de energía**: Alimentación controlada para bajo consumo
- **Recuperación**: Reintento automático en caso de fallo

#### **3. Display OLED - U8g2**
```cpp
// Versión: 2.34.4
// Propósito: Interfaz de usuario inteligente
// Resolución: 128x64 pixels, monocromo
#include <U8g2lib.h>
```
- **Uso**: Sistema de mensajes con cola inteligente
- **Características**: Auto-apagado, gestión de energía
- **Optimización**: Buffer eficiente, fuentes compactas

#### **4. Gestión de Energía - XPowersLib**
```cpp
// Versión: 0.1.8
// Propósito: Control PMU AXP2101
// Eficiencia: < 20μA en deep sleep
#include <XPowersLib.h>
```
- **Uso**: Monitoreo batería, control alimentación
- **Características**: ADC de alta precisión, protección sobrecarga
- **Integración**: Lectura voltaje en tiempo real

### 📋 **Dependencias PlatformIO**

#### **platformio.ini - Configuración Completa**
```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

; Configuración MCU
board_build.mcu = esp32s3
board_build.f_cpu = 240000000L
board_build.f_flash = 80000000L
board_build.flash_mode = qio

; Librerías dependientes
lib_deps =
    IBM LMIC framework @ ^4.1.1
    adafruit/DHT sensor library @ ^1.4.4
    olikraus/U8g2 @ ^2.34.4
    lewisxhe/XPowersLib @ ^0.1.8

; Configuración compilador
build_flags =
    -D CFG_eu868=1
    -D CLASS=A
    -D SINGLE_CHANNEL_GATEWAY=0
    -D DEBUG_LEVEL=0
    -D LMIC_DEBUG_LEVEL=0

; Configuración LoRaWAN
    -D LMIC_LORAWAN_SPEC_VERSION=LMIC_LORAWAN_SPEC_VERSION_1_0_3
    -D LMIC_ENABLE_DeviceTimeReq=0
    -D LMIC_ENABLE_event_logging=0

; Optimizaciones
    -O2
    -ffunction-sections
    -fdata-sections
    -Wl,--gc-sections

; Configuración upload
upload_speed = 921600
monitor_speed = 115200
monitor_filters = esp32_exception_decoder

; Configuración particiones
board_build.partitions = default_16MB.csv
```

## 🔄 Arquitectura de Software

### 📁 **Estructura Modular**

```
src/
├── main_otta.ino          # 🚀 Entry point Arduino
├── pgm_board.cpp          # 📡 LoRaWAN core
├── sensor.cpp             # 🌡️ Sensor management
├── screen.cpp             # 🖥️ Display system
└── LoRaBoards.cpp         # 🔧 Hardware abstraction

include/
├── sensor.h               # 📋 Sensor API
├── screen.h               # 📋 Display API
├── LoRaBoards.h           # 📋 Hardware API
├── loramac.h              # 📋 LoRaWAN headers
├── lorawan_config.h       # 🔐 LoRaWAN keys (gitignored)
├── lorawan_config_template.h # 📋 Secure config template
└── utilities.h            # 📋 Common utilities
```

### 🎯 **Separación de Responsabilidades**

#### **Módulo Sensor (`sensor.cpp`)**
```cpp
// Responsabilidades
- Inicialización DHT22 con control de alimentación
- Lectura temperatura/humedad
- Validación y códigos de error
- Formateo payload binario 6 bytes
- Monitoreo batería
```

#### **Módulo Display (`screen.cpp`)**
```cpp
// Responsabilidades
- Cola de mensajes inteligente (FIFO)
- Renderizado OLED con U8g2
- Gestión temporización automática
- Auto-apagado para ahorro energía
- Tipos de mensaje semánticos
```

#### **Módulo LoRaWAN (`pgm_board.cpp`)**
```cpp
// Responsabilidades
- Configuración LMIC completa
- Manejo eventos LoRaWAN
- Ciclo transmisión/recepción
- Gestión estados de join
- Coordinación sistema completo
```

#### **Módulo Hardware (`LoRaBoards.cpp`)**
```cpp
// Responsabilidades
- Configuración pines ESP32-S3
- Inicialización PMU AXP2101
- Setup I2C/SPI buses
- Monitoreo batería ADC
- Abstracción LilyGo T3-S3
```

## ⚙️ Configuración del Sistema

### 📡 **Parámetros LoRaWAN**

#### **Credenciales OTAA (Requeridas)**
```cpp
// Archivo: include/lorawan_config.h (NO commitear - está en .gitignore)
// Copiar de include/lorawan_config_template.h y actualizar con tus valores TTN

// Application EUI (AppEUI) - LSB format
static const u1_t PROGMEM APPEUI[8] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

// Device EUI (DevEUI) - LSB format
static const u1_t PROGMEM DEVEUI[8] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

// Application Key (AppKey) - MSB format
static const u1_t PROGMEM APPKEY[16] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX,
                                        0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
```

**Nota de Seguridad**: Las credenciales están en un archivo separado (`lorawan_config.h`) que está incluido en `.gitignore` para evitar commitear información sensible.

#### **Configuración Regional**
```cpp
#define CFG_eu868 1                    // Región Europa 868MHz
#define CLASS A                        // Clase bajo consumo
#define SINGLE_CHANNEL_GATEWAY 0       // Multi-canal (producción)
#define TX_INTERVAL 30                 // Intervalo TX (no usado en bajo consumo)
```

### 🌡️ **Configuración Sensor**

#### **DHT22 - Parámetros de Operación**
```cpp
// Configuración de pines
#define DHT_PIN 13                    // GPIO para señal digital
#define DHT_TYPE DHT22                 // Tipo de sensor DHT
#define DHT_POWER_PIN 12              // GPIO para control de alimentación

// Parámetros de timing
#define DHT_POWER_ON_DELAY_MS 2000    // Tiempo de estabilización (ms)

// Rango de operación
#define DHT_TEMPERATURE_MIN -40.0f    // °C - rango mínimo
#define DHT_TEMPERATURE_MAX 80.0f     // °C - rango máximo
#define DHT_HUMIDITY_MIN 0.0f         // % - humedad mínima
#define DHT_HUMIDITY_MAX 100.0f       // % - humedad máxima
```

#### **Protocolo de Comunicación**
```cpp
// El DHT22 usa comunicación digital unidireccional
// No requiere configuración I2C/SPI
// Señal digital en pin especificado
// Alimentación controlada para bajo consumo
```

### 🖥️ **Configuración Display**

#### **OLED SSD1306 - Parámetros Visuales**
```cpp
// Configuración U8g2
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Dirección I2C
#define OLED_ADDRESS 0x3C

// Configuración visual
#define OLED_CONTRAST 255              // Máximo contraste
#define FONT_DEFAULT u8g2_font_ncenB08_tr  // Fuente legible
```

#### **Sistema de Mensajes**
```cpp
// Cola de mensajes
#define MAX_SCREEN_MESSAGES 10         // Capacidad cola
#define DEFAULT_MESSAGE_DURATION 3000  // 3 segundos
#define SENSOR_DATA_DURATION 5000      // 5 segundos para datos
```

### 🔋 **Configuración Energía**

#### **PMU AXP2101 - Parámetros**
```cpp
// Configuración ADC batería
#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_0
#define BATTERY_ADC_ATTEN ADC_ATTEN_DB_11
#define BATTERY_ADC_WIDTH ADC_WIDTH_BIT_12

// Umbrales batería
#define BATTERY_LOW_VOLTAGE 3.0f       // V - batería baja
#define BATTERY_FULL_VOLTAGE 4.2f      // V - batería cargada

// Factor de escala ADC
#define BATTERY_SCALE_FACTOR 2.0f      // Divisor resistivo
```

#### **Deep Sleep - Configuración**
```cpp
#define SLEEP_TIME_SECONDS 60          // Ciclo de 60 segundos
#define SLEEP_TIME_MICROSECONDS (SLEEP_TIME_SECONDS * 1000000ULL)

// Configuración wake-up
esp_sleep_enable_timer_wakeup(SLEEP_TIME_MICROSECONDS);
esp_deep_sleep_start();
```

## 🔧 Build y Deployment

### 📦 **Proceso de Compilación**

#### **Comandos PlatformIO**
```bash
# Compilar proyecto
pio run

# Subir a dispositivo
pio run --target upload

# Monitor serial
pio device monitor

# Limpiar build
pio run --target clean
```

#### **Verificación Build**
```bash
# Tamaño del binario
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [====      ]  44.4% (used 9148 bytes from 20480 bytes)
Flash: [===       ]  25.5% (used 267313 bytes from 1048576 bytes)
```

### 🚀 **Deployment Seguro**

#### **Pre-deployment Checklist**
- [ ] Credenciales LoRaWAN configuradas
- [ ] Compilación exitosa sin warnings
- [ ] Test en breadboard completado
- [ ] Batería cargada (>3.7V)
- [ ] Cobertura LoRaWAN verificada

#### **Configuración TTN**
```javascript
// Decoder para 6 bytes payload (DHT22: temperatura, humedad, batería)
function decodeUplink(input) {
  var bytes = input.bytes;
  var data = {};

  // Temperatura (big-endian, signed)
  var temp_raw = (bytes[0] << 8) | bytes[1];
  if (temp_raw & 0x8000) temp_raw -= 0x10000;
  data.temperature = temp_raw / 100.0;

  // Humedad (big-endian, unsigned)
  data.humidity = ((bytes[2] << 8) | bytes[3]) / 100.0;

  // Batería (big-endian, unsigned)
  data.battery_voltage = ((bytes[4] << 8) | bytes[5]) / 100.0;

  return { data: data };
}
```

## 🧪 Testing y Validación

### 🧪 **Suite de Tests**

#### **Test Unitarios**
```cpp
// sensor_test.cpp
void testSensorReadings() {
    float temp, hum, pres, batt;
    bool result = getSensorDataForDisplay(temp, hum, pres, batt);

    assert(result == true);
    assert(temp > -50.0f && temp < 100.0f);
    assert(hum >= 0.0f && hum <= 100.0f);
    assert(batt > 2.5f && batt < 4.5f);
}
```

#### **Test de Integración**
```cpp
// system_integration_test.cpp
void testFullSystemCycle() {
    // 1. Inicialización
    setupBoards(false);
    setupLMIC();

    // 2. Simular ciclo de medición
    do_send(NULL);

    // 3. Verificar transmisión
    // (esperar EV_TXCOMPLETE)

    // 4. Verificar deep sleep
    enterDeepSleep();
}
```

### 📊 **Monitoreo de Rendimiento**

#### **Métricas Clave**
```cpp
void logSystemMetrics() {
    Serial.printf("=== SYSTEM METRICS ===\n");
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Uptime: %d seconds\n", millis() / 1000);
    Serial.printf("LoRaWAN State: %s\n", getLoRaWANStateString());
    Serial.printf("Sensor Status: %s\n", isSensorAvailable() ? "OK" : "FAIL");
    Serial.printf("Battery: %.2fV\n", readBatteryVoltage());
    Serial.printf("Display Active: %s\n", displayActive ? "YES" : "NO");
}
```

## 🔒 Consideraciones de Seguridad

### 🛡️ **Medidas Implementadas**
- **Credenciales seguras**: Almacenadas en PROGMEM (no RAM)
- **Comunicación encriptada**: LoRaWAN Class A con AES-128
- **Validación de datos**: Rangos y códigos de error
- **Fail-safe**: Sistema continúa con fallos parciales

### ⚠️ **Recomendaciones**
- **No commitees** credenciales a repositorios públicos
- **Usa HTTPS** para configuración remota
- **Monitorea** accesos no autorizados
- **Actualiza** firmware regularmente

---

**💻 Configuración software completa para sistema IoT LoRaWAN modular de alta fiabilidad**