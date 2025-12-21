# SD Card - SDカード画像表示

M5Stack Tab5のSDカードスロットを使用してファイルの読み書きと画像表示を行うプログラムです。

## 📋 概要

SDカードから画像ファイル（PNG/JPEG）を読み込んで表示します。M5GFXとSDライブラリの互換性問題を回避するため、画像をメモリバッファに読み込んでから描画する方式を採用しています。

## 🔧 必要なハードウェア

- M5Stack Tab5
- microSDカード（FAT32フォーマット推奨）
- 画像ファイル（PNG または JPEG形式）

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```

## 🚀 セットアップ

### ボード設定
- Board: ESP32P4 Dev Module
- USB CDC on boot: Enabled
- Flash Size: 16MB (128Mb)
- Partition Scheme: Custom
- PSRAM: Enabled（画像読み込みに必要）

### SDカードの準備
1. microSDカードをFAT32でフォーマット
2. 以下の画像ファイルをSDカードのルートディレクトリに配置：
   - `/tab5_test_picture_1280_x_720_01.png`
   - `/tab5_test_picture_1280_x_720_02.png`

**推奨画像サイズ**: 1280x720（Tab5の画面サイズ）

### プログラムのアップロード
Arduino IDEまたはArduino CLIでプログラムをアップロードしてください。

## 💻 使用方法

1. 画像ファイルを準備したmicroSDカードをTab5に挿入
2. プログラムをアップロード
3. 自動的に以下の処理が実行されます：
   - SDカード検出
   - テキストファイル書き込みテスト
   - 画像ファイル存在確認
   - 2つの画像を交互に表示

## 📊 機能

### SDカード操作
- **SDカード検出**: SPIインターフェースでSDカードを初期化
- **ファイル書き込み**: テキストファイルの作成と書き込み
- **ファイル存在確認**: 画像ファイルの存在をチェック

### 画像表示
- **PNG/JPEG対応**: 両形式の画像を自動判定して表示
- **メモリバッファ方式**: 互換性問題を回避
- **大容量画像対応**: PSRAMを使用して大きな画像も表示可能
- **エラーハンドリング**: ファイル読み込み失敗時のエラー表示

### デバッグ機能
- シリアルモニターに詳細な情報を出力
- ファイルサイズ、読み込み状況、描画結果を表示

## 🔍 プログラムの動作

### 初期化フロー
1. M5Unified初期化
2. シリアル通信開始（115200bps）
3. SDカード初期化（SPI、25MHz）
4. テキストファイル書き込みテスト
5. 画像ファイル存在確認

### メインループ
1. 画像1をメモリに読み込んで表示
2. 2秒待機
3. 画像2をメモリに読み込んで表示
4. 2秒待機
5. 繰り返し

### 画像読み込みフロー（`drawImageFromSD()`関数）
1. ファイルを開く
2. ファイルサイズを取得
3. メモリバッファを確保（`malloc`）
4. ファイル全体をバッファに読み込む
5. 拡張子から形式を判定（PNG/JPEG）
6. バッファから画像を描画
7. メモリを解放（`free`）

## 📝 コードの主要なポイント

### SPIピン定義
```cpp
#define SD_SPI_CS_PIN   42  // Chip Select
#define SD_SPI_SCK_PIN  43  // Clock
#define SD_SPI_MOSI_PIN 44  // Master Out Slave In
#define SD_SPI_MISO_PIN 39  // Master In Slave Out
```

### メモリバッファ経由の描画
```cpp
// SDカードから直接描画できない互換性問題を回避
uint8_t* buffer = (uint8_t*)malloc(fileSize);
file.read(buffer, fileSize);
M5.Display.drawPng(buffer, fileSize, 0, 0);
free(buffer);
```

### 自動形式判定
```cpp
if (strstr(path, ".png") || strstr(path, ".PNG")) {
    M5.Display.drawPng(buffer, fileSize, 0, 0);
} else if (strstr(path, ".jpg") || strstr(path, ".JPG")) {
    M5.Display.drawJpg(buffer, fileSize, 0, 0);
}
```

## 🎨 カスタマイズ

### 画像ファイル名の変更
```cpp
drawImageFromSD("/your_image.png");
```

### 表示時間の調整
```cpp
delay(2000);  // 2秒 → 任意の値に変更
```

### 画像位置の変更
```cpp
M5.Display.drawPng(buffer, fileSize, x, y);  // x, y座標を指定
```

### 対応形式の追加
BMP形式など他の形式も追加可能：
```cpp
M5.Display.drawBmp(buffer, fileSize, 0, 0);
```

## ⚠️ 注意事項

### メモリ容量
- 大きな画像（数MB以上）はメモリ不足になる可能性があります
- PSRAMを有効にしてください（ボード設定）
- 推奨画像サイズ: 1280x720以下

### SDカード
- FAT32フォーマット推奨
- 高速なClass 10以上のSDカードを推奨
- ファイル名は8.3形式（8文字+拡張子3文字）が安全

### 互換性
- M5GFX 0.2.15以降推奨
- ESP32-P4のSDライブラリとの互換性のため、メモリバッファ方式を採用

## 🔧 トラブルシューティング

### SDカードが検出されない
- SDカードが正しく挿入されているか確認
- FAT32でフォーマットされているか確認
- SPIピンの接続を確認

### 画像が表示されない
- ファイル名が正しいか確認（大文字小文字も区別）
- 画像形式がPNGまたはJPEGか確認
- 画像サイズが大きすぎないか確認（1280x720推奨）

### メモリ不足エラー
- PSRAM設定が有効になっているか確認
- 画像ファイルサイズを小さくする
- 画像を圧縮する

### シリアルモニターでデバッグ
```
SD card detected
Write test success
Image 01 found
Image 02 found
Drawing image 01...
File size: 123456 bytes
Successfully drew: /tab5_test_picture_1280_x_720_01.png
```

## 📖 参考資料

- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [M5GFX ライブラリ](https://github.com/m5stack/M5GFX)
- [Arduino SD ライブラリ](https://www.arduino.cc/reference/en/libraries/sd/)

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年12月6日  
**最終更新**: 2025年12月21日（メモリバッファ方式に変更）  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
