/**
 * @file dashboard_html.h
 * @brief Embedded Web Dashboard for ESP32 IoT Configuration Panel
 * @author IoT Device Dashboard Project
 * @version 2.1.0
 * @date 2025-01-30
 * 
 * @details
 * This file contains the complete web dashboard interface for the ESP32 IoT
 * configuration panel. The dashboard is embedded as a PROGMEM string to save
 * RAM and provides a modern, responsive web interface for device management.
 * 
 * @section features Dashboard Features
 * - Modern responsive design with dark theme
 * - Dual-mode operation (WiFi and GSM modes)
 * - Real-time status monitoring and updates
 * - Interactive configuration forms
 * - Captive portal support
 * - Mobile-friendly responsive layout
 * - Real-time data polling and caching
 * 
 * @section tabs Dashboard Tabs
 * 1. **User Management**: User profile configuration and system information
 * 2. **WiFi Configuration**: Network scanning, connection, and status monitoring
 * 3. **GSM Settings**: GSM operations, testing, and network information
 * 
 * @section api API Integration
 * The dashboard communicates with the ESP32 backend through REST API endpoints:
 * - GET/POST /api/status - System status information
 * - GET/POST /api/wifi/* - WiFi management operations
 * - GET/POST /api/gsm/* - GSM operations and testing
 * - GET/POST /api/load/save/user - User profile management
 * 
 * @section design Design Features
 * - Clean, modern interface with professional styling
 * - Intuitive navigation with tab-based organization
 * - Real-time status indicators and progress feedback
 * - Error handling and user feedback
 * - Responsive design for mobile and desktop
 * - Loading states and visual feedback
 * 
 * @section javascript JavaScript Features
 * - Asynchronous API communication
 * - Real-time data polling and updates
 * - Form validation and error handling
 * - Dynamic content updates
 * - Caching and performance optimization
 * - Cross-browser compatibility
 * 
 * @section css CSS Features
 * - Modern CSS Grid and Flexbox layouts
 * - Responsive design with media queries
 * - Dark theme with professional color scheme
 * - Smooth animations and transitions
 * - Mobile-first responsive design
 * - Cross-browser compatibility
 * 
 * @section performance Performance Optimizations
 * - Embedded in PROGMEM to save RAM
 * - Efficient API polling with caching
 * - Minimal external dependencies
 * - Optimized for low-memory devices
 * - Fast loading and responsive interface
 */

#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H

// Standard Arduino library for PROGMEM support
#include <Arduino.h>

/**
 * @brief Complete web dashboard HTML/CSS/JavaScript
 * 
 * This PROGMEM string contains the entire web dashboard including:
 * - HTML structure and content
 * - CSS styling and responsive design
 * - JavaScript functionality and API integration
 * - Real-time data polling and updates
 * - Form handling and validation
 * - Error handling and user feedback
 * 
 * The dashboard is designed to be self-contained and requires no external
 * resources, making it ideal for embedded applications.
 */
const char dashboard_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Configuration Panel</title>
  <style>
* { margin: 0; padding: 0; box-sizing: border-box; }
body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
  background: #0f172a;
  color: #ffffff; 
  min-height: 100vh; 
  display: flex; 
  align-items: center; 
  justify-content: center; 
  padding: 20px;
  line-height: 1.5;
}
.container { 
  width: 100%; 
  max-width: 1200px; 
  background: #1e293b;
  border: 1px solid #334155;
  border-radius: 12px; 
  padding: 32px; 
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
}
.header { text-align: center; margin-bottom: 32px; }
.header h1 { 
  font-size: 2rem; 
  font-weight: 600; 
  color: #ffffff;
  margin-bottom: 8px; 
  letter-spacing: -0.025em;
}
.header p { 
  color: #94a3b8; 
  font-size: 0.875rem; 
  font-weight: 400;
}
.mode-toggle-container { display: flex; justify-content: flex-start; margin-bottom: 24px; }
.mode-toggle { 
  display: flex; 
  background: #334155; 
  border-radius: 8px; 
  padding: 4px; 
  gap: 2px; 
}
.mode-btn { 
  background: transparent; 
  border: none; 
  padding: 8px 16px; 
  cursor: pointer; 
  border-radius: 6px;
  color: #94a3b8; 
  font-weight: 500; 
  font-size: 0.875rem; 
  transition: all 0.2s ease; 
}
.mode-btn:hover { 
  color: #ffffff; 
  background: #475569; 
}
.mode-btn.active { 
  background: #3b82f6; 
  color: #ffffff; 
  font-weight: 500; 
}
.tabs { 
  display: flex; 
  justify-content: center; 
  margin-bottom: 32px; 
  background: #334155;
  border-radius: 8px; 
  padding: 4px; 
  gap: 2px; 
}
.tab-btn { 
  background: transparent; 
  border: none; 
  padding: 12px 24px; 
  cursor: pointer; 
  border-radius: 6px;
  color: #94a3b8; 
  font-weight: 500; 
  font-size: 0.875rem; 
  transition: all 0.2s ease; 
}
.tab-btn:hover { 
  color: #ffffff; 
  background: #475569; 
}
.tab-btn.active { 
  background: #3b82f6; 
  color: #ffffff; 
  font-weight: 500; 
}
.tab-btn.hidden { display: none; }
.tab-content { display: none; }
.tab-content.active { display: block; }
.content-grid { 
  display: grid; 
  grid-template-columns: repeat(auto-fit, minmax(350px, 1fr)); 
  gap: 24px; 
}
.card { 
  background: #334155; 
  border: 1px solid #475569; 
  border-radius: 8px; 
  padding: 24px; 
}
.card h3 { 
  font-size: 1.125rem; 
  font-weight: 600; 
  color: #ffffff; 
  margin-bottom: 20px; 
  display: flex; 
  align-items: center; 
  gap: 8px; 
}
.status-row { 
  display: flex; 
  justify-content: space-between; 
  align-items: center; 
  padding: 12px 0; 
  border-bottom: 1px solid #475569; 
}
.status-row:last-child { 
  border-bottom: none; 
}
.status-label { 
  color: #94a3b8; 
  font-weight: 400; 
  font-size: 0.875rem;
}
.status-value { 
  color: #ffffff; 
  font-weight: 500; 
  font-size: 0.875rem;
}
.status-connected { color: #22d3ee; }
.status-disconnected { color: #94a3b8; }
.status-warning { color: #fbbf24; }
.form-group { margin-bottom: 20px; }
.form-group label { 
  display: block; 
  margin-bottom: 8px; 
  font-weight: 500; 
  color: #cbd5e1; 
  font-size: 0.875rem; 
}
.input-row { 
  display: grid; 
  grid-template-columns: 1fr auto; 
  gap: 12px; 
  align-items: end; 
}
input, select { 
  width: 100%; 
  padding: 12px 16px; 
  border: 1px solid #475569; 
  border-radius: 6px;
  background: #334155; 
  color: #ffffff; 
  font-size: 0.875rem; 
  transition: all 0.2s ease; 
}
input:focus, select:focus { 
  outline: none; 
  border-color: #3b82f6; 
  box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1); 
}
input:hover, select:hover { 
  border-color: #64748b; 
}
.network-dropdown { position: relative; }
.network-dropdown select { 
  appearance: none;
  background-image: url("data:image/svg+xml,%3csvg xmlns='http://www.w3.org/2000/svg' fill='none' viewBox='0 0 20 20'%3e%3cpath stroke='%2394a3b8' stroke-linecap='round' stroke-linejoin='round' stroke-width='1.5' d='M6 8l4 4 4-4'/%3e%3c/svg%3e");
  background-position: right 12px center; 
  background-repeat: no-repeat; 
  background-size: 16px; 
  padding-right: 40px; 
  cursor: pointer; 
}
.network-dropdown select:disabled { 
  opacity: 0.5; 
  cursor: not-allowed; 
}
.signal-indicator { 
  position: absolute; 
  right: 40px; 
  top: 50%; 
  transform: translateY(-50%); 
  font-size: 0.75rem; 
  padding: 2px 6px; 
  border-radius: 4px; 
  font-weight: 500; 
  pointer-events: none; 
}
.signal-strong { background: rgba(34, 211, 238, 0.2); color: #22d3ee; }
.signal-medium { background: rgba(251, 191, 36, 0.2); color: #fbbf24; }
.signal-weak { background: rgba(239, 68, 68, 0.2); color: #ef4444; }
.password-input { position: relative; }
.password-toggle { 
  position: absolute; 
  right: 12px; 
  top: 50%; 
  transform: translateY(-50%); 
  background: transparent; 
  border: none; 
  color: #94a3b8; 
  cursor: pointer; 
  padding: 8px; 
  border-radius: 4px; 
  transition: all 0.2s ease; 
}
.password-toggle:hover { 
  color: #ffffff; 
  background: #475569; 
}
.button-group { 
  display: flex; 
  gap: 12px; 
  margin-top: 24px; 
  flex-wrap: wrap; 
}
.button-row { 
  display: flex; 
  gap: 12px; 
  width: 100%; 
}
button { 
  padding: 12px 20px; 
  border: none; 
  border-radius: 6px; 
  cursor: pointer; 
  font-weight: 500; 
  font-size: 0.875rem;
  transition: all 0.2s ease; 
  position: relative; 
  overflow: hidden; 
  min-height: 40px; 
}
button:hover { 
  transform: translateY(-1px); 
}
button:active { 
  transform: translateY(0); 
}
button:disabled { 
  opacity: 0.5; 
  cursor: not-allowed; 
  transform: none; 
}
.btn-primary { 
  background: #3b82f6; 
  color: #ffffff; 
  font-weight: 500; 
}
.btn-primary:hover:not(:disabled) { 
  background: #2563eb; 
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1); 
}
.btn-secondary { 
  background: #475569; 
  color: #ffffff; 
  border: 1px solid #64748b; 
}
.btn-secondary:hover:not(:disabled) { 
  background: #64748b; 
  border-color: #94a3b8; 
}
.btn-danger { 
  background: #ef4444; 
  color: #ffffff; 
}
.btn-danger:hover:not(:disabled) { 
  background: #dc2626; 
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1); 
}
.btn-scan { 
  background: #22d3ee; 
  color: #0f172a; 
}
.btn-scan:hover:not(:disabled) { 
  background: #06b6d4; 
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1); 
}
.btn-full { flex: 1; }
.loading { 
  pointer-events: none; 
  opacity: 0.7; 
}
.loading::after { 
  content: ''; 
  position: absolute; 
  inset: 0; 
  background: linear-gradient(45deg, transparent, rgba(59, 130, 246, 0.2), transparent); 
  animation: shimmer 1.5s infinite; 
}
@keyframes shimmer { 
  0% { transform: translateX(-100%); } 
  100% { transform: translateX(100%); } 
}
@media (max-width: 768px) {
  .container { padding: 20px; }
  .content-grid { grid-template-columns: 1fr; }
  .tabs { flex-direction: column; }
  .button-row { flex-direction: column; }
  .input-row { grid-template-columns: 1fr; }
}
  </style>
</head>
<body>
  <div class="container">
    <!-- Mode Toggle -->
    <div class="mode-toggle-container">
      <div class="mode-toggle">
        <button class="mode-btn active" onclick="switchMode('wifi')" id="wifiModeBtn">WiFi Mode</button>
        <button class="mode-btn" onclick="switchMode('gsm')" id="gsmModeBtn">GSM Mode</button>
      </div>
    </div>

    <div class="header">
      <h1>Configuration Panel</h1>
      <p>Advanced IoT Monitoring & Configuration System</p>
    </div>

    <!-- Tabs -->
    <div class="tabs" id="tabContainer">
      <button class="tab-btn active" onclick="showTab('user')">User Management</button>
      <button class="tab-btn" onclick="showTab('wifi')" id="wifiTabBtn">WiFi Configuration</button>
      <button class="tab-btn" onclick="showTab('gsm')" id="gsmTabBtn">GSM Settings</button>
    </div>

    <!-- User Tab -->
    <div id="user" class="tab-content active">
      <div class="content-grid">
        <div class="card">
          <h3>System Information</h3>
          <div class="status-row">
            <span class="status-label">Device Model</span>
            <span class="status-value">ESP32 DevKit</span>
          </div>
          <div class="status-row">
            <span class="status-label">Firmware Version</span>
            <span class="status-value">v2.1.0</span>
          </div>
          <div class="status-row">
            <span class="status-label">Last Updated</span>
            <span class="status-value">2025-08-30</span>
          </div>
           <div class="status-row">
             <span class="status-label">Saved User Name</span>
             <span class="status-value" id="savedUserName">—</span>
           </div>
           <div class="status-row">
             <span class="status-label">Saved User Email</span>
             <span class="status-value" id="savedUserEmail">—</span>
           </div>
           <div class="status-row">
             <span class="status-label">Saved User Phone</span>
             <span class="status-value" id="savedUserPhone">—</span>
           </div>
        </div>

        <div class="card">
          <h3>User Profile</h3>

          <div class="form-group">
            <label for="userName">Full Name</label>
            <input type="text" id="userName" placeholder="Enter your full name">
          </div>

          <div class="form-group">
            <label for="userEmail">Email Address</label>
            <input type="email" id="userEmail" placeholder="your.email@example.com">
          </div>

          <div class="form-group">
            <label for="userContact">Contact Number</label>
            <input type="tel" id="userContact" placeholder="+94 XX XXXXXXX">
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-primary btn-full">Save Profile</button>
              <button class="btn-danger btn-full">Clear Form</button>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- WiFi Tab -->
    <div id="wifi" class="tab-content">
      <div class="content-grid">
        <div class="card">
          <h3>Network Status</h3>
          <div class="status-row">
            <span class="status-label">Access Point IP</span>
            <span class="status-value" id="apAddress">192.168.4.1</span>
          </div>
          <div class="status-row">
            <span class="status-label">Access Point SSID</span>
            <span class="status-value" id="apSSID">ESP32-AccessPoint</span>
          </div>
          <div class="status-row">
            <span class="status-label">WiFi Station IP</span>
            <span class="status-value" id="staAddress">Not connected</span>
          </div>
          <div class="status-row">
            <span class="status-label">Connected Network</span>
            <span class="status-value" id="connectedNetwork">None</span>
          </div>
          <div class="status-row">
            <span class="status-label">Internet Connection</span>
            <span class="status-value" id="internetStatus">Not connected</span>
          </div>
          <div class="status-row">
            <span class="status-label">Connected Devices</span>
            <span class="status-value" id="connectedDevices">0</span>
          </div>
        </div>

        <div class="card">
          <h3>WiFi Configuration</h3>
          <div class="input-row" style="margin-bottom: 24px;">
            <button class="btn-scan btn-full" onclick="scanWifi()" id="scanButton">Scan Networks</button>
          </div>

          <div class="form-group">
            <label for="networkSelect">Available Networks</label>
            <div class="network-dropdown">
              <select id="networkSelect" onchange="selectNetwork()" disabled>
                <option value="">Click 'Scan Networks' to find available networks</option>
              </select>
              <span class="signal-indicator" id="signalIndicator"></span>
            </div>
          </div>

          <div class="form-group">
            <label for="wifiPass">Network Password</label>
            <div class="password-input">
              <input type="password" id="wifiPass" placeholder="Enter WiFi password" disabled>
              <button type="button" class="password-toggle" onclick="togglePassword()">👁</button>
            </div>
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-primary btn-full" onclick="connectNetwork()" disabled id="connectBtn">Connect Network</button>
              <button class="btn-danger btn-full" onclick="disconnectNetwork()" id="disconnectBtn">Disconnect</button>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- GSM Tab -->
    <div id="gsm" class="tab-content">
      <div class="content-grid">
        <div class="card">
          <h3>GSM Status</h3>
          <div class="status-row">
            <span class="status-label">Connection Status</span>
            <span class="status-value status-connected">Active</span>
          </div>
          <div class="status-row">
            <span class="status-label">Signal Strength</span>
            <span class="status-value" id="signalStrength">— dBm</span>
          </div>
          <div class="status-row">
            <span class="status-label">Network Operator</span>
            <span class="status-value" id="carrierName">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Network Mode</span>
            <span class="status-value" id="networkMode">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Registration Status</span>
            <span class="status-value" id="registrationStatus">—</span>
          </div>
        </div>

        <div class="card">
          <h3>GSM Testing</h3>
          <div class="form-group">
            <label for="testPhoneNumber">Test Phone Number</label>
            <input type="text" id="testPhoneNumber" placeholder="Enter phone number for testing (e.g., +94719792341)">
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-secondary btn-full" onclick="testSMS()" id="smsTestBtn">Send Test SMS</button>
              <button class="btn-secondary btn-full" onclick="testCall()" id="callTestBtn">Make Test Call</button>
            </div>
            <div class="button-row">
              <button class="btn-secondary btn-full" onclick="detectNetwork()" id="detectNetworkBtn">Detect Network Info</button>
              <button class="btn-secondary btn-full" onclick="refreshGSMStatus()" id="refreshGSMBtn">Refresh GSM Status</button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <script>
let availableNetworks = [];
let selectedNetworkData = null;
let gsmPoll = null;
let currentMode = 'wifi'; // Default to WiFi mode

// ---------- Utilities ----------
async function apiGet(url) {
  try {
    const r = await fetch(url);
    if (!r.ok) {
      const errorText = await r.text();
      console.error('API GET Error:', url, r.status, errorText);
      throw new Error(`HTTP ${r.status}: ${errorText}`);
    }
    const jsonData = await r.json();
    console.log('API GET Success:', url, jsonData);
    return jsonData;
  } catch (error) {
    console.error('API GET Exception:', url, error);
    throw error;
  }
}
async function apiPost(url, bodyObj) {
  try {
    const r = await fetch(url, { 
      method: 'POST', 
      headers: {'Content-Type':'application/json'}, 
      body: JSON.stringify(bodyObj || {}) 
    });
    if (!r.ok) {
      const errorText = await r.text();
      console.error('API POST Error:', url, r.status, errorText);
      throw new Error(`HTTP ${r.status}: ${errorText}`);
    }
    const jsonData = await r.json();
    console.log('API POST Success:', url, jsonData);
    return jsonData;
  } catch (error) {
    console.error('API POST Exception:', url, error);
    throw error;
  }
}
function strengthFromRSSI(rssi) {
  if (rssi >= -60) return 'strong';
  if (rssi >= -75) return 'medium';
  return 'weak';
}

// ---------- Mode Toggle ----------
function switchMode(mode) {
  currentMode = mode;
  
  // Update mode buttons
  document.querySelectorAll(".mode-btn").forEach(btn => btn.classList.remove("active"));
  document.getElementById(mode + 'ModeBtn').classList.add("active");
  
  // Show/hide tabs based on mode
  const wifiTabBtn = document.getElementById('wifiTabBtn');
  const gsmTabBtn = document.getElementById('gsmTabBtn');
  
  if (mode === 'wifi') {
    wifiTabBtn.classList.remove('hidden');
    gsmTabBtn.classList.add('hidden');
    // Switch to WiFi tab if GSM was active
    if (document.getElementById('gsm').classList.contains('active')) {
      showTab('wifi');
    }
  } else if (mode === 'gsm') {
    wifiTabBtn.classList.add('hidden');
    gsmTabBtn.classList.remove('hidden');
    // Switch to GSM tab if WiFi was active
    if (document.getElementById('wifi').classList.contains('active')) {
      showTab('gsm');
    }
  }
}

// ---------- Tabs ----------
function showTab(tabId) {
  // Check if tab is available in current mode
  if (currentMode === 'wifi' && tabId === 'gsm') return;
  if (currentMode === 'gsm' && tabId === 'wifi') return;
  
  document.querySelectorAll(".tab-content").forEach(tab => tab.classList.remove("active"));
  document.querySelectorAll(".tab-btn").forEach(btn => btn.classList.remove("active"));
  document.getElementById(tabId).classList.add("active");
  event.target.classList.add("active");

  if (tabId === 'gsm') startGsmPoll(); else stopGsmPoll();
}

// ---------- Password toggle ----------
function togglePassword() {
  const passField = document.getElementById("wifiPass");
  const toggleBtn = document.querySelector(".password-toggle");
  if (passField.type === "password") { passField.type = "text"; toggleBtn.textContent = "👁"; }
  else { passField.type = "password"; toggleBtn.textContent = "👁"; }
}

// ---------- Status ----------
async function refreshStatus() {
  try {
    const st = await apiGet('/api/status');
    // AP
    document.getElementById('apAddress').textContent = st.ap?.ip || '—';
    document.getElementById('apSSID').textContent = st.ap?.ssid || '—';
    // Connected devices count
    document.getElementById('connectedDevices').textContent = st.ap?.connectedDevices || '0';
    // STA
    const staConnected = st.sta?.connected || false;
    const staIP = st.sta?.ip || '0.0.0.0';
    const staSSID = st.sta?.ssid || '';
    const staAddressEl = document.getElementById('staAddress');
    if (staConnected && staIP !== '0.0.0.0') {
      staAddressEl.textContent = staIP; staAddressEl.className = 'status-value status-connected';
    } else { staAddressEl.textContent = 'Not connected'; staAddressEl.className = 'status-value status-disconnected'; }
    const connectedNetworkEl = document.getElementById('connectedNetwork');
    if (staConnected && staSSID) { connectedNetworkEl.textContent = staSSID; connectedNetworkEl.className = 'status-value status-connected'; }
    else { connectedNetworkEl.textContent = 'None'; connectedNetworkEl.className = 'status-value status-disconnected'; }
    const internetStatusEl = document.getElementById('internetStatus');
    if (staConnected) { internetStatusEl.textContent = 'Connected'; internetStatusEl.className = 'status-value status-connected'; }
    else { internetStatusEl.textContent = 'Not connected'; internetStatusEl.className = 'status-value status-disconnected'; }
  } catch (e) {
    document.getElementById('apAddress').textContent = 'Error';
    document.getElementById('apSSID').textContent = 'Error';
    document.getElementById('connectedDevices').textContent = 'Error';
    document.getElementById('staAddress').textContent = 'Error';
    document.getElementById('connectedNetwork').textContent = 'Error';
    document.getElementById('internetStatus').textContent = 'Error';
  }
}

// ---------- WiFi ----------
async function scanWifi() {
  const button = document.getElementById('scanButton');
  const networkSelect = document.getElementById('networkSelect');
  const signalIndicator = document.getElementById('signalIndicator');
  const wifiPass = document.getElementById('wifiPass');
  const connectBtn = document.getElementById('connectBtn');

  button.classList.add('loading'); button.textContent = 'Scanning...'; button.disabled = true;
  networkSelect.disabled = true; wifiPass.disabled = true; connectBtn.disabled = true;
  wifiPass.value = ''; signalIndicator.style.display = 'none';
  networkSelect.innerHTML = '<option value="">Scanning for networks...</option>';

  try {
    console.log('Starting WiFi scan...');
    const scan = await apiGet('/api/wifi/scan');
    console.log('Scan response:', scan);
    
    if (!Array.isArray(scan)) {
      console.error('Invalid scan response:', scan);
      throw new Error('Invalid scan response: ' + JSON.stringify(scan));
    }
    
    const best = {}; 
    scan.forEach(n => { 
      if (!n.ssid) return; 
      if (!best[n.ssid] || (n.rssi > best[n.ssid].rssi)) best[n.ssid] = n; 
    });
    
    availableNetworks = Object.values(best).sort((a,b)=>b.rssi - a.rssi);
    networkSelect.innerHTML = '<option value="">Select a network...</option>';
    
    if (availableNetworks.length === 0) {
      networkSelect.innerHTML = '<option value="">No networks found</option>';
    } else {
      availableNetworks.forEach(n => {
        const opt = document.createElement('option');
        const sec = (n.security || (n.auth===0?'Open':'Secure'));
        const strength = n.strength || strengthFromRSSI(n.rssi || -80);
        opt.value = n.ssid; 
        opt.textContent = `${n.ssid} (${sec}, ${strength})`;
        networkSelect.appendChild(opt);
      });
    }
  } catch (e) {
    console.error('WiFi scan error:', e);
    alert('Scan failed: ' + e.message + '\nPlease try again or check if WiFi is enabled.');
    networkSelect.innerHTML = '<option value="">Scan failed - try again</option>';
  } finally {
    networkSelect.disabled = false;
    button.classList.remove('loading'); button.textContent = 'Scan Networks'; button.disabled = false;
  }
}

function selectNetwork() {
  const select = document.getElementById('networkSelect');
  const signalIndicator = document.getElementById('signalIndicator');
  const wifiPass = document.getElementById('wifiPass');
  const connectBtn = document.getElementById('connectBtn');

  const ssid = select.value;
  if (ssid) {
    selectedNetworkData = availableNetworks.find(n => n.ssid === ssid) || {ssid, rssi:-80, security:'Secure'};
    const strength = selectedNetworkData.strength || strengthFromRSSI(selectedNetworkData.rssi || -80);
    signalIndicator.textContent = strength[0].toUpperCase() + strength.slice(1);
    signalIndicator.className = `signal-indicator signal-${strength}`;
    signalIndicator.style.display = 'block';
    wifiPass.disabled = (selectedNetworkData.security === 'Open');
    connectBtn.disabled = false;
    if (!wifiPass.disabled) wifiPass.focus();
  } else {
    selectedNetworkData = null;
    signalIndicator.style.display = 'none';
    wifiPass.disabled = true; wifiPass.value = '';
    connectBtn.disabled = true;
  }
}

async function connectNetwork() {
  const wifiPass = document.getElementById('wifiPass');
  const connectBtn = document.getElementById('connectBtn');
  if (!selectedNetworkData) { alert('Please select a network'); return; }
  if ((selectedNetworkData.security !== 'Open') && !wifiPass.value.trim()) { alert('Please enter the network password'); wifiPass.focus(); return; }

  connectBtn.classList.add('loading'); connectBtn.textContent = 'Connecting...'; connectBtn.disabled = true;
  
  try {
    const resp = await apiPost('/api/wifi/connect', { ssid: selectedNetworkData.ssid, password: wifiPass.value });
    
    if (resp.success) {
      alert(`Successfully connected to ${resp.ssid}\nIP Address: ${resp.ip}\nSignal Strength: ${resp.rssi} dBm`);
      // Clear the form after successful connection
      document.getElementById('networkSelect').value = '';
      document.getElementById('wifiPass').value = '';
      document.getElementById('signalIndicator').style.display = 'none';
      document.getElementById('connectBtn').disabled = true;
      selectedNetworkData = null;
    } else {
      alert('Connection failed: ' + (resp.error || 'Unknown error'));
    }
  } catch(e) {
    alert('Connect failed: ' + e.message);
  } finally {
    connectBtn.classList.remove('loading'); connectBtn.textContent = 'Connect Network'; connectBtn.disabled = false;
    await refreshStatus();
  }
}

async function disconnectNetwork() {
  if (!confirm('Disconnect from current WiFi network?')) return;
  const disconnectBtn = document.getElementById('disconnectBtn');
  disconnectBtn.classList.add('loading'); disconnectBtn.textContent = 'Disconnecting...'; disconnectBtn.disabled = true;
  try {
    await apiPost('/api/wifi/disconnect', {});
    alert('Disconnected from WiFi network.');
  } catch (e) {
    alert('Disconnect failed: ' + e.message);
  } finally {
    disconnectBtn.classList.remove('loading'); disconnectBtn.textContent = 'Disconnect'; disconnectBtn.disabled = false;
    await refreshStatus();
    document.getElementById('networkSelect').value = '';
    document.getElementById('wifiPass').value = '';
    document.getElementById('signalIndicator').style.display = 'none';
    document.getElementById('connectBtn').disabled = true;
    selectedNetworkData = null;
  }
}

// ---------- GSM save/load ----------
async function loadGsm() {
  try {
    const d = await apiGet('/api/load/gsm');
    // GSM configuration is now handled automatically
  } catch (e) { console.warn('GSM load failed', e); }
}

function startGsmPoll() { 
  stopGsmPoll(); 
  fetchSignalStrength(); 
  gsmPoll = setInterval(fetchSignalStrength, 30000); // Reduced to 30 seconds instead of 5 seconds
}

function stopGsmPoll() { 
  if (gsmPoll) { 
    clearInterval(gsmPoll); 
    gsmPoll = null; 
  } 
}

// ---------- GSM Testing Functions ----------
async function fetchSignalStrength(forceRefresh = false) {
  try {
    const url = forceRefresh ? '/api/gsm/signal?force=true' : '/api/gsm/signal';
    const d = await apiGet(url);
    const signalDb = d.dbm || -999;
    const lastUpdate = d.lastUpdate || 0;
    const nextUpdate = d.nextUpdate || 0;
    const isCached = d.raw && d.raw.includes('cached');
    const isForceRefresh = d.forceRefresh || false;
    
    document.getElementById('signalStrength').textContent = signalDb + ' dBm';
    
    // Show cache status
    if (isForceRefresh) {
      document.getElementById('signalStrength').textContent += ' (Fresh data)';
    } else if (isCached && nextUpdate > 0) {
      const timeUntilUpdate = Math.max(0, Math.floor((nextUpdate - Date.now()) / 1000));
      const minutes = Math.floor(timeUntilUpdate / 60);
      const seconds = timeUntilUpdate % 60;
      document.getElementById('signalStrength').textContent += ` (Next update: ${minutes}:${seconds.toString().padStart(2, '0')})`;
    }
  } catch (e) {
    document.getElementById('signalStrength').textContent = '— dBm';
    console.warn('Signal strength error', e);
  }
}

async function detectNetwork(forceRefresh = false) {
  const btn = document.getElementById('detectNetworkBtn');
  btn.classList.add('loading');
  btn.textContent = 'Detecting...';
  btn.disabled = true;
  
  try {
    const url = forceRefresh ? '/api/gsm/network?force=true' : '/api/gsm/network';
    const d = await apiGet(url);
    document.getElementById('carrierName').textContent = d.carrierName || '—';
    document.getElementById('networkMode').textContent = d.networkMode || '—';
    document.getElementById('registrationStatus').textContent = d.isRegistered ? 'Registered' : 'Not Registered';
    document.getElementById('registrationStatus').className = d.isRegistered ? 'status-value status-connected' : 'status-value status-disconnected';
    
    const isForceRefresh = d.forceRefresh || false;
    const cacheStatus = isForceRefresh ? ' (Fresh data)' : ' (Cached data)';
    
    alert('Network detection completed:\nCarrier: ' + (d.carrierName || 'Unknown') + 
          '\nMode: ' + (d.networkMode || 'Unknown') + 
          '\nRegistered: ' + (d.isRegistered ? 'Yes' : 'No') +
          '\nData: ' + cacheStatus);
  } catch (e) {
    alert('Network detection failed: ' + e.message);
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Detect Network Info';
    btn.disabled = false;
  }
}

async function testSMS() {
  const phoneNumber = document.getElementById('testPhoneNumber').value;
  if (!phoneNumber) {
    alert('Please enter a test phone number');
    return;
  }
  
  const btn = document.getElementById('smsTestBtn');
  btn.classList.add('loading');
  btn.textContent = 'Sending SMS...';
  btn.disabled = true;
  
  try {
    const result = await apiPost('/api/gsm/sms', {
      phoneNumber: phoneNumber,
      message: 'Test SMS from ESP32 Configuration Panel - ' + new Date().toLocaleString()
    });
    
    if (result.success) {
      alert('SMS sent successfully to ' + phoneNumber);
    } else {
      alert('SMS failed: ' + (result.error || 'Unknown error'));
    }
  } catch (e) {
    alert('SMS test failed: ' + e.message);
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Send Test SMS';
    btn.disabled = false;
  }
}

async function testCall() {
  const phoneNumber = document.getElementById('testPhoneNumber').value;
  if (!phoneNumber) {
    alert('Please enter a test phone number');
    return;
  }
  
  const btn = document.getElementById('callTestBtn');
  btn.classList.add('loading');
  btn.textContent = 'Making Call...';
  btn.disabled = true;
  
  try {
    const result = await apiPost('/api/gsm/call', {
      phoneNumber: phoneNumber
    });
    
    if (result.success) {
      alert('Call initiated successfully to ' + phoneNumber + '\nCall will be active for 10 seconds');
    } else {
      alert('Call failed: ' + (result.error || 'Unknown error'));
    }
  } catch (e) {
    alert('Call test failed: ' + e.message);
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Make Test Call';
    btn.disabled = false;
  }
}


async function refreshGSMStatus() {
  const btn = document.getElementById('refreshGSMBtn');
  btn.classList.add('loading');
  btn.textContent = 'Refreshing...';
  btn.disabled = true;
  
  try {
    // Force refresh both signal and network data
    await fetchSignalStrength(true);  // Force refresh signal
    await detectNetwork(true);        // Force refresh network info
  } catch (e) {
    console.warn('GSM refresh error', e);
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Refresh GSM Status';
    btn.disabled = false;
  }
}

// ---------- USER save/load ----------
 async function loadUser() {
   try {
     const d = await apiGet('/api/load/user');
     document.getElementById('userName').value   = d.name  || '';
     document.getElementById('userEmail').value  = d.email || '';
     document.getElementById('userContact').value= d.phone || '';
     
     // Update saved user data display
     document.getElementById('savedUserName').textContent = d.name || '—';
     document.getElementById('savedUserEmail').textContent = d.email || '—';
     document.getElementById('savedUserPhone').textContent = d.phone || '—';
     
     // Don't auto-populate test phone number - let user enter it manually
   } catch (e) { console.warn('User load failed', e); }
 }

async function saveUser() {
  try {
    const name = document.getElementById('userName').value;
    const email = document.getElementById('userEmail').value;
    const phone = document.getElementById('userContact').value;
    
    await apiPost('/api/save/user', {
      name: name,
      email: email,
      phone: phone
    });
    
    // Update saved user data display with new values
    document.getElementById('savedUserName').textContent = name || '—';
    document.getElementById('savedUserEmail').textContent = email || '—';
    document.getElementById('savedUserPhone').textContent = phone || '—';
    
    // Don't auto-populate test phone number - let user enter it manually
    
    alert('Profile saved.');
  } catch (e) { alert('Save failed: ' + e.message); }
}

// ---------- Wire up ----------
document.addEventListener('DOMContentLoaded', function() {
  console.log('Dashboard loaded');

  // Expose
  window.scanWifi = scanWifi;
  window.selectNetwork = selectNetwork;
  window.connectNetwork = connectNetwork;
  window.disconnectNetwork = disconnectNetwork;
  window.togglePassword = togglePassword;
  window.switchMode = switchMode;
  window.showTab = showTab;
  window.testSMS = testSMS;
  window.testCall = testCall;
  window.detectNetwork = detectNetwork;
  window.refreshGSMStatus = refreshGSMStatus;

  // GSM testing functions are now handled by onclick attributes

  // USER buttons
  const userCard = document.querySelector('#user .card:nth-child(2)'); // Updated to second card after reordering
  if (userCard) {
    const [saveBtn, clearBtn] = userCard.querySelectorAll('.button-row button');
    if (saveBtn)  saveBtn.addEventListener('click', saveUser);
     if (clearBtn) clearBtn.addEventListener('click', () => {
       document.getElementById('userName').value='';
       document.getElementById('userEmail').value='';
       document.getElementById('userContact').value='';
     });
  }

  // Initialize mode toggle
  switchMode('wifi'); // Start in WiFi mode

  // Initial loads
  refreshStatus();
  loadGsm();
  loadUser();

  // Refresh WiFi status every 10s
  setInterval(refreshStatus, 10000);
});
  </script>
</body>
</html>

)rawliteral";

/**
 * @brief Length of the dashboard HTML string
 * 
 * This constant provides the exact length of the embedded HTML string.
 * It's calculated as sizeof(dashboard_html) - 1 to exclude the null terminator.
 * This value is used by the web server to properly serve the HTML content.
 */
const size_t dashboard_html_len = sizeof(dashboard_html) - 1;

#endif // DASHBOARD_HTML_H

