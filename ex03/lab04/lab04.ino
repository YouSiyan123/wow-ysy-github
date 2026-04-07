// lab03.ino - 定制节奏：(亮1s灭1s)*3 → (亮2.5s灭1s)*3 → 灭5s 循环
#define LED_PIN 2  // D2 对应 GPIO2

unsigned long lastMillis = 0;
int phase = 0;       // 0:第一组亮, 1:第一组灭, 2:第二组亮, 3:第二组灭, 4:停顿
int group1Count = 0; // 第一组已完成次数
int group2Count = 0; // 第二组已完成次数

// 时间配置（单位：毫秒）
const long GROUP1_ON  = 1000;  // 第一组亮 1s
const long GROUP1_OFF = 1000;  // 第一组灭 1s
const long GROUP2_ON  = 2500;  // 第二组亮 2.5s
const long GROUP2_OFF = 1000;  // 第二组灭 1s
const long PAUSE      = 5000;  // 结束后灭 5s

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // 上电必须熄灭！
  Serial.begin(115200);       // 打开串口调试
}

void loop() {
  unsigned long currentMillis = millis();
  long waitTime = 0;

  switch(phase) {
    case 0: // 第一组：亮 1s
      digitalWrite(LED_PIN, HIGH);
      waitTime = GROUP1_ON;
      phase = 1;
      Serial.println("Phase 0: Group1 ON");
      break;
      
    case 1: // 第一组：灭 1s
      digitalWrite(LED_PIN, LOW);
      waitTime = GROUP1_OFF;
      group1Count++;
      if (group1Count >= 3) { // 完成3次 → 进入第二组
        group1Count = 0;
        phase = 2;
      } else {
        phase = 0;
      }
      Serial.println("Phase 1: Group1 OFF, count=" + String(group1Count));
      break;
      
    case 2: // 第二组：亮 2.5s
      digitalWrite(LED_PIN, HIGH);
      waitTime = GROUP2_ON;
      phase = 3;
      Serial.println("Phase 2: Group2 ON");
      break;
      
    case 3: // 第二组：灭 1s
      digitalWrite(LED_PIN, LOW);
      waitTime = GROUP2_OFF;
      group2Count++;
      if (group2Count >= 3) { // 完成3次 → 进入停顿
        group2Count = 0;
        phase = 4;
      } else {
        phase = 2;
      }
      Serial.println("Phase 3: Group2 OFF, count=" + String(group2Count));
      break;
      
    case 4: // 停顿：灭 5s
      digitalWrite(LED_PIN, LOW);
      waitTime = PAUSE;
      phase = 0; // 循环回到第一组
      Serial.println("Phase 4: PAUSE OFF");
      break;
  }

  // 核心：只有时间到了才更新状态
  while (millis() - currentMillis < waitTime) {
    // 空循环等待，保证时间足够
  }
  lastMillis = currentMillis;
}
