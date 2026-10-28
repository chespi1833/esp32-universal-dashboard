// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef BLUETOOTH_FAKE_JAMMER_H
#define BLUETOOTH_FAKE_JAMMER_H

#include <Arduino.h>

/**
 * ============================================================================
 * @file BluetoothFakeJammer.h
 * @brief Módulo de Demostración y Simulación de Interferencia RF BLE (Fake Jammer)
 * 
 * NOTA LEGAL Y TÉCNICA:
 * Este módulo es 100% simulado con fines didácticos, de pruebas de interfaz de usuario
 * y demostración metodológica. NO emite interferencias electromagnéticas reales ni
 * altera la capa física de radio del ESP32.
 * ============================================================================
 */
class BluetoothFakeJammer {
private:
  static bool isJammingActive;
  static unsigned long startTime;
  static int targetChannel;
  static String attackMode;
  static unsigned long simulatedPacketsCount;

public:
  static void init() {
    isJammingActive = false;
    startTime = 0;
    targetChannel = 37;
    attackMode = "Beacon Flood";
    simulatedPacketsCount = 0;
    Serial.println("[FakeJammer] Módulo BluetoothFakeJammer inicializado en modo simulación.");
  }

  static bool start(int channel = 37, const String& mode = "Beacon Flood") {
    isJammingActive = true;
    startTime = millis();
    targetChannel = channel;
    attackMode = mode;
    simulatedPacketsCount = 0;
    Serial.printf("[FakeJammer] Simulación iniciada en Canal BLE %d con perfil: %s\n",
                  channel, mode.c_str());
    return true;
  }

  static bool stop() {
    if (!isJammingActive) return false;
    isJammingActive = false;
    unsigned long elapsed = (millis() - startTime) / 1000;
    Serial.printf("[FakeJammer] Simulación detenida. Duración: %lu s | Paquetes emulados: %lu\n",
                  elapsed, simulatedPacketsCount);
    return true;
  }

  static bool isActive() {
    return isJammingActive;
  }

  static String getStatusJson() {
    unsigned long elapsedSec = isJammingActive ? ((millis() - startTime) / 1000) : 0;
    
    if (isJammingActive) {
      simulatedPacketsCount += random(850, 1450);
    }

    int simulatedNoiseDbm = isJammingActive ? random(-38, -24) : random(-94, -86);
    int pps = isJammingActive ? random(1300, 2600) : 0;
    int freq = 2440;
    if (targetChannel == 37) freq = 2402;
    else if (targetChannel == 38) freq = 2426;
    else if (targetChannel == 39) freq = 2480;

    String json = "{";
    json += "\"status\":\"success\",";
    json += "\"active\":" + String(isJammingActive ? "true" : "false") + ",";
    json += "\"channel\":" + String(targetChannel) + ",";
    json += "\"frequencyMhz\":" + String(freq) + ",";
    json += "\"mode\":\"" + attackMode + "\",";
    json += "\"elapsedSeconds\":" + String(elapsedSec) + ",";
    json += "\"packetsTotal\":" + String(simulatedPacketsCount) + ",";
    json += "\"packetsPerSecond\":" + String(pps) + ",";
    json += "\"noiseDbm\":" + String(simulatedNoiseDbm) + ",";
    json += "\"dutyCyclePct\":" + String(isJammingActive ? random(91, 99) : 0) + ",";
    json += "\"isSimulation\":true,";
    json += "\"disclaimer\":\"Módulo de simulación visual y didáctica. Sin emisión física de radio.\"";
    json += "}";

    return json;
  }
};

// Inicialización de miembros estáticos
inline bool BluetoothFakeJammer::isJammingActive = false;
inline unsigned long BluetoothFakeJammer::startTime = 0;
inline int BluetoothFakeJammer::targetChannel = 37;
inline String BluetoothFakeJammer::attackMode = "Beacon Flood";
inline unsigned long BluetoothFakeJammer::simulatedPacketsCount = 0;

#endif // BLUETOOTH_FAKE_JAMMER_H
