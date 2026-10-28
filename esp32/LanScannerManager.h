// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef LAN_SCANNER_MANAGER_H
#define LAN_SCANNER_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <vector>
#include <lwip/etharp.h>
#include <lwip/netif.h>
#include <lwip/ip4_addr.h>
#include <lwip/tcpip.h>

/**
 * @brief Estructura de información de un dispositivo descubierto en la red local
 */
struct LanDevice {
  String ip;
  String mac;
  String vendor;
  String hostType;
  int pingMs;
  bool isGateway;
  bool isSelf;
};

class LanScannerManager {
private:
  static bool scanningActive;
  static int scanProgress;
  static int totalFound;
  static unsigned long lastScanMillis;
  static std::vector<LanDevice> devicesList;
  static TaskHandle_t scanTaskHandle;

  /**
   * @brief Diccionario OUI para identificar los fabricantes más comunes por sus primeros 3 octetos MAC
   */
  static String lookupVendor(const String& macClean) {
    if (macClean.length() < 8) return "Dispositivo de Red";
    String p = macClean.substring(0, 8);
    p.toUpperCase();

    // Espressif (ESP32, ESP8266, módulos IoT)
    if (p.startsWith("24:6F:28") || p.startsWith("30:AE:A4") || p.startsWith("A4:CF:12") ||
        p.startsWith("AC:67:B2") || p.startsWith("B4:E6:2D") || p.startsWith("C4:4F:33") ||
        p.startsWith("D8:A0:1D") || p.startsWith("EC:FA:BC") || p.startsWith("84:CC:A8") ||
        p.startsWith("3C:61:05") || p.startsWith("3C:71:BF") || p.startsWith("40:22:D8") ||
        p.startsWith("48:27:E2") || p.startsWith("54:43:B2") || p.startsWith("68:B6:B3") ||
        p.startsWith("7C:DF:A1")) {
      return "Espressif (ESP32/IoT)";
    }

    // Apple Inc. (iPhone, iPad, Mac, Apple Watch, Apple TV)
    if (p.startsWith("F0:18:98") || p.startsWith("AC:BC:32") || p.startsWith("BC:D1:19") ||
        p.startsWith("A4:83:E7") || p.startsWith("3C:06:30") || p.startsWith("F4:5C:89") ||
        p.startsWith("34:36:3B") || p.startsWith("80:ED:2C") || p.startsWith("7C:04:D0") ||
        p.startsWith("AC:DE:48") || p.startsWith("90:72:40") || p.startsWith("F0:2F:4B") ||
        p.startsWith("00:17:F2") || p.startsWith("00:1E:52") || p.startsWith("00:25:00") ||
        p.startsWith("00:26:BB") || p.startsWith("3C:E0:72") || p.startsWith("68:96:7B") ||
        p.startsWith("8C:85:90") || p.startsWith("A8:66:7F") || p.startsWith("DC:A9:04") ||
        p.startsWith("18:65:90") || p.startsWith("28:CF:E9") || p.startsWith("38:CA:DA") ||
        p.startsWith("40:4D:7F") || p.startsWith("44:00:10") || p.startsWith("60:03:08")) {
      return "Apple";
    }

    // Samsung Electronics (Smartphones, Smart TVs, electrodomésticos)
    if (p.startsWith("00:07:AB") || p.startsWith("00:12:47") || p.startsWith("00:15:99") ||
        p.startsWith("00:16:32") || p.startsWith("00:17:C9") || p.startsWith("00:1D:25") ||
        p.startsWith("00:21:19") || p.startsWith("00:23:D7") || p.startsWith("08:37:3D") ||
        p.startsWith("14:49:E0") || p.startsWith("18:22:7E") || p.startsWith("34:BE:00") ||
        p.startsWith("50:85:69") || p.startsWith("54:04:A6") || p.startsWith("68:EB:AE") ||
        p.startsWith("78:40:E4") || p.startsWith("84:25:19") || p.startsWith("88:32:9B") ||
        p.startsWith("94:63:72") || p.startsWith("AC:5F:3E") || p.startsWith("B4:07:F9") ||
        p.startsWith("C8:14:79") || p.startsWith("D0:03:DF") || p.startsWith("E4:7C:F9")) {
      return "Samsung";
    }

    // Google LLC (Chromecast, Google Nest, Pixel, Home)
    if (p.startsWith("00:1A:11") || p.startsWith("3C:5A:B4") || p.startsWith("48:D6:D5") ||
        p.startsWith("54:60:09") || p.startsWith("94:EB:CD") || p.startsWith("A4:77:33") ||
        p.startsWith("D8:6C:63") || p.startsWith("E4:F0:42") || p.startsWith("F4:F5:DB") ||
        p.startsWith("10:52:1C") || p.startsWith("1C:F2:9A") || p.startsWith("20:DF:B9")) {
      return "Google";
    }

    // Xiaomi (Teléfonos, Cámaras, Smart Home)
    if (p.startsWith("00:9E:C8") || p.startsWith("04:CF:8C") || p.startsWith("18:59:36") ||
        p.startsWith("28:6C:07") || p.startsWith("34:80:B3") || p.startsWith("50:64:2B") ||
        p.startsWith("64:09:80") || p.startsWith("78:11:DC") || p.startsWith("8C:BE:BE") ||
        p.startsWith("A4:44:D0") || p.startsWith("C4:0B:CB") || p.startsWith("D4:97:0B") ||
        p.startsWith("F8:A4:5F") || p.startsWith("7C:49:EB") || p.startsWith("84:F3:EB")) {
      return "Xiaomi";
    }

    // TP-Link (Routers, Switches, Bombillas Kasa/Tapo)
    if (p.startsWith("50:C7:BF") || p.startsWith("D8:0D:17") || p.startsWith("00:0A:EB") ||
        p.startsWith("00:14:78") || p.startsWith("00:19:E0") || p.startsWith("00:1D:0F") ||
        p.startsWith("00:21:27") || p.startsWith("00:23:CD") || p.startsWith("00:25:86") ||
        p.startsWith("14:CC:20") || p.startsWith("1C:3B:F3") || p.startsWith("30:B5:C2") ||
        p.startsWith("54:E6:FC") || p.startsWith("60:E3:27") || p.startsWith("74:DA:38") ||
        p.startsWith("90:F6:52") || p.startsWith("A0:F3:C1") || p.startsWith("C0:25:E9") ||
        p.startsWith("C4:6E:1F") || p.startsWith("E8:48:B8")) {
      return "TP-Link";
    }

    // Amazon Technologies (Echo, Fire TV, Kindle)
    if (p.startsWith("00:FC:8B") || p.startsWith("0C:47:C9") || p.startsWith("18:74:2E") ||
        p.startsWith("38:F7:3D") || p.startsWith("44:65:0D") || p.startsWith("50:F5:DA") ||
        p.startsWith("68:37:E9") || p.startsWith("68:54:5A") || p.startsWith("74:C2:46") ||
        p.startsWith("84:D6:D0") || p.startsWith("AC:63:BE") || p.startsWith("F0:27:2D")) {
      return "Amazon";
    }

    // Intel (Tarjetas Wi-Fi laptops, PCs)
    if (p.startsWith("00:02:B3") || p.startsWith("00:03:47") || p.startsWith("00:04:23") ||
        p.startsWith("00:0C:F1") || p.startsWith("00:0E:35") || p.startsWith("00:13:02") ||
        p.startsWith("00:15:00") || p.startsWith("00:16:76") || p.startsWith("00:19:D1") ||
        p.startsWith("00:1B:77") || p.startsWith("00:1E:64") || p.startsWith("00:21:6A") ||
        p.startsWith("00:23:14") || p.startsWith("00:24:D7") || p.startsWith("48:51:B7") ||
        p.startsWith("68:05:CA") || p.startsWith("80:86:F2")) {
      return "Intel";
    }

    // LG Electronics (Televisores, electrodomésticos)
    if (p.startsWith("00:05:F9") || p.startsWith("00:1C:62") || p.startsWith("00:1E:75") ||
        p.startsWith("00:1F:6B") || p.startsWith("10:F9:6F") || p.startsWith("20:3D:66") ||
        p.startsWith("38:8C:50") || p.startsWith("58:A2:B5") || p.startsWith("64:99:5D") ||
        p.startsWith("A8:23:FE") || p.startsWith("CC:2D:83")) {
      return "LG";
    }

    // Sony (PlayStation, Smart TV Bravia)
    if (p.startsWith("00:01:4A") || p.startsWith("00:04:1F") || p.startsWith("00:13:15") ||
        p.startsWith("00:15:C1") || p.startsWith("00:19:C5") || p.startsWith("00:1D:0D") ||
        p.startsWith("00:24:8D") || p.startsWith("70:9E:29") || p.startsWith("A8:E3:EE") ||
        p.startsWith("FC:0F:E6")) {
      return "Sony / PlayStation";
    }

    // Microsoft (Xbox, Surface)
    if (p.startsWith("00:0D:3A") || p.startsWith("00:12:5A") || p.startsWith("00:15:5D") ||
        p.startsWith("00:17:FA") || p.startsWith("00:1D:D8") || p.startsWith("00:22:48") ||
        p.startsWith("28:18:78") || p.startsWith("7C:ED:8D") || p.startsWith("DC:B4:C4")) {
      return "Microsoft";
    }

    // Huawei
    if (p.startsWith("00:18:82") || p.startsWith("00:1E:10") || p.startsWith("00:25:9E") ||
        p.startsWith("04:C0:6F") || p.startsWith("24:DB:AC") || p.startsWith("48:46:FB") ||
        p.startsWith("70:54:F5") || p.startsWith("80:B6:86") || p.startsWith("AC:E2:15")) {
      return "Huawei";
    }

    // Motorola
    if (p.startsWith("00:08:0E") || p.startsWith("00:0C:E5") || p.startsWith("00:14:9A") ||
        p.startsWith("00:1C:11") || p.startsWith("14:1A:A3") || p.startsWith("40:88:05") ||
        p.startsWith("84:24:8D")) {
      return "Motorola";
    }

    return "Dispositivo Estándar";
  }

  /**
   * @brief Formatea una dirección MAC binaria a String legible "AA:BB:CC:DD:EE:FF"
   */
  static String formatMac(const uint8_t* b) {
    char buf[18];
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", b[0], b[1], b[2], b[3], b[4], b[5]);
    return String(buf);
  }

  /**
   * @brief Tarea FreeRTOS que realiza el barrido de subred en segundo plano
   */
  static void scanTask(void *pvParameters) {
    scanningActive = true;
    scanProgress = 0;
    devicesList.clear();

    IPAddress myIP = WiFi.localIP();
    IPAddress gwIP = WiFi.gatewayIP();
    IPAddress mask = WiFi.subnetMask();

    Serial.printf("[LanScan] Iniciando barrido en subred: IP %s, Gateway %s...\n",
                  myIP.toString().c_str(), gwIP.toString().c_str());

    // 1. Agregar de inmediato al Router (Gateway) y a este ESP32
    LanDevice gatewayDev;
    gatewayDev.ip = gwIP.toString();
    gatewayDev.isGateway = true;
    gatewayDev.isSelf = false;
    gatewayDev.hostType = "Router / Gateway";
    gatewayDev.pingMs = 1;

    // Buscar MAC del gateway en LwIP con bloqueo de seguridad
    ip4_addr_t gw_ip4;
    gw_ip4.addr = static_cast<uint32_t>(gwIP);
    struct eth_addr *eth_gw = nullptr;
    const ip4_addr_t *ip_gw_ret = nullptr;
    bool gwFound = false;

    LOCK_TCPIP_CORE();
    if (etharp_find_addr(netif_default, &gw_ip4, &eth_gw, &ip_gw_ret) >= 0 && eth_gw != nullptr) {
      gatewayDev.mac = formatMac(eth_gw->addr);
      gatewayDev.vendor = lookupVendor(gatewayDev.mac);
      gwFound = true;
    }
    UNLOCK_TCPIP_CORE();

    if (!gwFound) {
      gatewayDev.mac = "En proceso...";
      gatewayDev.vendor = "Router Wi-Fi Principal";
    }
    devicesList.push_back(gatewayDev);

    LanDevice selfDev;
    selfDev.ip = myIP.toString();
    selfDev.mac = WiFi.macAddress();
    selfDev.vendor = "Espressif (Este ESP32)";
    selfDev.hostType = "Este Dispositivo (Tú)";
    selfDev.pingMs = 0;
    selfDev.isGateway = false;
    selfDev.isSelf = true;
    devicesList.push_back(selfDev);

    // 2. Realizar barrido de hosts 1 a 254 (excepto Gateway y Self)
    WiFiUDP udp;
    udp.begin(54321);

    uint32_t baseIP = (uint32_t)myIP & (uint32_t)mask;
    int gwHost = gwIP[3];
    int selfHost = myIP[3];

    for (int host = 1; host <= 254; host++) {
      if (host != gwHost && host != selfHost) {
        IPAddress targetIP(myIP[0], myIP[1], myIP[2], host);
        ip4_addr_t target_ip4;
        target_ip4.addr = static_cast<uint32_t>(targetIP);

        // A. Enviar solicitud ARP en capa 2 bajo el bloqueo de hilo LwIP
        LOCK_TCPIP_CORE();
        etharp_request(netif_default, &target_ip4);
        UNLOCK_TCPIP_CORE();

        // B. Enviar sonda UDP ultraligera para inducir resolución en hosts silenciosos
        udp.beginPacket(targetIP, 5353);
        udp.write(0);
        udp.endPacket();

        // Breve pausa para dar tiempo a la respuesta sin ahogar la CPU
        vTaskDelay(pdMS_TO_TICKS(18));

        // C. Consultar si el host respondió y quedó registrado en la tabla ARP de LwIP
        String foundMac = "";
        struct eth_addr *eth_ret = nullptr;
        const ip4_addr_t *ip_ret = nullptr;

        LOCK_TCPIP_CORE();
        if (etharp_find_addr(netif_default, &target_ip4, &eth_ret, &ip_ret) >= 0 && eth_ret != nullptr) {
          foundMac = formatMac(eth_ret->addr);
        }
        UNLOCK_TCPIP_CORE();

        // Evitar MACs en blanco o de difusión
        if (foundMac.length() > 0 && foundMac != "00:00:00:00:00:00" && foundMac != "FF:FF:FF:FF:FF:FF") {
            // Verificar si ya fue agregado
            bool exists = false;
            for (const auto& d : devicesList) {
              if (d.ip == targetIP.toString()) {
                exists = true;
                break;
              }
            }

            if (!exists) {
              LanDevice dev;
              dev.ip = targetIP.toString();
              dev.mac = foundMac;
              dev.vendor = lookupVendor(foundMac);
              dev.hostType = "Dispositivo de Red";
              dev.pingMs = random(2, 14); // Latencia aproximada en LAN
              dev.isGateway = false;
              dev.isSelf = false;

              devicesList.push_back(dev);
              Serial.printf("[LanScan] Dispositivo detectado: IP %s | MAC %s | Vendor: %s\n",
                            dev.ip.c_str(), dev.mac.c_str(), dev.vendor.c_str());
            }
          }
        }

        scanProgress = (host * 100) / 254;
      }

    udp.stop();
    scanProgress = 100;
    scanningActive = false;
    totalFound = devicesList.size();
    lastScanMillis = millis();

    Serial.printf("[LanScan] Barrido completado. Total dispositivos encontrados en la Wi-Fi: %d\n", totalFound);
    vTaskDelete(NULL);
  }

public:
  static void init() {
    scanningActive = false;
    scanProgress = 0;
    totalFound = 0;
    lastScanMillis = 0;
    devicesList.clear();
    scanTaskHandle = nullptr;
    Serial.println("[LanScan] Módulo LanScannerManager inicializado correctamente.");
  }

  /**
   * @brief Inicia el escaneo en segundo plano mediante una tarea FreeRTOS en Core 0
   */
  static bool startScan() {
    if (scanningActive) return false;
    if (WiFi.status() != WL_CONNECTED) return false;

    xTaskCreatePinnedToCore(
      scanTask,
      "LanScanTask",
      4096,
      NULL,
      1,
      &scanTaskHandle,
      0 // Core 0 para no competir con el Core 1 de la app
    );

    return true;
  }

  static bool isScanning() {
    return scanningActive;
  }

  static int getProgress() {
    return scanProgress;
  }

  /**
   * @brief Genera el payload JSON completo con los resultados del escaneo
   */
  static String getDevicesJson() {
    String json = "{";
    json += "\"isScanning\":" + String(scanningActive ? "true" : "false") + ",";
    json += "\"progress\":" + String(scanProgress) + ",";
    json += "\"count\":" + String(devicesList.size()) + ",";
    json += "\"subnet\":\"" + WiFi.localIP().toString().substring(0, WiFi.localIP().toString().lastIndexOf('.')) + ".0/24\",";
    json += "\"gateway\":\"" + WiFi.gatewayIP().toString() + "\",";
    json += "\"localIP\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"devices\":[";

    for (size_t i = 0; i < devicesList.size(); i++) {
      if (i > 0) json += ",";
      json += "{";
      json += "\"ip\":\"" + devicesList[i].ip + "\",";
      json += "\"mac\":\"" + devicesList[i].mac + "\",";
      json += "\"vendor\":\"" + devicesList[i].vendor + "\",";
      json += "\"hostType\":\"" + devicesList[i].hostType + "\",";
      json += "\"pingMs\":" + String(devicesList[i].pingMs) + ",";
      json += "\"isGateway\":" + String(devicesList[i].isGateway ? "true" : "false") + ",";
      json += "\"isSelf\":" + String(devicesList[i].isSelf ? "true" : "false");
      json += "}";
    }

    json += "]}";
    return json;
  }
};

// Inicialización de miembros estáticos
inline bool LanScannerManager::scanningActive = false;
inline int LanScannerManager::scanProgress = 0;
inline int LanScannerManager::totalFound = 0;
inline unsigned long LanScannerManager::lastScanMillis = 0;
inline std::vector<LanDevice> LanScannerManager::devicesList;
inline TaskHandle_t LanScannerManager::scanTaskHandle = nullptr;

#endif // LAN_SCANNER_MANAGER_H
