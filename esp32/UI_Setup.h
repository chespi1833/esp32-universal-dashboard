// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef UI_SETUP_H
#define UI_SETUP_H

#include <Arduino.h>

/**
 * @brief Página Web de Configuración Inicial (Captive Portal)
 * Diseño profesional, responsive, modo oscuro con micro-interacciones y 100% offline.
 */
const char PAGE_SETUP[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Configuración de Red - ESP32</title>
  <style>
    :root {
      --bg: #090d16;
      --card-bg: rgba(17, 24, 39, 0.85);
      --card-border: rgba(255, 255, 255, 0.08);
      --primary: #3b82f6;
      --primary-hover: #2563eb;
      --accent: #06b6d4;
      --success: #10b981;
      --warning: #f59e0b;
      --danger: #ef4444;
      --text: #f9fafb;
      --text-muted: #9ca3af;
      --font: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
    }

    * { box-sizing: border-box; margin: 0; padding: 0; }

    body {
      background-color: var(--bg);
      background-image: 
        radial-gradient(at 0% 0%, rgba(59, 130, 246, 0.15) 0px, transparent 50%),
        radial-gradient(at 100% 100%, rgba(6, 182, 212, 0.12) 0px, transparent 50%);
      font-family: var(--font);
      color: var(--text);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 1.25rem;
    }

    .container {
      width: 100%;
      max-width: 480px;
      background: var(--card-bg);
      backdrop-filter: blur(16px);
      -webkit-backdrop-filter: blur(16px);
      border: 1px solid var(--card-border);
      border-radius: 20px;
      padding: 2rem;
      box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.5), 0 0 20px rgba(59, 130, 246, 0.1);
    }

    .header {
      text-align: center;
      margin-bottom: 1.75rem;
    }

    .chip-badge {
      display: inline-flex;
      align-items: center;
      gap: 0.5rem;
      background: rgba(59, 130, 246, 0.12);
      border: 1px solid rgba(59, 130, 246, 0.3);
      padding: 0.35rem 0.85rem;
      border-radius: 9999px;
      font-size: 0.75rem;
      font-weight: 600;
      color: #60a5fa;
      margin-bottom: 0.75rem;
      letter-spacing: 0.05em;
      text-transform: uppercase;
    }

    .chip-dot {
      width: 6px;
      height: 6px;
      background: #3b82f6;
      border-radius: 50%;
      box-shadow: 0 0 8px #3b82f6;
    }

    h1 {
      font-size: 1.5rem;
      font-weight: 700;
      color: var(--text);
      letter-spacing: -0.025em;
    }

    p.subtitle {
      color: var(--text-muted);
      font-size: 0.875rem;
      margin-top: 0.25rem;
    }

    .step-box { display: block; }
    .hidden { display: none !important; }

    .scan-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 0.75rem;
    }

    .scan-header span {
      font-size: 0.825rem;
      font-weight: 600;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    .btn-icon {
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid var(--card-border);
      color: var(--text);
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 0.4rem;
      padding: 0.35rem 0.75rem;
      border-radius: 8px;
      font-size: 0.8rem;
      font-weight: 500;
      transition: all 0.2s;
    }

    .btn-icon:hover {
      background: rgba(255, 255, 255, 0.1);
      border-color: rgba(255, 255, 255, 0.2);
    }

    .spinning {
      animation: spin 1s linear infinite;
    }

    @keyframes spin {
      100% { transform: rotate(360deg); }
    }

    .network-list {
      max-height: 220px;
      overflow-y: auto;
      border: 1px solid var(--card-border);
      border-radius: 12px;
      background: rgba(0, 0, 0, 0.25);
      margin-bottom: 1.25rem;
    }

    .network-item {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 0.75rem 1rem;
      border-bottom: 1px solid rgba(255, 255, 255, 0.04);
      cursor: pointer;
      transition: background 0.15s;
    }

    .network-item:last-child { border-bottom: none; }
    .network-item:hover { background: rgba(59, 130, 246, 0.12); }
    .network-item.selected {
      background: rgba(59, 130, 246, 0.2);
      border-left: 3px solid var(--primary);
    }

    .net-info {
      display: flex;
      align-items: center;
      gap: 0.65rem;
      min-width: 0;
    }

    .net-name {
      font-size: 0.9rem;
      font-weight: 500;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
      max-width: 220px;
    }

    .net-meta {
      display: flex;
      align-items: center;
      gap: 0.5rem;
    }

    .signal-pill {
      font-size: 0.75rem;
      font-weight: 600;
      padding: 0.15rem 0.45rem;
      border-radius: 6px;
    }

    .signal-high { background: rgba(16, 185, 129, 0.15); color: #34d399; }
    .signal-med { background: rgba(245, 158, 11, 0.15); color: #fbbf24; }
    .signal-low { background: rgba(239, 68, 68, 0.15); color: #f87171; }

    .form-group {
      margin-bottom: 1.1rem;
    }

    label {
      display: block;
      font-size: 0.8rem;
      font-weight: 600;
      color: var(--text-muted);
      margin-bottom: 0.4rem;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    .input-wrapper {
      position: relative;
      display: flex;
      align-items: center;
    }

    input[type="text"], input[type="password"] {
      width: 100%;
      background: rgba(0, 0, 0, 0.35);
      border: 1px solid var(--card-border);
      border-radius: 10px;
      padding: 0.75rem 1rem;
      font-size: 0.95rem;
      color: var(--text);
      outline: none;
      transition: border-color 0.2s, box-shadow 0.2s;
    }

    input:focus {
      border-color: var(--primary);
      box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.25);
    }

    .toggle-pass {
      position: absolute;
      right: 0.75rem;
      background: none;
      border: none;
      color: var(--text-muted);
      cursor: pointer;
      padding: 0.25rem;
      display: flex;
      align-items: center;
    }

    .toggle-pass:hover { color: var(--text); }

    .btn-primary {
      width: 100%;
      background: linear-gradient(135deg, var(--primary), var(--accent));
      color: #fff;
      border: none;
      padding: 0.85rem 1.25rem;
      border-radius: 10px;
      font-size: 0.95rem;
      font-weight: 600;
      cursor: pointer;
      box-shadow: 0 4px 15px rgba(59, 130, 246, 0.3);
      transition: opacity 0.2s, transform 0.1s;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 0.5rem;
    }

    .btn-primary:hover { opacity: 0.95; }
    .btn-primary:active { transform: translateY(1px); }

    .btn-finish {
      background: linear-gradient(135deg, #10b981, #059669);
      box-shadow: 0 4px 15px rgba(16, 185, 129, 0.3);
    }

    /* Pantalla de Estado / Éxito */
    .status-card {
      text-align: center;
      padding: 1.5rem 0.5rem;
    }

    .status-icon-box {
      width: 64px;
      height: 64px;
      border-radius: 50%;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 1.25rem;
    }

    .status-icon-box.success {
      background: rgba(16, 185, 129, 0.15);
      border: 2px solid var(--success);
      color: var(--success);
    }

    .status-icon-box.connecting {
      background: rgba(59, 130, 246, 0.15);
      border: 2px solid var(--primary);
      color: var(--primary);
    }

    .status-icon-box.error {
      background: rgba(239, 68, 68, 0.15);
      border: 2px solid var(--danger);
      color: var(--danger);
    }

    .info-box {
      background: rgba(0, 0, 0, 0.3);
      border: 1px solid var(--card-border);
      border-radius: 12px;
      padding: 1rem;
      margin: 1.25rem 0;
      text-align: left;
    }

    .info-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 0.35rem 0;
      font-size: 0.875rem;
    }

    .info-row span:first-child { color: var(--text-muted); }
    .info-row span:last-child { font-weight: 600; color: #93c5fd; }

    .instructions-box {
      background: rgba(59, 130, 246, 0.08);
      border: 1px solid rgba(59, 130, 246, 0.2);
      border-radius: 10px;
      padding: 1rem;
      margin-bottom: 1.5rem;
      text-align: left;
      font-size: 0.85rem;
      line-height: 1.5;
      color: #bfdbfe;
    }

    .instructions-box ol {
      margin-left: 1.2rem;
      margin-top: 0.4rem;
    }

    .instructions-box li {
      margin-bottom: 0.3rem;
    }

    .empty-state {
      padding: 2rem 1rem;
      text-align: center;
      color: var(--text-muted);
      font-size: 0.875rem;
    }
  </style>
</head>
<body>

  <div class="container">
    <div class="header">
      <div class="chip-badge">
        <span class="chip-dot"></span>
        ESP32 DevKit V1
      </div>
      <h1 id="mainTitle">Configuración Wi-Fi</h1>
      <p class="subtitle" id="mainSubtitle">Vincula tu ESP32 a tu red local para comenzar</p>
    </div>

    <!-- PASO 1: SELECCIÓN Y CONTRASEÑA -->
    <div id="stepScan" class="step-box">
      <div class="scan-header">
        <span>Redes Cercanas</span>
        <button class="btn-icon" id="btnRefresh" onclick="startWifiScan()">
          <svg id="refreshIcon" width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M21.5 2v6h-6M2.5 22v-6h6M2 11.5a10 10 0 0 1 18.8-4.3M22 12.5a10 10 0 0 1-18.8 4.2"/>
          </svg>
          Actualizar
        </button>
      </div>

      <div class="network-list" id="networkList">
        <div class="empty-state">Buscando redes Wi-Fi cercanas...</div>
      </div>

      <div class="form-group">
        <label for="ssidInput">Red Wi-Fi Seleccionada (SSID)</label>
        <div class="input-wrapper">
          <input type="text" id="ssidInput" placeholder="Nombre de red Wi-Fi">
        </div>
      </div>

      <div class="form-group">
        <label for="passInput">Contraseña</label>
        <div class="input-wrapper">
          <input type="password" id="passInput" placeholder="Ingresa la contraseña de tu red">
          <button type="button" class="toggle-pass" onclick="togglePasswordVisibility()">
            <svg id="eyeIcon" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path>
              <circle cx="12" cy="12" r="3"></circle>
            </svg>
          </button>
        </div>
      </div>

      <button class="btn-primary" id="btnConnect" onclick="attemptConnect()">
        <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
          <path d="M5 12.55a11 11 0 0 1 14.08 0"></path>
          <path d="M1.42 9a16 16 0 0 1 21.16 0"></path>
          <path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path>
          <line x1="12" y1="20" x2="12.01" y2="20"></line>
        </svg>
        Conectar a esta Red
      </button>
    </div>

    <!-- PASO 2: CONECTANDO -->
    <div id="stepConnecting" class="step-box hidden">
      <div class="status-card">
        <div class="status-icon-box connecting">
          <svg class="spinning" width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <path d="M21 12a9 9 0 1 1-6.219-8.56"></path>
          </svg>
        </div>
        <h2>Conectando al Router...</h2>
        <p class="subtitle" style="margin-top: 0.5rem;" id="connectingMsg">
          Verificando señal y credenciales con tu red Wi-Fi. Por favor espera.
        </p>
      </div>
    </div>

    <!-- PASO 3: ÉXITO -->
    <div id="stepSuccess" class="step-box hidden">
      <div class="status-card">
        <div class="status-icon-box success">
          <svg width="34" height="34" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3">
            <polyline points="20 6 9 17 4 12"></polyline>
          </svg>
        </div>
        <h2>¡Conexión Exitosa!</h2>
        <p class="subtitle">El ESP32 se ha vinculado correctamente a tu red.</p>

        <div class="info-box">
          <div class="info-row">
            <span>Red Conectada:</span>
            <span id="succSSID">-</span>
          </div>
          <div class="info-row">
            <span>Dirección IP Asignada:</span>
            <span id="succIP">-</span>
          </div>
          <div class="info-row">
            <span>Nombre Local mDNS:</span>
            <span>http://esp32.local</span>
          </div>
        </div>

        <div class="instructions-box">
          <strong>📋 Próximos pasos:</strong>
          <ol>
            <li>Haz clic en el botón verde inferior para apagar este punto de acceso provisional.</li>
            <li>Vuelve a conectar tu móvil o PC a tu red Wi-Fi habitual.</li>
            <li>Ingresa a <strong>http://esp32.local</strong> (o mediante la IP directa) para acceder al panel principal del ESP32.</li>
          </ol>
        </div>

        <button class="btn-primary btn-finish" id="btnFinish" onclick="finishAndDisconnect()">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M18.36 6.64a9 9 0 1 1-12.73 0"></path>
            <line x1="12" y1="2" x2="12" y2="12"></line>
          </svg>
          Terminar y Desconectar Punto de Acceso
        </button>
      </div>
    </div>

    <!-- PASO 4: DESCONECTADO FINAL -->
    <div id="stepDone" class="step-box hidden">
      <div class="status-card">
        <div class="status-icon-box success">
          <svg width="34" height="34" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"></path>
            <polyline points="22 4 12 14.01 9 11.01"></polyline>
          </svg>
        </div>
        <h2>Punto de Acceso Apagado</h2>
        <p class="subtitle" style="margin-top: 0.75rem; line-height: 1.6;">
          La red temporal del ESP32 ha sido deshabilitada de forma segura.<br>
          Ya puedes reconectarte a tu Wi-Fi e ingresar a:<br>
          <strong style="color: #60a5fa; font-size: 1.1rem; display: inline-block; margin-top: 0.5rem;">http://esp32.local</strong>
        </p>
      </div>
    </div>

  </div>

  <script>
    let pollInterval = null;

    function showStep(stepId) {
      document.getElementById('stepScan').classList.add('hidden');
      document.getElementById('stepConnecting').classList.add('hidden');
      document.getElementById('stepSuccess').classList.add('hidden');
      document.getElementById('stepDone').classList.add('hidden');
      document.getElementById(stepId).classList.remove('hidden');
    }

    function togglePasswordVisibility() {
      const pass = document.getElementById('passInput');
      pass.type = (pass.type === 'password') ? 'text' : 'password';
    }

    async function startWifiScan() {
      const icon = document.getElementById('refreshIcon');
      const list = document.getElementById('networkList');
      icon.classList.add('spinning');
      list.innerHTML = '<div class="empty-state">Escaneando el entorno de radiofrecuencia...</div>';

      try {
        const res = await fetch('/api/wifi/scan');
        const data = await res.json();
        icon.classList.remove('spinning');

        if (!data.networks || data.networks.length === 0) {
          list.innerHTML = '<div class="empty-state">No se detectaron redes. Reintenta el escaneo.</div>';
          return;
        }

        // Ordenar por intensidad de señal (mayor a menor)
        data.networks.sort((a, b) => b.rssi - a.rssi);

        let html = '';
        data.networks.forEach(net => {
          let signalClass = 'signal-high';
          if (net.rssi < -80) signalClass = 'signal-low';
          else if (net.rssi < -65) signalClass = 'signal-med';

          const lockIcon = net.isSecure 
            ? '<svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect><path d="M7 11V7a5 5 0 0 1 10 0v4"></path></svg>'
            : '<svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="#10b981" stroke-width="2"><path d="M7 11V7a5 5 0 0 1 9.9-1"></path><rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect></svg>';

          html += `
            <div class="network-item" onclick="selectNetwork('${encodeURIComponent(net.ssid)}')">
              <div class="net-info">
                <div class="net-name">${net.ssid || '[Red Oculta]'}</div>
              </div>
              <div class="net-meta">
                ${lockIcon}
                <span class="signal-pill ${signalClass}">${net.rssi} dBm</span>
              </div>
            </div>
          `;
        });
        list.innerHTML = html;
      } catch (e) {
        icon.classList.remove('spinning');
        list.innerHTML = '<div class="empty-state">Error al consultar redes. Pulsa Actualizar.</div>';
      }
    }

    function selectNetwork(encodedSsid) {
      const ssid = decodeURIComponent(encodedSsid);
      document.getElementById('ssidInput').value = ssid;
      document.getElementById('passInput').focus();
    }

    async function attemptConnect() {
      const ssid = document.getElementById('ssidInput').value.trim();
      const pass = document.getElementById('passInput').value;

      if (!ssid) {
        alert('Por favor selecciona o escribe el nombre de una red Wi-Fi.');
        return;
      }

      showStep('stepConnecting');
      document.getElementById('connectingMsg').innerText = `Estableciendo enlace con "${ssid}"...`;

      try {
        const res = await fetch('/api/wifi/connect', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ ssid: ssid, password: pass })
        });
        
        // Iniciar sondeo de estado
        startStatusPolling();
      } catch (e) {
        alert('Error al enviar la solicitud de conexión.');
        showStep('stepScan');
      }
    }

    function startStatusPolling() {
      if (pollInterval) clearInterval(pollInterval);
      let attempts = 0;

      pollInterval = setInterval(async () => {
        attempts++;
        try {
          const res = await fetch('/api/wifi/status');
          const data = await res.json();

          if (data.status === 'connected') {
            clearInterval(pollInterval);
            document.getElementById('succSSID').innerText = data.ssid || 'Wi-Fi';
            document.getElementById('succIP').innerText = data.ip || 'Asignada por DHCP';
            showStep('stepSuccess');
          } else if (data.status === 'failed') {
            clearInterval(pollInterval);
            alert(data.message || 'No se pudo conectar a la red. Verifica la contraseña.');
            showStep('stepScan');
          } else if (attempts > 25) {
            clearInterval(pollInterval);
            alert('Tiempo de espera agotado. Verifica la señal y contraseña.');
            showStep('stepScan');
          }
        } catch (e) {
          // El ESP32 puede tardar un momento durante la negociación de radio
        }
      }, 1500);
    }

    async function finishAndDisconnect() {
      const btn = document.getElementById('btnFinish');
      btn.disabled = true;
      btn.innerText = 'Apagando red provisional...';

      try {
        await fetch('/api/wifi/finish', { method: 'POST' });
      } catch (e) {
        // En muchos casos la petición corta al desconectarse el AP, es comportamiento esperado
      }
      showStep('stepDone');
    }

    // Escanear redes automáticamente al cargar la página
    window.addEventListener('DOMContentLoaded', () => {
      startWifiScan();
    });
  </script>
</body>
</html>
)rawliteral";

#endif // UI_SETUP_H
