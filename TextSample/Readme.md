# TextSample - テキストサンプル

M5Stack Tab5の画面に様々なテキスト表示のサンプルを表示するプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5の画面に様々なテキスト表示のサンプルを表示します。複数のフォント、テキストスタイル、表示方法のデモンストレーションを提供します。

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
2. 画面に様々なテキストサンプルが表示されます
3. 異なるフォント、サイズ、スタイルのテキストが表示されます

## 📊 機能

- **複数フォント表示**: 様々なフォントでテキストを表示
- **テキストスタイル**: 異なるスタイルのテキスト表示
- **文字列操作**: 文字列の操作と表示の例
- **表示位置**: 様々な位置へのテキスト配置

## 🔍 プログラムの動作

1. **初期化**
   - M5GFXディスプレイの初期化
   - 画面の設定

2. **テキスト表示**
   - 複数のテキストサンプルを表示
   - 異なるフォントとスタイルで表示
   - 画面に描画

## 📝 コードの主要なポイント

### テキスト配列
```cpp
static constexpr const char* text[] = { 
    "hello world", "this", "is", "text", 
    "log", "vertical", "scroll", "sample" 
};
```

### テキストの表示
```cpp
display.drawString(text[i], x, y, &font);
```

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

