# Touch プログラム解説

## プログラム全体の概要

このプログラムは、M5Stackデバイスのタッチスクリーン機能を使用して、画面上のボタンをタッチした際に反応するインタラクティブなUIを実装したプログラムです。タッチイベントを検出し、ボタンが押されたかどうかを判定して表示を更新します。

## プログラムの流れ

### 1. 初期設定（1-8行目）

```1:8:Touch/Touch.ino
#include <M5Unified.h>
#include <M5GFX.h>

m5::touch_detail_t touchDetail;
static int32_t w;
static int32_t h;

LGFX_Button button;
```

- **必要なライブラリ**:
  - `M5Unified.h`: M5Stack統合ライブラリ（タッチ機能を含む）
  - `M5GFX.h`: M5Stackグラフィックライブラリ（ボタン機能を含む）
- **グローバル変数**:
  - `touchDetail`: タッチ情報を格納する構造体
  - `w`, `h`: 画面の幅と高さ（静的変数）
  - `button`: LGFX_Buttonクラスのインスタンス

### 2. setup()関数の処理（10-23行目）

#### 2-1. デバイス初期化（11行目）

```11:11:Touch/Touch.ino
  M5.begin();
```

- `M5.begin()`: M5Stackデバイスを初期化
  - 内部でタッチスクリーンも自動初期化される
  - ボードタイプに応じて適切なタッチドライバを検出・初期化

#### 2-2. 画面サイズ取得（13-14行目）

```13:14:Touch/Touch.ino
  w = M5.Lcd.width(); //画面の幅
  h = M5.Lcd.height();//画面の高さ
```

- `M5.Lcd.width()`: 画面の幅（ピクセル）を取得
- `M5.Lcd.height()`: 画面の高さ（ピクセル）を取得
- これらの値はボタンやテキストの配置に使用

#### 2-3. ディスプレイ初期化（16-19行目）

```16:19:Touch/Touch.ino
  M5.Lcd.fillScreen(WHITE);
  M5.Display.setRotation(0);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Button Released", w / 2, 0, &fonts::FreeMonoBold24pt7b);
```

- `M5.Lcd.fillScreen(WHITE)`: 画面を白色で塗りつぶし
- `M5.Display.setRotation(0)`: 画面回転を0度に設定（通常の向き）
- `M5.Display.setTextDatum(top_center)`: テキストの基準点を上中央に設定
- `M5.Display.drawString()`: 初期状態のテキストを描画
  - フォント: `FreeMonoBold24pt7b`（24ポイント太字モノスペース）

#### 2-4. ボタン初期化（21-22行目）

```21:22:Touch/Touch.ino
  button.initButton(&M5.Lcd, w / 2, h / 2, 200, 200, TFT_BLUE, TFT_YELLOW, TFT_BLACK, "BTN", 4, 4);
  button.drawButton();
```

- `button.initButton()`: ボタンを初期化
  - 引数:
    - `&M5.Lcd`: 描画先のディスプレイオブジェクト
    - `w / 2, h / 2`: ボタンの中心座標（画面中央）
    - `200, 200`: ボタンの幅と高さ（ピクセル）
    - `TFT_BLUE`: ボタンの外枠色
    - `TFT_YELLOW`: ボタンの塗りつぶし色
    - `TFT_BLACK`: テキスト色
    - `"BTN"`: ボタンに表示するテキスト
    - `4, 4`: テキストサイズ（X, Y方向）
- `button.drawButton()`: ボタンを画面に描画

### 3. loop()関数の処理（25-37行目）

```25:37:Touch/Touch.ino
void loop() {
  M5.update();
  touchDetail = M5.Touch.getDetail();//タッチデータを取得

  if (touchDetail.isPressed()) {
    if(button.contains(touchDetail.x, touchDetail.y)){
        M5.Display.drawString("Button  Pressed", w / 2, 0, &fonts::FreeMonoBold24pt7b);
    }
  }
  else {
    M5.Display.drawString("Button Released", w / 2, 0, &fonts::FreeMonoBold24pt7b);
  }
}
```

- **`M5.update()`**: M5Stackの状態を更新
  - 内部でタッチ情報も更新される
  - ボタン状態、センサー値なども更新
- **タッチ情報取得**: `M5.Touch.getDetail()`でタッチ詳細を取得
- **タッチ判定**: `touchDetail.isPressed()`でタッチ中かどうかを確認
- **ボタン内判定**: `button.contains()`でタッチ座標がボタン内かどうかを確認
- **表示更新**: タッチ状態に応じてテキストを更新

## 主要APIの詳細解説

### M5Unified API

#### `M5.begin()`
M5Stackデバイスを初期化します。
- **機能**:
  - I2Cバスの初期化
  - ディスプレイの初期化
  - タッチスクリーンの自動検出と初期化
  - ボードタイプの自動検出
- **戻り値**: なし（void）

#### `M5.update()`
M5Stackの状態を更新します。
- **機能**:
  - タッチ情報の更新（`Touch.update()`を内部で呼び出し）
  - ボタン状態の更新
  - センサー値の更新
- **呼び出しタイミング**: `loop()`内で定期的に呼び出す必要がある
- **内部処理**:
  - 現在時刻を取得
  - タッチドライバから生のタッチデータを取得
  - タッチ状態を解析（タッチ開始、終了、フリック、ドラッグなど）
  - 仮想ボタン（画面下部のボタンエリア）の処理

### Touch API（M5Unified）

#### `M5.Touch.getDetail(index)`
タッチ詳細情報を取得します。
- **引数**: `std::size_t index`（デフォルト: 0）
  - マルチタッチ対応デバイスで複数のタッチポイントがある場合、インデックスで指定
  - 通常は0（最初のタッチポイント）を使用
- **戻り値**: `const touch_detail_t&`（タッチ詳細構造体への参照）
- **取得できる情報**:
  - タッチ座標（`x`, `y`）
  - タッチ状態（`state`）
  - 前回の座標（`prev_x`, `prev_y`）
  - タッチ開始座標（`base_x`, `base_y`）
  - クリック回数（`click_count`）
- **使用例**:
```cpp
auto detail = M5.Touch.getDetail();
if (detail.isPressed()) {
    Serial.printf("タッチ座標: (%d, %d)\n", detail.x, detail.y);
}
```

#### `M5.Touch.getCount()`
現在のタッチポイント数を取得します。
- **戻り値**: `std::uint8_t`（0-5、最大5点まで対応）
- **使用例**:
```cpp
int count = M5.Touch.getCount();
Serial.printf("タッチポイント数: %d\n", count);
```

### touch_detail_t構造体

タッチ詳細情報を格納する構造体です。

#### メンバー変数

- **座標情報**:
  - `x`, `y`: 現在のタッチ座標（`int16_t`）
  - `prev_x`, `prev_y`: 前回のタッチ座標（`int16_t`）
  - `base_x`, `base_y`: タッチ開始時の座標（`int16_t`）
  - `size`: タッチサイズ（`uint8_t`、圧力の目安）
  - `id`: タッチポイントID（`uint8_t`、マルチタッチ用）

- **状態情報**:
  - `state`: タッチ状態（`touch_state_t`列挙型）
  - `base_msec`: タッチ開始時刻（`uint32_t`、ミリ秒）
  - `click_count`: 連続クリック回数（`uint8_t`）

#### メソッド

##### `isPressed()`
タッチ中かどうかを確認します。
- **戻り値**: `bool`
  - `true`: タッチ中
  - `false`: タッチされていない
- **内部実装**: `state & touch_state_t::mask_touch`

##### `wasPressed()`
タッチが開始された瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: タッチ開始の瞬間
  - `false`: それ以外
- **内部実装**: `state == touch_state_t::touch_begin`

##### `wasClicked()`
タッチが終了した瞬間（クリック）かどうかを確認します。
- **戻り値**: `bool`
  - `true`: タッチ終了の瞬間
  - `false`: それ以外
- **内部実装**: `state == touch_state_t::touch_end`

##### `isReleased()`
タッチが離されているかどうかを確認します。
- **戻り値**: `bool`
  - `true`: タッチされていない
  - `false`: タッチ中

##### `isHolding()`
長押し中かどうかを確認します。
- **戻り値**: `bool`
  - `true`: 長押し中（デフォルト500ms以上）
  - `false`: それ以外

##### `wasHold()`
長押しが開始された瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: 長押し開始の瞬間

##### `wasFlickStart()`
フリックが開始された瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: フリック開始の瞬間

##### `isFlicking()`
フリック中かどうかを確認します。
- **戻り値**: `bool`
  - `true`: フリック中

##### `wasFlicked()`
フリックが終了した瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: フリック終了の瞬間

##### `wasDragStart()`
ドラッグが開始された瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: ドラッグ開始の瞬間

##### `isDragging()`
ドラッグ中かどうかを確認します。
- **戻り値**: `bool`
  - `true`: ドラッグ中

##### `wasDragged()`
ドラッグが終了した瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: ドラッグ終了の瞬間

##### `deltaX()`, `deltaY()`
前回の座標からの移動量を取得します。
- **戻り値**: `int`
  - `deltaX()`: X方向の移動量
  - `deltaY()`: Y方向の移動量

##### `distanceX()`, `distanceY()`
タッチ開始座標からの移動量を取得します。
- **戻り値**: `int`
  - `distanceX()`: X方向の移動量
  - `distanceY()`: Y方向の移動量

##### `getClickCount()`
連続クリック回数を取得します。
- **戻り値**: `uint8_t`
  - ダブルクリック、トリプルクリックなどの判定に使用

### touch_state_t列挙型

タッチ状態を表す列挙型です。

```cpp
enum touch_state_t : uint8_t {
    none         = 0b0000,  // タッチなし
    touch        = 0b0001,  // タッチ中
    touch_end    = 0b0010,  // タッチ終了
    touch_begin  = 0b0011,  // タッチ開始
    
    hold         = 0b0101,  // 長押し中
    hold_end     = 0b0110,  // 長押し終了
    hold_begin   = 0b0111,  // 長押し開始
    
    flick        = 0b1001,  // フリック中
    flick_end    = 0b1010,  // フリック終了
    flick_begin  = 0b1011,  // フリック開始
    
    drag         = 0b1101,  // ドラッグ中
    drag_end     = 0b1110,  // ドラッグ終了
    drag_begin   = 0b1111,  // ドラッグ開始
    
    mask_touch   = 0b0001,  // タッチマスク
    mask_change  = 0b0010,  // 変化マスク
    mask_holding = 0b0100,  // 長押しマスク
    mask_moving  = 0b1000   // 移動マスク
};
```

### LGFX_Button API（M5GFX）

#### `button.initButton(gfx, x, y, w, h, outline, fill, textcolor, label, textsize_x, textsize_y)`
ボタンを初期化します。
- **引数**:
  - `gfx`: 描画先のディスプレイオブジェクト（`LovyanGFX*`）
  - `x`, `y`: ボタンの中心座標（`int16_t`）
  - `w`, `h`: ボタンの幅と高さ（`uint16_t`）
  - `outline`: 外枠の色（テンプレート引数、様々な色型に対応）
  - `fill`: 塗りつぶし色（テンプレート引数）
  - `textcolor`: テキスト色（テンプレート引数）
  - `label`: ボタンに表示するテキスト（`const char*`、最大11文字）
  - `textsize_x`, `textsize_y`: テキストサイズ（`float`、デフォルト: 1.0, 0.0）
- **注意**: 座標はボタンの中心を基準とする
- **内部処理**: 座標を左上基準に変換して内部変数に保存

#### `button.initButtonUL(gfx, x, y, w, h, outline, fill, textcolor, label, textsize_x, textsize_y)`
ボタンを初期化します（左上座標基準）。
- **引数**: `initButton()`と同じ
- **違い**: 座標が左上を基準とする（`initButton()`は中心基準）

#### `button.drawButton(inverted, long_name)`
ボタンを画面に描画します。
- **引数**:
  - `inverted`: 反転表示するかどうか（`bool`、デフォルト: `false`）
  - `long_name`: 長いテキスト（`const char*`、デフォルト: `nullptr`）
- **描画内容**:
  - 角丸矩形の塗りつぶし
  - 角丸矩形の外枠
  - テキストの描画
- **内部処理**:
  - 角丸の半径を計算（幅と高さの小さい方の1/4）
  - 塗りつぶし色とテキスト色を設定
  - テキストを中央に配置

#### `button.contains(x, y)`
指定座標がボタン内かどうかを判定します。
- **引数**:
  - `x`, `y`: 判定する座標（`int16_t`）
- **戻り値**: `bool`
  - `true`: ボタン内
  - `false`: ボタン外
- **内部実装**: 座標がボタンの矩形内かどうかをチェック

#### `button.press(p)`
ボタンの押下状態を設定します。
- **引数**: `bool p`（`true`=押下、`false`=離す）

#### `button.isPressed()`
ボタンが押下されているかどうかを確認します。
- **戻り値**: `bool`

#### `button.justPressed()`
ボタンが押下された瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: 前回は押下されていなかったが、今回は押下されている

#### `button.justReleased()`
ボタンが離された瞬間かどうかを確認します。
- **戻り値**: `bool`
  - `true`: 前回は押下されていたが、今回は離されている

#### `button.setLabelText(label)`
ボタンのテキストを変更します。
- **引数**: `const char* label`（新しいテキスト）

#### `button.setOutlineColor(clr)`
外枠の色を変更します。
- **引数**: テンプレート引数（様々な色型に対応）

#### `button.setFillColor(clr)`
塗りつぶし色を変更します。
- **引数**: テンプレート引数

#### `button.setTextColor(clr)`
テキスト色を変更します。
- **引数**: テンプレート引数

### Display API（M5GFX）

#### `M5.Lcd.width()`, `M5.Lcd.height()`
画面の幅と高さを取得します。
- **戻り値**: `int32_t`（ピクセル単位）

#### `M5.Lcd.fillScreen(color)`
画面全体を指定色で塗りつぶします。
- **引数**: 色（様々な色型に対応）

#### `M5.Display.setRotation(rotation)`
画面の回転角度を設定します。
- **引数**: `rotation`（0-3）
  - `0`: 0度（通常）
  - `1`: 90度時計回り
  - `2`: 180度
  - `3`: 270度時計回り

#### `M5.Display.setTextDatum(datum)`
テキストの基準点を設定します。
- **引数**: `textdatum_t`列挙型
  - `top_left`, `top_center`, `top_right`
  - `middle_left`, `middle_center`, `middle_right`
  - `bottom_left`, `bottom_center`, `bottom_right`

#### `M5.Display.drawString(text, x, y, font)`
文字列を描画します。
- **引数**:
  - `text`: 描画する文字列（`const char*`）
  - `x`, `y`: 座標（`setTextDatum()`で設定した基準点）
  - `font`: フォント（`const lgfx::IFont*`、省略可能）

## プログラムの特徴

1. **シンプルな構造**: タッチ検出とボタン判定のみに特化
2. **リアルタイム反応**: タッチ状態に応じて即座に表示を更新
3. **視覚的フィードバック**: ボタンの状態をテキストで表示
4. **座標判定**: タッチ座標がボタン内かどうかを正確に判定
5. **マルチタッチ対応**: 理論的には最大5点まで対応（このプログラムでは1点のみ使用）

## 使用例

### タッチ開始の検出
```cpp
auto detail = M5.Touch.getDetail();
if (detail.wasPressed()) {
    Serial.println("タッチ開始");
}
```

### クリックの検出
```cpp
auto detail = M5.Touch.getDetail();
if (detail.wasClicked()) {
    Serial.println("クリック");
}
```

### 長押しの検出
```cpp
auto detail = M5.Touch.getDetail();
if (detail.wasHold()) {
    Serial.println("長押し開始");
}
```

### フリックの検出
```cpp
auto detail = M5.Touch.getDetail();
if (detail.wasFlicked()) {
    int dx = detail.distanceX();
    int dy = detail.distanceY();
    Serial.printf("フリック: (%d, %d)\n", dx, dy);
}
```

### ドラッグの検出
```cpp
auto detail = M5.Touch.getDetail();
if (detail.isDragging()) {
    int dx = detail.deltaX();
    int dy = detail.deltaY();
    Serial.printf("ドラッグ: (%d, %d)\n", dx, dy);
}
```

### ダブルクリックの検出
```cpp
auto detail = M5.Touch.getDetail();
if (detail.wasClicked() && detail.getClickCount() == 2) {
    Serial.println("ダブルクリック");
}
```

### 複数のボタンを作成
```cpp
LGFX_Button btn1, btn2;

void setup() {
    M5.begin();
    btn1.initButton(&M5.Lcd, 100, 100, 150, 80, TFT_BLUE, TFT_WHITE, TFT_BLACK, "BTN1", 2, 2);
    btn2.initButton(&M5.Lcd, 300, 100, 150, 80, TFT_RED, TFT_WHITE, TFT_BLACK, "BTN2", 2, 2);
    btn1.drawButton();
    btn2.drawButton();
}

void loop() {
    M5.update();
    auto detail = M5.Touch.getDetail();
    if (detail.wasClicked()) {
        if (btn1.contains(detail.x, detail.y)) {
            Serial.println("ボタン1がクリックされました");
        } else if (btn2.contains(detail.x, detail.y)) {
            Serial.println("ボタン2がクリックされました");
        }
    }
}
```

### タッチ閾値の調整
```cpp
void setup() {
    M5.begin();
    // 長押しの閾値を1000msに設定
    M5.Touch.setHoldThresh(1000);
    // フリックの閾値を16ピクセルに設定
    M5.Touch.setFlickThresh(16);
}
```

このプログラムにより、M5Stackデバイスでタッチスクリーンを使ったインタラクティブなUIを実装することができます。

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
