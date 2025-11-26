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

### ⏱️ **2.1. Sistema de Backoff Exponencial para Joins**

#### **Síntomas**
```
EV_JOIN_FAILED
Esperando 10 segundos antes del siguiente intento...
EV_JOIN_FAILED
Esperando 2 minutos antes del siguiente intento...
EV_JOIN_FAILED
Esperando 5 minutos antes del siguiente intento...
Pantalla permanece encendida durante backoffs largos
Batería se agota rápidamente durante joins fallidos
```

#### **Diagnóstico Paso a Paso**
1. **Verificar implementación del backoff**
   ```cpp
   // Verificar en main.ino o pgm_board.cpp
   // Función de cálculo de tiempo de backoff
   uint32_t calculateBackoffTime(uint8_t attempt) {
       uint32_t baseTime = BACKOFF_BASE_TIME; // 10 segundos
       uint32_t maxTime = BACKOFF_MAX_TIME;   // 30 minutos
       uint32_t time = baseTime * (1 << attempt); // Exponencial
       return min(time, maxTime);
   }
   ```

2. **Verificar configuración de backoff**
   ```cpp
   // Verificar constantes definidas
   #define BACKOFF_BASE_TIME 10000        // 10 segundos
   #define BACKOFF_MAX_TIME 1800000       // 30 minutos
   #define BACKOFF_MAX_ATTEMPTS 5         // Máximo 5 intentos
   #define LIGHT_SLEEP_THRESHOLD 300000   // 5 minutos - usar light sleep
   ```

3. **Monitoreo del estado del backoff**
   ```cpp
   // Verificar variables de estado
   static uint8_t joinAttempts = 0;
   static uint32_t backoffStartTime = 0;
   static bool inBackoffPeriod = false;
   ```

4. **Verificar gestión de energía durante backoff**
   ```cpp
   // Verificar modo sleep durante backoff largo
   if (backoffTime >= LIGHT_SLEEP_THRESHOLD) {
       Serial.printf("Backoff largo (%d min) - usando light sleep\n",
                    backoffTime / 60000);
       enterLightSleep(backoffTime);
   } else {
       Serial.printf("Backoff corto (%d seg) - esperando activo\n",
                    backoffTime / 1000);
       delay(backoffTime);
   }
   ```

#### **Soluciones**
- **Configuración correcta**: Verificar constantes BACKOFF_* definidas
- **Implementación completa**: Asegurar función `calculateBackoffTime()` presente
- **Gestión de energía**: Confirmar light sleep para backoffs > 5 minutos
- **Reset de contador**: Join exitoso debe resetear `joinAttempts = 0`
- **Límite de intentos**: Máximo 5 intentos antes de reset completo

#### **Configuración Recomendada**
```cpp
// Configuración óptima para bajo consumo
#define BACKOFF_BASE_TIME 10000        // 10s - primer reintento
#define BACKOFF_MAX_TIME 1800000       // 30min - máximo backoff
#define BACKOFF_MAX_ATTEMPTS 5         // Reset después de 5 fallos
#define LIGHT_SLEEP_THRESHOLD 300000   // 5min - cambiar a light sleep
```

#### **Monitoreo del Sistema de Backoff**
```cpp
void printBackoffStatus() {
    Serial.println("=== ESTADO BACKOFF ===");
    Serial.printf("Intentos de join: %d/%d\n", joinAttempts, BACKOFF_MAX_ATTEMPTS);
    Serial.printf("En período backoff: %s\n", inBackoffPeriod ? "SÍ" : "NO");
    if (inBackoffPeriod) {
        uint32_t elapsed = millis() - backoffStartTime;
        uint32_t remaining = calculateBackoffTime(joinAttempts) - elapsed;
        Serial.printf("Tiempo restante: %d segundos\n", remaining / 1000);
    }
    Serial.println("=====================");
}
```

#### **Problemas Comunes y Soluciones**
- **Pantalla siempre encendida**: Verificar `LIGHT_SLEEP_THRESHOLD` y `enterLightSleep()`
- **Batería se agota**: Asegurar deep sleep normal cuando no hay backoff
- **Backoff no progresa**: Verificar contador `joinAttempts` se incrementa
- **Reset no funciona**: Confirmar `joinAttempts = 0` en join exitoso

---

### 🌡️ **3. Sensores No Responden**

#### **Síntomas**
```
Sensor DHT22 no encontrado
Sensor DS18B20 no encontrado
Sensor BMP280 no encontrado
Sensor HC-SR04 no encontrado
Temperatura: -999.0°C
Humedad: -1.0%
Presión: -1.0 hPa
Distancia: -1.0 cm
```

#### **Diagnóstico Paso a Paso**
1. **Verificar configuración de sensores activos**
   ```cpp
   // Verificar en config/config.h
   #define ENABLE_SENSOR_DHT22     true   // Temperatura + humedad
   #define ENABLE_SENSOR_DS18B20   false  // Temperatura OneWire
   #define ENABLE_SENSOR_BMP280    true   // Presión + temperatura
   #define ENABLE_SENSOR_HCSR04    false  // Distancia ultrasónico
   ```

2. **Conexiones físicas por sensor**

   **DHT22/DHT11:**
   ```
   ESP32 ←→ DHT22
   3.3V ←→ VCC
   GND  ←→ GND
   GPIO 13 ←→ DATA (señal digital)
   GPIO 12 ←→ POWER (control alimentación)
   ```

   **DS18B20 (OneWire):**
   ```
   ESP32 ←→ DS18B20
   3.3V ←→ VCC
   GND  ←→ GND
   GPIO 14 ←→ DATA (OneWire bus)
   4.7KΩ ←→ DATA + VCC (pull-up resistor)
   ```

   **BMP280 (I2C):**
   ```
   ESP32 ←→ BMP280
   3.3V ←→ VCC
   GND  ←→ GND
   GPIO 17 ←→ SDA
   GPIO 18 ←→ SCL
   ```

   **HC-SR04 (Ultrasonic):**
   ```
   ESP32 ←→ HC-SR04
   5V ←→ VCC (desde PMU)
   GND ←→ GND
   GPIO 25 ←→ TRIG
   GPIO 26 ←→ ECHO
   ```

3. **Configuración de pines**
   ```cpp
   // Verificar configuración en config.h
   #define DHT_PIN 13                    // Pin de datos DHT
   #define DHT_TYPE DHT22                // Tipo de sensor DHT
   #define DHT_POWER_PIN 12              // Pin de alimentación DHT
   #define DHT_POWER_ON_DELAY_MS 2000    // Tiempo de estabilización

   #define ONE_WIRE_BUS 14               // Pin OneWire para DS18B20
   #define DS18B20_RESOLUTION 12         // Resolución 12-bit

   #define BMP280_I2C_ADDRESS 0x76       // Dirección I2C BMP280
   #define BMP280_SEA_LEVEL_PRESSURE 1013.25 // Presión nivel del mar

   #define HCSR04_TRIG_PIN 25            // Pin trigger HC-SR04
   #define HCSR04_ECHO_PIN 26            // Pin echo HC-SR04
   #define HCSR04_MAX_DISTANCE 400       // Distancia máxima (cm)
   ```

4. **Test de comunicación por sensor**
   ```cpp
   // Función de diagnóstico completa para todos los sensores
   void testAllSensors() {
       Serial.println("=== DIAGNOSTICO SENSORES ===");

       // Test DHT22
       #if ENABLE_SENSOR_DHT22
       Serial.println("Testing DHT22...");
       pinMode(DHT_POWER_PIN, OUTPUT);
       digitalWrite(DHT_POWER_PIN, HIGH);
       delay(DHT_POWER_ON_DELAY_MS);
       DHT dht(DHT_PIN, DHT_TYPE);
       dht.begin();
       float temp_dht = dht.readTemperature();
       float hum = dht.readHumidity();
       if (isnan(temp_dht) || isnan(hum)) {
           Serial.println("❌ DHT22: Error de comunicación");
       } else {
           Serial.printf("✅ DHT22: OK - Temp: %.1f°C, Hum: %.1f%%\n", temp_dht, hum);
       }
       digitalWrite(DHT_POWER_PIN, LOW);
       #else
       Serial.println("⏭️ DHT22: Deshabilitado");
       #endif

       // Test DS18B20
       #if ENABLE_SENSOR_DS18B20
       Serial.println("Testing DS18B20...");
       OneWire oneWire(ONE_WIRE_BUS);
       DallasTemperature sensors(&oneWire);
       sensors.begin();
       sensors.requestTemperatures();
       float temp_ds = sensors.getTempCByIndex(0);
       if (temp_ds == DEVICE_DISCONNECTED_C) {
           Serial.println("❌ DS18B20: No encontrado");
       } else {
           Serial.printf("✅ DS18B20: OK - Temp: %.1f°C\n", temp_ds);
       }
       #else
       Serial.println("⏭️ DS18B20: Deshabilitado");
       #endif

       // Test BMP280
       #if ENABLE_SENSOR_BMP280
       Serial.println("Testing BMP280...");
       Adafruit_BMP280 bmp;
       if (!bmp.begin(BMP280_I2C_ADDRESS)) {
           Serial.println("❌ BMP280: No encontrado");
       } else {
           float pressure = bmp.readPressure() / 100.0F;
           float temp_bmp = bmp.readTemperature();
           Serial.printf("✅ BMP280: OK - Temp: %.1f°C, Pres: %.1f hPa\n", temp_bmp, pressure);
       }
       #else
       Serial.println("⏭️ BMP280: Deshabilitado");
       #endif

       // Test HC-SR04
       #if ENABLE_SENSOR_HCSR04
       Serial.println("Testing HC-SR04...");
       Ultrasonic ultrasonic(HCSR04_TRIG_PIN, HCSR04_ECHO_PIN);
       float distance = ultrasonic.read();
       if (distance < 0 || distance > HCSR04_MAX_DISTANCE) {
           Serial.println("❌ HC-SR04: Error de medición");
       } else {
           Serial.printf("✅ HC-SR04: OK - Dist: %.1f cm\n", distance);
       }
       #else
       Serial.println("⏭️ HC-SR04: Deshabilitado");
       #endif

       Serial.println("=== FIN DIAGNOSTICO SENSORES ===");
   }
   ```

#### **Soluciones**
- **Configuración**: Verificar defines `ENABLE_SENSOR_*` correctos
- **Conexiones**: Revisar cableado específico para cada sensor
- **Alimentación**: Sensores con control de power individual
- **Direcciones I2C**: Evitar conflictos (BMP280: 0x76 o 0x77)
- **Pull-up**: Resistor 4.7KΩ requerido para OneWire (DS18B20)
- **Tiempo**: Esperar estabilización después de encender sensores

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
- Carga solar no compensa el consumo
```

#### **Diagnóstico Paso a Paso**
1. **Medición de corriente con/sin solar**
   ```cpp
   // Medición completa incluyendo carga solar
   void measureCurrentConsumption() {
       float voltage = readBatteryVoltage();
       Serial.printf("Battery Voltage: %.2fV\n", voltage);
       
       // Verificar estado solar
       bool solarCharging = PMU->isVbusIn() && PMU->isBatteryCharging();
       Serial.printf("Solar Charging: %s\n", solarCharging ? "ACTIVE" : "INACTIVE");
       
       if (solarCharging) {
           float chargeCurrent = PMU->getBattChargeCurrent();
           Serial.printf("Charge Current: %.0fmA\n", chargeCurrent);
           Serial.println("⚡ Net consumption may be negative (charging)");
       }
       
       // Medir corriente en diferentes modos
       Serial.println("Testing different power modes...");
       
       // Modo activo
       Serial.println("Mode: ACTIVE - measure current...");
       delay(5000);
       
       // Modo sleep ligero
       Serial.println("Mode: LIGHT SLEEP - measure current...");
       enterLightSleep(10000); // 10 segundos
       
       // Deep sleep (requiere reset para medir)
       Serial.println("Mode: DEEP SLEEP - requires external measurement");
   }
   ```

2. **Verificación deep sleep**
   ```cpp
   // Verificar antes de deep sleep
   Serial.println("Preparing for deep sleep...");
   Serial.flush();
   delay(100); // Esperar transmisión serial
   
   // Apagar todos los periféricos
   powerDownPeripherals();
   
   // Configurar wake-up
   esp_sleep_enable_timer_wakeup(300000000ULL); // 5 minutos
   esp_deep_sleep_start();
   ```

3. **Gestión de componentes**
   ```cpp
   // Verificar apagado selectivo
   void powerDownPeripherals() {
       // Apagar sensor DHT22
       #if ENABLE_SENSOR_DHT22
       digitalWrite(DHT_POWER_PIN, LOW);
       #endif
       
       // Apagar sensor HC-SR04 (si usa power control)
       #if ENABLE_SENSOR_HCSR04
       // Nota: HC-SR04 no tiene control de power típico
       #endif
       
       // Apagar display
       u8g2->setPowerSave(1);
       
       // Apagar radio (automático en deep sleep)
       Serial.println("All peripherals powered down");
   }
   ```

4. **Análisis de ciclo de operación**
   ```cpp
   // Calcular consumo promedio por ciclo
   void analyzePowerCycle() {
       Serial.println("=== POWER CYCLE ANALYSIS ===");
       
       // Duración de cada fase (milisegundos)
       uint32_t activeTime = 2000;     // 2s medición + TX
       uint32_t lightSleepTime = 8000; // 8s procesamiento
       uint32_t displayTime = 5000;    // 5s display ON
       uint32_t deepSleepTime = 60000; // 60s deep sleep
       
       // Corrientes (mA)
       float activeCurrent = 120.0;
       float lightSleepCurrent = 10.0;
       float displayCurrent = 25.0;
       float deepSleepCurrent = 0.02;
       
       // Consumo por fase (mAh)
       float activeConsumption = (activeCurrent * activeTime) / 3600000.0;
       float lightSleepConsumption = (lightSleepCurrent * lightSleepTime) / 3600000.0;
       float displayConsumption = (displayCurrent * displayTime) / 3600000.0;
       float deepSleepConsumption = (deepSleepCurrent * deepSleepTime) / 3600000.0;
       
       float totalConsumption = activeConsumption + lightSleepConsumption + 
                               displayConsumption + deepSleepConsumption;
       
       Serial.printf("Active: %.4f mAh\n", activeConsumption);
       Serial.printf("Light Sleep: %.4f mAh\n", lightSleepConsumption);
       Serial.printf("Display: %.4f mAh\n", displayConsumption);
       Serial.printf("Deep Sleep: %.4f mAh\n", deepSleepConsumption);
       Serial.printf("Total per cycle: %.4f mAh\n", totalConsumption);
       
       // Calcular frecuencia para 1% batería/día (asumiendo 1000mAh batería)
       float dailyTarget = 10.0; // mAh/día (1% de 1000mAh)
       float cyclesPerDay = dailyTarget / totalConsumption;
       float cycleIntervalMinutes = 1440.0 / cyclesPerDay; // minutos
       
       Serial.printf("Cycles per day for 1%% battery drain: %.1f\n", cyclesPerDay);
       Serial.printf("Recommended cycle interval: %.0f minutes\n", cycleIntervalMinutes);
       
       Serial.println("=== END POWER ANALYSIS ===");
   }
   ```

#### **Soluciones**
- **Deep sleep**: Verificar `esp_deep_sleep_start()` se ejecuta correctamente
- **Display**: Confirmar auto-apagado después de mensajes (`setPowerSave(1)`)
- **Radio**: Duty cycle <1% (regulación LoRaWAN)
- **Sensores**: Apagar alimentación individual durante sleep
- **Carga solar**: Verificar compensación cuando hay sol disponible
- **Batería**: Verificar capacidad real y estado de salud

#### **Optimizaciones con Carga Solar**
- **Aumento frecuencia**: Más transmisiones cuando hay sol
- **Reducción sleep**: Ciclos más cortos durante carga activa
- **Monitoreo**: Alertas cuando carga solar no compensa consumo
- **Analytics**: Seguimiento de eficiencia energética neta

---

### ☀️ **5.1. Sistema de Carga Solar No Funciona**

#### **Síntomas**
```
Carga solar no detectada
Batería no se carga con panel solar
Estado solar siempre inactivo
```

#### **Diagnóstico Paso a Paso**
1. **Verificar conexión VBUS**
   ```cpp
   // Test básico de VBUS
   void testVBUS() {
       Serial.println("=== TEST VBUS ===");
       
       // Verificar si hay voltaje en VBUS
       bool vbusPresent = PMU->isVbusIn();
       Serial.printf("VBUS Present: %s\n", vbusPresent ? "YES" : "NO");
       
       if (vbusPresent) {
           // Verificar voltaje VBUS
           float vbusVolt = PMU->getVbusVoltage();
           Serial.printf("VBUS Voltage: %.2fV\n", vbusVolt);
           
           // Verificar si batería se está cargando
           bool charging = PMU->isBatteryCharging();
           Serial.printf("Battery Charging: %s\n", charging ? "YES" : "NO");
           
           // Verificar corriente de carga
           float chargeCurrent = PMU->getBattChargeCurrent();
           Serial.printf("Charge Current: %.0fmA\n", chargeCurrent);
       }
       
       Serial.println("=== FIN TEST VBUS ===");
   }
   ```

2. **Verificar configuración PMU**
   ```cpp
   // Verificar inicialización del PMU para carga solar
   bool initPMU() {
       if (!PMU) return false;
       
       // Configurar límites de voltaje VBUS
       PMU->setVbusVoltageLimit(XPOWERS_AXP192_VBUS_VOL_LIM_4V5);
       
       // Habilitar monitoreo
       PMU->enableBattDetection();
       PMU->enableVbusVoltageMeasure();
       PMU->enableBattVoltageMeasure();
       
       // Configurar corriente de carga (opcional)
       PMU->setChargeCurr(XPOWERS_AXP192_CHG_CUR_500MA);
       
       return true;
   }
   ```

3. **Test de panel solar**
   ```cpp
   // Función completa de diagnóstico solar
   void testSolarSystem() {
       Serial.println("=== DIAGNOSTICO SISTEMA SOLAR ===");
       
       // 1. Verificar VBUS
       bool vbus = PMU->isVbusIn();
       Serial.printf("1. VBUS Present: %s\n", vbus ? "YES" : "NO");
       
       if (!vbus) {
           Serial.println("❌ Panel solar no conectado o sin alimentación");
           Serial.println("   Verificar: Cable USB-C, panel solar, irradiancia");
           return;
       }
       
       // 2. Verificar voltaje VBUS
       float vbusVolt = PMU->getVbusVoltage();
       Serial.printf("2. VBUS Voltage: %.2fV (esperado: 4.5-5.5V)\n", vbusVolt);
       
       if (vbusVolt < 4.0) {
           Serial.println("❌ Voltaje VBUS bajo - panel dañado o sombreado");
       }
       
       // 3. Verificar estado de carga
       bool charging = PMU->isBatteryCharging();
       Serial.printf("3. Battery Charging: %s\n", charging ? "YES" : "NO");
       
       // 4. Verificar voltaje batería
       float battVolt = PMU->getBattVoltage();
       Serial.printf("4. Battery Voltage: %.2fV\n", battVolt);
       
       // 5. Verificar corriente de carga
       float chargeCurr = PMU->getBattChargeCurrent();
       Serial.printf("5. Charge Current: %.0fmA\n", chargeCurr);
       
       // 6. Verificar estado detallado
       int chargeStatus = PMU->getChargeStatus();
       Serial.printf("6. Charge Status: %d\n", chargeStatus);
       // 0: No charge, 1: Pre-charge, 2: CC, 3: CV, 4: Done
       
       Serial.println("=== FIN DIAGNOSTICO SOLAR ===");
   }
   ```

4. **Monitoreo en tiempo real**
   ```cpp
   // Función para monitoreo continuo
   void monitorSolarCharging() {
       static bool lastChargingState = false;
       static uint32_t lastCheck = 0;
       
       if (millis() - lastCheck > 5000) { // Cada 5 segundos
           bool currentState = PMU->isVbusIn() && PMU->isBatteryCharging();
           
           if (currentState != lastChargingState) {
               if (currentState) {
                   Serial.println("☀️ CARGA SOLAR DETECTADA");
                   sendInfoMessage("Solar ON", 2000);
               } else {
                   Serial.println("☀️ CARGA SOLAR FINALIZADA");
                   sendInfoMessage("Solar OFF", 2000);
               }
               lastChargingState = currentState;
           }
           
           lastCheck = millis();
       }
   }
   ```

#### **Soluciones**
- **Conexión física**: Verificar cable USB-C entre panel y placa
- **Panel solar**: Comprobar voltaje de salida (4.5-5.5V) con multímetro
- **Orientación**: Panel debe recibir luz solar directa
- **PMU configuración**: Verificar `setVbusVoltageLimit()` correcto
- **Corriente de carga**: Ajustar según capacidad del panel (100-1000mA)
- **Estado batería**: Si batería llena, carga puede no detectarse
- **Limpieza**: Panel libre de polvo/suciedad

#### **Problemas Comunes**
- **No detecta panel**: Cable USB defectuoso o panel sin voltaje
- **Carga intermitente**: Panel sombreado o nuboso
- **Voltaje bajo**: Panel dañado o conexiones sueltas
- **PMU no responde**: Reiniciar sistema o verificar I2C
- **Batería no carga**: Verificar estado de batería y límites de carga

---

### 📊 **6. Datos No Llegan a TTN**

#### **Síntomas**
```
EV_TXCOMPLETE
ACK recibido de gateway
# Pero no aparecen datos en TTN Console
```

#### **Diagnóstico Paso a Paso**
1. **Tamaño del payload dinámico**
   ```cpp
   // Verificar tamaño según sensores activos
   uint8_t payload[16]; // Máximo 16 bytes
   uint8_t size = getSensorPayload(payload, sizeof(payload));
   Serial.printf("Payload size: %d bytes\n", size);
   
   // Tamaños típicos:
   // 4 bytes: Solo batería
   // 6 bytes: Temperatura + batería
   // 8 bytes: Temperatura + humedad + batería
   // 10 bytes: + presión atmosférica
   // 12 bytes: + distancia
   // 14 bytes: + estado solar
   // 16 bytes: Todos los sensores + batería + solar
   ```

2. **Decoder TTN dinámico**
   ```javascript
   // Decoder actualizado para payload dinámico
   function decodeUplink(input) {
     var bytes = input.bytes;
     var data = {};
     var offset = 0;
     
     // Siempre incluye batería (últimos 2 bytes)
     var batteryIndex = bytes.length - 2;
     data.battery_voltage = ((bytes[batteryIndex] << 8) | bytes[batteryIndex + 1]) / 100.0;
     
     // Estado solar (si está presente - último byte)
     if (bytes.length % 2 === 0) { // Payload par incluye solar
       data.solar_charging = bytes[bytes.length - 1] === 1;
       batteryIndex--; // Batería está antes del estado solar
       data.battery_voltage = ((bytes[batteryIndex] << 8) | bytes[batteryIndex + 1]) / 100.0;
     }
     
     // Sensores según configuración
     if (bytes.length >= 2) {
       data.temperature = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
     }
     if (bytes.length >= 4) {
       data.humidity = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
     }
     if (bytes.length >= 6) {
       data.pressure = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
     }
     if (bytes.length >= 8) {
       data.distance = ((bytes[offset++] << 8) | bytes[offset++]) / 100.0;
     }
     
     return { data: data };
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
- **Decoder**: Actualizar para tamaño dinámico (4-16 bytes)
- **Escala**: Verificar división por 100 para temperatura/humedad
- **TTN Console**: Confirmar dispositivo registrado y activo
- **Gateway**: Verificar recepción de uplinks
- **Configuración sensores**: Verificar defines `ENABLE_SENSOR_*`

---

### 🐕 **7. Reinicio por Watchdog Timer**

#### **Síntomas**
```
rst:0x8 (TG1WDT_SYS_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
# Sistema se reinicia cada 5 minutos aproximadamente
# Logs muestran "ESP-ROM:esp32s3-..." seguido de reinicio
```

#### **Diagnóstico Paso a Paso**
1. **Confirmar reinicio por WDT**
   ```cpp
   // Agregar en setup() para detectar causa del reset
   #include <esp_system.h>
   
   void setup() {
       // ... código existente ...
       
       esp_reset_reason_t reset_reason = esp_reset_reason();
       Serial.print("Reset reason: ");
       switch(reset_reason) {
           case ESP_RST_POWERON:   Serial.println("Power-on"); break;
           case ESP_RST_SW:        Serial.println("Software"); break;
           case ESP_RST_PANIC:     Serial.println("Panic (exception)"); break;
           case ESP_RST_INT_WDT:   Serial.println("Interrupt WDT"); break;
           case ESP_RST_TASK_WDT:  Serial.println("Task WDT"); break;  // ← Este es el WDT
           case ESP_RST_WDT:       Serial.println("Other WDT"); break;
           case ESP_RST_DEEPSLEEP: Serial.println("Deep sleep"); break;
           default:                Serial.println("Unknown"); break;
       }
   }
   ```

2. **Verificar configuración del WDT**
   ```cpp
   // Verificar en main.ino
   esp_task_wdt_init(300, true);  // Debe ser 300 segundos
   esp_task_wdt_add(NULL);        // Debe estar presente
   ```

3. **Verificar alimentación del WDT**
   ```cpp
   // Verificar en loop()
   void loop() {
       loopLMIC();
       updateDisplay();
       esp_task_wdt_reset();  // ← Debe estar presente y ejecutándose
   }
   ```

4. **Analizar timing del loop**
   ```cpp
   // Agregar debug para timing
   void loop() {
       static uint32_t lastLoopTime = 0;
       uint32_t currentTime = millis();
       
       if (currentTime - lastLoopTime > 10000) {  // Warning cada 10s
           Serial.printf("Loop timing: %lu ms since last iteration\n", 
                        currentTime - lastLoopTime);
       }
       
       lastLoopTime = currentTime;
       
       loopLMIC();
       updateDisplay();
       esp_task_wdt_reset();
   }
   ```

#### **Causas Comunes de Reinicio por WDT**
- **Loop bloqueado**: Función `loopLMIC()` o `updateDisplay()` se cuelga
- **Stack overflow**: Recursión infinita o variables locales excesivas
- **Deadlock**: Espera infinita en funciones de LoRaWAN
- **Infinite loop**: Bucle sin salida en código de manejo de eventos
- **I2C/SPI hang**: Periférico I2C (sensor/display) deja de responder

#### **Soluciones**
- **Debug básico**: Agregar logs en puntos críticos del loop
- **Timing check**: Verificar que el loop se ejecuta al menos cada 4 minutos
- **Stack analysis**: Reducir variables locales grandes
- **I2C timeout**: Agregar timeouts en comunicaciones I2C
- **Exception handling**: Envolver código crítico en try-catch

#### **Configuración de Debug para WDT**
```cpp
// Modo debug - reducir timeout para testing rápido
#define WATCHDOG_DEBUG_MODE 1

void setup() {
    #if WATCHDOG_DEBUG_MODE
        esp_task_wdt_init(30, true);  // 30 segundos para debug
        Serial.println("WDT: Debug mode - 30s timeout");
    #else
        esp_task_wdt_init(300, true); // 5 minutos producción
        Serial.println("WDT: Production mode - 5min timeout");
    #endif
    esp_task_wdt_add(NULL);
}
```

#### **Test de WDT**
```cpp
// Función para testear WDT (usar con precaución)
void testWatchdogTrigger() {
    Serial.println("Testing WDT - system will reset in 5 minutes...");
    Serial.println("Comment out esp_task_wdt_reset() in loop() to test");
    
    // El WDT se activará automáticamente si no se resetea
    while(true) {
        delay(10000);  // 10 segundos
        Serial.printf("System still active at %lu ms\n", millis());
        // esp_task_wdt_reset();  // Comentar para test
    }
}
```

#### **Prevención de Reinicios por WDT**
- **Loop eficiente**: Mantener iteraciones del loop < 100ms
- **Timeouts apropiados**: Todas las operaciones I2C/SPI con timeout
- **Exception handling**: Capturar excepciones en código crítico
- **Watchdog feeding**: `esp_task_wdt_reset()` al final del loop
- **Debug logs**: Monitorear timing del sistema

#### **¿Cuándo es Normal un Reinicio por WDT?**
- **Durante desarrollo**: Frecuente mientras se debuggean cuelgues
- **En producción**: Muy raro (< 1 vez por semana)
- **Después de cambios**: Temporalmente más frecuente durante testing

#### **¿Cuándo Investigar?**
- **Reinicio diario**: Problema serio - revisar código de eventos
- **Reinicio continuo**: Bug crítico - revisar stack usage
- **Después de actualizaciones**: Verificar cambios en timing

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

    // Test 2: Sensores activos
    Serial.println("2. Testing sensors...");
    float t, h, p, d, b;
    bool sensorOk = getSensorDataForDisplay(t, h, p, d, b);
    Serial.printf("   Sensors: %s\n", sensorOk ? "OK" : "FAIL");
    
    // Detalles por sensor
    #if ENABLE_SENSOR_DHT22
    Serial.printf("   DHT22: Temp %.1f°C, Hum %.1f%%\n", t, h);
    #endif
    #if ENABLE_SENSOR_DS18B20
    Serial.printf("   DS18B20: Temp %.1f°C\n", t);
    #endif
    #if ENABLE_SENSOR_BMP280
    Serial.printf("   BMP280: Press %.1f hPa\n", p);
    #endif
    #if ENABLE_SENSOR_HCSR04
    Serial.printf("   HC-SR04: Dist %.1f cm\n", d);
    #endif

    // Test 3: Display
    Serial.println("3. Testing display...");
    sendInfoMessage("Diagnostic Test", 2000);

    // Test 4: Batería y solar
    Serial.println("4. Testing power system...");
    Serial.printf("   Battery: %.2fV\n", b);
    bool solarActive = PMU->isVbusIn() && PMU->isBatteryCharging();
    Serial.printf("   Solar: %s\n", solarActive ? "CHARGING" : "INACTIVE");

    // Test 5: Payload
    Serial.println("5. Testing payload...");
    uint8_t testPayload[16];
    uint8_t payloadSize = getSensorPayload(testPayload, sizeof(testPayload));
    Serial.printf("   Payload size: %d bytes\n", payloadSize);

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
Activo (TX + sensores):  120mA - 2s
Procesamiento:            25mA - 8s
Display ON:              25mA - 5s
Light Sleep:             10mA - variable
Deep Sleep:               0.02mA - 60s
Carga Solar:            -100/+500mA - variable (depende panel)
Promedio ciclo:           0.5mAh (sin solar)
Con solar (8h/día):     -2.0mAh neto (carga > consumo)
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
- Hardware: LilyGo T3 v1.6.1
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