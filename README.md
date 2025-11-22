# 🌡️ Sistema IoT LoRaWAN Modular - Sensor Ambiental Inteligente

> **🎓 Proyecto Académico - Sin Ánimo de Lucro**  
> Este proyecto ha sido desarrollado para apoyar a los investigadores y prototipos del Medialab de la Universidad de Oviedo. Todo el código, documentación y diseños están disponibles gratuitamente para fines educativos y de investigación.

> **¿Nuevo en IoT y LoRaWAN?**  
> Consulta la guía de introducción: [docs/inicio.md](docs/inicio.md) para aprender desde cero sobre la tecnología, conceptos básicos y primeros pasos.


[![PlatformIO](https://img.shields.io/badge/PlatformIO-6.1.11-blue.svg)](https://platformio.org)
[![ESP32](https://img.shields.io/badge/ESP32-240MHz-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![LoRaWAN](https://img.shields.io/badge/LoRaWAN-1.0.3-orange.svg)](https://lora-alliance.org)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

**Sistema IoT completo con arquitectura modular** para monitoreo ambiental remoto. Implementa medición precisa de temperatura, humedad, presión atmosférica y voltaje de batería, transmitiendo datos por LoRaWAN cada 60 segundos con **consumo ultra-bajo** (Hasta < 20μA en deep sleep).

## 🎯 Características Principales

### 🌡️ **Sensores Ambientales Avanzados**
- **BME280**: Temperatura (-40°C a +85°C), humedad (0-100%), presión (300-1100 hPa)
- **Precisión**: 0.01 unidades para todas las lecturas
- **Recuperación automática**: Sistema continúa funcionando aunque falle el sensor

### 📡 **Comunicación LoRaWAN Robusta**
- **Clase A**: Bajo consumo con bidireccionalidad
- **OTAA Join**: Activación segura sobre el aire
- **Adaptive Rate**: Spreading Factor 7-12 según condiciones
- **Región EU868**: Optimizado para Europa 868MHz

### 🔋 **Gestión de Energía Inteligente**
- **Deep Sleep**: 60 segundos entre ciclos - por defecto -
- **Monitoreo batería**: Voltaje en tiempo real
- **Auto-apagado**: Componentes se desactivan automáticamente
- **Eficiencia**: < 0.5mAh por ciclo (autonomía ~4 días)

### 🔋 **Ejemplos de Autonomía según Tiempo de Deep Sleep**

Suponiendo una batería LiPo 18650 de **3000mAh** y siendo conservadores con la media de estimación de consumo:

| Deep Sleep (segundos) | Ciclos/día | Consumo diario | Autonomía teórica (3000mAh) |
|----------------------|------------|---------------|-----------------------------|
| **60**               | 1440       | 216mAh        | ~14 días                    |
| **120**              | 720        | 108mAh        | ~28 días                    |
| **300**              | 288        | 43mAh         | ~70 días                    |
| **600**              | 144        | 22mAh         | ~136 días                   |

> **Ejemplo con placa solar:**  
Para operación continua, una placa solar debe suministrar al menos el consumo diario.  
- **Consumo típico:** 216mAh/día × 3.7V ≈ **0.8Wh/día**  
- **Placa recomendada:** 1W (5V, 200mA) con 1-2h de sol directo cubre la demanda y recarga la batería.

> **Nota:** La autonomía real depende de la calidad de la batería, condiciones ambientales y eficiencia de carga solar.

| Deep Sleep (segundos) | Ciclos/día | Consumo diario | Autonomía teórica (3000mAh) |
|----------------------|------------|---------------|-----------------------------|
| **60**               | 1440       | 216mAh        | ~14 días                    |
| **120**              | 720        | 108mAh        | ~28 días                    |
| **300**              | 288        | 43mAh         | ~70 días                    |
| **600**              | 144        | 22mAh         | ~136 días                   |

> **Nota:** La autonomía real puede variar según condiciones ambientales, calidad de la batería y consumo adicional por transmisión o display.

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

### 📦 **Payload de 8 Bytes (Big-Endian)**

| Campo | Bytes | Tipo | Rango | Precisión | Ejemplo |
|-------|-------|------|-------|-----------|---------|
| **Temperatura** | 0-1 | int16_t | -40°C a 85°C | 0.01°C | `25.67°C` |
| **Humedad** | 2-3 | uint16_t | 0-100% | 0.01% | `65.43%` |
| **Presión** | 4-5 | uint16_t | 300-1100 hPa | 0.01 hPa | `1013.25 hPa` |
| **Batería** | 6-7 | uint16_t | 0-5V | 0.01V | `3.85V` |

### 🔍 **Códigos de Error**
- **Temperatura**: `-999.0°C` (sensor fallando)
- **Humedad**: `-1.0%` (sensor fallando)
- **Presión**: `-1.0 hPa` (sensor fallando)
- **Batería**: Siempre disponible

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
   ```cpp
   // src/pgm_board.cpp
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
│   ├── main.cpp               # 🚀 Entry point Arduino (renombrado)
│   ├── pgm_board.cpp          # 📡 Núcleo LoRaWAN
│   ├── sensor.cpp             # 🌡️ Gestión sensores BME280
│   ├── screen.cpp             # 🖥️ Sistema display OLED
│   ├── LoRaBoards.cpp         # 🔧 Abstracción hardware LilyGo
│   ├── LoRaBoards.h           # 🔧 Headers hardware
│   └── utilities.h            # 📋 Utilidades comunes
├── 📁 include/
│   ├── sensor.h               # 📋 API sensores
│   ├── screen.h               # 📋 API display
│   ├── LoRaBoards.h           # 📋 API hardware LilyGo
│   ├── loramac.h              # 📋 Headers LoRaWAN
│   └── utilities.h            # 📋 Utilidades comunes
├── 📁 lib/
│   ├── Adafruit_BME280_Library/  # 🌡️ Librería sensor BME280
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
│   ├── libreriasyterceros.md  # 📋 Librerías y licencias
│   └── troubleshooting.md     # 🔧 Solución de problemas
├── platformio.ini             # ⚙️ Configuración PlatformIO
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

### 🌡️ **Configuración Sensor BME280**
```cpp
// Modos de operación
#define BME280_MODE_FORCED              // Bajo consumo

// Sobremuestreo para precisión
#define BME280_TEMPERATURE_OSR BME280_OSR_X2
#define BME280_HUMIDITY_OSR BME280_OSR_X1
#define BME280_PRESSURE_OSR BME280_OSR_X1

// Dirección I2C
bool sensorOk = bme.begin(0x76);        // Default
if (!sensorOk) sensorOk = bme.begin(0x77); // Fallback
```

### 🔋 **Gestión de Energía**
```cpp
#define SLEEP_TIME_SECONDS 60          // Ciclo de 60 segundos
#define BATTERY_LOW_VOLTAGE 3.0f       // Umbral batería baja
#define BATTERY_FULL_VOLTAGE 4.2f      // Batería cargada
```

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
- **[Librerías de Terceros](docs/libreriasyterceros.md)**: Propietarios, licencias y derechos

## 🔗 Integración TTN

### 📊 **Decoder JavaScript**
```javascript
function decodeUplink(input) {
  var bytes = input.bytes;
  return {
    data: {
      temperature: ((bytes[0] << 8) | bytes[1]) / 100.0,
      humidity: ((bytes[2] << 8) | bytes[3]) / 100.0,
      pressure: ((bytes[4] << 8) | bytes[5]) / 100.0,
      battery_voltage: ((bytes[6] << 8) | bytes[7]) / 100.0
    }
  };
}
```

### 📈 **Dashboard TTN**
- Temperatura, humedad, presión en gráficos
- Voltaje de batería con alertas
- RSSI/SNR para calidad de enlace
- Historial de transmisiones

## ⚡ Rendimiento y Eficiencia

### 📊 **Métricas de Consumo**
| Modo | Consumo | Duración | Energía |
|------|---------|----------|---------|
| **Activo** | 120mA | 1-2s | ~0.07mAh |
| **Idle** | 25mA | 8s | ~0.06mAh |
| **Display ON** | 25mA | 5s | ~0.03mAh |
| **Deep Sleep** | 20μA | 60s | ~0.0003mAh |
| **Total/ciclo** | - | 60s | **~0.15mAh** |

### 🔋 **Cálculo de Autonomía**
```
Batería LiPo 18650 (3000mAh):
- Consumo promedio: 0.15mAh/ciclo
- Ciclos/día: 1440 (24h ÷ 60s)
- Consumo diario: 216mAh
- Autonomía teórica: ~14 días
- Autonomía real: ~10-12 días (factor de seguridad)
```

### 📡 **Características de Enlace**
- **Alcance**: Hasta 10-20km (línea de vista)
- **Penetración**: Buena en entornos urbanos
- **Fiabilidad**: ACK automático en cada transmisión
- **Latencia**: 1-2 segundos por uplink

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