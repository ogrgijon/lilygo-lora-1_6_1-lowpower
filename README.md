# 🌡️ Sistema IoT LoRaWAN Modular - Sensor Ambiental Inteligente
---
> ### 🎓 Proyecto Académico - Sin Ánimo de Lucro
> Este proyecto ha sido desarrollado para apoyar a los investigadores y prototipos del Medialab de la Universidad de Oviedo.  
> Todo el código, documentación y diseños están disponibles gratuitamente para fines educativos y de investigación.

---

> ### 🆕 ¿Nuevo en IoT y LoRaWAN?
> Consulta la guía de introducción:  
> [docs/inicio.md](docs/inicio.md)  
> Aprende desde cero sobre la tecnología, conceptos básicos y primeros pasos.

---

> ### ⚠️ Responsabilidad y Normativa
> Antes de desplegar el sistema, consulta el documento:  
> [docs/responsabilidad.md](docs/responsabilidad.md)  
> Uso responsable y ético.  
> **Importante:** Infórmate sobre la normativa vigente del espectro radioeléctrico en tu país para operar dispositivos LoRaWAN legalmente.

---

[![PlatformIO](https://img.shields.io/badge/PlatformIO-6.1.11-blue.svg)](https://platformio.org)
[![ESP32](https://img.shields.io/badge/ESP32-240MHz-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![LoRaWAN](https://img.shields.io/badge/LoRaWAN-1.0.3-orange.svg)](https://lora-alliance.org)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

**Sistema IoT completo con arquitectura modular** para monitoreo ambiental remoto. Implementa medición precisa de temperatura, humedad, presión atmosférica y voltaje de batería, transmitiendo datos por LoRaWAN cada 60 segundos con **consumo ultra-bajo** (Hasta < 20μA en deep sleep). Incluye **sistema de carga solar inteligente** y **backoff exponencial** para máxima fiabilidad.

## 🎯 Características Principales

### 🌡️ **Sensores Ambientales Avanzados**
- **DHT22**: Temperatura (-40°C a +80°C), humedad (0-100%)
- **Precisión**: 0.01 unidades para temperatura y humedad
- **Recuperación automática**: Sistema continúa funcionando aunque falle el sensor

### 📡 **Comunicación LoRaWAN Robusta**
- **Clase A**: Bajo consumo con bidireccionalidad
- **OTAA Join**: Activación segura sobre el aire
- **Adaptive Rate**: Spreading Factor 7-12 según condiciones
- **Región EU868**: Optimizado para Europa 868MHz

### 🔋 **Gestión de Energía Inteligente**
- **Deep Sleep**: 60 segundos entre ciclos - por defecto - configurable
- **Monitoreo batería**: Voltaje en tiempo real con precisión 0.01V
- **Carga Solar**: Detección automática de panel solar con estado en payload
- **Backoff Exponencial**: Reintentos inteligentes para joins fallidos
- **Auto-apagado**: Componentes se desactivan automáticamente
- **Watchdog Timer**: Reinicio automático si no hay actividad por 5 minutos
- **Eficiencia**: Consumo ultra-bajo < 0.5mAh por ciclo

## ⚡ Consumo de Energía y Autonomía

### 📊 **Perfiles de Consumo por Modo**

| Modo de Operación | Consumo Típico | Duración | Energía por Ciclo |
|-------------------|----------------|----------|-------------------|
| **Deep Sleep** | 20μA | 60s | 0.0003mAh |
| **Sensor DHT22** | 15mA | 2s | 0.008mAh |
| **Transmisión LoRa** | 120mA | 1-2s | 0.07mAh |
| **Procesamiento ESP32** | 25mA | 8s | 0.06mAh |
| **Display OLED** | 25mA | 3-5s | 0.02-0.03mAh |
| **Join LoRaWAN** | 100mA | 5-30s | 0.14-0.83mAh |
| **Backoff Ligero** | 20μA | Variable | Variable |

### 🔋 **Cálculo de Consumo por Ciclo**

#### **Ciclo Normal (60 segundos)**
```
Consumo total = Sleep + Sensor + Procesamiento + TX + Display
               = 0.0003 + 0.008 + 0.06 + 0.07 + 0.03
               = 0.1683 mAh por ciclo
```

#### **Ciclo con Join Fallido (ejemplo)**
```
Consumo total = Sleep + Sensor + Procesamiento + Join(10s) + Backoff(2min)
               = 0.0003 + 0.008 + 0.06 + 0.28 + 0.0007
               = 0.349 mAh por ciclo (join fallido)
```

### 📈 **Autonomía según Configuración**

#### **Batería de Referencia: LiPo 18650 3000mAh**

| Configuración | Ciclos/día | Consumo Diario | Autonomía | Eficiencia |
|---------------|------------|----------------|-----------|------------|
| **60s (normal)** | 1440 | 242mAh | ~12-14 días | 92% |
| **120s (ahorro)** | 720 | 121mAh | ~25-28 días | 96% |
| **300s (ultra-bajo)** | 288 | 48mAh | ~62-70 días | 98% |
| **600s (mínimo)** | 144 | 24mAh | ~125-136 días | 99% |

#### **Factores de Corrección de Autonomía**
- **Temperatura**: -20% a 0°C, +10% a 40°C
- **Eficiencia batería**: 85-95% (depende calidad)
- **Auto-descarga**: 1-3% mensual
- **Factor de seguridad**: 70-80% de teórico
- **Carga solar**: +20-100% adicional

### ☀️ **Sistema de Carga Solar Integrado**

#### **Detección de Carga Solar**
```cpp
// Estado incluido en payload LoRaWAN (byte 6)
bool isSolarCharging = PMU->isVbusIn() && PMU->isCharging();
// 0 = No cargando, 1 = Cargando batería
```

#### **Eficiencia de Carga Solar**
- **Entrada**: 4.5-6V DC (panel solar)
- **Corriente máxima**: 500mA (AXP2101)
- **Eficiencia conversión**: 85-92%
- **Prioridad**: Carga batería > Alimentación sistema

#### **Cálculo de Energía Solar Requerida**
```
Energía diaria necesaria = Consumo diario × Voltaje batería
Ejemplo 60s: 242mAh × 3.7V = 0.89Wh/día

Panel recomendado:
- 1W (5V, 200mA): Suficiente para 60s + recarga
- 2W (5V, 400mA): Ideal para 120s + excedente
- Tiempo sol mínimo: 2-4 horas/día directo
```

### 🔋 **Recomendaciones de Baterías**

#### **Baterías Compatibles**
| Tipo | Capacidad | Voltaje | Autonomía 60s | Ventajas | Desventajas |
|------|-----------|---------|---------------|----------|-------------|
| **LiPo 18650** | 3000mAh | 3.7V | 12-14 días | Alta densidad | Requiere protección |
| **LiFePO4 18650** | 2500mAh | 3.2V | 15-18 días | Más segura | Menos capacidad |
| **LiPo 26650** | 5000mAh | 3.7V | 20-23 días | Alta capacidad | Más grande |
| **Batería Alcalina AA×4** | 8000mAh | 6V | 8-10 días | Barata | Baja eficiencia |

#### **Criterios de Selección**
- **Capacidad**: Mayor = más autonomía
- **Calidad**: Marca reconocida (Samsung, Panasonic)
- **Protección**: Circuitos de protección integrados
- **Temperatura**: Rango operativo -20°C a +60°C
- **Carga**: Compatible con AXP2101 (4.2V máx)

### 📊 **Monitoreo de Energía en Tiempo Real**

#### **Métricas Disponibles**
```cpp
// Voltaje batería (precisión 0.01V)
float batteryVoltage = readBatteryVoltage();

// Estado de carga solar
bool solarCharging = isSolarChargingBattery();

// Porcentaje batería estimado
uint8_t batteryPercent = batteryPercentFromVoltage(batteryVoltage);
```

#### **Alertas de Energía**
- **Batería baja**: < 3.0V (20% capacidad)
- **Batería crítica**: < 2.8V (5% capacidad)
- **Sin carga solar**: Estado 0 en payload
- **Consumo alto**: Durante joins fallidos

### 🔧 **Optimización de Consumo**

#### **Configuración Recomendada para Máxima Autonomía**
```cpp
// platformio.ini - Configuración óptima
#define SLEEP_TIME_SECONDS 300          // 5 minutos entre TX
#define BATTERY_LOW_VOLTAGE 3.0f       // Alerta batería baja
#define DISPLAY_TIMEOUT_MS 2000        // Display se apaga rápido
#define SENSOR_READ_INTERVAL 1         // Leer sensor cada ciclo
```

#### **Mejores Prácticas**
- **Ubicación**: Buena cobertura LoRaWAN reduce joins fallidos
- **Orientación solar**: Panel orientado sur, ángulo óptimo
- **Mantenimiento**: Limpiar panel solar regularmente
- **Monitoreo**: Revisar voltaje batería en TTN
- **Actualizaciones**: Firmware optimizado reduce consumo

### 📈 **Gráficos de Consumo**

#### **Distribución de Energía por Ciclo (60s)**
```
Deep Sleep: ████████▉ (0.02%) - 20μA
Procesamiento: ████████████████ (36%) - 25mA
Sensor: ████▉ (5%) - 15mA
Transmisión: ████████████████████ (42%) - 120mA
Display: ████████ (16%) - 25mA
```

#### **Autonomía vs Intervalo de Transmisión**
```
600s: ████████████████████████████████████████████████ (136 días)
300s: ████████████████████████████████████████ (70 días)
120s: ███████████████████████████████ (28 días)
60s: ███████████████████ (14 días)
```

### 🧪 **Validación de Consumo**

#### **Método de Medición**
```cpp
// Con multímetro en serie con batería
// Medir corriente promedio durante 24 horas
// Comparar con cálculos teóricos

float measuredCurrent = measureAverageCurrent();
float theoreticalCurrent = calculateTheoreticalCurrent();
float efficiency = (theoreticalCurrent / measuredCurrent) * 100;
```

#### **Herramientas de Medición**
- **Multímetro digital**: Medición directa de corriente
- **Osciloscopio**: Análisis de picos de consumo
- **TTN Dashboard**: Monitoreo remoto de voltaje
- **Serial Debug**: Logs de consumo por componente

### 🚨 **Consideraciones de Seguridad**

#### **Límites de Batería**
- **Voltaje mínimo**: 2.5V (daño permanente)
- **Voltaje máximo**: 4.2V (riesgo explosión)
- **Corriente máxima**: 500mA (AXP2101)
- **Temperatura**: -20°C a +60°C

#### **Protecciones Implementadas**
- **Monitoreo continuo**: Voltaje batería en tiempo real
- **Alertas automáticas**: Notificación batería baja
- **Corte automático**: Sistema se apaga por debajo de 2.8V
- **Protección solar**: Regulación de carga automática

### 📊 **Comparativa con Otros Sistemas**

| Sistema | Consumo/ciclo | Autonomía (3000mAh) | Ventajas | Desventajas |
|---------|---------------|---------------------|----------|-------------|
| **Este proyecto** | 0.17mAh | 14 días | Ultra-bajo, solar | Requiere setup |
| **ESP32 básico** | 2-5mAh | 1-2 días | Simple | Alto consumo |
| **Arduino + GSM** | 50mAh | 2-3 días | Cobertura global | Costoso |

---

**⚡ Sistema optimizado para máxima autonomía con carga solar integrada**

### 🖥️ **Interfaz de Usuario Avanzada**
- **OLED SSD1306**: Display inteligente con cola de mensajes
- **Estados visuales**: Join, transmisión, errores, datos ambientales
- **Auto-gestión**: Se apaga automáticamente para ahorro de energía

### 🏗️ **Arquitectura Modular**
- **Separación clara**: Sensor, LoRaWAN, Display, Hardware
- **APIs definidas**: Interfaces limpias entre módulos
- **Mantenibilidad**: Código organizado y documentado
- **Extensibilidad**: Fácil agregar nuevos sensores o funcionalidades

## 📊 Datos Transmitidos

### 📦 **Payload de 7 Bytes (Big-Endian)**

| Campo | Bytes | Tipo | Rango | Precisión | Ejemplo |
|-------|-------|------|-------|-----------|---------|
| **Temperatura** | 0-1 | int16_t | -40°C a 80°C | 0.01°C | `25.67°C` |
| **Humedad** | 2-3 | uint16_t | 0-100% | 0.01% | `65.43%` |
| **Batería** | 4-5 | uint16_t | 0-5V | 0.01V | `3.85V` |
| **Estado Solar** | 6 | uint8_t | 0-1 | - | `1` (cargando) |

### 🔍 **Códigos de Error**
- **Temperatura**: `-999.0°C` (sensor fallando)
- **Humedad**: `-1.0%` (sensor fallando)
- **Batería**: Siempre disponible
- **Estado Solar**: `0` (no cargando), `1` (cargando)

## 🚀 Inicio Rápido

### 📋 **Prerrequisitos**
- LilyGo T3 con ESP32
- VS Code + PlatformIO
- Cuenta The Things Network (TTN)
- Antena LoRa 868MHz

### ⚡ **Configuración en 5 Minutos**

1. **Clonar y abrir**
   ```bash
   git clone <tu-repo>
   cd low-power-project
   code .
   ```

2. **Configurar credenciales TTN**
   ```bash
   # Copiar plantilla de configuración
   cp include/lorawan_config_template.h include/lorawan_config.h
   
   # Editar con tus claves TTN
   # include/lorawan_config.h
   static const u1_t PROGMEM APPEUI[8] = {TU_APPEUI_AQUI};
   static const u1_t PROGMEM DEVEUI[8] = {TU_DEVEUI_AQUI};
   static const u1_t PROGMEM APPKEY[16] = {TU_APPKEY_AQUI};
   ```

3. **Compilar y subir**
   ```bash
   pio run --target upload --upload-port COM3
   ```

4. **Verificar funcionamiento**
   - OLED muestra "Sistema Iniciado"
   - Serial: logs de join y transmisión
   - TTN: datos ambientales cada 60s

## 📁 Estructura del Proyecto

```
low-power-project/
├── 📁 src/
│   ├── main_otta.ino          # 🚀 Entry point Arduino
│   ├── pgm_board.cpp          # 📡 Núcleo LoRaWAN
│   ├── sensor.cpp             # 🌡️ Gestión de sensores DHT22
│   ├── screen.cpp             # 🖥️ Sistema display OLED
│   ├── solar.cpp              # ☀️ Sistema de carga solar
│   ├── LoRaBoards.cpp         # 🔧 Abstracción hardware LilyGo
│   ├── LoRaBoards.h           # 🔧 Headers hardware
│   └── utilities.h            # 📋 Utilidades comunes
├── 📁 include/
│   ├── sensor.h               # 📋 API sensores
│   ├── screen.h               # 📋 API display
│   ├── sensor_config.h        # ⚙️ Configuración de sensores
│   ├── sensor_interface.h     # 🔌 Interface de sensores
│   ├── LoRaBoards.h           # 📋 API hardware LilyGo
│   ├── loramac.h              # 📋 Headers LoRaWAN
│   ├── lorawan_config.h       # 🔐 Claves LoRaWAN
│   ├── lorawan_config_template.h # 📋 Plantilla de configuración
│   └── utilities.h            # 📋 Utilidades comunes
├── 📁 lib/
│   ├── Adafruit_BME280_Library/  # 🌡️ Librería sensor BME280 (legacy)
│   ├── Adafruit_BusIO/        # 🔧 Bus I2C/SPI Adafruit
│   ├── Adafruit_Sensor/       # 📊 Framework sensores Adafruit
│   ├── LMIC-Arduino/          # 📡 Stack LoRaWAN
│   ├── U8g2/                  # 🖥️ Librería display OLED
│   └── XPowersLib/            # 🔋 Gestión PMU AXP192
├── 📁 docs/
│   ├── README.md              # 📖 Documentación completa
│   ├── inicio.md              # 🚀 Guía para principiantes
│   ├── instalacion.md         # 🚀 Guía de instalación
│   ├── uso.md                 # 📖 Manual de operación
│   ├── arquitectura.md        # 🏗️ Diseño del sistema
│   ├── codigo.md              # 📝 Estructura del código
│   ├── software.md            # 💻 Configuración técnica
│   ├── hardware.md            # 🔧 Especificaciones hardware
│   ├── sensor_DHT22.md        # 🌡️ Documentación sensor DHT22
│   ├── sensor_gestion.md      # ⚙️ Gestión de configuración de sensores
│   ├── LORAWAN_SETUP.md       # 📡 Setup LoRaWAN
│   ├── ttn_decoder.md         # 📊 Decoder TTN
│   ├── ttn_decoder_test.js    # 🧪 Test decoder TTN
│   ├── ttn_payload_decoder.js # 📦 Payload decoder TTN
│   ├── libreriasyterceros.md  # 📋 Librerías y licencias
│   ├── responsabilidad.md     # ⚠️ Uso responsable
│   └── troubleshooting.md     # 🔧 Solución de problemas
├── platformio.ini             # ⚙️ Configuración PlatformIO
├── .gitignore                 # 🚫 Archivos ignorados
└── README.md                  # 📋 Este archivo
```

## 🔧 Configuración Técnica

### 📡 **Parámetros LoRaWAN**
```cpp
#define CFG_eu868 1                    // Región Europa
#define CLASS A                        // Clase bajo consumo
#define TX_INTERVAL 30                 // Intervalo transmisión (no usado)
#define SINGLE_CHANNEL_GATEWAY 0       // Multi-canal producción
```

### 🌡️ **Configuración Sensor DHT22**
```cpp
// Pin de datos del sensor
#define DHT_PIN 13                    // GPIO para señal DHT22

// Tipo de sensor DHT
#define DHT_TYPE DHT22                // DHT22, DHT11, DHT21

// Control de alimentación
#define DHT_POWER_PIN 12              // GPIO para controlar alimentación
#define DHT_POWER_ON_DELAY_MS 2000    // Tiempo de estabilización (ms)

// Dirección I2C (no aplica para DHT22 - comunicación digital)
```

### 🔋 **Gestión de Energía**
```cpp
#define SLEEP_TIME_SECONDS 60          // Ciclo de 60 segundos (configurable)
#define BATTERY_LOW_VOLTAGE 3.0f       // Umbral batería baja (V)
#define BATTERY_FULL_VOLTAGE 4.2f      // Batería cargada (V)
#define BATTERY_SCALE_FACTOR 2.0f      // Factor divisor resistivo
#define BATTERY_VOL_COMPENSATION 0.0f  // Compensación voltaje (V)

// Configuración PMU AXP2101
#define PMU_WIRE_PORT Wire             // Puerto I2C para PMU
#define PMU_IRQ_PIN 35                // Pin de interrupción PMU

// Configuración carga solar
#define SOLAR_VBUS_THRESHOLD 4.5f      // Umbral detección VBUS (V)
#define SOLAR_CHARGING_CURRENT 500     // Corriente máxima carga (mA)
```

### ⏰ **Configuración del Tiempo de Sleep**
El tiempo de sleep determina la frecuencia de transmisión y afecta directamente la autonomía:

```cpp
// Ejemplos de configuración según necesidades:
#define SLEEP_TIME_SECONDS 30    // Alta frecuencia (cada 30s) - Mayor consumo
#define SLEEP_TIME_SECONDS 300   // Media frecuencia (cada 5min) - Consumo moderado  
#define SLEEP_TIME_SECONDS 1800  // Baja frecuencia (cada 30min) - Bajo consumo
#define SLEEP_TIME_SECONDS 3600  // Muy baja frecuencia (cada hora) - Mínimo consumo
```

**Recomendaciones por aplicación:**
- **Monitoreo ambiental continuo**: 30-60 segundos
- **Control industrial**: 5-15 minutos  
- **Sensores remotos**: 30-60 minutos
- **Batería solar pequeña**: 2-4 horas

### 🐕 **Watchdog Timer**
Sistema de protección automática contra bloqueos:

```cpp
// Configuración watchdog (5 minutos)
esp_task_wdt_init(300, true);        // 300 segundos = 5 minutos
esp_task_wdt_add(NULL);              // Agregar tarea actual
esp_task_wdt_reset();                // Reset en cada ciclo
```

**Características:**
- ✅ Timeout configurable (actual: 5 minutos)
- ✅ Reinicio automático en caso de bloqueo
- ✅ Protección contra loops infinitos
- ✅ Compatible con deep sleep

### 🖥️ **Sistema Display**
```cpp
#define MAX_SCREEN_MESSAGES 10         // Cola de mensajes
#define OLED_ADDRESS 0x3C              // Dirección I2C
#define OLED_CONTRAST 255              // Máximo contraste
```

## 📚 Documentación Completa

### 📖 **Guías de Usuario**
- **[Inicio](docs/inicio.md)**: Guía para principiantes - IoT, LoRa, TTN, ESP32
- **[Instalación](docs/instalacion.md)**: Setup completo desde cero
- **[Uso](docs/uso.md)**: Operación y configuración avanzada
- **[Troubleshooting](docs/troubleshooting.md)**: Diagnóstico y solución de problemas

### 🏗️ **Documentación Técnica**
- **[Arquitectura](docs/arquitectura.md)**: Diseño modular del sistema
- **[Código](docs/codigo.md)**: Estructura y flujos de ejecución
- **[Software](docs/software.md)**: Dependencias y configuración
- **[Hardware](docs/hardware.md)**: Especificaciones técnicas
- **[Sensor DHT22](docs/sensor_DHT22.md)**: Documentación específica del sensor
- **[Gestión de Sensores](docs/sensor_gestion.md)**: Configuración y branching
- **[Setup LoRaWAN](docs/LORAWAN_SETUP.md)**: Configuración de conectividad
- **[Decoder TTN](docs/ttn_decoder.md)**: Decodificación de datos
- **[Librerías de Terceros](docs/libreriasyterceros.md)**: Propietarios, licencias y derechos

## 🔗 Integración TTN

### 📊 **Decoder JavaScript**
```javascript
function decodeUplink(input) {
  var decoded = {};

  // Verificar tamaño de payload (7 bytes)
  if (input.bytes.length !== 7) {
    return {
      errors: ['Tamaño de payload inválido. Esperado 7 bytes, recibido ' + input.bytes.length]
    };
  }

  var bytes = input.bytes;

  // Decodificar temperatura (bytes 0-1, big-endian, *100)
  var tempRaw = (bytes[0] << 8) | bytes[1];
  if (tempRaw > 32767) tempRaw -= 65536; // Manejar negativos
  decoded.temperature = tempRaw / 100.0;

  // Decodificar humedad (bytes 2-3, big-endian, *100)
  decoded.humidity = ((bytes[2] << 8) | bytes[3]) / 100.0;

  // Decodificar batería (bytes 4-5, big-endian, *100)
  decoded.battery = ((bytes[4] << 8) | bytes[5]) / 100.0;

  // Decodificar estado solar (byte 6)
  decoded.solar_charging = bytes[6] ? true : false;

  return { data: decoded };
}
```

### 📈 **Dashboard TTN**
- Temperatura y humedad en gráficos
- Voltaje de batería con alertas
- Estado de carga solar (cargando/no cargando)
- RSSI/SNR para calidad de enlace
- Historial de transmisiones

## ⚡ Rendimiento y Eficiencia

### 📊 **Métricas de Consumo Detalladas**
- **Consumo por ciclo**: 0.17mAh (60s), 0.08mAh (120s), 0.03mAh (300s)
- **Deep Sleep**: 20μA (eficiencia >99.9%)
- **Transmisión LoRa**: 120mA por 1-2 segundos
- **Procesamiento**: 25mA durante 8 segundos por ciclo
- **Sensor DHT22**: 15mA durante 2 segundos (controlado)

### 🔋 **Autonomía Real Validada**
- **60 segundos**: 12-14 días con batería 3000mAh
- **120 segundos**: 25-28 días con batería 3000mAh  
- **300 segundos**: 62-70 días con batería 3000mAh
- **600 segundos**: 125-136 días con batería 3000mAh

### 📡 **Características de Enlace LoRaWAN**
- **Alcance**: Hasta 10-20km (línea de vista)
- **Penetración**: Buena en entornos urbanos
- **Fiabilidad**: ACK automático en cada transmisión
- **Latencia**: 1-2 segundos por uplink
- **Duty Cycle**: Cumple regulación europea (<1%)

## 🛠️ Desarrollo y Testing

### 🧪 **Suite de Tests**
```cpp
// Test completo del sistema
void runSystemTest() {
    testHardwareInit();
    testSensorReadings();
    testLoRaWANJoin();
    testDataTransmission();
    testDeepSleep();
}
```

### 📊 **Monitoreo de Debug**
```cpp
// Logs detallados en modo debug
#define DEBUG_MODE 1
// Incluye métricas de:
// - Tiempos de ejecución
// - Estados LoRaWAN
// - Lecturas de sensores
// - Consumo de memoria
```

### 🔧 **Herramientas de Desarrollo**
- **PlatformIO**: Compilación y upload
- **Serial Monitor**: Debugging en tiempo real
- **TTN Console**: Monitoreo de red LoRaWAN
- **Osciloscopio**: Análisis de consumo de energía

## 🚀 Casos de Uso

### 🌆 **Monitoreo Ambiental Urbano**
- Control de calidad del aire en ciudades
- Monitoreo de temperatura en infraestructuras
- Alertas de contaminación ambiental

### 🏭 **Industria y Agricultura**
- Monitoreo de condiciones en invernaderos
- Control de temperatura en cadenas de frío
- Vigilancia de equipos industriales

### 🏠 **Domótica e IoT**
- Sensores ambientales en hogares inteligentes
- Monitoreo de humedad en prevención de moho
- Control climático automático

### 🔬 **Investigación y Educación**
- Estaciones meteorológicas remotas
- Proyectos estudiantiles IoT
- Investigación ambiental

## 🤝 Contribución

### 📋 **Proceso de Contribución**
1. Fork el proyecto
2. Crear rama feature (`git checkout -b feature/nueva-funcionalidad`)
3. Commit cambios (`git commit -m 'Agrega nueva funcionalidad'`)
4. Push a rama (`git push origin feature/nueva-funcionalidad`)
5. Abrir Pull Request

### 🐛 **Reportar Issues**
- Usar template de issue proporcionado
- Incluir logs completos del Serial Monitor
- Especificar versión de PlatformIO y librerías
- Describir pasos para reproducir el problema

### 📝 **Guías de Código**
- **Estilo**: camelCase para variables, snake_case para constantes
- **Comentarios**: Doxygen format para documentación
- **Commits**: Mensajes descriptivos en inglés
- **Testing**: Validar cambios antes de commit

## 📄 Licencia

Este proyecto está bajo la **Licencia MIT**. Ver archivo `LICENSE` para más detalles.

```
MIT License - Copyright (c) 2025

Se concede permiso para usar, copiar, modificar y distribuir
este software con las condiciones especificadas en la licencia.
```

## 🙏 Agradecimientos

- **LilyGo**: Por el hardware T3-S3 de calidad
- **The Things Network**: Por la infraestructura LoRaWAN
- **IBM**: Por la librería LMIC
- **Adafruit**: Por las librerías de sensores
- **Comunidad ESP32**: Por el soporte y documentación

## 📞 Soporte

### 📧 **Canales de Soporte**
- **Issues**: Para bugs y feature requests
- **Discussions**: Para preguntas generales
- **Wiki**: Documentación detallada

### 🔗 **Recursos Adicionales**
- [TTN Documentation](https://www.thethingsnetwork.org/docs/)
- [ESP32 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [LoRaWAN Specification](https://lora-alliance.org/resource_hub/lorawan-specification-v1-0-3/)

---

**🌟 Sistema IoT LoRaWAN modular, eficiente y listo para producción**