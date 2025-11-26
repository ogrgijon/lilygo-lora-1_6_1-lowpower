# ⚙️ Desarrollo - Cómo Modificar y Extender el Sistema

**Guía completa para estudiantes que quieren desarrollar nuevos sensores y funcionalidades**

---

## 🎯 ¿Qué aprenderás aquí?

Esta guía te enseñará a:
- Entender la estructura del código
- Modificar configuraciones existentes
- Agregar nuevos sensores al sistema
- Depurar y probar tus cambios
- Contribuir al proyecto

**Tiempo estimado**: 45 minutos

---

## 📂 Estructura del Código

### 🏗️ Arquitectura Modular

```
low-power-project/
├── 📁 src/                    # Código fuente principal
│   ├── main.ino             # 🚀 Punto de entrada (Arduino)
│   ├── pgm_board.cpp         # 📡 Lógica LoRaWAN
│   ├── sensor.cpp            # 🌡️ Gestión multisensor
│   ├── screen.cpp            # 🖥️ Display OLED
│   └── LoRaBoards.cpp        # 🔧 Hardware LilyGo
├── 📁 src/sensor/            # 📂 Implementaciones de sensores
│   ├── sensor_dht22.cpp      # DHT22 (Temp + Humedad)
│   ├── sensor_ds18b20.cpp    # DS18B20 (Temperatura)
│   ├── sensor_bmp280.cpp     # BMP280 (Presión)
│   └── sensor_hcsr04.cpp     # HC-SR04 (Distancia)
├── 📁 config/                # ⚙️ Archivos de configuración
│   ├── config.h              # Configuración principal
│   ├── lorawan_config.h      # Claves TTN
│   └── hardware_config.h     # Config hardware
└── 📁 include/               # 🔧 Headers y interfaces
    ├── LoRaBoards.h
    ├── loramac.h
    └── utilities.h
```

### 🎯 Separación de Responsabilidades

| Módulo | Responsabilidad | Archivo Principal |
|--------|----------------|-------------------|
| **Main** | Inicialización y ciclo principal | `main.ino` |
| **LoRaWAN** | Comunicación con TTN | `pgm_board.cpp` |
| **Sensor** | Lectura de sensores | `sensor.cpp` + `src/sensor/*.cpp` |
| **Display** | Interfaz OLED | `screen.cpp` |
| **Hardware** | Configuración LilyGo | `LoRaBoards.cpp` |

---

## 🔧 Modificar Configuraciones Existentes

### ⚙️ Cambiar Intervalos de Envío

**Archivo**: `config/config.h`

```cpp
// Cambia el intervalo entre transmisiones (segundos)
#define SEND_INTERVAL_SECONDS 300  // 5 minutos

// Opciones comunes:
#define SEND_INTERVAL_SECONDS 60   // 1 minuto (desarrollo)
#define SEND_INTERVAL_SECONDS 600  // 10 minutos (ahorro)
#define SEND_INTERVAL_SECONDS 3600 // 1 hora (máximo ahorro)
```

### 🔋 Configurar Gestión de Energía

```cpp
// Control de batería
#define BATTERY_LOW_VOLTAGE 3.0f        // Alerta batería baja
#define BATTERY_SCALE_FACTOR 2.0f       // Factor divisor resistivo

// Control de display
#define ENABLE_DISPLAY true             // false = máximo ahorro
#define DISPLAY_TIMEOUT_MS 2000         // Auto-apagado display

// Watchdog
#define WATCHDOG_TIMEOUT_SECONDS 300    // 5 minutos
```

### 📡 Configurar LoRaWAN

**Archivo**: `config/lorawan_config.h`

```cpp
// Región LoRaWAN
#define CFG_eu868 1                    // Europa 868MHz

// Potencia de transmisión
#define TX_POWER_DBM 14                // 14dBm (máxima permitida)

// Claves TTN (reemplaza con tus valores)
static const u1_t PROGMEM APPEUI[8] = {/* tus valores */};
static const u1_t PROGMEM DEVEUI[8] = {/* tus valores */};
static const u1_t PROGMEM APPKEY[16] = {/* tus valores */};
```

---

## 🚀 Agregar un Nuevo Sensor (Guía Paso a Paso)

### Paso 1: Elige un Sensor

Para este ejemplo, agregaremos soporte para un **sensor de luz BMP280** que mide luminosidad en lux.

**Características del BMP280**:
- Interfaz: I2C
- Rango: 1-65535 lux
- Alimentación: 3.3V
- Dirección I2C: 0x23 (por defecto)

### Paso 2: Agrega la Configuración

**Archivo**: `config/config.h`

```cpp
// Agrega al final de la sección de sensores
#define ENABLE_SENSOR_BMP280    // Sensor de presión BMP280
```

### Paso 3: Crea el Archivo del Sensor

**Archivo**: `src/sensor/sensor_template.cpp`

```cpp
#ifdef ENABLE_SENSOR_BMP280

#include <Wire.h>
#include <BMP280.h>  // Librería del sensor
#include "config.h"

// Variables globales
static BMP280 lightMeter;
static bool sensor_available = false;

// Pines I2C (compartidos con otros sensores)
#define BMP280_I2C_SDA 21
#define BMP280_I2C_SCL 22
#define BMP280_I2C_ADDR 0x23  // Dirección por defecto

// Función de inicialización
bool sensor_BMP280_init(void) {
    // Inicializar I2C si no está inicializado
    Wire.begin(BMP280_I2C_SDA, BMP280_I2C_SCL);

    // Inicializar sensor
    if (lightMeter.begin(BMP280::CONTINUOUS_HIGH_RES_MODE, BMP280_I2C_ADDR)) {
        sensor_available = true;
        Serial.println("BMP280 inicializado correctamente");
        return true;
    } else {
        Serial.println("Error: BMP280 no encontrado");
        return false;
    }
}

// Función de lectura
bool sensor_BMP280_read(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    // Leer luminosidad
    float lux = lightMeter.readLightLevel();

    // Validar lectura
    if (lux < 0 || lux > 65535) {
        Serial.println("Error: Lectura BMP280 inválida");
        return false;
    }

    // Almacenar en estructura de datos
    data->light = lux;
    data->valid = true;

    Serial.printf("BMP280: %.1f lux\n", lux);
    return true;
}

// Función de disponibilidad
bool sensor_BMP280_available(void) {
    return sensor_available;
}

#endif // ENABLE_SENSOR_BMP280
```

### Paso 4: Actualiza la Estructura de Datos

**Archivo**: `config/config.h`

```cpp
// Agrega campo de luz a la estructura sensor_data_t
typedef struct {
    // Campos existentes
    float temperature;
    float humidity;
    float pressure;
    float distance;
    float battery_voltage;
    bool solar_charging;
    bool valid;

    // Nuevo campo para BMP280
    float light;  // Luminosidad en lux
} sensor_data_t;
```

### Paso 5: Integra el Sensor en el Sistema Principal

**Archivo**: `src/sensor.cpp`

```cpp
// Incluye el nuevo sensor
#include "sensor_template.h"

// En la función sensors_init_all()
bool sensors_init_all(void) {
    bool success = true;

    #ifdef ENABLE_SENSOR_DHT22
    success &= sensor_dht22_init();
    #endif

    #ifdef ENABLE_SENSOR_DS18B20
    success &= sensor_ds18b20_init();
    #endif

    // Agrega inicialización BMP280
    #ifdef ENABLE_SENSOR_BMP280
    success &= sensor_BMP280_init();
    #endif

    return success;
}

// En la función sensors_read_all()
bool sensors_read_all(sensor_data_t* data) {
    if (!data) return false;

    bool success = true;

    // Leer sensores existentes
    #ifdef ENABLE_SENSOR_DHT22
    success &= sensor_dht22_read(data);
    #endif

    #ifdef ENABLE_SENSOR_DS18B20
    success &= sensor_ds18b20_read(data);
    #endif

    // Leer BMP280
    #ifdef ENABLE_SENSOR_BMP280
    success &= sensor_BMP280_read(data);
    #endif

    // Leer batería (siempre)
    data->battery_voltage = readBatteryVoltage();
    data->solar_charging = isSolarChargingBattery();

    data->valid = success;
    return success;
}
```

### Paso 6: Actualiza el Payload LoRaWAN

**Archivo**: `src/sensor.cpp`

```cpp
// En la función getSensorPayload()
uint8_t getSensorPayload(uint8_t* payload, uint8_t maxSize) {
    sensor_data_t data;
    if (!sensors_read_all(&data)) {
        return 0; // Error
    }

    uint8_t offset = 0;

    // Temperatura (si hay sensores de temp)
    #if defined(ENABLE_SENSOR_DHT22) || defined(ENABLE_SENSOR_DS18B20) || defined(ENABLE_SENSOR_BMP280)
    if (offset + 2 <= maxSize) {
        int16_t temp = (int16_t)(data.temperature * 100);
        payload[offset++] = (temp >> 8) & 0xFF;
        payload[offset++] = temp & 0xFF;
    }
    #endif

    // Humedad (solo DHT22)
    #ifdef ENABLE_SENSOR_DHT22
    if (offset + 2 <= maxSize) {
        uint16_t hum = (uint16_t)(data.humidity * 100);
        payload[offset++] = (hum >> 8) & 0xFF;
        payload[offset++] = hum & 0xFF;
    }
    #endif

    // Presión (BMP280)
    #ifdef ENABLE_SENSOR_BMP280
    if (offset + 4 <= maxSize) {
        uint32_t pres = (uint32_t)((data.pressure - 900) * 10); // Offset para optimizar
        payload[offset++] = (pres >> 24) & 0xFF;
        payload[offset++] = (pres >> 16) & 0xFF;
        payload[offset++] = (pres >> 8) & 0xFF;
        payload[offset++] = pres & 0xFF;
    }
    #endif

    // Distancia (HC-SR04)
    #ifdef ENABLE_SENSOR_HCSR04
    if (offset + 2 <= maxSize) {
        uint16_t dist = (uint16_t)(data.distance * 100);
        payload[offset++] = (dist >> 8) & 0xFF;
        payload[offset++] = dist & 0xFF;
    }
    #endif

    // BMP280 - Luminosidad
    #ifdef ENABLE_SENSOR_BMP280
    if (offset + 2 <= maxSize) {
        uint16_t light = (uint16_t)(data.light);
        payload[offset++] = (light >> 8) & 0xFF;
        payload[offset++] = light & 0xFF;
    }
    #endif

    // Batería (siempre incluida)
    if (offset + 2 <= maxSize) {
        uint16_t batt = (uint16_t)(data.battery_voltage * 100);
        payload[offset++] = (batt >> 8) & 0xFF;
        payload[offset++] = batt & 0xFF;
    }

    // Estado solar
    if (offset + 1 <= maxSize) {
        payload[offset++] = data.solar_charging ? 1 : 0;
    }

    return offset; // Retorna tamaño real del payload
}
```

### Paso 7: Agrega la Librería

**Archivo**: `platformio.ini`

```ini
[env]
lib_deps =
    ; Librerías existentes
    adafruit/DHT sensor library@^1.4.4
    milesburton/DallasTemperature@^3.11.0
    adafruit/Adafruit BMP280 Library@^2.6.8
    ; Nueva librería para BMP280
    claws/BMP280@^1.3.0
```

### Paso 8: Actualiza la Documentación TTN

**Archivo**: `docs/6_uso.md` (sección decodificador)

```javascript
// Agrega campo de luz al decodificador TTN
function decodeUplink(input) {
  var bytes = input.bytes;
  var data = {};
  var offset = 0;

  // Determinar qué campos están presentes por el tamaño
  var payloadSize = bytes.length;

  // Temperatura (siempre presente en configs con sensores)
  if (payloadSize >= 7) {
    var tempRaw = (bytes[offset] << 8) | bytes[offset + 1];
    if (tempRaw > 32767) tempRaw -= 65536;
    data.temperature = tempRaw / 100.0;
    offset += 2;
  }

  // Humedad (DHT22)
  if (payloadSize >= 9 && offset + 2 <= payloadSize) {
    data.humidity = ((bytes[offset] << 8) | bytes[offset + 1]) / 100.0;
    offset += 2;
  }

  // Presión (BMP280)
  if (payloadSize >= 13 && offset + 4 <= payloadSize) {
    var presRaw = (bytes[offset] << 24) | (bytes[offset + 1] << 16) | (bytes[offset + 2] << 8) | bytes[offset + 3];
    data.pressure = (presRaw / 10.0) + 900;
    offset += 4;
  }

  // Distancia (HC-SR04)
  if (payloadSize >= 15 && offset + 2 <= payloadSize) {
    data.distance = ((bytes[offset] << 8) | bytes[offset + 1]) / 100.0;
    offset += 2;
  }

  // BMP280 - Luminosidad
  if (payloadSize >= 17 && offset + 2 <= payloadSize) {
    data.light = ((bytes[offset] << 8) | bytes[offset + 1]);
    offset += 2;
  }

  // Batería (siempre presente)
  if (offset + 2 <= payloadSize) {
    data.battery = ((bytes[offset] << 8) | bytes[offset + 1]) / 100.0;
    offset += 2;
  }

  // Estado solar
  if (offset + 1 <= payloadSize) {
    data.solar_charging = bytes[offset] ? true : false;
  }

  return { data: data };
}
```

### Paso 9: Prueba tu Nuevo Sensor

```cpp
// En config/config.h
#define ENABLE_SENSOR_BMP280    // Solo BMP280 para pruebas
// Comenta otros sensores

// Compila y sube
pio run --target upload

// Verifica en Serial Monitor
// Deberías ver: "BMP280 inicializado correctamente"
// Y lecturas como: "BMP280: 450.5 lux"
```

---

## 🐛 Depuración y Testing

### 📺 Usar Serial Monitor

```cpp
// Agrega logs de debug en tu código
#define DEBUG_MODE 1

// En funciones
Serial.printf("BMP280: Inicializando...\n");
Serial.printf("BMP280: Lectura = %.1f lux\n", lux);
```

### 🔍 Verificar I2C

```cpp
// Escanear dispositivos I2C
void scanI2C() {
    Serial.println("Escaneando I2C...");
    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Dispositivo encontrado en 0x%02X\n", address);
        }
    }
}
```

### 🧪 Tests Unitarios

```cpp
// Función de test para BMP280
void testBMP280() {
    Serial.println("=== TEST BMP280 ===");

    // Test inicialización
    if (sensor_BMP280_init()) {
        Serial.println("✅ Inicialización OK");
    } else {
        Serial.println("❌ Inicialización FALLÓ");
        return;
    }

    // Test lectura
    sensor_data_t test_data = {0};
    if (sensor_BMP280_read(&test_data)) {
        Serial.printf("✅ Lectura OK: %.1f lux\n", test_data.light);
    } else {
        Serial.println("❌ Lectura FALLÓ");
    }
}
```

---

## 🚀 Buenas Prácticas de Desarrollo

### 📝 Convenciones de Código

```cpp
// ✅ Bien: Nombres descriptivos
bool sensor_BMP280_init(void)
float readLightLevel(void)

// ❌ Mal: Nombres confusos
bool init(void)
float get(void)

// ✅ Bien: Comentarios claros
// Inicializa sensor BMP280 en modo alta resolución
bool sensor_BMP280_init(void) {
    // Código comentado
}

// ✅ Bien: Constantes con nombre descriptivo
#define BMP280_I2C_ADDR 0x23
#define BMP280_MODE BMP280::CONTINUOUS_HIGH_RES_MODE
```

### 🛡️ Manejo de Errores

```cpp
// ✅ Bien: Validación exhaustiva
bool sensor_BMP280_read(sensor_data_t* data) {
    if (!sensor_available || !data) {
        Serial.println("Error: Sensor no disponible o puntero nulo");
        return false;
    }

    float lux = lightMeter.readLightLevel();

    if (lux < 0 || lux > 65535) {
        Serial.printf("Error: Lectura inválida: %.1f lux\n", lux);
        return false;
    }

    data->light = lux;
    return true;
}
```

### ⚡ Optimización de Memoria

```cpp
// ✅ Bien: Variables locales cuando sea posible
bool sensor_BMP280_read(sensor_data_t* data) {
    // No uses variables globales innecesarias
    float lux = lightMeter.readLightLevel(); // Local
    // ...
}

// ✅ Bien: Liberar recursos no usados
#ifdef ENABLE_SENSOR_BMP280
// Código solo incluido si el sensor está habilitado
#endif
```

---

## 🤝 Contribuir al Proyecto

### 📋 Proceso de Contribución

1. **Fork** el repositorio
2. **Crea una rama** para tu feature: `git checkout -b feature/sensor-BMP280`
3. **Implementa** tu sensor siguiendo esta guía
4. **Testea** exhaustivamente
5. **Documenta** los cambios
6. **Commit**: `git commit -m "Add BMP280 light sensor support"`
7. **Push**: `git push origin feature/sensor-BMP280`
8. **Pull Request** con descripción detallada

### 📖 Documentación Requerida

Cuando agregues un nuevo sensor, actualiza:

- [ ] `README.md`: Menciona el nuevo sensor
- [ ] `docs/6_uso.md`: Configuración y ejemplos
- [ ] `docs/4_arquitectura.md`: Diagrama actualizado
- [ ] `platformio.ini`: Dependencias nuevas
- [ ] TTN decoder: Campos nuevos

### 🧪 Checklist de Calidad

- [ ] Código compila sin warnings
- [ ] Funciona con otros sensores habilitados
- [ ] Manejo correcto de errores
- [ ] Logs informativos en Serial
- [ ] Documentación actualizada
- [ ] Testeado con batería real
- [ ] Verificado consumo de energía

---

## 🎓 Consejos para Estudiantes

### 🧠 Conceptos Importantes

1. **Compilación Condicional**: Usa `#ifdef ENABLE_SENSOR_*` para incluir solo código necesario
2. **Endianness**: LoRaWAN usa Big-Endian para payloads
3. **Optimización**: Cada byte cuenta en comunicaciones IoT
4. **Robustez**: Siempre valida entradas y maneja errores
5. **Modularidad**: Separa inicialización, lectura y configuración

### 📚 Recursos de Aprendizaje

- **ESP32**: [Documentación oficial](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- **LoRaWAN**: [Especificación LoRaWAN](https://lora-alliance.org/resource_hub/lorawan-specification-v1-0-3/)
- **TTN**: [Guías TTN](https://www.thethingsnetwork.org/docs/)
- **I2C**: [Tutorial I2C](https://learn.sparkfun.com/tutorials/i2c)

### 🚀 Próximos Desafíos

Una vez que domines agregar sensores, prueba:

1. **Sensor analógico**: ADC del ESP32
2. **Sensor OneWire**: Múltiples DS18B20
3. **Sensor SPI**: Acelerómetro, giroscopio
4. **Sensor UART**: GPS, modem GSM
5. **Actuadores**: Relés, servos, LEDs

---

**¡Ahora tienes el poder de extender el sistema con cualquier sensor imaginable!** 🚀

¿Has agregado un nuevo sensor? ¡Comparte tu experiencia en los issues!
