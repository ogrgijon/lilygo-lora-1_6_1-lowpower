# 🚀 Guía de Instalación - Sistema IoT LoRaWAN Modular

## 📋 Lista de Verificación Pre-Instalación

### ✅ **Requisitos del Sistema**
- [ ] **Hardware**: LilyGo T3-S3 con ESP32-S3
- [ ] **OS**: Windows 10/11, macOS 10.15+, Ubuntu 18.04+
- [ ] **RAM**: 4GB mínimo, 8GB recomendado
- [ ] **Espacio**: 2GB libres en disco
- [ ] **USB**: Puerto USB 2.0/3.0 disponible

### ✅ **Cuenta TTN (The Things Network)**
- [ ] **Registro**: Cuenta gratuita en [TTN Console](https://console.thethingsnetwork.org)
- [ ] **Aplicación**: Creada y configurada
- [ ] **Dispositivo**: Registrado como OTAA End Device
- [ ] **Credenciales**: APPEUI, DEVEUI, APPKEY copiadas

## 🛠️ Instalación del Entorno de Desarrollo

### 📥 **Paso 1: Instalar VS Code**

#### **Windows/macOS**
1. Descargar desde [code.visualstudio.com](https://code.visualstudio.com)
2. Ejecutar instalador
3. Abrir VS Code y verificar instalación

#### **Linux (Ubuntu/Debian)**
```bash
# Instalar dependencias
sudo apt update
sudo apt install software-properties-common apt-transport-https wget

# Agregar repositorio Microsoft
wget -q https://packages.microsoft.com/keys/microsoft.asc -O- | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main"

# Instalar VS Code
sudo apt update
sudo apt install code

# Verificar instalación
code --version
```

### 🔧 **Paso 2: Instalar PlatformIO Extension**

1. **Abrir VS Code**
2. **Ir a Extensions** (Ctrl+Shift+X / Cmd+Shift+X)
3. **Buscar**: "PlatformIO IDE"
4. **Instalar** la extensión oficial
5. **Recargar** VS Code cuando se solicite

#### **Verificación PlatformIO**
```bash
# Abrir terminal integrado en VS Code
# Ejecutar para verificar instalación
pio --version
# Debería mostrar: PlatformIO Core, version X.X.X
```

### 📦 **Paso 3: Instalar Drivers USB**

#### **Windows - CP210x Driver**
1. Descargar desde [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
2. Ejecutar instalador como administrador
3. Reiniciar sistema

#### **macOS**
```bash
# Drivers incluidos en macOS moderno
# Verificar en Sistema → Información del Sistema → USB
```

#### **Linux**
```bash
# Instalar drivers CP210x
sudo apt install linux-modules-extra-$(uname -r)
sudo modprobe cp210x

# Verificar carga del módulo
lsmod | grep cp210x
```

## 📥 Clonación y Configuración del Proyecto

### 📋 **Paso 4: Clonar Repositorio**

```bash
# Clonar el proyecto
git clone https://github.com/tu-usuario/lilygo-lora-sensor.git
cd lilygo-lora-sensor/code/low-power-project

# Abrir en VS Code
code .
```

### ⚙️ **Paso 5: Verificar Estructura del Proyecto**

Después de abrir en VS Code, verificar que PlatformIO detecte el proyecto:

```
✅ platformio.ini detectado
✅ Librerías instalándose automáticamente
✅ Configuración ESP32-S3 cargada
```

### 🔧 **Paso 6: Primera Compilación**

```bash
# En terminal integrado VS Code
pio run

# Verificar salida exitosa:
# ========================= [SUCCESS] =========================
# Environment: esp32-s3-devkitc-1
# Platform: Espressif 32 (6.4.0)
# Framework: Arduino
```

## 📡 Configuración LoRaWAN (TTN)

### 🌐 **Paso 7: Configurar The Things Network**

#### **7.1 Crear Aplicación**
1. Ir a [TTN Console](https://console.thethingsnetwork.org)
2. **Create Application**
   - **Application ID**: `lilygo-sensor-demo`
   - **Description**: Sensor ambiental LoRaWAN modular

#### **7.2 Registrar End Device**
1. En la aplicación → **Add end device**
2. **Join method**: Over The Air Activation (OTAA)
3. **Frequency plan**: Europe 863-870 MHz (SF9 for RX2)
4. **LoRaWAN version**: 1.0.3
5. **Regional Parameters**: RP001 Regional Parameters 1.0.3 revision A

#### **7.3 Copiar Credenciales**
Después del registro, copiar:
- **Application EUI** (APPEUI)
- **Device EUI** (DEVEUI)
- **App Key** (APPKEY)

### 🔑 **Paso 8: Configurar Credenciales LoRaWAN**

**IMPORTANTE**: Las claves LoRaWAN ahora están en un archivo separado por seguridad.

```bash
# Copiar la plantilla de configuración
cp include/lorawan_config_template.h include/lorawan_config.h

# Editar el archivo con tus claves TTN
# include/lorawan_config.h
static const u1_t PROGMEM APPEUI[8] = {TU_APPEUI_AQUI};
static const u1_t PROGMEM DEVEUI[8] = {TU_DEVEUI_AQUI};
static const u1_t PROGMEM APPKEY[16] = {TU_APPKEY_AQUI};
```

#### **Conversión de Credenciales TTN**
TTN muestra claves en formato hexadecimal. Convertir a array de bytes:

**Ejemplo APPEUI:**
- TTN: `0000000000000000`
- Código: `{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}`

**Herramienta de conversión online**: [hex-to-byte-array](https://www.scadacore.com/tools/programming-calculators/online-hex-converter/)

**Nota de Seguridad**: El archivo `lorawan_config.h` está incluido en `.gitignore` y NO debe commitearse al repositorio.

### 📊 **Paso 9: Configurar Decoder TTN**

En TTN Console → Applications → Tu App → Payload formatters:

```javascript
// Uplink Decoder - 8 bytes payload
function decodeUplink(input) {
  var bytes = input.bytes;
  var data = {};

  // Temperatura (int16_t big-endian, escala 100)
  var temp_raw = (bytes[0] << 8) | bytes[1];
  if (temp_raw & 0x8000) temp_raw -= 0x10000;  // Sign extension
  data.temperature = temp_raw / 100.0;

  // Humedad (uint16_t big-endian, escala 100)
  data.humidity = ((bytes[2] << 8) | bytes[3]) / 100.0;

  // Presión (uint16_t big-endian, escala 100)
  data.pressure = ((bytes[4] << 8) | bytes[5]) / 100.0;

  // Batería (uint16_t big-endian, escala 100)
  data.battery_voltage = ((bytes[6] << 8) | bytes[7]) / 100.0;

  return {
    data: data,
    warnings: [],
    errors: []
  };
}
```

## 🔌 Hardware y Conexión

### 🛠️ **Paso 10: Preparar Hardware**

#### **Lista de Componentes**
- [ ] LilyGo T3-S3 board
- [ ] Antena LoRa 868MHz (SMA connector)
- [ ] Batería LiPo 18650 (3.7V nominal)
- [ ] Sensor BME280 (I2C)
- [ ] Display OLED SSD1306 0.96" (I2C)
- [ ] Cables jumper (opcional para testing)

#### **Conexiones I2C (Verificar)**
```
ESP32-S3 ←→ BME280
GPIO 17  ←→ SDA
GPIO 18  ←→ SCL
3.3V     ←→ VCC
GND      ←→ GND

ESP32-S3 ←→ OLED SSD1306
GPIO 17  ←→ SDA
GPIO 18  ←→ SCL
3.3V     ←→ VCC
GND      ←→ GND
```

### ⚡ **Paso 11: Primera Carga de Firmware**

```bash
# Compilar proyecto
pio run

# Cargar a dispositivo (asegurar que esté conectado)
pio run --target upload

# Si hay problemas de puerto:
pio run --target upload --upload-port COM5  # Windows
pio run --target upload --upload-port /dev/ttyUSB0  # Linux
pio run --target upload --upload-port /dev/cu.usbserial-0001  # macOS
```

#### **Verificación de Carga Exitosa**
```
✅ Firmware subido correctamente
✅ Reset automático del dispositivo
✅ LED azul parpadea durante inicialización
```

## 🧪 Verificación y Testing

### 📊 **Paso 12: Monitor Serial**

```bash
# Abrir monitor serial
pio device monitor

# O usar botón en VS Code PlatformIO
# Deberías ver logs como:
```

```
=== SISTEMA INICIALIZADO ===
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

### 📡 **Paso 13: Verificar TTN**

1. Ir a TTN Console → Tu Aplicación → Live Data
2. Esperar primer uplink (hasta 60 segundos)
3. Verificar datos decodificados:
   ```json
   {
     "temperature": 25.67,
     "humidity": 65.43,
     "pressure": 1013.25,
     "battery_voltage": 3.85
   }
   ```

### 🖥️ **Paso 14: Verificar Display OLED**

- **Inicio**: "Sistema Iniciado" (3s)
- **Join**: "Uniéndose OTAA...." (persistente hasta join)
- **Éxito**: "Unido a TTN!" (2s)
- **Datos**: Temperatura, humedad, presión, batería (5s)
- **Apagado**: Display se apaga automáticamente

## 🚨 Solución de Problemas de Instalación

### ❌ **Problema: PlatformIO no detecta placa**

#### **Síntomas**
```
No se encuentra puerto serial
Error: Please specify `upload_port` for environment
```

#### **Soluciones**
1. **Verificar conexión USB**
   ```bash
   # Windows
   devmgmt.msc → Puertos (COM & LPT) → Silicon Labs CP210x

   # Linux
   lsusb | grep CP210x
   ls /dev/ttyUSB*

   # macOS
   ls /dev/cu.usbserial*
   ```

2. **Instalar drivers**
   - Descargar de [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
   - Instalar y reiniciar

3. **Forzar puerto**
   ```bash
   pio run --target upload --upload-port COM3
   ```

### ❌ **Problema: Error de compilación**

#### **Síntomas**
```
Library Manager: Installing...
Error: Library 'IBM LMIC framework' not found
```

#### **Soluciones**
```bash
# Limpiar cache de librerías
pio lib --global uninstall IBM\ LMIC\ framework
pio lib --global install IBM\ LMIC\ framework

# Actualizar platformio
pio upgrade

# Limpiar proyecto
pio run --target clean
```

### ❌ **Problema: Join LoRaWAN fallido**

#### **Síntomas**
```
EV_JOINING
Join fallido - reintentando...
```

#### **Diagnóstico**
1. **Verificar credenciales**
   - Copiar exactamente de TTN
   - Verificar formato byte array

2. **Verificar cobertura**
   - Ir a [TTN Mapper](https://ttnmapper.org)
   - Buscar gateways cercanos

3. **Test con canal único** (temporal)
   ```cpp
   // En pgm_board.cpp
   #define SINGLE_CHANNEL_GATEWAY 1
   ```

## 🎯 Próximos Pasos

### 📚 **Aprender Más**
- [Documentación Arquitectura](arquitectura.md)
- [Guía de Uso](uso.md)
- [Solución de Problemas](troubleshooting.md)

### 🔧 **Personalización**
- Modificar intervalo de transmisión
- Agregar nuevos sensores
- Cambiar formato de display
- Implementar OTA updates

### 📊 **Monitoreo**
- Configurar dashboards en TTN
- Integrar con servicios externos
- Implementar alertas automáticas

---

**🚀 ¡Instalación completa! Tu sensor IoT LoRaWAN modular está listo para desplegar**