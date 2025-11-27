# HEX_SK6812 - Unit HEX LEDストリップ制御

M5Stack Unit HEX（SK6812 LEDストリップ）を制御して虹色のグラデーションエフェクトを表示するプログラムです。

## 📋 概要

このプログラムは、M5Stack Unit HEXに接続されたSK6812 LEDストリップ（37個のLED）を制御し、虹色のグラデーションエフェクトを表示します。色相値が時間とともに変化し、美しい虹色のアニメーションを実現します。

## 🔧 必要なハードウェア

- M5Stack Tab5（またはM5Core）
- M5Stack Unit HEX（SK6812 LEDストリップ搭載）

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```

- **FastLED**: LEDストリップ制御ライブラリ
  ```bash
  arduino-cli lib install "FastLED@3.9.10"
  ```

## 🚀 セットアップ

1. **ハードウェア接続**
   - Unit HEXをM5Stack Tab5のポートに接続
   - LEDストリップのピン: 53番ピン

2. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

3. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

プログラムをアップロードすると、自動的にLEDストリップに虹色のグラデーションエフェクトが表示されます。画面には「HEX Example」と「Display rainbow effect」のメッセージが表示されます。

## 📊 機能

- **37個のLED制御**: Unit HEXの37個のLEDを個別に制御
- **虹色グラデーション**: LEDストリップ全体に虹色のグラデーションを表示
- **アニメーション**: 色相値が時間とともに変化し、虹色が流れるように動く
- **明るさ調整**: LEDの明るさを調整可能（デフォルト: 5）

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - 電源モジュールの初期化
   - FastLEDライブラリの初期化
   - LEDストリップの設定（WS2811、GRB順）

2. **アニメーションループ**
   - 色相値（gHue）を基に虹色グラデーションを生成
   - 各LEDに色を設定（隣接LED間の色差: 7）
   - LEDストリップに色を反映
   - 20ミリ秒ごとに色相値を更新

## 📝 コードの主要なポイント

### LEDの設定
```cpp
#define Neopixel_PIN (53)  // LEDストリップのピン番号
#define NUM_LEDS     (37)  // LEDの数
```

### 色相値の更新
```cpp
EVERY_N_MILLISECONDS(20) {
    gHue++;  // 20ミリ秒ごとに色相値を増加
}
```

### グラデーション生成
```cpp
fill_rainbow(leds, NUM_LEDS, gHue, 7);
// 開始色相値: gHue
// 隣接LED間の色差: 7
```

## ⚙️ カスタマイズ

### 明るさの調整
```cpp
FastLED.setBrightness(5);  // 0-255の範囲で調整可能
```

### 色差の調整
```cpp
fill_rainbow(leds, NUM_LEDS, gHue, 7);  // 7を変更して色差を調整
```

### 更新速度の調整
```cpp
EVERY_N_MILLISECONDS(20) {  // 20を変更して更新速度を調整
    gHue++;
}
```

## 📖 参考資料

- [M5Stack Unit HEX 公式ドキュメント](https://docs.m5stack.com/)
- [FastLED ライブラリ](https://github.com/FastLED/FastLED)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)

## 📄 ライセンス

このプログラムは教育・学習目的で提供されています。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5 + Unit HEX  
**動作確認**: ✅ 正常動作確認済み

