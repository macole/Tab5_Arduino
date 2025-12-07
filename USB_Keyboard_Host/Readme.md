# USB_Keyboard_Host - USBキーボード入力を受け取るサンプル (EspUsbHost ライブラリ)

このサンプルは、M5Stack Tab5 の **USB Type-A ポート**に USBキーボードを接続し、  
キー入力を Tab5 側で処理するデモです。

[EspUsbHost](https://github.com/tanakamasayuki/EspUsbHost) ライブラリを使用して実装しています。

## 📋 概要

- Tab5 の USB Type-A ポートに USBキーボードを接続
- EspUsbHost ライブラリでキー入力を処理
- 入力された文字を画面とシリアルモニタに表示
- 修飾キー（Shift, Ctrl, Alt など）の状態を表示
- 特殊キー（Enter, Backspace など）の処理

## 🔧 必要な環境 / ハードウェア

- M5Stack Tab5 (ESP32-P4)
- USB Type-A ポート（Tab5 側）
- USB キーボード（USB Type-A コネクタ）
- Arduino IDE 2.x または Arduino CLI
- ESP32-P4 ボードパッケージ

## 📚 必要なライブラリ

- **M5Unified**
- **EspUsbHost**（GitHub からインストール）

### EspUsbHost のインストール方法

1. [EspUsbHost GitHub リポジトリ](https://github.com/tanakamasayuki/EspUsbHost) から ZIP ファイルをダウンロード
2. Arduino IDE で「スケッチ」 > 「ライブラリをインクルード」 > 「.ZIP形式のライブラリをインストール」を選択
3. ダウンロードした ZIP ファイルを選択してインストール

または、Arduino CLI の場合：

```bash
# GitHub から直接インストール（Arduino CLI が対応している場合）
arduino-cli lib install --git-url https://github.com/tanakamasayuki/EspUsbHost
```

```bash
arduino-cli lib install "M5Unified@0.2.10"
```

## 🚀 セットアップ

1. ボード設定（例）
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. スケッチ
   - ファイル: `USB_Keyboard_Host/USB_Keyboard_Host.ino`

3. アップロード
   - Tab5 を USB-C で PC に接続し、通常通り書き込みます。

4. 実行
   - 書き込み後、Tab5 の **USB Type-A ポート**に USBキーボードを接続します。
   - キーボードで文字を入力すると、Tab5 の画面とシリアルモニタに表示されます。

## 💻 動作

- **キーボード接続時**: 画面上部に "Keyboard: Connected" と表示（緑色）
- **キーボード未接続時**: 画面上部に "Keyboard: Waiting..." と表示（黄色）
- **文字入力**: 入力された文字が画面中央に表示され、シリアルモニタにも出力
- **修飾キー**: 画面下部に現在押されている修飾キー（Shift, Ctrl, Alt など）を表示
- **キーコード**: 画面下部に現在押されているキーコードを16進数で表示
- **特殊キー**: Enter キーで改行、Backspace キーで文字削除

## 🔍 EspUsbHost ライブラリの使用方法（キーボード）

### 基本的な使い方

1. **クラスの継承**
   ```cpp
   #include "EspUsbHost.h"
   
   class MyEspUsbHost : public EspUsbHost {
     // 仮想関数をオーバーライド
   };
   
   MyEspUsbHost usbHost;
   ```

2. **初期化**
   ```cpp
   void setup() {
     usbHost.begin();  // USB Host スタックを初期化
     // 日本語キーボードの場合は以下を追加
     // usbHost.setHIDLocal(HID_LOCAL_Japan_Katakana);
   }
   ```

3. **ループ処理**
   ```cpp
   void loop() {
     usbHost.task();  // USB Host イベントを処理（必須）
   }
   ```

### キーボード用の仮想関数

EspUsbHost クラスを継承して、以下の仮想関数をオーバーライドすることでキーボード入力を処理できます。

#### `onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier)`
キーが押されたときに呼び出されます。ASCII文字コード、HIDキーコード、修飾キーの状態が渡されます。

```cpp
void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) override {
  // ascii: ASCII文字コード（印字可能文字、Enter='\r', Backspace='\b' など）
  // keycode: HID キーコード（0x04='a', 0x05='b' など）
  // modifier: 修飾キーの状態（Shift, Ctrl, Alt など）
  
  if (' ' <= ascii && ascii <= '~') {
    // 印字可能文字
    Serial.printf("%c", ascii);
  } else if (ascii == '\r') {
    // Enter キー
    Serial.println();
  }
}
```

#### `onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report)`
キーボードレポート全体を処理します。複数のキーが同時に押されている場合や、修飾キーの状態を詳細に取得したい場合に使用します。

```cpp
void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report) override {
  // report.modifier: 修飾キーの状態
  // report.keycode[0-5]: 最大6つまでの同時押しキーコード
  // last_report: 前回のレポート（変化検出用）
  
  // 修飾キーのチェック
  if (report.modifier & KEYBOARD_MODIFIER_LEFTSHIFT) {
    // 左Shiftが押されている
  }
  
  // 押されているキーコードを確認
  for (int i = 0; i < 6; i++) {
    if (report.keycode[i] != 0) {
      Serial.printf("Key pressed: 0x%02X\n", report.keycode[i]);
    }
  }
}
```

#### `getKeycodeToAscii(uint8_t keycode, uint8_t shift)`
HIDキーコードをASCII文字に変換します。カスタムキーマップを実装する場合にオーバーライドできます。

```cpp
uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift) override {
  // デフォルトの変換テーブルを使用する場合は親クラスの実装を呼び出す
  return EspUsbHost::getKeycodeToAscii(keycode, shift);
  
  // または、カスタムキーマップを実装
  // ...
}
```

### 修飾キーの定数

EspUsbHost ライブラリでは、以下の定数を使用して修飾キーの状態を判定します。

```cpp
KEYBOARD_MODIFIER_LEFTCTRL    // 左Ctrl (0x01)
KEYBOARD_MODIFIER_RIGHTSHIFT  // 右Shift (0x02)
KEYBOARD_MODIFIER_LEFTALT     // 左Alt (0x04)
KEYBOARD_MODIFIER_LEFTGUI     // 左GUI/Windows (0x08)
KEYBOARD_MODIFIER_RIGHTCTRL   // 右Ctrl (0x10)
KEYBOARD_MODIFIER_RIGHTSHIFT  // 右Shift (0x20)
KEYBOARD_MODIFIER_RIGHTALT    // 右Alt (0x40)
KEYBOARD_MODIFIER_RIGHTGUI    // 右GUI/Windows (0x80)
```

### 修飾キーの判定例

```cpp
// 修飾キーが押されているかチェック
if (report.modifier & KEYBOARD_MODIFIER_LEFTSHIFT) {
  // 左Shiftが押されている
}

if (report.modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT)) {
  // Shift（左右どちらか）が押されている
}

if (report.modifier & KEYBOARD_MODIFIER_LEFTCTRL && 
    report.modifier & KEYBOARD_MODIFIER_LEFTALT) {
  // Ctrl+Alt が同時に押されている
}
```

### HID ロケールの設定

日本語キーボードを使用する場合は、`setHIDLocal()` でロケールを設定します。

```cpp
void setup() {
  usbHost.begin();
  usbHost.setHIDLocal(HID_LOCAL_Japan_Katakana);  // 日本語キーボード
}
```

利用可能なロケール：
- `HID_LOCAL_Japan_Katakana`: 日本語（カタカナ）
- デフォルト（US キーボード）

## 🔍 このサンプルのコードのポイント

- `EspUsbHost` を継承した `MyEspUsbHost` クラスを作成
- `onKeyboardKey()` メソッドをオーバーライドしてキー入力を処理
- `onKeyboard()` メソッドで修飾キーとキーコードを表示
- `onGone()` メソッドでデバイス切断を検出
- `usbHost.begin()` で初期化
- `usbHost.task()` を `loop()` で呼び出して USB Host イベントを処理（**必須**）
- 入力された文字を画面に表示（自動スクロール対応）

## ⚠️ 注意事項

- USB Host モードは **USB Type-A ポート**を使用します（USB-C ポートではない）。
- 一部の USB キーボードは認識されない場合があります（互換性の問題）。
- EspUsbHost ライブラリは ESP32-S3 や ESP32-P4 などの USB Host 対応チップで動作します。
- **`usbHost.task()` は `loop()` 内で必ず呼び出す必要があります**。これを呼び出さないと USB Host イベントが処理されません。
- 日本語キーボードを使用する場合は、`setHIDLocal(HID_LOCAL_Japan_Katakana)` を設定してください。

## 🐛 トラブルシューティング

### キーボードが認識されない

- USB Type-A ポートに正しく接続されているか確認
- 別の USB キーボードで試す（互換性の問題の可能性）
- シリアルモニタでエラーメッセージを確認

### 文字が正しく表示されない

- 日本語キーボードの場合は `setHIDLocal(HID_LOCAL_Japan_Katakana)` を設定
- シリアルモニタでキーコードを確認（`onKeyboard()` で表示される）

### キー入力が反応しない

- `usbHost.task()` が `loop()` 内で呼び出されているか確認
- シリアルモニタでキーボード接続メッセージが表示されるか確認

## 📝 参考資料

- [EspUsbHost GitHub リポジトリ](https://github.com/tanakamasayuki/EspUsbHost)
- EspUsbHost ライブラリの README と examples フォルダを参照
- 関連サンプル: `USB_Mouse_Host` - USB マウス入力を受け取るサンプル

## 📝 ライセンス

このサンプルは学習・検証用途で自由に利用できます。  
EspUsbHost ライブラリのライセンスに従います。

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5 (ESP32-P4)  
**開発環境**: Arduino IDE / Arduino CLI  
**使用ライブラリ**: EspUsbHost (https://github.com/tanakamasayuki/EspUsbHost)

