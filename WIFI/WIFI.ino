#include <M5Unified.h>
#include <WiFiManager.h> 

// 用于存储当前显示的状态
bool showConnectionInfo = false;

void setup() {
  auto cfg = M5.config();
  cfg.external_imu = false; 
  M5.begin(cfg);

  M5.Display.setRotation(1); 

  // 使用 WiFiManager 管理 Wi-Fi 连接
  WiFiManager wifiManager;

  // 如果无法自动连接，则启动 AP 模式，名称为 "ESP32_AP"
  if (!wifiManager.autoConnect("ESP32_AP")) //改成自己需要的名字 
    {
    M5.Display.println("Failed to connect to Wi-Fi");
    ESP.restart();
  }

  // 显示 Wi-Fi 信息
  displayWiFiInfo();
}

void loop() {
  // 检查 A 按钮是否被按下
  if (M5.BtnA.wasPressed()) {
    showConnectionInfo = true;  
    displayConnectionInfo();    
  }

  // 检查 B 按钮是否被按下
  if (M5.BtnB.wasPressed()) {
    showConnectionInfo = false; 
    displayWiFiInfo();          
  }

  // 更新按钮状态
  M5.update();
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
}
