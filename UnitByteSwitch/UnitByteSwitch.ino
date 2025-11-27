#include <M5Unified.h>
#include "unit_byte.hpp"

uint32_t color_list[]   = {0xff0000, 0x00ff00, 0x0000ff};
uint8_t color_index     = 0;
time_t last_update_time = 0;
uint8_t switchId        = 0x46;
UnitByte dev;

void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);
    Wire.begin();  // I2C通信の明示的な初期化
    M5.Display.setFont(&fonts::lgfxJapanMinchoP_16);
    M5.Display.setCursor(0, 0);
    M5.Display.setTextSize(2);
    M5.Display.println("Initializing...");

    // Tab5のデフォルトI2Cピンを使用（-1でデフォルト）
    int retry_count = 0;
    while (!dev.begin(&Wire, switchId, -1, -1, 400000)) {
        retry_count++;
        //M5.Display.fillRect(0, 40, M5.Display.width(), 80, TFT_BLACK);
        M5.Display.setCursor(0, 80);
        M5.Display.setTextColor(TFT_RED, TFT_BLACK);
        M5.Display.printf("Init Fail! (retry: %d)\n", retry_count);
        M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5.Display.println("Check connection:");
        M5.Display.println("I2C Addr: 0x46");
        M5.Display.println("SDA/SCL: Default");
        delay(1000);
        
        // 10回リトライしても失敗したら警告を表示して続行
        if (retry_count >= 10) {
            M5.Display.setTextColor(TFT_ORANGE, TFT_BLACK);
            M5.Display.println("\nContinuing anyway...");
            delay(2000);
            break;
        }
    }
    
    if (retry_count < 10) {
        M5.Display.fillRect(0, 40, M5.Display.width(), 80, TFT_BLACK);
        M5.Display.setCursor(0, 80);
        M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        M5.Display.println("Init Success!");
        delay(500);
    }
    
    dev.setLEDShowMode(BYTE_LED_USER_DEFINED);
    for (uint8_t i = 0; i <= 8; i++) {
        dev.setLEDBrightness(i, 250);
    }
}

void loop()
{
    // M5.Display.clear();
    M5.Display.setCursor(0, 300);
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t switchStatus8Bytes = dev.getSwitchStatus(i);
        M5.Display.printf("CH[%d]: %d\r\n", i, switchStatus8Bytes);
    }

    if (millis() - last_update_time > 1000) {
        color_index = (color_index + 1) % 3;
        for (int i = 0; i <= 8; i++) {
            dev.setRGB888(i, color_list[color_index]);
        }
        last_update_time = millis();
    }
} 