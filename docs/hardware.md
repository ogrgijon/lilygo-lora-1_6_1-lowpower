# Componentes Hardware

El proyecto usa una placa LilyGo T3-S3 con sensores para medir temperatura, humedad y enviar datos por LoRaWAN.

## Placa Principal: LilyGo T3-S3

- **Microcontrolador**: ESP32-S3 (240MHz, WiFi, Bluetooth)
- **Módulo LoRa**: SX1276/SX1278 (868MHz para Europa)
- **Sensor**: AM2302 (temperatura, humedad)
- **PMU**: AXP2101/AXP192 (gestión de batería)
- **Display**: OLED SSD1306 opcional
- **Conectores**: USB-C, antena SMA, batería LiPo

## Especificaciones Clave

| Componente | Detalles |
|------------|----------|
| **ESP32-S3** | Procesador dual-core, 16MB Flash |
| **LoRa** | Alcance hasta 10km, frecuencia 868MHz |
| **AM2302** | Temperatura: -40°C a 80°C, Humedad: 0-100% |
| **Batería** | LiPo 3.7V, autonomía ~6-8 meses |
| **Consumo** | Activo: 100mA, Sueño: 20μA |

## Conexiones

- **Antena LoRa**: Conectar a conector SMA
- **Batería**: Conectar a conector JST-PH
- **USB**: Para programación y alimentación

## Pines Principales

| Función | GPIO |
|---------|------|
| LoRa SPI | 5, 3, 6, 7 |
| I2C Sensores | 17, 18 |
| Display | I2C 0x3C |
| Batería ADC | 35 |

## Alimentación

- **Entrada**: USB 5V o batería 3.7V
- **Reguladores**: PMU controla voltajes para cada componente
- **Protecciones**: Sobrecarga, descarga profunda

## ☀️ Sistema de Carga Solar

### 🎯 **Funcionalidad**

El sistema implementa monitoreo completo de carga solar integrada con el PMU AXP2101, permitiendo detectar cuando la batería se está cargando a través de paneles solares conectados.

### 🔌 **Conexiones de Hardware**

#### **Entrada Solar VBUS**
- **Pin físico**: Conector USB-C adicional o pines VBUS dedicados
- **Voltaje**: 5V DC típico de paneles solares
- **Corriente**: Hasta 1A (dependiendo del panel)
- **Protección**: Diodo Schottky integrado en PMU

#### **Conexión al PMU AXP2101**
```
Panel Solar → USB-C/VBUS → AXP2101 → Batería LiPo
                    ↑
                Monitoreo ADC
```

### ⚙️ **Funcionamiento del Sistema**

#### **Detección de Carga Solar**
```cpp
// Función principal de detección
bool isSolarChargingBattery() {
    // Verificar si hay voltaje en VBUS
    if (PMU->isVbusIn()) {
        // Verificar si la batería está siendo cargada
        return PMU->isBatteryCharging();
    }
    return false;
}
```

#### **Estados de Carga Detallados**
```cpp
// Estados posibles de carga
enum ChargeStatus {
    CHARGE_NONE,       // No hay carga
    CHARGE_PRE,        // Pre-carga (voltaje bajo)
    CHARGE_CC,         // Carga constante (corriente constante)
    CHARGE_CV,         // Carga por voltaje (voltaje constante)
    CHARGE_DONE        // Carga completa
};
```

#### **Monitoreo en Tiempo Real**
```cpp
bool getSolarChargeStatus() {
    // Verificar conexión VBUS
    if (!PMU->isVbusIn()) {
        return false;  // No hay entrada solar
    }
    
    // Verificar estado de carga de batería
    return PMU->isBatteryCharging();
}

void checkSolarStatus() {
    static bool lastSolarState = false;
    bool currentSolarState = getSolarChargeStatus();
    
    if (currentSolarState != lastSolarState) {
        if (currentSolarState) {
            Serial.println("☀️ Carga solar DETECTADA");
        } else {
            Serial.println("☀️ Carga solar FINALIZADA");
        }
        lastSolarState = currentSolarState;
    }
}
```

### 📊 **Integración con Payload LoRaWAN**

#### **Formato de Datos**
```cpp
// Payload de 7 bytes incluye estado solar
uint8_t payload[7] = {
    temp_high, temp_low,     // Temperatura (°C * 100)
    hum_high, hum_low,       // Humedad (% * 100)
    batt_high, batt_low,     // Batería (V * 100)
    solar_status             // 0=no carga, 1=carga solar activa
};
```

#### **Decoder TTN**
```javascript
function decodeUplink(input) {
    var data = {};
    var bytes = input.bytes;
    
    // Temperatura, humedad, batería...
    data.temperature = ((bytes[0] << 8) | bytes[1]) / 100.0;
    data.humidity = ((bytes[2] << 8) | bytes[3]) / 100.0;
    data.battery_voltage = ((bytes[4] << 8) | bytes[5]) / 100.0;
    
    // Estado solar
    data.solar_charging = bytes[6] === 1;
    data.power_source = data.solar_charging ? "Solar" : "Battery only";
    
    return { data: data };
}
```

### 🔋 **Características Técnicas**

#### **Especificaciones del PMU AXP2101**
- **Voltaje entrada**: 4.5V - 5.5V (VBUS)
- **Corriente máxima**: 1A (configurable)
- **Eficiencia**: >90% en modo carga
- **Protecciones**: Sobretensión, sobrecorriente, temperatura

#### **Estados de Carga de Batería**
| Estado | Descripción | Corriente | Voltios |
|--------|-------------|-----------|---------|
| **Pre-charge** | Batería muy baja (<2.9V) | Baja | 2.9V |
| **Constant Current** | Carga rápida | Máxima | Variable |
| **Constant Voltage** | Carga final | Decreciente | 4.2V |
| **Charge Done** | Carga completa | ~0 | 4.2V |

### 📱 **Interfaz de Usuario**

#### **Indicadores en Display OLED**
- **Carga solar activa**: Muestra batería + indicador solar
- **Solo batería**: Display normal sin indicador solar
- **Mensajes de estado**: "Carga solar ON/OFF" en eventos

#### **Logs del Sistema**
```cpp
Serial.println("☀️ Carga solar DETECTADA - Panel conectado");
Serial.println("🔋 Alimentación por batería - Panel desconectado");
Serial.println("⚡ Estado de carga: PRE-CHARGE / CC / CV / DONE");
```

### 🔧 **Configuración y Setup**

#### **Conexión Física**
1. **Panel Solar** → Cable USB con conector tipo C
2. **PMU AXP2101** → Detecta automáticamente VBUS
3. **ESP32-S3** → Lee estado vía I2C del PMU

#### **Configuración Software**
```cpp
// En LoRaBoards.cpp - inicialización PMU
bool initPMU() {
    if (!PMU) return false;
    
    // Configurar detección VBUS
    PMU->setVbusVoltageLimit(XPOWERS_AXP192_VBUS_VOL_LIM_4V5);
    
    // Habilitar monitoreo de carga
    PMU->enableBattDetection();
    PMU->enableVbusVoltageMeasure();
    
    return true;
}
```

#### **Parámetros Ajustables**
```cpp
// Umbrales de voltaje (ajustables según panel solar)
#define SOLAR_MIN_VOLTAGE 4.0f      // V - voltaje mínimo para detección
#define SOLAR_MAX_VOLTAGE 5.5f      // V - voltaje máximo soportado

// Configuración de corriente de carga
#define SOLAR_CHARGE_CURRENT 500    // mA - corriente de carga solar
```

### 📊 **Monitoreo y Analytics**

#### **Métricas de Rendimiento**
- **Tiempo de carga**: Horas de sol efectivo por día
- **Eficiencia**: Energía solar convertida vs disponible
- **Autonomía**: Días de operación con carga solar
- **Estado batería**: Nivel de carga mantenido

#### **Dashboard TTN**
```javascript
// Visualización en TTN
if (data.solar_charging) {
    return {
        solar_status: "🟢 ACTIVE",
        battery_trend: "↗️ CHARGING",
        power_source: "Solar + Battery"
    };
} else {
    return {
        solar_status: "🔴 INACTIVE", 
        battery_trend: "↘️ DISCHARGING",
        power_source: "Battery only"
    };
}
```

### 🛠️ **Mantenimiento y Troubleshooting**

#### **Verificación del Sistema**
```cpp
void testSolarSystem() {
    Serial.println("=== SOLAR SYSTEM TEST ===");
    
    // Verificar VBUS
    bool vbus = PMU->isVbusIn();
    Serial.printf("VBUS Present: %s\n", vbus ? "YES" : "NO");
    
    // Verificar carga
    bool charging = PMU->isBatteryCharging();
    Serial.printf("Battery Charging: %s\n", charging ? "YES" : "NO");
    
    // Verificar voltaje VBUS
    float vbusVolt = PMU->getVbusVoltage();
    Serial.printf("VBUS Voltage: %.2fV\n", vbusVolt);
    
    // Verificar voltaje batería
    float battVolt = PMU->getBattVoltage();
    Serial.printf("Battery Voltage: %.2fV\n", battVolt);
}
```

#### **Problemas Comunes**
- **No detecta panel**: Verificar conexión USB-C física
- **Carga intermitente**: Panel solar con poca irradiancia
- **Voltaje bajo**: Panel dañado o sombreado
- **PMU no responde**: Reiniciar sistema o verificar I2C

#### **Optimización**
- **Orientación panel**: Máxima exposición solar
- **Limpieza**: Panel libre de polvo/suciedad
- **Conexiones**: Cables en buen estado
- **Configuración**: Ajustar corriente según capacidad del panel

### 🎯 **Beneficios del Sistema**

- ✅ **Energía renovable**: Carga automática con panel solar
- ✅ **Monitoreo remoto**: Estado de carga visible en TTN
- ✅ **Autonomía extendida**: Días/meses de operación continua
- ✅ **Bajo mantenimiento**: Sistema completamente automático
- ✅ **Transparencia**: Usuario informado del estado energético

### 🔗 **Integración con Otros Sistemas**

#### **Con Batería Principal**
- **Prioridad**: Carga solar tiene prioridad sobre USB
- **Conmutación**: Automática entre fuentes de energía
- **Protección**: PMU previene sobrecargas

#### **Con Display OLED**
- **Indicadores**: Iconos para estado de carga solar
- **Mensajes**: Notificaciones de cambios de estado
- **Ahorro energía**: Display se apaga durante carga nocturna

#### **Con LoRaWAN**
- **Payload expandido**: Incluye estado solar en transmisiones
- **Frecuencia**: Más transmisiones cuando hay sol disponible
- **Analytics**: Datos históricos de generación solar

---

**☀️ Sistema de carga solar proporciona autonomía energética completa con monitoreo remoto avanzado**