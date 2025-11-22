/**
 * Prueba del Decodificador de Payload TTN
 *
 * Ejecuta esto en Node.js para probar el decodificador localmente:
 * node ttn_decoder_test.js
 */

function decodeUplink(input) {
  var decoded = {};

  if (input.bytes.length !== 7) {
    return {
      errors: ['Tamaño de payload inválido. Esperado 7 bytes, recibido ' + input.bytes.length],
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

  // Estado solar (byte 6)
  decoded.solar_charging = bytes[6] ? true : false;

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

console.log("Prueba del Decodificador TTN DHT22 con Estado Solar");
console.log("==================================================");

// Prueba 1: Valores normales con carga solar
console.log("\nPrueba 1: Operación normal con carga solar");
var test1 = decodeUplink({ bytes: hexToBytes("092919AA018101") });
console.log("Hex: 092919AA018101");
console.log("Resultado:", JSON.stringify(test1.data, null, 2));

// Prueba 2: Valores normales sin carga solar
console.log("\nPrueba 2: Operación normal sin carga solar");
var test2 = decodeUplink({ bytes: hexToBytes("092919AA018100") });
console.log("Hex: 092919AA018100");
console.log("Resultado:", JSON.stringify(test2.data, null, 2));

// Prueba 3: Error del sensor
console.log("\nPrueba 3: Error del sensor");
var test3 = decodeUplink({ bytes: hexToBytes("D8F1FFFF017200") });
console.log("Hex: D8F1FFFF017200");
console.log("Resultado:", JSON.stringify(test3.data, null, 2));

// Prueba 4: Temperatura negativa con carga solar
console.log("\nPrueba 4: Temperatura negativa con carga solar");
var test4 = decodeUplink({ bytes: hexToBytes("FDC911AB01A401") });
console.log("Hex: FDC911AB01A401");
console.log("Resultado:", JSON.stringify(test4.data, null, 2));

// Prueba 5: Tamaño de payload inválido
console.log("\nPrueba 5: Tamaño de payload inválido");
var test5 = decodeUplink({ bytes: hexToBytes("092919AA01") }); // 5 bytes
console.log("Hex: 092919AA01 (5 bytes)");
console.log("Resultado:", test5);