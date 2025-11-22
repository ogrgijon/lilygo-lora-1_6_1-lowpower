# Sensor Configuration Management Guide

Este documento explica cómo crear y mantener diferentes configuraciones de sensores usando ramas Git.

## Arquitectura Modular de Sensores

### Estructura de Archivos

```
include/
├── sensor_config.h      # Configuración de sensor activo
└── sensor_interface.h   # Interfaz genérica de sensores

src/
├── sensor.cpp           # Implementaciones condicionales de sensores
└── main.cpp             # Código principal
```

**Nota**: Las implementaciones de sensores se incluyen condicionalmente en `sensor.cpp` usando `#ifdef` directives, no como archivos separados.

### Interfaz Genérica

Todos los sensores implementan la misma interfaz:

```cpp
bool sensor_init(void);
bool sensor_is_available(void);
bool sensor_read_all(sensor_data_t* data);
uint8_t sensor_get_payload(payload_config_t* config);
const char* sensor_get_name(void);
```

## Workflow para Nuevas Configuraciones de Sensor

### 1. Crear Rama para Nuevo Sensor

```bash
# Crear rama con nombre descriptivo
git checkout -b feature/sensor-[nombre]

# Ejemplos:
git checkout -b feature/sensor-sht30
git checkout -b feature/sensor-ds18b20
git checkout -b feature/sensor-multi-sensor
```

### 2. Configurar el Sensor

**Editar `include/sensor_config.h`:**
```cpp
// Descomentar el sensor deseado
#define USE_SENSOR_[NOMBRE]

// Configurar pines y parámetros específicos
#define [SENSOR]_PIN 13
#define [SENSOR]_I2C_ADDRESS 0x44
```

### 3. Implementar el Sensor

**Editar `src/sensor.cpp` y agregar implementación condicional:**
```cpp
#ifdef USE_SENSOR_[NOMBRE]
// Implementación [NOMBRE]
#include <libreria_del_sensor.h>
#include "sensor_interface.h"
#include "sensor_config.h"

// Variables y funciones específicas del sensor
bool sensor_init(void) {
    // Inicialización específica
    return true;
}
// ... otras funciones
#endif // USE_SENSOR_[NOMBRE]
```

**O crear archivo separado `src/sensor_[nombre].cpp` (alternativo):**
```cpp
#include "sensor_interface.h"
#include "sensor_config.h"
// Incluir librerías específicas del sensor

// Implementar todas las funciones de la interfaz
bool sensor_init(void) { /* ... */ }
// ... etc
```

### 4. Actualizar Dependencias

**Editar `platformio.ini`:**
```ini
lib_deps =
    # Librerías existentes...
    nueva/librería @ ^version
```

### 5. Probar y Documentar

```bash
# Compilar y probar
pio run -t upload

# Crear documentación
echo "# Sensor [NOMBRE]" > SENSOR_[NOMBRE]_README.md
# Documentar conexiones, características, etc.
```

### 6. Commit y Push

```bash
git add .
git commit -m "feat: Add [Sensor] support with [características]"
git push origin feature/sensor-[nombre]
```

## Gestión de Ramas

### Cambiar entre Configuraciones

```bash
# Ver ramas disponibles
git branch -a

# Cambiar a rama específica
git checkout feature/sensor-dht22
git checkout feature/sensor-sht30
git checkout main
```

### Merge a Main (cuando esté listo)

```bash
# Desde la rama feature
git checkout main
git merge feature/sensor-dht22

# Resolver conflictos si los hay
git add .
git commit
```

### Mantener Ramas Actualizadas

```bash
# Actualizar rama feature con cambios de main
git checkout feature/sensor-dht22
git merge main

# O rebase para historia más limpia
git rebase main
```

## Sensores Soportados

| Sensor | Rama | Temp | Hum | Pres | Payload | Notas |
|--------|------|------|-----|------|---------|-------|
| DHT22 | feature/sensor-dht22 | ✅ | ✅ | ❌ | 7 bytes | Simple, económico, +solar |
| SHT30 | feature/sensor-sht30 | ✅ | ✅ | ❌ | 6 bytes | Alta precisión |
| DS18B20 | feature/sensor-ds18b20 | ✅ | ❌ | ❌ | 4 bytes | Solo temperatura |
| Mock | feature/sensor-mock | ✅ | ✅ | ✅ | 8 bytes | Para testing |

## Buenas Prácticas

### 1. Nombres Descriptivos
- `feature/sensor-dht22` (bueno)
- `dht22` (demasiado corto)
- `sensor-dht22-implementation` (demasiado largo)

### 2. Documentación
Cada rama debe incluir:
- `SENSOR_[NOMBRE]_README.md`
- Conexiones de hardware
- Características del sensor
- Tamaño del payload
- Instrucciones de testing

### 3. Compatibilidad
- Usar compilación condicional en `sensor.cpp` con `#ifdef` directives
- Mantener funciones legacy en la parte inferior de `sensor.cpp`
- No romper código existente en `main.cpp`
- Documentar cambios incompatibles

### 4. Testing
- Probar inicialización
- Verificar lecturas válidas
- Comprobar payload LoRaWAN
- Validar consumo de batería

## Troubleshooting

### Error de Compilación
```bash
# Verificar configuración en sensor_config.h
# Asegurar que solo un sensor esté definido
# Comprobar dependencias en platformio.ini
```

### Sensor no Detectado
```bash
# Verificar conexiones físicas
# Comprobar direcciones I2C
# Revisar alimentación del sensor
```

### Payload Incorrecto
```bash
# Verificar PAYLOAD_SIZE_BYTES en sensor_config.h
# Comprobar endianness (big-endian)
# Validar escalado de valores (*100)
```

## Scripts de Automatización

Para automatizar la creación de nuevas configuraciones:

```bash
#!/bin/bash
# create_sensor_branch.sh

SENSOR_NAME=$1
BRANCH_NAME="feature/sensor-$SENSOR_NAME"

git checkout -b $BRANCH_NAME
# ... crear archivos template ...
# ... actualizar configuración ...
```

## Sistema Solar Integrado

### 🎯 **Funcionalidad**

El sistema solar no es un sensor tradicional de temperatura/humedad, sino un **sensor de estado energético** que detecta la presencia de carga solar y el estado de la batería.

### 🔧 **Implementación**

#### **Archivo Dedicado**
```
src/
└── solar.cpp              # Módulo dedicado al sistema solar
```

#### **Funciones Principales**
```cpp
// Detección básica
bool isSolarChargingBattery();        // ¿Está cargando la batería?

// Estado detallado
bool getSolarChargeStatus();          // Estado completo de carga

// Monitoreo continuo
void checkSolarStatus();              // Verificación periódica
```

#### **Integración con Payload**
```cpp
// El estado solar se incluye en el payload LoRaWAN
uint8_t payload[7] = {
    temp_high, temp_low,     // 2 bytes - temperatura
    hum_high, hum_low,       // 2 bytes - humedad  
    batt_high, batt_low,     // 2 bytes - batería
    solar_status             // 1 byte - estado solar (0/1)
};
```

### 📊 **Características Técnicas**

| Parámetro | Valor | Unidad |
|-----------|-------|--------|
| **Detección VBUS** | 4.5-5.5 | V |
| **Corriente máxima** | 1000 | mA |
| **Resolución temporal** | 1 | Hz |
| **Payload adicional** | 1 | byte |

### 🔄 **Estados del Sistema**

#### **Estados de Energía**
- **🔋 Solo Batería**: `solar_status = 0`
- **☀️ + 🔋 Solar + Batería**: `solar_status = 1`

#### **Estados de Carga Detallados**
```cpp
enum SolarChargeState {
    SOLAR_DISCONNECTED,      // Panel desconectado
    SOLAR_CONNECTED_IDLE,    // Panel conectado, no carga
    SOLAR_CHARGING_ACTIVE,   // Cargando activamente
    SOLAR_CHARGING_COMPLETE  // Carga completa
};
```

### 📡 **Transmisión LoRaWAN**

#### **Decoder TTN Actualizado**
```javascript
function decodeUplink(input) {
    var data = {};
    var bytes = input.bytes;
    
    // Sensores ambientales
    data.temperature = ((bytes[0] << 8) | bytes[1]) / 100.0;
    data.humidity = ((bytes[2] << 8) | bytes[3]) / 100.0;
    data.battery_voltage = ((bytes[4] << 8) | bytes[5]) / 100.0;
    
    // Sistema solar
    data.solar_charging = bytes[6] === 1;
    data.energy_status = data.solar_charging ? "Solar Powered" : "Battery Only";
    
    return { data: data };
}
```

### 🔧 **Configuración**

#### **Hardware**
- **Panel Solar**: Conectado vía USB-C/VBUS
- **PMU AXP2101**: Detecta automáticamente entrada solar
- **ESP32-S3**: Monitorea estado vía I2C

#### **Software**
```cpp
// En solar.cpp - configuración
#define SOLAR_CHECK_INTERVAL_MS 1000    // Verificar cada segundo
#define SOLAR_VBUS_MIN_VOLTAGE 4.0      // Voltaje mínimo de detección
```

### 📈 **Analytics y Monitoreo**

#### **Métricas Disponibles**
- **Tiempo de sol diario**: Horas con carga solar activa
- **Eficiencia energética**: Energía solar aprovechada
- **Autonomía extendida**: Días adicionales de operación
- **Tendencias de batería**: Carga vs descarga

#### **Dashboard TTN**
```javascript
// Visualización de estado energético
if (data.solar_charging) {
    return {
        status: "🟢 SOLAR ACTIVE",
        trend: "↗️ CHARGING",
        autonomy: "EXTENDED"
    };
} else {
    return {
        status: "🔴 BATTERY ONLY", 
        trend: "↘️ DISCHARGING",
        autonomy: "STANDARD"
    };
}
```

### 🧪 **Testing del Sistema Solar**

#### **Pruebas Unitarias**
```cpp
void testSolarDetection() {
    // Simular conexión solar
    assert(isSolarChargingBattery() == true);
    
    // Simular desconexión
    assert(isSolarChargingBattery() == false);
    
    // Verificar payload incluye estado solar
    uint8_t payload[7];
    uint8_t size = getSensorPayload(payload, sizeof(payload));
    assert(size == 7);
    assert(payload[6] == 1 || payload[6] == 0);
}
```

#### **Pruebas de Integración**
```cpp
void testSolarLoRaWAN() {
    // Conectar panel solar
    // Verificar transmisión incluye estado solar
    // Comprobar decoder TTN procesa correctamente
    // Validar dashboard muestra estado correcto
}
```

### 🔗 **Compatibilidad con Sensores**

#### **Todos los Sensores**
- ✅ **DHT22**: Compatible, payload expandido a 7 bytes
- ✅ **SHT30**: Compatible, requiere actualización payload
- ✅ **DS18B20**: Compatible, requiere actualización payload
- ✅ **Mock**: Compatible, simula estado solar

#### **Actualización de Payload**
```cpp
// Para sensores existentes, actualizar PAYLOAD_SIZE_BYTES
#ifdef USE_SENSOR_DHT22
#define PAYLOAD_SIZE_BYTES (2 + 2 + 2 + 1)  // temp + hum + batt + solar
#endif
```

### 🎯 **Beneficios del Sistema Solar**

- ✅ **Autonomía ilimitada**: Carga continua con panel solar
- ✅ **Monitoreo remoto**: Estado energético visible en TTN
- ✅ **Analytics avanzados**: Métricas de eficiencia solar
- ✅ **Bajo costo**: Usa hardware PMU existente
- ✅ **Transparente**: No afecta funcionamiento normal

---

## Conclusión

Esta arquitectura permite:
- ✅ Mantenimiento fácil de múltiples configuraciones
- ✅ Cambios rápidos entre sensores
- ✅ Código modular y reutilizable
- ✅ Testing independiente por sensor
- ✅ Documentación clara por configuración

Para contribuir con un nuevo sensor, sigue el workflow documentado y crea un Pull Request cuando esté listo.