#include <WiFi.h>
#include <WebServer.h>

// ===================== 配置区 =====================
const char* ssid = "heyheyhey";
const char* password = "newbee666666";
const int ledPin = 2;          // LED引脚
const int touchPin = T0;       // 触摸引脚（D16对应GPIO16，部分型号用T0=GPIO4，按硬件修改）

// ===================== 系统状态 =====================
bool isArmed = false;
bool isAlarm = false;
int touchValue = 0;
const int touchThreshold = 40; // 触摸阈值，可根据实际调整

WebServer server(80);

// ===================== 主页HTML（含仪表盘+控制） =====================
void handleRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP32 安防报警器 & 触摸仪表盘</title>
  <style>
    body{font-family:Arial,sans-serif;text-align:center;margin-top:30px;}
    h1{color:#333;}
    .dashboard{font-size:64px;font-weight:bold;color:#2196F3;margin:30px 0;}
    .status{font-size:24px;margin:20px 0;color:#555;}
    button{padding:12px 24px;font-size:18px;margin:0 10px;border:none;border-radius:8px;cursor:pointer;}
    .arm{background:#f59e0b;color:white;}
    .disarm{background:#10b981;color:white;}
  </style>
</head>
<body>
  <h1>ESP32 安防报警器</h1>
  <div class="status" id="status">当前状态：已撤防</div>
  <div class="dashboard" id="touchValue">--</div>
  <button class="arm" onclick="sendCmd('arm')">布防</button>
  <button class="disarm" onclick="sendCmd('disarm')">撤防</button>

  <script>
    // 实时更新触摸值（AJAX拉取）
    setInterval(() => {
      fetch('/data')
      .then(res => res.text())
      .then(val => {
        document.getElementById('touchValue').textContent = val;
      });
      
      // 同步更新系统状态
      fetch('/status')
      .then(res => res.json())
      .then(s => {
        const st = document.getElementById('status');
        if(s.alarm) st.textContent = '⚠️ 报警中！';
        else if(s.armed) st.textContent = '🔒 已布防';
        else st.textContent = '✅ 已撤防';
      });
    }, 200);

    // 发送布防/撤防命令
    function sendCmd(cmd) {
      fetch('/cmd?action=' + cmd);
    }
  </script>
</body>
</html>
)HTML";
  server.send(200, "text/html; charset=utf-8", html);
}

// ===================== 实时触摸值接口 =====================
void handleData() {
  touchValue = touchRead(touchPin);
  server.send(200, "text/plain", String(touchValue));
}

// ===================== 系统状态接口 =====================
void handleStatus() {
  String json = "{\"armed\":" + String(isArmed) + ",\"alarm\":" + String(isAlarm) + "}";
  server.send(200, "application/json; charset=utf-8", json);
}

// ===================== 布防/撤防命令接口 =====================
void handleCmd() {
  if(server.hasArg("action")){
    String cmd = server.arg("action");
    if(cmd == "arm"){
      isArmed = true;
      isAlarm = false;
      digitalWrite(ledPin, LOW);
    }else if(cmd == "disarm"){
      isArmed = false;
      isAlarm = false;
      digitalWrite(ledPin, LOW);
    }
  }
  server.send(200, "text/plain; charset=utf-8", "OK");
}

// ===================== 触摸检测与报警逻辑 =====================
void checkTouchAndAlarm() {
  // 仅布防且未报警时，触摸触发锁定报警
  if(isArmed && !isAlarm && touchValue < touchThreshold){
    isAlarm = true;
  }

  // 报警状态：LED高频闪烁（非阻塞，不影响Web服务）
  if(isAlarm){
    static unsigned long lastToggle = 0;
    if(millis() - lastToggle > 100){
      lastToggle = millis();
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  }else{
    digitalWrite(ledPin, LOW);
  }
}

// ===================== 初始化 =====================
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.println("访问地址: http://" + WiFi.localIP().toString());

  // 注册路由
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/status", handleStatus);
  server.on("/cmd", handleCmd);
  server.begin();
}

// ===================== 主循环 =====================
void loop() {
  server.handleClient();
  touchValue = touchRead(touchPin); // 实时更新触摸值
  checkTouchAndAlarm();
}
