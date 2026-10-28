// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include <Arduino.h>
#include "Config.h"

/**
 * ============================================================================
 * @file TestModule.h
 * @brief Módulo de Ejemplo Metodológico (Plantilla para Nuevas Funciones)
 * 
 * Este módulo sirve como referencia viva y ejecutable para que cualquier
 * desarrollador o agente de Inteligencia Artificial agregue nuevas capacidades
 * al firmware del ESP32 de forma estandarizada y segura.
 * ============================================================================
 */
class TestModule {
private:
  static unsigned long executionCount;
  static unsigned long lastExecutionMillis;
  static String lastResult;

public:
  /**
   * @brief Inicialización de recursos del módulo (se invoca al iniciar en modo STA)
   */
  static void init() {
    executionCount = 0;
    lastExecutionMillis = 0;
    lastResult = "Módulo listo. No se han ejecutado pruebas.";
    Serial.println("[TestModule] Módulo metodológico Test inicializado correctamente.");
  }

  /**
   * @brief Función principal de prueba metodológica requerida por la arquitectura
   * @param inputParam Cadena de texto enviada desde la interfaz web (payload)
   * @param commandCode Código de acción (1: Ping, 2: Pulso LED GPIO 2, 3: Eco, 4: Diagnóstico Heap)
   * @return String JSON formateado con los resultados para la interfaz web
   */
  static String TestFunction(const String& inputParam, int commandCode) {
    unsigned long startMicros = micros();
    executionCount++;
    lastExecutionMillis = millis();

    String status = "success";
    String detail = "";
    int ledBlinks = 0;

    switch (commandCode) {
      case 1: { // Test 1: Verificación de Latencia y Ping Interno
        detail = "ESP32 respondiendo activamente al ping interno.";
        break;
      }
      case 2: { // Test 2: Control Físico de Hardware (LED de Estado GPIO 2)
        // Destello visual de 3 pulsos rápidos para validación física
        for (int i = 0; i < 3; i++) {
          digitalWrite(PIN_LED_STATUS, LOW);
          delay(50);
          digitalWrite(PIN_LED_STATUS, HIGH);
          delay(50);
        }
        ledBlinks = 3;
        detail = "Hardware verificado: 3 pulsos generados en LED onboard (GPIO " + String(PIN_LED_STATUS) + ").";
        break;
      }
      case 3: { // Test 3: Eco y Transformación de Cadenas
        String reversed = "";
        for (int i = inputParam.length() - 1; i >= 0; i--) {
          reversed += inputParam[i];
        }
        detail = "Dato procesado. Eco invertido: '" + reversed + "' (Longitud: " + String(inputParam.length()) + " bytes).";
        break;
      }
      case 4: { // Test 4: Chequeo Profundo de Memoria del Sistema
        uint32_t freeHeap = ESP.getFreeHeap();
        uint32_t minFreeHeap = ESP.getMinFreeHeap();
        uint32_t maxAlloc = ESP.getMaxAllocHeap();
        detail = "Heap libre: " + String(freeHeap / 1024) + " KB | Mínimo histórico: " + String(minFreeHeap / 1024) + " KB | Bloque contiguo mayor: " + String(maxAlloc / 1024) + " KB";
        break;
      }
      default: {
        detail = "Comando personalizado (" + String(commandCode) + ") ejecutado con parámetro: '" + inputParam + "'.";
        break;
      }
    }

    unsigned long elapsedMicros = micros() - startMicros;
    lastResult = detail;

    // Construcción del JSON de respuesta
    String json = "{";
    json += "\"status\":\"" + status + "\",";
    json += "\"command\":" + String(commandCode) + ",";
    json += "\"executionCount\":" + String(executionCount) + ",";
    json += "\"elapsedMicros\":" + String(elapsedMicros) + ",";
    json += "\"ledBlinks\":" + String(ledBlinks) + ",";
    json += "\"inputParam\":\"" + inputParam + "\",";
    json += "\"detail\":\"" + detail + "\",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"timestamp\":" + String(millis());
    json += "}";

    Serial.printf("[TestModule] TestFunction ejecutada. Cmd: %d, Parámetro: '%s', Duración: %lu us\n",
                  commandCode, inputParam.c_str(), elapsedMicros);
    return json;
  }

  /**
   * @brief Devuelve el estado acumulado del módulo
   */
  static String getStatusJson() {
    String json = "{";
    json += "\"executionCount\":" + String(executionCount) + ",";
    json += "\"lastExecutionMillis\":" + String(lastExecutionMillis) + ",";
    json += "\"lastResult\":\"" + lastResult + "\"";
    json += "}";
    return json;
  }
};

// Inicialización de variables estáticas
inline unsigned long TestModule::executionCount = 0;
inline unsigned long TestModule::lastExecutionMillis = 0;
inline String TestModule::lastResult = "Módulo listo. No se han ejecutado pruebas.";

#endif // TEST_MODULE_H
