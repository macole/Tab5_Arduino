# USB_Mouse_Host - USBマウス入力を受け取るサンプル (EspUsbHost ライブラリ)

このサンプルは、M5Stack Tab5 の **USB Type-A ポート**に USBマウスを接続し、  
マウスの移動量やボタンクリックを Tab5 側で処理するデモです。

[EspUsbHost](https://github.com/tanakamasayuki/EspUsbHost) ライブラリを使用して実装しています。

✅ **動作確認済み**: Tab5 (ESP32-P4) で正常に動作することを確認しました。

## 📋 概要

- Tab5 の USB Type-A ポートに USBマウスを接続
- EspUsbHost ライブラリでマウス入力を処理
- マウスの移動量を画面にカーソルとして表示
- ボタンクリックをシリアルモニタに出力

## 🔧 必要な環境 / ハードウェア

- M5Stack Tab5 (ESP32-P4)
- USB Type-A ポート（Tab5 側）
- USB マウス（USB Type-A コネクタ）
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
   - ファイル: `USB_Mouse_Host/USB_Mouse_Host.ino`

3. アップロード
   - Tab5 を USB-C で PC に接続し、通常通り書き込みます。

4. 実行
   - 書き込み後、Tab5 の **USB Type-A ポート**に USBマウスを接続します。
   - マウスを動かすと、Tab5 の画面に赤いカーソルが表示されます。

## 💻 動作

- **マウス接続時**: 画面下部に "Mouse: Connected" と表示（緑色）
- **マウス未接続時**: 画面下部に "Mouse: Waiting..." と表示（黄色）
- **マウス移動**: 画面に赤い円（カーソル）が表示され、マウスの移動に追従
- **ボタンクリック**: シリアルモニタに "Left button pressed" / "Left button released" などが出力

## 🔍 EspUsbHost ライブラリの使用方法

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
   }
   ```

3. **ループ処理**
   ```cpp
   void loop() {
     usbHost.task();  // USB Host イベントを処理（必須）
   }
   ```

### マウス用の仮想関数

EspUsbHost クラスを継承して、以下の仮想関数をオーバーライドすることでマウス入力を処理できます。

#### `onMouse(hid_mouse_report_t report, uint8_t last_buttons)`
マウスレポート全体を処理します。移動量・ボタン状態・ホイールなどすべての情報が含まれます。

```cpp
void onMouse(hid_mouse_report_t report, uint8_t last_buttons) override {
  // report.x, report.y: 相対移動量（int8_t）
  // report.buttons: ボタン状態（uint8_t）
  // report.wheel: ホイール回転量（int8_t）
  // last_buttons: 前回のボタン状態
}
```

#### `onMouseButtons(hid_mouse_report_t report, uint8_t last_buttons)`
ボタンの状態が変化したときのみ呼び出されます。

```cpp
void onMouseButtons(hid_mouse_report_t report, uint8_t last_buttons) override {
  // ボタンの押下・解放を検出
  if (!(last_buttons & MOUSE_BUTTON_LEFT) && 
       (report.buttons & MOUSE_BUTTON_LEFT)) {
    // 左ボタンが押された
  }
}
```

#### `onMouseMove(hid_mouse_report_t report)`
マウスが移動したときのみ呼び出されます（ボタンが押されていない場合も含む）。

```cpp
void onMouseMove(hid_mouse_report_t report) override {
  // report.x, report.y で移動量を取得
}
```

#### `onGone(const usb_host_client_event_msg_t *eventMsg)`
USB デバイスが切断されたときに呼び出されます。

```cpp
void onGone(const usb_host_client_event_msg_t *eventMsg) override {
  // デバイス切断時の処理
}
```

### マウスボタンの定数

EspUsbHost ライブラリでは、以下の定数を使用してボタン状態を判定します。

```cpp
MOUSE_BUTTON_LEFT     // 左ボタン (0x01)
MOUSE_BUTTON_RIGHT    // 右ボタン (0x02)
MOUSE_BUTTON_MIDDLE   // 中ボタン（ホイールクリック）(0x04)
MOUSE_BUTTON_BACKWARD // 戻るボタン (0x08)
MOUSE_BUTTON_FORWARD  // 進むボタン (0x10)
```

### ボタン状態の判定例

```cpp
// ボタンが押されているかチェック
if (report.buttons & MOUSE_BUTTON_LEFT) {
  // 左ボタンが押されている
}

// ボタンの状態変化を検出
uint8_t button_changes = report.buttons ^ last_buttons;
if (button_changes & MOUSE_BUTTON_LEFT) {
  if (report.buttons & MOUSE_BUTTON_LEFT) {
    // 左ボタンが押された
  } else {
    // 左ボタンが離された
  }
}
```

### キーボード用の仮想関数（参考）

EspUsbHost はキーボード入力もサポートしています。

```cpp
void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) override {
  // ascii: ASCII文字コード
  // keycode: HID キーコード
  // modifier: 修飾キー（Shift, Ctrl など）
}

void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report) override {
  // キーボードレポート全体を処理
}

uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift) override {
  // キーコードをASCIIに変換（カスタムキーマップ用）
}
```

### その他のメソッド

- `setHIDLocal(uint8_t local)`: HID ロケールを設定（例: `HID_LOCAL_Japan_Katakana`）
- `onData(const usb_transfer_t *transfer)`: 生の USB 転送データを処理

## 🔍 このサンプルのコードのポイント

- `EspUsbHost` を継承した `MyEspUsbHost` クラスを作成
- `onMouse()` メソッドをオーバーライドしてマウスレポートを処理
- `onGone()` メソッドでデバイス切断を検出
- `usbHost.begin()` で初期化
- `usbHost.task()` を `loop()` で呼び出して USB Host イベントを処理（**必須**）
- `MOUSE_BUTTON_LEFT`, `MOUSE_BUTTON_RIGHT`, `MOUSE_BUTTON_MIDDLE` 定数を使用

## ⚠️ 注意事項

- USB Host モードは **USB Type-A ポート**を使用します（USB-C ポートではない）。
- 一部の USB マウスは認識されない場合があります（互換性の問題）。
- EspUsbHost ライブラリは ESP32-S3 や ESP32-P4 などの USB Host 対応チップで動作します。
- **`usbHost.task()` は `loop()` 内で必ず呼び出す必要があります**。これを呼び出さないと USB Host イベントが処理されません。
- マウスボタンの定数は `MOUSE_LEFT` ではなく `MOUSE_BUTTON_LEFT` です（`MOUSE_BUTTON_RIGHT`, `MOUSE_BUTTON_MIDDLE` も同様）。

## 🐛 トラブルシューティング

### マウスが認識されない

- USB Type-A ポートに正しく接続されているか確認
- 別の USB マウスで試す（互換性の問題の可能性）
- シリアルモニタでエラーメッセージを確認

### コンパイルエラー: `MOUSE_LEFT` was not declared

- `MOUSE_LEFT` ではなく `MOUSE_BUTTON_LEFT` を使用してください
- 同様に `MOUSE_RIGHT` → `MOUSE_BUTTON_RIGHT`, `MOUSE_MIDDLE` → `MOUSE_BUTTON_MIDDLE`

### マウスが動かない

- `usbHost.task()` が `loop()` 内で呼び出されているか確認
- シリアルモニタでマウス接続メッセージが表示されるか確認

## 📝 参考資料

- [EspUsbHost GitHub リポジトリ](https://github.com/tanakamasayuki/EspUsbHost)
- EspUsbHost ライブラリの README と examples フォルダを参照

## 📝 ライセンス

このサンプルは学習・検証用途で自由に利用できます。  
EspUsbHost ライブラリのライセンスに従います。

---

**作成日**: 2025年12月6日  
**最終更新**: 2025年12月6日（動作確認済み）  
**対象デバイス**: M5Stack Tab5 (ESP32-P4)  
**開発環境**: Arduino IDE / Arduino CLI  
**使用ライブラリ**: EspUsbHost (https://github.com/tanakamasayuki/EspUsbHost)  
**動作確認**: ✅ Tab5 で正常動作確認済み

