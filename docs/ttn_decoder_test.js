/**
 * Prueba del Decodificador de Payload TTN
 *
 * Ejecuta esto en Node.js para probar el decodificador localmente:
 * node ttn_decoder_test.js
 */

function decodeUplink(input) {
  var decoded = {};

  if (input.bytes.length !== 6) {
    return {
      errors: ['Tamaño de payload inválido. Esperado 6 bytes, recibido ' + input.bytes.length],
      warnings: []
    };
  }

  var bytes = input.bytes;

  // Temperatura (bytes 0-1, big-endian, *100)
  var tempRaw = (bytes[0] << 8) | bytes[1];
  if (tempRaw > 32767) {
    tempRaw -= 65536;
  }
  decoded.temperature = tempRaw / 100.0;

  // Humedad (bytes 2-3, big-endian, *100)
  var humRaw = (bytes[2] << 8) | bytes[3];
  decoded.humidity = humRaw / 100.0;

  // Batería (bytes 4-5, big-endian, *100)
  var battRaw = (bytes[4] << 8) | bytes[5];
  decoded.battery = battRaw / 100.0;

  decoded.valid = (decoded.temperature !== -999.0 && decoded.humidity !== -1.0);
  decoded.sensor = "DHT22";
  decoded.payload_size = input.bytes.length;

  return { data: decoded };
}

// Casos de prueba
function hexToBytes(hex) {
  var bytes = [];
  for (var i = 0; i < hex.length; i += 2) {
    bytes.push(parseInt(hex.substr(i, 2), 16));
  }
  return bytes;
}

console.log("Prueba del Decodificador TTN DHT22");
console.log("==================================");

// Prueba 1: Valores normales
console.log("\nPrueba 1: Operación normal");
var test1 = decodeUplink({ bytes: hexToBytes("092919AA0181") });
console.log("Hex: 092919AA0181");
console.log("Resultado:", JSON.stringify(test1.data, null, 2));

// Prueba 2: Error del sensor
console.log("\nPrueba 2: Error del sensor");
var test2 = decodeUplink({ bytes: hexToBytes("D8F1FFFF0172") });
console.log("Hex: D8F1FFFF0172");
console.log("Resultado:", JSON.stringify(test2.data, null, 2));

// Prueba 3: Temperatura negativa
console.log("\nPrueba 3: Temperatura negativa");
var test3 = decodeUplink({ bytes: hexToBytes("FDC911AB01A4") });
console.log("Hex: FDC911AB01A4");
console.log("Resultado:", JSON.stringify(test3.data, null, 2));

// Prueba 4: Tamaño de payload inválido
console.log("\nPrueba 4: Tamaño de payload inválido");
var test4 = decodeUplink({ bytes: hexToBytes("092919AA01") }); // 5 bytes
console.log("Hex: 092919AA01 (5 bytes)");
console.log("Resultado:", test4);