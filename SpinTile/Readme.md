# SpinTile - スピンタイルアニメーション

M5Stack Tab5の画面に回転するタイル（タイル）アニメーションを表示するプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5の画面に回転するタイルパターンを表示します。正弦波テーブルを使用して滑らかな回転アニメーションを実現しています。

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

1. プログラムをアップロード
2. 画面に回転するタイルパターンが表示されます
3. タイルが滑らかに回転しながらアニメーションします

## 📊 機能

- **回転アニメーション**: タイルが回転しながら表示される
- **正弦波テーブル**: 高速な三角関数計算のためのルックアップテーブル
- **滑らかな動き**: フレームごとに滑らかに更新
- **パターン表示**: 幾何学的なパターンを表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - 正弦波テーブルの準備
   - 画面の設定

2. **アニメーションループ**
   - 回転角度を更新
   - 正弦波テーブルを使用して座標を計算
   - タイルを描画
   - 画面を更新

## 📝 コードの主要なポイント

### 正弦波テーブル
```cpp
static constexpr const int qsintab[256] = { ... };
```

### 高速な正弦関数
```cpp
static int isin(int i) {
    // ルックアップテーブルを使用した高速計算
}
```

### 回転アニメーション
```cpp
int angle = (frame_count * rotation_speed) & 1023;
int x = isin(angle);
int y = isin(angle + 256);
```

## ⚙️ カスタマイズ

### 回転速度の変更
```cpp
int rotation_speed = 4;  // 値を大きくすると速く回転
```

### タイルサイズの変更
```cpp
int tile_size = 50;  // タイルのサイズを変更
```

### 色の変更
```cpp
M5.Display.fillRect(x, y, size, size, TFT_BLUE);
```

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

