#include <M5Unified.h>
#include <WiFi.h>
#include <esp_now.h>

// 图像缓冲区
uint8_t imageBuffer[240 * 135 * 2]; // 缓存 JPEG 数据
size_t receivedSize = 0;
bool imageComplete = false;

// 显示图像
void renderImage() {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.drawJpg(imageBuffer, receivedSize, 0, 0);
    receivedSize = 0; // 清空缓冲区
    imageComplete = false;
}

// ESP-NOW 接收回调
void onDataRecv(const uint8_t* macAddr, const uint8_t* data, int len) {
    if (len == 1 && data[0] == 0xFF) { // 检测结束标志
        imageComplete = true;
        renderImage();
        return;
    }

    if (receivedSize + len > sizeof(imageBuffer)) {
        Serial.println("Buffer overflow, discarding data");
        receivedSize = 0;
        return;
    }

    memcpy(imageBuffer + receivedSize, data, len);
    receivedSize += len;
    Serial.printf("Received chunk: %d bytes, total: %d bytes\n", len, receivedSize);
}

void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(TFT_BLACK);

    Serial.begin(115200); // 初始化串口
    Serial.println("Getting MAC Address...");

    // 显示 M5StickC Plus 的 MAC 地址
    WiFi.mode(WIFI_STA); // 确保处于 Wi-Fi 客户端模式
    WiFi.disconnect();
    String macAddress = WiFi.macAddress();
    Serial.print("M5StickC Plus MAC Address: ");
    Serial.println(macAddress);

    if (esp_now_init() != ESP_OK) {
        M5.Lcd.println("ESP-NOW init failed");
        Serial.println("ESP-NOW init failed");
        while (true);
    }

    esp_now_register_recv_cb(onDataRecv);
    M5.Lcd.println("ESP-NOW Ready");
    Serial.println("ESP-NOW Ready");
}

void loop() {
    if (imageComplete) {
        renderImage();
    }
    delay(100);
}
