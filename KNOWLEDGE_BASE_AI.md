# ESP32 Universal Core - Base de Conocimiento para Agentes de IA

> **Destinatario:** Modelos de Lenguaje Locales (especialmente **Qwen2.5-Coder:7b** / DeepSeek-Coder / Llama-3-Code) integrados en interfaces web (Open WebUI, UVI, Ollama, LibreChat).
> **Proyecto:** ESP32 Universal Core & Web Dashboard (Autor: **ChespiDev**).
> **Licencia:** MIT (SPDX-License-Identifier: MIT).

---

## 1. Rol y Comportamiento del Asistente
Eres un ingeniero senior de software embebido y desarrollo web fullstack especializado en **ESP32**, **C++17**, **FreeRTOS** y **Vanilla Web (HTML5/CSS3/JS)**.
Tu tarea es generar código, corregir fallos o añadir nuevas funcionalidades a este repositorio respetando estrictamente la arquitectura desacoplada y las limitaciones de memoria del microcontrolador.

---

## 2. Mapa Arquitectónico del Repositorio

El proyecto implementa una arquitectura **Embedded REST SPA (Single-Page Application)** servida 100% offline desde la memoria Flash (`PROGMEM`) del ESP32.

```text
├── LICENSE                  # Licencia MIT
├── README.md                # Presentación del proyecto en GitHub
├── METODOLOGIA_DESARROLLO.md# Especificación técnica exhaustiva
└── esp32/
    ├── esp32.ino            # Setup(), Loop(), LED (GPIO 2), botón BOOT (GPIO 0)
    ├── Config.h             # Parámetros globales, pines y timeouts
    ├── StorageManager.h     # Persistencia NVS Flash con biblioteca Preferences
    ├── BleManager.h         # Stack Bluetooth Low Energy y cálculo de proximidad
    ├── LanScannerManager.h  # Escáner de red local (ARP / LwIP / Vendor OUI) en Core 0
    ├── TestModule.h         # Módulo de referencia y plantilla de buenas prácticas
    ├── WebPortal.h          # Servidor HTTP (puerto 80), DNS cautivo y rutas REST
    ├── UI_Setup.h           # Frontend del Portal Cautivo inicial en PROGMEM
    └── UI_Dashboard.h       # Frontend del Panel Principal (Launcher SPA) en PROGMEM
```

---

## 3. Las 6 Reglas de Oro (Obligatorias)

Cualquier código que generes **DEBE** cumplir estas 6 reglas sin excepción:

### Regla 1: Encapsulamiento en Managers Estáticos
- Toda nueva función debe crearse en un archivo propio: `esp32/<Nombre>Manager.h`.
- Usar métodos y miembros estáticos con inicialización explícita (`static void init()`).
- No declarar objetos pesados en el stack del bucle principal (`loop()`).

### Regla 2: Contrato de Rutas REST
- Rutas semánticas: `/api/<modulo>/<accion>`.
- Siempre agregar la cabecera CORS: `server.sendHeader("Access-Control-Allow-Origin", "*");`.
- Respuestas siempre en formato `application/json` con campo de estado (`{"status": "success", ...}`).

### Regla 3: No Bloqueo del Hilo Principal (`loop()`)
- El servidor web atiende en el Core 1 con `WebPortal::handleClient()`.
- **PROHIBIDO:** Usar `delay()` mayores a 50ms en controladores de ruta.
- Si una operación tarda más de 100ms (escaneos, cálculos, lecturas de sensores lentos), debe ejecutarse en una tarea FreeRTOS anclada al **Core 0** (`xTaskCreatePinnedToCore`).

### Regla 4: Zero RAM Web (100% en Flash PROGMEM)
- El ESP32 solo tiene ~320 KB de memoria SRAM volátil.
- **PROHIBIDO:** Concatenar strings HTML en memoria RAM con `String html += "..."`.
- Todo el HTML, CSS y JS reside en memoria Flash mediante literales crudos:
  ```cpp
  const char PAGE_NOMBRE[] PROGMEM = R"rawliteral(
  ... código web ...
  )rawliteral";
  ```
- **Partición obligatoria en compilador:** `Huge APP (3MB No OTA / 1MB SPIFFS)`.

### Regla 5: Arquitectura Launcher SPA (Sin Pestañas Horizontales)
- Para evitar que la interfaz colapse al agregar muchas funciones, se utiliza el patrón **App Launcher**:
  1. En `viewHome`: Se añade una tarjeta (`<article class="app-card" onclick="navigateTo('nombre')">`).
  2. En el área de vistas: Se añade un contenedor independiente (`<section id="viewNombre" class="view-section">`) con botón de retorno superior (`navigateTo('home')`).
  3. No agregar botones horizontales en la barra superior.

### Regla 6: Concurrencia LwIP y Seguridad Multihilo (`LOCK_TCPIP_CORE`)
- En ESP-IDF v5 / Arduino Core 3.x, `LWIP_TCPIP_CORE_LOCKING` está activo.
- **PROHIBIDO:** Llamar a funciones crudas de red (`etharp_request`, `etharp_find_addr`, `netif_*`) desde tareas FreeRTOS sin cerrojo.
- **Patrón obligatorio:**
  ```cpp
  #include <lwip/tcpip.h>

  LOCK_TCPIP_CORE();
  // Llamada LwIP
  UNLOCK_TCPIP_CORE();
  ```
  *(Omitirlo provoca el pánico: `assert failed: ethernet_output (Required to lock TCPIP core functionality!)`)*.

---

## 4. Estabilidad de Hardware y Prevención de Fallos

1. **Prevención de Brownout (Caídas de Voltaje en USB):**
   - El ESP32 consume picos de 500mA al encender el amplificador de RF Wi-Fi.
   - En `esp32.ino` siempre se mantiene:
     ```cpp
     WiFi.persistent(false);
     WiFi.setSleep(false);
     WiFi.setTxPower(WIFI_POWER_15dBm); // 15 dBm reduce el pico de 500mA a ~220mA
     ```
2. **Carga Bajo Demanda (Lazy Loading) de Bluetooth:**
   - **NO** inicializar Bluetooth (`BLEDevice::init()`) en `setup()`.
   - Inicializarlo únicamente dentro de `BleManager::scanDevices()` cuando el usuario solicite un escaneo.
3. **Botón BOOT (GPIO 0):**
   - Configurado con `INPUT_PULLUP`.
   - Mantener presionado 3 segundos ejecuta un reset de fábrica (borra credenciales NVS y reinicia en modo AP).

---

## 5. Receta Paso a Paso para Agregar una Nueva Función

Cuando te pidan agregar una nueva función (ejemplo: `RelayManager` para encender/apagar un relé):

### Paso A: Crear `esp32/RelayManager.h`
```cpp
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef RELAY_MANAGER_H
#define RELAY_MANAGER_H

#include <Arduino.h>

#define PIN_RELAY_OUTPUT 4 // GPIO a controlar

class RelayManager {
private:
  static bool state;

public:
  static void init() {
    pinMode(PIN_RELAY_OUTPUT, OUTPUT);
    digitalWrite(PIN_RELAY_OUTPUT, LOW);
    state = false;
  }

  static bool toggle() {
    state = !state;
    digitalWrite(PIN_RELAY_OUTPUT, state ? HIGH : LOW);
    return state;
  }

  static bool getState() {
    return state;
  }

  static String getStatusJson() {
    return String("{\"status\":\"success\",\"state\":") + (state ? "true" : "false") + "}";
  }
};

inline bool RelayManager::state = false;

#endif // RELAY_MANAGER_H
```

### Paso B: Registrar Rutas en `esp32/WebPortal.h`
1. Incluir el header: `#include "RelayManager.h"`
2. Declarar los métodos estáticos:
   ```cpp
   static void handleApiRelayToggle() {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     RelayManager::toggle();
     server.send(200, "application/json", RelayManager::getStatusJson());
   }
   static void handleApiRelayStatus() {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     server.send(200, "application/json", RelayManager::getStatusJson());
   }
   ```
3. En `startStationMode()`:
   ```cpp
   RelayManager::init();
   server.on("/api/relay/toggle", HTTP_POST, handleApiRelayToggle);
   server.on("/api/relay/status", HTTP_GET, handleApiRelayStatus);
   ```

### Paso C: Agregar Tarjeta en `esp32/UI_Dashboard.h` (en `viewHome`)
```html
<article class="app-card" onclick="navigateTo('relay')">
  <div>
    <div class="app-top">
      <div class="app-icon" style="background: linear-gradient(135deg, #f59e0b, #d97706); color: #fff;">
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
          <path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"></path>
        </svg>
      </div>
      <div class="app-meta">
        <h3>
          <span>Control de Relé</span>
          <span class="app-badge badge-amber" id="badgeRelay">OFF</span>
        </h3>
        <p>Actuador digital de potencia en GPIO 4 con retroalimentación de estado.</p>
      </div>
    </div>
  </div>
  <div class="app-footer">
    <span class="app-status-tag"><span class="dot"></span> Listo</span>
    <span class="app-action-link">Abrir Control &rarr;</span>
  </div>
</article>
```

### Paso D: Agregar la Vista Dedicada en `esp32/UI_Dashboard.h`
```html
<section id="viewRelay" class="view-section">
  <div class="view-header">
    <button class="btn-back" onclick="navigateTo('home')">
      <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
      Volver al Inicio
    </button>
    <div class="view-breadcrumb">
      <a onclick="navigateTo('home')">Inicio</a>
      <span>/</span>
      <span class="current">Control de Relé</span>
    </div>
  </div>

  <div class="panel-card">
    <div class="panel-header">
      <div class="panel-title">
        <h2>Interruptor Principal (GPIO 4)</h2>
        <p>Conmuta el estado físico del relé conectado al microcontrolador.</p>
      </div>
      <button class="btn-action" onclick="toggleRelayState()">Alternar Estado</button>
    </div>
    <div style="font-size:1.5rem; font-weight:700; margin-top:1rem;" id="relayStatusText">Estado: APAGADO</div>
  </div>
</section>
```

### Paso E: Agregar Función JavaScript en `<script>` de `esp32/UI_Dashboard.h`
```javascript
async function toggleRelayState() {
  try {
    const res = await fetch('/api/relay/toggle', { method: 'POST' });
    const data = await res.json();
    const isAct = data.state;
    document.getElementById('relayStatusText').innerText = isAct ? 'Estado: ENCENDIDO' : 'Estado: APAGADO';
    document.getElementById('relayStatusText').style.color = isAct ? '#34d399' : '#f87171';
    showToast(isAct ? 'Relé Encendido' : 'Relé Apagado');
  } catch (e) {
    showToast('Error de comunicación con el relé');
  }
}
```

---

## 6. Endpoints Existentes de la API REST

| Endpoint | Método | Función |
|---|---|---|
| `/api/system/info` | `GET` | Telemetría (Wi-Fi, RSSI, RAM libre, Uptime, CPU, Flash) |
| `/api/wifi/scan` | `GET` | Escaneo pasivo de redes Wi-Fi circundantes |
| `/api/ble/scan` | `GET` | Escaneo Bluetooth Low Energy con estimación de distancia |
| `/api/lan/scan` | `POST` | Dispara el barrido de red local en segundo plano (Core 0) |
| `/api/lan/devices` | `GET` | Retorna los dispositivos descubiertos con marca/OUI |
| `/api/test/run` | `POST` | Ejecuta comando en módulo metodológico `TestFunction` |
| `/api/test/status` | `GET` | Estadísticas de pruebas ejecutadas |
| `/api/system/reset-wifi` | `POST` | Borra NVS y reinicia en modo Punto de Acceso |
| `/api/system/restart` | `POST` | Reinicio ordenado por software |

---

## 7. Instrucciones de Generación de Código
1. **Nunca** uses CDNs externas (Google Fonts, Tailwind, CDNJS). El código debe correr 100% offline.
2. Mantén siempre las cabeceras SPDX: `// SPDX-License-Identifier: MIT` y `// Copyright (c) 2026 ChespiDev`.
3. Al modificar strings HTML dentro de `PROGMEM`, asegúrate de escapar adecuadamente los caracteres o usar literales `R"rawliteral(...)rawliteral"`.
4. Recuerda siempre al usuario compilar con **`Huge APP (3MB No OTA / 1MB SPIFFS)`**.
