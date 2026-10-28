// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// CONFIGURACIÓN GLOBAL DEL SISTEMA - ESP32 DEVKIT V1
// ============================================================================

// Nombre de host mDNS. Permitirá acceder al ESP32 como http://esp32.local
#define MDNS_HOSTNAME "esp32"

// Prefijo de la red Wi-Fi de configuración creada en modo Punto de Acceso (AP)
// Se le añadirá al final los últimos 4 dígitos de la dirección MAC (ej: ESP32-Config-A1B2)
#define AP_SSID_PREFIX "ESP32-Config-"

// Contraseña para la red AP (vacío "" para red pública y abierta según requerimiento)
#define AP_WIFI_PASSWORD ""

// Dirección IP local del ESP32 en modo Punto de Acceso (Captive Portal)
#define AP_IP_OCTET_1 192
#define AP_IP_OCTET_2 168
#define AP_IP_OCTET_3 4
#define AP_IP_OCTET_4 1

// Pines de Hardware para ESP32 DevKit V1
#define PIN_LED_STATUS 2       // LED azul integrado (GPIO 2)
#define PIN_BUTTON_BOOT 0      // Botón BOOT integrado (GPIO 0) para reset físico

// Tiempos y Parámetros Operativos
#define WIFI_CONNECT_TIMEOUT_SEC 20     // Tiempo máximo para intentar conectar al router
#define BUTTON_RESET_HOLD_MS     3000   // Mantener pulsado botón BOOT 3s para borrar Wi-Fi
#define BLE_SCAN_TIME_SEC        4      // Duración del escaneo Bluetooth BLE en segundos

// Información del Firmware
#define FIRMWARE_NAME    "ESP32-Universal-Core"
#define FIRMWARE_VERSION "1.0.0"

#endif // CONFIG_H
