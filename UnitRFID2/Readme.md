# UnitRFID2 - RFIDカードリーダーサンプル

M5Stack Tab5とUnit RFID2を使用してRFIDカードを読み取り、PICCタイプとUIDを表示するサンプルプログラムです。

## 📋 概要

このプログラムは、M5Stack Unit RFID2を使用してRFIDカードやタグを読み取り、以下の情報を表示します：

- **PICCタイプ**: カードの種類（MIFARE Classic、MIFARE Mini、MIFARE 4Kなど）
- **UID**: カードの一意の識別子（16進数形式）

情報は**シリアルモニター**と**画面**の両方に表示されます。

## 🔧 必要なハードウェア

- M5Stack Tab5
- M5Stack Unit RFID2
- RFIDカードまたはタグ（MIFARE Classic対応）

## 📚 必要なライブラリ

以下のライブラリが必要です：

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```

- **MFRC522_I2C**: MFRC522用I2Cライブラリ
  ```bash
  arduino-cli lib install "MFRC522_I2C"
  ```

## 🚀 セットアップ

1. **ハードウェア接続**
   - Unit RFID2をM5Stack Tab5のI2Cポートに接続
   - I2Cアドレス: `0x28`

2. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

3. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. **シリアルモニターの起動**
   - ボーレート: 115200 baud
   - ツール > シリアルモニターで確認

2. **RFIDカードの読み取り**
   - Unit RFID2の上にRFIDカードまたはタグをかざす
   - 画面とシリアルモニターに情報が表示されます

## 📊 表示される情報

### 画面表示

```
      RFID2 Example
Tool>SerialMonitor 115200baud
PICC type: MIFARE 1KB
UID: XX XX XX XX
```

### シリアルモニター出力

```
PICC type: MIFARE 1KB
XX XX XX XX 

```

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - 電源モジュールの初期化
   - I2C通信の初期化（`Wire.begin()`）
   - MFRC522の初期化

2. **カード検出**
   - `PICC_IsNewCardPresent()`: 新しいカードが検出されたか確認
   - `PICC_ReadCardSerial()`: カードのシリアル番号を読み取り

3. **情報取得**
   - PICCタイプの取得と表示
   - MIFARE Classicタイプのチェック
   - UIDの取得と表示

## ⚠️ トラブルシューティング

### 問題1: 画面に初期メッセージしか表示されない

**症状**: 初期メッセージ（"RFID2 Example"など）は表示されるが、RFID読み取り時のメッセージが表示されない

**原因と解決策**:

1. **I2C通信の初期化不足**
   - `Wire.begin()`が呼ばれていない可能性
   - **解決策**: `setup()`関数に`Wire.begin()`を追加



### 問題2: カードが検出されない

**症状**: RFIDカードをかざしても反応がない

**解決策**:
- Unit RFID2が正しく接続されているか確認
- I2Cアドレス（0x28）が正しいか確認
- カードとUnit RFID2の距離を調整（数mm～数cm）
- 別のRFIDカードで試す

### 問題3: "Not MIFARE Classic"と表示される

**症状**: カードは検出されるが、MIFARE Classicタイプでないと表示される

**説明**: このプログラムはMIFARE Classicタイプのカードのみを処理します。他のタイプのカード（NTAG、FeliCaなど）ではこのメッセージが表示されます。

**解決策**: MIFARE Classic対応のカードを使用してください。

### 問題4: シリアルモニターに何も表示されない

**解決策**:
- ボーレートが115200に設定されているか確認
- シリアルモニターが正しいポートに接続されているか確認
- `Serial.begin(115200)`が`setup()`で呼ばれているか確認

## 📝 コードの主要なポイント

### I2C通信の初期化
```cpp
Wire.begin();  // I2C通信の初期化（必須）
```
**重要**: `Wire.begin()`を呼ばないと、MFRC522との通信が確立されません。

### カード検出のロジック
```cpp
if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
  // カードが検出された時の処理
}
```
両方の条件が満たされた場合のみ処理を実行します。

## 🔗 関連ファイル

- `UnitRFID2.ino`: メインプログラム
- `UnitRFID2_UID.ino`: シンプルなUID表示版（参考用）

## 📖 参考資料

- [M5Stack Unit RFID2 公式ドキュメント](https://docs.m5stack.com/)
- [MFRC522 データシート](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5 + Unit RFID2  
**動作確認**: ✅ 正常動作確認済み

