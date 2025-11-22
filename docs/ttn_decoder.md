# Decodificador de Payload TTN para Sensor LoRaWAN DHT22

## Configuración Rápida

1. **Copia la función decodificadora** desde `ttn_payload_decoder.js`
2. **Ve a tu aplicación TTN** → **Formateadores de payload** → **Uplink**
3. **Selecciona "JavaScript"** como tipo de formateador
4. **Pega la función decodificadora** en el editor de código
5. **Guarda** los cambios

## Estructura del Payload

El sensor DHT22 envía un **payload de 6 bytes**:

| Bytes | Tipo de Dato | Descripción | Multiplicador | Unidad | Rango |
|-------|--------------|-------------|---------------|--------|-------|
| 0-1   | int16_t      | Temperatura | ×100          | °C     | -50 a +80 |
| 2-3   | uint16_t     | Humedad     | ×100          | %      | 0 a 100 |
| 4-5   | uint16_t     | Batería     | ×100          | V      | 2.0 a 5.0 |

**Orden de bytes:** Big-endian (orden de red)

## Salida Decodificada

El decodificador retorna:

```json
{
  "temperature": 23.45,
  "humidity": 65.78,
  "battery": 3.85,
  "valid": true,
  "sensor": "DHT22",
  "payload_size": 6
}
```

## Manejo de Errores

- **Error de temperatura:** `-999.0°C`
- **Error de humedad:** `-1.0%`
- **Batería:** Siempre válida (medición por hardware)
- **Bandera válida:** `false` cuando se detectan errores del sensor

## Payloads de Prueba

### Operación normal:
- **Hex:** `092919AA0181`
- **Resultado:** `{"temperature": 23.45, "humidity": 65.78, "battery": 3.85, "valid": true}`

### Error del sensor:
- **Hex:** `D8F1FFFF0172`
- **Resultado:** `{"temperature": -999.0, "humidity": -1.0, "battery": 3.70, "valid": false}`

### Temperatura negativa:
- **Hex:** `FDC911AB01A4`
- **Resultado:** `{"temperature": -5.67, "humidity": 45.23, "battery": 4.20, "valid": true}`

## Integración con TTN

Después de configurar el decodificador:

1. **Los datos del dispositivo** mostrarán automáticamente valores decodificados
2. **La integración de almacenamiento** recibirá datos JSON estructurados
3. **Los webhooks** contendrán el payload decodificado
4. **Los mensajes MQTT** incluirán campos decodificados

## Solución de Problemas

- **"Tamaño de payload inválido"**: Verifica que tu dispositivo envíe exactamente 6 bytes
- **Valores incorrectos**: Verifica el orden de bytes (big-endian) y multiplicador (×100)
- **Temperaturas negativas**: El decodificador maneja correctamente complemento a dos

## Ejemplo de Payload de Webhook

```json
{
  "end_device_ids": {
    "device_id": "tu-device-id"
  },
  "uplink_message": {
    "decoded_payload": {
      "temperature": 23.45,
      "humidity": 65.78,
      "battery": 3.85,
      "valid": true,
      "sensor": "DHT22",
      "payload_size": 6
    },
    "payload_hex": "092919AA0181"
  }
}
```