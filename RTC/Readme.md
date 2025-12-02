# RTC プログラム解説

## プログラム全体の概要

このプログラムは、M5StackデバイスのRTC（Real-Time Clock）を使用して、手動で時刻を設定し、ディスプレイに時刻を表示するプログラムです。SNTPプログラムとは異なり、ネットワーク接続なしでRTCの時刻を設定・表示します。

## プログラムの流れ

### 1. 初期設定（1-6行目）

```1:6:RTC/RTC.ino

#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
```

- **必要なライブラリ**:
  - `Arduino.h`: Arduino基本機能
  - `M5GFX.h`: M5Stackのグラフィックライブラリ
  - `M5Unified.h`: M5Stack統合ライブラリ（RTC機能を含む）

### 2. setup()関数の処理

#### 2-1. デバイス初期化（9-13行目）

```9:13:RTC/RTC.ino
    M5.begin();
    Serial.begin(115200);

    M5.Display.setFont(&fonts::FreeMonoBoldOblique24pt7b);
    M5.Display.setRotation(3);
```

- `M5.begin()`: M5Unifiedライブラリの初期化
  - 内部でI2Cバス、ディスプレイ、RTCなどの初期化を自動実行
  - ボードタイプを自動検出して適切な設定を適用
- `Serial.begin(115200)`: シリアル通信の開始（デバッグ用）
- `M5.Display.setFont()`: ディスプレイのフォント設定
  - `&fonts::FreeMonoBoldOblique24pt7b`: 24ポイントの太字斜体モノスペースフォント
- `M5.Display.setRotation(3)`: 画面を270度回転（縦向き表示）

#### 2-2. RTC有効性確認（15-21行目）

```15:21:RTC/RTC.ino
    if (!M5.Rtc.isEnabled()) {
        Serial.println("RTC not found.");
        M5.Display.println("RTC not found.");
        for (;;) {
            vTaskDelay(500);
        }
    }
```

- `M5.Rtc.isEnabled()`: RTCが有効かどうかを確認
  - 戻り値: `true`（有効）または`false`（無効）
  - 内部実装: RTCインスタンスが存在するかどうかをチェック
- RTCが見つからない場合:
  - シリアルとディスプレイにエラーメッセージを表示
  - 無限ループで停止（`vTaskDelay(500)`で500ms待機を繰り返す）

#### 2-3. RTC時刻設定（23-28行目）

```23:28:RTC/RTC.ino
    M5.Display.println("RTC found.");
    Serial.println("RTC found.");

    M5.Display.println("Setup RTC Time");
    M5.Rtc.setDateTime( { { 2025, 11, 27 }, { 20, 10, 56 } } );
    delay(1000);
```

- `M5.Rtc.setDateTime()`: RTCに日時を設定
  - 引数: 初期化リスト形式で`rtc_datetime_t`構造体を渡す
  - 形式: `{ { 年, 月, 日 }, { 時, 分, 秒 } }`
  - この例: 2025年11月27日 20時10分56秒
  - 曜日は自動計算される（`weekDay`が無効値の場合）

### 3. loop()関数の処理（31-43行目）

```31:43:RTC/RTC.ino
void loop(void)
{
    static constexpr const char *const wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

    delay(500);

    auto dt = M5.Rtc.getDateTime();
    Serial.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n", dt.date.year, dt.date.month, dt.date.date,
                  wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);
    M5.Display.setCursor(0, 100);
    M5.Display.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d", dt.date.year, dt.date.month, dt.date.date,
                      wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);
}
```

- **曜日配列**: 静的配列で曜日の文字列を定義（0=日曜日、6=土曜日）
- **ループ間隔**: `delay(500)`で500ms待機（0.5秒ごとに更新）
- **時刻取得**: `M5.Rtc.getDateTime()`でRTCから現在の日時を取得
- **時刻表示**:
  - シリアル出力: `Serial.printf()`で書式付き文字列を出力
  - ディスプレイ表示: `M5.Display.setCursor(0, 100)`でカーソル位置を設定し、`M5.Display.printf()`で表示
  - 書式: `YYYY/MM/DD (曜日) HH:MM:SS`

## 主要APIの詳細解説

### M5Unified API

#### `M5.begin()`
M5Stackデバイス全体を初期化します。
- **機能**:
  - I2Cバスの初期化
  - ディスプレイの初期化
  - RTCの自動検出と初期化
  - ボードタイプの自動検出
- **戻り値**: なし（void）
- **内部処理**:
  - ボードタイプに応じて適切なRTCチップを検出
  - M5Tab5: RX8130
  - M5PowerHub: RTC_PowerHub
  - その他: PCF8563（デフォルト）

#### `M5.Display.setFont(font)`
ディスプレイのフォントを設定します。
- **引数**: フォントオブジェクトへのポインタ
- **利用可能なフォント**: `fonts::`名前空間内の各種フォント
  - `FreeMonoBoldOblique24pt7b`: 24ポイント太字斜体モノスペース
  - その他多数のフォントが利用可能

#### `M5.Display.setRotation(rotation)`
ディスプレイの回転角度を設定します。
- **引数**: 回転角度（0-3）
  - `0`: 0度（通常）
  - `1`: 90度時計回り
  - `2`: 180度
  - `3`: 270度時計回り（縦向き）

#### `M5.Display.setCursor(x, y)`
ディスプレイのカーソル位置を設定します。
- **引数**:
  - `x`: X座標（左端が0）
  - `y`: Y座標（上端が0）

#### `M5.Display.printf(format, ...)`
書式付き文字列をディスプレイに表示します。
- **引数**: `printf()`と同じ書式指定子
- **書式指定子の例**:
  - `%04d`: 4桁の整数（ゼロパディング）
  - `%02d`: 2桁の整数（ゼロパディング）
  - `%s`: 文字列

### RTC API（M5Unified）

#### `M5.Rtc.isEnabled()`
RTCが有効かどうかを確認します。
- **戻り値**: `bool`
  - `true`: RTCが有効（インスタンスが存在）
  - `false`: RTCが無効（インスタンスが存在しない）
- **内部実装**:
```cpp
bool isEnabled(void) const { return _rtc_instance.get() != nullptr; }
```
- **使用例**: RTCが存在するか確認してから操作を行う

#### `M5.Rtc.setDateTime(datetime)`
RTCに日時を設定します。
- **オーバーロード**:
  1. `setDateTime(const rtc_datetime_t& datetime)`
  2. `setDateTime(const rtc_datetime_t* datetime)`
  3. `setDateTime(const rtc_date_t* date, const rtc_time_t* time)`
  4. `setDateTime(const tm* datetime)`
- **引数の形式**:
  - 初期化リスト: `{ { 年, 月, 日 }, { 時, 分, 秒 } }`
  - 構造体: `rtc_datetime_t`オブジェクト
  - 標準C: `tm`構造体へのポインタ
- **自動計算機能**:
  - `weekDay`が無効値（>6）の場合、年月日から自動計算
  - Zellerの合同式を使用して曜日を算出
- **使用例**:
```cpp
// 初期化リスト形式（このプログラムで使用）
M5.Rtc.setDateTime({ { 2025, 11, 27 }, { 20, 10, 56 } });

// 構造体形式
rtc_datetime_t dt;
dt.date = { 2025, 11, 27, 0 };  // 年, 月, 日, 曜日
dt.time = { 20, 10, 56 };        // 時, 分, 秒
M5.Rtc.setDateTime(dt);
```

#### `M5.Rtc.getDateTime()`
RTCから現在の日時を取得します。
- **オーバーロード**:
  1. `getDateTime()`: `rtc_datetime_t`を返す（このプログラムで使用）
  2. `getDateTime(rtc_datetime_t* datetime)`: ポインタ経由で取得
  3. `getDateTime(rtc_date_t* date, rtc_time_t* time)`: 日付と時刻を分けて取得
  4. `getDate(rtc_date_t* date)`: 日付のみ取得
  5. `getTime(rtc_time_t* time)`: 時刻のみ取得
- **戻り値**: `rtc_datetime_t`構造体
- **戻り値の構造**:
  - `dt.date.year`: 年（1900-2099）
  - `dt.date.month`: 月（1-12）
  - `dt.date.date`: 日（1-31）
  - `dt.date.weekDay`: 曜日（0=日曜日、6=土曜日）
  - `dt.time.hours`: 時（0-23）
  - `dt.time.minutes`: 分（0-59）
  - `dt.time.seconds`: 秒（0-59）

### データ構造

#### `rtc_datetime_t`構造体
RTCの日時を表す構造体です。
```cpp
struct rtc_datetime_t {
    rtc_date_t date;  // 日付情報
    rtc_time_t time;  // 時刻情報
};
```

#### `rtc_date_t`構造体
日付情報を表す構造体です。
```cpp
struct rtc_date_t {
    std::int16_t year;     // 年（1900-2099）
    std::int8_t month;     // 月（1-12）
    std::int8_t date;      // 日（1-31）
    std::int8_t weekDay;   // 曜日（0=日曜日、6=土曜日）
};
```
- **属性**: `__attribute__((packed))`により、メモリ効率が最適化される
- **コンストラクタ**: 初期化リストで直接値を設定可能

#### `rtc_time_t`構造体
時刻情報を表す構造体です。
```cpp
struct rtc_time_t {
    std::int8_t hours;     // 時（0-23）
    std::int8_t minutes;   // 分（0-59）
    std::int8_t seconds;   // 秒（0-59）
};
```
- **属性**: `__attribute__((packed))`により、メモリ効率が最適化される
- **コンストラクタ**: 初期化リストで直接値を設定可能

### その他のRTC API（参考）

#### `M5.Rtc.getVoltLow()`
RTCの電圧低下を検出します。
- **戻り値**: `bool`
  - `true`: 電圧低下を検出（バッテリ切れの可能性）
  - `false`: 正常

#### `M5.Rtc.setTimerIRQ(timer_msec)`
タイマー割り込みを設定します。
- **引数**: タイマー間隔（ミリ秒、0で無効化）
- **戻り値**: 設定されたミリ秒数

#### `M5.Rtc.setAlarmIRQ(date, time)`
アラーム割り込みを設定します。
- **引数**: 日時（`rtc_date_t*`, `rtc_time_t*`）
- **戻り値**: 成功時は0以上、失敗時は-1

#### `M5.Rtc.getIRQstatus()`
割り込みステータスを取得します。
- **戻り値**: `bool`（割り込み発生時は`true`）

#### `M5.Rtc.clearIRQ()`
割り込みフラグをクリアします。

#### `M5.Rtc.disableIRQ()`
割り込みを無効化します。

## プログラムの特徴

1. **シンプルな構造**: ネットワーク接続不要で、RTCの基本機能のみを使用
2. **手動時刻設定**: プログラム内で直接時刻を設定（SNTPプログラムとは異なる）
3. **リアルタイム表示**: 0.5秒ごとにRTCの時刻を取得して表示
4. **エラーハンドリング**: RTCが見つからない場合の適切な処理
5. **視覚的フィードバック**: シリアルとディスプレイの両方に情報を表示

## SNTPプログラムとの違い

| 項目 | RTCプログラム | SNTPプログラム |
|------|-------------|---------------|
| 時刻設定方法 | 手動（プログラム内） | ネットワーク経由（NTP同期） |
| ネットワーク | 不要 | 必要（WiFi） |
| 時刻の正確性 | 手動設定に依存 | NTPサーバーから自動取得 |
| 用途 | RTCの動作確認、オフライン環境 | 正確な時刻同期が必要な環境 |

## 使用例

### 現在時刻を設定する場合
```cpp
// 現在の日時を設定（例: 2025年1月15日 14時30分0秒）
M5.Rtc.setDateTime({ { 2025, 1, 15 }, { 14, 30, 0 } });
```

### 日付のみ更新する場合
```cpp
rtc_date_t date = { 2025, 12, 25, 0 };  // 年, 月, 日, 曜日
M5.Rtc.setDate(date);
```

### 時刻のみ更新する場合
```cpp
rtc_time_t time = { 12, 0, 0 };  // 時, 分, 秒
M5.Rtc.setTime(time);
```

### 標準Cのtm構造体から設定する場合
```cpp
time_t now = time(nullptr);
struct tm* timeinfo = localtime(&now);
M5.Rtc.setDateTime(timeinfo);
```

このプログラムにより、M5StackデバイスでRTCの基本機能を確認し、手動で時刻を設定・表示することができます。

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
