// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

/**
 * @brief Gestor de escaneo y telemetría Bluetooth Low Energy (BLE)
 * Realiza escaneos activos, calcula la distancia estimada en base a RSSI y TxPower,
 * y decodifica todos los metadatos de los paquetes de anuncio (Advertising Data).
 */
class BleManager {
private:
  static bool isInitialized;

  /**
   * @brief Convierte una cadena de bytes binarios a una representación Hexadecimal legible
   */
  static String toHexString(const String& data) {
    if (data.length() == 0) return "";
    String hex = "";
    char buffer[4];
    for (size_t i = 0; i < data.length(); i++) {
      sprintf(buffer, "%02X", (uint8_t)data[i]);
      hex += buffer;
      if (i < data.length() - 1) hex += " ";
    }
    return hex;
  }

  /**
   * @brief Calcula la distancia aproximada en metros según el modelo Log-Distance Path Loss
   * RSSI = -10 * n * log10(d) + TxPower
   * d = 10 ^ ((TxPower - RSSI) / (10 * n))
   */
  static float calculateEstimatedDistance(int rssi, int txPower, float pathLossExponent = 2.0f) {
    if (rssi == 0) return -1.0f; // No medible
    float ratio = (float)(txPower - rssi) / (10.0f * pathLossExponent);
    float distance = pow(10.0f, ratio);
    if (distance < 0.1f) distance = 0.1f;
    return distance;
  }

public:
  static void init() {
    if (!isInitialized) {
      BLEDevice::init("ESP32-DevKit-Core");
      isInitialized = true;
      Serial.println("[BLE] Stack Bluetooth inicializado correctamente.");
    }
  }

  /**
   * @brief Ejecuta un escaneo BLE y devuelve el resultado en formato JSON estructurado
   * @param scanDuration Duración del escaneo en segundos (por defecto 4s)
   */
  static String scanDevices(int scanDuration = 4) {
    init();

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true); // Active scan para solicitar Scan Response (nombre, mfg)
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    Serial.printf("[BLE] Iniciando escaneo de %d segundos...\n", scanDuration);
    BLEScanResults *foundDevices = pBLEScan->start(scanDuration, false);
    int count = (foundDevices != nullptr) ? foundDevices->getCount() : 0;
    Serial.printf("[BLE] Escaneo finalizado. Dispositivos encontrados: %d\n", count);

    String json = "{\"count\":" + String(count) + ",\"devices\":[";

    for (int i = 0; i < count; i++) {
      BLEAdvertisedDevice dev = foundDevices->getDevice(i);

      // Nombre del dispositivo
      String name = dev.haveName() ? String(dev.getName().c_str()) : "Dispositivo Desconocido";
      name.replace("\"", "\\\"");
      name.replace("\n", " ");
      name.replace("\r", "");

      // Dirección MAC y Tipo
      String mac = String(dev.getAddress().toString().c_str());
      mac.toUpperCase();
      String addrType = (dev.getAddressType() == 0) ? "Public" : "Random";

      // Medición RSSI
      int rssi = dev.getRSSI();

      // Potencia de Transmisión (TxPower a 1 metro)
      int txPower = -59; // Valor de calibración estándar a 1 metro en caso de no transmitirse
      bool hasTxPower = dev.haveTXPower();
      if (hasTxPower) {
        int tx = dev.getTXPower();
        // Filtrar valores plausibles de RSSI a 1m
        if (tx < -30 && tx > -100) {
          txPower = tx;
        }
      }

      // Cálculo de distancia estimada
      float distance = calculateEstimatedDistance(rssi, txPower, 2.0f);
      String distCategory = "Cercano";
      if (distance < 1.0f) {
        distCategory = "Inmediato (<1m)";
      } else if (distance < 3.0f) {
        distCategory = "Cercano (1-3m)";
      } else if (distance < 8.0f) {
        distCategory = "Medio (3-8m)";
      } else {
        distCategory = "Lejano (>8m)";
      }

      // Servicio UUID anunciado
      String serviceUUID = "";
      if (dev.haveServiceUUID()) {
        serviceUUID = String(dev.getServiceUUID().toString().c_str());
      }

      // Datos de Fabricante (Manufacturer Data)
      String mfgHex = "";
      if (dev.haveManufacturerData()) {
        mfgHex = toHexString(dev.getManufacturerData());
      }

      // Datos de Servicio (Service Data)
      String srvHex = "";
      if (dev.haveServiceData()) {
        srvHex = toHexString(dev.getServiceData());
      }

      // Apariencia (Bluetooth SIG Appearance category)
      uint32_t appearance = dev.haveAppearance() ? dev.getAppearance() : 0;

      if (i > 0) json += ",";
      json += "{";
      json += "\"name\":\"" + name + "\",";
      json += "\"mac\":\"" + mac + "\",";
      json += "\"addrType\":\"" + addrType + "\",";
      json += "\"rssi\":" + String(rssi) + ",";
      json += "\"txPower\":" + (hasTxPower ? String(txPower) : "null") + ",";
      json += "\"distance\":" + String(distance, 2) + ",";
      json += "\"distCategory\":\"" + distCategory + "\",";
      json += "\"serviceUUID\":\"" + serviceUUID + "\",";
      json += "\"mfgData\":\"" + mfgHex + "\",";
      json += "\"serviceData\":\"" + srvHex + "\",";
      json += "\"appearance\":" + String(appearance);
      json += "}";
    }

    json += "]}";

    // Muy importante: liberar memoria dinámica del escáner BLE
    pBLEScan->clearResults();

    return json;
  }
};

// Definición de variable estática
inline bool BleManager::isInitialized = false;

#endif // BLE_MANAGER_H
