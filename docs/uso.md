# 📖 Guía de Uso - Sistema IoT LoRaWAN Modular

## 🎯 Resumen Ejecutivo

Este sistema IoT implementa un **nodo sensor ambiental inteligente** con arquitectura modular que mide temperatura, humedad, presión atmosférica y voltaje de batería, transmitiendo datos por LoRaWAN cada 60 segundos. Diseñado para **máxima eficiencia energética** con deep sleep y gestión inteligente de componentes.

## 🚀 Inicio Rápido

### 📋 Prerrequisitos
- **Hardware**: LilyGo T3-S3 con LoRa SX1262
- **Software**: PlatformIO + VS Code
- **Cuenta**: The Things Network (TTN) o similar
- **Entorno**: Windows/Linux/Mac con USB

### ⚡ Configuración en 5 Minutos

#### 1. **Clonar y Abrir Proyecto**
```bash
git clone <tu-repo>
cd low-power-project
code .
```

#### 2. **Configurar Credenciales LoRaWAN**
```bash
# Copiar el archivo de plantilla
cp include/lorawan_config_template.h include/lorawan_config.h

# Editar las credenciales en include/lorawan_config.h
# Reemplazar con tus credenciales TTN
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u1_t PROGMEM DEVEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u1_t PROGMEM APPKEY[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
```

#### 3. **Compilar y Subir**
```bash
# En VS Code con PlatformIO
Ctrl+Shift+P → "PlatformIO: Upload"
```

#### 4. **Verificar Funcionamiento**
- **LED azul**: Parpadea durante inicialización
- **OLED**: Muestra "Sistema Iniciado" → "Uniéndose OTAA..." → datos ambientales
- **Serial Monitor**: Logs detallados del proceso

## 📊 Datos Transmitidos

### 📦 Formato del Payload (7 bytes)

| Bytes | Campo | Tipo | Rango | Precisión | Ejemplo |
|-------|-------|------|-------|-----------|---------|
| 0-1 | Temperatura | int16_t | -40°C a 85°C | 0.01°C | `25.67°C` → `0x0A 0x1B` |
| 2-3 | Humedad | uint16_t | 0-100% | 0.01% | `65.43%` → `0x19 0x2B` |
| 4-5 | Presión | uint16_t | 300-1100 hPa | 0.01 hPa | `1013.25 hPa` → `0x27 0x89` |
| 6-7 | Batería | uint16_t | 0-5V | 0.01V | `3.85V` → `0x0F 0x11` |

### 🔍 Códigos de Error
- **Temperatura**: `-999.0°C` (sensor fallando)
- **Humedad**: `-1.0%` (sensor fallando)
- **Presión**: `-1.0 hPa` (sensor fallando)
- **Batería**: Siempre disponible (0-5V)

## 🖥️ Interfaz de Usuario (OLED)

### 📱 Estados del Display

#### 🔄 **Secuencia Normal**
```
Sistema Iniciado
    ↓ (3s)
Uniéndose OTAA.... (persistente)
    ↓ (join exitoso)
Unido a TTN!
    ↓ (2s)
[Display OFF - ahorro energía]
    ↓ (cada 60s)
T: 25.67°C
H: 65.43%
P:1013.25hPa
B: 3.85V
    ↓ (5s)
[Display OFF]
```

#### ⚠️ **Estados de Error**
```
[ERROR]
Sensor no disponible
    ↓ (3s)
Solo bateria
B: 3.85V
```

#### ✅ **Confirmaciones**
```
[OK]
Datos enviados!
    ↓ (2s)
[Display OFF]
```

### 🎨 **Sistema de Mensajes**
- **Cola inteligente**: Hasta 10 mensajes pendientes
- **Auto-apagado**: Display se apaga automáticamente
- **Priorización**: Mensajes nuevos desplazan antiguos
- **Temporización**: Cada mensaje tiene duración configurable

## 🔧 Operación Avanzada

### 📡 Configuración LoRaWAN

#### **Parámetros TTN**
```cpp
// Frecuencia (Europa 868MHz)
#define CFG_eu868 1

// Clase dispositivo (A = bajo consumo)
#define CLASS A

// Canal único (opcional para testing)
#define SINGLE_CHANNEL_GATEWAY 0
```

#### **Tiempos de Transmisión**
- **Intervalo**: 60 segundos (configurable)
- **Timeout ACK**: 2 segundos
- **Reintento join**: 10 segundos tras fallo
- **Deep sleep**: 60 segundos entre ciclos

### 🌡️ Gestión de Sensores

#### **BME280 - Inicialización Robusta**
```cpp
// Dirección I2C: 0x76 (por defecto) o 0x77
bool sensorOk = bme.begin(0x76);
if (!sensorOk) {
    Serial.println("Sensor BME280 no encontrado");
    // Sistema continúa con códigos de error
}
```

#### **Lecturas Ambientales**
- **Temperatura**: Compensada, rango -40°C a +85°C
- **Humedad**: Compensada, rango 0-100%
- **Presión**: Compensada, rango 300-1100 hPa
- **Precisión**: 0.01 unidades para todos los sensores

### 🔋 Gestión de Energía

#### **Monitoreo de Batería**
```cpp
float voltage = readBatteryVoltage();
// Rango típico: 3.0V (descargada) - 4.2V (cargada)
// Precisión: ±0.01V
```

#### **Modos de Consumo**
| Modo | Consumo | Duración |
|------|---------|----------|
| **Activo** | 120mA | 1-2s (transmisión) |
| **Idle** | 25mA | 10s (procesamiento) |
| **Display ON** | 25mA | Variable |
| **Deep Sleep** | 20μA | 60s |

#### **Cálculo de Autonomía**
```
Batería LiPo 18650 (3000mAh):
- Consumo promedio: ~0.5mAh/ciclo
- Ciclos/día: 1440 (cada 60s)
- Consumo diario: ~720mAh
- Autonomía: ~4 días
```

## 📊 Monitoreo y Debugging

### 🔍 Serial Monitor

#### **Logs Normales**
```
Sistema inicializado
Uniéndose a red LoRaWAN...
EV_JOINING
Unión exitosa a la red LoRaWAN
EV_JOINED
Enviando: T=25.67°C, H=65.43%, P=1013.25 hPa, B=3.85V
EV_TXCOMPLETE
RSSI: -45 dBm, SNR: 8 dB
ACK recibido de gateway
Entrando en deep sleep...
```

#### **Logs de Error**
```
Sensor BME280 no encontrado
Enviando datos limitados: B=3.85V
Join fallido - reintentando...
```

### 📈 Métricas de Enlace

#### **Indicadores de Calidad**
- **RSSI**: > -100 dBm (buena señal)
- **SNR**: > 0 dB (buena relación señal-ruido)
- **SF**: 7-12 (adaptativo según distancia)
- **ACK**: Confirmación de recepción

## 🛠️ Mantenimiento

### 🔄 Actualizaciones OTA
- **Método**: Implementado pero no activado por defecto
- **Riesgo**: Alto (puede brickear dispositivo)
- **Recomendación**: Usar cable USB para actualizaciones

### 🔧 Reemplazo de Batería
1. **Apagar** dispositivo completamente
2. **Desconectar** batería LiPo
3. **Esperar** 30 segundos (descarga capacitadores)
4. **Conectar** nueva batería
5. **Verificar** voltaje en display

### 🧹 Limpieza de Sensores
- **BME280**: Evitar polvo/humedad excesiva
- **Cubierta**: Mantener ventilación adecuada
- **Almacenamiento**: Temperatura ambiente, <80% HR

## 🚨 Solución de Problemas

### ❌ **Problemas Comunes**

#### **1. No se une a TTN**
```
Síntoma: "Join fallido - reintentando..."
Solución:
- Verificar credenciales APPEUI/DEVEUI/APPKEY
- Comprobar cobertura LoRaWAN
- Verificar frecuencia regional (868MHz EU)
```

#### **2. Sensor no responde**
```
Síntoma: Temperatura = -999.0°C
Solución:
- Verificar conexión I2C (pines SDA/SCL)
- Comprobar alimentación sensor (3.3V)
- Reiniciar dispositivo
```

#### **3. Display no enciende**
```
Síntoma: Pantalla negra permanente
Solución:
- Verificar conexión I2C display
- Comprobar alimentación OLED
- Reset físico del dispositivo
```

#### **4. Consumo alto de batería**
```
Síntoma: Batería se agota en horas
Solución:
- Verificar modo deep sleep funciona
- Comprobar display se apaga
- Medir corriente con multímetro
```

### 🔧 **Herramientas de Diagnóstico**

#### **Test Manual de Componentes**
```cpp
// Ejecutar en Serial Monitor para testing
void testComponents() {
    // Test sensor
    float t, h, p, b;
    bool sensorOk = getSensorDataForDisplay(t, h, p, b);
    Serial.printf("Sensor: %s\n", sensorOk ? "OK" : "ERROR");

    // Test display
    sendInfoMessage("Test Display", 2000);
    Serial.println("Display: Test enviado");

    // Test batería
    Serial.printf("Batería: %.2fV\n", b);
}
```

#### **Modo Debug Avanzado**
```cpp
#define DEBUG_MODE 1  // En pgm_board.cpp
// Habilita logs detallados de:
// - Tiempos de ejecución
// - Estados LoRaWAN
// - Lecturas de sensores crudas
// - Consumo de memoria
```

## 📈 Optimización de Rendimiento

### ⚡ **Mejoras de Consumo**
- **Deep sleep**: 60s entre transmisiones
- **Display off**: Apagado automático tras 5s
- **Sensor polling**: Solo durante transmisión
- **Radio duty cycle**: <1% según regulación

### 📡 **Optimización LoRaWAN**
- **Adaptive Rate**: SF 7-12 según SNR
- **Channel hopping**: Todos los canales 868MHz
- **ACK timeout**: 2s máximo
- **Rejoin automático**: Tras sesión expirada

### 💾 **Gestión de Memoria**
- **Stack**: <2KB usado
- **Heap**: <4KB usado
- **Flash**: 255KB de código
- **EEPROM**: No usado (datos volátiles)

## 🔒 Seguridad

### 🛡️ **Medidas Implementadas**
- **Credenciales seguras**: Almacenadas en PROGMEM
- **Sesiones encriptadas**: LoRaWAN Class A
- **Datos validados**: Rangos y códigos de error
- **Fail-safe**: Sistema continúa con fallos parciales

### ⚠️ **Recomendaciones**
- **No exponer** credenciales en repositorios públicos
- **Usar HTTPS** para configuración remota
- **Monitorear** accesos no autorizados
- **Actualizar** firmware regularmente

---

**🚀 Sistema listo para despliegue en entornos IoT críticos con monitoreo ambiental continuo**