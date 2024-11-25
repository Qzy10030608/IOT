#include <M5Unified.h>
#include <cmath> // 用于数学函数，如 sqrt

// 常量定义
#define GRAPH_HEIGHT 80
#define GRAPH_WIDTH 160
#define SAMPLE_RATE 10
#define BUZZER_PIN 2
#define ALARM_DURATION 2000

// 计步器相关常量
#define STEP_DELAY 300 // 每次步行动作的最小时间间隔（毫秒）

// 全局变量
float threshold = 5.0;      // 阈值（用于页面 2 和页面 4）
int currentPage = 0;        // 当前页面索引
int step_count = 0;         // 步数计数（用于页面 3）
bool isAlarmActive = false; // 警报状态（用于页面 4）
unsigned long lastStepTime = 0; // 上一次记录步数的时间
unsigned long alarmStartTime = 0; // 警报开始时间
float prev_magnitude = 0;   // 页面 3 的加速度数据大小

// 页面 2 振动数据和图形的状态
static int16_t lastX = 0;
static int16_t lastY = GRAPH_HEIGHT;

// 初始化
void setup() {
  M5.begin(); // 初始化 M5StickC Plus
  M5.Display.setRotation(3); // 横屏模式
  pinMode(35, INPUT_PULLUP); // 设置 GPIO35 为按钮输入

  // 配置蜂鸣器为 PWM 输出
  ledcSetup(0, 2000, 8);          // 频道 0，2000 Hz，分辨率 8 位
  ledcAttachPin(BUZZER_PIN, 0);   // 将频道 0 绑定到 BUZZER_PIN

  Serial.begin(115200); // 开启串口调试
  page1(); // 默认显示页面 1
}

// 主循环
void loop() {
  M5.update(); // 更新按钮状态

  // 按钮 A 增大阈值
  if (M5.BtnA.wasPressed()) {
    threshold += 1.0;
    updateCurrentPage();
  }

  // 按钮 B 减小阈值
  if (M5.BtnB.wasPressed() && threshold > 1.0) {
    threshold -= 1.0;
    updateCurrentPage();
  }

  // 按钮 C 切换页面
  static unsigned long lastButtonCTime = 0;
  if (digitalRead(35) == LOW && millis() - lastButtonCTime > 1000) {
    lastButtonCTime = millis();
    currentPage = (currentPage + 1) % 4; // 页面循环切换
    updateCurrentPage();
  }

  // 页面持续更新
  if (currentPage == 1) {
    updatePage2(); // 更新振动图形
  } else if (currentPage == 2) {
    updatePage3(); // 更新计步器
  } else if (currentPage == 3) {
    updatePage4(); // 更新加速度检测和警报
  }
}

// 切换到当前页面
void updateCurrentPage() {
  switch (currentPage) {
    case 0: page1(); break;
    case 1: page2(); break;
    case 2: page3(); break;
    case 3: page4(); break;
  }
}

// 页面 1: Hello World
void page1() {
  M5.Display.fillScreen(GREEN);
  M5.Display.setTextSize(4);
  M5.Display.setTextColor(RED);
  M5.Display.setCursor((M5.Display.width() - 140) / 2, M5.Display.height() / 2 - 40);
  M5.Display.println("22TE456");
  displayThreshold();
}

// 页面 2: 振动数据和图形
void page2() {
  M5.Display.fillScreen(BLACK);
  lastX = 0; // 重置图形起始点
  lastY = GRAPH_HEIGHT; // 确保每次页面切换时图形从顶部开始
  displayThreshold();
}

// 页面 2: 更新振动数据和图形
void updatePage2() {
  float accX, accY, accZ;
  M5.Imu.getAccel(&accX, &accY, &accZ);
  float magnitude = sqrt(accX * accX + accY * accY + accZ * accZ);

  int16_t x = lastX + 1;
  int16_t y = GRAPH_HEIGHT - (magnitude * 10);

  if (x >= GRAPH_WIDTH) {
    x = 0;
    lastX = 0;
    M5.Display.fillRect(0, 20, GRAPH_WIDTH, GRAPH_HEIGHT, BLACK);
    int16_t threshold_y = GRAPH_HEIGHT - (threshold * 10) + 20; // 固定红线位置
    M5.Display.drawLine(0, threshold_y, GRAPH_WIDTH, threshold_y, TFT_RED);
  }

  M5.Display.drawLine(lastX, lastY, x, y, TFT_GREEN);
  lastX = x;
  lastY = y;

  if (magnitude > threshold) {
    ledcWrite(0, 128); // 激活蜂鸣器
  } else {
    ledcWrite(0, 0); // 停止蜂鸣器
  }

  delay(SAMPLE_RATE);
}

// 页面 3: 计步器
void page3() {
  M5.Display.fillScreen(BLACK); // 每次进入页面 3 时刷新屏幕
  M5.Display.setTextSize(3);
  M5.Display.setTextColor(BLUE);
  M5.Display.setCursor(10, 50);
  M5.Display.printf("Steps: %d", step_count);
  displayThreshold();
}

// 页面 3: 更新计步器逻辑
void updatePage3() {
  float ax, ay, az;
  M5.Imu.getAccel(&ax, &ay, &az);

  // 计算当前加速度的大小
  float magnitude = sqrt(ax * ax + ay * ay + az * az);

  // 直接在此函数内设置计步阈值为 1.0
  float stepThreshold = 1.0;

  // 判断是否为步行动作
  if (abs(magnitude - prev_magnitude) > stepThreshold &&
      (millis() - lastStepTime > STEP_DELAY)) {
    step_count++;
    lastStepTime = millis();
    Serial.printf("Step counted! Current steps: %d\n", step_count); // 调试输出

    // 只在步数变化时更新屏幕显示
    M5.Display.fillRect(10, 50, 200, 30, BLACK); // 只清理显示步数的区域
    M5.Display.setTextSize(3);
    M5.Display.setTextColor(BLUE);
    M5.Display.setCursor(10, 50);
    M5.Display.printf("Steps: %d", step_count);
  }

  prev_magnitude = magnitude; // 更新上一次加速度的大小
}

// 页面 4: 加速度检测和警报
void page4() {
  M5.Display.fillScreen(BLACK);
  displayThreshold();
}

// 页面 4: 更新加速度检测和警报
void updatePage4() {
  float ax, ay, az;
  M5.Imu.getAccel(&ax, &ay, &az);
  float magnitude = sqrt(ax * ax + ay * ay + az * az);

  if (magnitude > threshold && !isAlarmActive) {
    isAlarmActive = true;
    alarmStartTime = millis();
    ledcWrite(0, 128); // 激活蜂鸣器
    M5.Display.fillScreen(RED);
    M5.Display.setTextSize(3);
    M5.Display.setTextColor(GREEN);
    M5.Display.setCursor(10, 200);
    M5.Display.println("FALL DETECTED!");
  }

  if (isAlarmActive && millis() - alarmStartTime > ALARM_DURATION) {
    isAlarmActive = false;
    ledcWrite(0, 0); // 停止蜂鸣器
    M5.Display.fillScreen(BLACK);
    displayThreshold();
  }
}

// 显示阈值
void displayThreshold() {
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(BLUE);
  M5.Display.fillRect(0, 0, M5.Display.width(), 20, BLACK);
  M5.Display.setCursor(10, 0);
  M5.Display.printf("Threshold: %.1f", threshold);
}
