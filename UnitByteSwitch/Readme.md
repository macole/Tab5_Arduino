# UnitByteSwitch - Unit ByteSwitch制御

M5Stack Tab5とUnit ByteSwitchを使用して8つのスイッチとLEDを制御するプログラムです。

## 📋 概要

このプログラムは、M5Stack Unit ByteSwitchを制御して、8つのスイッチの状態を読み取り、各スイッチに対応するLEDの色を制御します。画面には各スイッチの状態が表示され、LEDは1秒ごとに色が変化します。

## 🔧 必要なハードウェア

- M5Stack Tab5
- M5Stack Unit ByteSwitch

## 📚 必要なライブラリ

### 1. M5Unifiedライブラリ

Arduino IDEまたはArduino CLIでインストール：

```bash
arduino-cli lib install "M5Unified@0.2.10"
```

または、Arduino IDEのライブラリマネージャーから「M5Unified」を検索してインストール。

### 2. Unit ByteSwitchライブラリ（プロジェクト内ライブラリ）

このプロジェクトには`unit_byte.hpp`と`unit_byte.cpp`が含まれています。これらはプロジェクトフォルダ内に配置されているため、追加のインストールは不要です。

**重要**: `unit_byte.hpp`と`unit_byte.cpp`は`UnitByteSwitch.ino`と同じフォルダに配置されている必要があります。

ドライバライブラリ
Unit ByteSwitch は設計上 Unit ByteButton と非常に似ているため、M5Unit-ByteButton ライブラリを再利用して駆動します。


```
UnitByteSwitch/
├── UnitByteSwitch.ino
├── unit_byte.hpp      ← このファイルが必要
├── unit_byte.cpp      ← このファイルが必要
└── Readme.md
```

## 🚀 セットアップ

### 1. ファイルの配置確認

プロジェクトフォルダに以下のファイルがすべて含まれていることを確認してください：

- `UnitByteSwitch.ino`
- `unit_byte.hpp`
- `unit_byte.cpp`

### 2. ハードウェア接続

- Unit ByteSwitchをM5Stack Tab5のI2Cポートに接続
- I2Cアドレス: `0x46`（デフォルト）

### 3. ボード設定

- **Board**: ESP32P4 Dev Module
- **USB CDC on boot**: Enabled
- **Flash Size**: 16MB (128Mb)
- **Partition Scheme**: Custom
- **PSRAM**: Enabled

### 4. プログラムのアップロード

Arduino IDEまたはArduino CLIでプログラムをアップロード：

```bash
arduino-cli compile --fqbn esp32:esp32:esp32p4 UnitByteSwitch/UnitByteSwitch.ino
arduino-cli upload -p /dev/cu.usbmodem* --fqbn esp32:esp32:esp32p4 UnitByteSwitch/UnitByteSwitch.ino
```

## 💻 使用方法

1. **プログラムの起動**
   - プログラムをアップロードすると、自動的にUnit ByteSwitchが初期化されます
   - 初期化中は「Initializing...」と表示されます
   - 初期化に成功すると「Init Success!」と表示されます
   - 初期化に失敗した場合は、リトライ回数とエラーメッセージが表示されます

2. **スイッチの状態確認**
   - 画面下部に各スイッチ（CH[0]〜CH[7]）の状態が表示されます
   - `0`: スイッチOFF
   - `1`: スイッチON
   - リアルタイムで更新されます

3. **LEDの色変化**
   - 各スイッチに対応するLEDが1秒ごとに色を変化させます
   - 色の順序: 赤 → 緑 → 青 → 赤...
   - 全9つのLED（0〜8）が同期して色を変化します

## 📊 機能

- **8つのスイッチ読み取り**: 各スイッチのON/OFF状態を読み取り
- **8つのLED制御**: 各スイッチに対応するLEDの色を制御
- **RGB888色指定**: 24ビットRGB色（0xRRGGBB形式）でLEDの色を指定
- **明るさ調整**: LEDの明るさを0〜255の範囲で調整
- **リアルタイム表示**: スイッチの状態を画面にリアルタイム表示

## 🔍 プログラムの動作

### 1. 初期化（setup関数）

```cpp
// M5Stackの初期化
auto cfg = M5.config();
M5.begin(cfg);

// I2C通信の明示的な初期化（重要！）
Wire.begin();

// Unit ByteSwitchの初期化
dev.begin(&Wire, switchId, -1, -1, 400000);
// パラメータ:
// - &Wire: I2C通信オブジェクト
// - switchId: I2Cアドレス（0x46）
// - -1: SDAピン（デフォルトを使用）
// - -1: SCLピン（デフォルトを使用）
// - 400000: I2C速度（400kHz）

// LED表示モードをユーザー定義に設定
dev.setLEDShowMode(BYTE_LED_USER_DEFINED);

// 全LEDの明るさを250に設定
for (uint8_t i = 0; i <= 8; i++) {
    dev.setLEDBrightness(i, 250);
}
```

**重要**: `Wire.begin()`を明示的に呼び出すことが重要です。M5Unifiedの初期化だけでは不十分な場合があります。

### 2. メインループ（loop関数）

```cpp
// 各スイッチの状態を読み取り、画面下部に表示
M5.Display.setCursor(0, 300);  // 画面下部に表示
for (uint8_t i = 0; i < 8; i++) {
    uint8_t switchStatus8Bytes = dev.getSwitchStatus(i);
    M5.Display.printf("CH[%d]: %d\r\n", i, switchStatus8Bytes);
}

// 1秒ごとにLEDの色を変更
if (millis() - last_update_time > 1000) {
    color_index = (color_index + 1) % 3;
    for (int i = 0; i <= 8; i++) {
        dev.setRGB888(i, color_list[color_index]);
    }
    last_update_time = millis();
}
```

スイッチの状態は画面下部（Y座標300）に表示され、初期化メッセージと重ならないように配置されています。

## 📝 コードの主要なポイント

### M5Unifiedの初期化

```cpp
auto cfg = M5.config();
M5.begin(cfg);
```

Tab5では`M5.config()`を使用して設定を取得してから初期化します。

### I2C通信の初期化

```cpp
Wire.begin();  // 明示的なI2C初期化（重要！）
```

**重要**: `Wire.begin()`を明示的に呼び出すことが重要です。M5Unifiedの初期化だけでは不十分な場合があります。

### I2Cアドレスの設定

```cpp
uint8_t switchId = 0x46;  // Unit ByteSwitchのI2Cアドレス
```

### Unit ByteSwitchの初期化

```cpp
dev.begin(&Wire, switchId, -1, -1, 400000);
// -1, -1: デフォルトI2Cピンを使用（Tab5推奨）
// 400000: I2C速度（400kHz）
```

Tab5ではI2Cピンに`-1`を指定してデフォルトピンを使用することを推奨します。

### 色の定義

```cpp
uint32_t color_list[] = {0xff0000, 0x00ff00, 0x0000ff};
// 0xff0000: 赤
// 0x00ff00: 緑
// 0x0000ff: 青
```

色は24ビットRGB形式（0xRRGGBB）で指定します。

### スイッチ状態の読み取り

```cpp
uint8_t switchStatus8Bytes = dev.getSwitchStatus(i);
// i: スイッチ番号（0〜7）
// 戻り値: 0（OFF）または1（ON）
```

### LED色の設定

```cpp
dev.setRGB888(i, color);
// i: LED番号（0〜8）
// color: RGB888色（0xRRGGBB形式）
```

## 🛠️ Unit ByteSwitchライブラリの主要な関数

### 初期化

```cpp
bool begin(TwoWire* wire, uint8_t addr, uint8_t sda, uint8_t scl, uint32_t speed);
```

### スイッチ状態の読み取り

```cpp
uint8_t getSwitchStatus();              // 全スイッチの状態（1バイト）
uint8_t getSwitchStatus(uint8_t num);   // 指定スイッチの状態（0〜7）
```

### LED制御

```cpp
void setLEDBrightness(uint8_t num, uint8_t brightness);  // 明るさ設定（0〜255）
void setRGB888(uint8_t num, uint32_t color);              // RGB888色設定
void setRGB233(uint8_t num, uint32_t color);              // RGB233色設定
void setLEDShowMode(byte_led_t mode);                     // LED表示モード設定
```

### システム定義色（スイッチON/OFF時の色）

```cpp
void setSwitchOffRGB888(uint8_t num, uint32_t color);  // OFF時の色
void setSwitchOnRGB888(uint8_t num, uint32_t color);    // ON時の色
```

### その他の機能

```cpp
void setIRQEnable(bool en);              // 割り込み有効化
void setFlashWriteBack();                // フラッシュへの書き込み
uint8_t setI2CAddress(uint8_t newAddr); // I2Cアドレス変更
uint8_t getFirmwareVersion();            // ファームウェアバージョン取得
```

## ⚙️ カスタマイズ

### LEDの色を変更

```cpp
uint32_t color_list[] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00};
// 4色に変更する場合
color_index = (color_index + 1) % 4;  // ループ内も変更
```

### 色変化の間隔を変更

```cpp
if (millis() - last_update_time > 2000) {  // 2秒ごとに変更
    // ...
}
```

### LEDの明るさを変更

```cpp
dev.setLEDBrightness(i, 128);  // 明るさを128に設定（0〜255）
```

### スイッチON/OFF時の色を設定

```cpp
dev.setSwitchOffRGB888(0, 0x000000);  // スイッチ0のOFF時の色（黒）
dev.setSwitchOnRGB888(0, 0xff0000);  // スイッチ0のON時の色（赤）
```

## ⚠️ トラブルシューティング

### 問題1: コンパイルエラー「unit_byte.hppが見つからない」

**症状**: コンパイル時に`unit_byte.hpp`が見つからないエラーが発生する

**解決策**:
1. `unit_byte.hpp`と`unit_byte.cpp`が`UnitByteSwitch.ino`と同じフォルダにあることを確認
2. ファイル名のスペルミスがないか確認
3. Arduino IDEを使用している場合、スケッチフォルダを再確認

### 問題2: 「Unit ByteSwitch init Fail!」と表示される

**症状**: 初期化に失敗し、エラーメッセージが表示される

**原因と解決策**:

1. **I2C通信の初期化不足**
   - `Wire.begin()`が呼ばれていない可能性
   - **解決策**: `setup()`関数で`Wire.begin()`を明示的に呼び出す

2. **I2Cピン設定の問題**
   - 固定のピン番号（2, 1など）を指定している場合、Tab5では動作しない可能性がある
   - **解決策**: I2Cピンに`-1`を指定してデフォルトピンを使用

```cpp
// 修正前（動作しない可能性）
dev.begin(&Wire, switchId, 2, 1, 400000);

// 修正後（動作確認済み）
Wire.begin();  // 明示的なI2C初期化
dev.begin(&Wire, switchId, -1, -1, 400000);  // デフォルトピンを使用
```

3. **ハードウェア接続の問題**
   - Unit ByteSwitchが正しく接続されているか確認
   - I2Cアドレスが正しいか確認（デフォルト: 0x46）
   - I2Cケーブルの接続を確認

4. **I2Cアドレスの変更**
   - 別のI2Cアドレスに変更されている場合は、コード内の`switchId`を変更

```cpp
uint8_t switchId = 0x46;  // 実際のI2Cアドレスに変更
```

5. **I2C速度の問題**
   - 400kHzで動作しない場合は、100kHzに下げて試す

```cpp
dev.begin(&Wire, switchId, -1, -1, 100000);  // 100kHzに変更
```

### 問題3: スイッチの状態が読み取れない

**解決策**:
1. Unit ByteSwitchが正しく初期化されているか確認
2. I2C通信が正常に動作しているか確認
3. スイッチが物理的にON/OFFされているか確認

### 問題4: LEDが点灯しない

**解決策**:
1. LED表示モードが`BYTE_LED_USER_DEFINED`に設定されているか確認
2. LEDの明るさが0でないか確認
3. 色の値が正しいか確認（0xRRGGBB形式）

### 問題5: I2Cアドレスを変更したい

**方法**:
```cpp
// 初期化後にI2Cアドレスを変更
dev.setI2CAddress(0x48);  // 新しいアドレスに変更

// 次回起動時から新しいアドレスを使用
uint8_t switchId = 0x48;  // コード内も変更
```

## 📖 Unit ByteSwitchの仕様

### スイッチ数
- 8つのスイッチ（CH[0]〜CH[7]）

### LED数
- 9つのLED（0〜8）
- 各スイッチに対応するLED + 1つの追加LED

### I2C通信
- デフォルトアドレス: `0x46`
- I2C速度: 最大400kHz（デフォルト）

### LED色形式
- **RGB888**: 24ビット色（8ビット × 3チャンネル）
- **RGB233**: 8ビット色（2ビットR + 3ビットG + 3ビットB）

### LED表示モード
- **BYTE_LED_USER_DEFINED**: ユーザー定義モード（プログラムで制御）
- **BYTE_LED_MODE_DEFAULT**: システムデフォルトモード（スイッチON/OFF時の色）

## 💡 使用例

### 例1: スイッチの状態に応じてLEDの色を変更

```cpp
void loop() {
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t status = dev.getSwitchStatus(i);
        if (status == 1) {
            dev.setRGB888(i, 0x00ff00);  // ON時は緑
        } else {
            dev.setRGB888(i, 0xff0000);  // OFF時は赤
        }
    }
    delay(100);
}
```

### 例2: システム定義モードを使用

```cpp
void setup() {
    // ...
    dev.setLEDShowMode(BYTE_LED_MODE_DEFAULT);
    
    // スイッチ0のON/OFF時の色を設定
    dev.setSwitchOffRGB888(0, 0x000000);  // OFF時: 黒
    dev.setSwitchOnRGB888(0, 0xff0000);   // ON時: 赤
    
    // フラッシュに保存
    dev.setFlashWriteBack();
}
```

### 例3: 割り込みを使用

```cpp
void setup() {
    // ...
    dev.setIRQEnable(true);  // 割り込みを有効化
}

void loop() {
    // 割り込みが発生したらスイッチ状態を確認
    // （割り込み処理の実装が必要）
}
```

## 🔗 関連ファイル

- `UnitByteSwitch.ino`: メインプログラム
- `unit_byte.hpp`: Unit ByteSwitchライブラリのヘッダーファイル
- `unit_byte.cpp`: Unit ByteSwitchライブラリの実装ファイル

## 📖 参考資料

- [M5Stack Unit ByteSwitch 公式ドキュメント](https://docs.m5stack.com/)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [I2C通信について](https://en.wikipedia.org/wiki/I%C2%B2C)

## 📄 ライセンス

このプログラムは教育・学習目的で提供されています。  
Unit ByteSwitchライブラリ（unit_byte.hpp, unit_byte.cpp）はM5Stack Technology CO LTDの著作物です。

---

## ✅ 動作確認済みの設定

以下の設定で正常に動作することを確認しました：

- **I2C初期化**: `Wire.begin()`を明示的に呼び出し
- **I2Cピン**: `-1, -1`（デフォルトピンを使用）
- **I2C速度**: `400000`（400kHz）
- **I2Cアドレス**: `0x46`
- **リトライ機能**: 10回までリトライ後、警告を表示して続行

## 📝 修正履歴

### 2025年11月 - Tab5対応版

- `Wire.begin()`の明示的な呼び出しを追加
- I2Cピンを`-1, -1`に変更（デフォルトピン使用）
- リトライ機能を追加（最大10回）
- エラーメッセージの改善
- 表示位置の最適化

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5 + Unit ByteSwitch  
**動作確認**: ✅ 正常動作確認済み（Tab5対応版）

