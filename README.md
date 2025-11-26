# 🌡️ Sistema IoT LoRaWAN Multisensor - Bajo Consumo

> ### 🎓 Proyecto Académico - Sin Ánimo de Lucro
> Este proyecto ha sido desarrollado para apoyar a los investigadores y prototipos del Medialab de la Universidad de Oviedo.  

> Todo el código, documentación y diseños están disponibles gratuitamente para fines educativos y de investigación.

---

> ### ⚠️ Responsabilidad y Normativa
> Antes de desplegar el sistema, consulta el documento:  
> [docs/2_responsabilidad.md](docs/2_responsabilidad.md)  
> Uso responsable y ético.  
> **Importante:** Infórmate sobre la normativa vigente del espectro radioeléctrico en tu país para operar dispositivos LoRaWAN legalmente.

---

**Nodo sensor ambiental inteligente para ESP32 LilyGo T3 v1.6.1 con LoRaWAN**

[![PlatformIO](https://img.shields.io/badge/PlatformIO-3776AB?logo=platformio)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/ESP32-000000?logo=espressif)](https://www.espressif.com/)
[![LoRaWAN](https://img.shields.io/badge/LoRaWAN-EU868-FF6B35)](https://lora-alliance.org/)
[![TTN](https://img.shields.io/badge/The%20Things%20Network-000000)](https://www.thethingsnetwork.org/)

---

## 🎯 ¿Qué es este proyecto?

Un **sistema IoT completo y modular** que combina:
- **ESP32 LilyGo T3 v1.6.1** con ultra bajo consumo
- **LoRaWAN** para comunicación de largo alcance
- **Múltiples sensores ambientales** configurables (DHT22, BMP280, etc.)
- **Gestión avanzada de energía** con batería y carga solar
- **Integración completa** con The Things Network (TTN)

### ✨ Características principales
- 🔋 **Ultra bajo consumo**: Hasta 136 días de autonomía con batería 3000mAh
- 🔧 **Completamente modular**: Habilita/deshabilita sensores individualmente
- 📡 **LoRaWAN nativo**: Integración directa con TTN usando OTAA
- ☀️ **Carga solar**: Energía renovable integrada con panel solar
- 📊 **Payload dinámico**: Se adapta automáticamente según sensores activos
- 🖥️ **Display OLED**: Interfaz visual con feedback contextual

---

## 🚀 Inicio Rápido (5 minutos)

### 1. **Instala el entorno**
```bash
# Clona el proyecto
git clone <tu-repo>
cd low-power-project

# Abre en VS Code con PlatformIO
code .
```

### 2. **Configura tus sensores** (`config/config.h`)
```cpp
// DESCOMENTA los sensores que quieres usar
#define ENABLE_SENSOR_DHT22      // Temperatura + Humedad
#define ENABLE_SENSOR_BMP280     // Presión atmosférica
//#define ENABLE_SENSOR_DS18B20    // Temperatura adicional
//#define ENABLE_SENSOR_HCSR04     // Sensor de distancia
```

### 3. **Configura credenciales LoRaWAN** (`lorawan_config.h`)
```cpp
static const u1_t PROGMEM APPEUI[8] = {/* tus valores de TTN */};
static const u1_t PROGMEM DEVEUI[8] = {/* tus valores de TTN */};
static const u1_t PROGMEM APPKEY[16] = {/* tus valores de TTN */};
```

### 4. **Compila y sube**
```bash
pio run --target upload --upload-port COM3
```

### 5. **¡Listo!** Ve tus datos en TTN Console

---

## 📚 Documentación

| Documento | Contenido | Tiempo |
|-----------|-----------|--------|
| [**📖 Guía de Uso**](docs/6_uso.md) | Configuración completa paso a paso | 20 min |
| [**🏛️ Arquitectura**](docs/4_arquitectura.md) | Diseño técnico del sistema | 15 min |
| [**🔧 Troubleshooting**](docs/9_troubleshooting.md) | Solución de problemas comunes | 15 min |
| [**⚙️ Desarrollo**](docs/5_desarrollo.md) | Modificar y extender el código | 30 min |
| [**📡 Configuración TTN**](docs/7_configuracion_ttn.md) | Setup completo en TTN | 10 min |

---

### 📊 Configuraciones de Ejemplo

### 🌡️ **Monitoreo Ambiental Básico**
```cpp
#define ENABLE_SENSOR_DHT22      // Temperatura + Humedad
#define SEND_INTERVAL_SECONDS 300 // Cada 5 minutos
```
**Autonomía**: ~70 días | **Payload**: 7 bytes | **Campos**: Temp, Hum, Batt, Solar

### 🏭 **Estación Meteorológica Completa**
```cpp
#define ENABLE_SENSOR_DHT22      // Temp + Humedad
#define ENABLE_SENSOR_BMP280     // Presión atmosférica
#define SEND_INTERVAL_SECONDS 600 // Cada 10 minutos
```
**Autonomía**: ~62 días | **Payload**: 9 bytes | **Campos**: Temp, Hum, Pres, Batt, Solar

### 🔋 **Solo Monitoreo de Batería**
```cpp
// Todos los sensores comentados
#define ENABLE_SENSOR_NONE       // Solo batería y estado solar
#define SEND_INTERVAL_SECONDS 1800 // Cada 30 minutos
```
**Autonomía**: ~136 días | **Payload**: 4 bytes | **Campos**: Batt, Solar

---

## 📡 Decoder TTN Automático

El sistema genera automáticamente el decoder JavaScript según tus sensores configurados.

Para obtener el decoder personalizado:
1. Habilita `SHOW_TTN_DECODER true` en `config/config.h`
2. Compila y sube el código
3. Abre Serial Monitor (115200 baud)
4. Copia el código JavaScript generado
5. Pégalo en TTN Console → Payload formatters → Uplink

### 🔧 Decodificador Universal

```javascript
function decodeUplink(input) {
  var bytes = input.bytes;
  var data = {};
  var offset = 0;

  // Determinar qué campos están presentes por el tamaño del payload
  var payloadSize = bytes.length;

  // Temperatura y humedad (para DHT22, 6 bytes)
  if (payloadSize >= 6) {
    data.temperature = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
    data.humidity = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
  }

  // Presión (para DHT22 + BMP280, 8 bytes)
  if (payloadSize >= 8) {
    data.pressure = ((bytes[offset++] << 8) | bytes[offset++]) / 10.0;
  }

  // Batería (siempre presente, últimos 2 bytes)
  data.battery_voltage = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;

  return { data: data, warnings: [], errors: [] };
}
```

### 📊 Ejemplos de Payload

| Configuración | Payload (hex) | Datos Decodificados |
|---------------|---------------|-------------------|
| **Solo batería** | `0DAC` | `{"battery_voltage": 3.85}` |
| **DHT22** | `01F4 0FA0 0DAC` | `{"temperature": 25.00, "humidity": 65.20, "battery_voltage": 3.85}` |
| **DHT22 + BMP280** | `01F4 0FA0 2328 0DAC` | `{"temperature": 25.00, "humidity": 65.20, "pressure": 1013.2, "battery_voltage": 3.85}` |

---

## 🔋 Eficiencia Energética

### ⚡ Consumo por Configuración (Batería 3000mAh)

| Configuración | Intervalo | Consumo/ciclo | Autonomía | Payload |
|---------------|-----------|---------------|-----------|---------|
| **Solo batería** | 30 min | 0.015mAh | 136 días | 4 bytes |
| **DHT22 básico** | 5 min | 0.03mAh | 70 días | 7 bytes |
| **DHT22 + BMP280** | 10 min | 0.04mAh | 62 días | 9 bytes |
| **Máx. sensores** | 10 min | 0.08mAh | 26 días | 9 bytes |

### 💡 Ciclo de Operación Típico

```
┌─────────────────┐    ┌──────────────────┐    ┌──────────────┐
│   Deep Sleep    │ -> │ Procesamiento +  │ -> │   TX LoRa    │
│   (20μA, 60s)   │    │   Sensores       │    │   (120mA)    │
│                 │    │   (25mA, 8s)     │    │   (2s)       │
└─────────────────┘    └──────────────────┘    └──────────────┘
         │                        │                       │
         └─ Consumo promedio: ────┴────── 0.5mAh/día ────┘
```

### ☀️ Optimizaciones con Carga Solar

| Condiciones | Panel Solar | Consumo Neto | Autonomía |
|-------------|-------------|--------------|-----------|
| **Sin sol** | 0W | +0.5mAh/día | 70 días |
| **Sol parcial** | 1W (4h/día) | -0.2mAh/día | Ilimitada |
| **Sol completo** | 2W (8h/día) | -2.0mAh/día | Ilimitada |

### 🔌 Estados de Energía

- **🔋 Solo Batería**: Consumo continuo de batería
- **☀️ Carga Solar**: Batería se carga cuando hay sol disponible
- **🔋 + ☀️ Híbrido**: Batería + carga solar simultánea
- **⚡ Energía Ilimitada**: Con panel solar adecuado

---

## 📁 Estructura del Proyecto

```
low-power-project/
├── 📁 config/                    # ⚙️ Configuración del sistema
│   ├── config.h                  # Configuración principal
│   ├── hardware_config.h         # Configuración hardware
│   ├── lorawan_config.h          # Credenciales LoRaWAN
│   ├── lorawan_config_template.h # Plantilla configuración TTN
│   └── sensor/                   # Configuraciones específicas de sensores
│       ├── sensor_bmp280.h       # Config BMP280
│       ├── sensor_dht11.h        # Config DHT11
│       ├── sensor_dht22.h        # Config DHT22
│       ├── sensor_ds18b20.h      # Config DS18B20
│       ├── sensor_hcsr04.h       # Config HC-SR04
│       ├── sensor_none.h         # Config sin sensores
│       └── sensor_template.h     # Plantilla para nuevos sensores
├── 📁 src/                       # 📄 Código fuente principal
│   ├── main.ino                 # 🚀 Punto de entrada principal
│   ├── LoRaBoards.cpp/.h         # 📡 Configuración hardware LoRa
│   ├── pgm_board.cpp             # 🔧 Gestión LoRaWAN y OTAA
│   ├── sensor.cpp                # 🌡️ Lógica multisensor
│   ├── screen.cpp                # 🖥️ Gestión display OLED
│   └── utilities.h               # 🛠️ Utilidades comunes
├── 📁 include/                   # 📋 Headers y librerías
│   ├── LoRaBoards.h              # Headers hardware LoRa
│   ├── loramac.h                 # Headers LoRaWAN
│   └── utilities.h               # Headers utilidades
├── 📁 docs/                      # 📚 Documentación completa
│   ├── 1_guiadeinicio.md         # 🚀 Guía de inicio rápido
│   ├── 2_responsabilidad.md      # ⚖️ Responsabilidades del proyecto
│   ├── 3_hardware.md             # 🔧 Especificaciones hardware
│   ├── 4_arquitectura.md         # 🏗️ Arquitectura del sistema
│   ├── 5_desarrollo.md           # 🔧 Guía de desarrollo
│   ├── 6_uso.md                  # 📖 Guía de uso detallada
│   ├── 7_configuracion_ttn.md    # 📡 Setup TTN
│   ├── 8_ttn_decoder.md          # 📊 Decodificadores TTN
│   ├── 9_troubleshooting.md      # 🛠️ Solución de problemas
│   ├── board.jpg                 # Imagen hardware
│   └── datasheet_T3_V1.6.1.pdf   # Datasheet del dispositivo
├── platformio.ini                # ⚙️ Configuración PlatformIO
├── README.md                     # 📄 Este archivo
└── .gitignore                    # 🚫 Archivos ignorados por Git
```

## 🛠️ Desarrollo

### 📦 Dependencias PlatformIO

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    DHT sensor library for ESPx
    Adafruit BMP280 Library
    DallasTemperature
    OneWire
    U8g2
    MCCI LoRaWAN LMIC library
    XPowersLib
```

### 🧪 Testing y Debug

```bash
# Compilar proyecto
pio run

# Subir a la placa
pio run --target upload --upload-port COM3

# Monitor serial
pio device monitor

# Limpiar y reconstruir
pio run --target clean && pio run
```

### 🔍 Debug Avanzado

```cpp
// En config/config.h
#define ENABLE_SERIAL_LOGS true
#define LOG_LEVEL 2              // 0: ninguno, 1: básico, 2: detallado
#define SHOW_TTN_DECODER true    // Genera decoder TTN automáticamente
```

---

## 📈 Dashboard TTN

### Visualización de Datos
```
🌡️ Temperatura: 23.45°C
💧 Humedad: 65.30%
🌪️ Presión: 1013.25 hPa
🔋 Batería: 3.85V (85%)
☀️ Solar: Cargando
📡 RSSI: -95dBm, SNR: 8.5dB
```

### JSON Decodificado
```json
{
  "temperature": 23.45,
  "humidity": 65.30,
  "pressure": 1013.25,
  "battery_voltage": 3.85,
  "solar_charging": true
}
```

---

## 🔧 Desarrollo y Extensiones

### 🤓 **Para Estudiantes**
Este proyecto es perfecto para aprender:
- **IoT desde cero**: Conceptos básicos hasta implementación
- **ESP32**: Programación embebida en C++
- **LoRaWAN**: Redes de largo alcance
- **Sensores**: Interfaces I2C, OneWire, GPIO
- **Energía**: Gestión de consumo y carga solar

### 🚀 **Agregar Nuevo Sensor**
```cpp
// 1. Define en config.h
#define ENABLE_SENSOR_MI_SENSOR

// 2. Implementa sensor_mi_sensor.cpp
bool sensor_mi_sensor_read_all(sensor_data_t* data) {
    data->mi_variable = leer_sensor();
    data->battery = readBatteryVoltage();
    return true;
}

// 3. ¡Listo! El sistema lo detecta automáticamente
```

---

## 📞 Soporte

### 🆘 **¿Problemas?**
1. **Lee primero**: [Guía de troubleshooting](docs/9_troubleshooting.md)
2. **Configuración**: [Guía de uso](docs/6_uso.md)
3. **TTN Setup**: [Configuración TTN](docs/7_configuracion_ttn.md)

### 🐛 **Reportar Bugs**
```markdown
**Entorno:**
- Hardware: LilyGo T3 v1.6.1
- Sensores activos: DHT22, BMP280
- TTN Region: EU868

**Síntomas:**
- Descripción del problema
- Logs del Serial Monitor
- Estado en TTN Console
```

---

## 🛠️ Hardware Requerido

### 📋 Lista de Componentes

| Componente | Especificación | Notas |
|------------|----------------|-------|
| **Placa principal** | LilyGo T3 v1.6.1 | ESP32 + LoRa SX1276 + PMU AXP2101 |
| **Antena** | 868MHz LoRaWAN | Incluida con la placa |
| **Batería** | LiPo 3.7V 3000mAh | Recomendado para máxima autonomía |
| **Panel solar** | 5V USB-C | Opcional, carga automática |
| **Sensores** | Según configuración | DHT22, BMP280, DS18B20, HC-SR04 |

### 🔌 Conexiones de Sensores

| Sensor | Pines ESP32 | Alimentación | Notas |
|--------|-------------|--------------|-------|
| **DHT22** | GPIO 13 (DATA), GPIO 12 (POWER) | 3.3V | Control individual de alimentación |
| **BMP280** | I2C: GPIO 17 (SDA), 18 (SCL) | 3.3V | Dirección I2C: 0x76 o 0x77 |
| **DS18B20** | GPIO 14 (OneWire) | 3.3V | Requiere resistor pull-up 4.7KΩ |
| **HC-SR04** | GPIO 25 (TRIG), 26 (ECHO) | 5V | Desde PMU AXP2101 |
| **OLED SSD1306** | I2C: GPIO 17 (SDA), 18 (SCL) | 3.3V | Dirección I2C: 0x3C |

### ⚡ Diagrama de Conexiones

```
ESP32 LilyGo T3 v1.6.1
├── 🔌 USB-C (Programación + Carga Solar)
├── 📡 Antena LoRa 868MHz
├── 🔋 Batería LiPo 3.7V
├── 🌡️ DHT22 (GPIO 13/12)
├── 🌪️ BMP280 (I2C GPIO 17/18)
├── 🌡️ DS18B20 (GPIO 14)
├── 📏 HC-SR04 (GPIO 25/26)
└── 🖥️ OLED (I2C GPIO 17/18)
```

---

## 📄 Licencia

**MIT License** - Libre para uso educativo

---

**¡Bienvenido al mundo del IoT con LoRaWAN!** 🌟

*[Empieza aquí](docs/6_uso.md)* | *[Arquitectura técnica](docs/4_arquitectura.md)* | *[Solución de problemas](docs/9_troubleshooting.md)*

---
**📅 Actualizado: Noviembre 2025** | **🔧 LilyGo T3 v1.6.1** | **📡 LoRaWAN EU868**

# LILYGO LoRa T3 Low Power PINOUT

[Datasheet](docs/datasheet_T3_V1.6.1.pdf)

![Board](docs/board.jpg)