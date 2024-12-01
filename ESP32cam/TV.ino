#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>

// WiFi 配置
const char* ssid = "XXX-XXX-XXX";         // 替换为您的 WiFi 名称
const char* password = "XXX-XXX-XXX";     // 替换为您的 WiFi 密码

// ESP32-CAM 的 HTTP 流地址
const char* imageURL = "http://自己的地址capture"; // 替换为您的 ESP32-CAM 地址

void setup() {
    // 初始化 M5StickC Plus
    M5.begin();
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(TFT_BLACK);

    // 初始化串口监视器
    Serial.begin(115200);
    Serial.println("Connecting to WiFi...");

    // 连接到 WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    M5.Lcd.println("WiFi connected!");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // 开始 HTTP 请求
        http.begin(imageURL);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) { // 成功获取数据
            int len = http.getSize();
            uint8_t* buffer = (uint8_t*)malloc(len); // 分配缓冲区以保存图像数据
            if (buffer) {
                WiFiClient* stream = http.getStreamPtr();
                size_t received = 0;
                while (http.connected() && received < len) {
                    size_t available = stream->available();
                    if (available) {
                        size_t readLen = stream->readBytes(buffer + received, available);
                        received += readLen;
                    }
                }

                // 渲染图像
                M5.Lcd.fillScreen(TFT_BLACK);
                M5.Lcd.drawJpg(buffer, len, 0, 0);

                Serial.println("Image displayed successfully.");
                free(buffer); // 释放内存
            }
        } else {
            Serial.printf("HTTP request failed, code: %d\n", httpCode);
            M5.Lcd.println("Failed to fetch image");
        }

        http.end();
    } else {
        Serial.println("WiFi disconnected");
        M5.Lcd.println("WiFi disconnected");
    }

    delay(1000); // 每秒更新一次图像
}
