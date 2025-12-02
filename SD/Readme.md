# SD - SDカード機能

M5Stack Tab5のSDカード機能を使用して、ファイルの読み書きとPNG画像の表示を行うプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5のSDカードスロットを使用して、SDカード内のファイルを読み書きし、PNG画像を表示するデモプログラムです。テキストファイルの作成とPNG画像のループ再生を行います。

## 🔧 必要なハードウェア

- M5Stack Tab5
- microSDカード（FAT32形式）

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```
- **M5GFX**: M5Stack用グラフィックライブラリ
  ```bash
  arduino-cli lib install "M5GFX@0.2.15"
  ```
- **SD**: Arduino標準ライブラリ（Arduino IDEに含まれています）
- **SPI**: Arduino標準ライブラリ（Arduino IDEに含まれています）

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **SDカードの準備**
   - microSDカードをFAT32形式でフォーマット
   - ルートディレクトリに以下のPNG画像を配置：
     - `TestPicture01.png`（解像度: 720×1280推奨）
     - `TestPicture02.png`（解像度: 720×1280推奨）
   - SDカードをTab5に挿入（端子面はスクリーンと同じ方向）

3. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. SDカードを準備してTab5に挿入
2. プログラムをアップロード
3. プログラムが自動的に以下を実行します：
   - SDカードの検出
   - `WriteTest.txt`ファイルの作成と書き込み
   - PNG画像のループ再生

## 📊 機能

- **SDカード検出**: SDカードの有無を確認
- **ファイル書き込み**: テキストファイルの作成と書き込み
- **PNG画像表示**: SDカード内のPNG画像を表示
- **ループ再生**: 複数の画像を順番に表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - SPIバスの初期化（SDカード用ピン設定）
   - SDカードの検出

2. **ファイル操作**
   - `WriteTest.txt`ファイルを作成
   - テキストデータを書き込み

3. **画像表示ループ**
   - `TestPicture01.png`を表示
   - `TestPicture02.png`を表示
   - 繰り返し

## 📝 コードの主要なポイント

### SDカード用SPIピンの設定
```cpp
#define SD_SPI_CS_PIN   42
#define SD_SPI_SCK_PIN  43
#define SD_SPI_MOSI_PIN 44
#define SD_SPI_MISO_PIN 39
```

### SPIバスの初期化
```cpp
SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
```

### SDカードの初期化
```cpp
if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
    // エラー処理
}
```

### ファイルの書き込み
```cpp
auto file = SD.open("/WriteTest.txt", FILE_WRITE, true);
if (file) {
    file.print("Hello, world!");
    file.close();
}
```

### PNG画像の表示
```cpp
M5.Display.drawPngFile(SD, "/TestPicture01.png");
```

## 🔧 トラブルシューティング

### SDカードが検出されない場合
- SDカードがFAT32形式でフォーマットされているか確認してください
- SDカードが正しく挿入されているか確認してください
- SDカードの容量が大きすぎる場合、32GB以下を推奨します
- シリアルモニターでエラーメッセージを確認してください

### 画像が表示されない場合
- 画像ファイル名が正しいか確認してください（`TestPicture01.png`、`TestPicture02.png`）
- 画像の解像度が720×1280であることを確認してください
- PNG形式であることを確認してください

### ファイルが書き込めない場合
- SDカードが書き込み可能か確認してください（書き込み保護スイッチ）
- SDカードの空き容量を確認してください

## 📖 参考資料

- [SD Library | Arduino Documentation](https://www.arduino.cc/reference/en/libraries/sd/)
- [M5GFX API - drawPngFile](https://github.com/m5stack/M5GFX)

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
