#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "heyheyhey";
const char* password = "newbee666666";

WebServer server(80);

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实验1</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:50px;">
  <h1>ESP32 Web服务器实验</h1>
  <h2>第一部分：静态页面显示</h2>
  <p>如果你能看到这个页面，说明实验成功。</p>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

void setup() {
  Serial.begin(115200);
  
  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}