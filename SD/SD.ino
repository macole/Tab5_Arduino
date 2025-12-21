/*
 * SD Card - SDカード画像表示
 * 
 * M5Stack Tab5のSDカードスロットを使用して画像を表示します。
 * PNG/JPEG画像をメモリに読み込んでから描画します。
 */

#include <M5Unified.h>
#include <SPI.h>
#include <SD.h>

// SDカード用SPIピン定義
#define SD_SPI_CS_PIN   42
#define SD_SPI_SCK_PIN  43
#define SD_SPI_MOSI_PIN 44
#define SD_SPI_MISO_PIN 39

// 画像をメモリに読み込んで描画する関数
bool drawImageFromSD(const char* path) {
    File file = SD.open(path, FILE_READ);
    if (!file) {
        Serial.printf("Failed to open: %s\n", path);
        return false;
    }
    
    size_t fileSize = file.size();
    Serial.printf("File size: %d bytes\n", fileSize);
    
    // ファイルをメモリに読み込む
    uint8_t* buffer = (uint8_t*)malloc(fileSize);
    if (!buffer) {
        Serial.println("Failed to allocate memory");
        file.close();
        return false;
    }
    
    size_t bytesRead = file.read(buffer, fileSize);
    file.close();
    
    if (bytesRead != fileSize) {
        Serial.println("Failed to read file completely");
        free(buffer);
        return false;
    }
    
    // バッファから画像を描画
    bool result = false;
    if (strstr(path, ".png") || strstr(path, ".PNG")) {
        result = M5.Display.drawPng(buffer, fileSize, 0, 0);
    } else if (strstr(path, ".jpg") || strstr(path, ".JPG") || 
               strstr(path, ".jpeg") || strstr(path, ".JPEG")) {
        result = M5.Display.drawJpg(buffer, fileSize, 0, 0);
    }
    
    free(buffer);
    
    if (result) {
        Serial.printf("Successfully drew: %s\n", path);
    } else {
        Serial.printf("Failed to draw: %s\n", path);
    }
    
    return result;
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    
    Serial.begin(115200);
    delay(100);

    M5.Display.setRotation(1);
    M5.Display.setFont(&fonts::FreeMonoBold18pt7b);

    // SDカード初期化（25MHz）
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        M5.Display.print("\n SD card not detected\n");
        Serial.println("SD card not detected");
        while (1) {
            delay(1000);
        }
    } else {
        M5.Display.print("\n SD card detected\n");
        Serial.println("SD card detected");
    }
    delay(1000);

    // テキストファイルを書き込み
    M5.Display.print("\n SD card write test...\n");
    File file = SD.open("/WriteTest.txt", FILE_WRITE, true);
    if (file) {
        file.print("Hello, world!\nSD card write success!\n");
        file.close();
        M5.Display.print(" SD card write success\n");
        Serial.println("Write test success");
    } else {
        M5.Display.print(" Failed to create TXT file\n");
        Serial.println("Write test failed");
    }
    delay(1000);

    // 画像ファイルの存在確認
    M5.Display.print("\n Image file check...\n");
    if (SD.exists("/tab5_test_picture_1280_x_720_01.png")) {
        M5.Display.print(" Image 01 found\n");
        Serial.println("Image 01 found");
    } else {
        M5.Display.print(" Image 01 not found\n");
        Serial.println("Image 01 not found");
    }
    if (SD.exists("/tab5_test_picture_1280_x_720_02.png")) {
        M5.Display.print(" Image 02 found\n");
        Serial.println("Image 02 found");
    } else {
        M5.Display.print(" Image 02 not found\n");
        Serial.println("Image 02 not found");
    }
    
    delay(2000);
    M5.Display.clear();
}

void loop() {
    // 画像1を表示
    Serial.println("Drawing image 01...");
    if (drawImageFromSD("/tab5_test_picture_1280_x_720_01.png")) {
        delay(2000);
    } else {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setCursor(400, 360);
        M5.Display.setTextColor(TFT_RED);
        M5.Display.print("Image 01 failed to load");
        delay(2000);
    }
    
    // 画像2を表示
    Serial.println("Drawing image 02...");
    if (drawImageFromSD("/tab5_test_picture_1280_x_720_02.png")) {
        delay(2000);
    } else {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setCursor(400, 360);
        M5.Display.setTextColor(TFT_RED);
        M5.Display.print("Image 02 failed to load");
        delay(2000);
    }
}
