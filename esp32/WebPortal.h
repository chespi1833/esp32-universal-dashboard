// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef WEB_PORTAL_H
#define WEB_PORTAL_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <esp_system.h>
#include "Config.h"
#include "StorageManager.h"
#include "BleManager.h"
#include "TestModule.h"
#include "LanScannerManager.h"
#include "UI_Setup.h"
#include "UI_Dashboard.h"

/**
 * @brief Servidor Web Universal y Portal Cautivo
 * Administra las rutas HTTP, endpoints de la API REST, redirección DNS para el portal cautivo
 * y el ciclo de vida del cambio entre Modo AP (Configuración) y Modo STA (Operación).
 */
class WebPortal {
private:
  static WebServer server;
  static DNSServer dnsServer;
  static bool isApMode;

  // Variables de control durante la configuración de conexión
  static String pendingSSID;
  static String pendingPass;
  static bool isConnectingRouter;
  static unsigned long connectStartTime;

  // Control de reinicio diferido
  static bool restartScheduled;
  static unsigned long restartTime;

  // ================= PARSERS Y HELPERS =================
  static String extractJsonField(const String& json, const String& field) {
    String pattern = "\"" + field + "\":\"";
    int start = json.indexOf(pattern);
    if (start == -1) {
      pattern = "\"" + field + "\":";
      start = json.indexOf(pattern);
      if (start == -1) return "";
      start += pattern.length();
      int end = json.indexOf(",", start);
      if (end == -1) end = json.indexOf("}", start);
      if (end == -1) return "";
      String val = json.substring(start, end);
      val.trim();
      return val;
    }
    start += pattern.length();
    int end = json.indexOf("\"", start);
    if (end == -1) return "";
    return json.substring(start, end);
  }

  static String getWifiScanJson() {
    Serial.println("[Wi-Fi] Iniciando escaneo de redes de 2.4 GHz...");
    int n = WiFi.scanNetworks(false, true);
    Serial.printf("[Wi-Fi] Escaneo finalizado. Redes encontradas: %d\n", n);

    String json = "{\"count\":" + String(n) + ",\"networks\":[";
    for (int i = 0; i < n; i++) {
      String ssid = WiFi.SSID(i);
      ssid.replace("\"", "\\\"");
      String bssid = WiFi.BSSIDstr(i);
      int rssi = WiFi.RSSI(i);
      int channel = WiFi.channel(i);
      wifi_auth_mode_t enc = WiFi.encryptionType(i);

      String encStr = "Abierta";
      bool isSecure = true;
      switch (enc) {
        case WIFI_AUTH_OPEN: encStr = "Abierta"; isSecure = false; break;
        case WIFI_AUTH_WEP: encStr = "WEP"; break;
        case WIFI_AUTH_WPA_PSK: encStr = "WPA"; break;
        case WIFI_AUTH_WPA2_PSK: encStr = "WPA2"; break;
        case WIFI_AUTH_WPA_WPA2_PSK: encStr = "WPA/WPA2"; break;
        case WIFI_AUTH_WPA2_ENTERPRISE: encStr = "WPA2-Enterprise"; break;
        case WIFI_AUTH_WPA3_PSK: encStr = "WPA3"; break;
        case WIFI_AUTH_WPA2_WPA3_PSK: encStr = "WPA2/WPA3"; break;
        default: encStr = "Desconocida"; break;
      }

      int quality = 0;
      if (rssi <= -100) quality = 0;
      else if (rssi >= -50) quality = 100;
      else quality = 2 * (rssi + 100);

      if (i > 0) json += ",";
      json += "{";
      json += "\"ssid\":\"" + ssid + "\",";
      json += "\"bssid\":\"" + bssid + "\",";
      json += "\"rssi\":" + String(rssi) + ",";
      json += "\"quality\":" + String(quality) + ",";
      json += "\"channel\":" + String(channel) + ",";
      json += "\"security\":\"" + encStr + "\",";
      json += "\"isSecure\":" + String(isSecure ? "true" : "false");
      json += "}";
    }
    json += "]}";
    WiFi.scanDelete();
    return json;
  }

  // ================= CONTROLADORES DE RUTA (AP MODE) =================
  static void handleServeSetupPage() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.send_P(200, "text/html", PAGE_SETUP);
  }

  static void handleApiWifiScan() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String json = getWifiScanJson();
    server.send(200, "application/json", json);
  }

  static void handleApiWifiConnect() {
    String ssid = "";
    String pass = "";

    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      ssid = extractJsonField(body, "ssid");
      pass = extractJsonField(body, "password");
    } else {
      if (server.hasArg("ssid")) ssid = server.arg("ssid");
      if (server.hasArg("password")) pass = server.arg("password");
    }

    if (ssid.length() == 0) {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"SSID no puede estar vacío\"}");
      return;
    }

    pendingSSID = ssid;
    pendingPass = pass;
    isConnectingRouter = true;
    connectStartTime = millis();

    Serial.printf("[Wi-Fi] Iniciando intento de conexión a: %s\n", ssid.c_str());

    // Mantener AP y encender receptor STA para vincularse al router
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(pendingSSID.c_str(), pendingPass.c_str());

    server.send(200, "application/json", "{\"status\":\"connecting\"}");
  }

  static void handleApiWifiStatus() {
    server.sendHeader("Cache-Control", "no-cache");
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
      isConnectingRouter = false;
      String ip = WiFi.localIP().toString();
      int rssi = WiFi.RSSI();
      Serial.printf("[Wi-Fi] ¡Conectado al router con éxito! IP asignada: %s\n", ip.c_str());
      server.send(200, "application/json", "{\"status\":\"connected\",\"ip\":\"" + ip + "\",\"ssid\":\"" + pendingSSID + "\",\"rssi\":" + String(rssi) + "}");
    } else if (isConnectingRouter && (millis() - connectStartTime > (WIFI_CONNECT_TIMEOUT_SEC * 1000))) {
      isConnectingRouter = false;
      Serial.println("[Wi-Fi] Tiempo de espera agotado al intentar conectar.");
      server.send(200, "application/json", "{\"status\":\"failed\",\"message\":\"Tiempo de espera agotado. Revisa la contraseña.\"}");
    } else if (status == WL_CONNECT_FAILED) {
      isConnectingRouter = false;
      Serial.println("[Wi-Fi] Error de autenticación.");
      server.send(200, "application/json", "{\"status\":\"failed\",\"message\":\"Contraseña incorrecta o error de enlace.\"}");
    } else {
      server.send(200, "application/json", "{\"status\":\"connecting\"}");
    }
  }

  static void handleApiWifiFinish() {
    Serial.println("[Wi-Fi] Finalizando configuración y apagando punto de acceso...");
    
    // Guardar credenciales de forma permanente en memoria NVS Flash
    StorageManager::saveCredentials(pendingSSID, pendingPass);

    server.send(200, "application/json", "{\"success\":true,\"message\":\"Configuración guardada. Apagando AP y reiniciando en modo normal...\"}");

    // Programar reinicio limpio en 1.2 segundos para asegurar entrega de respuesta HTTP
    restartScheduled = true;
    restartTime = millis() + 1200;
  }

  static void handleCaptiveRedirect() {
    String host = server.hostHeader();
    if (host != "192.168.4.1") {
      server.sendHeader("Location", "http://192.168.4.1/", true);
      server.send(302, "text/plain", "");
      return;
    }
    handleServeSetupPage();
  }

  // ================= CONTROLADORES DE RUTA (STA DASHBOARD MODE) =================
  static void handleServeDashboardPage() {
    server.sendHeader("Cache-Control", "no-cache");
    server.send_P(200, "text/html", PAGE_DASHBOARD);
  }

  static void handleApiBleScan() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String json = BleManager::scanDevices(BLE_SCAN_TIME_SEC);
    server.send(200, "application/json", json);
  }

  static void handleApiSystemInfo() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    String ssid = WiFi.SSID();
    String ip = WiFi.localIP().toString();
    int rssi = WiFi.RSSI();
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t uptimeSec = millis() / 1000;
    String mac = WiFi.macAddress();
    uint32_t cpuFreq = getCpuFrequencyMhz();
    uint32_t flashSize = ESP.getFlashChipSize();
    const char* sdk = ESP.getSdkVersion();

    String json = "{";
    json += "\"ssid\":\"" + ssid + "\",";
    json += "\"ip\":\"" + ip + "\",";
    json += "\"rssi\":" + String(rssi) + ",";
    json += "\"freeHeap\":" + String(freeHeap) + ",";
    json += "\"uptimeSec\":" + String(uptimeSec) + ",";
    json += "\"mac\":\"" + mac + "\",";
    json += "\"chipModel\":\"ESP32-D0WDQ6 Dual Core\",";
    json += "\"cpuFreq\":" + String(cpuFreq) + ",";
    json += "\"flashSize\":" + String(flashSize) + ",";
    json += "\"sdk\":\"" + String(sdk) + "\"";
    json += "}";

    server.send(200, "application/json", json);
  }

  static void handleApiResetWifi() {
    Serial.println("[System] Solicitud de reseteo de Wi-Fi recibida.");
    StorageManager::clearCredentials();
    server.send(200, "application/json", "{\"success\":true,\"message\":\"Credenciales borradas. Reiniciando en modo AP...\"}");
    restartScheduled = true;
    restartTime = millis() + 1000;
  }

  static void handleApiRestart() {
    Serial.println("[System] Reiniciando microcontrolador...");
    server.send(200, "application/json", "{\"success\":true}");
    restartScheduled = true;
    restartTime = millis() + 1000;
  }

  // ================= CONTROLADORES DE RUTA (MÓDULO TEST) =================
  static void handleApiTestRun() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String param = "Default Param";
    int cmd = 1;

    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      String p = extractJsonField(body, "param");
      if (p.length() > 0) param = p;
      String c = extractJsonField(body, "command");
      if (c.length() > 0) cmd = c.toInt();
    } else {
      if (server.hasArg("param")) param = server.arg("param");
      if (server.hasArg("command")) cmd = server.arg("command").toInt();
    }

    String json = TestModule::TestFunction(param, cmd);
    server.send(200, "application/json", json);
  }

  static void handleApiTestStatus() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String json = TestModule::getStatusJson();
    server.send(200, "application/json", json);
  }

  // ================= CONTROLADORES DE RUTA (ESCÁNER DE RED LAN) =================
  static void handleApiLanScan() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    bool started = LanScannerManager::startScan();
    if (started) {
      server.send(200, "application/json", "{\"status\":\"started\",\"message\":\"Escaneo de red local iniciado en segundo plano\"}");
    } else {
      if (LanScannerManager::isScanning()) {
        server.send(200, "application/json", "{\"status\":\"busy\",\"message\":\"Ya hay un escaneo de red en progreso\"}");
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Wi-Fi no conectado a la red local\"}");
      }
    }
  }

  static void handleApiLanDevices() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String json = LanScannerManager::getDevicesJson();
    server.send(200, "application/json", json);
  }

  static void handleNotFound() {
    if (isApMode) {
      handleCaptiveRedirect();
    } else {
      server.send(404, "text/plain", "Recurso no encontrado. Accede a http://esp32.local");
    }
  }

public:
  /**
   * @brief Configura e inicia el servidor en Modo AP (Portal Cautivo)
   */
  static void startApMode(const String& apSSID) {
    isApMode = true;
    WiFi.mode(WIFI_AP);

    IPAddress apIP(AP_IP_OCTET_1, AP_IP_OCTET_2, AP_IP_OCTET_3, AP_IP_OCTET_4);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(apSSID.c_str(), AP_WIFI_PASSWORD);

    Serial.printf("[AP] Red abierta creada: %s\n", apSSID.c_str());
    Serial.printf("[AP] Servidor Web y DNS disponibles en: http://%s\n", apIP.toString().c_str());

    // Iniciar servidor DNS en puerto 53 para portal cautivo
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", apIP);

    // Rutas para configuración inicial
    server.on("/", HTTP_GET, handleServeSetupPage);
    server.on("/api/wifi/scan", HTTP_GET, handleApiWifiScan);
    server.on("/api/wifi/connect", HTTP_POST, handleApiWifiConnect);
    server.on("/api/wifi/status", HTTP_GET, handleApiWifiStatus);
    server.on("/api/wifi/finish", HTTP_POST, handleApiWifiFinish);

    // Detección de portales cautivos de Android, iOS, Windows y macOS
    server.on("/generate_204", handleServeSetupPage);
    server.on("/gen_204", handleServeSetupPage);
    server.on("/hotspot-detect.html", handleServeSetupPage);
    server.on("/canonical.html", handleServeSetupPage);
    server.on("/connecttest.txt", handleServeSetupPage);
    server.on("/ncsi.txt", handleServeSetupPage);

    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("[AP] Servidor HTTP de configuración activo en puerto 80.");
  }

  /**
   * @brief Configura e inicia el servidor en Modo Normal (Estación STA conectada a router)
   */
  static void startStationMode() {
    isApMode = false;

    // Iniciar respondedor mDNS (esp32.local)
    if (MDNS.begin(MDNS_HOSTNAME)) {
      Serial.printf("[mDNS] Respondedor activo. Accede como: http://%s.local\n", MDNS_HOSTNAME);
      MDNS.addService("http", "tcp", 80);
    } else {
      Serial.println("[mDNS] Error al iniciar mDNS.");
    }

    // Inicializar submódulos de servicio
    TestModule::init();
    LanScannerManager::init();

    // Rutas para Panel de Control Principal
    server.on("/", HTTP_GET, handleServeDashboardPage);
    server.on("/api/wifi/scan", HTTP_GET, handleApiWifiScan);
    server.on("/api/ble/scan", HTTP_GET, handleApiBleScan);
    server.on("/api/lan/scan", HTTP_POST, handleApiLanScan);
    server.on("/api/lan/devices", HTTP_GET, handleApiLanDevices);
    server.on("/api/test/run", HTTP_POST, handleApiTestRun);
    server.on("/api/test/status", HTTP_GET, handleApiTestStatus);
    server.on("/api/system/info", HTTP_GET, handleApiSystemInfo);
    server.on("/api/system/reset-wifi", HTTP_POST, handleApiResetWifi);
    server.on("/api/system/restart", HTTP_POST, handleApiRestart);

    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("[STA] Servidor HTTP del Dashboard activo en puerto 80.");
  }

  /**
   * @brief Bucle de atención de peticiones en loop()
   */
  static void handleClient() {
    if (isApMode) {
      dnsServer.processNextRequest();
    }
    server.handleClient();

    // Gestión de reinicio diferido para garantizar respuesta HTTP antes de reiniciar
    if (restartScheduled && millis() > restartTime) {
      Serial.println("[System] Ejecutando reinicio programado...");
      ESP.restart();
    }
  }

  static bool getIsApMode() {
    return isApMode;
  }
};

// Inicialización de variables estáticas
inline WebServer WebPortal::server(80);
inline DNSServer WebPortal::dnsServer;
inline bool WebPortal::isApMode = false;
inline String WebPortal::pendingSSID = "";
inline String WebPortal::pendingPass = "";
inline bool WebPortal::isConnectingRouter = false;
inline unsigned long WebPortal::connectStartTime = 0;
inline bool WebPortal::restartScheduled = false;
inline unsigned long WebPortal::restartTime = 0;

#endif // WEB_PORTAL_H
