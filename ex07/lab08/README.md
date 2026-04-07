#include <WiFi.h>
#include <WebServer.h>

const char* ap_ssid = "ESP32-ysy009";
const char* ap_pass = "12345678";

const int ledPin   = 2;    // 你的灯
const int touchPin = 16;   // 你现在用的 D16

bool isArmed = false;      // 布防
bool isAlarm = false;      // 报警

WebServer server(80);

// 网页（不乱码）
void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html><head><meta charset=UTF-8></head>";
  html += "<body text-align=center>";
  html += "<h1>ESP32 安防报警器</h1>";
  html += "<p id=status>状态：已撤防</p>";
  html += "<button onclick=fetch('/arm')>布防</button>";
  html += "<button onclick=fetch('/disarm')>撤防</button>";
  html += "<script>";
  html += "setInterval(()=>{";
  html += "fetch('/status').then(r=>r.json()).then(d=>{";
  html += "if(d.alarm) document.getElementById('status').innerText='报警中！';";
  html += "else if(d.armed) document.getElementById('status').innerText='已布防';";
  html += "else document.getElementById('status').innerText='已撤防';";
  html += "});},1000);";
  html += "</script></body></html>";

  server.send(200, "text/html; charset=utf-8", html);
}

void handleArm() {
  isArmed = true;
  isAlarm = false;
  server.send(200, "OK");
}

void handleDisarm() {
  isArmed = false;
  isAlarm = false;
  digitalWrite(ledPin, LOW);
  server.send(200, "OK");
}

void handleStatus() {
  String s = "{\"armed\":" + String(isArmed) + ",\"alarm\":" + String(isAlarm) + "}";
  server.send(200, "application/json", s);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(touchPin, INPUT_PULLUP);
  digitalWrite(ledPin, LOW);

  WiFi.softAP(ap_ssid, ap_pass);
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();

  // 布防时才检测触摸
  if (isArmed && !isAlarm) {
    if (digitalRead(touchPin) == LOW) {
      isAlarm = true;
    }
  }

  // 报警：灯快速闪
  if (isAlarm) {
    static unsigned long t = 0;
    if (millis() - t > 100) {
      t = millis();
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  } else {
    digitalWrite(ledPin, LOW);
  }
}
