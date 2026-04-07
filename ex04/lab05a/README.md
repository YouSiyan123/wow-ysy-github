#define LED_PIN    2
#define TOUCH_PIN  T0
#define THRESHOLD  400

int brightness = 0;
int fadeAmount = 10;
const int delayTime[4] = {0, 50, 20, 5}; // 1慢 2中 3快
int speedLevel = 1;

// 新增：记录是否已经切换过
bool hasSwitched = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  
  Serial.println("==========");
  Serial.println("初始档位: 1");
  Serial.println("按住触摸，档位会疯狂切换！");
}

void loop() {
  int touchVal = touchRead(TOUCH_PIN);
  bool currentTouch = (touchVal < THRESHOLD);

  if (currentTouch) {
    if (!hasSwitched) {
      int old = speedLevel;
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1;
      
      Serial.print("【强制切换】");
      Serial.print(old);
      Serial.print(" → ");
      Serial.print(speedLevel);
      Serial.print(" | Delay: ");
      Serial.println(delayTime[speedLevel]);
      
      hasSwitched = true; // 防止一直刷屏
    }
  } else {
    hasSwitched = false; // 松开后重置，下次摸还能切
  }

  analogWrite(LED_PIN, brightness);
  brightness += fadeAmount;
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount *= -1;
  }
  delay(delayTime[speedLevel]);
}