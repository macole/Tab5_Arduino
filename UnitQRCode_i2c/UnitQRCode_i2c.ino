/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file i2c_mode.ino
 * @brief Unit QRCode I2C Mode Example
 * @version 0.2
 * @date 2024-06-19
 *
 *
 * @Hardwares: M5Core + Unit QRCode
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5UnitQRCode: https://github.com/m5stack/M5Unit-QRCode
 */

#include <M5Unified.h>
#include <M5GFX.h>
#include "M5UnitQRCode.h"

M5Canvas canvas(&M5.Display);

M5UnitQRCodeI2C qrcode;

#define I2C_AUTO_SCAN_MODE

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);
    Wire.begin();  // I2C通信の明示的な初期化

    canvas.setColorDepth(1);  // mono color
    canvas.createSprite(M5.Display.width(), M5.Display.height());
    canvas.setFont(&fonts::lgfxJapanMinchoP_16);  // 適切なサイズのフォントを設定
    canvas.setTextSize(1.5);  // フォントサイズを1.5に設定（重ならないように）
    canvas.setTextScroll(true);

    // 初期化メッセージを表示
    canvas.fillScreen(0);
    canvas.setTextColor(1, 0);
    canvas.setCursor(0, 0);
    canvas.println("Unit QRCode I2C");
    canvas.println("Initializing...");
    canvas.pushSprite(0, 0);

    // Tab5のデフォルトI2Cピンを使用（-1でデフォルト）
    int retry_count = 0;
    while (!qrcode.begin(&Wire, UNIT_QRCODE_ADDR, -1, -1, 100000U)) {
        retry_count++;
        canvas.fillScreen(0);
        canvas.setCursor(0, 0);
        canvas.setTextColor(1, 0);
        canvas.printf("Init Fail! (retry: %d)\n", retry_count);
        canvas.println("Check connection:");
        canvas.println("I2C Addr: 0x29");
        canvas.println("SDA/SCL: Default");
        Serial.printf("Unit QRCode I2C Init Fail (retry: %d)\r\n", retry_count);
        canvas.pushSprite(0, 0);
        delay(1000);
        
        // 10回リトライしても失敗したら警告を表示して続行
        if (retry_count >= 10) {
            canvas.println("\nContinuing anyway...");
            canvas.pushSprite(0, 0);
            delay(2000);
            break;
        }
    }

    if (retry_count < 10) {
        canvas.fillScreen(0);
        canvas.setCursor(0, 0);
        canvas.setTextColor(1, 0);
        canvas.println("Unit QRCode I2C");
        canvas.println("Init Success!");
        Serial.println("Unit QRCode I2C Init Success");
    }
    
#ifdef I2C_AUTO_SCAN_MODE
    canvas.println("\nMode: Auto Scan");
    canvas.println("Waiting for QR code...");
    canvas.pushSprite(0, 0);
    qrcode.setTriggerMode(AUTO_SCAN_MODE);
#else
    canvas.println("\nMode: Manual Scan");
    canvas.println("Touch screen to scan");
    canvas.pushSprite(0, 0);
    qrcode.setTriggerMode(MANUAL_SCAN_MODE);
#endif
}

void loop() {
    M5.update();
    
    // QRコード読み取り状態の表示
    static uint32_t last_status_update = 0;
    if (millis() - last_status_update > 500) {
        uint8_t status = qrcode.getDecodeReadyStatus();
        int status_y = M5.Display.height() - 30;  // ステータス表示位置
        canvas.setCursor(0, status_y);
        canvas.setTextColor(1, 0);
        canvas.fillRect(0, status_y, M5.Display.width(), 30, 0);
        if (status == 1) {
            canvas.println("Status: Decoded!");
        } else {
            canvas.println("Status: Scanning...");
        }
        canvas.pushSprite(0, 0);
        last_status_update = millis();
    }
    
    if (qrcode.getDecodeReadyStatus() == 1) {
        uint8_t buffer[512] = {0};
        uint16_t length     = qrcode.getDecodeLength();
        
        // シリアルモニターに出力
        Serial.printf("len:%d\r\n", length);
        Serial.printf("decode data: ");
        
        // 画面に表示（既存の内容をクリアしてから表示）
        canvas.fillRect(0, 80, M5.Display.width(), M5.Display.height() - 110, 0);
        canvas.setCursor(0, 80);
        canvas.setTextColor(1, 0);
        canvas.println("QR Code Detected!");
        canvas.printf("Length: %d bytes\n", length);
        canvas.println("Data:");
        
        qrcode.getDecodeData(buffer, length);
        // データを表示（長い場合は折り返し）
        int chars_per_line = M5.Display.width() / 12;  // フォントサイズに応じた1行の文字数
        for (int i = 0; i < length; i++) {
            Serial.printf("%c", buffer[i]);
            canvas.printf("%c", buffer[i]);
            // 一定文字数ごとに改行（画面幅を考慮）
            if ((i + 1) % chars_per_line == 0 && i < length - 1) {
                canvas.println();
            }
        }
        Serial.println();
        canvas.println();
        canvas.println("---");
        canvas.pushSprite(0, 0);
        
        // デコード完了後、次のスキャンのためにリセット
        delay(1000);
    }
    
#ifndef I2C_AUTO_SCAN_MODE
    // Tab5ではタッチ操作でスキャン開始/停止
    auto touch_count = M5.Touch.getCount();
    if (touch_count > 0) {
        auto t = M5.Touch.getDetail(0);
        if (t.wasPressed()) {
            // タッチでスキャン開始
            qrcode.setDecodeTrigger(1);
            canvas.fillRect(0, 80, M5.Display.width(), 60, 0);
            canvas.setCursor(0, 80);
            canvas.setTextColor(1, 0);
            canvas.println("Scan started...");
            canvas.pushSprite(0, 0);
        }
    }
    
    // 長押しでスキャン停止
    if (touch_count > 0) {
        auto t = M5.Touch.getDetail(0);
        if (t.wasHold()) {
            qrcode.setDecodeTrigger(0);
            canvas.fillRect(0, 80, M5.Display.width(), 60, 0);
            canvas.setCursor(0, 80);
            canvas.setTextColor(1, 0);
            canvas.println("Scan stopped.");
            canvas.pushSprite(0, 0);
        }
    }
#endif
}