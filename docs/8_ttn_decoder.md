# 🚀 Decodificador TTN - Sensor LoRaWAN

## ⚡ Configuración Automática (30 segundos)

¡El decodificador se genera **automáticamente** desde la placa!

1. **Conecta la placa** y abre **Serial Monitor**
2. **Espera a que se inicie** - verás el decodificador JavaScript impreso
3. **Copia el código** que aparece en el serial
4. **Ve a TTN Console** → Tu aplicación → **Payload formatters** → **Uplink**
5. **Pega el código** y guarda

¡Listo! El decodificador está **personalizado** para tu configuración exacta.

## 📊 Estructura del Payload

| Campo | Bytes | Tipo | Descripción | Ejemplo |
|-------|-------|------|-------------|---------|
| Temperatura | 0-1 | int16 | °C × 100 | 23.45°C |
| Humedad | 2-3 | uint16 | % × 100 | 65.78% |
| Batería | 4-5 | uint16 | V × 100 | 3.85V |
| Estado Solar | 6 | uint8 | 0=no, 1=sí | true |

**Total: 7 bytes** - Big-endian

## 🔍 Debug con Serial Monitor

**Todo el debug** se hace desde Serial Monitor:

```
=== TTN PAYLOAD DECODER ===
function decodeUplink(input) {
  // Código JavaScript generado automáticamente
}
=== END DECODER ===

DEBUG: PMU battery voltage: 3.85 V
DEBUG: Battery payload - voltage: 3.85 V, int: 385 (0x0181)
```

Si ves `battery: 0` en TTN pero `3.85V` en logs:
- El PMU funciona para display pero falla en payload
- Revisa timing o inicialización del PMU

## 📱 Salida Decodificada

```json
{
  "temperature": 23.45,
  "humidity": 65.78,
  "battery": 3.85,
  "solar_charging": true,
  "valid": true
}
```

## ⚠️ Problema de Batería Diagnosticado

Si `battery = 0` en TTN:
1. Busca `DEBUG: PMU battery voltage:` en Serial
2. Si es `0.0 V` → PMU no inicializado
3. Si es `3.85 V` → Problema en conversión a payload
4. Compara con `DEBUG: Battery payload - voltage:`

## 🧪 Verificación Rápida

- **Serial Monitor** muestra el decodificador → ✅ Configurado
- **DEBUG logs** muestran voltaje real → ✅ PMU funcionando
- **TTN** recibe valores correctos → ✅ Todo OK

¡La placa **genera todo automáticamente**! 🎯