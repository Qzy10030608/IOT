#include <M5Unified.h>
#include <WiFiManager.h> 

bool showConnectionInfo = false;
WiFiManager wifiManager;

void setup() {
  auto cfg = M5.config();
  cfg.external_imu = false; 
  M5.begin(cfg);

  M5.Display.setRotation(1); 

  // 尝试自动连接 Wi-Fi，若失败则启动 AP 模式
  if (!wifiManager.autoConnect("STICK_456")) {
    M5.Display.println("Failed to connect to Wi-Fi");
    ESP.restart();
  }

  displayWiFiInfo();
}

void loop() {
  M5.update();

  // 按下 A 按钮显示连接信息
  if (M5.BtnA.wasPressed()) {
    showConnectionInfo = true;
    displayConnectionInfo();
  }

  // 按下 B 按钮返回 Wi-Fi 信息
  if (M5.BtnB.wasPressed() && !M5.BtnB.pressedFor(5000)) {
    showConnectionInfo = false;
    displayWiFiInfo();
  }

  // 长按 B 按钮 5 秒重置 Wi-Fi
  if (M5.BtnB.pressedFor(5000)) {
    resetWiFiAndReconnect();
  }
}

// 显示 Wi-Fi 的 SSID 和 IP 地址
void displayWiFiInfo() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(TFT_BLUE);
  M5.Display.setTextSize(2);

  M5.Display.setCursor(10, 10);
  M5.Display.println("SSID: " + WiFi.SSID());

  M5.Display.setCursor(10, 40);
  M5.Display.println("IP Address: ");
  M5.Display.setCursor(10, 60);
  M5.Display.println(WiFi.localIP());

  M5.Display.setCursor(10, 100);
  M5.Display.println("Press A for connections");
  M5.Display.setCursor(10, 120);
  M5.Display.println("Hold B 5s to reset Wi-Fi");
}

// 显示连接信息
void displayConnectionInfo() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(TFT_GREEN);
  M5.Display.setTextSize(2);

  M5.Display.setCursor(10, 10);
  M5.Display.println("Connection Success!");

  int numClients = WiFi.softAPgetStationNum();
  M5.Display.setCursor(10, 40);
  M5.Display.println("Devices connected: " + String(numClients));

  M5.Display.setCursor(10, 80);
  M5.Display.println("Press B to return");
  M5.Display.setCursor(10, 100);
  M5.Display.println("Hold B 5s to reset Wi-Fi");
}

// 重置 Wi-Fi 设置并重新连接
void resetWiFiAndReconnect() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(TFT_RED);
  M5.Display.setCursor(10, 10);
  M5.Display.println("Resetting Wi-Fi...");

  wifiManager.resetSettings(); // 清除保存的 Wi-Fi 设置
  delay(1000);

  ESP.restart(); // 重启设备进入配置模式
}
