/*
 * RTC - リアルタイムクロック
 * 
 * M5Stack Tab5の内蔵RTCを使用して日時を表示します。
 */

#include <M5Unified.h>

void setup(void) {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);

    M5.Display.setFont(&fonts::FreeMonoBoldOblique24pt7b);
    M5.Display.setRotation(3);

    if (!M5.Rtc.isEnabled()) {
        Serial.println("RTC not found.");
        M5.Display.println("RTC not found.");
        for (;;) {
            vTaskDelay(500);
        }
    }

    M5.Display.println("RTC found.");
    Serial.println("RTC found.");

    M5.Display.println("Setup RTC Time");
    // RTCに日時を設定（年, 月, 日, 時, 分, 秒）
    M5.Rtc.setDateTime({{2025, 12, 24}, {20, 10, 56}});
    delay(1000);
}

void loop(void) {
    // 曜日の配列
    static constexpr const char* const wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    delay(500);

    // RTCから現在日時を取得
    auto dt = M5.Rtc.getDateTime();
    
    // シリアル出力
    Serial.printf("RTC UTC: %04d/%02d/%02d (%s) %02d:%02d:%02d\r\n", 
                  dt.date.year, dt.date.month, dt.date.date,
                  wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);
    
    // 画面表示
    M5.Display.setCursor(0, 100);
    M5.Display.printf("RTC UTC: %04d/%02d/%02d (%s) %02d:%02d:%02d", 
                      dt.date.year, dt.date.month, dt.date.date,
                      wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);
} 