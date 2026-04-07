// 定义三个LED引脚（根据你的实际接线修改）
#define LED_PIN_R 2    // 红灯引脚
#define LED_PIN_Y 13   // 黄灯引脚
#define LED_PIN_G 14   // 绿灯引脚

const int freq = 5000;
const int resolution = 8;

// 每个灯的亮度和方向
int duty1 = 0, dir1 = 1;
int duty2 = 85, dir2 = 1;
int duty3 = 170, dir3 = 1;

void setup() {
  Serial.begin(115200); // 初始化串口，解决Serial报错
  // 绑定PWM到引脚
  ledcAttach(LED_PIN_R, freq, resolution);
  ledcAttach(LED_PIN_Y, freq, resolution);
  ledcAttach(LED_PIN_G, freq, resolution);
}

void loop() {
  // 更新灯1（红灯）
  duty1 += dir1;
  if(duty1 >= 255 || duty1 <= 0) dir1 = -dir1;
  ledcWrite(LED_PIN_R, duty1);

  // 更新灯2（黄灯）
  duty2 += dir2;
  if(duty2 >= 255 || duty2 <= 0) dir2 = -dir2;
  ledcWrite(LED_PIN_Y, duty2);

  // 更新灯3（绿灯）
  duty3 += dir3;
  if(duty3 >= 255 || duty3 <= 0) dir3 = -dir3;
  ledcWrite(LED_PIN_G, duty3);

  delay(10); // 控制呼吸速度
  Serial.println("Breathing cycle completed"); // 这句可以删掉，因为现在是持续呼吸，不是一轮一轮
}