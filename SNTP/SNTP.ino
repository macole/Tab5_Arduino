#define NTP_TIMEZONE "UTC-9"
#define NTP_SERVER1  "0.pool.ntp.org"
#define NTP_SERVER2  "1.pool.ntp.org"
#define NTP_SERVER3  "2.pool.ntp.org"

#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <esp_sntp.h>
#include <sntp.h>

#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

// WiFi認証情報は secrets.h から読み込みます
// secrets.h.example をコピーして secrets.h を作成し、実際のSSIDとパスワードを設定してください
#include "secrets.h"

void setup(void)
{
    M5.begin();
    Serial.begin(115200);

    M5.Display.setFont(&fonts::FreeMonoBoldOblique24pt7b);
    M5.Display.setRotation(3);
    WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3, SDIO2_RST);

    // If you select the M5Tab5 board in Arduino IDE, you could use the default pins defined.
    // WiFi.setPins(BOARD_SDIO_ESP_HOSTED_CLK, BOARD_SDIO_ESP_HOSTED_CMD, BOARD_SDIO_ESP_HOSTED_D0,
    //              BOARD_SDIO_ESP_HOSTED_D1, BOARD_SDIO_ESP_HOSTED_D2, BOARD_SDIO_ESP_HOSTED_D3,
    //              BOARD_SDIO_ESP_HOSTED_RESET);

    // STA MODE
    WiFi.mode(WIFI_STA);
    M5.Display.println("WiFi mode set to STA");
    WiFi.begin(ssid, password);
    M5.Display.print("Connecting to ");
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Display.print(".");
    }
    M5.Display.println("");
    M5.Display.print("Connected to ");
    M5.Display.println(ssid);
    M5.Display.print("IP address: ");
    M5.Display.println(WiFi.localIP());

    if (!M5.Rtc.isEnabled()) {
        Serial.println("RTC not found.");
        M5.Display.println("RTC not found.");
        for (;;) {
            vTaskDelay(500);
        }
    }

    M5.Display.println("RTC found.");
    Serial.println("RTC found.");

    M5.Display.println("SNTP Sync...");

    configTzTime(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        Serial.print('.');
        delay(1000);
    }

    Serial.println("\r\n NTP Connected.");

    time_t t = time(nullptr) + 1;  // Advance one second.
    while (t > time(nullptr));     /// Synchronization in seconds
    M5.Rtc.setDateTime(gmtime(&t));
    M5.Display.clear();
}

void loop(void)
{
    static constexpr const char *const wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

    delay(500);

    auto dt = M5.Rtc.getDateTime();
    Serial.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n", dt.date.year, dt.date.month, dt.date.date,
                  wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);
    M5.Display.setCursor(0, 0);
    M5.Display.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d", dt.date.year, dt.date.month, dt.date.date,
                      wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);

    /// ESP32 internal timer
    auto t = time(nullptr);
    {
        auto tm = gmtime(&t);  // for UTC.
        Serial.printf("ESP32 UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n", tm->tm_year + 1900, tm->tm_mon + 1,
                      tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
        M5.Display.setCursor(0, 60);
        M5.Display.printf("ESP32 UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1,
                          tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
    }

    {
        auto tm = localtime(&t);  // for local timezone.
        Serial.printf("ESP32 %s:%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n", NTP_TIMEZONE, tm->tm_year + 1900,
                      tm->tm_mon + 1, tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
        M5.Display.setCursor(0, 120);
        M5.Display.printf("ESP32 %s:%04d/%02d/%02d (%s)  %02d:%02d:%02d", NTP_TIMEZONE, tm->tm_year + 1900,
                          tm->tm_mon + 1, tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
} 