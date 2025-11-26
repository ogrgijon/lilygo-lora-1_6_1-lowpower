# Configuración LoRaWAN

## Configuración Inicial

Para configurar las claves LoRaWAN de tu dispositivo:

### 1. Copiar la plantilla
```bash
cp config/lorawan_config_template.h config/lorawan_config.h
```

### 2. Obtener claves de TTN
Ve a tu aplicación en [The Things Network](https://console.thethingsnetwork.org) y:
- Copia el **Application EUI** (AppEUI) en formato LSB
- Copia el **Device EUI** (DevEUI) en formato LSB
- Copia el **Application Key** (AppKey) en formato MSB

### 3. Actualizar lorawan_config.h
Edita `config/lorawan_config.h` y reemplaza los valores de ejemplo con tus claves reales:

```cpp
// Application EUI (AppEUI) - LSB format
static const u1_t PROGMEM APPEUI[8] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

// Device EUI (DevEUI) - LSB format
static const u1_t PROGMEM DEVEUI[8] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

// Application Key (AppKey) - MSB format
static const u1_t PROGMEM APPKEY[16] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
```

## Seguridad

- El archivo `lorawan_config.h` está incluido en `.gitignore`
- Nunca commiteas tus claves LoRaWAN al repositorio
- Comparte solo `lorawan_config_template.h` como ejemplo

## Verificación

Después de configurar, compila y sube el código:
```bash
pio run --target upload
```

El dispositivo debería unirse a la red TTN y comenzar a enviar datos.