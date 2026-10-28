# ESP32 Universal Core & Web Dashboard

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32%20DevKit%20V1-red.svg)](https://www.espressif.com/)
[![Arduino: ESP32 Core](https://img.shields.io/badge/Arduino_Core-3.x-teal.svg)](https://github.com/espressif/arduino-esp32)
[![SPDX: MIT](https://img.shields.io/badge/SPDX-MIT-green.svg)](https://spdx.org/licenses/MIT.html)

Plantilla de desarrollo profesional y desacoplada para microcontroladores **ESP32**. Implementa una arquitectura **Embedded REST Single-Page Application (SPA)** servida 100% offline desde la memoria Flash (`PROGMEM`), con soporte para escaneo de redes Wi-Fi, balizas Bluetooth (BLE), escáner de dispositivos en la red local (LAN) y un sistema de tarjetas modulares escalable.

---

## 🌟 Características Principales

- **SPA 100% Offline (Zero Dependencies):** Interfaz moderna y fluida construida en HTML5, CSS3 nativo y JavaScript Vanilla asíncrono. No requiere conexión a Internet ni bibliotecas externas (sin Tailwind ni Bootstrap).
- **Portal Cautivo de Configuración (Modo AP):** Primer inicio automático con red abierta `ESP32-Config-XXXX` y servidor DNS cautivo (`192.168.4.1`) para escanear y conectar al router Wi-Fi.
- **Panel de Control Dashboard (Modo STA):** Acceso mediante mDNS a [`http://esp32.local`](http://esp32.local) con barra de estado compacta (44px) que monitorea Wi-Fi, RSSI, memoria RAM libre y tiempo de actividad.
- **Escáner de Red Local (LAN Scanner):** Barrido de subred en segundo plano mediante FreeRTOS en Core 0. Sondeo híbrido (ARP en Capa 2 + sonda UDP en Capa 4) con resolución de fabricantes por OUI MAC embebida.
- **Escáner Bluetooth Low Energy (BLE):** Detección de dispositivos periféricos cercanos con cálculo de proximidad por RSSI.
- **Arquitectura de Launcher de Aplicaciones:** Menú de inicio en cuadrícula (`apps-grid`) con vistas independientes y botón de retorno superior (`navigateTo`), evitando el colapso horizontal de pestañas al agregar nuevas funciones.
- **Módulo Metodológico de Prueba (`TestModule`):** Implementación de referencia con comandos integrados (Ping, pulsos de LED, eco de texto y diagnóstico de heap).
- **Persistencia en Memoria Flash (NVS):** Almacenamiento seguro de credenciales con la biblioteca `Preferences`. Restablecimiento de fábrica manteniendo presionado el botón `BOOT` por 5 segundos.

---

## 📐 Estructura del Repositorio

```text
├── .gitignore               # Exclusión de archivos de entorno local y binarios temporales
├── LICENSE                  # Licencia MIT oficial
├── METODOLOGIA_DESARROLLO.md# Documentación técnica, reglas de arquitectura y guía paso a paso
├── README.md                # Este documento de presentación
└── esp32/
    ├── BleManager.h         # Stack Bluetooth Low Energy y cálculo de proximidad
    ├── Config.h             # Constantes del sistema, pines GPIO y timeouts
    ├── LanScannerManager.h  # Escáner de red local (ARP / LwIP / Vendor OUI) en Core 0
    ├── StorageManager.h     # Persistencia NVS en memoria Flash (Preferences)
    ├── TestModule.h         # Módulo de referencia y plantilla metodológica
    ├── UI_Dashboard.h       # Frontend HTML/CSS/JS del Panel de Control (PROGMEM)
    ├── UI_Setup.h           # Frontend HTML/CSS/JS del Portal Cautivo inicial (PROGMEM)
    ├── WebPortal.h          # Servidor HTTP, DNS cautivo y enrutamiento REST
    └── esp32.ino            # Setup, bucle principal, gestión de LED y botón BOOT
```

---

## 🛠️ Requisitos de Hardware

- **Placa:** ESP32 DevKit V1 (30 o 38 pines, chip ESP32-WROOM-32).
- **LED Onboard:** GPIO 2 (utilizado para estado de conexión y pulsos de prueba).
- **Botón Físico:** GPIO 0 / Botón `BOOT` (para reset de fábrica).

---

## ⚙️ Configuración en Arduino IDE

1. Instalar el paquete de placas **ESP32 by Espressif Systems** (versión 3.x recomendada).
2. Seleccionar la placa: `Tools -> Board -> esp32 -> ESP32 Dev Module`.
3. **CONFIGURACIÓN OBLIGATORIA DE MEMORIA:**
   ```text
   Tools -> Partition Scheme -> "Huge APP (3MB No OTA / 1MB SPIFFS)"
   ```
   > ⚠️ **Importante:** Debido a la coexistencia de los stacks Wi-Fi, Servidor Web y Bluetooth LE, la partición predeterminada de 1.2MB no es suficiente. Es obligatorio seleccionar **Huge APP**.

4. Conectar la placa por USB, seleccionar el puerto COM correspondiente y presionar **Subir (Upload)**.

---

## 🚀 Primer Inicio y Uso

1. Al encender el ESP32 por primera vez (o tras un reset), el LED onboard parpadeará rápidamente indicando que está en **Modo Configuración (AP)**.
2. Desde un smartphone o PC, conectarse a la red Wi-Fi: `ESP32-Config-XXXX`.
3. Se abrirá automáticamente el portal cautivo (o ingresar a `http://192.168.4.1`).
4. Seleccionar la red Wi-Fi hogareña, ingresar la contraseña y hacer clic en **"Probar Conexión"**.
5. Al validar, presionar **"Guardar y Conectar"**.
6. El ESP32 se conectará a la red y el panel estará disponible en:
   ```text
   http://esp32.local  (o a través de su dirección IP asignada)
   ```

---

## 🔌 Contrato de la API REST

| Endpoint | Método | Descripción |
|---|---|---|
| `/api/status` | `GET` | Telemetría general (Wi-Fi, RSSI, RAM libre, Uptime) |
| `/api/ble/scan` | `POST` | Inicia un barrido Bluetooth Low Energy |
| `/api/lan/scan` | `POST` | Inicia el escaneo de dispositivos en la subred local |
| `/api/lan/status` | `GET` | Consulta el porcentaje de avance del escaneo LAN |
| `/api/lan/devices` | `GET` | Retorna el listado JSON de dispositivos descubiertos en la LAN |
| `/api/test/run` | `POST` | Ejecuta un comando en el módulo de prueba (`TestFunction`) |
| `/api/reset` | `POST` | Restablecimiento de fábrica (borra NVS y reinicia en AP) |

---

## 📚 Metodología para Desarrolladores

Para comprender cómo agregar nuevas funciones, crear endpoints REST y enlazar nuevas tarjetas y vistas en la interfaz web, consulta la [Guía Técnica de Metodología de Desarrollo](METODOLOGIA_DESARROLLO.md).

---

## 📄 Licencia

Este proyecto está licenciado bajo los términos de la **Licencia MIT** y cumple con los estándares de la **Linux Foundation (SPDX-License-Identifier: MIT)**.

Consulta el archivo [LICENSE](LICENSE) para más detalles.

Copyright (c) 2026 **ChespiDev**.
