# LongTextScroll - 長いテキストのスクロール表示

M5Stack Tab5の画面に長いテキストを横スクロールで表示するプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5の画面に長いテキストを横方向にスクロール表示します。画面幅を超える長いテキストを滑らかにスクロールさせて表示します。日本語テキストにも対応しています。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5GFX**: M5Stack用グラフィックライブラリ
  ```bash
  arduino-cli lib install "M5GFX@0.2.15"
  ```

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. プログラムをアップロード
2. 画面に長いテキストが横方向にスクロール表示されます
3. テキストが画面端に達すると、最初から繰り返します

## 📊 機能

- **横スクロール**: 長いテキストを横方向にスクロール表示
- **日本語対応**: 日本語テキストの表示に対応
- **EPD対応**: 電子ペーパーディスプレイ（EPD）モードに対応
- **滑らかなスクロール**: ピクセル単位で滑らかにスクロール

## 🔍 プログラムの動作

1. **初期化**
   - M5GFXディスプレイの初期化
   - キャンバス（スプライト）の作成
   - EPDモードの設定（EPDディスプレイの場合）

2. **スクロールループ**
   - テキストの位置を更新
   - キャンバスにテキストを描画
   - キャンバスを画面に転送
   - スクロール速度に応じて待機

## 📝 コードの主要なポイント

### テキストの定義
```cpp
static constexpr char text[] = "Hello world ! こんにちは世界！ this is long long string sample...";
```

### スクロール位置の更新
```cpp
textpos -= scrollstep;  // スクロール位置を更新
if (textpos < -textlen * 8) {  // テキストが画面外に出たら
    textpos = display.width();  // 最初から繰り返し
}
```

### キャンバスへの描画
```cpp
canvas.fillRect(0, 0, width, height, BLACK);  // 背景をクリア
canvas.setTextColor(WHITE);
canvas.drawString(text, textpos, y);  // テキストを描画
```

### EPDモードの設定
```cpp
if (display.isEPD()) {
    scrollstep = 16;  // EPDでは高速スクロール
    display.setEpdMode(epd_mode_t::epd_fastest);
}
```

## ⚙️ カスタマイズ

### スクロール速度の変更
```cpp
int scrollstep = 2;  // 値を大きくすると速くスクロール
```

### テキストの変更
```cpp
static constexpr char text[] = "あなたのテキストをここに";
```

### フォントの変更
```cpp
canvas.setFont(&fonts::FreeMonoBold24pt7b);
```

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

