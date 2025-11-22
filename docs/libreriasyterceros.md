# Documentación de Librerías de Terceros

## Introducción

Este proyecto utiliza varias librerías de código abierto y de terceros para implementar la funcionalidad de un sensor LoRaWAN IoT. A continuación se detalla cada librería utilizada, incluyendo información sobre sus propietarios, licencias y derechos asociados.

## Librerías Principales

### 1. DHT Sensor Library
- **Propietario**: Adafruit Industries LLC
- **Licencia**: MIT License
- **Versión utilizada**: 1.4.4
- **Propósito**: Control de sensores DHT (DHT11, DHT22, DHT21) para medición de temperatura y humedad
- **Repositorio**: https://github.com/adafruit/DHT-sensor-library
- **Derechos y restricciones**:
  - ✅ Uso comercial permitido
  - ✅ Modificación del código permitida
  - ✅ Distribución permitida (con inclusión del aviso de copyright)
  - ✅ Uso privado y comercial sin restricciones
- **Dependencias**: Adafruit Unified Sensor Driver (opcional)

### 2. U8g2 Library
- **Propietario**: Oliver Kraus
- **Licencia**: BSD 2-Clause License (New BSD License)
- **Versión utilizada**: 2.36.15
- **Propósito**: Control de displays OLED monocromáticos (SSD1306)
- **Repositorio**: https://github.com/olikraus/u8g2
- **Derechos y restricciones**:
  - ✅ Uso comercial permitido
  - ✅ Modificación del código permitida
  - ✅ Distribución permitida (con inclusión del aviso de copyright)
  - ⚠️ Fuentes tienen licencias variables (ver documentación específica)
- **Características**: Soporta más de 40 controladores de display diferentes

### 3. XPowersLib
- **Propietario**: Lewis He (para LilyGo)
- **Licencia**: MIT License
- **Versión utilizada**: 0.1.9
- **Propósito**: Control del PMU AXP192 para gestión de energía
- **Repositorio**: https://github.com/lewisxhe/XPowersLib
- **Derechos y restricciones**:
  - ✅ Uso comercial permitido
  - ✅ Modificación del código permitida
  - ✅ Distribución permitida (con inclusión del aviso de copyright)
  - ✅ Uso privado y comercial sin restricciones
- **Nota**: Desarrollada específicamente para placas LilyGo con chips X-Powers

### 4. LMIC-Arduino (MCCI LoRaWAN LMIC Library)
- **Propietario**: MCCI Corporation
- **Licencia**: MIT License (con componentes Eclipse Public License v1.0)
- **Versión utilizada**: 1.5.0+arduino-2
- **Propósito**: Implementación de stack LoRaWAN completo
- **Repositorio**: https://github.com/mcci-catena/arduino-lmic
- **Derechos y restricciones**:
  - ✅ Uso comercial permitido
  - ✅ Modificación del código permitida
  - ✅ Distribución permitida (con inclusión del aviso de copyright)
  - ⚠️ Algunos componentes heredados usan Eclipse Public License
- **Características**: Soporta múltiples regiones LoRaWAN (EU868, US915, AS923, etc.)

## Librerías del Framework Arduino/ESP32

### 5. ESP32 BLE Arduino
- **Propietario**: Espressif Systems
- **Licencia**: Apache License 2.0 (para componentes ESP32)
- **Versión utilizada**: 2.0.0
- **Propósito**: Soporte Bluetooth Low Energy para ESP32
- **Repositorio**: Parte del core ESP32 Arduino
- **Derechos y restricciones**:
  - ✅ Uso comercial permitido
  - ✅ Modificación del código permitida
  - ✅ Distribución permitida (con inclusión del aviso de copyright)
  - ✅ Patentes incluidas en la licencia

### 6. Wire (I2C)
- **Propietario**: Arduino
- **Licencia**: LGPL 2.1
- **Propósito**: Comunicación I2C para sensores y displays
- **Derechos y restricciones**:
  - ✅ Uso comercial permitido
  - ✅ Modificación del código permitida
  - ⚠️ Si se modifican las librerías, los cambios deben distribuirse bajo LGPL
  - ⚠️ Aplicaciones que usan la librería pueden tener licencias más restrictivas

### 7. SPI
- **Propietario**: Arduino
- **Licencia**: LGPL 2.1
- **Propósito**: Comunicación SPI para el módulo LoRa
- **Derechos y restricciones**: Mismas que Wire (LGPL 2.1)

### 8. FS (File System)
- **Propietario**: Arduino
- **Licencia**: LGPL 2.1
- **Propósito**: Sistema de archivos para ESP32
- **Derechos y restricciones**: Mismas que Wire (LGPL 2.1)

### 9. SD
- **Propietario**: Arduino
- **Licencia**: LGPL 2.1
- **Propósito**: Soporte para tarjetas SD
- **Derechos y restricciones**: Mismas que Wire (LGPL 2.1)

### 10. WiFi
- **Propietario**: Arduino/ESP32
- **Licencia**: LGPL 2.1 / Apache 2.0
- **Propósito**: Conectividad WiFi (aunque no utilizada en este proyecto)
- **Derechos y restricciones**: Mismas que Wire (LGPL 2.1)

## Resumen de Licencias

| Licencia | Librerías | Permisos | Restricciones |
|----------|-----------|----------|---------------|
| **MIT** | XPowersLib, LMIC-Arduino, DHT Sensor Library | Uso comercial, modificación, distribución | Incluir copyright |
| **BSD 3-Clause** | Adafruit BME280 (legacy) | Uso comercial, modificación, distribución | No usar nombre del propietario para promoción |
| **BSD 2-Clause** | U8g2 | Uso comercial, modificación, distribución | Incluir copyright |
| **LGPL 2.1** | Wire, SPI, FS, SD, WiFi | Uso comercial, modificación, distribución | Cambios deben ser LGPL |
| **Apache 2.0** | ESP32 BLE Arduino | Uso comercial, modificación, distribución | Incluir copyright, patentes |

## Consideraciones Legales

### Derechos de Autor
- Todos los propietarios mantienen sus derechos de autor sobre sus respectivas librerías
- El proyecto debe incluir los avisos de copyright correspondientes
- Las modificaciones realizadas deben respetar las licencias originales

### Distribución
- ✅ Se permite distribuir el proyecto completo con todas las librerías
- ✅ Se permite uso comercial del proyecto
- ⚠️ Deben mantenerse los avisos de copyright en el código distribuido
- ⚠️ Si se modifican librerías LGPL, los cambios deben distribuirse bajo LGPL

### Responsabilidades
- Los propietarios de las librerías no ofrecen garantías
- El usuario asume la responsabilidad del uso correcto
- Se recomienda verificar las licencias actualizadas en los repositorios oficiales

## Actualizaciones y Mantenimiento

- Las versiones de las librerías deben verificarse periódicamente
- Se recomienda usar versiones estables y probadas
- Los cambios en las licencias deben monitorearse
- Mantener documentación actualizada de dependencias

## Contactos y Soporte

- **Adafruit Industries**: https://www.adafruit.com/
- **Oliver Kraus (U8g2)**: https://github.com/olikraus
- **Lewis He (XPowersLib)**: https://github.com/lewisxhe
- **MCCI Corporation**: https://mcci.com/
- **Espressif Systems**: https://www.espressif.com/
- **Arduino**: https://www.arduino.cc/

---

*Documento creado el: Noviembre 2025*
*Última revisión: Noviembre 2025*
