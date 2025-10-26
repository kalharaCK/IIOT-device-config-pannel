#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H
#include <Arduino.h>

const char dashboard_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 IoT Configuration Panel</title>
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
.status-error { color: #ef4444; }
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
.input-with-button { 
  display: grid; 
  grid-template-columns: 1fr auto; 
  gap: 12px; 
  align-items: end; 
}
input, select, textarea { 
  width: 100%; 
  padding: 12px 16px; 
  border: 1px solid #475569; 
  border-radius: 6px;
  background: #334155; 
  color: #ffffff; 
  font-size: 0.875rem; 
  transition: all 0.2s ease; 
}
input:focus, select:focus, textarea:focus { 
  outline: none; 
  border-color: #3b82f6; 
  box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1); 
}
input:hover, select:hover, textarea:hover { 
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
.btn-success { 
  background: #10b981; 
  color: #ffffff; 
}
.btn-success:hover:not(:disabled) { 
  background: #059669; 
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1); 
}
.btn-info { 
  background: #0ea5e9; 
  color: #ffffff; 
}
.btn-info:hover:not(:disabled) { 
  background: #0284c7; 
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1); 
}
.btn-full { flex: 1; }
.btn-auto { width: auto; }
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
.message { 
  padding: 12px 16px; 
  border-radius: 6px; 
  margin: 16px 0; 
  font-size: 0.875rem; 
}
.message.success { 
  background: rgba(16, 185, 129, 0.2); 
  color: #10b981; 
  border: 1px solid rgba(16, 185, 129, 0.3); 
}
.message.error { 
  background: rgba(239, 68, 68, 0.2); 
  color: #ef4444; 
  border: 1px solid rgba(239, 68, 68, 0.3); 
}
.message.warning { 
  background: rgba(245, 158, 11, 0.2); 
  color: #f59e0b; 
  border: 1px solid rgba(245, 158, 11, 0.3); 
}
.message.info { 
  background: rgba(14, 165, 233, 0.2); 
  color: #0ea5e9; 
  border: 1px solid rgba(14, 165, 233, 0.3); 
}
.email-status { 
  margin-top: 8px; 
  font-size: 0.75rem; 
  padding: 8px 12px; 
  border-radius: 4px; 
}
.email-status.success { 
  background: rgba(16, 185, 129, 0.2); 
  color: #10b981; 
  border: 1px solid rgba(16, 185, 129, 0.3); 
}
.email-status.error { 
  background: rgba(239, 68, 68, 0.2); 
  color: #ef4444; 
  border: 1px solid rgba(239, 68, 68, 0.3); 
}
.email-status.info { 
  background: rgba(14, 165, 233, 0.2); 
  color: #0ea5e9; 
  border: 1px solid rgba(14, 165, 233, 0.3); 
}
/* Toggle Switch Styles */
.toggle-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 0;
  border-bottom: 1px solid #475569;
}
.toggle-row:last-child {
  border-bottom: none;
}
.toggle-label {
  color: #cbd5e1;
  font-weight: 500;
  font-size: 0.875rem;
}
.toggle-switch {
  width: 50px;
  height: 24px;
  background: #475569;
  border-radius: 12px;
  position: relative;
  cursor: pointer;
  transition: background 0.3s ease;
}
.toggle-switch.active {
  background: #3b82f6;
}
.toggle-slider {
  width: 20px;
  height: 20px;
  background: white;
  border-radius: 50%;
  position: absolute;
  top: 2px;
  left: 2px;
  transition: transform 0.3s ease;
  box-shadow: 0 2px 4px rgba(0,0,0,0.2);
}
.toggle-switch.active .toggle-slider {
  transform: translateX(26px);
}

@media (max-width: 768px) {
  .container { padding: 20px; }
  .content-grid { grid-template-columns: 1fr; }
  .tabs { flex-direction: column; }
  .button-row { flex-direction: column; }
  .input-row { grid-template-columns: 1fr; }
  .input-with-button { grid-template-columns: 1fr; }
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
      <div style="display: flex; justify-content: space-between; align-items: flex-start;">
        <div>
          <h1>ESP32 IoT Configuration Panel</h1>
          <p>Advanced IoT Monitoring & Configuration System v2.2.0</p>
        </div>
        <button class="btn-danger" onclick="exitDashboard()" style="padding: 8px 16px; font-size: 0.8rem; margin-left: 16px;">Exit Dashboard</button>
      </div>
    </div>

    <!-- Tabs -->
    <div class="tabs" id="tabContainer">
      <button class="tab-btn active" onclick="showTab('user')">User Management</button>
      <button class="tab-btn" onclick="showTab('wifi')" id="wifiTabBtn">WiFi Configuration</button>
      <button class="tab-btn" onclick="showTab('gsm')" id="gsmTabBtn">GSM Settings</button>
      <button class="tab-btn" onclick="showTab('device')" id="deviceTabBtn">Device Dashboard</button>
    </div>

    <!-- User Tab -->
    <div id="user" class="tab-content active">
      <div class="content-grid">
        <div class="card">
          <h3>📊 System Information</h3>
          <div class="status-row">
            <span class="status-label">Device Model</span>
            <span class="status-value" id="deviceModel">Loading...</span>
          </div>
          <div class="status-row">
            <span class="status-label">Firmware Version</span>
            <span class="status-value" id="firmwareVersion">Loading...</span>
          </div>
          <div class="status-row">
            <span class="status-label">Last Updated</span>
            <span class="status-value" id="lastUpdated">Loading...</span>
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
          <h3>👤 User Profile</h3>

          <div class="form-group">
            <label for="userName">Full Name</label>
            <input type="text" id="userName" placeholder="Enter your full name">
          </div>

          <div class="form-group">
            <label for="userEmail">Email Address</label>
            <div class="input-with-button">
              <input type="email" id="userEmail" placeholder="your.email@example.com">
            </div>
            <div id="emailStatus" class="email-status" style="display: none;"></div>
          </div>

          <div class="form-group">
            <label for="userContact">Contact Number</label>
            <input type="tel" id="userContact" placeholder="+94 XX XXXXXXX">
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-primary btn-full" onclick="saveUser()">Save Profile</button>
              <button class="btn-danger btn-full" onclick="clearUserForm()">Clear Form</button>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- WiFi Tab -->
    <div id="wifi" class="tab-content">
      <div class="content-grid">
        <div class="card">
          <h3>📡 Network Status</h3>
          <!--
          <div class="status-row">
            <span class="status-label">Access Point SSID</span>
            <span class="status-value" id="apSSID">ESP32-AccessPoint</span>
          </div>
          -->
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
          <h3>🔧 WiFi Configuration</h3>
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
          <div id="wifiMessage"></div>
        </div>
      </div>
    </div>

    <!-- GSM Tab -->
    <div id="gsm" class="tab-content">
      <div class="content-grid">
        <div class="card">
          <h3>📶 GSM Status</h3>
          <div class="status-row">
            <span class="status-label">Connection Status</span>
            <span class="status-value" id="gsmConnectionStatus">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Signal Strength</span>
            <span class="status-value" id="signalStrength">—</span>
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
          <h3>🧪 GSM Testing</h3>
          <div class="form-group">
            <label for="testPhoneNumber">Test Phone Number</label>
            <input type="text" id="testPhoneNumber" placeholder="+94 XX XXXXXXX">
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
            <!--
            <div class="button-row">
              <button class="btn-danger btn-full" onclick="stopGsmProcesses()" id="stopGsmBtn">STOP THE PROCESS</button>
            </div>
            -->
          </div>
        </div>
      </div>
    </div>

    <!-- Device Dashboard Tab (Available in both WiFi and GSM modes) -->
    <div id="device" class="tab-content">
      <div class="content-grid">
        <!-- Environmental Sensors -->
        <div class="card">
          <h3>🌡️ Environmental Sensors</h3>
          <div class="status-row">
            <span class="status-label">Temperature</span>
            <span class="status-value" id="temperatureValue">--°C</span>
          </div>
          <div class="status-row">
            <span class="status-label">Humidity</span>
            <span class="status-value" id="humidityValue">--%</span>
          </div>
          <div class="status-row">
            <span class="status-label">Light Level</span>
            <span class="status-value" id="lightValue">-- lx</span>
          </div>
          <div class="status-row">
            <span class="status-label">Last Update</span>
            <span class="status-value" id="sensorLastUpdate">--</span>
          </div>
        </div>

        <!-- Automation Thresholds -->
        <div class="card">
          <h3>⚙️ Automation Thresholds</h3>
          <div class="form-group">
            <label for="tempMin">Min Temperature (°C)</label>
            <input type="number" id="tempMin" placeholder="e.g. 18" min="0" max="50">
          </div>
          <div class="form-group">
            <label for="tempMax">Max Temperature (°C)</label>
            <input type="number" id="tempMax" placeholder="e.g. 28" min="0" max="50">
          </div>
          <div class="form-group">
            <label for="humidityThreshold">Humidity Threshold (%)</label>
            <input type="number" id="humidityThreshold" placeholder="e.g. 70" min="0" max="100">
          </div>
          <div class="form-group">
            <label for="lightThreshold">Light Level Threshold (lx)</label>
            <input type="number" id="lightThreshold" placeholder="e.g. 1000" min="0" max="10000">
          </div>
          <button class="btn-primary" onclick="saveThresholds()">Save Thresholds</button>
          <div id="thresholdMessage"></div>
        </div>

        <!-- Automation Control -->
        <div class="card">
          <h3>🤖 Automation Control</h3>
          <div class="form-group">
            <div class="toggle-row">
              <span class="toggle-label">Auto Temperature Control</span>
              <div class="toggle-switch" id="tempToggle" onclick="toggleAutoTemperature()">
                <div class="toggle-slider"></div>
              </div>
            </div>
            <div class="toggle-row">
              <span class="toggle-label">Auto Humidity Control</span>
              <div class="toggle-switch" id="humidityToggle" onclick="toggleAutoHumidity()">
                <div class="toggle-slider"></div>
              </div>
            </div>
            <div class="toggle-row">
              <span class="toggle-label">Auto Light Control</span>
              <div class="toggle-switch" id="lightToggle" onclick="toggleAutoLight()">
                <div class="toggle-slider"></div>
              </div>
            </div>
            <div class="toggle-row">
              <span class="toggle-label">Auto Irrigation System</span>
              <div class="toggle-switch" id="irrigationToggle" onclick="toggleAutoIrrigation()">
                <div class="toggle-slider"></div>
              </div>
            </div>
          </div>
          <div id="controlMessage"></div>
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

function showMessage(elementId, message, type = 'success') {
  const element = document.getElementById(elementId);
  element.textContent = message;
  element.className = `message ${type}`;
  element.style.display = 'block';
  setTimeout(() => {
    element.style.display = 'none';
  }, 5000);
}

function showEmailStatus(message, type = 'info') {
  const element = document.getElementById('emailStatus');
  element.textContent = message;
  element.className = `email-status ${type}`;
  element.style.display = 'block';
}

// Inline email validation for User Profile
function validateUserEmailInput() {
  const emailInput = document.getElementById('userEmail');
  const statusEl = document.getElementById('emailStatus');
  const value = (emailInput.value || '').trim();
  if (!value) {
    statusEl.style.display = 'none';
    return;
  }
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  if (emailRegex.test(value)) {
    showEmailStatus('Looks good. Email format is valid.', 'success');
  } else {
    showEmailStatus('Invalid email format. Example: name@example.com', 'error');
  }
}

// ---------- Mode Toggle ----------
function switchMode(mode) {
  currentMode = mode;
  
  // Update mode buttons
  document.querySelectorAll(".mode-btn").forEach(btn => btn.classList.remove("active"));
  document.getElementById(mode + 'ModeBtn').classList.add("active");
  
  // Show/hide tabs based on mode
  const wifiTabBtn = document.getElementById('wifiTabBtn');
  const deviceTabBtn = document.getElementById('deviceTabBtn');
  const gsmTabBtn = document.getElementById('gsmTabBtn');
  
  if (mode === 'wifi') {
    wifiTabBtn.classList.remove('hidden');
    deviceTabBtn.classList.remove('hidden');
    gsmTabBtn.classList.add('hidden');
    // Switch to WiFi tab if GSM was active
    if (document.getElementById('gsm').classList.contains('active')) {
      showTab('wifi');
    }
  } else if (mode === 'gsm') {
    wifiTabBtn.classList.add('hidden');
    deviceTabBtn.classList.remove('hidden');  // Show device dashboard in GSM mode
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
  if (currentMode === 'gsm' && tabId === 'wifi') return;  // Only WiFi tab is restricted in GSM mode
  
  document.querySelectorAll(".tab-content").forEach(tab => tab.classList.remove("active"));
  document.querySelectorAll(".tab-btn").forEach(btn => btn.classList.remove("active"));
  document.getElementById(tabId).classList.add("active");
  const activeBtn = document.querySelector(`.tabs .tab-btn[onclick="showTab('${tabId}')"]`);
  if (activeBtn) activeBtn.classList.add("active");

  if (tabId === 'gsm') startGsmPoll(); 
  else stopGsmPoll();
  
  // Update sensors when device tab is shown
  if (tabId === 'device') updateSensors();
}

// ---------- Password toggle ----------
function togglePassword() {
  const passField = document.getElementById("wifiPass");
  const toggleBtn = document.querySelector(".password-toggle");
  if (passField.type === "password") { 
    passField.type = "text"; 
    toggleBtn.textContent = "🙈"; 
  } else { 
    passField.type = "password"; 
    toggleBtn.textContent = "👁"; 
  }
}

// email password toggle removed with email tab

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
      staAddressEl.textContent = staIP; 
      staAddressEl.className = 'status-value status-connected';
    } else { 
      staAddressEl.textContent = 'Not connected'; 
      staAddressEl.className = 'status-value status-disconnected'; 
    }
    const connectedNetworkEl = document.getElementById('connectedNetwork');
    if (staConnected && staSSID) { 
      connectedNetworkEl.textContent = staSSID; 
      connectedNetworkEl.className = 'status-value status-connected'; 
    } else { 
      connectedNetworkEl.textContent = 'None'; 
      connectedNetworkEl.className = 'status-value status-disconnected'; 
    }
    const internetStatusEl = document.getElementById('internetStatus');
    if (staConnected) { 
      internetStatusEl.textContent = 'Connected'; 
      internetStatusEl.className = 'status-value status-connected'; 
    } else { 
      internetStatusEl.textContent = 'Not connected'; 
      internetStatusEl.className = 'status-value status-disconnected'; 
    }
    
    // email status removed (email tab moved to config.html)
  } catch (e) {
    console.error('Status refresh error:', e);
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

  button.classList.add('loading'); 
  button.textContent = 'Scanning...'; 
  button.disabled = true;
  networkSelect.disabled = true; 
  wifiPass.disabled = true; 
  connectBtn.disabled = true;
  wifiPass.value = ''; 
  signalIndicator.style.display = 'none';
  networkSelect.innerHTML = '<option value="">Scanning for networks...</option>';

  try {
    console.log('Starting WiFi scan...');
    
    // Start async scan
    const scanStart = await apiGet('/api/wifi/scan');
    console.log('Scan started:', scanStart);
    
    if (scanStart.status !== 'scanning') {
      throw new Error('Failed to start scan: ' + JSON.stringify(scanStart));
    }
    
    // Wait for scan to complete and get results
    let attempts = 0;
    let scanResult = null;
    
    while (attempts < 20) { // Wait up to 10 seconds (20 * 500ms)
      await new Promise(resolve => setTimeout(resolve, 500));
      
      try {
        scanResult = await apiGet('/api/wifi/scan/results');
        console.log('Scan results:', scanResult);
        break;
      } catch (e) {
        console.log(`Scan attempt ${attempts + 1}/20: ${e.message}`);
        attempts++;
      }
    }
    
    if (!scanResult) {
      throw new Error('Scan timed out or failed');
    }
    
    if (!Array.isArray(scanResult)) {
      console.error('Invalid scan response:', scanResult);
      throw new Error('Invalid scan response: ' + JSON.stringify(scanResult));
    }
    
    const best = {}; 
    scanResult.forEach(n => { 
      if (!n.ssid) return; 
      if (!best[n.ssid] || (n.rssi > best[n.ssid].rssi)) best[n.ssid] = n; 
    });
    
    availableNetworks = Object.values(best).sort((a,b)=>b.rssi - a.rssi);
    networkSelect.innerHTML = '<option value="">Select a network...</option>';
    
    if (availableNetworks.length === 0) {
      networkSelect.innerHTML = '<option value="">No networks found</option>';
      showMessage('wifiMessage', 'No networks found. Try moving closer to a router.', 'warning');
    } else {
      availableNetworks.forEach(n => {
        const opt = document.createElement('option');
        const sec = (n.security || (n.auth===0?'Open':'Secure'));
        const strength = n.strength || strengthFromRSSI(n.rssi || -80);
        opt.value = n.ssid; 
        opt.textContent = `${n.ssid} (${sec}, ${strength})`;
        opt.dataset.rssi = n.rssi || -80;
        opt.dataset.security = sec;
        networkSelect.appendChild(opt);
      });
      showMessage('wifiMessage', `Found ${availableNetworks.length} network(s)`, 'success');
    }
  } catch (e) {
    console.error('WiFi scan error:', e);
    showMessage('wifiMessage', 'Scan failed: ' + e.message, 'error');
    networkSelect.innerHTML = '<option value="">Scan failed - try again</option>';
  } finally {
    networkSelect.disabled = false;
    button.classList.remove('loading'); 
    button.textContent = 'Scan Networks'; 
    button.disabled = false;
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
    wifiPass.disabled = true; 
    wifiPass.value = '';
    connectBtn.disabled = true;
  }
}

async function connectNetwork() {
  const wifiPass = document.getElementById('wifiPass');
  const connectBtn = document.getElementById('connectBtn');
  if (!selectedNetworkData) { 
    showMessage('wifiMessage', 'Please select a network first', 'error');
    return; 
  }
  if ((selectedNetworkData.security !== 'Open') && !wifiPass.value.trim()) { 
    showMessage('wifiMessage', 'Please enter the network password', 'error');
    wifiPass.focus(); 
    return; 
  }

  connectBtn.classList.add('loading'); 
  connectBtn.textContent = 'Connecting...'; 
  connectBtn.disabled = true;
  
  try {
    const resp = await apiPost('/api/wifi/connect', { 
      ssid: selectedNetworkData.ssid, 
      password: wifiPass.value 
    });
    
    if (resp.success) {
      showMessage('wifiMessage', `Successfully connected to ${resp.ssid}`, 'success');
      // Clear the form after successful connection
      document.getElementById('networkSelect').value = '';
      document.getElementById('wifiPass').value = '';
      document.getElementById('signalIndicator').style.display = 'none';
      document.getElementById('connectBtn').disabled = true;
      selectedNetworkData = null;
      // Refresh status to show new connection
      setTimeout(() => refreshStatus(), 2000);
    } else {
      showMessage('wifiMessage', 'Connection failed: ' + (resp.error || 'Unknown error'), 'error');
    }
  } catch(e) {
    showMessage('wifiMessage', 'Connect failed: ' + e.message, 'error');
  } finally {
    connectBtn.classList.remove('loading'); 
    connectBtn.textContent = 'Connect Network'; 
    connectBtn.disabled = false;
  }
}

async function disconnectNetwork() {
  if (!confirm('Disconnect from current WiFi network?')) return;
  const disconnectBtn = document.getElementById('disconnectBtn');
  disconnectBtn.classList.add('loading'); 
  disconnectBtn.textContent = 'Disconnecting...'; 
  disconnectBtn.disabled = true;
  try {
    await apiPost('/api/wifi/disconnect', {});
    showMessage('wifiMessage', 'Disconnected from WiFi network', 'success');
  } catch (e) {
    showMessage('wifiMessage', 'Disconnect failed: ' + e.message, 'error');
  } finally {
    disconnectBtn.classList.remove('loading'); 
    disconnectBtn.textContent = 'Disconnect'; 
    disconnectBtn.disabled = false;
    await refreshStatus();
    document.getElementById('networkSelect').value = '';
    document.getElementById('wifiPass').value = '';
    document.getElementById('signalIndicator').style.display = 'none';
    document.getElementById('connectBtn').disabled = true;
    selectedNetworkData = null;
  }
}

// ---------- GSM Functions ----------
function startGsmPoll() { 
  stopGsmPoll(); 
  fetchSignalStrength(); 
  gsmPoll = setInterval(fetchSignalStrength, 30000);
}

function stopGsmPoll() { 
  if (gsmPoll) { 
    clearInterval(gsmPoll); 
    gsmPoll = null; 
  } 
}

// Stop ongoing GSM operations (polling and any loading states)
function stopGsmProcesses() {
  try {
    stopGsmPoll();
    // Re-enable and reset any GSM-related buttons currently loading
    const ids = ['smsTestBtn', 'callTestBtn', 'detectNetworkBtn', 'refreshGSMBtn'];
    ids.forEach(id => {
      const btn = document.getElementById(id);
      if (btn) {
        btn.classList.remove('loading');
        // Restore default labels
        if (id === 'smsTestBtn') btn.textContent = 'Send Test SMS';
        else if (id === 'callTestBtn') btn.textContent = 'Make Test Call';
        else if (id === 'detectNetworkBtn') btn.textContent = 'Detect Network Info';
        else if (id === 'refreshGSMBtn') btn.textContent = 'Refresh GSM Status';
        btn.disabled = false;
      }
    });
  } catch (e) {
    console.warn('Failed to stop GSM processes', e);
  }
}

async function fetchSignalStrength(forceRefresh = false) {
  try {
    const url = forceRefresh ? '/api/gsm/signal?force=true' : '/api/gsm/signal';
    const d = await apiGet(url);
    const signalDb = d.dbm || 0;
    const lastUpdate = d.lastUpdate || 0;
    const nextUpdate = d.nextUpdate || 0;
    const isCached = d.raw && d.raw.includes('cached');
    const isForceRefresh = d.forceRefresh || false;
    
    document.getElementById('signalStrength').textContent = signalDb + ' dBm';
    
    // Update GSM connection status
    const gsmStatusEl = document.getElementById('gsmConnectionStatus');
    if (signalDb !== -999) {
      gsmStatusEl.textContent = 'Active';
      gsmStatusEl.className = 'status-value status-connected';
    } else {
      gsmStatusEl.textContent = 'Inactive';
      gsmStatusEl.className = 'status-value status-disconnected';
    }
    
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
    document.getElementById('signalStrength').textContent = '—';
    document.getElementById('gsmConnectionStatus').textContent = 'Error';
    document.getElementById('gsmConnectionStatus').className = 'status-value status-error';
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
    await fetchSignalStrength(true);
    await detectNetwork(true);
  } catch (e) {
    console.warn('GSM refresh error', e);
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Refresh GSM Status';
    btn.disabled = false;
  }
}

// ---------- Email Functions ----------
async function loadEmailConfig() {
  try {
    const d = await apiGet('/api/load/email');
    document.getElementById('smtpHost').value = d.smtpHost || 'smtp.gmail.com';
    document.getElementById('smtpPort').value = d.smtpPort || 465;
    document.getElementById('emailAccountInput').value = d.emailAccount || '';
    document.getElementById('senderName').value = d.senderName || 'ESP32 Dashboard';
    // Note: Password is not returned for security
    
    // Update status display
    if (d.emailAccount) {
      document.getElementById('smtpServer').textContent = d.smtpHost || '—';
      document.getElementById('emailAccount').textContent = d.emailAccount || '—';
    }
  } catch (e) { 
    console.warn('Email config load failed', e); 
  }
}

async function saveEmailConfig() {
  const btn = event.target;
  btn.classList.add('loading');
  btn.textContent = 'Saving...';
  btn.disabled = true;
  
  try {
    const config = {
      smtpHost: document.getElementById('smtpHost').value,
      smtpPort: parseInt(document.getElementById('smtpPort').value),
      emailAccount: document.getElementById('emailAccountInput').value,
      emailPassword: document.getElementById('emailPassword').value,
      senderName: document.getElementById('senderName').value
    };
    
    if (!config.smtpHost || !config.emailAccount || !config.emailPassword) {
      alert('Please fill in all required fields: SMTP Host, Email Account, and App Password');
      return;
    }
    
    const result = await apiPost('/api/save/email', config);
    
    if (result.success) {
      showMessage('emailTestResult', 'Email configuration saved successfully!', 'success');
      // Update status display
      document.getElementById('smtpServer').textContent = config.smtpHost;
      document.getElementById('emailAccount').textContent = config.emailAccount;
      document.getElementById('emailConfigStatus').textContent = 'Configured';
      document.getElementById('emailConfigStatus').className = 'status-value status-connected';
    } else {
      showMessage('emailTestResult', 'Failed to save email configuration: ' + (result.message || 'Unknown error'), 'error');
    }
  } catch (e) {
    showMessage('emailTestResult', 'Save failed: ' + e.message, 'error');
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Save Email Settings';
    btn.disabled = false;
  }
}

async function sendTestEmail() {
  const btn = document.getElementById('sendTestEmailBtn');
  const recipient = document.getElementById('testRecipientEmail').value;
  const subject = document.getElementById('testEmailSubject').value;
  const content = document.getElementById('testEmailContent').value;
  
  if (!recipient) {
    alert('Please enter a recipient email address');
    return;
  }
  
  btn.classList.add('loading');
  btn.textContent = 'Sending...';
  btn.disabled = true;
  
  try {
    const result = await apiPost('/api/email/send', {
      to: recipient,
      subject: subject,
      content: content
    });
    
    if (result.success) {
      showMessage('emailTestResult', 'Test email sent successfully!', 'success');
      document.getElementById('lastEmailTest').textContent = new Date().toLocaleString();
    } else {
      showMessage('emailTestResult', 'Email sending failed: ' + (result.error || 'Unknown error'), 'error');
    }
  } catch (e) {
    showMessage('emailTestResult', 'Email test failed: ' + e.message, 'error');
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Send Test Email';
    btn.disabled = false;
  }
}

async function sendQuickTest() {
  const recipient = document.getElementById('testRecipientEmail').value;
  if (!recipient) {
    alert('Please enter a recipient email address for quick test');
    return;
  }
  
  // Use the legacy endpoint for quick testing
  try {
    const response = await fetch(`/sendDummyEmail?to=${encodeURIComponent(recipient)}`);
    const result = await response.json();
    showMessage('emailTestResult', result.message, result.message.includes('✅') ? 'success' : 'error');
    if (result.message.includes('✅')) {
      document.getElementById('lastEmailTest').textContent = new Date().toLocaleString();
    }
  } catch (e) {
    showMessage('emailTestResult', 'Quick test failed: ' + e.message, 'error');
  }
}

async function checkEmailConfiguration() {
  const btn = document.getElementById('checkConfigBtn');
  btn.classList.add('loading');
  btn.textContent = 'Checking...';
  btn.disabled = true;
  
  try {
    const config = {
      smtpHost: document.getElementById('smtpHost').value,
      smtpPort: parseInt(document.getElementById('smtpPort').value),
      emailAccount: document.getElementById('emailAccountInput').value,
      emailPassword: document.getElementById('emailPassword').value
    };
    
    if (!config.smtpHost || !config.emailAccount || !config.emailPassword) {
      showMessage('emailTestResult', 'Please fill in all required email configuration fields', 'warning');
      return;
    }
    
    // Test the configuration by attempting to send a test email
    const testRecipient = config.emailAccount; // Send to self for testing
    const result = await apiPost('/api/email/send', {
      to: testRecipient,
      subject: 'Email Configuration Test',
      content: 'This is a test email to verify your SMTP configuration. If you receive this, your email settings are correct!'
    });
    
    if (result.success) {
      showMessage('emailTestResult', 'Email configuration test passed! Configuration is working correctly.', 'success');
      document.getElementById('lastEmailTest').textContent = new Date().toLocaleString();
    } else {
      showMessage('emailTestResult', 'Email configuration test failed: ' + (result.error || 'Unknown error. Check your SMTP settings and credentials.'), 'error');
    }
  } catch (e) {
    showMessage('emailTestResult', 'Configuration check failed: ' + e.message, 'error');
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Check Configuration';
    btn.disabled = false;
  }
}

async function checkEmail() {
  const email = document.getElementById('userEmail').value;
  const btn = document.getElementById('checkEmailBtn');
  
  if (!email) {
    showEmailStatus('Please enter an email address first', 'error');
    return;
  }
  
  // Basic email validation
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  if (!emailRegex.test(email)) {
    showEmailStatus('Please enter a valid email address', 'error');
    return;
  }
  
  btn.classList.add('loading');
  btn.textContent = 'Checking...';
  btn.disabled = true;
  
  try {
    // Use the quick test endpoint to check if email can be sent
    const response = await fetch(`/sendDummyEmail?to=${encodeURIComponent(email)}`);
    const result = await response.json();
    
    if (result.message.includes('✅') || result.message.includes('successfully')) {
      showEmailStatus('Email configuration is working! Test email sent successfully.', 'success');
    } else {
      showEmailStatus('Email test failed: ' + result.message, 'error');
    }
  } catch (e) {
    showEmailStatus('Email check failed: ' + e.message, 'error');
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Check Email';
    btn.disabled = false;
  }
}

function clearEmailForm() {
  document.getElementById('emailPassword').value = '';
  document.getElementById('testRecipientEmail').value = '';
  document.getElementById('testEmailSubject').value = 'Test Email from ESP32 Dashboard';
  document.getElementById('testEmailContent').value = 'This is a test email sent from your ESP32 IoT Configuration Panel.';
  document.getElementById('emailTestResult').style.display = 'none';
}

// ---------- System Information Functions ----------
async function loadSystemInfo() {
  try {
    const d = await apiGet('/api/system/info');
    document.getElementById('deviceModel').textContent = d.deviceModel || '—';
    document.getElementById('firmwareVersion').textContent = d.firmwareVersion || '—';
    document.getElementById('lastUpdated').textContent = d.lastUpdated || '—';
  } catch (e) { 
    console.warn('System info load failed', e);
    document.getElementById('deviceModel').textContent = 'Error';
    document.getElementById('firmwareVersion').textContent = 'Error';
    document.getElementById('lastUpdated').textContent = 'Error';
  }
}
function exitDashboard() {
  if (confirm('Are you sure you want to exit the dashboard?')) {

    // Restart ESP32 first
    fetch('/api/restart')
      .then(() => {
        console.log("ESP32 restarting...");

        // Attempt to close page
        window.close();
        setTimeout(() => {
          window.location.href = 'about:blank';
        }, 500);
      })
      .catch(() => {
        alert("Failed to restart ESP32");
      });
  }
}



// ---------- User Functions ----------
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
  } catch (e) { 
    console.warn('User load failed', e); 
  }
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
    
    alert('Profile saved successfully!');
  } catch (e) { 
    alert('Save failed: ' + e.message); 
  }
}

function clearUserForm() {
  document.getElementById('userName').value = '';
  document.getElementById('userEmail').value = '';
  document.getElementById('userContact').value = '';
  document.getElementById('emailStatus').style.display = 'none';
}

// ---------- Initialize ----------
document.addEventListener('DOMContentLoaded', function() {
  console.log('Dashboard loaded');

  // Expose functions to global scope
  window.scanWifi = scanWifi;
  window.selectNetwork = selectNetwork;
  window.connectNetwork = connectNetwork;
  window.disconnectNetwork = disconnectNetwork;
  window.togglePassword = togglePassword;
  // email toggle removed
  window.switchMode = switchMode;
  window.showTab = showTab;
  // Device Dashboard controls
  window.saveThresholds = function() {
    const tmin = parseFloat(document.getElementById('tempMin')?.value || '');
    const tmax = parseFloat(document.getElementById('tempMax')?.value || '');
    const hth  = parseFloat(document.getElementById('humidityThreshold')?.value || '');
    const lth  = parseFloat(document.getElementById('lightThreshold')?.value || '');
    let msg = 'Saved:';
    if (!isNaN(tmin)) msg += ` Min ${tmin}°C`;
    if (!isNaN(tmax)) msg += ` Max ${tmax}°C`;
    if (!isNaN(hth)) msg += ` Hum ${hth}%`;
    if (!isNaN(lth)) msg += ` Light ${lth} lx`;
    showMessage('thresholdMessage', msg.trim() || 'No changes', 'success');
  };
  
  // Sensor data fetching and display
  window.updateSensors = async function() {
    try {
      const response = await fetch('/api/sensors');
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      const data = await response.json();
      
      // Update sensor displays
      document.getElementById('temperatureValue').textContent = `${data.temperature}°C`;
      document.getElementById('humidityValue').textContent = `${data.humidity}%`;
      document.getElementById('lightValue').textContent = `${data.light} lx`;
      document.getElementById('sensorLastUpdate').textContent = new Date().toLocaleTimeString();
      
      // Add visual indicators for sensor status
      updateSensorStatus('temperatureValue', data.temperature, 18, 32);
      updateSensorStatus('humidityValue', data.humidity, 30, 90);
      updateSensorStatus('lightValue', data.light, 0, 2000);
      
    } catch (error) {
      console.error('Sensor update error:', error);
      document.getElementById('temperatureValue').textContent = 'Error';
      document.getElementById('humidityValue').textContent = 'Error';
      document.getElementById('lightValue').textContent = 'Error';
      document.getElementById('sensorLastUpdate').textContent = 'Error';
    }
  };
  
  // Helper function to add visual status indicators
  window.updateSensorStatus = function(elementId, value, min, max) {
    const element = document.getElementById(elementId);
    const range = max - min;
    const normalized = (value - min) / range;
    
    // Remove existing status classes
    element.classList.remove('status-connected', 'status-warning', 'status-error');
    
    if (normalized < 0.2 || normalized > 0.8) {
      element.classList.add('status-error'); // Critical range
    } else if (normalized < 0.3 || normalized > 0.7) {
      element.classList.add('status-warning'); // Warning range
    } else {
      element.classList.add('status-connected'); // Normal range
    }
  };
  
  // Toggle functions with visual feedback
  window.toggleAutoTemperature = function(){
    const toggle = document.getElementById('tempToggle');
    toggle.classList.toggle('active');
    const isActive = toggle.classList.contains('active');
    showMessage('controlMessage', `Auto Temperature Control: ${isActive ? 'ON' : 'OFF'}`, 'success');
  };
  window.toggleAutoHumidity = function(){
    const toggle = document.getElementById('humidityToggle');
    toggle.classList.toggle('active');
    const isActive = toggle.classList.contains('active');
    showMessage('controlMessage', `Auto Humidity Control: ${isActive ? 'ON' : 'OFF'}`, 'success');
  };
  window.toggleAutoLight = function(){
    const toggle = document.getElementById('lightToggle');
    toggle.classList.toggle('active');
    const isActive = toggle.classList.contains('active');
    showMessage('controlMessage', `Auto Light Control: ${isActive ? 'ON' : 'OFF'}`, 'success');
  };
  window.toggleAutoIrrigation = function(){
    const toggle = document.getElementById('irrigationToggle');
    toggle.classList.toggle('active');
    const isActive = toggle.classList.contains('active');
    showMessage('controlMessage', `Auto Irrigation System: ${isActive ? 'ON' : 'OFF'}`, 'success');
  };
  window.testSMS = testSMS;
  window.testCall = testCall;
  window.detectNetwork = detectNetwork;
  window.refreshGSMStatus = refreshGSMStatus;
  window.stopGsmProcesses = stopGsmProcesses;
  window.saveUser = saveUser;
  window.clearUserForm = clearUserForm;
  window.checkEmail = checkEmail;
  window.updateSensors = updateSensors;
  // email helpers removed

  // Initialize mode toggle
  switchMode('wifi'); // Start in WiFi mode

  // Initial loads
  refreshStatus();
  loadSystemInfo();  // Load system information
  // loadGsm removed (no implementation present)
  loadUser();
  // email config load removed
  
  // Initial sensor update
  updateSensors();

  // Refresh status every 10s
  setInterval(refreshStatus, 10000);
  
  // Update sensors every 3 seconds
  setInterval(updateSensors, 3000);

  // Live email validation
  const emailInputEl = document.getElementById('userEmail');
  if (emailInputEl) {
    emailInputEl.addEventListener('input', validateUserEmailInput);
  }
});
  </script>
</body>
</html>
</html>
</html>

)rawliteral";

const size_t dashboard_html_len = sizeof(dashboard_html) - 1;

#endif // DASHBOARD_HTML_H
