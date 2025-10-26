#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void loadEnvFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream is_line(line);
        std::string key, value;

        if (std::getline(is_line, key, '=') && std::getline(is_line, value))
        {
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (value.size() >= 2)
            {
                if ((value.front() == '"' && value.back() == '"') ||
                    (value.front() == '\'' && value.back() == '\''))
                {
                    value = value.substr(1, value.size() - 2);
                }
            }

            setenv(key.c_str(), value.c_str(), 1);
        }
    }
}

std::string xorCipher(const std::string &text, const std::string &key)
{
    // If key is empty, return the original text unchanged
    if (key.empty())
        return text;

    std::string result = text;
    for (size_t i = 0; i < text.size(); ++i)
        result[i] = text[i] ^ key[i % key.size()];
    return result;
}

std::string getEmbeddedDashboardHTML()
{
    return R"HTML(<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <title>Matt_daemon Status</title>
  <style>
    :root{font-family:Inter, ui-sans-serif, system-ui, -apple-system, "Segoe UI", Roboto, "Helvetica Neue", Arial;}
    body{display:flex;align-items:center;justify-content:center;min-height:100vh;margin:0;background:#0f172a;color:#e6eef8}
    .card{background:linear-gradient(180deg, rgba(255,255,255,0.03), rgba(255,255,255,0.015));padding:28px;border-radius:12px;box-shadow:0 8px 30px rgba(2,6,23,0.6);width:420px;margin:20px}
    h1{font-size:20px;margin:0 0 12px}
    .row{display:flex;justify-content:space-between;padding:10px 12px;border-radius:8px;background:rgba(255,255,255,0.01);margin-bottom:8px}
    .label{opacity:.8}
    .value{font-weight:700}
    .status-up{color:#4ade80}
    .status-down{color:#fb7185}
    .footer{margin-top:12px;font-size:13px;color:rgba(230,238,248,0.65)}
    .meta{font-size:12px;color:rgba(230,238,248,0.55);margin-top:6px}
    .small-btn{background:transparent;border:1px solid rgba(230,238,248,0.06);padding:6px 8px;border-radius:8px;color:inherit;cursor:pointer;font-size:12px}
    .small-btn:hover{background:rgba(255,255,255,0.05)}
    .error{color:#ffb4b4}
  </style>
</head>
<body>
  <div class="card" role="region" aria-label="Daemon status card">
    <h1>Matt_daemon Status Dashboard</h1>

    <div id="daemonState" class="row">
      <div class="label">Status</div>
      <div id="statusValue" class="value status-up">—</div>
    </div>

    <div id="uptimeRow" class="row">
      <div class="label">Uptime</div>
      <div id="uptimeValue" class="value">—</div>
    </div>

    <div id="connectionsRow" class="row">
      <div class="label">Active connections</div>
      <div id="connectionsValue" class="value">—</div>
    </div>

    <div id="messagesRow" class="row">
      <div class="label">Total messages</div>
      <div id="messagesValue" class="value">—</div>
    </div>

    <div class="footer">
      <div class="meta">Server: <span id="sourceHost">localhost:4242</span></div>
      <div style="margin-top:6px;display:flex;gap:8px;align-items:center">
        <button id="refreshBtn" class="small-btn">Refresh Now</button>
        <div id="lastSeen" class="meta" style="margin-left:auto">Last update: —</div>
      </div>
      <div id="errorBox" class="meta error" style="margin-top:8px;display:none"></div>
    </div>
  </div>

<script>
const statusValue = document.getElementById('statusValue');
const uptimeValue = document.getElementById('uptimeValue');
const connectionsValue = document.getElementById('connectionsValue');
const messagesValue = document.getElementById('messagesValue');
const lastSeen = document.getElementById('lastSeen');
const errorBox = document.getElementById('errorBox');
const refreshBtn = document.getElementById('refreshBtn');

let pollHandle = null;

function fmtUptime(sec){
  if (typeof sec !== 'number' || !isFinite(sec)) return '—';
  const d = Math.floor(sec / 86400);
  sec %= 86400;
  const h = Math.floor(sec / 3600);
  sec %= 3600;
  const m = Math.floor(sec / 60);
  const s = sec % 60;
  const parts = [];
  if (d) parts.push(d+'d');
  if (h) parts.push(h+'h');
  if (m) parts.push(m+'m');
  parts.push(s+'s');
  return parts.join(' ');
}

function setError(msg){
  if (!msg) { errorBox.style.display='none'; errorBox.textContent=''; return; }
  errorBox.style.display='block';
  errorBox.textContent = msg;
}

function applyData(data){
  try {
    statusValue.textContent = data.status || 'running';
    statusValue.className = 'value ' + (data.status === 'running' ? 'status-up' : 'status-down');

    const up = data.uptime;
    uptimeValue.textContent = up !== undefined ? fmtUptime(up) : '—';

    connectionsValue.textContent = data.active_connections || '—';
    messagesValue.textContent = (data.total_messages !== undefined) ? String(data.total_messages) : '—';

    lastSeen.textContent = 'Last update: ' + (new Date()).toLocaleTimeString();
    setError(null);
  } catch (e){
    setError('Failed to parse data: ' + e.message);
  }
}

async function fetchStatus(){
  try{
    const res = await fetch('/status', {cache:'no-store'});
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const data = await res.json();
    applyData(data);
    return true;
  } catch (err){
    setError('Error: ' + err.message);
    statusValue.textContent = 'Error';
    statusValue.className = 'value status-down';
    return false;
  }
}

function startPolling(){
  if (pollHandle) clearInterval(pollHandle);
  fetchStatus();
  pollHandle = setInterval(fetchStatus, 2000);
}

refreshBtn.addEventListener('click', fetchStatus);

// Start polling on page load
startPolling();
</script>
</body>
</html>)HTML";
}

std::string loadDashboardHTML()
{
    // Try to load from file first
    std::ifstream file("dashboard.html");
    if (file.is_open())
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    // Fallback to embedded HTML
    return getEmbeddedDashboardHTML();
}