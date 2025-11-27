# Phomemo T02 調査結果ドキュメント

**調査日**: 2025年11月  
**調査目的**: Phomemo T02の仕様、M5Stack/ESP32での実装例、ライブラリの調査

---

## 📋 Phomemo T02 基本仕様

### 物理仕様

| 項目 | 仕様 |
|------|------|
| **サイズ** | 約86-87mm × 84-90mm × 40-42mm |
| **重量** | 約150-255g |
| **印刷方式** | 感熱式（サーマル） |
| **解像度** | 203 DPI |
| **用紙幅** | 50-53mm（感熱ロール紙） |
| **バッテリー** | 1000mAh リチウム電池（充電式） |
| **充電時間** | 約2時間 |
| **充電方式** | マイクロUSB |

### 接続仕様

| 項目 | 仕様 |
|------|------|
| **接続方法** | Bluetooth 4.0以上（4.2対応） |
| **対応OS** | iOS 8.0以上、Android 4.0以上 |
| **専用アプリ** | Phomemo（App Store / Google Play） |
| **Bluetoothプロファイル** | SPP (Serial Port Profile) 推定 |
| **通信プロトコル** | ESC/POS互換（推定、未確認） |

### 機能

- インク不要の感熱式印刷
- 写真、メモ、ラベル、シールの印刷
- 多彩なテンプレートと編集機能
- コンパクトで持ち運び可能
- 静音設計

---

## 🔍 調査結果サマリー

### ✅ 確認できた情報

1. **Phomemo T02の基本仕様**
   - 物理サイズ、重量、解像度、用紙幅など
   - Bluetooth接続方式
   - 専用アプリ「Phomemo」の存在

2. **一般的な仕様**
   - 感熱式プリンター
   - 203 DPI解像度
   - 50-53mm用紙幅

### ❌ 見つからなかった情報

1. **M5Stack/ESP32での実装例**
   - 公開されている実装例やサンプルコードは見つかりませんでした
   - GitHubでの関連プロジェクトも見つかりませんでした
   - Arduino/ESP32フォーラムでも関連情報は見つかりませんでした

2. **専用ライブラリ**
   - Phomemo T02専用のArduino/ESP32ライブラリは存在しないようです
   - 公式SDKや開発者向けAPIも公開されていないようです
   - サードパーティ製のライブラリも見つかりませんでした

3. **通信プロトコル詳細**
   - 公式のプロトコル仕様書は公開されていない
   - ESC/POSコマンドの対応状況は不明
   - Bluetoothプロファイルの詳細も不明（SPPかBLEか）

4. **開発者向け情報**
   - 公式の開発者向けドキュメントは見つかりませんでした
   - API仕様やSDKの提供は確認できませんでした

---

## 🔧 ESP32 Bluetooth実装に関する情報

### ESP32 Bluetooth機能

ESP32は以下のBluetooth機能をサポートしています：

1. **Bluetooth Classic (SPP)**
   - Serial Port Profile (SPP) によるシリアル通信
   - `BluetoothSerial` ライブラリを使用
   - 一般的なBluetoothプリンターとの通信に使用可能

2. **Bluetooth Low Energy (BLE)**
   - 低消費電力通信
   - BLE Serial ライブラリを使用
   - Phomemo T02がBLE対応かは不明

### ESP32 BluetoothSerial ライブラリ

```cpp
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  SerialBT.begin("ESP32_Device"); // デバイス名
  // または
  SerialBT.begin("ESP32_Device", true); // マスターモード
}

void loop() {
  if (SerialBT.available()) {
    String data = SerialBT.readString();
    // データ処理
  }
  
  SerialBT.println("Hello from ESP32");
}
```

### 接続方法

1. **マスターモード（ESP32が接続を開始）**
   ```cpp
   SerialBT.begin("ESP32", true);
   SerialBT.connect("Phomemo_T02_MAC");
   ```

2. **スレーブモード（他のデバイスからの接続を待機）**
   ```cpp
   SerialBT.begin("ESP32");
   // 他のデバイスからの接続を待機
   ```

---

## 🖨️ ESC/POS コマンド仕様

### 一般的なサーマルプリンター（58mm幅、203 DPI）

Phomemo T02は一般的なESC/POSコマンドセットを使用している可能性が高いです。

### 主要なESC/POSコマンド

#### 1. 初期化コマンド

```
ESC @ (0x1B 0x40)
- プリンターを初期化
- すべての設定をリセット
```

#### 2. テキスト制御

```
LF (0x0A)
- 改行

CR (0x0D)
- キャリッジリターン

ESC E n (0x1B 0x45 n)
- 太字設定
- n=1: 太字ON, n=0: 太字OFF

ESC a n (0x1B 0x61 n)
- テキスト配置
- n=0: 左揃え, n=1: 中央揃え, n=2: 右揃え

GS ! n (0x1D 0x21 n)
- フォントサイズ設定
- n=0x00: 通常, n=0x11: 2倍（幅・高さ）
- n=0x01: 高さ2倍, n=0x10: 幅2倍
```

#### 3. 用紙カット

```
GS V m (0x1D 0x56 m)
- 用紙カット
- m=0: 完全カット, m=1: 部分カット
- m=65: 完全カット（nドット送紙後）
- m=66: 部分カット（nドット送紙後）

GS V m n (0x1D 0x56 m n)
- nドット送紙後にカット
```

#### 4. 画像印刷

```
GS v 0 (0x1D 0x76 0)
- ラスター画像印刷
- 画像データを送信

GS ( L (0x1D 0x28 0x4C)
- ラスター画像コマンド
```

#### 5. その他の制御

```
ESC d n (0x1B 0x64 n)
- n行送紙

ESC J n (0x1B 0x4A n)
- nドット送紙

GS H n (0x1D 0x48 n)
- バーコードの位置
- n=0: 上, n=1: 下, n=2: 上下
```

### 画像印刷の形式

58mm幅、203 DPIの場合：
- **印刷幅**: 384 dots (48mm)
- **画像形式**: 1ビットビットマップ
- **データ形式**: 縦方向8ドット単位（1バイト）

画像データの送信例：
```
GS v 0 (0x1D 0x76 0x30)
[画像データのサイズ（2バイト、リトルエンディアン）]
[画像データ（384 dots × 高さ）]
```

---

## 📚 参考になるライブラリとリソース

### 1. ESP32 BluetoothSerial

- **公式ドキュメント**: ESP-IDF BluetoothSerial API
- **Arduino Core**: ESP32 Arduino Coreに標準搭載
- **参考URL**: https://github.com/espressif/arduino-esp32

### 2. 一般的なESC/POSライブラリ

#### Arduino Thermal Printer Library
- **GitHub**: https://github.com/adafruit/Adafruit-Thermal-Printer-Library
- **説明**: Adafruit製のサーマルプリンターライブラリ
- **注意**: 主にシリアル接続用（Bluetooth対応は要確認）

#### ESC/POS Command Reference
- **Epson公式**: https://reference.epson-biz.com/modules/ref_escpos/
- **説明**: ESC/POSコマンドの完全なリファレンス

### 3. サーマルプリンター実装例

一般的な58mmサーマルプリンターの実装例：
- GitHubで「thermal printer arduino」で検索
- 「ESP32 bluetooth printer」で検索

---

## 🎯 実装アプローチ

### Phase 1: Bluetooth接続の確立

1. **デバイス検出**
   ```cpp
   // Bluetoothスキャン（ESP32では制限あり）
   // または、MACアドレスを直接指定
   ```

2. **接続確立**
   ```cpp
   BluetoothSerial SerialBT;
   SerialBT.begin("M5Stack_Tab5", true); // マスターモード
   SerialBT.connect("Phomemo_T02_MAC");
   ```

3. **接続確認**
   ```cpp
   if (SerialBT.hasClient()) {
     // 接続成功
   }
   ```

### Phase 2: 基本的なテキスト印刷

1. **初期化コマンド送信**
   ```cpp
   SerialBT.write(0x1B); // ESC
   SerialBT.write(0x40); // @
   ```

2. **テキスト送信**
   ```cpp
   SerialBT.print("Hello World\n");
   SerialBT.write(0x0A); // LF
   ```

3. **用紙カット**
   ```cpp
   SerialBT.write(0x1D); // GS
   SerialBT.write(0x56); // V
   SerialBT.write(0x00); // 完全カット
   ```

### Phase 3: 高度な機能

1. **フォントサイズ変更**
2. **テキスト配置**
3. **画像印刷**
4. **日本語対応**

---

## ⚠️ 実装時の注意点

### 1. プロトコル未公開

- Phomemo T02の公式プロトコル仕様は公開されていない
- 一般的なESC/POSコマンドを試行する必要がある
- 実機での動作確認が必須

### 2. Bluetooth接続

- Phomemo T02がBluetooth Classic (SPP) をサポートしているか確認
- BLEのみの対応の可能性もある
- 接続時のペアリングPINが必要な場合がある

### 3. 文字コード

- 日本語文字のエンコーディング（UTF-8、Shift-JIS等）
- Phomemo T02が対応する文字コードを確認
- 文字コード変換が必要な可能性

### 4. 画像形式

- 1ビットビットマップへの変換
- 384 dots幅へのリサイズ
- 縦方向8ドット単位のデータ形式

### 5. タイミング制御

- コマンド送信後の待機時間
- 印刷完了の検出（可能であれば）
- エラーハンドリング

---

## 🔬 推奨される調査方法

### 1. プロトコル解析

- Phomemo公式アプリの通信を解析
- Bluetoothパケットキャプチャツールを使用
- Wireshark、nRF Connect等

### 2. 実機テスト

- 基本的なESC/POSコマンドを順次試行
- 動作するコマンドを特定
- エラーレスポンスの確認

### 3. 公式サポートへの問い合わせ

- Phomemo公式サポートに開発者向け情報を問い合わせ
- APIやSDKの提供の有無を確認
- 技術文書の提供を依頼

### 4. コミュニティでの情報収集

- GitHubでの関連プロジェクト検索
- Arduino/ESP32フォーラムでの情報収集
- Reddit、Stack Overflow等での質問

---

## 📖 参考URL

### Phomemo T02関連

- Phomemo公式サイト: https://phomemo.com/
- 製品ページ（日本語）: https://phomemo.com/ja-br/products/phomemo-t02-inkless-mini-sticker-printer
- 製品ページ（英語）: https://phomemo.com/lg/products/t02-portable-printer
- レビュー記事: https://ken-jiji.com/phomemo-t02-review/
- YouTubeレビュー動画: 
  - https://www.youtube.com/watch?v=VgdwNdBU6Uk
  - https://www.youtube.com/watch?v=eRgLO4ZxoaY

### ESP32 Bluetooth関連

- ESP32 Arduino Core: https://github.com/espressif/arduino-esp32
- ESP-IDF Bluetooth API: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/

### ESC/POS関連

- Epson ESC/POS Reference: https://reference.epson-biz.com/modules/ref_escpos/
- Adafruit Thermal Printer Library: https://github.com/adafruit/Adafruit-Thermal-Printer-Library

### M5Stack関連

- M5Stack公式ドキュメント: https://docs.m5stack.com/
- M5Unified GitHub: https://github.com/m5stack/M5Unified

---

## 📝 調査結果のまとめ

### 結論

1. **Phomemo T02の基本仕様は確認できた**
   - 物理仕様、接続仕様、機能は明確

2. **M5Stack/ESP32での実装例は見つからなかった**
   - 公開されている実装例やライブラリは存在しない
   - 独自実装が必要

3. **一般的なESC/POSコマンドの使用が推測される**
   - 58mm幅、203 DPIの一般的なサーマルプリンター
   - 標準的なESC/POSコマンドセットを使用している可能性が高い

4. **実装にはプロトコル解析が必要**
   - 公式仕様が公開されていないため
   - 実機での動作確認とコマンド試行が必要

### 次のステップ

1. **実機でのプロトコル解析**
   - Bluetooth通信のキャプチャ
   - コマンドの特定

2. **基本的な接続テスト**
   - ESP32 BluetoothSerialでの接続試行
   - 基本的なESC/POSコマンドの送信

3. **段階的な機能実装**
   - テキスト印刷から開始
   - 画像印刷、日本語対応へ拡張

---

**調査完了日**: 2025年11月  
**調査者**: AI Assistant  
**次回更新**: 実機テスト結果に基づいて更新予定

