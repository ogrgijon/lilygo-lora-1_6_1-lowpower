# Componentes Hardware

El proyecto usa una placa LilyGo T3-S3 con sensores para medir temperatura, humedad y enviar datos por LoRaWAN.

## Placa Principal: LilyGo T3-S3

- **Microcontrolador**: ESP32-S3 (240MHz, WiFi, Bluetooth)
- **Módulo LoRa**: SX1276/SX1278 (868MHz para Europa)
- **Sensor**: AM2302 (temperatura, humedad)
- **PMU**: AXP2101/AXP192 (gestión de batería)
- **Display**: OLED SSD1306 opcional
- **Conectores**: USB-C, antena SMA, batería LiPo

## Especificaciones Clave

| Componente | Detalles |
|------------|----------|
| **ESP32-S3** | Procesador dual-core, 16MB Flash |
| **LoRa** | Alcance hasta 10km, frecuencia 868MHz |
| **AM2302** | Temperatura: -40°C a 80°C, Humedad: 0-100% |
| **Batería** | LiPo 3.7V, autonomía ~6-8 meses |
| **Consumo** | Activo: 100mA, Sueño: 20μA |

## Conexiones

- **Antena LoRa**: Conectar a conector SMA
- **Batería**: Conectar a conector JST-PH
- **USB**: Para programación y alimentación

## Pines Principales

| Función | GPIO |
|---------|------|
| LoRa SPI | 5, 3, 6, 7 |
| I2C Sensores | 17, 18 |
| Display | I2C 0x3C |
| Batería ADC | 35 |

## Alimentación

- **Entrada**: USB 5V o batería 3.7V
- **Reguladores**: PMU controla voltajes para cada componente
- **Protecciones**: Sobrecarga, descarga profunda

El hardware está diseñado para bajo consumo, con modos de sueño profundo para maximizar autonomía.