#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <M5Unified.h>
#include <M5GFX.h>

#define SD_SPI_CS_PIN   42
#define SD_SPI_SCK_PIN  43
#define SD_SPI_MOSI_PIN 44
#define SD_SPI_MISO_PIN 39

void setup()
{
    M5.begin();

    M5.Display.setRotation(1);
    M5.Display.setFont(&fonts::FreeMonoBold18pt7b);

    // SDカード初期化
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        // SDカードの初期化に失敗、または存在しない場合にメッセージを表示
        M5.Display.print("\n SD card not detected\n");
        while (1);
    } else {
        M5.Display.print("\n SD card detected\n");
    }
    delay(1000);

    // TXTファイルを書き込み
    M5.Display.print("\n SD card write test...\n");
    auto file = SD.open("/WriteTest.txt", FILE_WRITE, true);
    if (file) {
        file.print("Hello, world! \nSD card write success! \n");
        file.close();
        M5.Display.print(" SD card write success\n");
    } else {
        M5.Display.print(" Failed to create TXT file\n");
    }
    delay(1000);

    M5.Display.print("\n SD card read test...\n");
    if (SD.open("/TestPicture01.png", FILE_READ, false)) {
        M5.Display.print(" PNG file 01 detected\n");
    } else {
        M5.Display.print(" PNG file 01 not detected\n");
    }
    if (SD.open("/TestPicture02.png", FILE_READ, false)) {
        M5.Display.print(" PNG file 02 detected\n");
    } else {
        M5.Display.print(" PNG file 02 not detected\n");
    }
}

void loop()
{
    // PNGファイルを読み込み画像を描画
    M5.Display.drawPngFile(SD, "/tab5_test_picture_1280_x_720_01.png");
    delay(1000);
    M5.Display.drawPngFile(SD, "/tab5_test_picture_1280_x_720_02.png");
    delay(1000);
} 