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
      <h1>ESP32 Configuration</h1>
      <p>Access-Point Configuration</p>
    </div>

    <!-- Access-Point Configuration -->
    <div class="content-grid">
      <div class="card">
        <h3>📡 Access-Point (AP) Configuration</h3>
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
  </div>

  <script>
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

function showMessage(id, text, type = 'success') {
  const el = document.getElementById(id);
  el.textContent = text;
  el.className = `message ${type}`;
  el.style.display = 'block';
  setTimeout(() => { el.style.display = 'none'; }, 4000);
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

// ---------- Load AP Settings from ESP32 ----------
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
  }
}

// ---------- Init ----------
document.addEventListener('DOMContentLoaded', function() {
  // Load AP settings from ESP32
  loadApFromESP32();
  
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