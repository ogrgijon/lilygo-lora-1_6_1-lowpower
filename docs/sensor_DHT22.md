# Sensor Configuration: DHT22

Esta rama implementa soporte para el sensor DHT22 en lugar del BME280 por defecto.

## Cambios en esta rama

### Sensor DHT22
- **Temperatura**: ✅ Disponible (-40°C a 80°C)
- **Humedad**: ✅ Disponible (0% a 100%)
- **Presión**: ❌ No disponible (DHT22 no mide presión)

### Tamaño del Payload
- **BME280**: 8 bytes (Temp + Hum + Pres + Bat)
- **DHT22**: 6 bytes (Temp + Hum + Bat)

### Configuración
Para cambiar entre sensores, modifica `include/sensor_config.h`:

```cpp
// Para usar BME280 (por defecto en main)
#define USE_SENSOR_BME280

// Para usar DHT22 (esta rama)
#define USE_SENSOR_DHT22
```

## Hardware Connections

### DHT22 Pinout
- **VCC**: 3.3V
- **GND**: GND
- **DATA**: GPIO 13 (configurable en `sensor_config.h`)

### Cableado recomendado
```
ESP32 GPIO 13 ─── DHT22 DATA
ESP32 3.3V ───── DHT22 VCC
ESP32 GND ────── DHT22 GND
```

## Dependencias
- `adafruit/DHT sensor library @ ^1.4.4`

## Testing
Para probar esta configuración:

1. Conecta el DHT22 según el pinout arriba
2. Compila y sube el código
3. Verifica en el monitor serial: "Sensor DHT22 inicializado"
4. Los datos se enviarán en formato LoRaWAN con 6 bytes de payload

## Comparación de Sensores

| Característica | BME280 | DHT22 |
|---|---|---|
| Temperatura | -40°C to 85°C | -40°C to 80°C |
| Humedad | 0-100% | 0-100% |
| Presión | 300-1100 hPa | N/A |
| Precisión Temp | ±1°C | ±0.5°C |
| Precisión Hum | ±3% | ±2-5% |
| Payload Size | 8 bytes | 6 bytes |
| Costo | $$$ | $ |

## Volver a BME280
Para volver al sensor BME280:
```bash
git checkout main
```

## Crear nueva rama para otro sensor
```bash
git checkout -b feature/sensor-[nombre]
# Modificar sensor_config.h
# Crear sensor_[nombre].cpp
# Actualizar platformio.ini si es necesario
```