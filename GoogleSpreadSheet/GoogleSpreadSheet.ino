#include <M5Unified.h>  // 使用 M5Unified 库替换 M5StickC Plus 库
#include <WiFi.h>         // 引入 WiFi 库
#include <HTTPClient.h>   // 引入 HTTP 客户端库

// WiFi 配置
const char* ssid = "xxxxx";     // WiFi 名称
const char* password = "xxx-xxxxxxx"; // WiFi 密码

// Google Sheets API 配置
const String googleSheetUrl = "";  //自己的URL

// 状态变量
bool wifiConnected = false;      // WiFi 连接状态
bool timerRunning = false;       // 计时器运行状态
unsigned long startTime = 0;     // 计时器起始时间
unsigned long elapsedTime = 0;   // 计时器累计时间

void setup() {
  Serial.begin(115200);           // 初始化串口监视，波特率为 115200
  auto cfg = M5.config();
  cfg.output_power = true;
  M5.begin(cfg);                    // 初始化 M5StickC Plus
  M5.Display.setRotation(3);        // 设置屏幕为横屏模式
  M5.Display.fillScreen(TFT_BLACK); // 设置背景为黑色

  // 显示连接 WiFi 状态
  M5.Display.setTextColor(TFT_YELLOW);   // 设置文字颜色为黄色
  M5.Display.setTextSize(2);             // 设置文字大小
  M5.Display.setCursor(10, 30);
  M5.Display.print("Coudee time Standby");

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.setCursor(10, 60);
    M5.Display.print(".");
    Serial.print(".");
  }
  wifiConnected = true;

  // 显示连接成功
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(10, 30);
  M5.Display.print("Codee now Connected 456");
  Serial.println("\nWiFi Connected!");
}

void loop() {
  M5.update(); // 更新按键状态

  // 显示计时器
  if (timerRunning) {
    elapsedTime = millis() - startTime;
  }
  displayTime(elapsedTime);

  // A 按钮单击：启动/停止计时器
  if (M5.BtnA.wasPressed()) {
    if (timerRunning) {
      timerRunning = false; // 停止计时
      Serial.println("Timer Stopped");
    } else {
      timerRunning = true;
      startTime = millis() - elapsedTime; // 继续计时
      Serial.println("Timer Started");
    }
  }

  // A 按钮长按 3 秒：复原计时器
  if (M5.BtnA.pressedFor(3000)) {
    resetTimer();
  }

  // B 按钮长按 3 秒：上传数据
  if (M5.BtnB.pressedFor(3000)) {
    if (wifiConnected) {
      uploadData(); // 上传计时数据
    }
  }
}

// 显示计时器时间
void displayTime(unsigned long timeMillis) {
  static unsigned long lastDisplayedTime = 0;

  // 如果时间没有变化，则不刷新显示
  if (timeMillis == lastDisplayedTime) return;
  lastDisplayedTime = timeMillis;

  // 计算时分秒
  unsigned long seconds = (timeMillis / 1000) % 60;
  unsigned long minutes = (timeMillis / (1000 * 60)) % 60;
  unsigned long hours = (timeMillis / (1000 * 60 * 60));

  // 输出到串口监视
  Serial.printf("Time: %02lu:%02lu:%02lu\n", hours, minutes, seconds);

  // 清空数字显示区域并显示时间
  M5.Display.fillRect(0, 50, 240, 100, TFT_BLACK); // 清空计时器区域
  M5.Display.setTextColor(TFT_BLUE);               // 设置数字颜色为蓝色
  M5.Display.setTextSize(4);                       // 设置数字大小
  M5.Display.setCursor(20, 60);                    // 数字居中显示
  M5.Display.printf("%02lu:%02lu:%02lu", hours, minutes, seconds);
}

// 复原计时器
void resetTimer() {
  timerRunning = false;
  elapsedTime = 0; // 重置计时器
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(10, 30);
  M5.Display.setTextColor(TFT_YELLOW);
  M5.Display.print("Timer Reset");
  Serial.println("Timer Reset");
  delay(1000); // 显示 1 秒复原状态
  M5.Display.fillScreen(TFT_BLACK);
}

// 上传数据到 Google Sheets
void uploadData() {
  HTTPClient http;

  // 添加 check 参数，固定为 1
  String url = googleSheetUrl + "?check=1";

  http.begin(url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // 跟随重定向
  int httpCode = http.GET();
  String payload = http.getString(); // 获取响应内容
  http.end();

  // 打印响应状态码和内容
  Serial.printf("HTTP Response Code: %d\n", httpCode);
  Serial.println("Response Payload: " + payload);

  if (httpCode == HTTP_CODE_OK) {
    flashScreen(3); // 屏幕闪烁三次表示成功
  } else {
    showUploadFailed(httpCode, payload); // 显示上传失败
  }
}


// 屏幕闪烁
void flashScreen(int times) {
  for (int i = 0; i < times; i++) {
    M5.Display.fillScreen(TFT_BLACK);
    delay(300);
    M5.Display.fillScreen(TFT_YELLOW);
    delay(300);
  }
  M5.Display.fillScreen(TFT_BLACK);
}

// 显示上传失败
void showUploadFailed(int httpCode, String payload) {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_RED);
  M5.Display.setTextSize(3);
  M5.Display.setCursor(10, 30);
  M5.Display.printf("Upload Failed\nCode: %d\n%s", httpCode, payload.c_str());
  Serial.printf("Upload Failed\nCode: %d\n%s\n", httpCode, payload.c_str());
  delay(2000); // 显示 2 秒
  M5.Display.fillScreen(TFT_BLACK); // 恢复正常显示
}

