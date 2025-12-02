# Wi-Fi Antenna スイッチャー解説

## プログラム概要

このスケッチは M5Tab5 の IO エキスパンダを制御し、Wi-Fi アンテナの接続を内部（本体アンテナ）と外部（外付けアンテナ）で切り替えます。同時にディスプレイへ現在のアンテナモードを表示します。ハードウェア的な切替ラインを直接叩くだけなので、ループ処理や追加のロジックは不要です。

## コード全体

```1:26:Wifi_Antenna/Wifi_Antenna.ino
#include "M5Unified.h"

void setup()
{
    auto cfg = M5.config();
    cfg.output_power = true;  // ensure display power when using battery units
    M5.begin(cfg);

    M5.Display.setTextSize(2);
    M5.Display.setCursor(0, 0);
    M5.Display.println("Wi-Fi Antenna");

    // Use internal Wi-Fi antenna
    auto& ioe = M5.getIOExpander(0);
    ioe.digitalWrite(0, false);
    M5.Display.println("Mode: Internal");

    // Use external Wi-Fi antenna
    // auto& ioe = M5.getIOExpander(0);
    // ioe.digitalWrite(0, true);
    // M5.Display.println("Mode: External");
}

void loop()
{
}
```

## 処理の流れ

### 1. 初期化 (`setup`)

1. **設定取得と電源出力の有効化**
   ```cpp
   auto cfg = M5.config();
   cfg.output_power = true;
   M5.begin(cfg);
   ```
   - `M5.config()` でデフォルト設定を取得し、`output_power` を `true` に設定。
   - バッテリ駆動ユニットでディスプレイや IO 周辺へ電力を供給するためのフラグ。
   - `M5.begin(cfg)` で本体・LCD・IOエキスパンダを初期化。

2. **ディスプレイの初期表示**
   ```cpp
   M5.Display.setTextSize(2);
   M5.Display.setCursor(0, 0);
   M5.Display.println("Wi-Fi Antenna");
   ```
   - テキストサイズを 2 に設定し、左上にタイトルを表示。

3. **アンテナ切替ラインの制御**
   ```cpp
   auto& ioe = M5.getIOExpander(0);
   ioe.digitalWrite(0, false);
   M5.Display.println("Mode: Internal");
   ```
   - IO エキスパンダ 0 番のポート 0 を `LOW`（`false`）に設定 → 内蔵アンテナを選択。
   - 選択状態をディスプレイへ表示。

4. **外部アンテナ用コード（コメント）**
   ```cpp
   // ioe.digitalWrite(0, true);
   // M5.Display.println("Mode: External");
   ```
   - `true` を書き込むと外部アンテナ経路へ切り替わる。
   - 利用時はコメントアウトを解除し、表示文言も合わせて変更する。

### 2. ループ (`loop`)

```cpp
void loop()
{
}
```
- ループ処理は存在せず、起動時に一度だけ設定を行う用途。
- 動的切替が不要なため `loop` は空。

## 使用 API 詳説

### `M5.config()` / `M5.begin(cfg)`
- **役割**: M5Unified の設定取得と初期化。
- **`output_power`**:
  - `true` にすると LCD・IO拡張など外部ユニットへの電源供給を強制的にオン。
  - バッテリ駆動時に画面が点灯しない場合はこの設定を行う。

### `M5.Display`
- `setTextSize(size)`: 文字の拡大倍率（整数または浮動小数）を設定。
- `setCursor(x, y)`: 表示カーソル位置を設定。次の `print/println` で使用される。
- `println(text)`: テキスト表示後にカーソルを次行へ進める。

### `M5.getIOExpander(index)`
- **戻り値**: `IOExpander_Class&`（M5Unified の IO エキスパンダ制御オブジェクト）。
- Tab5 ではアンテナ切替ラインなどが IO エキスパンダ経由で制御されている。

#### `ioe.digitalWrite(pin, level)`
- **引数**:
  - `pin`: IO エキスパンダ上のピン番号（ここでは 0）。
  - `level`: `true` で HIGH、`false` で LOW。
- アンテナパス選択ラインに対して HIGH/LOW を出力し、リレーまたは RF スイッチを切り替える。

## 実運用のポイント

1. **切替の確認**
   - 外付けアンテナ接続後に `digitalWrite(0, true)` を有効化。
   - 受信感度や接続安定性を見ながら適切なアンテナを選択。

2. **デバッグ表示**
   - 追加で `Serial.println()` を入れておくと切替状態をシリアルモニタでも追跡可能。

3. **動的切替をしたい場合**
   - ボタン入力やタッチ操作と組み合わせ、`loop()` で `digitalWrite` を切り替えるよう拡張できる。
   - 切替時には表示を更新し、どちらのモードかを常に明示する。

## 応用アイデア

- Wi-Fi RSSI（受信レベル）を測定し、切替前後で表示することでベストなアンテナを自動選択。
- タッチ UI で「Internal / External」ボタンを追加し、ユーザーが即座に切替できるようにする。
- アンテナ切替時に Wi-Fi をリセットしたくない場合、`digitalWrite` 前後で接続状態を保存しておき、切替後に自動再接続する処理を追加。

このスケッチはハードウェア制御の最小例となるため、任意の UI や計測ロジックを組み合わせて発展させやすい構成です。

