#include <WiFi.h>

#define LED_PIN      2
#define TOUCH_PIN    T0

bool isArmed = false;
bool isAlarming = false;

const char* ssid = "heyheyhey";
const char* password = "newbee666666";

WiFiServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 安防报警器</title>
  <style>
    body { text-align:center;font-family:Arial; }
    .btn { padding:15px 30px;font-size:20px;margin:10px;border:none;border-radius:5px;color:white; }
    .arm { background-color:#2196F3; }
    .disarm { background-color:#f44336; }
    .status { margin:20px;font-size:24px; }
  </style>
</head>
<body>
  <h1>ESP32 物联网安防系统</h1>
  <div class="status">状态：%STATUS%</div>
  <button class="btn arm" onclick="window.location.href='/arm'">布防(Arm)</button>
  <button class="btn disarm" onclick="window.location.href='/disarm'">撤防(Disarm)</button>
</body>
</html>
)rawliteral";

void handleClient(WiFiClient client) {
  if (!client || !client.connected()) return;

  String req = client.readStringUntil('\r');
  client.flush();

  String statusText;
  if (isAlarming) statusText = "🔴 报警中";
  else if (isArmed) statusText = "🟢 已布防";
  else statusText = "⚪ 已撤防";

  String resp = index_html;
  resp.replace("%STATUS%", statusText);

  if (req.indexOf("/arm") != -1) {
    isArmed = true;
    isAlarming = false;
    Serial.println("已布防");
  }
  if (req.indexOf("/disarm") != -1) {
    isArmed = false;
    isAlarming = false;
    Serial.println("已撤防");
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.print(resp);
  client.stop();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi已连接 IP: " + WiFi.localIP().toString());

  server.begin();
  Serial.println("===== 系统启动完成 =====");
}

void loop() {
  WiFiClient client = server.available();
  if (client) handleClient(client);

  // 触摸读取（关键！）
  uint16_t t = touchRead(TOUCH_PIN);

  // 你触摸后是 170~190，所以 <500 必触发
  if (isArmed && !isAlarming && t < 500) {
    isAlarming = true;
    Serial.println("触摸触发！报警开始！");
  }

  // 报警闪烁
  if (isAlarming) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
