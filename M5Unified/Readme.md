# M5Unified - Hello World

M5Stack Tab5の基本的なHello Worldプログラムです。カウンター機能付きで、M5Unifiedライブラリの基本的な使用方法をデモンストレーションします。

## 📋 概要

このプログラムは、M5Stack Tab5の画面に「Hello World」を表示し、1秒ごとにカウンターをインクリメントして表示します。M5Unifiedライブラリの基本的な初期化と使用方法を示しています。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
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

プログラムをアップロードすると、画面に「Hello World」が表示され、その下にカウンターが1秒ごとに更新されます。シリアルモニターでも同じ情報が出力されます。

## 📊 機能

- **Hello World表示**: 画面に大きなテキストで「Hello World」を表示
- **カウンター機能**: 1秒ごとにカウンターをインクリメント
- **シリアル出力**: シリアルモニターにも情報を出力

## 🔍 プログラムの動作

1. **初期化**
   - `M5.config()`でボードに合わせた自動設定
   - `M5.begin(cfg)`でM5Stackを初期化
   - 画面に「Hello World」を表示

2. **メインループ**
   - カウンターをインクリメント
   - 画面にカウンター値を表示
   - シリアルモニターにも出力
   - 1秒待機

## 📝 コードの主要なポイント

### M5Unifiedの初期化
```cpp
auto cfg = M5.config();  // ボードに合わせた自動設定
M5.begin(cfg);           // M5Stackを初期化
```

### テキストサイズの設定
```cpp
M5.Display.setTextSize(8);  // 大きなテキストサイズ
```

### カウンター表示
```cpp
M5.Display.setCursor(100, 200);  // カーソル位置を設定
M5.Display.printf("COUNT: %d\n", count);  // カウンターを表示
```

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

