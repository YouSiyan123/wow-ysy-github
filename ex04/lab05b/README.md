#include <WiFi.h>
#include <WebServer.h>

// ===================== 配置参数 =====================
// WiFi 信息（请修改为你自己的WiFi名称和密码）
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";

// LED引脚（根据你的开发板修改，ESP32一般用2号引脚）
const int ledPin = 2;
// PWM通道（0-15任意选）
const int pwmChannel = 0;
// PWM频率（5kHz，人耳无噪音）
const int pwmFreq = 5000;
// PWM分辨率（8位，对应0-255亮度）
const int pwmResolution = 8;

// 创建Web服务器对象，监听80端口
WebServer server(80);

// ===================== HTML网页代码 =====================
const char* htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 无极调光器</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 80vh;
            background-color: #f0f0f0;
        }
        h1 {
            color: #333;
        }
        .slider-container {
            width: 80%;
            max-width: 500px;
            text-align: center;
        }
        input[type="range"] {
            width: 100%;
            height: 25px;
            -webkit-appearance: none;
            background: #ddd;
            border-radius: 15px;
            outline: none;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 40px;
            height: 40px;
            border-radius: 50%;
            background: #2196F3;
            cursor: pointer;
            transition: background 0.3s;
        }
        input[type="range"]::-webkit-slider-thumb:hover {
            background: #1976D2;
        }
        #brightness-value {
            font-size: 24px;
            margin-top: 20px;
            color: #333;
        }
    </style>
</head>
<body>
    <h1>ESP32 LED 无极调光器</h1>
    <div class="slider-container">
        <input type="range" id="brightness" min="0" max="255" value="0">
        <div id="brightness-value">亮度: 0</div>
    </div>

    <script>
        // 获取滑动条元素
        const slider = document.getElementById('brightness');
        const valueDisplay = document.getElementById('brightness-value');

        // 监听滑动条变化事件
        slider.addEventListener('input', function() {
            const brightness = this.value;
            // 更新页面显示的亮度值
            valueDisplay.textContent = `亮度: ${brightness}`;
            // 发送GET请求到ESP32，更新LED亮度
            fetch(`/set?brightness=${brightness}`)
                .catch(err => console.error('请求失败:', err));
        });
    </script>
</body>
</html>
)HTML";

// ===================== 处理网页请求 =====================
void handleRoot() {
  // 发送HTML网页
  server.send(200, "text/html", htmlPage);
}

// ===================== 处理亮度设置请求 =====================
void handleSetBrightness() {
  if (server.hasArg("brightness")) {
    // 从URL中获取亮度值
    String brightnessStr = server.arg("brightness");
    int brightness = brightnessStr.toInt();
    
    // 限制亮度范围在0-255之间
    brightness = constrain(brightness, 0, 255);
    
    // 设置PWM输出，控制LED亮度
    ledcWrite(pwmChannel, brightness);
    
    // 发送成功响应
    server.send(200, "text/plain", "OK");
  } else {
    // 没有参数，返回错误
    server.send(400, "text/plain", "Bad Request");
  }
}

// ===================== 初始化与主循环 =====================
void setup() {
  // 启动串口调试
  Serial.begin(115200);
  
  // 配置PWM
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  // 将PWM通道绑定到LED引脚
  ledcAttachPin(ledPin, pwmChannel);
  // 初始亮度设为0（熄灭）
  ledcWrite(pwmChannel, 0);

  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功!");
  Serial.print("ESP32 IP地址: ");
  Serial.println(WiFi.localIP());

  // 注册路由
  server.on("/", handleRoot);
  server.on("/set", handleSetBrightness);

  // 启动Web服务器
  server.begin();
  Serial.println("Web服务器已启动");
}

void loop() {
  // 处理客户端请求
  server.handleClient();
}
