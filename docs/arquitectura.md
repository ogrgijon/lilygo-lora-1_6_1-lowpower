# 🏗️ Arquitectura del Sistema IoT LoRaWAN

## 📋 Visión General

El sistema implementa una arquitectura modular altamente desacoplada, diseñada para máxima robustez, mantenibilidad y eficiencia energética. Cada módulo tiene responsabilidades claras y bien definidas, permitiendo desarrollo independiente y testing aislado.

## 🧩 Arquitectura Modular

```mermaid
graph TB
    subgraph "🏠 Sistema Principal"
        MAIN[main_otta.ino<br/>Punto de entrada]
        LOOP[Bucle principal<br/>Gestión de eventos]
    end

    subgraph "📡 Módulo LoRaWAN"
        LMIC[pgm_board.cpp<br/>Comunicación LoRaWAN]
        JOIN[Join OTAA<br/>Gestión de sesión]
        TX[Transmisión<br/>Payload handling]
        EVENTS[Manejo de eventos<br/>ACK, errores]
    end

    subgraph "🌡️ Módulo Sensor"
        BME280[sensor.cpp<br/>BME280 + batería]
        PAYLOAD[Creación payload<br/>8 bytes binario]
        VALIDATION[Validación de datos<br/>Manejo de errores]
        RETRY[Reintentos<br/>Recuperación]
    end

    subgraph "🖥️ Módulo Display"
        OLED[screen.cpp<br/>Sistema de mensajes]
        QUEUE[Cola de mensajes<br/>MAX 10 mensajes]
        TIMING[Temporización<br/>Duraciones variables]
        POWER[Gestión de energía<br/>Auto-off/on]
    end

    subgraph "🔧 Módulo Hardware"
        BOARD[LoRaBoards.cpp<br/>Configuración física]
        PMU[Power Management<br/>AXP2101]
        PINS[Configuración pines<br/>SPI, I2C, DIO]
    end

    MAIN --> LMIC
    MAIN --> OLED
    LOOP --> LMIC
    LOOP --> OLED

    LMIC --> BME280
    LMIC --> EVENTS
    LMIC --> TX

    TX --> PAYLOAD
    PAYLOAD --> BME280

    LMIC --> OLED
    BME280 --> OLED

    BOARD --> PMU
    BOARD --> PINS
    BME280 --> BOARD
    OLED --> BOARD
```

## 📦 Componentes del Sistema

### 🎯 **Módulo Principal (`main_otta.ino`)**
**Responsabilidades:**
- Punto de entrada del programa Arduino
- Inicialización de todos los subsistemas
- Bucle principal de procesamiento de eventos

**Interfaz:**
```cpp
void setup()     // Inicialización única
void loop()      // Ciclo continuo
```

### 📡 **Módulo LoRaWAN (`pgm_board.cpp`)**
**Responsabilidades:**
- Comunicación completa con red LoRaWAN
- Gestión del ciclo de vida OTAA
- Manejo de eventos de transmisión
- Coordinación del ciclo de medición/envío/sueño

**Funciones clave:**
- `setupLMIC()`: Configuración inicial OTAA
- `do_send()`: Ciclo de medición y transmisión
- `onEvent()`: Callbacks de eventos LoRaWAN
- `loopLMIC()`: Procesamiento de eventos

**Estados del ciclo:**
```mermaid
stateDiagram-v2
    [*] --> Inicialización
    Inicialización --> Join_OTAA
    Join_OTAA --> Espera_Join
    Espera_Join --> Join_Exitoso
    Espera_Join --> Join_Fallido
    Join_Fallido --> Reintento_Join
    Reintento_Join --> Espera_Join
    Join_Exitoso --> Medición
    Medición --> Transmisión
    Transmisión --> Espera_ACK
    Espera_ACK --> ACK_Recibido
    Espera_ACK --> Timeout
    ACK_Recibido --> Deep_Sleep
    Timeout --> Deep_Sleep
    Deep_Sleep --> Medición
```

### 🌡️ **Módulo Sensor (`sensor.cpp`)**
**Responsabilidades:**
- Gestión completa del sensor BME280
- Medición de parámetros ambientales
- Medición de voltaje de batería
- Validación de datos y manejo de errores
- Creación del payload binario

**Funciones clave:**
- `initSensor()`: Inicialización BME280
- `getSensorPayload()`: Creación payload 8 bytes
- `getSensorDataForDisplay()`: Datos para UI
- `isSensorAvailable()`: Estado del sensor

**Diagrama de flujo:**
```mermaid
flowchart TD
    A[Inicio] --> B{¿Sensor OK?}
    B -->|Sí| C[Leer BME280]
    B -->|No| D[Marcar error]
    C --> E{¿Lectura válida?}
    E -->|Sí| F[Datos OK]
    E -->|No| D
    D --> G[Usar códigos de error]
    F --> H[Leer batería]
    G --> H
    H --> I[Crear payload 8 bytes]
    I --> J[Retornar datos]
```

### 🖥️ **Módulo Display (`screen.cpp`)**
**Responsabilidades:**
- Gestión inteligente de la pantalla OLED
- Sistema de cola de mensajes con prioridades
- Temporización automática de mensajes
- Gestión de energía (apagado automático)

**Tipos de mensaje:**
```cpp
enum ScreenMessageType {
    MSG_INFO,           // Información general
    MSG_WARNING,        // Advertencias
    MSG_ERROR,          // Errores
    MSG_SUCCESS,        // Confirmaciones
    MSG_SENSOR_DATA,    // Datos ambientales
    MSG_STATUS          // Estado del sistema
};
```

**Sistema de cola:**
- **Capacidad**: 10 mensajes máximo
- **Prioridad**: FIFO con eliminación de mensajes antiguos
- **Temporización**: Duraciones configurables por tipo
- **Energía**: Auto-apagado después de cada mensaje

### 🔧 **Módulo Hardware (`LoRaBoards.cpp`)**
**Responsabilidades:**
- Configuración física de la placa LilyGo
- Gestión de pines y periféricos
- Control del PMU AXP2101
- Inicialización de buses (SPI, I2C)

**Configuraciones:**
- **SPI**: Comunicación con módulo LoRa
- **I2C**: Sensores y display OLED
- **GPIO**: Control de alimentación y estado
- **PMU**: Gestión de batería y alimentación

## 🔄 Flujos de Datos

### 📊 **Flujo de Medición y Transmisión**
```mermaid
sequenceDiagram
    participant Main
    participant LoRaWAN
    participant Sensor
    participant Display

    Main->>LoRaWAN: do_send()
    LoRaWAN->>Sensor: getSensorPayload()
    Sensor->>Sensor: Leer BME280 + batería
    Sensor-->>LoRaWAN: Payload 8 bytes
    LoRaWAN->>Sensor: getSensorDataForDisplay()
    Sensor-->>LoRaWAN: Datos formateados
    LoRaWAN->>Display: displaySensorData()
    Display-->>Display: Mostrar en OLED
    LoRaWAN->>LoRaWAN: LMIC_setTxData2()
    LoRaWAN-->>Main: Transmisión iniciada
```

### 🔋 **Flujo de Gestión de Energía**
```mermaid
stateDiagram-v2
    [*] --> Activo
    Activo --> Medición: 2-3 segundos
    Medición --> Transmisión: 1-2 segundos
    Transmisión --> Espera_ACK: Máx 2 segundos
    Espera_ACK --> Deep_Sleep: ACK recibido
    Espera_ACK --> Deep_Sleep: Timeout
    Deep_Sleep --> Activo: Timer 60s

    note right of Deep_Sleep
        Consumo: ~20μA
        Duración: 60 segundos
    end note

    note right of Activo
        Consumo: ~100mA
        Duración: ~5-10 segundos
    end note
```

## 🛡️ Manejo de Errores y Robustez

### 🌡️ **Estrategias de Recuperación del Sensor**
- **Inicialización fallida**: Continúa sin sensor, envía códigos de error
- **Lectura inválida**: Reintenta automáticamente, usa valores de error
- **Sensor desconectado**: Sistema completo sigue funcionando
- **Batería siempre disponible**: Medición independiente del sensor BME280

### 📡 **Robustez LoRaWAN**
- **Join fallido**: Reintentos automáticos con backoff
- **Transmisión fallida**: Sistema continúa, próxima transmisión
- **ACK perdido**: No bloquea el ciclo, continúa con deep sleep
- **Sesión expirada**: Re-join automático

### 🖥️ **Gestión de Display**
- **Cola llena**: Eliminación automática de mensajes antiguos
- **Display apagado**: Auto-encendido cuando llegan mensajes
- **Mensajes expirados**: Transición automática al siguiente
- **Energía crítica**: Mensajes prioritarios para batería baja

## 📊 Métricas de Diseño

### ⚡ **Eficiencia Energética**
| Estado | Consumo | Duración | % del Ciclo |
|--------|---------|----------|-------------|
| **Activo** | 100mA | 5-10s | 1.4-2.8% |
| **Deep Sleep** | 20μA | 60s | 97.2-98.6% |
| **Promedio** | 0.5mA | 60s | 100% |

### 💾 **Uso de Recursos**
| Recurso | Uso Actual | Disponible | % Usado |
|---------|------------|------------|---------|
| **Flash** | 366KB | 1310KB | 28% |
| **RAM** | 25KB | 320KB | 7.8% |
| **CPU** | Pico 240MHz | 240MHz | Variable |

### 📡 **Rendimiento LoRaWAN**
| Parámetro | Valor | Notas |
|-----------|-------|-------|
| **Frecuencia** | 868MHz | Banda europea |
| **SF inicial** | 7 | Adaptativo 7-12 |
| **Potencia** | 14dBm | Máxima permitida |
| **Alcance** | 10km | Línea de vista |
| **Tiempo TX** | ~1-2s | Por paquete |

## 🔧 Patrones de Diseño Implementados

### 🏭 **Factory Pattern**
- Creación centralizada de payloads en `sensor.cpp`
- Abstracción de diferentes tipos de sensor

### 🎯 **Observer Pattern**
- Callbacks LoRaWAN (`onEvent`) para eventos asíncronos
- Sistema de mensajes del display

### 🔄 **State Pattern**
- Estados del ciclo LoRaWAN (join, transmitiendo, durmiendo)
- Estados del display (activo, inactivo, mostrando mensaje)

### 🛡️ **Error Recovery Pattern**
- Graceful degradation cuando fallan componentes
- Códigos de error reconocibles
- Reintentos automáticos con backoff

### ⚡ **Power Management Pattern**
- Estados de bajo consumo bien definidos
- Transiciones automáticas basadas en eventos
- Monitoreo continuo de batería

## 🎯 Principios de Diseño Aplicados

- **🔀 Separación de responsabilidades**: Cada módulo una función clara
- **🔌 Bajo acoplamiento**: Interfaces bien definidas entre módulos
- **🧪 Alta cohesión**: Funcionalidades relacionadas agrupadas
- **🛡️ Fail-safe**: Sistema robusto ante fallos
- **⚡ Eficiencia**: Optimización de recursos y energía
- **📖 Mantenibilidad**: Código claro y bien documentado

---

**🏗️ Arquitectura modular para máxima robustez y mantenibilidad**