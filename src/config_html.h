/**
 * @file config_html.h
 * @brief Configuration Mode Web Interface for ESP32 IoT Configuration Panel
 * @author IoT Device Dashboard Project
 * @version 2.0.0
 * @date 2025-01-30
 * 
 * @details
 * This file contains the configuration mode web interface that is displayed
 * when the DRD (Device Reset Detection) system detects 2 button presses
 * within a 5-second interval. This interface provides a simplified
 * configuration experience for device setup and recovery.
 * 
 * @section features Configuration Mode Features
 * - Modern FOTA Dashboard UI design
 * - Firmware and Access Point configuration
 * - Real-time status monitoring
 * - Export/Import settings functionality
 * - Mobile-friendly responsive design
 * 
 * @section design Design Features
 * - Clean, minimal interface focused on configuration
 * - Dark theme with professional styling
 * - Tab-based organization for different settings
 * - Real-time validation and feedback
 * - Responsive design for mobile devices
 */

#ifndef CONFIG_HTML_H
#define CONFIG_HTML_H

/**
 * @brief Configuration mode web interface HTML/CSS/JavaScript
 * 
 * This PROGMEM string contains the configuration mode interface including:
 * - HTML structure optimized for configuration tasks
 * - CSS styling with modern dark theme
 * - JavaScript functionality for configuration management
 * - Firmware and Access Point configuration
 * - Settings export/import capabilities
 * - Real-time validation and feedback
 */
const char config_html[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>FOTA Dashboard</title>
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
.header { text-align: center; margin-bottom: 24px; }
.header h1 {
  font-size: 1.75rem;
  font-weight: 600;
  color: #ffffff;
  margin-bottom: 6px;
  letter-spacing: -0.02em;
}
.header p { color: #94a3b8; font-size: 0.9rem; }

.tabs {
  display: flex;
  justify-content: center;
  margin-bottom: 24px;
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
.tab-btn:hover { color: #ffffff; background: #475569; }
.tab-btn.active { background: #3b82f6; color: #ffffff; font-weight: 500; }

.tab-content { display: none; }
.tab-content.active { display: block; }

.content-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
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
  margin-bottom: 16px;
  display: flex;
  align-items: center;
  gap: 8px;
}
.status-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 10px 0;
  border-bottom: 1px solid #475569;
}
.status-row:last-child { border-bottom: none; }
.status-label { color: #94a3b8; font-size: 0.85rem; }
.status-value { color: #ffffff; font-weight: 500; font-size: 0.9rem; }
.status-connected { color: #22d3ee; }
.status-warning { color: #fbbf24; }
.status-error { color: #ef4444; }

.status-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 16px;
  margin-top: 16px;
}

.status-card {
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 8px;
  padding: 16px;
}

.status-card h4 {
  margin: 0 0 12px 0;
  color: #4CAF50;
  font-size: 14px;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.system-info-section {
  margin-top: 20px;
}

.system-info-section h3 {
  margin-bottom: 20px;
  color: #fff;
  font-size: 18px;
}

.form-group { margin-bottom: 16px; }
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
input, textarea {
  width: 100%;
  padding: 12px 16px;
  border: 1px solid #475569;
  border-radius: 6px;
  background: #334155;
  color: #ffffff;
  font-size: 0.9rem;
  transition: all 0.2s ease;
}
input:focus, textarea:focus {
  outline: none;
  border-color: #3b82f6;
  box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
}
input:hover, textarea:hover { border-color: #64748b; }

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
  padding: 6px 8px;
  border-radius: 4px;
  transition: all 0.2s ease;
}
.password-toggle:hover { color: #ffffff; background: #475569; }

.button-group { display: flex; gap: 12px; margin-top: 16px; flex-wrap: wrap; }
.button-row { display: flex; gap: 12px; width: 100%; }
button {
  padding: 12px 16px;
  border: none;
  border-radius: 6px;
  cursor: pointer;
  font-weight: 500;
  font-size: 0.9rem;
  transition: all 0.2s ease;
  position: relative;
  overflow: hidden;
  min-height: 40px;
}
button:hover { transform: translateY(-1px); }
button:active { transform: translateY(0); }
button:disabled { opacity: 0.5; cursor: not-allowed; transform: none; }
.btn-primary { background: #3b82f6; color: #ffffff; }
.btn-primary:hover:not(:disabled) { background: #2563eb; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.1); }
.btn-secondary { background: #475569; color: #ffffff; border: 1px solid #64748b; }
.btn-secondary:hover:not(:disabled) { background: #64748b; border-color: #94a3b8; }
.btn-success { background: #10b981; color: #ffffff; }
.btn-success:hover:not(:disabled) { background: #059669; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.1); }
.btn-danger { background: #ef4444; color: #ffffff; }
.btn-danger:hover:not(:disabled) { background: #dc2626; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.1); }
.btn-info { background: #0ea5e9; color: #ffffff; }
.btn-info:hover:not(:disabled) { background: #0284c7; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.1); }
.btn-full { flex: 1; }
.btn-auto { width: auto; }

.message {
  padding: 12px 16px;
  border-radius: 6px;
  margin: 12px 0;
  font-size: 0.875rem;
  display: none;
}
.message.success { background: rgba(16,185,129,0.2); color: #10b981; border: 1px solid rgba(16,185,129,0.3); }
.message.error { background: rgba(239,68,68,0.2); color: #ef4444; border: 1px solid rgba(239,68,68,0.3); }
.message.info { background: rgba(14,165,233,0.2); color: #0ea5e9; border: 1px solid rgba(14,165,233,0.3); }
.muted { color: #94a3b8; font-size: 0.8rem; }

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
    <div class="header">
      <h1>FOTA Dashboard</h1>
      <p>Firmware & Access-Point Configuration</p>
    </div>

    <!-- Tabs -->
    <div class="tabs">
      <button class="tab-btn active" onclick="showTab('fwTab', event)">Firmware / Repo</button>
      <button class="tab-btn" onclick="showTab('apTab', event)">Access-Point (AP)</button>
      <button class="tab-btn" onclick="showTab('systemTab', event)">System Info</button>
    </div>

    <!-- Firmware / Repo Tab -->
    <div id="fwTab" class="tab-content active">
      <div class="content-grid">
        <div class="card">
          <h3>📦 Firmware / Repository</h3>
          <div class="form-group">
            <label for="repoPath">Repository Path</label>
            <input type="text" id="repoPath" placeholder="e.g., C:/projects/device-firmware">
          </div>
          <div class="form-group">
            <label for="fwUrl">Firmware URL</label>
            <input type="url" id="fwUrl" placeholder="https://example.com/firmware.bin">
            <div class="muted" id="fwUrlHint">URL must start with http or https</div>
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-primary btn-full" onclick="saveFirmware()">Save</button>
              <button class="btn-success btn-full" onclick="applyFirmware()" id="fwApplyBtn">Apply</button>
            </div>
            <div class="button-row">
              <button class="btn-secondary btn-full" onclick="resetFirmware()">Reset</button>
              <button class="btn-info btn-full" onclick="copySettings()">Copy to Clipboard</button>
            </div>
            <div class="button-row">
              <button class="btn-secondary btn-full" onclick="exportSettings()">Export JSON</button>
              <button class="btn-secondary btn-full" onclick="importSettingsTrigger()">Import JSON</button>
              <input type="file" id="importFile" accept="application/json" style="display:none" onchange="importSettings(event)">
            </div>
          </div>

          <div id="fwMessage" class="message"></div>

          <div class="status-row" style="margin-top: 8px;">
            <span class="status-label">Last Saved</span>
            <span class="status-value" id="fwLastSaved">Never</span>
          </div>
          <div class="status-row">
            <span class="status-label">Saved Repo Path</span>
            <span class="status-value" id="fwSavedRepo">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Saved Firmware URL</span>
            <span class="status-value" id="fwSavedUrl">—</span>
          </div>
        </div>
      </div>
    </div>

    <!-- Access-Point Tab -->
    <div id="apTab" class="tab-content">
      <div class="content-grid">
        <div class="card">
          <h3>📡 Access-Point (AP)</h3>
          <div class="form-group">
            <label for="apSsid">SSID (max 32 chars)</label>
            <input type="text" id="apSsid" placeholder="ESP32-AP">
          </div>
          <div class="form-group">
            <label for="apPass">Password (8–63 chars, leave empty for open)</label>
            <div class="password-input">
              <input type="password" id="apPass" placeholder="Enter AP password (optional)">
              <button type="button" class="password-toggle" onclick="toggleApPassword()">👁</button>
            </div>
            <div class="muted" id="apSecurityHint">AP is <span id="apSecState" class="status-warning">Open</span></div>
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-primary btn-full" onclick="saveAp()">Save</button>
              <button class="btn-success btn-full" onclick="applyAp()" id="apApplyBtn">Apply</button>
            </div>
            <div class="button-row">
              <button class="btn-secondary btn-full" onclick="resetAp()">Reset</button>
              <button class="btn-info btn-full" onclick="copySettings()">Copy to Clipboard</button>
            </div>
          </div>

          <div id="apMessage" class="message"></div>

          <div class="status-row" style="margin-top: 8px;">
            <span class="status-label">Current AP Status</span>
            <span class="status-value" id="currentApStatus">Loading...</span>
          </div>
          <div class="status-row">
            <span class="status-label">Current SSID</span>
            <span class="status-value" id="currentApSsid">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Current Security</span>
            <span class="status-value" id="currentApSec">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Last Saved</span>
            <span class="status-value" id="apLastSaved">Never</span>
          </div>
        </div>
      </div>
    </div>

    <div id="systemTab" class="tab-content">
      <div class="content-grid">
        <div class="card">
          <h3>📊 System Information</h3>
          
          <div class="system-info-section">
            <div class="status-grid">
              <div class="status-card">
                <h4>Device Status</h4>
                <div class="status-row">
                  <span class="status-label">System Status</span>
                  <span class="status-value" id="systemStatus">Loading...</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Uptime</span>
                  <span class="status-value" id="systemUptime">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Free Memory</span>
                  <span class="status-value" id="systemMemory">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Firmware Version</span>
                  <span class="status-value" id="firmwareVersion">—</span>
                </div>
              </div>

              <div class="status-card">
                <h4>WiFi Configuration</h4>
                <div class="status-row">
                  <span class="status-label">Access Point SSID</span>
                  <span class="status-value" id="sysApSsid">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">AP Security</span>
                  <span class="status-value" id="sysApSecurity">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Station SSID</span>
                  <span class="status-value" id="sysStaSsid">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Station Status</span>
                  <span class="status-value" id="sysStaStatus">—</span>
                </div>
              </div>

              <div class="status-card">
                <h4>User Profile</h4>
                <div class="status-row">
                  <span class="status-label">Name</span>
                  <span class="status-value" id="sysUserName">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Email</span>
                  <span class="status-value" id="sysUserEmail">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Phone</span>
                  <span class="status-value" id="sysUserPhone">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Profile Status</span>
                  <span class="status-value" id="sysUserStatus">—</span>
                </div>
              </div>

              <div class="status-card">
                <h4>Firmware Settings</h4>
                <div class="status-row">
                  <span class="status-label">Repository Path</span>
                  <span class="status-value" id="sysRepoPath">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Firmware URL</span>
                  <span class="status-value" id="sysFwUrl">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Last Updated</span>
                  <span class="status-value" id="sysFwLastSaved">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Settings Status</span>
                  <span class="status-value" id="sysFwStatus">—</span>
                </div>
              </div>

              <div class="status-card">
                <h4>Email Configuration</h4>
                <div class="status-row">
                  <span class="status-label">SMTP Host</span>
                  <span class="status-value" id="sysEmailHost">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">SMTP Port</span>
                  <span class="status-value" id="sysEmailPort">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Email Account</span>
                  <span class="status-value" id="sysEmailAccount">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">Email Status</span>
                  <span class="status-value" id="sysEmailStatus">—</span>
                </div>
              </div>

              <div class="status-card">
                <h4>GSM Configuration</h4>
                <div class="status-row">
                  <span class="status-label">Carrier</span>
                  <span class="status-value" id="sysGsmCarrier">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">APN</span>
                  <span class="status-value" id="sysGsmApn">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">APN User</span>
                  <span class="status-value" id="sysGsmUser">—</span>
                </div>
                <div class="status-row">
                  <span class="status-label">GSM Status</span>
                  <span class="status-value" id="sysGsmStatus">—</span>
                </div>
              </div>
            </div>

            <div class="button-group" style="margin-top: 20px;">
              <button onclick="refreshSystemInfo()" class="btn btn-primary">Refresh System Info</button>
              <button onclick="exportSystemInfo()" class="btn btn-secondary">Export System Report</button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <script>
// ---------- Storage Keys & Defaults ----------
const STORAGE_KEY = 'fotaSettings';
const defaultSettings = {
  firmware: { repoPath: '', fwUrl: '', lastSaved: null },
  ap: { ssid: '', password: '', lastSaved: null }
};

// ---------- Utilities ----------
function getSettings() {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return structuredClone(defaultSettings);
    const parsed = JSON.parse(raw);
    return { ...structuredClone(defaultSettings), ...parsed };
  } catch (e) {
    console.warn('Settings parse error, resetting to defaults', e);
    return structuredClone(defaultSettings);
  }
}

function setSettings(next) {
  localStorage.setItem(STORAGE_KEY, JSON.stringify(next));
}

function nowISO() {
  return new Date().toLocaleString();
}

function showMessage(id, text, type = 'success') {
  const el = document.getElementById(id);
  el.textContent = text;
  el.className = `message ${type}`;
  el.style.display = 'block';
  setTimeout(() => { el.style.display = 'none'; }, 4000);
}

function validateUrl(url) {
  if (!url) return false;
  try {
    const u = new URL(url);
    return u.protocol === 'http:' || u.protocol === 'https:';
  } catch { return false; }
}

// ---------- ESP32 API Communication ----------
async function apiGet(url) {
  try {
    const response = await fetch(url);
    if (!response.ok) {
      const errorText = await response.text();
      console.error('API GET Error:', url, response.status, errorText);
      throw new Error(`HTTP ${response.status}: ${errorText}`);
    }
    const jsonData = await response.json();
    console.log('API GET Success:', url, jsonData);
    return jsonData;
  } catch (error) {
    console.error('API GET Exception:', url, error);
    throw error;
  }
}

async function apiPost(url, bodyObj) {
  try {
    const response = await fetch(url, { 
      method: 'POST', 
      headers: {'Content-Type':'application/json'}, 
      body: JSON.stringify(bodyObj || {}) 
    });
    if (!response.ok) {
      const errorText = await response.text();
      console.error('API POST Error:', url, response.status, errorText);
      throw new Error(`HTTP ${response.status}: ${errorText}`);
    }
    const jsonData = await response.json();
    console.log('API POST Success:', url, jsonData);
    return jsonData;
  } catch (error) {
    console.error('API POST Exception:', url, error);
    throw error;
  }
}

function importSettingsTrigger() {
  document.getElementById('importFile').click();
}

function exportSettings() {
  const btn = document.getElementById('exportBtn');
  const originalText = btn.textContent;
  
  btn.disabled = true;
  btn.textContent = 'Exporting...';
  
  // Export settings to ESP32
  exportSettingsFromESP32(btn, originalText);
}

async function exportSettingsFromESP32(btn, originalText) {
  try {
    const result = await apiGet('/api/config/export');
    
    // Create downloadable JSON file
    const blob = new Blob([JSON.stringify(result, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `esp32-config-${new Date().toISOString().split('T')[0]}.json`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    
    showMessage('fwMessage', 'Configuration exported successfully', 'success');
  } catch (e) {
    showMessage('fwMessage', 'Export failed: ' + e.message, 'error');
  } finally {
    btn.disabled = false;
    btn.textContent = originalText;
  }
}

async function copySettings() {
  try {
    await navigator.clipboard.writeText(JSON.stringify(getSettings(), null, 2));
    showMessage('fwMessage', 'Settings copied to clipboard', 'success');
  } catch (e) {
    showMessage('fwMessage', 'Copy failed: ' + e.message, 'error');
  }
}

function importSettings(event) {
  const file = event.target.files && event.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = () => {
    try {
      const parsed = JSON.parse(reader.result);
      const merged = { ...structuredClone(defaultSettings), ...parsed };
      setSettings(merged);
      hydrateFromStorage();
      showMessage('fwMessage', 'Settings imported successfully', 'success');
    } catch (e) {
      showMessage('fwMessage', 'Import failed: invalid JSON', 'error');
    }
  };
  reader.readAsText(file);
  // reset input so same file can be re-imported if needed
  event.target.value = '';
}

// ---------- Tabs ----------
function showTab(id, ev) {
  document.querySelectorAll('.tab-content').forEach(t => t.classList.remove('active'));
  document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
  document.getElementById(id).classList.add('active');
  if (ev && ev.target) ev.target.classList.add('active');
}

// ---------- Firmware / Repo ----------
function saveFirmware() {
  const repoPath = document.getElementById('repoPath').value.trim();
  const fwUrl = document.getElementById('fwUrl').value.trim();

  if (fwUrl && !validateUrl(fwUrl)) {
    showMessage('fwMessage', 'Invalid URL. Use http/https.', 'error');
    return;
  }

  // Save to ESP32 instead of localStorage
  saveFirmwareToESP32(repoPath, fwUrl);
}

async function saveFirmwareToESP32(repoPath, fwUrl) {
  const btn = document.querySelector('button[onclick="saveFirmware()"]');
  const originalText = btn.textContent;
  
  btn.disabled = true;
  btn.textContent = 'Saving...';
  
  try {
    const result = await apiPost('/api/config/firmware/save', {
      repoPath: repoPath,
      fwUrl: fwUrl
    });
    
    if (result.success) {
      showMessage('fwMessage', result.message, 'success');
      
      // Update status display
      document.getElementById('fwLastSaved').textContent = result.lastSaved || nowISO();
      
      // Also save to localStorage as backup
      const settings = getSettings();
      settings.firmware.repoPath = result.repoPath;
      settings.firmware.fwUrl = result.fwUrl;
      settings.firmware.lastSaved = result.lastSaved || nowISO();
      setSettings(settings);
    } else {
      showMessage('fwMessage', result.message, 'error');
    }
  } catch (e) {
    showMessage('fwMessage', 'Save failed: ' + e.message, 'error');
  } finally {
    btn.disabled = false;
    btn.textContent = originalText;
  }
}

function applyFirmware() {
  const btn = document.getElementById('fwApplyBtn');
  btn.disabled = true;
  btn.textContent = 'Applying...';
  // Simulate apply
  setTimeout(() => {
    btn.disabled = false;
    btn.textContent = 'Apply';
    showMessage('fwMessage', 'Firmware apply simulated successfully', 'success');
  }, 1200);
}

function resetFirmware() {
  document.getElementById('repoPath').value = '';
  document.getElementById('fwUrl').value = '';
  showMessage('fwMessage', 'Firmware form reset', 'info');
}

function reflectFirmwareSaved(settings) {
  const fw = (settings || getSettings()).firmware;
  document.getElementById('fwLastSaved').textContent = fw.lastSaved ? fw.lastSaved : 'Never';
  document.getElementById('fwSavedRepo').textContent = fw.repoPath || '—';
  document.getElementById('fwSavedUrl').textContent = fw.fwUrl || '—';
}

// ---------- AP ----------
function toggleApPassword() {
  const pass = document.getElementById('apPass');
  const btn = document.querySelectorAll('.password-toggle')[0];
  if (pass.type === 'password') { pass.type = 'text'; btn.textContent = '🙈'; }
  else { pass.type = 'password'; btn.textContent = '👁'; }
}

function validateAp(ssid, password) {
  if (ssid.length > 32) {
    return 'SSID must be at most 32 characters';
  }
  if (password && (password.length < 8 || password.length > 63)) {
    return 'Password must be 8–63 characters or empty for open AP';
  }
  return null;
}

function updateApSecurityHint() {
  const pass = document.getElementById('apPass').value;
  const sec = document.getElementById('apSecState');
  if (!pass) { sec.textContent = 'Open'; sec.className = 'status-warning'; }
  else { sec.textContent = 'Secured'; sec.className = 'status-connected'; }
}

function saveAp() {
  const ssid = document.getElementById('apSsid').value.trim();
  const password = document.getElementById('apPass').value;
  const err = validateAp(ssid, password);
  if (err) { showMessage('apMessage', err, 'error'); return; }
  
  // Save to ESP32 instead of localStorage
  saveApToESP32(ssid, password);
}

async function saveApToESP32(ssid, password) {
  const btn = document.querySelector('button[onclick="saveAp()"]');
  const originalText = btn.textContent;
  
  btn.disabled = true;
  btn.textContent = 'Saving...';
  
  try {
    const result = await apiPost('/api/config/ap/save', {
      apSsid: ssid,
      apPass: password
    });
    
    if (result.success) {
      showMessage('apMessage', result.message, 'success');
      
      // Update current status display
      document.getElementById('currentApSsid').textContent = result.apSsid;
      document.getElementById('currentApSec').textContent = result.apPass === '[SET]' ? 'Secured' : 'Open';
      document.getElementById('currentApSec').className = result.apPass === '[SET]' ? 'status-value status-connected' : 'status-value status-warning';
      document.getElementById('apLastSaved').textContent = nowISO();
      
      // Also save to localStorage as backup
      const settings = getSettings();
      settings.ap.ssid = result.apSsid;
      settings.ap.password = result.apPass === '[SET]' ? '[SET]' : '';
      settings.ap.lastSaved = nowISO();
      setSettings(settings);
    } else {
      showMessage('apMessage', result.message, 'error');
    }
  } catch (e) {
    showMessage('apMessage', 'Save failed: ' + e.message, 'error');
  } finally {
    btn.disabled = false;
    btn.textContent = originalText;
  }
}

function applyAp() {
  const btn = document.getElementById('apApplyBtn');
  const originalText = btn.textContent;
  
  btn.disabled = true;
  btn.textContent = 'Applying...';
  
  // Apply AP settings to ESP32
  applyApToESP32(btn, originalText);
}

async function applyApToESP32(btn, originalText) {
  try {
    const result = await apiPost('/api/config/ap/apply', {});
    
    if (result.success) {
      showMessage('apMessage', result.message + ' - New IP: ' + result.apIp, 'success');
      
      // Update current status after apply
      document.getElementById('currentApSsid').textContent = result.apSsid;
      document.getElementById('currentApStatus').textContent = 'Active';
      document.getElementById('currentApStatus').className = 'status-value status-connected';
    } else {
      showMessage('apMessage', result.message, 'error');
    }
  } catch (e) {
    showMessage('apMessage', 'Apply failed: ' + e.message, 'error');
  } finally {
    btn.disabled = false;
    btn.textContent = originalText;
  }
}

function resetAp() {
  document.getElementById('apSsid').value = '';
  document.getElementById('apPass').value = '';
  updateApSecurityHint();
  showMessage('apMessage', 'AP form reset', 'info');
}

function reflectApSaved(settings) {
  const ap = (settings || getSettings()).ap;
  document.getElementById('apLastSaved').textContent = ap.lastSaved ? ap.lastSaved : 'Never';
  document.getElementById('apSavedSsid').textContent = ap.ssid || '—';
  document.getElementById('apSavedSec').textContent = ap.password ? 'Secured' : 'Open';
  document.getElementById('apSavedSec').className = ap.password ? 'status-value status-connected' : 'status-value status-warning';
}

// ---------- Load Settings from ESP32 ----------
async function loadApFromESP32() {
  try {
    const result = await apiGet('/api/config/ap/load');
    
    // Update form fields
    document.getElementById('apSsid').value = result.apSsid || '';
    document.getElementById('apPass').value = result.apPass || '';
    updateApSecurityHint();
    
    // Update status display
    document.getElementById('currentApStatus').textContent = 'Active';
    document.getElementById('currentApStatus').className = 'status-value status-connected';
    document.getElementById('currentApSsid').textContent = result.apSsid || '—';
    document.getElementById('currentApSec').textContent = result.apPass ? 'Secured' : 'Open';
    document.getElementById('currentApSec').className = result.apPass ? 'status-value status-connected' : 'status-value status-warning';
    document.getElementById('apLastSaved').textContent = result.lastSaved || 'Unknown';
    
    console.log('AP settings loaded from ESP32:', result);
  } catch (e) {
    console.warn('Failed to load AP settings from ESP32:', e);
    document.getElementById('currentApStatus').textContent = 'Error';
    document.getElementById('currentApStatus').className = 'status-value status-error';
    // Fallback to localStorage
    hydrateFromStorage();
  }
}

async function loadFirmwareFromESP32() {
  try {
    const result = await apiGet('/api/config/firmware/load');
    
    // Update form fields
    document.getElementById('repoPath').value = result.repoPath || '';
    document.getElementById('fwUrl').value = result.fwUrl || '';
    
    // Update status display
    document.getElementById('fwLastSaved').textContent = result.lastSaved || 'Unknown';
    
    console.log('Firmware settings loaded from ESP32:', result);
  } catch (e) {
    console.warn('Failed to load firmware settings from ESP32:', e);
    // Fallback to localStorage
    hydrateFromStorage();
  }
}

async function loadAllSettingsFromESP32() {
  // Load all settings from ESP32 in parallel
  await Promise.allSettled([
    loadApFromESP32(),
    loadFirmwareFromESP32(),
    loadSystemInfo()
  ]);
}

// ---------- System Information Functions ----------
async function loadSystemInfo() {
  try {
    // Load both system status and configuration data
    const [statusResult, configResult] = await Promise.all([
      apiGet('/api/config/status'),
      apiGet('/api/config/export')
    ]);
    
    // Update device status with real-time data
    document.getElementById('systemStatus').textContent = statusResult.systemStatus || 'Online';
    document.getElementById('systemStatus').className = 'status-value status-connected';
    document.getElementById('systemUptime').textContent = formatUptime(statusResult.uptime || Date.now());
    document.getElementById('systemMemory').textContent = formatBytes(statusResult.freeMemory || 0);
    document.getElementById('firmwareVersion').textContent = statusResult.firmwareVersion || '2.2.0';
    
    // Update WiFi configuration with real-time status
    document.getElementById('sysApSsid').textContent = statusResult.wifi.apSsid || '—';
    document.getElementById('sysApSecurity').textContent = configResult.wifi.apPass === '[SET]' ? 'Secured' : 'Open';
    document.getElementById('sysApSecurity').className = configResult.wifi.apPass === '[SET]' ? 'status-value status-connected' : 'status-value status-warning';
    document.getElementById('sysStaSsid').textContent = statusResult.wifi.staSsid || '—';
    document.getElementById('sysStaStatus').textContent = statusResult.wifi.staStatus || 'Disconnected';
    document.getElementById('sysStaStatus').className = statusResult.wifi.staStatus === 'Connected' ? 'status-value status-connected' : 'status-value status-warning';
    
    // Update user profile
    document.getElementById('sysUserName').textContent = configResult.user.name || '—';
    document.getElementById('sysUserEmail').textContent = configResult.user.email || '—';
    document.getElementById('sysUserPhone').textContent = configResult.user.phone || '—';
    const userStatus = statusResult.config.userConfigured ? 'Configured' : 'Not Set';
    document.getElementById('sysUserStatus').textContent = userStatus;
    document.getElementById('sysUserStatus').className = userStatus === 'Configured' ? 'status-value status-connected' : 'status-value status-warning';
    
    // Update firmware settings
    document.getElementById('sysRepoPath').textContent = configResult.firmware.repoPath || '—';
    document.getElementById('sysFwUrl').textContent = configResult.firmware.fwUrl || '—';
    document.getElementById('sysFwLastSaved').textContent = configResult.firmware.lastSaved || '—';
    const fwStatus = statusResult.config.firmwareConfigured ? 'Configured' : 'Not Set';
    document.getElementById('sysFwStatus').textContent = fwStatus;
    document.getElementById('sysFwStatus').className = fwStatus === 'Configured' ? 'status-value status-connected' : 'status-value status-warning';
    
    // Update email configuration
    document.getElementById('sysEmailHost').textContent = configResult.email.smtpHost || '—';
    document.getElementById('sysEmailPort').textContent = configResult.email.smtpPort || '—';
    document.getElementById('sysEmailAccount').textContent = configResult.email.emailAccount || '—';
    const emailStatus = statusResult.config.emailConfigured ? 'Configured' : 'Not Set';
    document.getElementById('sysEmailStatus').textContent = emailStatus;
    document.getElementById('sysEmailStatus').className = emailStatus === 'Configured' ? 'status-value status-connected' : 'status-value status-warning';
    
    // Update GSM configuration
    document.getElementById('sysGsmCarrier').textContent = configResult.gsm.carrierName || '—';
    document.getElementById('sysGsmApn').textContent = configResult.gsm.apn || '—';
    document.getElementById('sysGsmUser').textContent = configResult.gsm.apnUser || '—';
    const gsmStatus = statusResult.config.gsmConfigured ? 'Configured' : 'Not Set';
    document.getElementById('sysGsmStatus').textContent = gsmStatus;
    document.getElementById('sysGsmStatus').className = gsmStatus === 'Configured' ? 'status-value status-connected' : 'status-value status-warning';
    
    console.log('System information loaded:', { status: statusResult, config: configResult });
  } catch (e) {
    console.warn('Failed to load system information:', e);
    document.getElementById('systemStatus').textContent = 'Error';
    document.getElementById('systemStatus').className = 'status-value status-error';
  }
}

function formatUptime(milliseconds) {
  const seconds = Math.floor(milliseconds / 1000);
  const hours = Math.floor(seconds / 3600);
  const minutes = Math.floor((seconds % 3600) / 60);
  const secs = seconds % 60;
  
  if (hours > 0) {
    return `${hours}h ${minutes}m ${secs}s`;
  } else if (minutes > 0) {
    return `${minutes}m ${secs}s`;
  } else {
    return `${secs}s`;
  }
}

function formatBytes(bytes) {
  if (bytes === 0) return '0 B';
  const k = 1024;
  const sizes = ['B', 'KB', 'MB', 'GB'];
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  return parseFloat((bytes / Math.pow(k, i)).toFixed(1)) + ' ' + sizes[i];
}

function refreshSystemInfo() {
  const btn = document.querySelector('button[onclick="refreshSystemInfo()"]');
  const originalText = btn.textContent;
  
  btn.disabled = true;
  btn.textContent = 'Refreshing...';
  
  loadSystemInfo().finally(() => {
    btn.disabled = false;
    btn.textContent = originalText;
  });
}

function exportSystemInfo() {
  const btn = document.querySelector('button[onclick="exportSystemInfo()"]');
  const originalText = btn.textContent;
  
  btn.disabled = true;
  btn.textContent = 'Exporting...';
  
  exportSettingsFromESP32(btn, originalText);
}

// ---------- Hydration ----------
function hydrateFromStorage() {
  const settings = getSettings();
  // Firmware fields
  document.getElementById('repoPath').value = settings.firmware.repoPath || '';
  document.getElementById('fwUrl').value = settings.firmware.fwUrl || '';
  reflectFirmwareSaved(settings);
  // AP fields
  document.getElementById('apSsid').value = settings.ap.ssid || '';
  document.getElementById('apPass').value = settings.ap.password || '';
  updateApSecurityHint();
  reflectApSaved(settings);
}

// ---------- Init ----------
document.addEventListener('DOMContentLoaded', function() {
  // Load all settings from ESP32 first, then fallback to localStorage
  loadAllSettingsFromESP32();
  
  // Setup event listeners
  document.getElementById('apPass').addEventListener('input', updateApSecurityHint);
});
  </script>
</body>
</html>



)rawliteral";

// Length of the configuration HTML string
const size_t config_html_len = sizeof(config_html) - 1;

#endif