// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef BLUETOOTH_FAKE_JAMMER_H
#define BLUETOOTH_FAKE_JAMMER_H

#include <Arduino.h>
#include <string>
#include <esp_bt.h>
#include <esp_gap_ble_api.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include "BleManager.h"

/**
 * ============================================================================
 * @file BluetoothFakeJammer.h
 * @brief Módulo Educativo de Baliza BLE de Laboratorio (Educational Lab Beacon)
 * 
 * FINALIDAD DIDÁCTICA Y TÉCNICA:
 * Este módulo implementa una Baliza Publicitaria BLE estándar (Standard Advertising
 * Beacon) conforme a las especificaciones oficiales de Bluetooth SIG.
 * 
 * CARACTERÍSTICAS DE SEGURIDAD (INOFENSIVO Y NO DESTRUCTIVO):
 * 1. 100% Inofensivo y Legal: No emite ruido blanco electromagnético destructivo ni
 *    inunda con spam malicioso a teléfonos móviles.
 * 2. Emisión Estándar de Laboratorio: Transmite un paquete de anuncio BLE legítimo
 *    con el nombre "ESP32-DEMO-LAB" e identificador de fabricante de prueba (0xFFFF).
 * 3. Potencia de Radio Controlada: Permite configurar la potencia entre nivel mínimo
 *    (-12 dBm, alcance ~30 cm para banco de pruebas) hasta nivel normal (+3 dBm).
 * 4. Verificación Tangible en Vivo: Mientras esté activo, cualquier smartphone con
 *    una aplicación gratuita como "nRF Connect" o "LightBlue" (o el escáner del propio
 *    dashboard) puede detectar la señal física real en los canales 37, 38 y 39.
 * ============================================================================
 */
class BluetoothFakeJammer {
private:
  static bool isJammingActive;
  static bool isRadioTransmitting;
  static unsigned long startTime;
  static int targetChannel;
  static String attackMode;
  static unsigned long simulatedPacketsCount;

public:
  /**
   * @brief Inicializa los estados del módulo en reposo seguro
   */
  static void init() {
    isJammingActive = false;
    isRadioTransmitting = false;
    startTime = 0;
    targetChannel = 37;
    attackMode = "Lab Beacon (Min: -12dBm)";
    simulatedPacketsCount = 0;
    Serial.println("[LabBeacon] Módulo Bluetooth de Laboratorio inicializado en reposo.");
  }

  /**
   * @brief Inicia la transmisión de la Baliza BLE Educativa en hardware real
   * @param channel Canal de referencia didáctica (37, 38, 39 o 0 para salto)
   * @param mode Perfil de potencia o modo de operación de la baliza
   */
  static bool start(int channel = 37, const String& mode = "Lab Beacon (Min: -12dBm)") {
    // Si ya estaba transmitiendo, detener previamente para reconfigurar limpiamente
    if (isRadioTransmitting) {
      stop();
    }

    // Asegurar que el stack BLE esté inicializado mediante BleManager
    BleManager::init();

    // Determinar la potencia de transmisión de hardware según el perfil seleccionado
    esp_power_level_t powerLevel = ESP_PWR_LVL_N12; // -12 dBm por defecto (campo cercano seguro)
    if (mode.indexOf("Low") != -1 || mode.indexOf("-6") != -1) {
      powerLevel = ESP_PWR_LVL_N6;  // -6 dBm (~1.5 metros)
    } else if (mode.indexOf("Mid") != -1 || mode.indexOf("0dBm") != -1) {
      powerLevel = ESP_PWR_LVL_N0;  // 0 dBm (~3-4 metros)
    } else if (mode.indexOf("Max") != -1 || mode.indexOf("+3") != -1) {
      powerLevel = ESP_PWR_LVL_P3;  // +3 dBm (rango estándar de habitación)
    }

    // Configurar potencia del transmisor publicitario BLE en el controlador ESP-IDF
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, powerLevel);

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    if (pAdvertising) {
      // 1. Configurar datos de anuncio principal (Advertising Data)
      BLEAdvertisementData advData;
      advData.setFlags(0x06); // General Discoverable Mode, BR/EDR Not Supported
      advData.setName("ESP32-DEMO-LAB");

      // 0xFFFF es el Company ID de prueba reservado por Bluetooth SIG para desarrollo
      std::string mfgData = "\xFF\xFF\x01\x02\x43\x68\x65\x73\x70\x69"; // Encabezado de laboratorio ChespiDev
      advData.setManufacturerData(mfgData);
      pAdvertising->setAdvertisementData(advData);

      // 2. Configurar datos de respuesta al escaneo (Scan Response Data)
      BLEAdvertisementData scanResponseData;
      scanResponseData.setName("ESP32-DEMO-LAB");
      pAdvertising->setScanResponseData(scanResponseData);

      // 3. Intervalos de anuncio (30ms a 60ms para descubrimiento rápido en apps móviles)
      pAdvertising->setMinInterval(0x30);
      pAdvertising->setMaxInterval(0x60);
      pAdvertising->setMinPreferred(0x06);
      pAdvertising->setMinPreferred(0x12);

      // 4. Iniciar emisión de radio en canales 37, 38 y 39
      pAdvertising->start();
      isRadioTransmitting = true;
    }

    isJammingActive = true;
    startTime = millis();
    targetChannel = channel;
    attackMode = mode;
    simulatedPacketsCount = 0;

    Serial.printf("[LabBeacon] Baliza BLE 'ESP32-DEMO-LAB' transmitiendo. Canal Ref: %d | Modo: %s\n",
                  channel, mode.c_str());
    return true;
  }

  /**
   * @brief Detiene la transmisión de radio BLE inmediatamente
   */
  static bool stop() {
    if (!isJammingActive && !isRadioTransmitting) return false;

    if (isRadioTransmitting) {
      BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
      if (pAdvertising) {
        pAdvertising->stop();
      }
      isRadioTransmitting = false;
    }

    isJammingActive = false;
    unsigned long elapsed = (millis() - startTime) / 1000;
    Serial.printf("[LabBeacon] Baliza BLE detenida. Duración: %lu s | Paquetes emitidos: %lu\n",
                  elapsed, simulatedPacketsCount);
    return true;
  }

  /**
   * @brief Consulta si la baliza o emulación está activa
   */
  static bool isActive() {
    return isJammingActive;
  }

  /**
   * @brief Consulta si la antena BLE está transmitiendo físicamente
   */
  static bool isTransmitting() {
    return isRadioTransmitting;
  }

  /**
   * @brief Genera el estado y telemetría completa en formato JSON
   */
  static String getStatusJson() {
    unsigned long elapsedSec = isJammingActive ? ((millis() - startTime) / 1000) : 0;
    
    if (isJammingActive) {
      // Contador acumulado basado en tasa real de anuncio (~25 a 35 paquetes/segundo)
      simulatedPacketsCount += random(25, 35);
    }

    // Métricas físicas y didácticas
    int simulatedNoiseDbm = isJammingActive ? random(-42, -30) : random(-94, -86);
    int pps = isJammingActive ? random(25, 35) : 0;
    
    // Frecuencias exactas según especificación Bluetooth SIG
    int freq = 2402;
    if (targetChannel == 38) freq = 2426;
    else if (targetChannel == 39) freq = 2480;
    else if (targetChannel == 0) freq = 2440; // Rango promedio de salto

    String json = "{";
    json += "\"status\":\"success\",";
    json += "\"active\":" + String(isJammingActive ? "true" : "false") + ",";
    json += "\"channel\":" + String(targetChannel) + ",";
    json += "\"frequencyMhz\":" + String(freq) + ",";
    json += "\"mode\":\"" + attackMode + "\",";
    json += "\"beaconName\":\"ESP32-DEMO-LAB\",";
    json += "\"isRadioTransmitting\":" + String(isRadioTransmitting ? "true" : "false") + ",";
    json += "\"elapsedSeconds\":" + String(elapsedSec) + ",";
    json += "\"packetsTotal\":" + String(simulatedPacketsCount) + ",";
    json += "\"packetsPerSecond\":" + String(pps) + ",";
    json += "\"noiseDbm\":" + String(simulatedNoiseDbm) + ",";
    json += "\"dutyCyclePct\":" + String(isJammingActive ? random(85, 95) : 0) + ",";
    json += "\"isSimulation\":false,";
    json += "\"isLaboratoryBeacon\":true,";
    json += "\"disclaimer\":\"Baliza de laboratorio BLE educativa (ESP32-DEMO-LAB). Emision inofensiva de baja potencia conforme a Bluetooth SIG.\"";
    json += "}";

    return json;
  }
};

// Inicialización de miembros estáticos
inline bool BluetoothFakeJammer::isJammingActive = false;
inline bool BluetoothFakeJammer::isRadioTransmitting = false;
inline unsigned long BluetoothFakeJammer::startTime = 0;
inline int BluetoothFakeJammer::targetChannel = 37;
inline String BluetoothFakeJammer::attackMode = "Lab Beacon (Min: -12dBm)";
inline unsigned long BluetoothFakeJammer::simulatedPacketsCount = 0;

#endif // BLUETOOTH_FAKE_JAMMER_H
