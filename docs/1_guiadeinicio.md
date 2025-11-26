# 🚀 Primeros Pasos - Guía Completa para Principiantes

**De cero a tu primer nodo LoRaWAN en 30 minutos**

---

## 🌐 Paso 0: ¿Qué vas a aprender?

Esta guía te llevará desde **conceptos básicos de IoT** hasta tener tu primer dispositivo LoRaWAN funcionando. No necesitas experiencia previa.

### 🎯 Al final de esta guía sabrás:
- Qué es IoT, LoRa y LoRaWAN
- Cómo configurar The Things Network (TTN)
- Instalar el entorno de desarrollo
- Configurar y programar tu dispositivo
- Ver tus datos en tiempo real

---

## 📚 Paso 1: Conceptos Básicos (10 minutos)

### 🌐 ¿Qué es el IoT (Internet of Things)?

**IoT significa "Internet of Things" o "Internet de las Cosas".**

Imagina que todos los objetos cotidianos de tu casa pudieran conectarse a internet y "hablar" entre sí:
- **Tu nevera** que te avisa cuando se acaba la leche
- **Las luces inteligentes** que se encienden cuando llegas a casa
- **El termostato** que aprende tus preferencias de temperatura

### 🔧 ¿Cómo funciona?
Los dispositivos IoT tienen:
- **Sensores**: Para medir temperatura, humedad, movimiento, etc.
- **Conectividad**: Para enviar datos a internet (WiFi, Bluetooth, LoRa)
- **Procesamiento**: Un pequeño cerebro que toma decisiones
- **Batería**: Para funcionar sin cables

### 📡 ¿Qué es LoRa y LoRaWAN?

**LoRa** es una tecnología de comunicación inalámbrica de largo alcance y bajo consumo.

#### 🌊 Analogía simple:
- **WiFi**: Como caminar rápido pero solo puedes ir 50 metros
- **4G/5G**: Como ir en coche pero consumes mucha gasolina
- **LoRa**: Como ir en bicicleta eléctrica - vas lejos (hasta 20km) pero consumes muy poca energía

#### 📊 Características clave de LoRa:
- **Alcance**: Hasta 20 kilómetros en campo abierto
- **Consumo**: Muy bajo (puedes funcionar años con una batería)
- **Costo**: Comunicación gratuita (no paga roaming)
- **Velocidad**: Más lenta que WiFi, pero suficiente para sensores

### 🔄 LoRaWAN (La red LoRa):
LoRaWAN es como el "sistema postal" de LoRa. Organiza cómo los dispositivos se conectan a internet a través de **gateways** (antenas especiales).

### 🏗️ ¿Qué es The Things Network (TTN)?
**TTN es una red global gratuita de gateways LoRaWAN.**
- **Gateways**: Antenas instaladas por voluntarios en todo el mundo
- **Red gratuita**: Cualquiera puede usar los gateways sin pagar
- **Comunidad**: Miles de personas colaboran para expandir la cobertura

### 🌍 Cobertura en España:
TTN tiene gateways en más de 150 países, incluyendo España. En muchas ciudades hay cobertura suficiente.

**💡 Nota importante**: Antes de desplegar cualquier dispositivo IoT, revisa el [**descargo de responsabilidad**](responsibility.md) para entender limitaciones legales y términos de uso.

---

## 🛠️ Paso 2: Prepara tu Entorno de Desarrollo (10 minutos)

### 📋 Lista de Verificación
- [ ] **Hardware**: LilyGo T3 v1.6.1 con ESP32
- [ ] **OS**: Windows 10/11, macOS 10.15+, Ubuntu 18.04+
- [ ] **RAM**: 4GB mínimo, 8GB recomendado
- [ ] **Espacio**: 2GB libres en disco
- [ ] **USB**: Puerto USB disponible

### 📥 Instala VS Code

#### **Windows/macOS**
1. Descarga desde [code.visualstudio.com](https://code.visualstudio.com)
2. Ejecuta el instalador
3. Abre VS Code y verifica la instalación

#### **Linux (Ubuntu/Debian)**
```bash
# Instala dependencias
sudo apt update
sudo apt install software-properties-common apt-transport-https wget

# Agrega repositorio Microsoft
wget -q https://packages.microsoft.com/keys/microsoft.asc -O- | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main"

# Instala VS Code
sudo apt update
sudo apt install code

# Verifica instalación
code --version
```

### 🔧 Instala PlatformIO Extension

1. **Abre VS Code**
2. **Ve a Extensions** (Ctrl+Shift+X / Cmd+Shift+X)
3. **Busca**: "PlatformIO IDE"
4. **Instala** la extensión oficial
5. **Reinicia** VS Code cuando se solicite

#### **Verificación**
```bash
# Abre terminal integrado en VS Code
# Ejecuta para verificar instalación
pio --version
# Debería mostrar: PlatformIO Core, version X.X.X
```

### 📦 Instala Drivers USB

#### **Windows - CP210x Driver**
1. Descarga desde [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
2. Ejecuta instalador como administrador
3. Reinicia el sistema

#### **macOS**
```bash
# Drivers incluidos en macOS moderno
# Verifica en Sistema → Información del Sistema → USB
```

#### **Linux**
```bash
# Instala drivers CP210x
sudo apt install linux-modules-extra-$(uname -r)
sudo modprobe cp210x

# Verifica carga del módulo
lsmod | grep cp210x
```

---

## 📡 Paso 3: Configura The Things Network (5 minutos)

### 1. **Crea cuenta en TTN**
1. Ve a [console.thethingsnetwork.org](https://console.thethingsnetwork.org)
2. Regístrate con email (gratuito)
3. Verifica tu email

### 2. **Crea una aplicación**
1. En TTN Console, ve a **Applications**
2. Click **Add application**
3. **Application ID**: `mi-primer-sensor` (elige un nombre único)
4. **Description**: `Mi primer sensor LoRaWAN`
5. Click **Create application**

### 3. **Registra tu dispositivo**
1. En tu aplicación, ve a **End devices**
2. Click **Add end device**
3. Selecciona **Enter end device specifics manually**
4. **Frequency plan**: `Europe 863-870 MHz (SF9 for RX2)`
5. **LoRaWAN version**: `MAC V1.0.3`
6. **JoinEUI**: Deja en automático (se generará)
7. **DevEUI**: Deja en automático (se generará)
8. **AppKey**: Deja en automático (se generará)
9. **End device ID**: `mi-dispositivo-01`
10. Click **Register end device**

### 4. **Copia las credenciales**
Después de registrar, ve a **Overview** y copia:
- **Application EUI** (AppEUI)
- **Device EUI** (DevEUI)
- **App Key**

**⚠️ Importante**: Guarda estas claves, las necesitarás después.

---

## ⚙️ Paso 4: Configura el Proyecto (5 minutos)

### 📥 Clona y abre el proyecto
```bash
# Clona el proyecto
git clone <tu-repo-url>
cd low-power-project

# Abre en VS Code
code .
```

### 🔧 Configura los sensores
Edita `config/config.h`:

```cpp
// DESCOMENTA el sensor que quieres usar
#define ENABLE_SENSOR_DHT22      // Temperatura + Humedad
// #define ENABLE_SENSOR_DS18B20    // Temperatura adicional
// #define ENABLE_SENSOR_BMP280     // Presión atmosférica
// #define ENABLE_SENSOR_HCSR04     // Sensor de distancia
```

### 📡 Configura TTN
Edita `config/lorawan_config.h` con las claves que copiaste:

```cpp
// Reemplaza con tus valores de TTN (formato hexadecimal)
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Tu AppEUI
static const u1_t PROGMEM DEVEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Tu DevEUI
static const u1_t PROGMEM APPKEY[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Tu AppKey
```

### 🔌 Conecta tu dispositivo
1. Conecta la placa LilyGo T3 v1.6.1 por USB
2. Verifica que VS Code la detecte (busca "COM" en Windows o "/dev/tty" en Linux/macOS)

---

## 🚀 Paso 5: Compila y Sube (5 minutos)

### ⚡ Compila el proyecto
```bash
# En VS Code, abre terminal integrado (Ctrl+ñ)
pio run
```

### 📤 Sube a la placa
```bash
# Asegúrate de que la placa esté conectada
pio run --target upload
```

### 📺 Verifica funcionamiento
- La pantalla OLED debería mostrar "Sistema Iniciado"
- En TTN Console deberías ver que el dispositivo se conecta
- Cada 60 segundos deberías recibir datos

---

## 📊 Paso 6: Visualiza tus Datos (5 minutos)

### 🖥️ En TTN Console
1. Ve a tu aplicación → End devices → Tu dispositivo
2. Ve a la pestaña **Live data**
3. Deberías ver paquetes llegando cada minuto

### 📈 Crea un dashboard simple
1. En TTN, ve a **Payload formatters**
2. Selecciona **Custom JavaScript formatter**
3. Pega este código:

```javascript
function decodeUplink(input) {
  var bytes = input.bytes;
  var data = {};

  // Leer temperatura (bytes 0-1)
  var tempRaw = (bytes[0] << 8) | bytes[1];
  if (tempRaw > 32767) tempRaw -= 65536;
  data.temperature = tempRaw / 100.0;

  // Leer humedad (bytes 2-3)
  data.humidity = ((bytes[2] << 8) | bytes[3]) / 100.0;

  // Leer batería (bytes 4-5)
  data.battery = ((bytes[4] << 8) | bytes[5]) / 100.0;

  // Leer estado solar (byte 6)
  data.solar_charging = bytes[6] ? true : false;

  return { data: data };
}
```

4. **Save changes**
5. Ahora verás los datos decodificados en **Live data**

---

## 🎉 ¡Felicitaciones!

**¡Has creado tu primer dispositivo IoT LoRaWAN!** 🌟

### 📊 Qué has logrado:
- ✅ Entender conceptos básicos de IoT
- ✅ Configurar The Things Network
- ✅ Instalar entorno de desarrollo
- ✅ Programar un ESP32
- ✅ Enviar datos por LoRaWAN
- ✅ Visualizar datos en tiempo real

### 🚀 Próximos pasos:
1. **Lee** [**Uso Básico**](usage.md) para aprender a configurar diferentes sensores
2. **Experimenta** cambiando intervalos de envío
3. **Añade** más sensores a tu configuración
4. **Estudia** [**Responsabilidad**](responsibility.md) para entender limitaciones legales
5. **Profundiza** leyendo [**Arquitectura**](architecture.md)

### ❓ ¿Problemas?
Si algo no funciona:
1. Revisa [**Troubleshooting**](../docs/9_troubleshooting.md)
2. Verifica conexiones físicas
3. Comprueba logs en Serial Monitor de VS Code

---

**¡Bienvenido al fascinante mundo del IoT!** 🚀📡🌡️

*Recuerda: lee el [**descargo de responsabilidad**](responsibility.md) antes de usar el proyecto.*

¿Tienes preguntas? No dudes en abrir un issue en el repositorio.