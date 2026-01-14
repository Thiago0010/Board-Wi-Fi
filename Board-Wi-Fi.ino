#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <nvs_flash.h>

// --- INSTÂNCIAS ---
WebServer server(80);
Preferences preferences;

// --- VARIÁVEIS GLOBAIS ---
String current_ssid = "";
String current_pw = "";
String eventLog = "";

void addLog(String msg) {
  eventLog = "[" + String(millis() / 1000) + "s] " + msg + "<br>" + eventLog;
  if (eventLog.length() > 1500) eventLog = eventLog.substring(0, 1500);
}

// --- INTERFACE HTML (Embutida) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Verificador Wi-Fi - Admin</title>
    <style>
        body { font-family: 'Segoe UI', sans-serif; background: #121212; color: #e0e0e0; margin: 0; padding: 15px; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 15px; max-width: 1000px; margin: auto; }
        .card { background: #1e1e1e; padding: 15px; border-radius: 8px; border-top: 4px solid #ff9800; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }
        h2 { color: #ff9800; font-size: 1.1rem; margin-top: 0; border-bottom: 1px solid #333; padding-bottom: 5px; }
        input { width: calc(100% - 22px); padding: 10px; margin: 5px 0; background: #333; border: 1px solid #444; color: white; border-radius: 4px; }
        .btn { background: #ff9800; color: #000; border: none; padding: 10px; border-radius: 4px; cursor: pointer; font-weight: bold; width: 100%; margin-top: 10px; transition: 0.3s; }
        .btn:hover { background: #e68900; }
        .status-box { font-size: 0.9rem; line-height: 1.6; }
        #log { background: #000; color: #0f0; font-family: monospace; height: 120px; overflow-y: auto; padding: 10px; border-radius: 4px; font-size: 0.75rem; border: 1px solid #333; }
        .wifi-item { padding: 5px; border-bottom: 1px solid #333; cursor: pointer; font-size: 0.85rem; }
        .wifi-item:hover { background: #333; }
    </style>
</head>
<body>
    <h1 style="text-align:center; color:#ff9800; font-size: 1.5rem;">Boards Utility</h1>
    <div class="grid">
        <div class="card" style="border-top-color: #00bcd4;">
            <h2>Configurar Wi-Fi</h2>
            <input type="text" id="ssid" placeholder="SSID da Rede">
            <input type="password" id="pass" placeholder="Senha">
            <button class="btn" style="background:#00bcd4;" onclick="saveWifi()">Salvar e Reiniciar</button>
            <button class="btn" style="background:#555; color:#fff;" onclick="scanWifi()">Escanear Redes</button>
            <div id="wifi-list" style="margin-top:10px; max-height: 150px; overflow-y: auto;"></div>
        </div>
        <div class="card">
            <h2>Status do Sistema</h2>
            <div id="sys" class="status-box">Carregando dados...</div>
        </div>
        <div class="card" style="grid-column: span 1;">
            <h2>Console Log</h2>
            <div id="log">Iniciando...</div>
        </div>
    </div>
    <script>
        function update() {
            fetch('/status').then(r => r.json()).then(data => {
                document.getElementById('sys').innerHTML = `
                    <b>IP:</b> ${data.ip}<br>
                    <b>Rede Atual:</b> ${data.ssid}<br>
                    <b>Sinal:</b> ${data.rssi} dBm<br>
                    <b>Heap:</b> ${data.heap} KB<br>
                    <b>Uptime:</b> ${data.uptime}s
                `;
                document.getElementById('log').innerHTML = data.logs;
            });
        }
        function scanWifi() {
            document.getElementById('wifi-list').innerHTML = "Buscando...";
            fetch('/scan').then(r => r.json()).then(data => {
                let html = "";
                data.forEach(n => {
                    html += `<div class="wifi-item" onclick="document.getElementById('ssid').value='${n.s}'">${n.s} (${n.r}dBm)</div>`;
                });
                document.getElementById('wifi-list').innerHTML = html;
            });
        }
        function saveWifi() {
            const s = document.getElementById('ssid').value;
            const p = document.getElementById('pass').value;
            if(!s) return alert('Escolha uma rede!');
            fetch(`/set_wifi?ssid=${encodeURIComponent(s)}&pass=${encodeURIComponent(p)}`)
            .then(() => alert('Configurado! O dispositivo vai reiniciar.'));
        }
        setInterval(update, 3000);
        update();
    </script>
</body>
</html>
)rawliteral";

// --- HANDLERS ---
void handleRoot() { server.send(200, "text/html", index_html); }

void handleStatus() {
  String json = "{";
  json += "\"ip\":\"" + (WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : WiFi.softAPIP().toString()) + "\",";
  json += "\"ssid\":\"" + (WiFi.status() == WL_CONNECTED ? WiFi.SSID() : "Desconectado") + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"heap\":" + String(ESP.getFreeHeap() / 1024) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"logs\":\"" + eventLog + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleScan() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; ++i) {
    json += "{\"s\":\"" + WiFi.SSID(i) + "\",\"r\":" + String(WiFi.RSSI(i)) + "}";
    if (i < n - 1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleSetWifi() {
  if (server.hasArg("ssid")) {
    String newS = server.arg("ssid");
    String newP = server.arg("pass");
    preferences.putString("ssid", newS);
    preferences.putString("pass", newP);
    server.send(200, "text/plain", "OK");
    delay(1000);
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
  
  preferences.begin("wifi-conf", false);
  current_ssid = preferences.getString("ssid", "M5Stick_New");
  current_pw = preferences.getString("pass", "");

  addLog("Iniciando Board...");

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(current_ssid.c_str(), current_pw.c_str());

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.softAP("M5-Stick-Config", "12345678");
    addLog("Modo AP: Conecte em 'Board-Config'");
  } else {
    addLog("Conectado a: " + current_ssid);
  }

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/scan", handleScan);
  server.on("/set_wifi", handleSetWifi);
  
  server.begin();
}

void loop() {
  server.handleClient();
}