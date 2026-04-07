#define LED_PIN 2
#define LED_PIN_R 26

void setup() {
  // 初始化串口通信，设置波特率为115200
  Serial.begin(115200);
  // 将LED引脚设置为输出模式
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_R, OUTPUT);
}

void loop() {
  Serial.println("Hello ESP32!");    // 串口输出提示
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN_R, HIGH);  // 点亮LED
  
  delay(1000);                 // 保持1秒（1000毫秒）
  
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN_R, LOW);   // 熄灭LED
  
  delay(1000);                 // 保持1秒
}