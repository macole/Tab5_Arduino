# SNTP プログラム解説

## プログラム全体の概要

このプログラムは、M5StackデバイスでWiFi経由のSNTP（Simple Network Time Protocol）同期を行い、RTC（Real-Time Clock）に時刻を設定して、ディスプレイに時刻を表示するプログラムです。

## プログラムの流れ

### 1. 初期設定（1-24行目）

```1:24:SNTP/SNTP.ino
#define NTP_TIMEZONE "UTC-8"
#define NTP_SERVER1  "0.pool.ntp.org"
#define NTP_SERVER2  "1.pool.ntp.org"
#define NTP_SERVER3  "2.pool.ntp.org"

#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <esp_sntp.h>
#include <sntp.h>

#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

//const char *ssid     = "ssid";
//const char *password = "password";
const char *ssid     = "TP-Link_3DE3";
const char *password = "11411550";
```

- **タイムゾーン設定**: `UTC-8`（UTCから8時間遅れ、太平洋標準時など）日本だとUTC+9
- **NTPサーバー**: pool.ntp.org の3つのサーバーを指定（冗長性のため）
- **SDIO2ピン定義**: M5Tab5のWiFiモジュール用のピン設定
- **WiFi認証情報**: SSIDとパスワードを設定

### 2. setup()関数の処理

#### 2-1. デバイス初期化（28-32行目）

```28:32:SNTP/SNTP.ino
    M5.begin();
    Serial.begin(115200);

    M5.Display.setFont(&fonts::FreeMonoBoldOblique24pt7b);
    M5.Display.setRotation(3);
```

- `M5.begin()`: M5Unifiedライブラリの初期化
- `Serial.begin(115200)`: シリアル通信の開始（デバッグ用）
- `M5.Display.setFont()`: ディスプレイのフォント設定
- `M5.Display.setRotation(3)`: 画面を270度回転

#### 2-2. WiFiピン設定（33行目）

```33:33:SNTP/SNTP.ino
    WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3, SDIO2_RST);
```

- `WiFi.setPins()`: SDIO2インターフェースのピン割り当てを設定
  - CLK: クロック信号
  - CMD: コマンド信号
  - D0-D3: データライン（4ビット幅）
  - RST: リセット信号

#### 2-3. WiFi接続（41-54行目）

```41:54:SNTP/SNTP.ino
    // STA MODE
    WiFi.mode(WIFI_STA);
    M5.Display.println("WiFi mode set to STA");
    WiFi.begin(ssid, password);
    M5.Display.print("Connecting to ");
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Display.print(".");
    }
    M5.Display.println("");
    M5.Display.print("Connected to ");
    M5.Display.println(ssid);
    M5.Display.print("IP address: ");
    M5.Display.println(WiFi.localIP());
```

- `WiFi.mode(WIFI_STA)`: ステーションモード（WiFiアクセスポイントに接続するモード）に設定
- `WiFi.begin(ssid, password)`: WiFi接続を開始（非同期処理）
- `WiFi.status()`: 接続状態を取得
  - `WL_CONNECTED`: 接続済み
  - `WL_DISCONNECTED`: 未接続
  - `WL_CONNECT_FAILED`: 接続失敗
- `WiFi.localIP()`: 取得したIPアドレスを返す

#### 2-4. RTC確認（56-65行目）

```56:65:SNTP/SNTP.ino
    if (!M5.Rtc.isEnabled()) {
        Serial.println("RTC not found.");
        M5.Display.println("RTC not found.");
        for (;;) {
            vTaskDelay(500);
        }
    }

    M5.Display.println("RTC found.");
    Serial.println("RTC found.");
```

- `M5.Rtc.isEnabled()`: RTCが有効かどうかを確認
- RTCが見つからない場合は無限ループで停止

#### 2-5. SNTP同期（67-80行目）

```67:80:SNTP/SNTP.ino
    M5.Display.println("SNTP Sync...");

    configTzTime(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        Serial.print('.');
        delay(1000);
    }

    Serial.println("\r\n NTP Connected.");

    time_t t = time(nullptr) + 1;  // Advance one second.
    while (t > time(nullptr));     /// Synchronization in seconds
    M5.Rtc.setDateTime(gmtime(&t));
    M5.Display.clear();
```

- `configTzTime()`: タイムゾーンとNTPサーバーを設定
  - 第1引数: タイムゾーン文字列（例: "UTC-8", "JST-9"）
  - 第2-4引数: NTPサーバー（最大3つ、優先順位順）
- `sntp_get_sync_status()`: SNTP同期状態を取得
  - `SNTP_SYNC_STATUS_RESET`: リセット状態
  - `SNTP_SYNC_STATUS_IN_PROGRESS`: 同期中
  - `SNTP_SYNC_STATUS_COMPLETED`: 同期完了
- `time(nullptr)`: 現在のUnix時刻（1970年1月1日からの秒数）を取得
- `gmtime(&t)`: Unix時刻をUTCのtm構造体に変換
- `M5.Rtc.setDateTime()`: RTCに時刻を設定

### 3. loop()関数の処理（84-116行目）

```84:116:SNTP/SNTP.ino
void loop(void)
{
    static constexpr const char *const wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

    delay(500);

    auto dt = M5.Rtc.getDateTime();
    Serial.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n", dt.date.year, dt.date.month, dt.date.date,
                  wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);
    M5.Display.setCursor(0, 0);
    M5.Display.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d", dt.date.year, dt.date.month, dt.date.date,
                      wd[dt.date.weekDay], dt.time.hours, dt.time.minutes, dt.time.seconds);

    /// ESP32 internal timer
    auto t = time(nullptr);
    {
        auto tm = gmtime(&t);  // for UTC.
        Serial.printf("ESP32 UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n", tm->tm_year + 1900, tm->tm_mon + 1,
                      tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
        M5.Display.setCursor(0, 60);
        M5.Display.printf("ESP32 UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1,
                          tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
    }

    {
        auto tm = localtime(&t);  // for local timezone.
        Serial.printf("ESP32 %s:%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n", NTP_TIMEZONE, tm->tm_year + 1900,
                      tm->tm_mon + 1, tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
        M5.Display.setCursor(0, 120);
        M5.Display.printf("ESP32 %s:%04d/%02d/%02d (%s)  %02d:%02d/%02d", NTP_TIMEZONE, tm->tm_year + 1900,
                          tm->tm_mon + 1, tm->tm_mday, wd[tm->tm_wday], tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
}
```

- **RTC時刻の表示**: `M5.Rtc.getDateTime()`で取得したRTCの時刻をUTCとして表示
- **ESP32内部時刻（UTC）**: `time(nullptr)`と`gmtime()`で取得したESP32内部時刻をUTCで表示
- **ESP32内部時刻（ローカル）**: `localtime()`で取得したローカルタイムゾーンでの時刻を表示

## 主要APIの詳細解説

### WiFi API（ESP32 Arduino Core）

#### `WiFi.mode(mode)`
WiFiの動作モードを設定します。
- `WIFI_STA`: ステーションモード（WiFiアクセスポイントに接続）
- `WIFI_AP`: アクセスポイントモード（他のデバイスを接続可能にする）
- `WIFI_AP_STA`: 両方のモードを同時に使用

#### `WiFi.begin(ssid, password)`
指定されたSSIDとパスワードでWiFi接続を開始します。非同期処理のため、接続完了まで待つ必要があります。

#### `WiFi.status()`
現在のWiFi接続状態を返します。
- `WL_CONNECTED`: 接続済み
- `WL_DISCONNECTED`: 未接続
- `WL_CONNECT_FAILED`: 接続失敗
- `WL_CONNECTION_LOST`: 接続が失われた

#### `WiFi.localIP()`
現在のローカルIPアドレスを`IPAddress`オブジェクトとして返します。

### SNTP API（ESP-IDF）

#### `configTzTime(tz, server1, server2, server3)`
タイムゾーンとNTPサーバーを設定します。
- **tz**: タイムゾーン文字列
  - 形式: "UTC±N" または "TZ文字列"
  - 例: "UTC-8", "JST-9", "CST-8"
- **server1, server2, server3**: NTPサーバーのホスト名（優先順位順）
  - 最大3つまで指定可能
  - 最初のサーバーが失敗した場合、次のサーバーに自動的に切り替わる

#### `sntp_get_sync_status()`
SNTP同期の現在の状態を返します。
- `SNTP_SYNC_STATUS_RESET`: リセット状態（同期未開始）
- `SNTP_SYNC_STATUS_IN_PROGRESS`: 同期中
- `SNTP_SYNC_STATUS_COMPLETED`: 同期完了
- `SNTP_SYNC_STATUS_COMPLETED`: 同期失敗

#### `sntp_set_sync_mode(mode)`
SNTP同期モードを設定します（通常は自動設定されるため、明示的に呼ぶ必要はありません）。

### 標準C時刻API

#### `time_t time(time_t *t)`
現在のUnix時刻（1970年1月1日 00:00:00 UTCからの経過秒数）を取得します。
- 引数が`nullptr`の場合、戻り値として時刻を返す
- 引数が指定された場合、そのポインタにも時刻を書き込む

#### `struct tm *gmtime(const time_t *timep)`
Unix時刻をUTC（協定世界時）の`tm`構造体に変換します。
- `tm`構造体のメンバー:
  - `tm_year`: 年（1900年からの年数、例: 2024年は124）
  - `tm_mon`: 月（0-11、0が1月）
  - `tm_mday`: 日（1-31）
  - `tm_hour`, `tm_min`, `tm_sec`: 時、分、秒
  - `tm_wday`: 曜日（0=日曜日、6=土曜日）

#### `struct tm *localtime(const time_t *timep)`
Unix時刻をローカルタイムゾーンの`tm`構造体に変換します。
- `configTzTime()`で設定したタイムゾーンが適用される

### M5Unified API

#### `M5.Rtc.getDateTime()`
RTCから現在の日時を取得します。
- 戻り値: `RTC_DateTime`構造体
  - `date.year`, `date.month`, `date.date`: 年、月、日
  - `date.weekDay`: 曜日（0=日曜日）
  - `time.hours`, `time.minutes`, `time.seconds`: 時、分、秒

#### `M5.Rtc.setDateTime(tm)`
RTCに日時を設定します。
- 引数: `tm`構造体へのポインタ

#### `M5.Display.setCursor(x, y)`
ディスプレイのカーソル位置を設定します。
- 座標は左上が(0, 0)

#### `M5.Display.printf(format, ...)`
書式付き文字列をディスプレイに表示します。
- `printf()`と同じ書式指定子が使用可能

## プログラムの特徴

1. **冗長性**: 3つのNTPサーバーを使用することで、1つのサーバーがダウンしても他のサーバーで同期可能
2. **時刻の比較**: RTCとESP32内部時刻の両方を表示することで、時刻の正確性を確認可能
3. **タイムゾーン対応**: UTCとローカルタイムゾーンの両方を表示
4. **視覚的フィードバック**: 接続状態や同期状態をディスプレイに表示

このプログラムにより、M5Stackデバイスで正確な時刻同期と表示が可能になります。

