#define LED_R_PIN 4
#define LED_Y_PIN 5
int brightness_R = 0;  // 灯R初始亮度（最暗）
int brightness_Y = 255; // 灯Y初始亮度（最亮）
// 呼吸步长（控制渐变速度，数值越大变化越快）
int step = 1;
void setup() {
  // 初始化引脚为输出模式
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_Y_PIN, OUTPUT);
  // 可选：开启串口调试（查看运行状态）
  Serial.begin(9600);
  Serial.println("警车双闪灯启动！");
}
void loop() {
  // 1. 灯R亮度增加，灯Y亮度减少
  brightness_R += step;
  brightness_Y -= step;
  // 2. 检测边界并反转呼吸方向（反相同步）
  // 灯R到达最亮（255）时，灯Y刚好到达最暗（0）
  if (brightness_R >= 255) {
    brightness_R = 255;
    brightness_Y = 0;
    step = -step; // 反转步长，开始变暗
  }
  // 灯R到达最暗（0）时，灯Y刚好到达最亮（255）
  else if (brightness_R <= 0) {
    brightness_R = 0;
    brightness_Y = 255;
    step = -step; // 反转步长，开始变亮
  }
  // 3. 输出PWM信号控制LED亮度
  analogWrite(LED_R_PIN, brightness_R);
  analogWrite(LED_Y_PIN, brightness_Y);
  // 4. 延时控制呼吸速度（可调整，数值越小速度越快）
  delay(5);
}
