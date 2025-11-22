# 🔧 Guía de Solución de Problemas - Sistema IoT LoRaWAN Modular

## 🎯 Diagnóstico Sistemático

Esta guía proporciona un **enfoque estructurado** para resolver problemas en el sistema IoT LoRaWAN modular. Sigue el orden de diagnóstico recomendado para una resolución eficiente.

## 📋 Checklist de Diagnóstico Inicial

### ✅ **Verificación Básica del Hardware**
- [ ] **Alimentación**: LED azul parpadea al conectar USB
- [ ] **OLED Display**: Muestra "Sistema Iniciado" en los primeros 3 segundos
- [ ] **Serial Monitor**: Logs aparecen a 115200 baud
- [ ] **Batería**: Voltaje > 3.0V (mostrado en display)

### ✅ **Verificación de Conectividad**
- [ ] **USB**: Dispositivo reconocido en Device Manager (Windows)
- [ ] **PlatformIO**: Compilación exitosa sin errores
- [ ] **Upload**: Proceso de subida completa sin timeouts
- [ ] **Serial**: Comunicación bidireccional funciona

## 🚨 Problemas Críticos

### 🔥 **1. Sistema No Inicia (Pantalla Negra Permanente)**

#### **Síntomas**
```
- Display OLED completamente negro
- No hay logs en Serial Monitor
- LED azul no parpadea
```

#### **Diagnóstico Paso a Paso**
1. **Hardware básico**
   ```bash
   # Verificar alimentación
   Multímetro: 3.3V en pines de ESP32
   ```

2. **Conexión USB**
   ```bash
   # Windows: Device Manager
   Buscando: "Silicon Labs CP210x USB to UART Bridge"
   # Linux/Mac: dmesg | grep tty
   ```

3. **Código básico**
   ```cpp
   // Agregar al inicio de setup()
   Serial.begin(115200);
   Serial.println("=== DIAGNOSTICO INICIAL ===");
   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, HIGH); // LED on
   delay(1000);
   ```

#### **Soluciones**
- **Cable USB**: Probar cable diferente/certificado
- **Puerto USB**: Cambiar puerto o usar USB 2.0
- **Drivers**: Instalar CP210x desde Silicon Labs
- **ESP32**: Verificar no está brickeado (bootloader)

---

### 📡 **2. Fallo de Join LoRaWAN**

#### **Síntomas**
```
EV_JOINING
Join fallido - reintentando...
EV_JOIN_FAILED (repetitivo)
```

#### **Diagnóstico Paso a Paso**
1. **Credenciales TTN**
   ```cpp
   // Verificar en pgm_board.cpp
   static const u1_t PROGMEM APPEUI[8] = {TU_APPEUI_AQUI};
   static const u1_t PROGMEM DEVEUI[8] = {TU_DEVEUI_AQUI};
   static const u1_t PROGMEM APPKEY[16] = {TU_APPKEY_AQUI};
   ```

2. **Configuración regional**
   ```cpp
   // Verificar defines
   #define CFG_eu868 1  // Europa 868MHz
   #define CLASS A     // Bajo consumo
   ```

3. **Cobertura LoRaWAN**
   ```bash
   # Verificar gateways cercanos
   https://www.thethingsnetwork.org/map
   ```

#### **Soluciones**
- **Credenciales**: Copiar exactamente desde TTN Console
- **Formato**: Verificar endianness (LSB first)
- **Gateway**: Mover dispositivo más cerca de gateway
- **Frecuencia**: Confirmar región EU868

---

### 🌡️ **3. Sensor DHT22 No Responde**

#### **Síntomas**
```
Sensor DHT22 no encontrado
Temperatura: -999.0°C
Humedad: -1.0%
```

#### **Diagnóstico Paso a Paso**
1. **Conexión física**
   ```
   ESP32 ←→ DHT22
   3.3V ←→ VCC
   GND  ←→ GND
   GPIO 13 ←→ DATA (señal digital)
   GPIO 12 ←→ POWER (control alimentación)
   ```

2. **Configuración de pines**
   ```cpp
   // Verificar configuración en sensor_config.h
   #define DHT_PIN 13                    // Pin de datos
   #define DHT_TYPE DHT22                // Tipo de sensor
   #define DHT_POWER_PIN 12              // Pin de alimentación
   #define DHT_POWER_ON_DELAY_MS 2000    // Tiempo de estabilización
   ```

3. **Test de comunicación**
   ```cpp
   // Función de diagnóstico para DHT22
   void testDHT22() {
       // Encender sensor
       pinMode(DHT_POWER_PIN, OUTPUT);
       digitalWrite(DHT_POWER_PIN, HIGH);
       delay(DHT_POWER_ON_DELAY_MS);

       // Intentar lectura
       DHT dht(DHT_PIN, DHT_TYPE);
       dht.begin();

       float temp = dht.readTemperature();
       float hum = dht.readHumidity();

       if (isnan(temp) || isnan(hum)) {
           Serial.println("DHT22: Error de comunicación");
       } else {
           Serial.printf("DHT22: OK - Temp: %.1f°C, Hum: %.1f%%\n", temp, hum);
       }

       // Apagar sensor
       digitalWrite(DHT_POWER_PIN, LOW);
   }
   ```

#### **Soluciones**
- **Conexión**: Verificar cableado digital (no I2C)
- **Alimentación**: 3.3V estable con control de power
- **Tiempo**: Esperar 2 segundos después de encender
- **Tipo**: Confirmar DHT22 (no DHT11)

---

### 🖥️ **4. Display OLED No Funciona**

#### **Síntomas**
```
- Pantalla negra
- Mensajes no aparecen
- Sistema funciona pero sin feedback visual
```

#### **Diagnóstico Paso a Paso**
1. **Conexión física**
   ```
   ESP32 ←→ OLED SSD1306
   3.3V ←→ VCC
   GND  ←→ GND
   SDA  ←→ SDA (GPIO 17)
   SCL  ←→ SCL (GPIO 18)
   ```

2. **Dirección I2C**
   ```cpp
   // Verificar dirección correcta
   #define OLED_ADDRESS 0x3C  // SSD1306
   // o 0x3D para algunos módulos
   ```

3. **Tipo de display**
   ```cpp
   // Verificar configuración U8g2
   U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
   ```

4. **Test básico**
   ```cpp
   void testDisplay() {
       if (!u8g2) {
           Serial.println("Display no inicializado");
           return;
       }
       u8g2->clearBuffer();
       u8g2->drawStr(0, 20, "TEST DISPLAY");
       u8g2->sendBuffer();
       Serial.println("Test display enviado");
   }
   ```

#### **Soluciones**
- **Dirección**: Probar 0x3C y 0x3D
- **Tipo**: Confirmar SSD1306 (no SH1106)
- **Contraste**: Ajustar con `u8g2->setContrast(255)`
- **Reset**: Reset físico del ESP32

---

### 🔋 **5. Consumo Alto de Batería**

#### **Síntomas**
```
- Batería se agota en horas/días
- Display nunca se apaga
- No entra en deep sleep
```

#### **Diagnóstico Paso a Paso**
1. **Medición de corriente**
   ```cpp
   // Agregar mediciones de debug
   void measureCurrent() {
       float voltage = readBatteryVoltage();
       Serial.printf("Voltaje batería: %.2fV\n", voltage);

       // Medir corriente con multímetro
       Serial.println("Modo activo - medir corriente...");
       delay(5000);
   }
   ```

2. **Verificación deep sleep**
   ```cpp
   // Verificar antes de deep sleep
   Serial.println("Entrando en deep sleep...");
   Serial.flush();
   delay(100); // Esperar transmisión serial
   esp_deep_sleep_start();
   ```

3. **Gestión de componentes**
   ```cpp
   // Verificar apagado de periféricos
   void powerDownPeripherals() {
       // Apagar sensor
       // Apagar display
       // Apagar radio (automático)
       Serial.println("Periféricos apagados");
   }
   ```

#### **Soluciones**
- **Deep sleep**: Verificar `esp_deep_sleep_start()` se ejecuta
- **Display**: Confirmar auto-apagado tras mensajes
- **Radio**: Duty cycle <1% (regulación LoRaWAN)
- **Batería**: Verificar capacidad y estado

---

### 📊 **6. Datos No Llegan a TTN**

#### **Síntomas**
```
EV_TXCOMPLETE
ACK recibido de gateway
# Pero no aparecen datos en TTN Console
```

#### **Diagnóstico Paso a Paso**
1. **Payload format**
   ```cpp
   // Verificar formato 6 bytes
   uint8_t payload[6];
   uint8_t size = getSensorPayload(payload, sizeof(payload));
   Serial.printf("Payload size: %d bytes\n", size);
   ```

2. **Decoder TTN**
   ```javascript
   // Decoder actualizado para 6 bytes
   function decodeUplink(input) {
     var bytes = input.bytes;
     return {
       temperature: ((bytes[0] << 8) | bytes[1]) / 100.0,
       humidity: ((bytes[2] << 8) | bytes[3]) / 100.0,
       battery: ((bytes[4] << 8) | bytes[5]) / 100.0
     };
   }
   ```

3. **Frecuencia y canal**
   ```cpp
   // Verificar configuración LoRaWAN
   #define CFG_eu868 1
   // Verificar canal único si es necesario
   #define SINGLE_CHANNEL_GATEWAY 0
   ```

#### **Soluciones**
- **Decoder**: Actualizar para 6 bytes (temperatura, humedad, batería)
- **Escala**: Verificar división por 100
- **TTN Console**: Confirmar dispositivo registrado y activo
- **Gateway**: Verificar recepción de uplinks

---

## 🛠️ Herramientas de Diagnóstico Avanzado

### 🔍 **Modo Debug Completo**
```cpp
// Agregar al inicio de pgm_board.cpp
#define DEBUG_MODE 1

#if DEBUG_MODE
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(...)
#endif
```

### 📊 **Monitor de Rendimiento**
```cpp
void performanceMonitor() {
    static uint32_t lastCheck = 0;
    if (millis() - lastCheck > 10000) { // Cada 10s
        DEBUG_PRINTF("Free heap: %d bytes\n", ESP.getFreeHeap());
        DEBUG_PRINTF("Uptime: %d seconds\n", millis() / 1000);
        lastCheck = millis();
    }
}
```

### 🔄 **Test Suite Automático**
```cpp
void runDiagnostics() {
    Serial.println("=== DIAGNOSTIC SUITE ===");

    // Test 1: Hardware básico
    Serial.println("1. Testing hardware...");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    // Test 2: Sensor
    Serial.println("2. Testing sensor...");
    float t, h, p, b;
    bool sensorOk = getSensorDataForDisplay(t, h, p, b);
    Serial.printf("   Sensor: %s\n", sensorOk ? "OK" : "FAIL");

    // Test 3: Display
    Serial.println("3. Testing display...");
    sendInfoMessage("Diagnostic Test", 2000);

    // Test 4: Batería
    Serial.println("4. Testing battery...");
    Serial.printf("   Voltage: %.2fV\n", b);

    Serial.println("=== DIAGNOSTIC COMPLETE ===");
}
```

## 📈 Métricas de Calidad del Sistema

### 📊 **Indicadores Normales**
| Métrica | Valor Normal | Valor de Alerta |
|---------|-------------|-----------------|
| **RSSI** | > -100 dBm | < -120 dBm |
| **SNR** | > 0 dB | < -10 dB |
| **Free Heap** | > 100KB | < 50KB |
| **Join Time** | < 30s | > 60s |
| **Battery Drain** | < 1%/hora | > 5%/hora |

### ⚡ **Perfiles de Consumo**
```
Activo (TX):     120mA - 2s
Procesamiento:    25mA - 8s
Display ON:       25mA - 5s
Deep Sleep:        0.02mA - 60s
Promedio ciclo:    0.5mAh
```

## 🚨 Procedimiento de Emergencia

### 🔥 **Sistema Completamente Inoperativo**
1. **Reset físico**: Mantener botón BOOT 5 segundos
2. **Cargar firmware básico**: Versión sin LoRaWAN
3. **Test componentes**: Uno por uno
4. **Restaurar configuración**: Credenciales por defecto

### 💾 **Recuperación de Firmware**
```cpp
// Código mínimo para recuperación
void setup() {
    Serial.begin(115200);
    Serial.println("RECOVERY MODE");
    // Solo inicialización básica
}

void loop() {
    // Bucle vacío - esperando comandos
}
```

## 📞 Soporte y Escalación

### 📧 **Información para Reportar Bugs**
```
Asunto: [BUG] Sistema IoT LoRaWAN - <descripción breve>

Descripción del problema:
- ¿Qué esperabas que pasara?
- ¿Qué pasó realmente?
- ¿Cuándo empezó el problema?

Entorno:
- Versión del código: <commit hash>
- Hardware: LilyGo T3-S3
- PlatformIO: <versión>
- TTN Region: EU868

Logs relevantes:
[Pegar logs del Serial Monitor]

Configuración TTN:
- Device EUI: <tu_device_eui>
- Application EUI: <tu_app_eui>
- Gateway cercano: <ubicación aproximada>
```

### 🔗 **Recursos Adicionales**
- [TTN Console](https://console.thethingsnetwork.org)
- [PlatformIO Documentation](https://docs.platformio.org)
- [ESP32 Troubleshooting](https://docs.espressif.com/projects/esp32)
- [LoRaWAN Regional Parameters](https://lora-alliance.org/resource_hub/lorawan-regional-parameters-v1-0-3reva/)

---

**🔧 Diagnóstico sistemático para resolución eficiente de problemas en sistemas IoT críticos**