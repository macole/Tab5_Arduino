# Power プログラム解説

## プログラム全体の概要

このプログラムは、M5Stackデバイス（特にM5Tab5）の電源管理機能を使用して、バッテリーの充電状態、電圧、残量を監視し、ディスプレイに表示するプログラムです。電源管理IC（PMIC）の状態をリアルタイムで確認できます。

## プログラムの流れ

### 1. 初期設定（1行目）

```1:1:Power/Power.ino
#include "M5Unified.h"
```

- **必要なライブラリ**: `M5Unified.h`のみで、電源管理機能を含む統合ライブラリ

### 2. setup()関数の処理（3-12行目）

#### 2-1. デバイス初期化（5-6行目）

```5:6:Power/Power.ino
    auto cfg = M5.config();
    M5.begin(cfg);
```

- `M5.config()`: M5Unifiedの設定オブジェクトを取得
  - デフォルト設定を取得し、必要に応じてカスタマイズ可能
- `M5.begin(cfg)`: 設定を適用してM5Stackデバイスを初期化
  - 内部でPowerクラスも自動初期化される
  - ボードタイプに応じて適切なPMICを検出・初期化

#### 2-2. ディスプレイ設定（7-11行目）

```7:11:Power/Power.ino
    M5.Display.setRotation(3);
    M5.Display.setTextDatum(top_center);
    M5.Display.setFont(&fonts::FreeMonoBold24pt7b);
    M5.Display.clear(WHITE);
    M5.Display.drawString("Tab5 Power Manager", M5.Lcd.width() / 2, 0);
```

- `M5.Display.setRotation(3)`: 画面を270度回転（縦向き表示）
- `M5.Display.setTextDatum(top_center)`: テキストの基準点を上中央に設定
  - これにより、`drawString()`の座標がテキストの上中央を基準とする
- `M5.Display.setFont()`: 24ポイントの太字モノスペースフォントを設定
- `M5.Display.clear(WHITE)`: 画面を白色でクリア
- `M5.Display.drawString()`: タイトル文字列を描画
  - `M5.Lcd.width() / 2`: 画面幅の中央（X座標）
  - `0`: 上端（Y座標）

### 3. loop()関数の処理（14-25行目）

```14:25:Power/Power.ino
void loop()
{
    M5.Display.clear();
    bool bat_ischarging = M5.Power.isCharging();
    M5.Display.setCursor(0, 100);
    M5.Display.printf("Bat Charging: %s\n", bat_ischarging ? "Yes" : "No");
    int bat_vol = M5.Power.getBatteryVoltage();
    M5.Display.printf("Bat Voltage: %dmv\n", bat_vol);
    int bat_level = M5.Power.getBatteryLevel();
    M5.Display.printf("Bat Level: %d\n", bat_level);
    delay(1000);
}
```

- **画面クリア**: 毎ループで画面をクリア（フリッカーを防ぐため）
- **充電状態取得**: `M5.Power.isCharging()`で充電中かどうかを確認
- **バッテリー電圧取得**: `M5.Power.getBatteryVoltage()`で電圧を取得（ミリボルト単位）
- **バッテリー残量取得**: `M5.Power.getBatteryLevel()`で残量を取得（0-100%）
- **表示**: カーソル位置を設定して、各情報を書式付き文字列で表示
- **待機**: `delay(1000)`で1秒待機（1秒ごとに更新）

## 主要APIの詳細解説

### M5Unified API

#### `M5.config()`
M5Unifiedの設定オブジェクトを取得します。
- **戻り値**: `config_t`構造体への参照
- **使用例**:
```cpp
auto cfg = M5.config();
cfg.output_power = true;  // 電源出力を有効化
cfg.led_brightness = 128; // LED明るさを設定
M5.begin(cfg);
```

#### `M5.begin(cfg)`
M5Stackデバイスを初期化します。
- **引数**: `config_t`構造体（省略可能、デフォルト設定を使用）
- **機能**:
  - I2Cバスの初期化
  - ディスプレイの初期化
  - Powerクラスの自動初期化
  - ボードタイプの自動検出
  - PMICの自動検出と初期化

### Power API（M5Unified）

#### `M5.Power.isCharging()`
バッテリーが充電中かどうかを確認します。
- **戻り値**: `is_charging_t`列挙型
  - `is_charging_t::is_charging`: 充電中
  - `is_charging_t::is_discharging`: 放電中（充電されていない）
  - `is_charging_t::charge_unknown`: 状態不明（PMICが未対応の場合）
- **内部実装**:
  - ボードタイプとPMICに応じて適切な方法で充電状態を検出
  - M5Tab5: IOエキスパンダーのCHG_STATピンを読み取り
  - AXP192/AXP2101: PMICレジスタから状態を取得
  - IP5306: PMICレジスタから状態を取得
- **注意**: CoreInk、M5Paper、一部のM5Stack（非I2C IP5306）では機能しない場合がある
- **使用例**:
```cpp
auto status = M5.Power.isCharging();
if (status == is_charging_t::is_charging) {
    Serial.println("充電中");
} else if (status == is_charging_t::is_discharging) {
    Serial.println("放電中");
}
```

#### `M5.Power.getBatteryVoltage()`
バッテリー電圧を取得します。
- **戻り値**: `int16_t`（ミリボルト単位）
  - 正常時: 電圧値（例: 3700 = 3.7V）
  - エラー時: 0 または -1
- **内部実装**:
  - ボードタイプとPMICに応じて適切な方法で電圧を測定
  - M5Tab5: INA226からバス電圧を取得（2S Li-Poのため、実際の電圧は取得値の2倍）
  - AXP192/AXP2101: PMICのADCから電圧を取得
  - IP5306: PMICレジスタから電圧を取得
  - ADC: ESP32のADCから電圧を取得（比率を適用）
- **使用例**:
```cpp
int voltage_mv = M5.Power.getBatteryVoltage();
float voltage_v = voltage_mv / 1000.0f;
Serial.printf("バッテリー電圧: %.2fV\n", voltage_v);
```

#### `M5.Power.getBatteryLevel()`
バッテリー残量を取得します。
- **戻り値**: `int32_t`（0-100のパーセンテージ）
  - `0-100`: 残量（%）
  - `-1`: エラー（測定不能）
  - `-2`: 未対応モデル
- **内部実装**:
  - ボードタイプとPMICに応じて適切な方法で残量を計算
  - **電圧ベース計算**（デフォルト）:
    - 電圧から残量を推定: `(電圧 - 3300) * 100 / (4150 - 3350)`
    - 3.3V以下: 0%
    - 4.15V以上: 100%
  - **PMIC内蔵機能**:
    - AXP192: PMICの内蔵アルゴリズムを使用
    - AXP2101: PMICの内蔵アルゴリズムを使用
    - IP5306: PMICの内蔵アルゴリズムを使用
  - **M5Tab5**: INA226から取得した電圧を2S Li-Po用に調整（×500）
- **注意**: 電圧ベースの計算は概算値であり、実際の残量と異なる場合がある
- **使用例**:
```cpp
int level = M5.Power.getBatteryLevel();
if (level >= 0) {
    Serial.printf("バッテリー残量: %d%%\n", level);
} else {
    Serial.println("バッテリー残量を取得できません");
}
```

### Display API（M5GFX）

#### `M5.Display.setRotation(rotation)`
ディスプレイの回転角度を設定します。
- **引数**: 回転角度（0-3）
  - `0`: 0度（通常）
  - `1`: 90度時計回り
  - `2`: 180度
  - `3`: 270度時計回り（縦向き）

#### `M5.Display.setTextDatum(datum)`
テキストの基準点（アンカーポイント）を設定します。
- **引数**: `textdatum_t`列挙型
  - `top_left`: 左上
  - `top_center`: 上中央
  - `top_right`: 右上
  - `middle_left`: 左中央
  - `middle_center`: 中央
  - `middle_right`: 右中央
  - `bottom_left`: 左下
  - `bottom_center`: 下中央
  - `bottom_right`: 右下
- **効果**: `drawString()`の座標がこの基準点を基準とする

#### `M5.Display.drawString(text, x, y)`
文字列を描画します。
- **引数**:
  - `text`: 描画する文字列
  - `x`: X座標（`setTextDatum()`で設定した基準点）
  - `y`: Y座標（`setTextDatum()`で設定した基準点）

#### `M5.Display.setCursor(x, y)`
カーソル位置を設定します（`printf()`などで使用）。
- **引数**:
  - `x`: X座標（左端が0）
  - `y`: Y座標（上端が0）

#### `M5.Display.printf(format, ...)`
書式付き文字列をディスプレイに表示します。
- **引数**: `printf()`と同じ書式指定子
- **書式指定子の例**:
  - `%d`: 整数
  - `%s`: 文字列
  - `%dmv`: 整数 + "mv"文字列

### その他のPower API（参考）

#### `M5.Power.getBatteryCurrent()`
バッテリー電流を取得します。
- **戻り値**: `int32_t`（ミリアンペア単位）
  - 正の値: 充電電流
  - 負の値: 放電電流
- **注意**: 対応しているPMICのみ（AXP192、AXP2101など）

#### `M5.Power.setBatteryCharge(enable)`
バッテリー充電を有効/無効にします。
- **引数**: `bool`（`true`=有効、`false`=無効）

#### `M5.Power.setChargeCurrent(max_mA)`
充電電流の上限を設定します。
- **引数**: `uint16_t`（ミリアンペア単位、150-3150）
- **注意**: CoreInk、M5Paper、一部のM5Stackでは機能しない

#### `M5.Power.setChargeVoltage(max_mV)`
充電電圧の上限を設定します。
- **引数**: `uint16_t`（ミリボルト単位、4200-4400）
- **注意**: CoreInk、M5Paper、一部のM5Stackでは機能しない

#### `M5.Power.getVBUSVoltage()`
VBUS（USB給電）電圧を取得します。
- **戻り値**: `int16_t`（ミリボルト単位、-1=未対応）
- **注意**: AXP192またはAXP2101を搭載したモデルのみ

#### `M5.Power.powerOff()`
電源をオフにします。
- **機能**: デバイスをシャットダウン

#### `M5.Power.timerSleep(seconds)`
タイマー付きスリープに入ります。
- **引数**: 起動までの秒数

#### `M5.Power.deepSleep(micro_seconds, touch_wakeup)`
ESP32のディープスリープに入ります。
- **引数**:
  - `micro_seconds`: 起動までのマイクロ秒（0=無制限）
  - `touch_wakeup`: タッチで起動可能かどうか

#### `M5.Power.lightSleep(micro_seconds, touch_wakeup)`
ESP32のライトスリープに入ります。
- **引数**: `deepSleep()`と同じ

## データ構造

### `is_charging_t`列挙型
充電状態を表す列挙型です。
```cpp
enum is_charging_t {
    is_discharging = 0,  // 放電中
    is_charging,         // 充電中
    charge_unknown       // 状態不明
};
```

### `pmic_t`列挙型
電源管理ICの種類を表す列挙型です。
```cpp
enum pmic_t {
    pmic_unknown,   // 不明
    pmic_adc,       // ADC（ESP32内蔵）
    pmic_axp192,    // AXP192
    pmic_ip5306,    // IP5306
    pmic_axp2101,   // AXP2101
    pmic_aw32001    // AW32001
};
```

## プログラムの特徴

1. **シンプルな構造**: 電源情報の取得と表示のみに特化
2. **リアルタイム監視**: 1秒ごとにバッテリー状態を更新
3. **視覚的フィードバック**: ディスプレイに充電状態、電圧、残量を表示
4. **ボード対応**: M5Tab5を想定した表示（タイトルに"Tab5 Power Manager"）
5. **エラーハンドリング**: 一部のAPIは未対応モデルでエラー値を返す

## M5Tab5の電源管理について

M5Tab5は以下の電源管理システムを使用しています：

- **PMIC**: INA226（電流・電圧モニターIC）
- **バッテリー**: 2S Li-Po（2セル直列）
- **電圧測定**: INA226のバス電圧を読み取り、2S用に調整（×500）
- **充電検出**: IOエキスパンダーのCHG_STATピン（pin6）を読み取り

## 使用例

### バッテリー残量に応じた処理
```cpp
int level = M5.Power.getBatteryLevel();
if (level < 20) {
    Serial.println("バッテリー残量が少ないです");
    // 低電力モードに切り替えなど
}
```

### 充電状態の監視
```cpp
auto status = M5.Power.isCharging();
if (status == is_charging_t::is_charging) {
    Serial.println("充電中です");
    // 充電中の処理
}
```

### 電圧の監視
```cpp
int voltage = M5.Power.getBatteryVoltage();
if (voltage < 3200) {
    Serial.println("バッテリー電圧が低いです");
    // 警告処理など
}
```

### 充電制御
```cpp
// 充電を有効化
M5.Power.setBatteryCharge(true);

// 充電電流を500mAに制限
M5.Power.setChargeCurrent(500);

// 充電電圧を4.2Vに設定
M5.Power.setChargeVoltage(4200);
```

このプログラムにより、M5Stackデバイスでバッテリーの状態を監視し、電源管理を行うことができます。

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
