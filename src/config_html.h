#ifndef CONFIG_HTML_H
#define CONFIG_HTML_H
#include <Arduino.h>

const char config_html[] PROGMEM = R"rawliteral(
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
      <div style="display: flex; justify-content: space-between; align-items: flex-start;">
        <div>
          <h1>ESP32 Configuration</h1>
          <p>Access-Point Configuration</p>
        </div>
        <button class="btn-danger" onclick="exitDashboard()" style="padding: 8px 16px; font-size: 0.8rem; margin-left: 16px;">Exit Dashboard</button>
      </div>
    </div>
    
    <!-- Tabs -->
    <div class="tabs" id="tabContainer">
      <button class="tab-btn active" onclick="showTab('ap')">AP Configuration</button>
      <button class="tab-btn" onclick="showTab('email')">Email Settings</button>
    </div>

    <!-- AP Tab -->
    <div id="ap" class="tab-content active">
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

    <!-- Email Tab -->
    <div id="email" class="tab-content">
      <div class="content-grid">
        <div class="card">
          <h3>📧 Email Configuration</h3>
          <div class="status-row">
            <span class="status-label">Email Status</span>
            <span class="status-value" id="emailConfigStatus">Not configured</span>
          </div>
          <div class="status-row">
            <span class="status-label">SMTP Server</span>
            <span class="status-value" id="smtpServer">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Email Account</span>
            <span class="status-value" id="emailAccount">—</span>
          </div>
          <div class="status-row">
            <span class="status-label">Last Test</span>
            <span class="status-value" id="lastEmailTest">Never</span>
          </div>
        </div>

        <div class="card">
          <h3>⚙️ SMTP Settings</h3>
          <div class="form-group">
            <label for="smtpHost">SMTP Host</label>
            <input type="text" id="smtpHost" placeholder="smtp.gmail.com" value="smtp.gmail.com">
          </div>

          <div class="form-group">
            <label for="smtpPort">SMTP Port</label>
            <input type="number" id="smtpPort" placeholder="465" value="465">
          </div>

          <div class="form-group">
            <label for="emailAccountInput">Email Address</label>
            <input type="email" id="emailAccountInput" placeholder="your.email@gmail.com">
          </div>

          <div class="form-group">
            <label for="emailPassword">App Password</label>
            <div class="password-input">
              <input type="password" id="emailPassword" placeholder="Gmail app password">
              <button type="button" class="password-toggle" onclick="toggleEmailPassword()">👁</button>
            </div>
            <small style="color: #94a3b8; font-size: 0.75rem; margin-top: 4px; display: block;">
              For Gmail, use an App Password (not your regular password)
            </small>
          </div>

          <div class="form-group">
            <label for="senderName">Sender Name</label>
            <input type="text" id="senderName" placeholder="ESP32 Dashboard" value="ESP32 Dashboard">
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-primary btn-full" onclick="saveEmailConfig()">Save Email Settings</button>
              <button class="btn-danger btn-full" onclick="clearEmailForm()">Clear Form</button>
            </div>
          </div>
        </div>

        <div class="card">
          <h3>🧪 Email Testing</h3>
          <div class="form-group">
            <label for="testRecipientEmail">Recipient Email</label>
            <input type="email" id="testRecipientEmail" placeholder="recipient@example.com">
          </div>

          <div class="form-group">
            <label for="testEmailSubject">Subject</label>
            <input type="text" id="testEmailSubject" placeholder="Test Email from ESP32" value="Test Email from ESP32 Dashboard">
          </div>

          <div class="form-group">
            <label for="testEmailContent">Message Content</label>
            <textarea id="testEmailContent" rows="4" placeholder="Enter your test message here...">This is a test email sent from your ESP32 IoT Configuration Panel.</textarea>
          </div>

          <div class="button-group">
            <div class="button-row">
              <button class="btn-success btn-full" onclick="sendTestEmail('wifi')" id="sendTestEmailWifiBtn">Send test email via WiFi</button>
              <button class="btn-success btn-full" onclick="sendTestEmail('gsm')" id="sendTestEmailGsmBtn">Send test email via GSM</button>
            </div>
          </div>
          
          <div id="emailTestResult" class="message" style="display: none;"></div>
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

// ---------- Tabs ----------
function showTab(tabId) {
  document.querySelectorAll('.tab-content').forEach(tab => tab.classList.remove('active'));
  document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
  const tab = document.getElementById(tabId);
  if (tab) tab.classList.add('active');
  const activeBtn = document.querySelector(`.tabs .tab-btn[onclick="showTab('${tabId}')"]`);
  if (activeBtn) activeBtn.classList.add('active');
}

// ---------- AP ----------
function toggleApPassword() {
  const pass = document.getElementById('apPass');
  const btn = document.querySelectorAll('.password-toggle')[0];
  if (pass.type === 'password') { pass.type = 'text'; btn.textContent = '🙈'; }
  else { pass.type = 'password'; btn.textContent = '👁'; }
}

function toggleEmailPassword() {
  const passField = document.getElementById('emailPassword');
  const toggleBtn = document.querySelectorAll('.password-toggle')[1];
  if (passField.type === 'password') { 
    passField.type = 'text'; 
    toggleBtn.textContent = '🙈'; 
  } else { 
    passField.type = 'password'; 
    toggleBtn.textContent = '👁'; 
  }
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

// ---------- Email Functions ----------
async function loadEmailConfig() {
  try {
    const d = await apiGet('/api/load/email');
    document.getElementById('smtpHost').value = d.smtpHost || 'smtp.gmail.com';
    document.getElementById('smtpPort').value = d.smtpPort || 465;
    document.getElementById('emailAccountInput').value = d.emailAccount || '';
    document.getElementById('senderName').value = d.senderName || 'ESP32 Dashboard';
    if (d.emailAccount) {
      document.getElementById('smtpServer').textContent = d.smtpHost || '—';
      document.getElementById('emailAccount').textContent = d.emailAccount || '—';
      const emailConfigStatusEl = document.getElementById('emailConfigStatus');
      emailConfigStatusEl.textContent = 'Configured';
      emailConfigStatusEl.className = 'status-value status-connected';
    }
  } catch (e) { 
    console.warn('Email config load failed', e); 
  }
}

async function saveEmailConfig() {
  const btn = event.target;
  btn.disabled = true;
  const original = btn.textContent;
  btn.textContent = 'Saving...';
  try {
    const config = {
      smtpHost: document.getElementById('smtpHost').value,
      smtpPort: parseInt(document.getElementById('smtpPort').value),
      emailAccount: document.getElementById('emailAccountInput').value,
      emailPassword: document.getElementById('emailPassword').value,
      senderName: document.getElementById('senderName').value
    };
    if (!config.smtpHost || !config.emailAccount || !config.emailPassword) {
      showMessage('emailTestResult', 'Please fill in SMTP Host, Email, and App Password', 'error');
      return;
    }
    const result = await apiPost('/api/save/email', config);
    if (result.success) {
      showMessage('emailTestResult', 'Email configuration saved successfully!', 'success');
      document.getElementById('smtpServer').textContent = config.smtpHost;
      document.getElementById('emailAccount').textContent = config.emailAccount;
      const emailConfigStatusEl = document.getElementById('emailConfigStatus');
      emailConfigStatusEl.textContent = 'Configured';
      emailConfigStatusEl.className = 'status-value status-connected';
    } else {
      showMessage('emailTestResult', 'Failed to save email configuration: ' + (result.message || 'Unknown error'), 'error');
    }
  } catch (e) {
    showMessage('emailTestResult', 'Save failed: ' + e.message, 'error');
  } finally {
    btn.disabled = false;
    btn.textContent = original;
  }
}

async function sendTestEmail(via) {
  const btn = via === 'gsm' ? document.getElementById('sendTestEmailGsmBtn') : document.getElementById('sendTestEmailWifiBtn');
  const recipient = document.getElementById('testRecipientEmail').value;
  const subject = document.getElementById('testEmailSubject').value;
  const content = document.getElementById('testEmailContent').value;
  if (!recipient) { alert('Please enter a recipient email address'); return; }
  btn.disabled = true; const original = btn.textContent; btn.textContent = 'Sending...';
  try {
    const endpoint = via === 'gsm' ? '/api/email/send?via=gsm' : '/api/email/send?via=wifi';
    const result = await apiPost(endpoint, { to: recipient, subject: subject, content: content });
    if (result.success) {
      showMessage('emailTestResult', 'Test email sent successfully!', 'success');
      document.getElementById('lastEmailTest').textContent = new Date().toLocaleString();
    } else {
      showMessage('emailTestResult', 'Email sending failed: ' + (result.error || 'Unknown error'), 'error');
    }
  } catch (e) {
    showMessage('emailTestResult', 'Email test failed: ' + e.message, 'error');
  } finally {
    btn.disabled = false; btn.textContent = original;
  }
}

async function sendQuickTest() {
  const recipient = document.getElementById('testRecipientEmail').value;
  if (!recipient) { alert('Please enter a recipient email address for quick test'); return; }
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
  btn.disabled = true; const original = btn.textContent; btn.textContent = 'Checking...';
  try {
    const config = {
      smtpHost: document.getElementById('smtpHost').value,
      smtpPort: parseInt(document.getElementById('smtpPort').value),
      emailAccount: document.getElementById('emailAccountInput').value,
      emailPassword: document.getElementById('emailPassword').value
    };
    if (!config.smtpHost || !config.emailAccount || !config.emailPassword) {
      showMessage('emailTestResult', 'Please fill in all required email configuration fields', 'info');
      return;
    }
    const testRecipient = config.emailAccount;
    const result = await apiPost('/api/email/send', { to: testRecipient, subject: 'Email Configuration Test', content: 'This is a test email to verify your SMTP configuration. If you receive this, your email settings are correct!' });
    if (result.success) {
      showMessage('emailTestResult', 'Email configuration test passed! Configuration is working correctly.', 'success');
      document.getElementById('lastEmailTest').textContent = new Date().toLocaleString();
    } else {
      showMessage('emailTestResult', 'Email configuration test failed: ' + (result.error || 'Unknown error. Check your SMTP settings and credentials.'), 'error');
    }
  } catch (e) {
    showMessage('emailTestResult', 'Configuration check failed: ' + e.message, 'error');
  } finally {
    btn.disabled = false; btn.textContent = original;
  }
}

function clearEmailForm() {
  document.getElementById('emailPassword').value = '';
  document.getElementById('testRecipientEmail').value = '';
  document.getElementById('testEmailSubject').value = 'Test Email from ESP32 Dashboard';
  document.getElementById('testEmailContent').value = 'This is a test email sent from your ESP32 IoT Configuration Panel.';
  document.getElementById('emailTestResult').style.display = 'none';
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
// ---------- Init ----------
// ---------- AP Configuration Functions ----------
async function loadApFromESP32() {
  try {
    const data = await apiGet('/api/load/ap');
    document.getElementById('apSsid').value = data.apSsid || '';
    document.getElementById('apPass').value = data.apPass || '';
    
    // Update current status
    document.getElementById('currentApSsid').textContent = data.currentApSsid || '—';
    document.getElementById('currentApStatus').textContent = `${data.connectedDevices} devices connected`;
    document.getElementById('currentApSec').textContent = data.apPass ? 'WPA2' : 'Open';
    
    updateApSecurityHint();
  } catch (error) {
    console.error('Failed to load AP config:', error);
    showMessage('apMessage', 'Failed to load AP configuration', 'error');
  }
}

async function saveAp() {
  const btn = document.querySelector('button[onclick="saveAp()"]');
  btn.classList.add('loading');
  btn.textContent = 'Saving...';
  btn.disabled = true;
  
  try {
    const apSsid = document.getElementById('apSsid').value.trim();
    const apPass = document.getElementById('apPass').value;
    
    if (!apSsid) {
      showMessage('apMessage', 'SSID cannot be empty', 'error');
      return;
    }
    
    if (apSsid.length > 32) {
      showMessage('apMessage', 'SSID too long (max 32 characters)', 'error');
      return;
    }
    
    if (apPass && apPass.length < 8) {
      showMessage('apMessage', 'Password must be at least 8 characters or empty', 'error');
      return;
    }
    
    const result = await apiPost('/api/save/ap', {
      apSsid: apSsid,
      apPass: apPass
    });
    
    if (result.success) {
      showMessage('apMessage', result.message, 'success');
      document.getElementById('apLastSaved').textContent = new Date().toLocaleString();
    } else {
      showMessage('apMessage', result.message || 'Failed to save AP configuration', 'error');
    }
  } catch (error) {
    showMessage('apMessage', 'Save failed: ' + error.message, 'error');
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Save';
    btn.disabled = false;
  }
}

async function applyAp() {
  const btn = document.getElementById('apApplyBtn');
  btn.classList.add('loading');
  btn.textContent = 'Applying...';
  btn.disabled = true;
  
  try {
    // First save the configuration
    await saveAp();
    
    showMessage('apMessage', 'AP configuration saved. Device restart required to apply changes.', 'info');
  } catch (error) {
    showMessage('apMessage', 'Apply failed: ' + error.message, 'error');
  } finally {
    btn.classList.remove('loading');
    btn.textContent = 'Apply';
    btn.disabled = false;
  }
}

function resetAp() {
  document.getElementById('apSsid').value = 'ESP32-AccessPoint';
  document.getElementById('apPass').value = '12345678';
  updateApSecurityHint();
  showMessage('apMessage', 'AP settings reset to defaults', 'info');
}

function copySettings() {
  const apSsid = document.getElementById('apSsid').value;
  const apPass = document.getElementById('apPass').value;
  
  const settings = `AP SSID: ${apSsid}\nAP Password: ${apPass || 'Open'}`;
  
  navigator.clipboard.writeText(settings).then(() => {
    showMessage('apMessage', 'Settings copied to clipboard', 'success');
  }).catch(() => {
    showMessage('apMessage', 'Failed to copy to clipboard', 'error');
  });
}

function toggleApPassword() {
  const passField = document.getElementById('apPass');
  const toggleBtn = document.querySelector('button[onclick="toggleApPassword()"]');
  
  if (passField.type === 'password') {
    passField.type = 'text';
    toggleBtn.textContent = '🙈';
  } else {
    passField.type = 'password';
    toggleBtn.textContent = '👁';
  }
}

function updateApSecurityHint() {
  const apPass = document.getElementById('apPass').value;
  const secState = document.getElementById('apSecState');
  
  if (apPass && apPass.length >= 8) {
    secState.textContent = 'WPA2 Protected';
    secState.className = 'status-connected';
  } else if (apPass && apPass.length > 0) {
    secState.textContent = 'Weak Password';
    secState.className = 'status-error';
  } else {
    secState.textContent = 'Open';
    secState.className = 'status-warning';
  }
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

document.addEventListener('DOMContentLoaded', function() {
  // Load AP settings from ESP32
  loadApFromESP32();
  // Load Email settings from ESP32
  loadEmailConfig();
  
  // Setup event listeners
  document.getElementById('apPass').addEventListener('input', updateApSecurityHint);
});
  </script>
</body>
</html>
)rawliteral";

const size_t config_html_len = sizeof(config_html) - 1;

#endif // CONFIG_HTML_H 
