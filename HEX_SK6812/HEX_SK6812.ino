/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
 * @Hardwares: M5Core + Unit HEX
 * @Platform Version: Arduino M5Stack Board Manager v2.1.3
 * @Dependent Library:
 * M5Stack@^0.4.6: https://github.com/m5stack/M5Stack
 * FastLED@^3.9.10: https://github.com/FastLED/FastLED
 * 
 * ハードウェア: M5Core + Unit HEX
 * プラットフォーム: Arduino M5Stack Board Manager v2.1.3
 * 依存ライブラリ:
 * M5Stack@^0.4.6: https://github.com/m5stack/M5Stack
 * FastLED@^3.9.10: https://github.com/FastLED/FastLED
 */

#include <M5Unified.h>
#include "FastLED.h"

#define Neopixel_PIN (53)
#define NUM_LEDS     (37)

CRGB leds[NUM_LEDS];
uint8_t gHue = 0;  // 初期色相値

void setup()
{
    M5.begin();             // M5Stackの初期化
    M5.Power.begin();       // 電源モジュールの初期化
    M5.Lcd.setTextSize(2);  // テキストサイズを2に設定
    M5.Lcd.println("      HEX Example");
    M5.Lcd.println("Display rainbow effect");

    // Neopixel（LEDストリップ）の初期化
    FastLED.addLeds<WS2811, Neopixel_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(5);  // LEDの明るさを5に設定
}

void loop()
{
    fill_rainbow(leds, NUM_LEDS, gHue,
                 7);  // LEDストリップの0～37個のLEDをグラデーション虹色に設定
                      // 開始色相値をgHueに設定し、隣接するLED間の色差を7に設定
    FastLED.show();  // LEDの色を更新
    EVERY_N_MILLISECONDS(20)
    {
        gHue++;
    }  // 20ミリ秒ごとに実行される
}