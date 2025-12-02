# USB_Mouse - USBマウスデモとライブラリ仕様メモ

このサンプルは、USBマウスの入力をM5Stack Tab5側で扱うイメージのコードですが、現状の `USBHID` ライブラリ仕様とは一致していないため、そのままではコンパイルエラーになります。  
ここでは、発生したエラーの内容と原因、そして今後どのような方向で実装を考えるべきかをドキュメントとして整理します。

---

## ❗ 発生しているコンパイルエラー

`USB_Mouse/USB_Mouse.ino` の `setup()` 内で次の行を書いた場合：

```cpp
HID.onEvent(mouse_callback);
```

コンパイル時に以下のエラーが発生します。

```text
error: invalid conversion from 'void (*)(void*, uint8_t*, uint16_t)'
       {aka 'void (*)(void*, unsigned char*, short unsigned int)'}
       to 'esp_event_handler_t'
       {aka 'void (*)(void*, const char*, long int, void*)'} [-fpermissive]
```

## 🔍 エラーの原因

`USBHID::onEvent()` が要求しているコールバックの型と、`mouse_callback` の型が一致していないためです。

- `USBHID::onEvent()` が要求する型（`esp_event_handler_t`）は、`USBHID.h` で次のように宣言されています。

```cpp
void onEvent(esp_event_handler_t callback);
```

`esp_event_handler_t` は実際には以下のようなシグネチャを持ちます。

```cpp
void (*esp_event_handler_t)(void* arg,
                            esp_event_base_t event_base,
                            int32_t event_id,
                            void* event_data);
```

- 一方、このスケッチで定義している `mouse_callback` は次の形になっています。

```cpp
void mouse_callback(void* arg, uint8_t* report, uint16_t len);
```

つまり、

- `onEvent()` は **「ESP-IDF のイベントループ用コールバック（event_base, event_id など）」** を期待しているのに対し、
- このスケッチ側は **「HIDマウスレポート (uint8_t* report) を直接受け取るコールバック」** を想定している

というミスマッチが発生しており、その結果としてコンパイラが「型が違う」とエラーを出しています。

## 📚 USBHID ライブラリの前提

`USBHID` ライブラリ（`USBHID.h`）は、主に **USB HID「デバイス」モード** 用のクラスです。

- ESP32（Tab5）が **ホストPCから見て「キーボード／マウス／ゲームパッドなどのHIDデバイスとして振る舞う」** 場面で使う想定
- `onEvent()` で受け取るのは、
  - HIDプロトコルの変更
  - Idleの変更
  - などの **制御イベント**
 であり、「マウスレポート（座標やボタン状態）」そのものではありません。

一方、この `USB_Mouse.ino` がやろうとしているのは、

- 「USBマウスをTab5に接続し、そのレポート（ボタン・移動量）を受け取って画面上のカーソルを動かす」

という **USB Hostモードの「マウス入力」処理** であり、これは `USBHID` クラスだけでは実現できません。

## ✅ ひとまずの対処（コンパイルを通す）

現時点の Arduino 向け `USBHID` API には、「`uint8_t* report` を受け取るマウスレポートコールバック」は用意されていません。  
そのため、**今のスケッチの形のままでは、正しいコールバックを登録する方法が存在しません。**

まずコンパイルを通したい場合は、`USB_Mouse.ino` の `setup()` から

```cpp
HID.onEvent(mouse_callback);
```

の行を削除（またはコメントアウト）します。

```cpp
void setup() {
    M5.begin();
    M5.Display.setRotation(3);
    M5.Display.clear();
    
    Serial.begin(115200);
    
    // USB Host初期化
    HID.begin();
    // HID.onEvent(mouse_callback);  // ← ここをコメントアウトまたは削除

    ...
}
```

これにより、USBマウスのレポートはまだ扱えませんが、**スケッチ自体はビルド可能な状態**になります。

## 🎯 今後の実装方針メモ

USBマウスの入力を本格的に扱いたい場合は、次のいずれかの方向で検討が必要です。

- **A. Tab5 を USB HID デバイスとして使う**（PC側から見てマウス／キーボードとして振る舞う）
  - `USBHID` / `USBHIDMouse` などを使い、「レポートを送る側」のコードに書き換える。
  - 例: `move(x, y)`, `click()` などを呼んで PC 上のカーソルを動かす。
- **B. Tab5 を USB Host としてマウス入力を受け取る**
  - ESP-IDF の USB Host stack（`usb_host.h`, `hid_host.h` など）や TinyUSB Host API を直接使う。
  - Arduinoレベルではラップされていないため、**かなり低レベルな実装**が必要になる。

このリポジトリの現行サンプルでは、

- 「USBマウスからのレポートを完全に扱う USB Host 実装」までは行わず、
- 上記のように **エラー原因とライブラリ仕様のメモを残した状態** にとどめています。

将来、本格的な USB Host マウスサンプルを追加する際には、このドキュメントを設計メモとして参照してください。


