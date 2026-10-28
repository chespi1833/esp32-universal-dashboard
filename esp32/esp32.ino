/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 ChespiDev
 * ============================================================================
 * PROYECTO: ESP32 DevKit V1 - Núcleo Estándar y Universal de Desarrollo
 * ============================================================================
 *
 * Flujo Operativo:
 * 1. Primer inicio / Sin configuración previa:
 *    - El ESP32 crea un Punto de Acceso (AP) Wi-Fi público y abierto (ej:
 * ESP32-Config-A1B2).
 *    - El usuario se conecta a la red e ingresa al portal de configuración
 * (Captive Portal / 192.168.4.1).
 *    - La interfaz escanea redes Wi-Fi cercanas, solicita la contraseña y
 * prueba la conexión.
 *    - Al conectarse, muestra un resumen informativo con la IP y la URL
 * http://esp32.local.
 *    - El botón "Terminar y Desconectar" apaga la red provisional y guarda los
 * datos en memoria NVS.
 *
 * 2. Operación Normal (Conectado a la red local):
 *    - El ESP32 se conecta automáticamente a la red Wi-Fi configurada.
 *    - Se activa el servicio mDNS: accesible desde cualquier navegador en
 * http://esp32.local.
 *    - Home Dashboard profesional, moderno y extensible con:
 *      * Escáner Wi-Fi en tiempo real (SSID, RSSI, canal, seguridad, BSSID).
 *      * Escáner Bluetooth Low Energy (BLE) con cálculo de distancia estimada y
 * detalles completos.
 *      * Sistema modular listo para integrar sensores, relés y telemetría de
 * futuros proyectos.
 *      * Diagnóstico de hardware y restablecimiento de red por software o
 * mediante botón físico BOOT (3s).
 *
 * NOTA DE COMPILACIÓN (Arduino IDE):
 * En Herramientas -> Partition Scheme, seleccionar:
 * "Huge APP (3MB No OTA / 1MB SPIFFS)" o "Minimal SPIFFS (1.9MB APP with
 * OTA/190KB SPIFFS)" para que el stack combinado Wi-Fi + BLE quepa holgadamente
 * en la memoria Flash de 4MB.
 * ============================================================================
 */

#include "BleManager.h"
#include "Config.h"
#include "StorageManager.h"
#include "WebPortal.h"
#include <Arduino.h>
#include <WiFi.h>

// Variables para control del botón físico BOOT (GPIO 0)
unsigned long buttonPressStartTime = 0;
bool buttonActive = false;

// Variables para animación del LED de estado (GPIO 2)
unsigned long lastLedToggleTime = 0;
bool ledState = false;

/**
 * @brief Genera el nombre único del Punto de Acceso utilizando los últimos 4
 * caracteres de la MAC
 */
String getUniqueApSSID() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String suffix = mac.substring(mac.length() - 4);
  suffix.toUpperCase();
  return String(AP_SSID_PREFIX) + suffix;
}

/**
 * @brief Inicia el modo de configuración por Punto de Acceso (AP)
 */
void enterSetupMode() {
  Serial.println("\n[Modo] Iniciando MODO DE CONFIGURACIÓN (Punto de Acceso)");
  String apSSID = getUniqueApSSID();
  WebPortal::startApMode(apSSID);
}

/**
 * @brief Intenta conectar al router utilizando las credenciales almacenadas
 */
bool connectToSavedWifi() {
  String ssid = StorageManager::getSSID();
  String pass = StorageManager::getPassword();

  if (ssid.length() == 0)
    return false;

  Serial.println("\n[Wi-Fi] Conectando a la red almacenada: " + ssid);

  // Limpiar estados previos y desactivar guardado automático en NVS
  WiFi.persistent(false);
  WiFi.disconnect(true);
  delay(100);

  // Configurar modo Estación
  WiFi.mode(WIFI_STA);
  delay(50);

  // Suavizar picos de consumo de radio para evitar caídas de voltaje (Brownout)
  WiFi.setSleep(false);
  WiFi.setTxPower(
      WIFI_POWER_15dBm); // 15 dBm reduce el consumo pico de 500mA a ~220mA

  Serial.println("[Wi-Fi] Iniciando enlace de radio con el router...");
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("[Wi-Fi] Esperando respuesta del router...");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED &&
         (millis() - startTime < (WIFI_CONNECT_TIMEOUT_SEC * 1000))) {
    delay(500);
    Serial.print(".");
    digitalWrite(
        PIN_LED_STATUS,
        !digitalRead(PIN_LED_STATUS)); // Parpadeo rápido durante la conexión
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(PIN_LED_STATUS, HIGH); // LED fijo = Conexión exitosa
    Serial.println("[Wi-Fi] ¡Conexión establecida!");
    Serial.printf("[Wi-Fi] Dirección IP local: %s\n",
                  WiFi.localIP().toString().c_str());
    Serial.printf("[Wi-Fi] Intensidad RSSI: %d dBm\n", WiFi.RSSI());
    return true;
  } else {
    Serial.println("[Wi-Fi] No se pudo conectar a la red guardada (tiempo de "
                   "espera agotado).");
    return false;
  }
}

/**
 * @brief Configuración inicial del microcontrolador
 */
void setup() {
  // Inicialización de la consola serie
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n========================================================");
  Serial.printf("    %s v%s\n", FIRMWARE_NAME, FIRMWARE_VERSION);
  Serial.println("    ESP32 DevKit V1 - Arquitectura Estándar");
  Serial.println("========================================================");

  // Diagnóstico automático de la causa del reinicio anterior
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.printf("[Diagnóstico] Causa del último reinicio: %d ", reason);
  switch (reason) {
  case ESP_RST_POWERON:
    Serial.println("(Encendido por primera vez)");
    break;
  case ESP_RST_SW:
    Serial.println("(Reinicio por software)");
    break;
  case ESP_RST_PANIC:
    Serial.println("(¡Excepción de software / Panic!)");
    break;
  case ESP_RST_INT_WDT:
    Serial.println("(Watchdog de interrupción)");
    break;
  case ESP_RST_TASK_WDT:
    Serial.println("(Watchdog de tarea)");
    break;
  case ESP_RST_BROWNOUT:
    Serial.println("(¡BROWNOUT! Caída de voltaje en alimentación USB)");
    break;
  default:
    Serial.printf("(Código %d)\n", reason);
    break;
  }
  Serial.println();

  // Configuración de pines de Hardware
  Serial.println("[Init] 1/3 Configurando pines de Hardware...");
  pinMode(PIN_LED_STATUS, OUTPUT);
  pinMode(PIN_BUTTON_BOOT, INPUT_PULLUP);
  digitalWrite(PIN_LED_STATUS, LOW);

  // El stack BLE se inicializa bajo demanda (Lazy Loading) al escanear,
  // evitando el consumo masivo de RAM (~80KB) y pánico de RF al arrancar.
  Serial.println(
      "[Init] 2/3 Stack BLE preparado en modo bajo demanda (Lazy Loading).");

  // Comprobar si existen credenciales Wi-Fi guardadas en memoria NVS
  Serial.println("[Init] 3/3 Consultando almacenamiento NVS...");
  if (StorageManager::hasSavedCredentials()) {
    Serial.println("[Init] Se detectaron credenciales guardadas en NVS.");
    if (connectToSavedWifi()) {
      // Iniciar servidor web del Dashboard principal
      WebPortal::startStationMode();
      Serial.printf(
          "[Init] Sistema operativo listo. Accede desde: http://%s.local\n",
          MDNS_HOSTNAME);
    } else {
      // Si la conexión falla (router apagado o clave cambiada), abrir modo
      // configuración
      Serial.println("[Init] Conexión fallida. Activando modo Punto de Acceso "
                     "de respaldo.");
      enterSetupMode();
    }
  } else {
    // Primer encendido de fábrica: iniciar punto de acceso de configuración
    Serial.println("[Init] Sin configuración Wi-Fi previa. Iniciando modo de "
                   "configuración inicial.");
    enterSetupMode();
  }
}

/**
 * @brief Bucle principal de ejecución continua
 */
void loop() {
  // 1. Atención de peticiones Web, DNS y API REST
  WebPortal::handleClient();

  // 2. Control visual del LED según el modo de operación
  if (WebPortal::getIsApMode()) {
    // En modo AP de configuración: Parpadeo suave (500ms ON / 500ms OFF)
    if (millis() - lastLedToggleTime >= 500) {
      lastLedToggleTime = millis();
      ledState = !ledState;
      digitalWrite(PIN_LED_STATUS, ledState);
    }
  } else {
    // En modo STA conectado: LED encendido fijo
    digitalWrite(PIN_LED_STATUS, HIGH);
  }

  // 3. Monitorización del Botón Físico BOOT (GPIO 0) para Restablecimiento de
  // Fábrica Si se mantiene presionado durante 3 segundos, borra las
  // credenciales y reinicia en modo AP.
  if (digitalRead(PIN_BUTTON_BOOT) == LOW) {
    if (!buttonActive) {
      buttonActive = true;
      buttonPressStartTime = millis();
      Serial.println("[Botón] Botón BOOT presionado. Mantén presionado 3 "
                     "segundos para resetear Wi-Fi...");
    } else {
      if (millis() - buttonPressStartTime >= BUTTON_RESET_HOLD_MS) {
        Serial.println("[Botón] ¡Restablecimiento confirmado! Borrando "
                       "credenciales Wi-Fi...");

        // Señalización lumínica de confirmación con parpadeo ultrarrápido
        for (int i = 0; i < 10; i++) {
          digitalWrite(PIN_LED_STATUS, !digitalRead(PIN_LED_STATUS));
          delay(80);
        }

        StorageManager::clearCredentials();
        Serial.println("[Botón] Reiniciando ESP32 en modo configuración...");
        ESP.restart();
      }
    }
  } else {
    if (buttonActive) {
      buttonActive = false;
    }
  }
}
