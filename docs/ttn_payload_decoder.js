/**
 * Decodificador de Payload TTN para Sensor LoRaWAN DHT22
 *
 * Copia y pega esta función en el decodificador de payload de tu aplicación TTN.
 *
 * Estructura del payload (6 bytes):
 * - Bytes 0-1: Temperatura (int16_t * 100, big-endian)
 * - Bytes 2-3: Humedad (uint16_t * 100, big-endian)
 * - Bytes 4-5: Voltaje de batería (uint16_t * 100, big-endian)
 */

function decodeUplink(input) {
  var decoded = {};

  // Verificar si tenemos el tamaño de payload esperado
  if (input.bytes.length !== 6) {
    return {
      errors: ['Tamaño de payload inválido. Esperado 6 bytes, recibido ' + input.bytes.length],
      warnings: []
    };
  }

  var bytes = input.bytes;

  // Decodificar temperatura (bytes 0-1, big-endian, *100)
  var tempRaw = (bytes[0] << 8) | bytes[1];
  // Manejar temperaturas negativas (int16_t)
  if (tempRaw > 32767) {
    tempRaw -= 65536;
  }
  decoded.temperature = tempRaw / 100.0;

  // Decodificar humedad (bytes 2-3, big-endian, *100)
  var humRaw = (bytes[2] << 8) | bytes[3];
  decoded.humidity = humRaw / 100.0;

  // Decodificar batería (bytes 4-5, big-endian, *100)
  var battRaw = (bytes[4] << 8) | bytes[5];
  decoded.battery = battRaw / 100.0;

  // Verificar valores de error
  decoded.valid = (decoded.temperature !== -999.0 && decoded.humidity !== -1.0);

  // Agregar metadatos
  decoded.sensor = "DHT22";
  decoded.payload_size = input.bytes.length;

  return {
    data: decoded
  };
}

/**
 * Ejemplos de uso y casos de prueba:
 *
 * Payload de prueba 1 (normal): 092919AA0181
 * Esperado: Temperatura: 23.45°C, Humedad: 65.78%, Batería: 3.85V
 *
 * Payload de prueba 2 (error): D8F1FFFF0172
 * Esperado: Temperatura: -999°C, Humedad: -1%, Batería: 3.70V, válido: false
 *
 * Payload de prueba 3 (temperatura negativa): FDC911AB01A4
 * Esperado: Temperatura: -5.67°C, Humedad: 45.23%, Batería: 4.20V
 */