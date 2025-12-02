# UnitRFID2_UID - RFID UID表示（シンプル版）

M5Stack Tab5とUnit RFID2を使用してRFIDカードのUIDのみを表示するシンプルなプログラムです。

## 📋 概要

このプログラムは、M5Stack Unit RFID2を使用してRFIDカードやタグを読み取り、UID（一意の識別子）のみを画面に表示します。`UnitRFID2.ino`のシンプル版で、PICCタイプのチェックなどを行わず、UIDのみを表示します。

## 🔧 必要なハードウェア

- M5Stack Tab5
- M5Stack Unit RFID2
- RFIDカードまたはタグ

## 📚 必要なライブラリ

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

1. **RFIDカードの読み取り**
   - Unit RFID2の上にRFIDカードまたはタグをかざす
   - 画面にUIDが表示されます

2. **表示形式**
   - `[UID]`の後に16進数形式でUIDが表示されます
   - 例: `[UID] 04 A5 3B 2C`

## 📊 機能

- **UID表示**: RFIDカードのUIDを16進数形式で表示
- **シンプルな実装**: 最小限のコードで動作
- **リアルタイム読み取り**: カードをかざすと即座に表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - I2C通信の初期化（`Wire.begin()`）
   - MFRC522の初期化

2. **カード検出ループ**
   - 新しいカードが検出されたか確認
   - カードのシリアル番号を読み取り
   - UIDを16進数形式で表示

## 📝 コードの主要なポイント

### I2C通信の初期化
```cpp
Wire.begin();  // I2C通信の初期化（必須）
```

### カード検出
```cpp
if(!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(200);
    return;
}
```

### UIDの表示
```cpp
M5.Lcd.print("[UID]");
for (byte i=0; i<mfrc522.uid.size; i++) {
    M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
}
```

## 🔗 関連ファイル

- `UnitRFID2.ino`: 詳細版（PICCタイプ表示、MIFARE Classicチェック付き）

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

