# Sensor Configuration: DHT22

Esta rama implementa soporte para el sensor DHT22 en lugar del BME280 por defecto.

## Cambios en esta rama

### Sensor DHT22
- **Temperatura**: ✅ Disponible (-40°C a 80°C)
- **Humedad**: ✅ Disponible (0% a 100%)
- **Presión**: ❌ No disponible (DHT22 no mide presión)

### Tamaño del Payload
- **DHT22**: 6 bytes (Temp + Hum + Bat)

## Hardware Connections

### DHT22 Pinout (Ultra-Low Power Configuration)
- **VCC**: GPIO 12 (controlado por software - NO conectar a 3.3V)
- **GND**: GND
- **DATA**: GPIO 13 (configurable en `sensor_config.h`)

### Cableado recomendado para ultra bajo consumo
```
ESP32 GPIO 12 ─── DHT22 VCC  (control de alimentación)
ESP32 GPIO 13 ─── DHT22 DATA
ESP32 GND ────── DHT22 GND
```

**⚠️ IMPORTANTE**: No conectes VCC del DHT22 directamente a 3.3V. Debe conectarse al GPIO 12 para control de alimentación.

### Configuración de pines
```cpp
#define DHT_PIN 13              // Pin de datos
#define DHT_POWER_PIN 12        // Pin de control de alimentación
#define DHT_POWER_ON_DELAY_MS 2000  // Tiempo de estabilización (2 segundos)
```

## Ultra-Low Power Management

Esta implementación incluye gestión avanzada de energía para minimizar el consumo:

### Funcionamiento
1. **Apagado por defecto**: El sensor permanece sin alimentación la mayor parte del tiempo
2. **Encendido bajo demanda**: Se activa solo cuando se necesita leer datos
3. **Tiempo de estabilización**: Espera 2 segundos después de encender antes de leer
4. **Apagado inmediato**: Se desactiva inmediatamente después de la lectura

### Beneficios de consumo
- **Sin sensor**: ~0.1-0.5μA (solo fuga del GPIO)
- **Con sensor activo**: ~1-2mA durante lecturas (muy breve)
- **Ahorro**: >99% de reducción en consumo del sensor vs. alimentación continua

### Secuencia de lectura
```
Tiempo: 0ms     2000ms   2100ms   2101ms+
Estado: OFF ──► ON ───► STABLE ─► READ ─► OFF
```

### Configuración de energía
```cpp
#define DHT_POWER_PIN 12           // GPIO que controla VCC del sensor
#define DHT_POWER_ON_DELAY_MS 2000 // Tiempo de calentamiento necesario
```

## Testing
Para probar esta configuración:

1. Conecta el DHT22 según el pinout de ultra bajo consumo (VCC a GPIO 12)
2. Compila y sube el código
3. Verifica en el monitor serial:
   - "Sensor DHT22 inicializado (alimentación controlada)"
   - "DHT22: Alimentación ON"
   - "DHT22: Esperando 2000 ms para estabilización..."
   - "DHT22: Lectura exitosa - Temp: XX.X°C, Hum: XX.X%"
   - "DHT22: Alimentación OFF"
4. Los datos se enviarán en formato LoRaWAN con 6 bytes de payload

## Troubleshooting

### Problemas comunes con gestión de energía

**Lecturas NaN o -999**
- Verifica que VCC del DHT22 esté conectado a GPIO 12, NO a 3.3V
- Aumenta `DHT_POWER_ON_DELAY_MS` si el sensor necesita más tiempo de estabilización
- Verifica conexiones de DATA y GND

**Sensor no responde**
- Confirma que el GPIO 12 puede proporcionar suficiente corriente (DHT22 necesita ~1mA)
- Verifica que no haya otros dispositivos consumiendo corriente del mismo GPIO

**Consumo mayor de lo esperado**
- Asegúrate de que el sensor se apaga después de cada lectura
- Verifica que el GPIO esté en modo INPUT cuando está "apagado"

**Lecturas inestables**
- El DHT22 puede necesitar más tiempo de estabilización en ambientes fríos
- Considera aumentar el delay de estabilización a 3000ms


## Crear nueva rama para otro sensor
```bash
git checkout -b feature/sensor-[nombre]
# Modificar sensor_config.h
# Crear sensor_[nombre].cpp
# Actualizar platformio.ini si es necesario
```