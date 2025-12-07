# UnitQRCode_i2c - QRコードリーダー

M5Stack Tab5とUnit QRCodeを使用してQRコードを読み取るプログラムです。I2C通信でUnit QRCodeと接続し、自動または手動でQRコードをスキャンできます。

## 📋 概要

このプログラムは、M5Stack Unit QRCodeを使用してQRコードを読み取り、その内容を画面とシリアルモニターに表示します。自動スキャンモードと手動スキャンモードの両方に対応しており、読み取り状態もリアルタイムで表示されます。

## 🔧 必要なハードウェア

- M5Stack Tab5
- M5Stack Unit QRCode

## 📚 必要なライブラリ

### 1. M5Unifiedライブラリ

Arduino IDEまたはArduino CLIでインストール：

```bash
arduino-cli lib install "M5Unified@0.2.10"
```

### 2. M5GFXライブラリ

```bash
arduino-cli lib install "M5GFX@0.2.15"
```

### 3. M5UnitQRCodeライブラリ

```bash
arduino-cli lib install "M5UnitQRCode"
```

または、GitHubからインストール：

```bash
# GitHubリポジトリからクローン
git clone https://github.com/m5stack/M5Unit-QRCode.git
# Arduino IDEのライブラリフォルダに配置
```

## 🚀 セットアップ

### 1. ハードウェア接続

- Unit QRCodeをM5Stack Tab5のI2Cポートに接続
- I2Cアドレス: `0x29`（デフォルト）

### 2. ボード設定

- **Board**: ESP32P4 Dev Module
- **USB CDC on boot**: Enabled
- **Flash Size**: 16MB (128Mb)
- **Partition Scheme**: Custom
- **PSRAM**: Enabled

### 3. プログラムのアップロード

Arduino IDEまたはArduino CLIでプログラムをアップロード：

```bash
arduino-cli compile --fqbn esp32:esp32:esp32p4 UnitQRCode_i2c/UnitQRCode_i2c.ino
arduino-cli upload -p /dev/cu.usbmodem* --fqbn esp32:esp32:esp32p4 UnitQRCode_i2c/UnitQRCode_i2c.ino
```

## 💻 使用方法

### 自動スキャンモード（デフォルト）

1. **プログラムの起動**
   - プログラムをアップロードすると、自動的にUnit QRCodeが初期化されます
   - 初期化に成功すると「Init Success!」と表示されます
   - 「Mode: Auto Scan」と表示され、自動スキャンが開始されます

2. **QRコードの読み取り**
   - Unit QRCodeのカメラにQRコードをかざす
   - QRコードが検出されると、画面とシリアルモニターにデータが表示されます

### 手動スキャンモード

コード内で`#define I2C_AUTO_SCAN_MODE`をコメントアウトすると、手動スキャンモードになります：

```cpp
// #define I2C_AUTO_SCAN_MODE  // この行をコメントアウト
```

1. **スキャンの開始**
   - 画面をタッチしてスキャンを開始
   - 「Scan started...」と表示されます

2. **スキャンの停止**
   - 画面を長押ししてスキャンを停止
   - 「Scan stopped.」と表示されます

## 📊 機能

- **QRコード読み取り**: Unit QRCodeでQRコードを読み取り
- **自動/手動スキャン**: 2つのスキャンモードに対応
- **リアルタイム状態表示**: スキャン状態を画面下部に表示
- **データ表示**: 読み取ったデータを画面とシリアルモニターに表示
- **長いデータの折り返し**: 画面幅に応じて自動改行
- **リトライ機能**: 初期化失敗時に自動リトライ（最大10回）

## 🔍 プログラムの動作

### 1. 初期化（setup関数）

```cpp
// M5Stackの初期化
auto cfg = M5.config();
M5.begin(cfg);

// I2C通信の明示的な初期化（重要！）
Wire.begin();

// キャンバスの作成（モノクロ、スクロール対応）
canvas.setColorDepth(1);
canvas.createSprite(M5.Display.width(), M5.Display.height());
canvas.setFont(&fonts::lgfxJapanMinchoP_16);
canvas.setTextSize(1.5);

// Unit QRCodeの初期化
qrcode.begin(&Wire, UNIT_QRCODE_ADDR, -1, -1, 100000U);
// -1, -1: デフォルトI2Cピンを使用
// 100000U: I2C速度（100kHz）
```

### 2. メインループ（loop関数）

```cpp
// スキャン状態の表示（500msごとに更新）
if (millis() - last_status_update > 500) {
    // 画面下部に「Scanning...」または「Decoded!」を表示
}

// QRコードが検出された場合
if (qrcode.getDecodeReadyStatus() == 1) {
    // データを読み取り
    // 画面とシリアルモニターに表示
    // 長いデータは自動改行
}
```

## 📝 コードの主要なポイント

### スキャンモードの選択

```cpp
#define I2C_AUTO_SCAN_MODE  // 自動スキャンモード（デフォルト）

// または

// #define I2C_AUTO_SCAN_MODE  // コメントアウトで手動スキャンモード
```

### I2C通信の初期化

```cpp
Wire.begin();  // 明示的なI2C初期化（重要！）
```

**重要**: `Wire.begin()`を明示的に呼び出すことが重要です。

### Unit QRCodeの初期化

```cpp
qrcode.begin(&Wire, UNIT_QRCODE_ADDR, -1, -1, 100000U);
// UNIT_QRCODE_ADDR: 0x29（デフォルトI2Cアドレス）
// -1, -1: デフォルトI2Cピンを使用（Tab5推奨）
// 100000U: I2C速度（100kHz）
```

### フォントと表示設定

```cpp
canvas.setFont(&fonts::lgfxJapanMinchoP_16);  // 16ptフォント
canvas.setTextSize(1.5);  // テキストサイズ1.5倍
canvas.setTextScroll(true);  // スクロール有効
```

### 長いデータの折り返し

```cpp
int chars_per_line = M5.Display.width() / 12;  // 1行の文字数
// 一定文字数ごとに改行
```

## ⚙️ カスタマイズ

### スキャンモードの変更

```cpp
// 自動スキャンモード
#define I2C_AUTO_SCAN_MODE
qrcode.setTriggerMode(AUTO_SCAN_MODE);

// 手動スキャンモード
// #define I2C_AUTO_SCAN_MODE  // コメントアウト
qrcode.setTriggerMode(MANUAL_SCAN_MODE);
```

### フォントサイズの変更

```cpp
canvas.setFont(&fonts::lgfxJapanMinchoP_16);  // 16pt
canvas.setTextSize(1.5);  // サイズを変更（1.0〜3.0程度）
```

### I2C速度の変更

```cpp
qrcode.begin(&Wire, UNIT_QRCODE_ADDR, -1, -1, 400000U);  // 400kHzに変更
```

### ステータス更新間隔の変更

```cpp
if (millis() - last_status_update > 1000) {  // 1秒ごとに更新
    // ...
}
```

## ⚠️ トラブルシューティング

### 問題1: 「Unit QRCode I2C Init Fail!」と表示される

**症状**: 初期化に失敗し、エラーメッセージが表示される

**原因と解決策**:

1. **I2C通信の初期化不足**
   - `Wire.begin()`が呼ばれていない可能性
   - **解決策**: `setup()`関数で`Wire.begin()`を明示的に呼び出す

2. **I2Cピン設定の問題**
   - 固定のピン番号（21, 22など）を指定している場合、Tab5では動作しない可能性がある
   - **解決策**: I2Cピンに`-1`を指定してデフォルトピンを使用

```cpp
// 修正前（動作しない可能性）
qrcode.begin(&Wire, UNIT_QRCODE_ADDR, 21, 22, 100000U);

// 修正後（動作確認済み）
Wire.begin();  // 明示的なI2C初期化
qrcode.begin(&Wire, UNIT_QRCODE_ADDR, -1, -1, 100000U);  // デフォルトピンを使用
```

3. **ハードウェア接続の問題**
   - Unit QRCodeが正しく接続されているか確認
   - I2Cアドレスが正しいか確認（デフォルト: 0x29）
   - I2Cケーブルの接続を確認

4. **I2C速度の問題**
   - 100kHzで動作しない場合は、50kHzに下げて試す

```cpp
qrcode.begin(&Wire, UNIT_QRCODE_ADDR, -1, -1, 50000U);  // 50kHzに変更
```

### 問題2: QRコードが読み取れない

**解決策**:
- QRコードとUnit QRCodeの距離を調整（数cm〜数十cm）
- 照明条件を確認（明るすぎる、暗すぎる環境を避ける）
- QRコードのサイズを確認（小さすぎると読み取れない場合がある）
- QRコードが汚れていないか確認

### 問題3: 文字が重なって表示される

**解決策**:
- フォントサイズを調整

```cpp
canvas.setTextSize(1.5);  // 1.0〜2.0の範囲で調整
```

- フォントを変更

```cpp
canvas.setFont(&fonts::lgfxJapanMinchoP_12);  // より小さいフォント
```

### 問題4: 長いデータが画面からはみ出す

**解決策**:
- 折り返し処理が実装されているため、自動的に改行されます
- それでもはみ出す場合は、`chars_per_line`の値を調整

```cpp
int chars_per_line = M5.Display.width() / 10;  // より少ない文字数で改行
```

## 📖 Unit QRCodeの仕様

### I2C通信
- **デフォルトアドレス**: `0x29`
- **I2C速度**: 最大100kHz（推奨）

### スキャンモード
- **AUTO_SCAN_MODE**: 自動的にQRコードをスキャン
- **MANUAL_SCAN_MODE**: 手動でスキャンを開始/停止

### データサイズ
- 最大512バイトまで読み取り可能

## 💡 使用例

### 例1: 読み取ったデータを処理する

```cpp
if (qrcode.getDecodeReadyStatus() == 1) {
    uint8_t buffer[512] = {0};
    uint16_t length = qrcode.getDecodeLength();
    qrcode.getDecodeData(buffer, length);
    
    // データを文字列として処理
    String data = String((char*)buffer);
    
    // URLの場合
    if (data.startsWith("http://") || data.startsWith("https://")) {
        Serial.println("URL detected: " + data);
    }
    
    // テキストの場合
    else {
        Serial.println("Text: " + data);
    }
}
```

### 例2: 読み取り回数をカウント

```cpp
static int scan_count = 0;

if (qrcode.getDecodeReadyStatus() == 1) {
    scan_count++;
    canvas.setCursor(0, 0);
    canvas.printf("Scan count: %d\n", scan_count);
    // ...
}
```

## 🔗 関連ファイル

- `UnitQRCode_i2c.ino`: メインプログラム

## 📖 参考資料

- [M5Stack Unit QRCode 公式ドキュメント](https://docs.m5stack.com/)
- [M5UnitQRCode ライブラリ](https://github.com/m5stack/M5Unit-QRCode)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [M5GFX ライブラリ](https://github.com/m5stack/M5GFX)

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

**注意**: M5UnitQRCodeライブラリはM5Stack Technology CO LTDの著作物です。各ライブラリのライセンスを確認してください。

---

## ✅ 動作確認済みの設定

以下の設定で正常に動作することを確認しました：

- **I2C初期化**: `Wire.begin()`を明示的に呼び出し
- **I2Cピン**: `-1, -1`（デフォルトピンを使用）
- **I2C速度**: `100000U`（100kHz）
- **I2Cアドレス**: `0x29`
- **フォント**: `lgfxJapanMinchoP_16`
- **テキストサイズ**: `1.5`
- **リトライ機能**: 10回までリトライ後、警告を表示して続行

## 📝 修正履歴

### 2025年12月6日 - Tab5対応版

- `Wire.begin()`の明示的な呼び出しを追加
- I2Cピンを`-1, -1`に変更（デフォルトピン使用）
- リトライ機能を追加（最大10回）
- エラーメッセージの改善
- フォントサイズの最適化（文字の重なりを解消）
- ディスプレイへの状態表示機能を追加
- 長いデータの自動折り返し機能を追加
- タッチ操作による手動スキャン対応（Tab5用）

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5 + Unit QRCode  
**動作確認**: ✅ 正常動作確認済み（Tab5対応版）

