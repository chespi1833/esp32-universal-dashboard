// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

/**
 * @brief Gestor de persistencia en memoria Flash no volátil (NVS)
 * Utiliza la biblioteca oficial Preferences de ESP32 para almacenar de forma segura
 * las credenciales Wi-Fi y configuraciones operativas.
 */
class StorageManager {
private:
  static Preferences prefs;
  static const char* NAMESPACE;

public:
  static void init() {
    // La inicialización se realiza bajo demanda en cada operación
  }

  /**
   * @brief Comprueba si ya existen credenciales Wi-Fi válidas guardadas
   */
  static bool hasSavedCredentials() {
    prefs.begin(NAMESPACE, true); // Modo sólo lectura
    String ssid = prefs.getString("ssid", "");
    prefs.end();
    return (ssid.length() > 0);
  }

  /**
   * @brief Obtiene el SSID guardado en memoria
   */
  static String getSSID() {
    prefs.begin(NAMESPACE, true);
    String ssid = prefs.getString("ssid", "");
    prefs.end();
    return ssid;
  }

  /**
   * @brief Obtiene la contraseña guardada en memoria
   */
  static String getPassword() {
    prefs.begin(NAMESPACE, true);
    String pass = prefs.getString("pass", "");
    prefs.end();
    return pass;
  }

  /**
   * @brief Guarda las nuevas credenciales de red Wi-Fi
   */
  static bool saveCredentials(const String& ssid, const String& password) {
    if (ssid.length() == 0) return false;
    
    prefs.begin(NAMESPACE, false); // Modo lectura/escritura
    prefs.putString("ssid", ssid);
    prefs.putString("pass", password);
    prefs.putBool("configured", true);
    prefs.end();
    
    Serial.printf("[Storage] Credenciales guardadas exitosamente para la red: %s\n", ssid.c_str());
    return true;
  }

  /**
   * @brief Borra las credenciales y restablece el estado de fábrica
   */
  static void clearCredentials() {
    prefs.begin(NAMESPACE, false);
    prefs.clear();
    prefs.end();
    Serial.println("[Storage] Credenciales Wi-Fi eliminadas de NVS.");
  }
};

// Definición estática del namespace
inline Preferences StorageManager::prefs;
inline const char* StorageManager::NAMESPACE = "esp32_cfg";

#endif // STORAGE_MANAGER_H
