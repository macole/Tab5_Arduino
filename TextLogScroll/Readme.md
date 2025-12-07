# TextLogScroll - テキストログの縦スクロール

M5Stack Tab5の画面に複数のテキスト行を縦方向にスクロール表示するプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5の画面に複数のテキスト行を縦方向にスクロール表示します。ログ形式のテキストを上から下へスクロールさせて表示します。キャンバス（スプライト）を使用して滑らかな表示を実現しています。

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
2. 画面に複数のテキスト行が縦方向にスクロール表示されます
3. テキストが画面下端に達すると、最初から繰り返します

## 📊 機能

- **縦スクロール**: 複数のテキスト行を縦方向にスクロール表示
- **ログ形式**: 複数行のテキストを順番に表示
- **キャンバス使用**: スプライトを使用した滑らかな表示
- **EPD対応**: 電子ペーパーディスプレイ（EPD）モードに対応

## 🔍 プログラムの動作

1. **初期化**
   - M5GFXディスプレイの初期化
   - キャンバス（スプライト）の作成
   - EPDモードの設定（EPDディスプレイの場合）

2. **スクロールループ**
   - 各テキスト行の位置を更新
   - キャンバスにテキスト行を描画
   - キャンバスを画面に転送
   - スクロール速度に応じて待機

## 📝 コードの主要なポイント

### テキスト配列の定義
```cpp
static constexpr const char* text[] = { 
    "hello world", "this", "is", "text", 
    "log", "vertical", "scroll", "sample" 
};
```

### キャンバスの作成
```cpp
M5Canvas canvas(&display);
canvas.createSprite(display.width(), display.height());
```

### 縦スクロールの実装
```cpp
for (int i = 0; i < text_count; i++) {
    int y = base_y + i * line_height - scroll_pos;
    if (y >= 0 && y < display.height()) {
        canvas.drawString(text[i], x, y);
    }
}
```

## ⚙️ カスタマイズ

### テキスト行の追加
```cpp
static constexpr const char* text[] = { 
    "line 1", "line 2", "line 3", "line 4" 
};
```

### スクロール速度の変更
```cpp
scroll_pos += scroll_speed;  // scroll_speedの値を変更
```

### 行間隔の変更
```cpp
int line_height = 30;  // 行間隔を変更
```

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

