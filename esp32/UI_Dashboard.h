// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ChespiDev

#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#include <Arduino.h>

/**
 * @brief Panel de Control Principal del ESP32 (Home Dashboard)
 * Servido en http://esp32.local tras la conexión exitosa.
 * Arquitectura modular de aplicaciones/vistas independientes (App Launcher),
 * barra de estado compacta de alto rendimiento y módulo de prueba metodológico.
 * 100% offline y compatible con PROGMEM.
 */
const char PAGE_DASHBOARD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 DevKit - Panel de Control</title>
  <style>
    :root {
      --bg: #070b13;
      --card-bg: rgba(15, 23, 42, 0.78);
      --card-border: rgba(255, 255, 255, 0.08);
      --primary: #3b82f6;
      --primary-hover: #2563eb;
      --accent-cyan: #06b6d4;
      --accent-purple: #8b5cf6;
      --accent-emerald: #10b981;
      --accent-amber: #f59e0b;
      --accent-rose: #f43f5e;
      --text: #f8fafc;
      --text-muted: #94a3b8;
      --font: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
    }

    * { box-sizing: border-box; margin: 0; padding: 0; }

    body {
      background-color: var(--bg);
      background-image: 
        radial-gradient(at 0% 0%, rgba(59, 130, 246, 0.12) 0px, transparent 40%),
        radial-gradient(at 100% 0%, rgba(6, 182, 212, 0.1) 0px, transparent 40%),
        radial-gradient(at 50% 100%, rgba(139, 92, 246, 0.08) 0px, transparent 50%);
      font-family: var(--font);
      color: var(--text);
      min-height: 100vh;
      padding: 1.25rem;
    }

    .container {
      max-width: 1120px;
      margin: 0 auto;
    }

    /* BARRA SUPERIOR NAVBAR */
    .navbar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      background: var(--card-bg);
      backdrop-filter: blur(14px);
      -webkit-backdrop-filter: blur(14px);
      border: 1px solid var(--card-border);
      border-radius: 14px;
      padding: 0.85rem 1.25rem;
      margin-bottom: 0.75rem;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
    }

    .nav-brand {
      display: flex;
      align-items: center;
      gap: 0.75rem;
      cursor: pointer;
    }

    .brand-icon {
      width: 36px;
      height: 36px;
      border-radius: 9px;
      background: linear-gradient(135deg, var(--primary), var(--accent-cyan));
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 0 14px rgba(59, 130, 246, 0.4);
    }

    .brand-text h1 {
      font-size: 1.05rem;
      font-weight: 700;
      letter-spacing: -0.02em;
    }

    .brand-text p {
      font-size: 0.72rem;
      color: var(--text-muted);
      letter-spacing: 0.03em;
    }

    .nav-actions {
      display: flex;
      align-items: center;
      gap: 0.75rem;
    }

    .btn-nav-home {
      background: rgba(255, 255, 255, 0.06);
      border: 1px solid var(--card-border);
      color: var(--text);
      padding: 0.4rem 0.85rem;
      border-radius: 8px;
      font-size: 0.8rem;
      font-weight: 600;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 0.4rem;
      transition: all 0.2s;
    }

    .btn-nav-home:hover {
      background: rgba(255, 255, 255, 0.12);
      border-color: rgba(255, 255, 255, 0.2);
    }

    .status-badge {
      display: inline-flex;
      align-items: center;
      gap: 0.45rem;
      background: rgba(16, 185, 129, 0.12);
      border: 1px solid rgba(16, 185, 129, 0.3);
      padding: 0.35rem 0.75rem;
      border-radius: 9999px;
      font-size: 0.75rem;
      font-weight: 600;
      color: #34d399;
    }

    .pulse-dot {
      width: 7px;
      height: 7px;
      background: #10b981;
      border-radius: 50%;
      box-shadow: 0 0 8px #10b981;
      animation: pulse 2s infinite;
    }

    @keyframes pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.4; transform: scale(0.85); }
    }

    /* 1. BARRA DE ESTADO COMPACTA (MÉTRICAS MINIMIZADAS) */
    .status-bar {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 0.6rem;
      margin-bottom: 1.25rem;
    }

    @media (max-width: 860px) {
      .status-bar {
        grid-template-columns: repeat(2, 1fr);
      }
    }

    @media (max-width: 480px) {
      .status-bar {
        grid-template-columns: 1fr;
      }
    }

    .status-chip {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 10px;
      padding: 0.55rem 0.85rem;
      display: flex;
      align-items: center;
      gap: 0.7rem;
      backdrop-filter: blur(10px);
      transition: all 0.2s;
    }

    .status-chip:hover {
      border-color: rgba(255, 255, 255, 0.16);
      background: rgba(30, 41, 59, 0.85);
    }

    .chip-icon {
      width: 28px;
      height: 28px;
      border-radius: 7px;
      display: flex;
      align-items: center;
      justify-content: center;
      flex-shrink: 0;
    }

    .chip-info {
      display: flex;
      flex-direction: column;
      min-width: 0;
      line-height: 1.2;
    }

    .chip-label {
      font-size: 0.65rem;
      text-transform: uppercase;
      font-weight: 700;
      color: var(--text-muted);
      letter-spacing: 0.04em;
    }

    .chip-val-row {
      display: flex;
      align-items: baseline;
      gap: 0.35rem;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }

    .chip-value {
      font-size: 0.88rem;
      font-weight: 700;
      color: #fff;
    }

    .chip-sub {
      font-size: 0.7rem;
      color: var(--accent-cyan);
    }

    /* BREADCRUMB / BARRA DE RETORNO EN VISTAS HIJAS */
    .view-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 1.25rem;
      padding: 0.5rem 0;
    }

    .btn-back {
      background: rgba(255, 255, 255, 0.06);
      border: 1px solid var(--card-border);
      color: var(--text);
      padding: 0.5rem 1rem;
      border-radius: 10px;
      font-size: 0.85rem;
      font-weight: 600;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 0.5rem;
      transition: all 0.2s;
    }

    .btn-back:hover {
      background: rgba(59, 130, 246, 0.2);
      border-color: rgba(59, 130, 246, 0.4);
      color: #93c5fd;
      transform: translateX(-2px);
    }

    .view-breadcrumb {
      font-size: 0.85rem;
      color: var(--text-muted);
      display: flex;
      align-items: center;
      gap: 0.45rem;
    }

    .view-breadcrumb a {
      color: var(--text-muted);
      text-decoration: none;
      cursor: pointer;
    }

    .view-breadcrumb a:hover {
      color: var(--text);
      text-decoration: underline;
    }

    .view-breadcrumb .current {
      color: #fff;
      font-weight: 600;
    }

    /* CONTENEDOR DE PÁGINAS / VISTAS */
    .view-section {
      display: none;
      animation: fadeIn 0.22s ease-in-out;
    }

    .view-section.active {
      display: block;
    }

    @keyframes fadeIn {
      from { opacity: 0; transform: translateY(4px); }
      to { opacity: 1; transform: translateY(0); }
    }

    /* 2. CATÁLOGO DE APLICACIONES / HUB DE INICIO */
    .hub-hero {
      margin-bottom: 1.5rem;
    }

    .hub-hero h2 {
      font-size: 1.45rem;
      font-weight: 800;
      letter-spacing: -0.02em;
    }

    .hub-hero p {
      font-size: 0.85rem;
      color: var(--text-muted);
      margin-top: 0.25rem;
    }

    .apps-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
      gap: 1.25rem;
    }

    .app-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 16px;
      padding: 1.35rem;
      backdrop-filter: blur(14px);
      box-shadow: 0 10px 25px rgba(0, 0, 0, 0.25);
      cursor: pointer;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      position: relative;
      overflow: hidden;
      transition: all 0.25s ease;
    }

    .app-card:hover {
      transform: translateY(-3px);
      border-color: rgba(59, 130, 246, 0.4);
      box-shadow: 0 15px 35px rgba(59, 130, 246, 0.15);
    }

    .app-top {
      display: flex;
      align-items: flex-start;
      gap: 1rem;
      margin-bottom: 1rem;
    }

    .app-icon {
      width: 48px;
      height: 48px;
      border-radius: 12px;
      display: flex;
      align-items: center;
      justify-content: center;
      flex-shrink: 0;
      box-shadow: 0 6px 16px rgba(0, 0, 0, 0.3);
    }

    .app-meta {
      flex: 1;
    }

    .app-meta h3 {
      font-size: 1.15rem;
      font-weight: 700;
      color: #fff;
      display: flex;
      align-items: center;
      justify-content: space-between;
    }

    .app-badge {
      font-size: 0.65rem;
      font-weight: 700;
      text-transform: uppercase;
      padding: 0.2rem 0.5rem;
      border-radius: 6px;
      letter-spacing: 0.04em;
    }

    .app-meta p {
      font-size: 0.825rem;
      color: var(--text-muted);
      line-height: 1.45;
      margin-top: 0.35rem;
    }

    .app-footer {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding-top: 0.85rem;
      border-top: 1px solid rgba(255, 255, 255, 0.05);
      margin-top: 0.5rem;
    }

    .app-status-tag {
      font-size: 0.72rem;
      color: #94a3b8;
      display: flex;
      align-items: center;
      gap: 0.35rem;
    }

    .app-status-tag .dot {
      width: 6px;
      height: 6px;
      border-radius: 50%;
      background: #10b981;
    }

    .app-action-link {
      font-size: 0.825rem;
      font-weight: 700;
      color: var(--primary);
      display: inline-flex;
      align-items: center;
      gap: 0.35rem;
      transition: gap 0.2s;
    }

    .app-card:hover .app-action-link {
      gap: 0.55rem;
      color: #60a5fa;
    }

    /* PANELES DE CONTENIDO DE LAS VISTAS */
    .panel-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 16px;
      padding: 1.5rem;
      backdrop-filter: blur(14px);
      box-shadow: 0 15px 35px rgba(0, 0, 0, 0.3);
      margin-bottom: 1.5rem;
    }

    .panel-header {
      display: flex;
      flex-wrap: wrap;
      justify-content: space-between;
      align-items: center;
      gap: 1rem;
      margin-bottom: 1.25rem;
    }

    .panel-title h2 {
      font-size: 1.25rem;
      font-weight: 700;
      display: flex;
      align-items: center;
      gap: 0.6rem;
    }

    .panel-title p {
      font-size: 0.825rem;
      color: var(--text-muted);
      margin-top: 0.2rem;
    }

    .panel-actions {
      display: flex;
      align-items: center;
      gap: 0.75rem;
    }

    .search-input {
      background: rgba(0, 0, 0, 0.35);
      border: 1px solid var(--card-border);
      border-radius: 10px;
      padding: 0.55rem 0.85rem;
      color: var(--text);
      font-size: 0.85rem;
      outline: none;
      min-width: 200px;
    }

    .search-input:focus {
      border-color: var(--primary);
    }

    .btn-action {
      background: linear-gradient(135deg, var(--primary), var(--accent-cyan));
      color: #fff;
      border: none;
      padding: 0.55rem 1.15rem;
      border-radius: 10px;
      font-size: 0.85rem;
      font-weight: 600;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 0.5rem;
      box-shadow: 0 4px 14px rgba(59, 130, 246, 0.3);
      transition: all 0.2s;
    }

    .btn-action:hover { opacity: 0.92; transform: translateY(-1px); }
    .btn-action:disabled { opacity: 0.6; cursor: not-allowed; }

    .btn-secondary {
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid var(--card-border);
      color: var(--text);
      box-shadow: none;
    }
    .btn-secondary:hover { background: rgba(255, 255, 255, 0.1); }

    .btn-danger {
      background: linear-gradient(135deg, #ef4444, #dc2626);
      color: #fff;
      box-shadow: 0 4px 14px rgba(239, 68, 68, 0.3);
    }

    /* TABLA WI-FI */
    .table-responsive {
      overflow-x: auto;
      border-radius: 10px;
      border: 1px solid var(--card-border);
    }

    table {
      width: 100%;
      border-collapse: collapse;
      font-size: 0.85rem;
      text-align: left;
    }

    thead {
      background: rgba(0, 0, 0, 0.35);
    }

    th {
      padding: 0.85rem 1rem;
      font-weight: 700;
      color: var(--text-muted);
      text-transform: uppercase;
      font-size: 0.725rem;
      letter-spacing: 0.04em;
      border-bottom: 1px solid var(--card-border);
    }

    td {
      padding: 0.85rem 1rem;
      border-bottom: 1px solid rgba(255, 255, 255, 0.04);
    }

    tbody tr:hover {
      background: rgba(255, 255, 255, 0.02);
    }

    .badge {
      display: inline-block;
      padding: 0.25rem 0.55rem;
      border-radius: 6px;
      font-size: 0.725rem;
      font-weight: 700;
    }

    .badge-green { background: rgba(16, 185, 129, 0.15); color: #34d399; }
    .badge-yellow { background: rgba(245, 158, 11, 0.15); color: #fbbf24; }
    .badge-red { background: rgba(239, 68, 68, 0.15); color: #f87171; }
    .badge-blue { background: rgba(59, 130, 246, 0.15); color: #60a5fa; }
    .badge-purple { background: rgba(139, 92, 246, 0.15); color: #c084fc; }

    /* BLE GRID */
    .ble-grid {
      display: flex;
      flex-direction: column;
      gap: 0.85rem;
    }

    .ble-card {
      background: rgba(0, 0, 0, 0.25);
      border: 1px solid var(--card-border);
      border-radius: 12px;
      padding: 1.15rem;
      transition: all 0.2s;
    }

    .ble-card:hover {
      border-color: rgba(59, 130, 246, 0.35);
      background: rgba(0, 0, 0, 0.35);
    }

    .ble-main-row {
      display: flex;
      flex-wrap: wrap;
      justify-content: space-between;
      align-items: center;
      gap: 1rem;
    }

    .ble-title-group {
      display: flex;
      align-items: center;
      gap: 0.85rem;
    }

    .ble-icon-circle {
      width: 40px;
      height: 40px;
      border-radius: 10px;
      background: rgba(59, 130, 246, 0.15);
      color: #60a5fa;
      display: flex;
      align-items: center;
      justify-content: center;
    }

    .ble-name-wrapper {
      display: flex;
      align-items: center;
      gap: 0.6rem;
    }

    .ble-dev-name {
      font-size: 1rem;
      font-weight: 700;
      color: #fff;
    }

    .ble-dev-mac {
      font-size: 0.775rem;
      color: var(--text-muted);
      font-family: monospace;
      margin-top: 0.15rem;
    }

    .btn-detail-toggle {
      background: rgba(255, 255, 255, 0.06);
      border: 1px solid var(--card-border);
      color: var(--text);
      padding: 0.25rem 0.6rem;
      border-radius: 6px;
      font-size: 0.725rem;
      font-weight: 600;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 0.35rem;
      transition: all 0.2s;
    }

    .btn-detail-toggle:hover {
      background: rgba(255, 255, 255, 0.12);
      color: #fff;
    }

    .ble-metrics-group {
      display: flex;
      align-items: center;
      gap: 0.6rem;
    }

    .distance-tag {
      font-size: 0.75rem;
      font-weight: 700;
      padding: 0.35rem 0.65rem;
      border-radius: 6px;
      display: inline-flex;
      align-items: center;
      gap: 0.3rem;
    }

    .ble-details-panel {
      margin-top: 1rem;
      padding-top: 1rem;
      border-top: 1px solid rgba(255, 255, 255, 0.06);
      display: none;
    }

    .ble-details-panel.open {
      display: block;
    }

    .details-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 0.65rem;
    }

    .detail-item {
      background: rgba(0, 0, 0, 0.3);
      padding: 0.6rem 0.85rem;
      border-radius: 8px;
      border: 1px solid rgba(255, 255, 255, 0.04);
      display: flex;
      flex-direction: column;
      gap: 0.2rem;
    }

    .detail-item span:first-child {
      font-size: 0.675rem;
      color: var(--text-muted);
      text-transform: uppercase;
      font-weight: 600;
    }

    .detail-item span:last-child {
      color: #e2e8f0;
      font-family: monospace;
      font-size: 0.825rem;
      word-break: break-all;
    }

    /* 3. VISTA TEST FUNCTION */
    .test-box {
      background: rgba(0, 0, 0, 0.3);
      border: 1px solid var(--card-border);
      border-radius: 12px;
      padding: 1.25rem;
      margin-bottom: 1.25rem;
    }

    .form-group {
      margin-bottom: 1rem;
    }

    .form-group label {
      display: block;
      font-size: 0.8rem;
      font-weight: 700;
      color: var(--text);
      margin-bottom: 0.4rem;
      text-transform: uppercase;
      letter-spacing: 0.03em;
    }

    .form-select, .form-input {
      width: 100%;
      background: rgba(0, 0, 0, 0.4);
      border: 1px solid var(--card-border);
      border-radius: 10px;
      padding: 0.65rem 0.85rem;
      color: var(--text);
      font-size: 0.875rem;
      outline: none;
      transition: border-color 0.2s;
    }

    .form-select:focus, .form-input:focus {
      border-color: var(--accent-emerald);
    }

    .form-select option {
      background: #0f172a;
      color: #fff;
    }

    /* TERMINAL CONSOLE */
    .terminal {
      background: #020617;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 10px;
      padding: 1rem;
      font-family: "Courier New", Courier, monospace;
      font-size: 0.825rem;
      color: #38bdf8;
      max-height: 240px;
      overflow-y: auto;
      white-space: pre-wrap;
      word-break: break-all;
      box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.5);
    }

    .terminal-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 0.6rem;
      padding-bottom: 0.4rem;
      border-bottom: 1px solid rgba(255, 255, 255, 0.06);
      font-size: 0.72rem;
      color: var(--text-muted);
    }

    /* PESTAÑA SISTEMA */
    .sys-info-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 1rem;
      margin-bottom: 1.25rem;
    }

    .sys-item {
      background: rgba(0, 0, 0, 0.3);
      border: 1px solid var(--card-border);
      border-radius: 10px;
      padding: 0.85rem 1rem;
    }

    .sys-item label {
      display: block;
      font-size: 0.7rem;
      color: var(--text-muted);
      text-transform: uppercase;
      font-weight: 600;
      margin-bottom: 0.25rem;
    }

    .sys-item span {
      font-size: 0.95rem;
      font-weight: 700;
      color: #fff;
    }

    /* TOAST */
    #toast {
      position: fixed;
      bottom: 2rem;
      right: 2rem;
      background: #1e293b;
      border: 1px solid rgba(255, 255, 255, 0.12);
      color: #fff;
      padding: 0.75rem 1.25rem;
      border-radius: 10px;
      font-size: 0.85rem;
      font-weight: 600;
      box-shadow: 0 10px 25px rgba(0,0,0,0.5);
      display: flex;
      align-items: center;
      gap: 0.5rem;
      opacity: 0;
      transform: translateY(10px);
      transition: all 0.25s ease;
      z-index: 9999;
    }

    #toast.show {
      opacity: 1;
      transform: translateY(0);
    }

    .spinning {
      animation: spin 1s linear infinite;
    }

    @keyframes spin {
      100% { transform: rotate(360deg); }
    }
  </style>
</head>
<body>

  <div class="container">
    
    <!-- BARRA SUPERIOR NAVBAR -->
    <header class="navbar">
      <div class="nav-brand" onclick="navigateTo('home')">
        <div class="brand-icon">
          <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#fff" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
            <rect x="4" y="4" width="16" height="16" rx="2"></rect>
            <rect x="9" y="9" width="6" height="6"></rect>
            <line x1="9" y1="1" x2="9" y2="4"></line>
            <line x1="15" y1="1" x2="15" y2="4"></line>
            <line x1="9" y1="20" x2="9" y2="23"></line>
            <line x1="15" y1="20" x2="15" y2="23"></line>
            <line x1="20" y1="9" x2="23" y2="9"></line>
            <line x1="20" y1="14" x2="23" y2="14"></line>
            <line x1="1" y1="9" x2="4" y2="9"></line>
            <line x1="1" y1="14" x2="4" y2="14"></line>
          </svg>
        </div>
        <div class="brand-text">
          <h1>ESP32 DevKit Core</h1>
          <p>Plataforma Modular de Desarrollo</p>
        </div>
      </div>

      <div class="nav-actions">
        <button class="btn-nav-home" id="btnNavHome" onclick="navigateTo('home')" style="display:none;">
          <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 9l9-7 9 7v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z"></path><polyline points="9 22 9 12 15 12 15 22"></polyline></svg>
          Inicio
        </button>
        <div class="status-badge">
          <span class="pulse-dot"></span>
          <span>En Línea</span>
        </div>
      </div>
    </header>

    <!-- 1. BARRA DE ESTADO COMPACTA (MÉTRICAS REDUCIDAS SIN PERDER DATOS) -->
    <section class="status-bar">
      <!-- CHIP WI-FI -->
      <div class="status-chip" title="Red Wi-Fi Conectada e IP Local">
        <div class="chip-icon" style="background: rgba(59, 130, 246, 0.15); color: #60a5fa;">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M5 12.55a11 11 0 0 1 14.08 0"></path>
            <path d="M1.42 9a16 16 0 0 1 21.16 0"></path>
            <path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path>
            <line x1="12" y1="20" x2="12.01" y2="20"></line>
          </svg>
        </div>
        <div class="chip-info">
          <span class="chip-label">Red Wi-Fi</span>
          <div class="chip-val-row">
            <span class="chip-value" id="statSSID">...</span>
            <span class="chip-sub" id="statIP">IP: ...</span>
          </div>
        </div>
      </div>

      <!-- CHIP SEÑAL -->
      <div class="status-chip" title="Intensidad de señal RSSI y calidad de enlace">
        <div class="chip-icon" style="background: rgba(6, 182, 212, 0.15); color: #22d3ee;">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M2 20h20"></path>
            <path d="M5 20v-4"></path>
            <path d="M9 20v-8"></path>
            <path d="M13 20v-12"></path>
            <path d="M17 20v-16"></path>
          </svg>
        </div>
        <div class="chip-info">
          <span class="chip-label">Señal / RSSI</span>
          <div class="chip-val-row">
            <span class="chip-value" id="statRSSI">... dBm</span>
            <span class="chip-sub" id="statQuality">...%</span>
          </div>
        </div>
      </div>

      <!-- CHIP MEMORIA -->
      <div class="status-chip" title="Memoria RAM libre del microcontrolador">
        <div class="chip-icon" style="background: rgba(16, 185, 129, 0.15); color: #34d399;">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <rect x="2" y="2" width="20" height="8" rx="2" ry="2"></rect>
            <rect x="2" y="14" width="20" height="8" rx="2" ry="2"></rect>
            <line x1="6" y1="6" x2="6.01" y2="6"></line>
            <line x1="6" y1="18" x2="6.01" y2="18"></line>
          </svg>
        </div>
        <div class="chip-info">
          <span class="chip-label">RAM Libre</span>
          <div class="chip-val-row">
            <span class="chip-value" id="statHeap">... KB</span>
            <span class="chip-sub">/ 320 KB</span>
          </div>
        </div>
      </div>

      <!-- CHIP UPTIME -->
      <div class="status-chip" title="Tiempo de actividad ininterrumpido">
        <div class="chip-icon" style="background: rgba(139, 92, 246, 0.15); color: #c084fc;">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <circle cx="12" cy="12" r="10"></circle>
            <polyline points="12 6 12 12 16 14"></polyline>
          </svg>
        </div>
        <div class="chip-info">
          <span class="chip-label">Tiempo Activo</span>
          <div class="chip-val-row">
            <span class="chip-value" id="statUptime">00:00:00</span>
            <span class="chip-sub">esp32.local</span>
          </div>
        </div>
      </div>
    </section>

    <!-- ================================================================= -->
    <!-- 2. VISTA PRINCIPAL: MENÚ DE FUNCIONES (APP HUB / LAUNCHER)       -->
    <!-- ================================================================= -->
    <main id="viewHome" class="view-section active">
      <div class="hub-hero">
        <h2>Funciones & Módulos Disponibles</h2>
        <p>Selecciona una función para abrir su interfaz de control dedicada.</p>
      </div>

      <div class="apps-grid">
        
        <!-- TARJETA 1: ESCÁNER WI-FI -->
        <article class="app-card" onclick="navigateTo('wifi')">
          <div>
            <div class="app-top">
              <div class="app-icon" style="background: linear-gradient(135deg, #1d4ed8, #3b82f6); color: #fff;">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                  <path d="M5 12.55a11 11 0 0 1 14.08 0"></path>
                  <path d="M1.42 9a16 16 0 0 1 21.16 0"></path>
                  <path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path>
                  <line x1="12" y1="20" x2="12.01" y2="20"></line>
                </svg>
              </div>
              <div class="app-meta">
                <h3>
                  <span>Escáner Wi-Fi</span>
                  <span class="app-badge badge-blue">RF 2.4 GHz</span>
                </h3>
                <p>Análisis en vivo del espectro de redes inalámbricas, intensidad RSSI, canales, BSSID y protocolos de seguridad.</p>
              </div>
            </div>
          </div>
          <div class="app-footer">
            <span class="app-status-tag"><span class="dot"></span> Radio Activa</span>
            <span class="app-action-link">Abrir Función &rarr;</span>
          </div>
        </article>

        <!-- TARJETA 2: ESCÁNER BLE -->
        <article class="app-card" onclick="navigateTo('ble')">
          <div>
            <div class="app-top">
              <div class="app-icon" style="background: linear-gradient(135deg, #0284c7, #06b6d4); color: #fff;">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                  <polyline points="6.5 6.5 17.5 17.5 12 23 12 1 17.5 6.5 6.5 17.5"></polyline>
                </svg>
              </div>
              <div class="app-meta">
                <h3>
                  <span>Escáner Bluetooth</span>
                  <span class="app-badge badge-blue">BLE 4.2 / 5</span>
                </h3>
                <p>Detección de periféricos, balizas y wearables con cálculo de distancia estimada (Log-Distance) y datos HEX de fabricante.</p>
              </div>
            </div>
          </div>
          <div class="app-footer">
            <span class="app-status-tag"><span class="dot"></span> Radio BLE Lista</span>
            <span class="app-action-link">Abrir Función &rarr;</span>
          </div>
        </article>

        <!-- TARJETA 3: MÓDULO TEST (REQUERIMIENTO PUNTO 3) -->
        <article class="app-card" onclick="navigateTo('test')">
          <div>
            <div class="app-top">
              <div class="app-icon" style="background: linear-gradient(135deg, #059669, #10b981); color: #fff;">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                  <polygon points="12 2 2 7 12 12 22 7 12 2"></polygon>
                  <polyline points="2 17 12 22 22 17"></polyline>
                  <polyline points="2 12 12 17 22 12"></polyline>
                </svg>
              </div>
              <div class="app-meta">
                <h3>
                  <span>Módulo Test</span>
                  <span class="app-badge badge-green">Metodológico</span>
                </h3>
                <p>Ejemplo interactivo con función C++ TestFunction(): control de LED GPIO 2, medición de microsegundos, eco y plantilla de código.</p>
              </div>
            </div>
          </div>
          <div class="app-footer">
            <span class="app-status-tag"><span class="dot" style="background:#10b981"></span> TestFunction() Online</span>
            <span class="app-action-link" style="color:#34d399">Abrir Función &rarr;</span>
          </div>
        </article>

        <!-- TARJETA: ESCÁNER DE RED LOCAL (LAN) -->
        <article class="app-card" onclick="navigateTo('lan')">
          <div>
            <div class="app-top">
              <div class="app-icon" style="background: linear-gradient(135deg, #4f46e5, #7c3aed); color: #fff;">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                  <rect x="2" y="2" width="6" height="6" rx="1"></rect>
                  <rect x="16" y="2" width="6" height="6" rx="1"></rect>
                  <rect x="9" y="16" width="6" height="6" rx="1"></rect>
                  <path d="M5 8v3a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V8"></path>
                  <line x1="12" y1="12" x2="12" y2="16"></line>
                </svg>
              </div>
              <div class="app-meta">
                <h3>
                  <span>Dispositivos en Wi-Fi (LAN)</span>
                  <span class="app-badge badge-purple">Capa 2 / ARP</span>
                </h3>
                <p>Descubre qué equipos están conectados a tu red Wi-Fi (smartphones, PCs, Smart TVs, consolas) con IP, MAC y fabricante.</p>
              </div>
            </div>
          </div>
          <div class="app-footer">
            <span class="app-status-tag"><span class="dot" style="background:#a855f7"></span> Escáner ARP Listo</span>
            <span class="app-action-link" style="color:#c084fc">Abrir Función &rarr;</span>
          </div>
        </article>

        <!-- TARJETA 5: SIMULADOR JAMMER BLE (DEMO FAKE) -->
        <article class="app-card" onclick="navigateTo('jammer')">
          <div>
            <div class="app-top">
              <div class="app-icon" style="background: linear-gradient(135deg, #b91c1c, #ef4444); color: #fff;">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                  <path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"></path>
                </svg>
              </div>
              <div class="app-meta">
                <h3>
                  <span>Simulador Jammer BLE</span>
                  <span class="app-badge badge-red" style="background:rgba(239,68,68,0.2); color:#f87171; border-color:rgba(239,68,68,0.4);">Demo / Cyber</span>
                </h3>
                <p>Simulador didáctico de saturación e interferencia RF en canales publicitarios Bluetooth (37, 38, 39).</p>
              </div>
            </div>
          </div>
          <div class="app-footer">
            <span class="app-status-tag"><span class="dot" style="background:#ef4444"></span> Simulación Lista</span>
            <span class="app-action-link" style="color:#f87171">Abrir Demo &rarr;</span>
          </div>
        </article>

        <!-- TARJETA 4: SISTEMA Y HARDWARE -->
        <article class="app-card" onclick="navigateTo('system')">
          <div>
            <div class="app-top">
              <div class="app-icon" style="background: linear-gradient(135deg, #d97706, #f59e0b); color: #fff;">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                  <circle cx="12" cy="12" r="3"></circle>
                  <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path>
                </svg>
              </div>
              <div class="app-meta">
                <h3>
                  <span>Sistema & Hardware</span>
                  <span class="app-badge badge-yellow">Diagnóstico</span>
                </h3>
                <p>Parámetros internos de la arquitectura ESP32, memoria Flash, MAC STA, restablecimiento de Wi-Fi y reinicio.</p>
              </div>
            </div>
          </div>
          <div class="app-footer">
            <span class="app-status-tag"><span class="dot"></span> Dual Core 240MHz</span>
            <span class="app-action-link" style="color:#fbbf24">Abrir Función &rarr;</span>
          </div>
        </article>

        <!-- TARJETA 5: GUÍA METODOLÓGICA DE ESCALABILIDAD -->
        <article class="app-card" onclick="navigateTo('guide')" style="border-style: dashed; border-color: rgba(139, 92, 246, 0.35);">
          <div>
            <div class="app-top">
              <div class="app-icon" style="background: linear-gradient(135deg, #7c3aed, #8b5cf6); color: #fff;">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                  <line x1="12" y1="5" x2="12" y2="19"></line>
                  <line x1="5" y1="12" x2="19" y2="12"></line>
                </svg>
              </div>
              <div class="app-meta">
                <h3>
                  <span>+ Agregar Función</span>
                  <span class="app-badge badge-purple">Metodología</span>
                </h3>
                <p>Aprende las 4 reglas estandarizadas para agregar infinitas funciones al menú sin que el código ni la UI se desborden.</p>
              </div>
            </div>
          </div>
          <div class="app-footer">
            <span class="app-status-tag" style="color:#c084fc;">Arquitectura Modular</span>
            <span class="app-action-link" style="color:#c084fc">Ver Guía &rarr;</span>
          </div>
        </article>

      </div>
    </main>

    <!-- ================================================================= -->
    <!-- VISTA DEDICADA 1: ESCÁNER WI-FI                                   -->
    <!-- ================================================================= -->
    <section id="viewWifi" class="view-section">
      <div class="view-header">
        <button class="btn-back" onclick="navigateTo('home')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
          Volver al Inicio
        </button>
        <div class="view-breadcrumb">
          <a onclick="navigateTo('home')">Inicio</a>
          <span>/</span>
          <span class="current">Escáner Wi-Fi</span>
        </div>
      </div>

      <div class="panel-card">
        <div class="panel-header">
          <div class="panel-title">
            <h2>
              <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#3b82f6" stroke-width="2.2">
                <path d="M5 12.55a11 11 0 0 1 14.08 0"></path>
                <path d="M1.42 9a16 16 0 0 1 21.16 0"></path>
                <path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path>
                <line x1="12" y1="20" x2="12.01" y2="20"></line>
              </svg>
              Escáner de Espectro Wi-Fi (2.4 GHz)
            </h2>
            <p>Monitorea y analiza todas las redes inalámbricas al alcance del ESP32 con sus propiedades técnicas.</p>
          </div>
          <div class="panel-actions">
            <input type="text" id="wifiSearch" class="search-input" placeholder="Filtrar por SSID o Canal..." oninput="filterWifiList()">
            <button class="btn-action" id="btnWifiScan" onclick="triggerWifiScan()">
              <svg id="wifiSpinIcon" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <path d="M21.5 2v6h-6M2.5 22v-6h6M2 11.5a10 10 0 0 1 18.8-4.3M22 12.5a10 10 0 0 1-18.8 4.2"/>
              </svg>
              Escanear Redes
            </button>
          </div>
        </div>

        <div class="table-responsive">
          <table>
            <thead>
              <tr>
                <th>SSID (Nombre de Red)</th>
                <th>Intensidad / RSSI</th>
                <th>Canal</th>
                <th>Seguridad</th>
                <th>BSSID (Dirección MAC)</th>
              </tr>
            </thead>
            <tbody id="wifiTableBody">
              <tr>
                <td colspan="5" style="text-align:center; padding: 2.5rem; color: var(--text-muted);">
                  Haz clic en "Escanear Redes" para analizar el espectro Wi-Fi en tiempo real.
                </td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>
    </section>

    <!-- ================================================================= -->
    <!-- VISTA DEDICADA 2: ESCÁNER BLUETOOTH (BLE)                         -->
    <!-- ================================================================= -->
    <section id="viewBle" class="view-section">
      <div class="view-header">
        <button class="btn-back" onclick="navigateTo('home')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
          Volver al Inicio
        </button>
        <div class="view-breadcrumb">
          <a onclick="navigateTo('home')">Inicio</a>
          <span>/</span>
          <span class="current">Escáner Bluetooth (BLE)</span>
        </div>
      </div>

      <div class="panel-card">
        <div class="panel-header">
          <div class="panel-title">
            <h2>
              <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#06b6d4" stroke-width="2.2">
                <polyline points="6.5 6.5 17.5 17.5 12 23 12 1 17.5 6.5 6.5 17.5"></polyline>
              </svg>
              Escáner de Señales Bluetooth Low Energy
            </h2>
            <p>Descubre balizas, periféricos y wearables con cálculo de distancia estimada y análisis de paquetes de anuncio.</p>
          </div>
          <div class="panel-actions">
            <input type="text" id="bleSearch" class="search-input" placeholder="Buscar por Nombre o MAC..." oninput="filterBleList()">
            <button class="btn-action" id="btnBleScan" onclick="triggerBleScan()">
              <svg id="bleSpinIcon" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <circle cx="12" cy="12" r="10"></circle>
                <circle cx="12" cy="12" r="4"></circle>
                <line x1="4.93" y1="4.93" x2="9.17" y2="9.17"></line>
              </svg>
              Iniciar Escaneo BLE (4s)
            </button>
          </div>
        </div>

        <div id="bleResultsList" class="ble-grid">
          <div style="text-align:center; padding: 3rem 1rem; color: var(--text-muted); background: rgba(0,0,0,0.2); border-radius: 12px;">
            Presiona <strong>"Iniciar Escaneo BLE"</strong> para buscar dispositivos en el área.
          </div>
        </div>
      </div>
    </section>

    <!-- ================================================================= -->
    <!-- VISTA DEDICADA 3: MÓDULO TEST (TESTFUNCTION)                      -->
    <!-- ================================================================= -->
    <section id="viewTest" class="view-section">
      <div class="view-header">
        <button class="btn-back" onclick="navigateTo('home')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
          Volver al Inicio
        </button>
        <div class="view-breadcrumb">
          <a onclick="navigateTo('home')">Inicio</a>
          <span>/</span>
          <span class="current">Módulo Test (TestFunction)</span>
        </div>
      </div>

      <div class="panel-card">
        <div class="panel-header">
          <div class="panel-title">
            <h2>
              <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#10b981" stroke-width="2.2">
                <polygon points="12 2 2 7 12 12 22 7 12 2"></polygon>
                <polyline points="2 17 12 22 22 17"></polyline>
                <polyline points="2 12 12 17 22 12"></polyline>
              </svg>
              Módulo de Prueba Metodológico (TestFunction)
            </h2>
            <p>Demuestra el ciclo completo: Petición Frontend &rarr; Endpoint REST &rarr; Ejecución C++ en ESP32 &rarr; Respuesta JSON.</p>
          </div>
        </div>

        <div class="test-box">
          <div style="display:grid; grid-template-columns: repeat(auto-fit, minmax(260px, 1fr)); gap: 1rem;">
            
            <div class="form-group">
              <label for="testCommand">Acción a Ejecutar (TestFunction):</label>
              <select id="testCommand" class="form-select">
                <option value="1">1 - Ping y Medición de Latencia (C++)</option>
                <option value="2">2 - Destello de LED Físico (GPIO 2 Onboard)</option>
                <option value="3">3 - Eco y Procesamiento de Cadena</option>
                <option value="4">4 - Diagnóstico Profundo de Memoria Heap</option>
              </select>
            </div>

            <div class="form-group">
              <label for="testParam">Parámetro de Entrada (inputParam):</label>
              <input type="text" id="testParam" class="form-input" value="Hola ESP32 DevKit" placeholder="Escribe un dato para enviar...">
            </div>

          </div>

          <div style="display:flex; justify-content:flex-end; gap:0.75rem; margin-top:0.5rem;">
            <button class="btn-action" id="btnRunTest" onclick="runTestFunction()" style="background: linear-gradient(135deg, #059669, #10b981);">
              <svg id="testSpinIcon" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>
              Ejecutar TestFunction()
            </button>
          </div>
        </div>

        <!-- CONSOLA DE SALIDA -->
        <div style="margin-top: 1.25rem;">
          <div class="terminal-header">
            <span>📡 Consola de Respuesta del ESP32 (JSON en tiempo real)</span>
            <span id="testDurationTag" style="color:#34d399">Esperando ejecución...</span>
          </div>
          <div id="testOutputConsole" class="terminal">// Presiona "Ejecutar TestFunction()" para enviar la orden por HTTP POST al ESP32.
// La respuesta estructurada aparecerá aquí con los microsegundos exactos medidos en el chip.</div>
        </div>
      </div>
    </section>

    <!-- ================================================================= -->
    <!-- VISTA DEDICADA 4: SISTEMA Y HARDWARE                              -->
    <!-- ================================================================= -->
    <section id="viewSystem" class="view-section">
      <div class="view-header">
        <button class="btn-back" onclick="navigateTo('home')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
          Volver al Inicio
        </button>
        <div class="view-breadcrumb">
          <a onclick="navigateTo('home')">Inicio</a>
          <span>/</span>
          <span class="current">Sistema & Hardware</span>
        </div>
      </div>

      <div class="panel-card">
        <div class="panel-header">
          <div class="panel-title">
            <h2>
              <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#f59e0b" stroke-width="2.2">
                <circle cx="12" cy="12" r="3"></circle>
                <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path>
              </svg>
              Diagnóstico de Hardware y Operación
            </h2>
            <p>Parámetros internos de la arquitectura ESP32 DevKit V1 y opciones de gestión de red.</p>
          </div>
        </div>

        <div class="sys-info-grid">
          <div class="sys-item">
            <label>Modelo de Microcontrolador</label>
            <span id="sysChip">ESP32 (Dual Core)</span>
          </div>
          <div class="sys-item">
            <label>Frecuencia de CPU</label>
            <span id="sysCpu">240 MHz</span>
          </div>
          <div class="sys-item">
            <label>Memoria Flash SPI</label>
            <span id="sysFlash">4 MB</span>
          </div>
          <div class="sys-item">
            <label>Dirección MAC Wi-Fi (STA)</label>
            <span id="sysMac">...</span>
          </div>
          <div class="sys-item">
            <label>Versión de SDK / Core</label>
            <span id="sysSdk">ESP-IDF</span>
          </div>
          <div class="sys-item">
            <label>Firmware Base</label>
            <span>v1.0.0 Standard Core</span>
          </div>
        </div>

        <div style="display: flex; flex-wrap: wrap; gap: 1rem; margin-top: 1.5rem; padding-top: 1.5rem; border-top: 1px solid var(--card-border);">
          <button class="btn-action btn-secondary" onclick="resetWifiConfig()">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M3 12a9 9 0 0 1 15-6.7L21 8M21 3v5h-5M21 12a9 9 0 0 1-15 6.7L3 16M3 21v-5h5"/>
            </svg>
            Reconfigurar Wi-Fi (Abrir Modo AP)
          </button>

          <button class="btn-action btn-danger" onclick="restartDevice()">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M18.36 6.64a9 9 0 1 1-12.73 0"></path>
              <line x1="12" y1="20" x2="12" y2="12"></line>
            </svg>
            Reiniciar ESP32
          </button>
        </div>
      </div>
    </section>

    <!-- ================================================================= -->
    <!-- VISTA DEDICADA: ESCÁNER DE RED LOCAL (LAN / DISPOSITIVOS)         -->
    <!-- ================================================================= -->
    <section id="viewLan" class="view-section">
      <div class="view-header">
        <button class="btn-back" onclick="navigateTo('home')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
          Volver al Inicio
        </button>
        <div class="view-breadcrumb">
          <a onclick="navigateTo('home')">Inicio</a>
          <span>/</span>
          <span class="current">Dispositivos en Wi-Fi (LAN)</span>
        </div>
      </div>

      <div class="panel-card">
        <div class="panel-header">
          <div class="panel-title">
            <h2>
              <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#a855f7" stroke-width="2.2">
                <rect x="2" y="2" width="6" height="6" rx="1"></rect>
                <rect x="16" y="2" width="6" height="6" rx="1"></rect>
                <rect x="9" y="16" width="6" height="6" rx="1"></rect>
                <path d="M5 8v3a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V8"></path>
                <line x1="12" y1="12" x2="12" y2="16"></line>
              </svg>
              Dispositivos Conectados a la Misma Red Wi-Fi
            </h2>
            <p>Explora la subred local mediante sondeo ARP y paquetes de capa 2 para descubrir equipos activos, marcas y direcciones físicas.</p>
          </div>
          <div class="panel-actions">
            <input type="text" id="lanSearch" class="search-input" placeholder="Filtrar por IP, MAC o Fabricante..." oninput="filterLanList()">
            <button class="btn-action" id="btnLanScan" onclick="triggerLanScan()" style="background: linear-gradient(135deg, #6366f1, #8b5cf6);">
              <svg id="lanSpinIcon" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <circle cx="12" cy="12" r="10"></circle>
                <path d="M12 2a10 10 0 0 1 10 10"></path>
              </svg>
              Escanear Mi Wi-Fi
            </button>
          </div>
        </div>

        <!-- BARRA DE INFORMACIÓN Y PROGRESO -->
        <div id="lanProgressBox" style="display:none; margin-bottom:1.25rem; background:rgba(0,0,0,0.3); border:1px solid rgba(139,92,246,0.3); border-radius:10px; padding:0.85rem 1rem;">
          <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:0.4rem; font-size:0.8rem;">
            <span style="color:#c084fc; font-weight:600;" id="lanProgressLabel">Explorando 254 direcciones IP en la subred...</span>
            <span style="color:#fff; font-weight:700;" id="lanProgressPct">0%</span>
          </div>
          <div style="width:100%; height:6px; background:rgba(255,255,255,0.08); border-radius:3px; overflow:hidden;">
            <div id="lanProgressBar" style="width:0%; height:100%; background:linear-gradient(90deg, #6366f1, #a855f7); transition:width 0.3s;"></div>
          </div>
        </div>

        <div class="table-responsive">
          <table>
            <thead>
              <tr>
                <th>Dirección IP</th>
                <th>Fabricante Estimado (OUI)</th>
                <th>Dirección MAC</th>
                <th>Tipo de Equipo</th>
                <th>Latencia</th>
              </tr>
            </thead>
            <tbody id="lanTableBody">
              <tr>
                <td colspan="5" style="text-align:center; padding: 2.5rem; color: var(--text-muted);">
                  Presiona <strong>"Escanear Mi Wi-Fi"</strong> para descubrir todos los dispositivos conectados a tu red.
                </td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>
    </section>

    <!-- ================================================================= -->
    <!-- VISTA DEDICADA 5: GUÍA METODOLÓGICA DE ESCALABILIDAD              -->
    <!-- ================================================================= -->
    <section id="viewGuide" class="view-section">
      <div class="view-header">
        <button class="btn-back" onclick="navigateTo('home')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
          Volver al Inicio
        </button>
        <div class="view-breadcrumb">
          <a onclick="navigateTo('home')">Inicio</a>
          <span>/</span>
          <span class="current">Guía Metodológica</span>
        </div>
      </div>

      <div class="panel-card">
        <div class="panel-title" style="margin-bottom:1.25rem;">
          <h2>
            <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#8b5cf6" stroke-width="2.2">
              <path d="M4 19.5A2.5 2.5 0 0 1 6.5 17H20"></path>
              <path d="M6.5 2H20v20H6.5A2.5 2.5 0 0 1 4 19.5v-15A2.5 2.5 0 0 1 6.5 2z"></path>
            </svg>
            Metodología para Agregar Nuevas Funciones
          </h2>
          <p>Sigue estos 4 pasos para que el sistema escale indefinidamente sin romper su estructura principal.</p>
        </div>

        <div style="display:flex; flex-direction:column; gap:1rem;">
          <div style="background:rgba(0,0,0,0.3); border:1px solid var(--card-border); border-radius:10px; padding:1rem;">
            <strong style="color:#60a5fa; display:block; margin-bottom:0.35rem;">Paso 1: Crear el módulo C++ (&lt;Nombre&gt;Manager.h)</strong>
            <p style="font-size:0.85rem; color:var(--text-muted); line-height:1.5;">Crea una clase con métodos estáticos en la carpeta <code>esp32/</code> (ejemplo: <code>SensorManager.h</code>). Implementa métodos que lean hardware y devuelvan cadenas JSON formateadas.</p>
          </div>

          <div style="background:rgba(0,0,0,0.3); border:1px solid var(--card-border); border-radius:10px; padding:1rem;">
            <strong style="color:#34d399; display:block; margin-bottom:0.35rem;">Paso 2: Registrar Rutas en WebPortal.h</strong>
            <p style="font-size:0.85rem; color:var(--text-muted); line-height:1.5;">Incluye tu cabecera en <code>WebPortal.h</code> y registra tus endpoints REST en el método <code>startStationMode()</code> usando <code>server.on("/api/...", HTTP_GET/POST, handler);</code>.</p>
          </div>

          <div style="background:rgba(0,0,0,0.3); border:1px solid var(--card-border); border-radius:10px; padding:1rem;">
            <strong style="color:#fbbf24; display:block; margin-bottom:0.35rem;">Paso 3: Crear la Tarjeta en el Menú de Inicio (UI_Dashboard.h)</strong>
            <p style="font-size:0.85rem; color:var(--text-muted); line-height:1.5;">En <code>UI_Dashboard.h</code>, añade un bloque <code>&lt;article class="app-card" onclick="navigateTo('tu-modulo')"&gt;</code> dentro de <code>apps-grid</code>.</p>
          </div>

          <div style="background:rgba(0,0,0,0.3); border:1px solid var(--card-border); border-radius:10px; padding:1rem;">
            <strong style="color:#c084fc; display:block; margin-bottom:0.35rem;">Paso 4: Crear la Vista Dedicada (&lt;section id="viewTuModulo"&gt;)</strong>
            <p style="font-size:0.85rem; color:var(--text-muted); line-height:1.5;">Crea un contenedor <code>&lt;section id="viewTuModulo" class="view-section"&gt;</code> con el botón de retorno y tu lógica JavaScript para interactuar con la API REST.</p>
          </div>
        </div>
      </div>
    </section>

    <!-- ================================================================= -->
    <!-- VISTA DEDICADA: SIMULADOR JAMMER BLUETOOTH (DEMO FAKE)           -->
    <!-- ================================================================= -->
    <section id="viewJammer" class="view-section">
      <div class="view-header">
        <button class="btn-back" onclick="navigateTo('home')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
          Volver al Inicio
        </button>
        <div class="view-breadcrumb">
          <a onclick="navigateTo('home')">Inicio</a>
          <span>/</span>
          <span class="current">Simulador Jammer BLE</span>
        </div>
      </div>

      <div style="background: rgba(239, 68, 68, 0.12); border: 1px solid rgba(239, 68, 68, 0.35); border-radius: 12px; padding: 1rem 1.25rem; margin-bottom: 1.5rem; display: flex; align-items: center; gap: 0.85rem;">
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#ef4444" stroke-width="2.2">
          <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"></path>
          <line x1="12" y1="9" x2="12" y2="13"></line>
          <line x1="12" y1="17" x2="12.01" y2="17"></line>
        </svg>
        <div style="font-size: 0.85rem; color: #fca5a5; line-height: 1.4;">
          <strong style="color: #fff;">Aviso Didáctico:</strong> Este módulo es una <strong>simulación de interfaz 100% virtual</strong> para demostraciones y prototipado visual. No se emite interferencia física ni se bloquea ningún dispositivo real.
        </div>
      </div>

      <!-- PANEL DE CONTROL DEL JAMMER -->
      <div class="panel-card" style="margin-bottom: 1.5rem;">
        <div class="panel-header">
          <div class="panel-title">
            <h2>Configuración del Ataque Simulado</h2>
            <p>Selecciona el canal publicitario BLE y el vector de ruido sintético a emular.</p>
          </div>
          <button id="btnToggleJammer" class="btn-action" onclick="toggleJammerSimulation()" style="background: linear-gradient(135deg, #dc2626, #ef4444); box-shadow: 0 4px 14px rgba(239,68,68,0.4);">
            <svg id="jammerIconPlay" width="16" height="16" viewBox="0 0 24 24" fill="currentColor">
              <polygon points="5 3 19 12 5 21 5 3"></polygon>
            </svg>
            <span id="jammerBtnText">Iniciar Simulación</span>
          </button>
        </div>

        <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(240px, 1fr)); gap: 1rem; margin-top: 1rem;">
          <div>
            <label style="display:block; font-size:0.8rem; font-weight:600; color:var(--text-muted); margin-bottom:0.4rem;">Canal BLE Objetivo:</label>
            <select id="jammerChannel" class="search-input" style="width:100%; cursor:pointer;">
              <option value="37">Canal 37 (2402 MHz - ADV Principal 1)</option>
              <option value="38">Canal 38 (2426 MHz - ADV Principal 2)</option>
              <option value="39">Canal 39 (2480 MHz - ADV Principal 3)</option>
              <option value="0">Salto Espectral (Hopping 37-39)</option>
            </select>
          </div>

          <div>
            <label style="display:block; font-size:0.8rem; font-weight:600; color:var(--text-muted); margin-bottom:0.4rem;">Vector de Inundación:</label>
            <select id="jammerMode" class="search-input" style="width:100%; cursor:pointer;">
              <option value="Beacon Flood">Inundación de Balizas (Beacon Flood)</option>
              <option value="Gaussian White Noise">Ruido Blanco Gaussiano (RF Jam)</option>
              <option value="Deauth Spoofing">Desautenticación Simulada</option>
            </select>
          </div>
        </div>
      </div>

      <!-- TELEMETRÍA EN TIEMPO REAL DEL SIMULADOR -->
      <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 1rem; margin-bottom: 1.5rem;">
        <div class="panel-card" style="padding:1rem;">
          <div style="font-size:0.75rem; color:var(--text-muted); font-weight:600; text-transform:uppercase;">Estado Emulador</div>
          <div id="jammerMetricStatus" style="font-size:1.15rem; font-weight:700; color:#9ca3af; margin-top:0.35rem;">INACTIVO</div>
        </div>
        <div class="panel-card" style="padding:1rem;">
          <div style="font-size:0.75rem; color:var(--text-muted); font-weight:600; text-transform:uppercase;">Frecuencia Base</div>
          <div id="jammerMetricFreq" style="font-size:1.15rem; font-weight:700; color:#60a5fa; margin-top:0.35rem;">2402 MHz</div>
        </div>
        <div class="panel-card" style="padding:1rem;">
          <div style="font-size:0.75rem; color:var(--text-muted); font-weight:600; text-transform:uppercase;">Piso de Ruido (RSSI)</div>
          <div id="jammerMetricNoise" style="font-size:1.15rem; font-weight:700; color:#34d399; margin-top:0.35rem;">-92 dBm</div>
        </div>
        <div class="panel-card" style="padding:1rem;">
          <div style="font-size:0.75rem; color:var(--text-muted); font-weight:600; text-transform:uppercase;">Tasa de Paquetes</div>
          <div id="jammerMetricPps" style="font-size:1.15rem; font-weight:700; color:#f87171; margin-top:0.35rem;">0 pkt/s</div>
        </div>
      </div>

      <!-- VISUALIZADOR DE ESPECTRO SIMULADO -->
      <div class="panel-card">
        <div class="panel-title" style="margin-bottom: 1rem;">
          <h2>Actividad Espectral Simulada (2.4 GHz)</h2>
          <p>Representación gráfica del espectro de radiofrecuencia ocupado por la emulación.</p>
        </div>

        <div style="height: 120px; background: rgba(0,0,0,0.4); border: 1px solid var(--card-border); border-radius: 10px; display: flex; align-items: flex-end; justify-content: space-around; padding: 0.5rem; gap: 3px; overflow: hidden;" id="spectrumVisualizer">
        </div>

        <div style="display:flex; justify-content:space-between; margin-top:0.5rem; font-size:0.75rem; color:var(--text-muted); font-family:monospace;">
          <span>2.402 GHz (Ch 37)</span>
          <span>2.426 GHz (Ch 38)</span>
          <span>2.480 GHz (Ch 39)</span>
        </div>

        <!-- CONSOLA DE EVENTOS HACKER DEMO -->
        <div style="margin-top: 1.25rem;">
          <div style="font-size:0.8rem; font-weight:600; color:var(--text-muted); margin-bottom:0.4rem;">Registro de Eventos de Simulación:</div>
          <div id="jammerConsoleLog" style="height: 110px; overflow-y: auto; background: #0b0f19; border: 1px solid rgba(239,68,68,0.25); border-radius: 8px; padding: 0.75rem; font-family: monospace; font-size: 0.775rem; color: #4ade80; line-height: 1.5;">
            <div>[SYS] Motor de simulación BluetoothFakeJammer listo.</div>
            <div>[SYS] Selecciona canal y presiona "Iniciar Simulación".</div>
          </div>
        </div>
      </div>
    </section>

  </div>

  <!-- TOAST NOTIFICACIONES -->
  <div id="toast">
    <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#10b981" stroke-width="2.5"><polyline points="20 6 9 17 4 12"></polyline></svg>
    <span id="toastMsg">Notificación</span>
  </div>

  <script>
    let rawWifiData = [];
    let rawBleData = [];
    let rawLanData = [];
    let lanPollingInterval = null;

    // ================= SISTEMA DE NAVEGACIÓN MODULAR (ROUTER SPA) =================
    function navigateTo(viewName) {
      // Ocultar todas las vistas
      document.querySelectorAll('.view-section').forEach(sec => sec.classList.remove('active'));
      
      const targetId = (viewName === 'home') ? 'viewHome' : 'view' + viewName.charAt(0).toUpperCase() + viewName.slice(1);
      const targetEl = document.getElementById(targetId);
      
      if (targetEl) {
        targetEl.classList.add('active');
        window.scrollTo({ top: 0, behavior: 'smooth' });
      } else {
        document.getElementById('viewHome').classList.add('active');
      }

      // Control del botón "Inicio" en el Navbar superior
      const btnNavHome = document.getElementById('btnNavHome');
      if (btnNavHome) {
        btnNavHome.style.display = (viewName === 'home') ? 'none' : 'inline-flex';
      }

      // Actualizar hash de la URL sin recargar la página para historial del navegador
      if (location.hash !== '#' + viewName) {
        history.pushState(null, '', '#' + viewName);
      }
    }

    // Soporte para botón Atrás/Adelante del navegador
    window.addEventListener('popstate', () => {
      const hash = location.hash.replace('#', '') || 'home';
      navigateTo(hash);
    });

    function showToast(msg) {
      const toast = document.getElementById('toast');
      document.getElementById('toastMsg').innerText = msg;
      toast.classList.add('show');
      setTimeout(() => toast.classList.remove('show'), 2800);
    }

    // ================= 1. TELEMETRÍA DEL SISTEMA (STATUS BAR) =================
    async function loadSystemStats() {
      try {
        const res = await fetch('/api/system/info');
        const data = await res.json();

        // Actualizar barra de estado compacta
        document.getElementById('statSSID').innerText = data.ssid || 'Desconectado';
        document.getElementById('statIP').innerText = 'IP: ' + (data.ip || '0.0.0.0');
        document.getElementById('statRSSI').innerText = (data.rssi || 0) + ' dBm';
        
        let q = 0;
        if (data.rssi >= -50) q = 100;
        else if (data.rssi <= -100) q = 0;
        else q = 2 * (data.rssi + 100);
        document.getElementById('statQuality').innerText = q + '%';

        document.getElementById('statHeap').innerText = Math.round(data.freeHeap / 1024) + ' KB';
        
        let sec = data.uptimeSec || 0;
        let hrs = Math.floor(sec / 3600);
        let mins = Math.floor((sec % 3600) / 60);
        let s = sec % 60;
        document.getElementById('statUptime').innerText = 
          `${String(hrs).padStart(2,'0')}:${String(mins).padStart(2,'0')}:${String(s).padStart(2,'0')}`;

        // Diagnóstico en pestaña sistema
        if (document.getElementById('sysChip')) {
          document.getElementById('sysChip').innerText = data.chipModel || 'ESP32';
          document.getElementById('sysCpu').innerText = data.cpuFreq + ' MHz';
          document.getElementById('sysFlash').innerText = Math.round(data.flashSize / (1024*1024)) + ' MB';
          document.getElementById('sysMac').innerText = data.mac || '-';
          document.getElementById('sysSdk').innerText = data.sdk || '-';
        }
      } catch (e) {
        console.error('Error al actualizar métricas:', e);
      }
    }

    // ================= ESCÁNER WI-FI =================
    async function triggerWifiScan() {
      const btn = document.getElementById('btnWifiScan');
      const icon = document.getElementById('wifiSpinIcon');
      const tbody = document.getElementById('wifiTableBody');

      btn.disabled = true;
      icon.classList.add('spinning');
      tbody.innerHTML = `
        <tr>
          <td colspan="5" style="text-align:center; padding: 2.5rem; color: #60a5fa;">
            <div style="font-weight:600; margin-bottom:0.4rem;">Analizando espectro Wi-Fi (2.4 GHz)...</div>
            <div style="font-size:0.8rem; color:var(--text-muted)">Por favor espera unos segundos.</div>
          </td>
        </tr>`;

      try {
        const res = await fetch('/api/wifi/scan');
        const data = await res.json();
        rawWifiData = data.networks || [];
        rawWifiData.sort((a, b) => b.rssi - a.rssi);
        renderWifiTable(rawWifiData);
        showToast(`${rawWifiData.length} redes Wi-Fi detectadas`);
      } catch (e) {
        tbody.innerHTML = `<tr><td colspan="5" style="text-align:center; padding: 2rem; color: #f87171;">Error al consultar redes Wi-Fi.</td></tr>`;
      } finally {
        btn.disabled = false;
        icon.classList.remove('spinning');
      }
    }

    function renderWifiTable(networks) {
      const tbody = document.getElementById('wifiTableBody');
      if (networks.length === 0) {
        tbody.innerHTML = `<tr><td colspan="5" style="text-align:center; padding: 2rem; color: var(--text-muted);">No se encontraron redes.</td></tr>`;
        return;
      }

      let html = '';
      networks.forEach(net => {
        let badgeClass = 'badge-green';
        if (net.rssi < -80) badgeClass = 'badge-red';
        else if (net.rssi < -65) badgeClass = 'badge-yellow';

        let secBadge = net.isSecure ? `<span class="badge badge-blue">${net.security}</span>` : `<span class="badge badge-green">Abierta</span>`;

        html += `
          <tr>
            <td>
              <strong style="color: #fff;">${net.ssid || '<i style="color:var(--text-muted)">[Red Oculta]</i>'}</strong>
            </td>
            <td>
              <span class="badge ${badgeClass}">${net.rssi} dBm (${net.quality}%)</span>
            </td>
            <td>Canal ${net.channel}</td>
            <td>${secBadge}</td>
            <td style="font-family: monospace; color: #94a3b8;">
              ${net.bssid}
              <button onclick="copyToClipboard('${net.bssid}')" title="Copiar MAC" style="background:none; border:none; color:var(--primary); cursor:pointer; margin-left: 0.35rem;">📋</button>
            </td>
          </tr>
        `;
      });
      tbody.innerHTML = html;
    }

    function filterWifiList() {
      const q = document.getElementById('wifiSearch').value.toLowerCase();
      const filtered = rawWifiData.filter(n => 
        (n.ssid && n.ssid.toLowerCase().includes(q)) || 
        String(n.channel).includes(q) || 
        (n.bssid && n.bssid.toLowerCase().includes(q))
      );
      renderWifiTable(filtered);
    }

    // ================= ESCÁNER BLUETOOTH (BLE) =================
    async function triggerBleScan() {
      const btn = document.getElementById('btnBleScan');
      const icon = document.getElementById('bleSpinIcon');
      const list = document.getElementById('bleResultsList');

      btn.disabled = true;
      icon.classList.add('spinning');
      list.innerHTML = `
        <div style="text-align:center; padding: 3rem 1rem; color: #06b6d4; background: rgba(0,0,0,0.25); border-radius: 12px;">
          <div style="font-size: 1rem; font-weight: 700;">Escaneo Bluetooth BLE en progreso (4s)...</div>
          <div style="font-size: 0.8rem; color: var(--text-muted); margin-top: 0.35rem;">Capturando paquetes de anuncio y estimando proximidad.</div>
        </div>
      `;

      try {
        const res = await fetch('/api/ble/scan');
        const data = await res.json();
        rawBleData = data.devices || [];
        rawBleData.sort((a, b) => b.rssi - a.rssi);
        renderBleList(rawBleData);
        showToast(`${rawBleData.length} dispositivos BLE detectados`);
      } catch (e) {
        list.innerHTML = `<div style="text-align:center; padding: 2rem; color: #f87171; background: rgba(0,0,0,0.2); border-radius: 12px;">Error al ejecutar escaneo BLE.</div>`;
      } finally {
        btn.disabled = false;
        icon.classList.remove('spinning');
      }
    }

    function renderBleList(devices) {
      const list = document.getElementById('bleResultsList');
      if (devices.length === 0) {
        list.innerHTML = `
          <div style="text-align:center; padding: 3rem 1rem; color: var(--text-muted); background: rgba(0,0,0,0.2); border-radius: 12px;">
            No se detectaron dispositivos Bluetooth activos en este ciclo.
          </div>
        `;
        return;
      }

      let html = '';
      devices.forEach((dev, idx) => {
        let distBadge = 'badge-green';
        if (dev.distance < 1.0) distBadge = 'badge-green';
        else if (dev.distance < 3.0) distBadge = 'badge-blue';
        else if (dev.distance < 8.0) distBadge = 'badge-yellow';
        else distBadge = 'badge-purple';

        html += `
          <div class="ble-card">
            <div class="ble-main-row">
              <div class="ble-title-group">
                <div class="ble-icon-circle">
                  <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <polyline points="6.5 6.5 17.5 17.5 12 23 12 1 17.5 6.5 6.5 17.5"></polyline>
                  </svg>
                </div>
                <div>
                  <div class="ble-name-wrapper">
                    <span class="ble-dev-name">${dev.name}</span>
                    <button class="btn-detail-toggle" onclick="toggleBleDetails(${idx})">
                      <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="16" x2="12" y2="12"></line><line x1="12" y1="8" x2="12.01" y2="8"></line></svg>
                      Detalles
                    </button>
                  </div>
                  <div class="ble-dev-mac">${dev.mac} (${dev.addrType})</div>
                </div>
              </div>

              <div class="ble-metrics-group">
                <span class="distance-tag ${distBadge}">
                  ~${dev.distance} m (${dev.distCategory})
                </span>
                <span class="badge badge-blue">
                  ${dev.rssi} dBm
                </span>
              </div>
            </div>

            <div id="bleDetails_${idx}" class="ble-details-panel">
              <div class="details-grid">
                <div class="detail-item">
                  <span>Dirección MAC</span>
                  <span>${dev.mac}</span>
                </div>
                <div class="detail-item">
                  <span>Tipo Dirección</span>
                  <span>${dev.addrType}</span>
                </div>
                <div class="detail-item">
                  <span>RSSI Medido</span>
                  <span>${dev.rssi} dBm</span>
                </div>
                <div class="detail-item">
                  <span>TxPower Calibrado</span>
                  <span>${dev.txPower !== null ? dev.txPower + ' dBm' : 'Estándar (-59 dBm)'}</span>
                </div>
                <div class="detail-item" style="grid-column: 1 / -1;">
                  <span>UUIDs de Servicios</span>
                  <span>${dev.serviceUUID || 'Ninguno anunciado'}</span>
                </div>
                <div class="detail-item" style="grid-column: 1 / -1;">
                  <span>Datos de Fabricante (HEX)</span>
                  <span>${dev.mfgData || 'Sin datos de fabricante'}</span>
                </div>
              </div>
            </div>
          </div>
        `;
      });
      list.innerHTML = html;
    }

    function toggleBleDetails(idx) {
      const el = document.getElementById(`bleDetails_${idx}`);
      if (el) el.classList.toggle('open');
    }

    function filterBleList() {
      const q = document.getElementById('bleSearch').value.toLowerCase();
      const filtered = rawBleData.filter(d => 
        (d.name && d.name.toLowerCase().includes(q)) || 
        (d.mac && d.mac.toLowerCase().includes(q))
      );
      renderBleList(filtered);
    }

    // ================= 3. MÓDULO TEST (TESTFUNCTION) =================
    async function runTestFunction() {
      const cmd = parseInt(document.getElementById('testCommand').value);
      const param = document.getElementById('testParam').value;
      const btn = document.getElementById('btnRunTest');
      const icon = document.getElementById('testSpinIcon');
      const consoleEl = document.getElementById('testOutputConsole');
      const tagEl = document.getElementById('testDurationTag');

      btn.disabled = true;
      icon.classList.add('spinning');
      tagEl.innerText = 'Enviando petición a TestFunction()...';

      const clientStart = performance.now();

      try {
        const payload = { command: cmd, param: param };
        const res = await fetch('/api/test/run', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(payload)
        });

        const clientElapsedMs = Math.round(performance.now() - clientStart);
        const data = await res.json();

        tagEl.innerText = `HTTP ${res.status} OK | Cliente: ${clientElapsedMs}ms | ESP32: ${data.elapsedMicros || 0}µs`;

        const logEntry = `[${new Date().toLocaleTimeString()}] HTTP ${res.status} OK
Comando enviado: ${cmd} | Parámetro: "${param}"
Tiempo en ESP32: ${data.elapsedMicros} microsegundos (${(data.elapsedMicros/1000).toFixed(2)} ms)
LED pulsos: ${data.ledBlinks} | RAM Libre: ${Math.round(data.freeHeap/1024)} KB
Detalle: ${data.detail}

JSON Crudo recibido del ESP32:
${JSON.stringify(data, null, 2)}
---------------------------------------------------------------`;

        consoleEl.innerText = logEntry;
        showToast('TestFunction() completada con éxito');
      } catch (e) {
        tagEl.innerText = 'Error de comunicación';
        consoleEl.innerText = `[${new Date().toLocaleTimeString()}] ERROR: No se pudo comunicar con /api/test/run.\nDetalle: ${e.message}`;
        showToast('Error al ejecutar TestFunction()');
      } finally {
        btn.disabled = false;
        icon.classList.remove('spinning');
      }
    }

    function copyToClipboard(text) {
      navigator.clipboard.writeText(text).then(() => {
        showToast('Copiado al portapapeles');
      }).catch(() => {
        showToast('Error al copiar');
      });
    }

    // ================= 4. ESCÁNER DE RED LOCAL (LAN / DISPOSITIVOS) =================
    async function triggerLanScan() {
      const btn = document.getElementById('btnLanScan');
      const icon = document.getElementById('lanSpinIcon');
      const pBox = document.getElementById('lanProgressBox');
      const pBar = document.getElementById('lanProgressBar');
      const pPct = document.getElementById('lanProgressPct');
      const pLbl = document.getElementById('lanProgressLabel');
      const tbody = document.getElementById('lanTableBody');

      btn.disabled = true;
      icon.classList.add('spinning');
      pBox.style.display = 'block';
      pBar.style.width = '0%';
      pPct.innerText = '0%';
      pLbl.innerText = 'Iniciando barrido ARP de la subred...';

      tbody.innerHTML = `
        <tr>
          <td colspan="5" style="text-align:center; padding: 2.5rem; color: #a855f7;">
            <div style="font-weight:600; margin-bottom:0.4rem;">Sondeando dispositivos en tu red Wi-Fi...</div>
            <div style="font-size:0.8rem; color:var(--text-muted)">Consultando direcciones MAC y fabricantes. Esto toma ~4 segundos.</div>
          </td>
        </tr>
      `;

      try {
        const res = await fetch('/api/lan/scan', { method: 'POST' });
        const resData = await res.json();

        // Iniciar sondeo de progreso
        if (lanPollingInterval) clearInterval(lanPollingInterval);
        lanPollingInterval = setInterval(pollLanStatus, 600);
      } catch (e) {
        tbody.innerHTML = `<tr><td colspan="5" style="text-align:center; padding:2rem; color:#f87171;">Error al iniciar el escaneo de red.</td></tr>`;
        btn.disabled = false;
        icon.classList.remove('spinning');
        pBox.style.display = 'none';
      }
    }

    async function pollLanStatus() {
      try {
        const res = await fetch('/api/lan/devices');
        const data = await res.json();

        const pBar = document.getElementById('lanProgressBar');
        const pPct = document.getElementById('lanProgressPct');
        const pLbl = document.getElementById('lanProgressLabel');
        const btn = document.getElementById('btnLanScan');
        const icon = document.getElementById('lanSpinIcon');
        const pBox = document.getElementById('lanProgressBox');

        let prog = data.progress || 0;
        pBar.style.width = prog + '%';
        pPct.innerText = prog + '%';
        pLbl.innerText = `Sondeando ${data.subnet || 'red local'}... ${data.count || 0} dispositivos encontrados`;

        if (!data.isScanning) {
          clearInterval(lanPollingInterval);
          lanPollingInterval = null;
          btn.disabled = false;
          icon.classList.remove('spinning');
          pBox.style.display = 'none';

          rawLanData = data.devices || [];
          renderLanTable(rawLanData);
          showToast(`${rawLanData.length} equipos detectados en tu Wi-Fi`);
        }
      } catch (e) {
        console.error('Error al consultar estado LAN:', e);
      }
    }

    function renderLanTable(devices) {
      const tbody = document.getElementById('lanTableBody');
      if (devices.length === 0) {
        tbody.innerHTML = `<tr><td colspan="5" style="text-align:center; padding:2rem; color:var(--text-muted);">No se detectaron dispositivos adicionales.</td></tr>`;
        return;
      }

      let html = '';
      devices.forEach(dev => {
        let hostBadge = '<span class="badge badge-purple">Dispositivo LAN</span>';
        if (dev.isGateway) {
          hostBadge = '<span class="badge badge-blue">Router / Puerta de Enlace</span>';
        } else if (dev.isSelf) {
          hostBadge = '<span class="badge badge-green">Este ESP32 (Tú)</span>';
        }

        let vendorBadge = 'badge-yellow';
        if (dev.vendor.includes('Apple')) vendorBadge = 'badge-blue';
        else if (dev.vendor.includes('Samsung')) vendorBadge = 'badge-blue';
        else if (dev.vendor.includes('Espressif')) vendorBadge = 'badge-green';
        else if (dev.vendor.includes('Google') || dev.vendor.includes('Amazon')) vendorBadge = 'badge-purple';

        html += `
          <tr>
            <td>
              <strong style="color: #fff; font-family: monospace; font-size: 0.95rem;">${dev.ip}</strong>
            </td>
            <td>
              <span class="badge ${vendorBadge}" style="font-size:0.775rem;">${dev.vendor}</span>
            </td>
            <td style="font-family: monospace; color: #94a3b8;">
              ${dev.mac}
              <button onclick="copyToClipboard('${dev.mac}')" title="Copiar MAC" style="background:none; border:none; color:var(--primary); cursor:pointer; margin-left: 0.35rem;">📋</button>
            </td>
            <td>${hostBadge}</td>
            <td style="font-family: monospace; color: #34d399;">~${dev.pingMs} ms</td>
          </tr>
        `;
      });
      tbody.innerHTML = html;
    }

    function filterLanList() {
      const q = document.getElementById('lanSearch').value.toLowerCase();
      const filtered = rawLanData.filter(d => 
        (d.ip && d.ip.toLowerCase().includes(q)) || 
        (d.mac && d.mac.toLowerCase().includes(q)) || 
        (d.vendor && d.vendor.toLowerCase().includes(q)) ||
        (d.hostType && d.hostType.toLowerCase().includes(q))
      );
      renderLanTable(filtered);
    }

    // ================= 5. SIMULADOR JAMMER BLE (DEMO) =================
    let jammerActive = false;
    let jammerPollInterval = null;
    let spectrumBars = [];

    function initSpectrumBars() {
      const container = document.getElementById('spectrumVisualizer');
      if (!container || container.children.length > 0) return;
      container.innerHTML = '';
      for (let i = 0; i < 32; i++) {
        const bar = document.createElement('div');
        bar.style.flex = '1';
        bar.style.background = 'linear-gradient(to top, #1e293b, #334155)';
        bar.style.height = '8%';
        bar.style.borderRadius = '2px';
        bar.style.transition = 'height 0.15s ease';
        container.appendChild(bar);
        spectrumBars.push(bar);
      }
    }

    function logJammerEvent(msg) {
      const log = document.getElementById('jammerConsoleLog');
      if (!log) return;
      const d = new Date();
      const timeStr = d.toTimeString().split(' ')[0];
      const line = document.createElement('div');
      line.innerText = `[${timeStr}] ${msg}`;
      log.appendChild(line);
      log.scrollTop = log.scrollHeight;
    }

    async function toggleJammerSimulation() {
      const channel = document.getElementById('jammerChannel').value;
      const mode = document.getElementById('jammerMode').value;

      if (!jammerActive) {
        try {
          const res = await fetch('/api/jammer/start', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ channel: parseInt(channel), mode: mode })
          });
          const data = await res.json();
          jammerActive = true;
          updateJammerView(data);
          logJammerEvent(`SIMULACIÓN INICIADA: Canal ${channel} | Modo: ${mode}`);
          showToast('Simulación Jammer Iniciada');
          if (jammerPollInterval) clearInterval(jammerPollInterval);
          jammerPollInterval = setInterval(pollJammerTelemetry, 800);
        } catch (e) {
          showToast('Error al iniciar emulador');
        }
      } else {
        try {
          const res = await fetch('/api/jammer/stop', { method: 'POST' });
          const data = await res.json();
          jammerActive = false;
          clearInterval(jammerPollInterval);
          jammerPollInterval = null;
          updateJammerView(data);
          logJammerEvent('SIMULACIÓN DETENIDA: Señal normalizada.');
          showToast('Simulación Jammer Detenida');
        } catch (e) {
          showToast('Error al detener emulador');
        }
      }
    }

    async function pollJammerTelemetry() {
      if (!jammerActive) return;
      try {
        const res = await fetch('/api/jammer/status');
        const data = await res.json();
        updateJammerView(data);
      } catch (e) {
        console.error('Error al consultar telemetría Jammer:', e);
      }
    }

    function updateJammerView(data) {
      const btn = document.getElementById('btnToggleJammer');
      const btnText = document.getElementById('jammerBtnText');
      const stEl = document.getElementById('jammerMetricStatus');
      const frEl = document.getElementById('jammerMetricFreq');
      const nsEl = document.getElementById('jammerMetricNoise');
      const ppEl = document.getElementById('jammerMetricPps');

      initSpectrumBars();

      if (data.active) {
        jammerActive = true;
        btn.style.background = 'linear-gradient(135deg, #475569, #334155)';
        btnText.innerText = 'Detener Simulación';
        stEl.innerText = 'TRANSMITIENDO';
        stEl.style.color = '#ef4444';
        frEl.innerText = data.frequencyMhz + ' MHz';
        nsEl.innerText = data.noiseDbm + ' dBm';
        nsEl.style.color = '#f87171';
        ppEl.innerText = data.packetsPerSecond.toLocaleString() + ' pkt/s';

        // Animar barras de espectro con ruido simulado
        spectrumBars.forEach((bar, idx) => {
          const h = Math.floor(Math.random() * 70) + 25;
          bar.style.height = h + '%';
          bar.style.background = (h > 65) 
            ? 'linear-gradient(to top, #dc2626, #ef4444)' 
            : 'linear-gradient(to top, #ea580c, #f59e0b)';
        });
      } else {
        jammerActive = false;
        btn.style.background = 'linear-gradient(135deg, #dc2626, #ef4444)';
        btnText.innerText = 'Iniciar Simulación';
        stEl.innerText = 'INACTIVO';
        stEl.style.color = '#9ca3af';
        frEl.innerText = data.frequencyMhz + ' MHz';
        nsEl.innerText = data.noiseDbm + ' dBm';
        nsEl.style.color = '#34d399';
        ppEl.innerText = '0 pkt/s';

        // Reducir barras a nivel base
        spectrumBars.forEach(bar => {
          bar.style.height = (Math.floor(Math.random() * 8) + 4) + '%';
          bar.style.background = 'linear-gradient(to top, #1e293b, #334155)';
        });
      }
    }

    // ================= ACCIONES DE SISTEMA =================
    async function resetWifiConfig() {
      if (confirm('¿Restablecer configuración Wi-Fi? El ESP32 borrará las credenciales y reiniciará en modo Punto de Acceso.')) {
        try {
          await fetch('/api/system/reset-wifi', { method: 'POST' });
          alert('Credenciales borradas. Conéctate a la red ESP32-Config...');
          window.location.reload();
        } catch (e) {
          alert('Comando enviado.');
        }
      }
    }

    async function restartDevice() {
      if (confirm('¿Reiniciar el microcontrolador ESP32?')) {
        try {
          await fetch('/api/system/restart', { method: 'POST' });
          showToast('Reiniciando microcontrolador...');
          setTimeout(() => window.location.reload(), 4000);
        } catch (e) {
          showToast('Reiniciando...');
        }
      }
    }

    // Inicialización al cargar la página
    window.addEventListener('DOMContentLoaded', () => {
      // Rutas iniciales según Hash en URL (ej: #wifi, #ble, #test)
      const initialView = location.hash.replace('#', '') || 'home';
      navigateTo(initialView);

      // Cargar métricas y auto-refresco
      loadSystemStats();
      setInterval(loadSystemStats, 4000);
    });
  </script>
</body>
</html>
)rawliteral";

#endif // UI_DASHBOARD_H
