# ESP32 Universal Core - Metodología de Desarrollo y Arquitectura Técnica

> **Audiencia:** Desarrolladores de firmware/software embebido, desarrolladores Frontend y **Agentes de Inteligencia Artificial** dedicados a la generación o mantenimiento de código para este proyecto.

---

## 1. Visión General y Patrón Arquitectónico

Este proyecto implementa una arquitectura **Embedded REST Single-Page Application (SPA)** desacoplada para microcontroladores ESP32 (DevKit V1). 

El sistema elimina la recarga de páginas web completas y la dependencia de bibliotecas externas o CDNs, garantizando **operación 100% offline** directamente servida desde la memoria Flash (`PROGMEM`) del ESP32.

```
┌────────────────────────────────────────────────────────────────────────┐
│                        FRONTEND (Navegador Cliente)                    │
│   SPA en PROGMEM · Menú Launcher de Funciones · Vistas Independientes  │
│   Barra de Estado Compacta (44px) · JS Fetch Asíncrono (JSON)          │
└────────────────────────────────────▲───────────────────────────────────┘
                                     │ HTTP REST (JSON / APIs)
┌────────────────────────────────────▼───────────────────────────────────┐
│                      CAPA DE ENRUTAMIENTO HTTP                         │
│   WebPortal.h (WebServer en Puerto 80, Respondedor mDNS: esp32.local)   │
└────────────────────────────────────▲───────────────────────────────────┘
                                     │ Llamadas Estáticas C++
┌────────────────────────────────────▼───────────────────────────────────┐
│                     MÓDULOS DE SERVICIO (MANAGERS)                     │
│   StorageManager   ·   BleManager   ·   TestModule   ·   [Nuevos]      │
│   (Persistencia NVS)   (Stack BLE)     (Plantilla)     (Sensores/etc.) │
└────────────────────────────────────▲───────────────────────────────────┘
                                     │ HAL / APIs Espressif
┌────────────────────────────────────▼───────────────────────────────────┐
│                     HARDWARE ESP32 & NÚCLEO ARDUINO                    │
│   GPIOs (LED 2 / Boot 0) · Radios Wi-Fi & BLE · Flash NVS · FreeRTOS   │
└────────────────────────────────────────────────────────────────────────┘
```

### Máquina de Estados Operativa
El firmware conmuta entre dos modos excluyentes:
1. **Modo AP (Configuración / Portal Cautivo):** Si no hay credenciales guardadas en NVS o tras restablecimiento de fábrica, crea una red abierta `ESP32-Config-XXXX`, levanta un servidor DNS cautivo (`192.168.4.1`) y sirve [UI_Setup.h](esp32/UI_Setup.h).
2. **Modo Estación STA (Dashboard Operativo):** Al conectarse al router, activa mDNS (`http://esp32.local`) y sirve el panel de control principal [UI_Dashboard.h](esp32/UI_Dashboard.h).

---

## 2. Estructura del Proyecto

```text
ESP32 - interfaz/
├── esp32/
│   ├── esp32.ino              # Punto de entrada setup() y loop(), gestión de LED y botón BOOT
│   ├── Config.h               # Constantes globales, pines GPIO y tiempos de timeout
│   ├── StorageManager.h       # Persistencia en memoria NVS Flash con biblioteca Preferences
│   ├── BleManager.h           # Stack Bluetooth Low Energy y cálculo de proximidad
│   ├── LanScannerManager.h    # Escáner de red local (ARP / LwIP / Vendor OUI) en Core 0
│   ├── BluetoothFakeJammer.h  # Simulador de interferencia RF BLE (Demo didáctica)
│   ├── TestModule.h           # MÓDULO METODOLÓGICO DE REFERENCIA (TestFunction)
│   ├── WebPortal.h            # Servidor HTTP, DNS cautivo y despacho de rutas REST
│   ├── UI_Setup.h             # HTML/CSS/JS del Portal Cautivo de configuración inicial
│   └── UI_Dashboard.h         # HTML/CSS/JS del Panel de Control (SPA y vistas modulares)
├── .vscode/
│   └── c_cpp_properties.json  # Configuración de IntelliSense para ESP32 Core
├── compile_flags.txt          # Flags para el analizador Clangd
└── METODOLOGIA_DESARROLLO.md  # Esta especificación técnica
```

---

## 3. Las 6 Reglas Fundamentales de Arquitectura y Escalabilidad

Para que el proyecto crezca indefinidamente sin corromper la memoria del microcontrolador ni romper la interfaz, se deben respetar estrictamente estas seis reglas:

### Regla 1: Encapsulamiento Estricto de Módulos (C++ Managers)
- Cada nueva funcionalidad debe residir en su propio archivo de cabecera: `esp32/<Nombre>Manager.h` o `esp32/<Nombre>Module.h`.
- Toda la clase debe implementar **métodos estáticos** e inicialización explícita (`static void init()`).
- No declarar objetos pesados en el stack dentro de `loop()`. Todo búfer dinámico debe liberarse inmediatamente tras generar la respuesta JSON.

### Regla 2: Contrato de Endpoints REST
- Todos los endpoints de servicio deben comenzar con `/api/<modulo>/<accion>`.
- Métodos HTTP semánticos:
  - `GET /api/<modulo>/status` o `/api/<modulo>/data`: Lectura de datos.
  - `POST /api/<modulo>/run` o `/api/<modulo>/set`: Ejecución de órdenes o escritura.
- Las respuestas siempre deben incluir:
  - `server.sendHeader("Access-Control-Allow-Origin", "*");`
  - Formato MIME `application/json`.
  - JSON estructurado con campo de estado (`{"status": "success", ...}`).

### Regla 3: No Bloqueo del Bucle Principal (`loop()`)
- El servidor web atiende en el hilo principal mediante `WebPortal::handleClient()`.
- **PROHIBIDO:** Usar `delay()` bloqueantes mayores a 50ms en controladores de ruta.
- Si una operación de hardware requiere tiempo (ej. lectura de sensor lento o escaneo), se debe realizar de forma asíncrona mediante temporización con `millis()` o mediante una tarea FreeRTOS (`xTaskCreatePinnedToCore`) en el Core 0.

### Regla 4: Optimización Estricta de Memoria (SRAM vs Flash)
- El ESP32 dispone de ~320 KB de memoria SRAM volátil.
- **PROHIBIDO:** Concatenar cadenas HTML gigantes en RAM con `String html += "..."`.
- Todo el código web (HTML, CSS y JS) debe almacenarse exclusivamente en memoria Flash mediante literales de texto crudo en `PROGMEM`:
  ```cpp
  const char PAGE_NOMBRE[] PROGMEM = R"rawliteral(
  ... código ...
  )rawliteral";
  ```
- **Particionamiento Flash:** Debido a la coexistencia de los stacks Wi-Fi + BLE + Servidor Web, en el compilador o Arduino IDE siempre se debe seleccionar:
  `Tools -> Partition Scheme -> Huge APP (3MB No OTA / 1MB SPIFFS)`

### Regla 5: Arquitectura de Interfaz SPA (Menú Launcher + Vistas)
- Para evitar que la interfaz se sobrecargue horizontalmente al agregar nuevas funciones, se utiliza el patrón **App Launcher / Catálogo de Funciones**.
- **Página de Inicio (`viewHome`):** Muestra una cuadrícula de tarjetas de funciones (`app-card`). Cada tarjeta cuenta con icono, título, badge de estado, descripción y enlace de apertura.
- **Vistas Específicas (`view<Nombre>`):** Cada función tiene su propio contenedor `<section id="view<Nombre>" class="view-section">` que ocupa el área de trabajo completa con su barra de retorno superior (`btn-back` &rarr; `navigateTo('home')`).
- **Barra de Estado Compacta (`status-bar`):** La telemetría general (Wi-Fi, RSSI, RAM y Uptime) se muestra en una barra horizontal compacta de solo 44px de altura para no quitar espacio a las aplicaciones.

### Regla 6: Concurrencia de Red y Seguridad Multihilo en LwIP (Core Locking)
- En ESP-IDF v5 y Arduino ESP32 Core 3.x, la pila de red LwIP tiene activada la protección multihilo obligatoria (`LWIP_TCPIP_CORE_LOCKING = 1`).
- La pila TCP/IP se ejecuta en su propia tarea de FreeRTOS (`tiT`).
- **PROHIBIDO:** Invocar directamente funciones de bajo nivel de capa 2/3 de LwIP (`etharp_request`, `etharp_find_addr`, manipulación de `netif`) desde tareas externas de FreeRTOS sin adquirir el cerrojo del núcleo TCP/IP.
- **Patrón Obligatorio:** Cualquier llamada a la API interna de LwIP debe estar protegida con las macros de sincronización de `<lwip/tcpip.h>`:
  ```cpp
  #include <lwip/tcpip.h>

  LOCK_TCPIP_CORE();
  // Llamada a función interna de LwIP (ej. etharp_request, etharp_find_addr)
  UNLOCK_TCPIP_CORE();
  ```
- **Consecuencia de omitirlo:** El ESP32 colapsa instantáneamente con un pánico de aserción del sistema: `assert failed: ethernet_output ... (Required to lock TCPIP core functionality!)` provocando un reinicio inmediato.

---

## 4. Guía Metodológica Paso a Paso: Cómo Agregar una Nueva Función

Sigue este procedimiento estandarizado de 4 fases para añadir cualquier función (ejemplo: `RelayManager` para control de actuadores o `SensorManager` para telemetría ambiental):

### Fase 1: Crear el Backend en C++ (`esp32/SensorManager.h`)
```cpp
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

class SensorManager {
private:
  static float lastTemperature;

public:
  static void init() {
    lastTemperature = 25.0f;
    Serial.println("[Sensor] SensorManager inicializado.");
  }

  static String readDataJson() {
    // Lectura de sensor real (ej. I2C / ADC)
    lastTemperature = 24.5f + (random(-10, 10) / 10.0f);

    String json = "{";
    json += "\"status\":\"success\",";
    json += "\"temperature\":" + String(lastTemperature, 1) + ",";
    json += "\"humidity\":55.0,";
    json += "\"timestamp\":" + String(millis());
    json += "}";
    return json;
  }
};

inline float SensorManager::lastTemperature = 0.0f;

#endif // SENSOR_MANAGER_H
```

### Fase 2: Registrar Rutas en `esp32/WebPortal.h`
1. Incluir el header al inicio del archivo:
   ```cpp
   #include "SensorManager.h"
   ```
2. Agregar el método controlador en la clase `WebPortal`:
   ```cpp
   static void handleApiSensorRead() {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     String json = SensorManager::readDataJson();
     server.send(200, "application/json", json);
   }
   ```
3. Registrar la ruta en `startStationMode()`:
   ```cpp
   SensorManager::init();
   server.on("/api/sensor/read", HTTP_GET, handleApiSensorRead);
   ```

### Fase 3: Agregar la Tarjeta en el Menú de Inicio (`esp32/UI_Dashboard.h`)
Dentro de `<div class="apps-grid">` en `viewHome`:
```html
<article class="app-card" onclick="navigateTo('sensor')">
  <div>
    <div class="app-top">
      <div class="app-icon" style="background: linear-gradient(135deg, #059669, #10b981); color: #fff;">
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
          <path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"></path>
        </svg>
      </div>
      <div class="app-meta">
        <h3>
          <span>Sensor Ambiental</span>
          <span class="app-badge badge-green">Telemetría</span>
        </h3>
        <p>Monitoreo continuo de temperatura, humedad y presión atmosférica en tiempo real.</p>
      </div>
    </div>
  </div>
  <div class="app-footer">
    <span class="app-status-tag"><span class="dot"></span> En servicio</span>
    <span class="app-action-link">Abrir Función &rarr;</span>
  </div>
</article>
```

### Fase 4: Crear la Vista Dedicada y Lógica JavaScript (`esp32/UI_Dashboard.h`)
1. Crear el contenedor de vista al mismo nivel que las demás vistas:
```html
<section id="viewSensor" class="view-section">
  <div class="view-header">
    <button class="btn-back" onclick="navigateTo('home')">
      <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
      Volver al Inicio
    </button>
    <div class="view-breadcrumb">
      <a onclick="navigateTo('home')">Inicio</a>
      <span>/</span>
      <span class="current">Sensor Ambiental</span>
    </div>
  </div>

  <div class="panel-card">
    <div class="panel-header">
      <div class="panel-title">
        <h2>Lectura en Tiempo Real</h2>
        <p>Datos adquiridos desde el sensor físico conectado al ESP32.</p>
      </div>
      <button class="btn-action" onclick="fetchSensorData()">Actualizar Lectura</button>
    </div>
    <div id="sensorDisplay" style="font-size:1.5rem; font-weight:700; color:#34d399;">-- °C</div>
  </div>
</section>
```
2. Agregar la función en el `<script>`:
```javascript
async function fetchSensorData() {
  try {
    const res = await fetch('/api/sensor/read');
    const data = await res.json();
    document.getElementById('sensorDisplay').innerText = data.temperature + ' °C';
    showToast('Lectura actualizada');
  } catch (e) {
    showToast('Error al leer sensor');
  }
}
```

---

## 5. Referencia del Módulo de Prueba: `TestFunction`

El proyecto incluye de forma predeterminada el módulo de referencia [TestModule.h](esp32/TestModule.h).

### Firma C++ de la Función
```cpp
static String TestModule::TestFunction(const String& inputParam, int commandCode);
```

### Tabla de Comandos Integrados
| Código (`commandCode`) | Acción Realizada en el ESP32 | Salida Generada |
|---|---|---|
| `1` | Verificación de Ping y Latencia | Retorna timestamp y tiempo de CPU en microsegundos |
| `2` | Destello físico de Hardware (LED GPIO 2) | Ejecuta 3 destellos de 50ms en el LED onboard |
| `3` | Eco y Transformación de Cadenas | Invierte la cadena `inputParam` y cuenta caracteres |
| `4` | Diagnóstico de Memoria SRAM | Reporta heap libre, mínimo histórico y bloque contiguo |

### Endpoint Asociado
- **Ruta:** `POST /api/test/run`
- **Payload Request (JSON):**
  ```json
  {
    "command": 2,
    "param": "Texto de prueba"
  }
  ```
- **Response (JSON):**
  ```json
  {
    "status": "success",
    "command": 2,
    "executionCount": 1,
    "elapsedMicros": 300412,
    "ledBlinks": 3,
    "inputParam": "Texto de prueba",
    "detail": "Hardware verificado: 3 pulsos generados en LED onboard (GPIO 2).",
    "freeHeap": 217480,
    "timestamp": 128450
  }
  ```

---

## 6. Módulo de Escáner de Red Local: `LanScannerManager`

El proyecto implementa un escáner de dispositivos conectados a la misma red local Wi-Fi ([LanScannerManager.h](esp32/LanScannerManager.h)).

### Arquitectura de Funcionamiento
1. **Ejecución Asíncrona en FreeRTOS:**
   - La tarea `scanTask` se crea dinámicamente con `xTaskCreatePinnedToCore` anclada al **Core 0** (stack de 4096 bytes, prioridad 1), dejando el Core 1 completamente libre para atender peticiones HTTP del servidor web.
2. **Técnica de Sondeo Híbrido de Capa 2 y Capa 4:**
   - **ARP Request (Capa 2):** Envía peticiones ARP a cada dirección IP del rango (`1` a `254`).
   - **Sonda UDP Ligera (Capa 4):** Envía un datagrama UDP de 1 byte al puerto 5353 (mDNS) hacia cada host. Esto "despierta" las interfaces de red de teléfonos (iOS / Android) y laptops que se encuentran en modo de ahorro de energía.
   - **Resolución en Caché ARP de LwIP:** Tras un retraso de 18ms (`pdMS_TO_TICKS(18)`), consulta la tabla ARP del kernel mediante `etharp_find_addr()`.
3. **Identificación de Fabricantes por MAC (OUI Lookup):**
   - Incorpora una base de datos embebida en memoria Flash con los prefijos OUI (Organizationally Unique Identifier) más comunes: Apple, Espressif, Samsung, Intel, Google, Xiaomi, Raspberry Pi, TP-Link, Huawei, etc.
4. **Manejo Obligatorio de Concurrencia LwIP:**
   - En conformidad con la **Regla 6**, toda interacción con `etharp_request()` y `etharp_find_addr()` se encuentra estrictamente delimitada con `LOCK_TCPIP_CORE()` y `UNLOCK_TCPIP_CORE()`.

### Contrato de Endpoints REST
- **Iniciar Escaneo:** `POST /api/lan/scan`
  - Respuesta: `{"status": "scanning", "message": "Barrido de red iniciado en segundo plano"}`
- **Progreso en Tiempo Real:** `GET /api/lan/status`
  - Respuesta: `{"scanning": true, "progress": 45, "total": 3, "lastScan": 12850}`
- **Listado de Dispositivos:** `GET /api/lan/devices`
  - Respuesta:
    ```json
    {
      "devices": [
        {
          "ip": "192.168.100.1",
          "mac": "E4:C3:2A:11:22:33",
          "vendor": "TP-Link Technologies",
          "type": "Router / Gateway",
          "ping": 1,
          "isGateway": true,
          "isSelf": false
        },
        {
          "ip": "192.168.100.21",
          "mac": "3C:61:05:AA:BB:CC",
          "vendor": "Espressif (Este ESP32)",
          "type": "Este Dispositivo (Tú)",
          "ping": 0,
          "isGateway": false,
          "isSelf": true
        },
        {
          "ip": "192.168.100.45",
          "mac": "BC:D1:19:44:55:66",
          "vendor": "Apple Inc.",
          "type": "Dispositivo de Red",
          "ping": 8,
          "isGateway": false,
          "isSelf": false
        }
      ],
      "total": 3
    }
    ```

---

## 7. [AI CONTRACT] Directivas para Agentes e Inteligencias Artificiales

Cuando un modelo de lenguaje o agente de código automatizado sea instruido para modificar o extender este repositorio, **DEBE** acatar las siguientes instrucciones:

1. **Inmutabilidad del Núcleo:** No modificar el flujo principal de [esp32.ino](esp32/esp32.ino) ni [StorageManager.h](esp32/StorageManager.h) a menos que se solicite explícitamente cambiar la persistencia de credenciales o el botón físico.
2. **Respeto a PROGMEM:** Toda adición visual debe realizarse dentro del literal crudo `PAGE_DASHBOARD` en [UI_Dashboard.h](esp32/UI_Dashboard.h) o en un archivo `.h` dedicado. No utilizar `server.sendContent()` fragmentado sin control de flujo.
3. **No Agregar Pestañas Horizontales:** Para cualquier función nueva, **no agregar botones horizontales en barras de pestañas**. Se debe agregar siempre una tarjeta en `apps-grid` y una vista `<section id="view..." class="view-section">` que use la función de enrutamiento `navigateTo('nombre')`.
4. **Respetar Partición Huge APP:** Recordar al usuario en cada modificación de compilación que debe mantener la partición de 3MB.
5. **Autocontenido Offline:** No agregar `<link>` o `<script>` con CDNs externas (como Tailwind, Bootstrap o Google Fonts remotos). Todo el CSS y JS debe ser Vanilla nativo, embebido y funcional sin conexión a Internet.
6. **Protección Multihilo LwIP:** Si se agregan funcionalidades que interactúan con protocolos de bajo nivel de red (ARP, ICMP, DHCP, DNS crudo o tablas LwIP), se DEBEN usar obligatoriamente `LOCK_TCPIP_CORE()` y `UNLOCK_TCPIP_CORE()` de `<lwip/tcpip.h>` para no violar `LWIP_TCPIP_CORE_LOCKING`.
