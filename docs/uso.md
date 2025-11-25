# 📖 Guía de Uso - Sistema Multisensor LoRaWAN

## 🎯 Sistema Multisensor Configurable

Este sistema IoT implementa un **nodo sensor ambiental inteligente y configurable** que puede medir temperatura, humedad, presión atmosférica y voltaje de batería. El sistema permite **habilitar/deshabilita sensores individualmente** para adaptarse a diferentes necesidades.

## 🚀 Inicio Rápido

### 📋 Prerrequisitos
- **Hardware**: LilyGo T3 v1.6.1 con LoRa SX1276
- **Software**: PlatformIO + VS Code
- **Cuenta**: The Things Network (TTN)
- **Sensores**: Según configuración deseada

### ⚡ Configuración en 5 Minutos

#### 1. **Configurar Sensores** (`config/config.h`)
```cpp
// DESCOMENTA los sensores que quieres usar
#define ENABLE_SENSOR_DHT22      // DHT22 (Temperatura + Humedad)
#define ENABLE_SENSOR_BMP280     // BMP280 (Presión + Temperatura)
//#define ENABLE_SENSOR_DS18B20    // DS18B20 (Temperatura OneWire)
//#define ENABLE_SENSOR_HCSR04     // HC-SR04 (Distancia ultrasónica)
```

#### 2. **Configurar Credenciales LoRaWAN**
Editar `lorawan_config.h` con tus credenciales TTN:
```cpp
static const u1_t PROGMEM APPEUI[8] = {/* tus valores */};
static const u1_t PROGMEM DEVEUI[8] = {/* tus valores */};
static const u1_t PROGMEM APPKEY[16] = {/* tus valores */};
```

#### 3. **Compilar y Subir**
```bash
pio run --target upload
```

## 📊 Configuraciones de Ejemplo

### Configuración Básica (DHT22)
```cpp
#define ENABLE_SENSOR_DHT22      // Solo temperatura + humedad
// Otros sensores comentados
```
**Payload**: 7 bytes (Temp, Humedad, Batería, Estado Solar)

### Configuración Completa
```cpp
#define ENABLE_SENSOR_DHT22      // Temp + Humedad
#define ENABLE_SENSOR_BMP280     // Presión atmosférica
```
**Payload**: 9 bytes (Temp, Humedad, Presión, Batería, Estado Solar)

### Solo Batería
```cpp
// Todos los sensores comentados
#define ENABLE_SENSOR_NONE       // Solo monitoreo de batería
```
**Payload**: 4 bytes (Batería, Estado Solar)

## 📦 Formato del Payload Dinámico

El payload se adapta automáticamente según los sensores activos:

| Sensores Activos | Campos | Tamaño | Ejemplo |
|------------------|--------|--------|---------|
| DHT22 | Temp, Hum, Batt, Solar | 7 bytes | `01F4 0FA0 0DAC 01` |
| DHT22 + BMP280 | Temp, Hum, Pres, Batt, Solar | 9 bytes | `01F4 0FA0 2328 0DAC 01` |
| Todos | Temp, Hum, Pres, Batt, Solar | 9 bytes | `01F4 0FA0 2328 0DAC 01` |

### 🔧 Decodificador TTN Universal

```javascript
function decodeUplink(input) {
  var bytes = input.bytes;
  var data = {};
  var offset = 0;

  // Determinar qué campos están presentes por el tamaño del payload
  var payloadSize = bytes.length;

  // Temperatura (siempre presente en configuraciones con sensores de temp)
  if (payloadSize >= 7) {
    data.temperature = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
  }

  // Humedad (si payload >= 7 y hay sensores de humedad)
  if (payloadSize >= 7 && (payloadSize === 7 || payloadSize >= 9)) {
    data.humidity = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
  }

  // Presión (si payload >= 9)
  if (payloadSize >= 9) {
    data.pressure = ((bytes[offset++] << 8) | bytes[offset++]) / 10.0;
  }

  // Batería (siempre presente, últimos 2 bytes)
  data.battery_voltage = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;

  // Estado solar (siempre presente, último byte)
  data.solar_charging = bytes[offset] === 1;
  data.energy_source = data.solar_charging ? "Solar + Battery" : "Battery Only";

  return { data: data, warnings: [], errors: [] };
}
```

## 🔧 Configuración Avanzada

### Parámetros del Sistema (`config/config.h`)

```cpp
// Timing
#define SEND_INTERVAL_SECONDS 300    // Intervalo entre envíos
#define WATCHDOG_TIMEOUT_MINUTES 5   // Timeout del watchdog

// Energía
#define ENABLE_SOLAR_CHARGING true   // Habilitar carga solar
#define BATTERY_LOW_THRESHOLD 20     // Umbral batería baja (%)

// Display
#define ENABLE_DISPLAY true          // Activar pantalla OLED
#define SHOW_ACTIVITY_INDICATORS true // Mostrar indicadores

// LoRaWAN
#define LORAWAN_REGION LMIC_region_t::LMIC_REGION_eu868
#define TX_POWER_DBM 14              // Potencia TX (máx 14dBm)
```

### Sensores Soportados

| Sensor | Pines | Datos | Precisión | Rango |
|--------|-------|-------|-----------|-------|
| **DHT22** | GPIO 13 | Temp, Hum | ±0.5°C, ±3% | -40°C~80°C, 0~100% |
| **BMP280** | I2C (0x76/0x77) | Pres, Temp | ±1hPa, ±1°C | 300~1100hPa |
| **DS18B20** | GPIO 14 (OneWire) | Temp | ±0.5°C | -55°C~125°C |
| **HC-SR04** | GPIO 25/26 | Distancia | ±3mm | 2cm~400cm |

## 📱 Estados del Display OLED

### Secuencia Normal
1. **"Proyecto LoRaWAN"** (2s)
2. **"Bajo Consumo"** (2s)
3. **"Uniéndose OTAA..."** (persistente hasta join)
4. **"Unido a TTN!"** (3s)
5. **Datos ambientales** (5s) - según sensores activos
6. **"Datos enviados!"** (2s)
7. **Pantalla apagada** (ahorro energía)

### Ejemplos de Display

**Con DHT22**: `T:23.5C H:65.2% B:3.85V ☀️`
**Con BMP280**: `T:23.1C P:1013.2hPa B:3.87V`
**Solo batería**: `Solo bateria: 3.90V`

## 📍 Verificación y Monitoreo

### Serial Monitor
```
Sistema IoT Multisensor v2.0
Sensor: DHT22 inicializado
Uniéndose a TTN...
Unido exitosamente!
DHT22: Lectura OK - T:23.45°C H:65.20%
Payload: 7 bytes enviados
Deep sleep: 300s
```

### TTN Console
- **Device Overview**: Uplinks cada 5 minutos
- **Live Data**: Valores ambientales en tiempo real
- **Payload Decoder**: Verificar decodificación correcta

## 🛠️ Troubleshooting

### Problemas Comunes

**❌ "Sensor no encontrado"**
- Verificar conexiones físicas
- Comprobar pines en configuración
- Revisar alimentación del sensor

**❌ "Join failed"**
- Verificar credenciales TTN
- Comprobar región LoRaWAN
- Revisar antena y cobertura

**❌ "Payload vacío"**
- Verificar configuración de sensores
- Comprobar lecturas individuales
- Revisar formato del payload

### Debug Avanzado
```cpp
// Habilitar logs detallados
#define ENABLE_SERIAL_LOGS true
#define LOG_LEVEL 2  // 0: ninguno, 1: básico, 2: detallado
```

---

**🎓 Sistema Multisensor Extensible** | **📅 Noviembre 2025**