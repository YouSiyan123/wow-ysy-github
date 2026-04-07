#define TOUCH_PIN 4
#define LED_PIN   2
#define THRESHOLD 500

const int freq = 5000;
const int resolution = 8;

int speedLevel = 1;
bool lastTouch = false;

// 呼吸灯非阻塞变量
int duty = 0;
int dir = 1; // 1=变亮，-1=变暗
unsigned long prevMillis = 0;

void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, freq, resolution);
}

void loop() {
  // 触摸检测（立即响应）
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouch = (touchValue < THRESHOLD);

  if (currentTouch && !lastTouch) {
    speedLevel = (speedLevel % 3) + 1;
    Serial.print("Speed Level: ");
    Serial.println(speedLevel);
    delay(200);
  }
  lastTouch = currentTouch;

  // 档位速度
  int step, delayTime;
  if (speedLevel == 1) {
    step = 1; delayTime = 7;
  } else if (speedLevel == 2) {
    step = 2; delayTime = 5;
  } else {
    step = 4; delayTime = 3;
  }

  // 非阻塞呼吸灯
  if (millis() - prevMillis >= delayTime) {
    prevMillis = millis();
    duty += dir * step;

    if (duty >= 255) {
      duty = 255;
      dir = -1;
    }
    if (duty <= 0) {
      duty = 0;
      dir = 1;
    }

    ledcWrite(LED_PIN, duty);
  }
}