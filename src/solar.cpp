#include <XPowersLib.h>
#include <Arduino.h>

// Asume que 'pmu' es la instancia global de XPowersAXP2101 inicializada en LoRaBoards.cpp
extern XPowersAXP2101 pmu;

/**
 * @brief Verifica si la placa solar está cargando la batería
 * @return true si hay entrada VBUS y la batería está cargándose
 */
bool isSolarChargingBattery() {
    // Verifica si hay entrada VBUS (placa solar conectada y generando voltaje)
    if (!pmu.isVbusIn()) {
        return false;  // No hay entrada solar
    }
    
    // Verifica si la batería está cargándose
    return pmu.isCharging();
}

/**
 * @brief Obtiene el estado detallado de carga de la placa solar
 * @return Estado de carga según enum xpowers_chg_status_t
 */
xpowers_chg_status_t getSolarChargeStatus() {
    if (!pmu.isVbusIn()) {
        return XPOWERS_AXP2101_CHG_STOP_STATE;  // No hay carga
    }
    return pmu.getChargerStatus();
}

/**
 * @brief Función de monitoreo para depuración y logging
 * Imprime el estado de carga solar en Serial
 */
void checkSolarStatus() {
    if (isSolarChargingBattery()) {
        Serial.println("Placa solar cargando batería");
    } else {
        Serial.println("Batería no cargándose (posiblemente sin sol o batería llena)");
    }
    
    // Log del estado detallado
    xpowers_chg_status_t status = getSolarChargeStatus();
    switch (status) {
        case XPOWERS_AXP2101_CHG_TRI_STATE:
            Serial.println("Carga tri-estado");
            break;
        case XPOWERS_AXP2101_CHG_PRE_STATE:
            Serial.println("Pre-carga (solar baja potencia)");
            break;
        case XPOWERS_AXP2101_CHG_CC_STATE:
            Serial.println("Carga constante (solar activa)");
            break;
        case XPOWERS_AXP2101_CHG_CV_STATE:
            Serial.println("Carga voltaje constante (casi llena)");
            break;
        case XPOWERS_AXP2101_CHG_DONE_STATE:
            Serial.println("Carga completa");
            break;
        case XPOWERS_AXP2101_CHG_STOP_STATE:
            Serial.println("Sin carga");
            break;
        default:
            Serial.println("Estado desconocido");
            break;
    }
}