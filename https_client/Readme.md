# HTTPS Client プログラム解説

## プログラム概要

このスケッチは M5Stack（M5Unified）と ESP32 の WiFi/SSL 機能を使って HTTPS サーバーへアクセスし、取得したデータをシリアルおよび LCD に表示します。HTTPS 通信のためにルート証明書を組み込み、`WiFiClientSecure` と `HTTPClient` を組み合わせて TLS 接続を確立します。接続中の状態は LCD に逐次表示されます。

## コード全体の流れ

### 1. 定義と準備

```1:33:https_client/https_client.ino
#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char *ssid     = "TP-Link_3DE3";
const char *pass     = "11411550";

const char* rootCA = "-----BEGIN CERTIFICATE----- ...";
```

- 必要なライブラリをインクルードし、接続先 SSID/パスワードを設定。
- 公開鍵認証のルート証明書（Baltimore CyberTrust Root）をベタ書きで保持。HTTPS サイトの証明書を検証するために使用。

### 2. `setup()` 処理

```34:155:https_client/https_client.ino
auto cfg = M5.config();
M5.begin(cfg);
Serial.begin(115200);
M5.Display.println("WiFi Connecting...");
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED) { ... }
M5.Display.println("Setting time...");
setClock();
M5.Display.println("Connecting to Yahoo News...");
WiFiClientSecure *client = new WiFiClientSecure;
client->setCACert(rootCA);
HTTPClient https;
if (https.begin(*client, "https://news.yahoo.co.jp/pickup/rss.xml")) {
    int status = https.GET();
    if (status == HTTP_CODE_OK) {
        String xml = https.getString();
        Serial.print(xml);
        ...
    }
    ...
}
https.end();
delete client;
```

1. **M5初期化**: `M5.begin(cfg)` で液晶や WiFi を初期化。シリアルを 115200bps でオープン。
2. **WiFi 接続**: `WiFi.begin()` → `WiFi.status()` で接続待ち。LCD に進捗ドットを表示。
3. **IP 表示**: 接続成功後、LCD/シリアルにローカルIPを表示。
4. **時刻同期 (`setClock`)**: SSL 検証に必要な RTC を NTP (`configTime`) でセット。
5. **HTTPS アクセス**:
   - `WiFiClientSecure` をヒープ上に生成し、`setCACert(rootCA)` で CA を登録。
   - `HTTPClient` を使って Yahoo ニュース RSS へ GET リクエスト。
   - 成功時 (HTTP 200) は XML をシリアルに出力し、LCD に概要情報を表示。
   - エラー時は LCD/シリアルにエラー種別（HTTP status / connect error / GET 失敗）を出力。
6. **クリーンアップ**: `https.end()` と `delete client` で接続資源を解放。

### 3. `loop()` と `setClock()`

```157:170:https_client/https_client.ino
void loop() {
  M5.update();
  delay(100);
}

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    yield();
    nowSecs = time(nullptr);
  }
}
```

- `loop()` は UI 更新のみ。
- `setClock()` は NTP で時刻を取得し、RTC がセットされるまで待機。SSL の証明書有効期限チェックが失敗しないよう必須。

## 主要 API 解説

### M5Unified
- `M5.begin(cfg)`: ボード全体の初期化。LCD、ボタン、スピーカーなどをまとめてセットアップ。
- `M5.Display`: `setTextSize`, `setTextColor`, `fillScreen`, `setCursor`, `println` で LCD 表示を制御。
- `M5.update()`: ボタン/タッチなどの状態を更新。

### WiFi / ネットワーク
- `WiFi.begin(ssid, pass)`: WiFi 接続を開始。
- `WiFi.status()`: 接続状態を取得 (`WL_CONNECTED` になるまでループ)。
- `WiFi.localIP()`: 割り当てられたローカル IP アドレス。
- `WiFiClientSecure`: TLS 対応クライアント。`setCACert()` で信頼する CA 証明書を登録。

### HTTPClient
- `HTTPClient https;`
- `https.begin(client, url)`: 既存の `WiFiClientSecure` を利用して HTTP 接続を開始。
- `https.GET()`: GET リクエストを送信し、HTTP ステータスコードを返す。
- `https.getString()`: レスポンスボディ（テキスト）を `String` として取得。
- `https.end()`: 接続を閉じる。

### 時刻設定 (`configTime`)
- `configTime(gmtOffset, daylightOffset, ntpServer1, ntpServer2)`: NTP サーバーから時刻を取得。SSL 検証に必要。
- `time(nullptr)`: 現在の UNIX 時刻を取得し、NTP 同期完了を確認。
- `yield()`: FreeRTOS のウォッチドッグリセットを防ぐための処理。

## ルート証明書について
- ここで使われているのは Baltimore CyberTrust Root (Azure/AWS 等で一般的)。
- 対象サイトの証明書チェーンがこのルートで終端している必要がある。異なるサイトに接続する場合は、対応するルート証明書へ差し替えが必要。

## よくあるカスタマイズ
- **接続先 URL の変更**: `https.begin(*client, "https://example.com/path")` を他サイトへ変更。証明書も要更新。
- **POST/PUT の実装**: `https.POST(payload)` や `https.addHeader()` で REST API へ書き込みを行う。
- **JSON パース**: `ArduinoJson` 等でレスポンスを解析し、LCD に必要な情報のみ表示。
- **自動リトライ**: HTTP エラー時に一定回数リトライするよう `for` ループを追加。
- **証明書ピンニング**: `client->setCertificate()` / `setPrivateKey()` / `setFingerprint()` を使用してセキュリティを強化。

## 注意点
1. **ヒープ使用量**: `WiFiClientSecure` を `new` しているため、使い終わったら `delete` を忘れない。
2. **時刻同期必須**: `setClock()` を呼ばないと大半の HTTPS 接続で失敗する。
3. **表示サイズ**: LCD のテキストサイズを適宜変更可能。本文では 1〜2 サイズを用途に応じて使い分け。
4. **ブロッキング処理**: WiFi 接続や HTTP 通信はブロックするため、UI を固めたくない場合は非同期化や別タスク化を検討。

このサンプルを基に、任意の HTTPS API からデータ取得・表示する IoT ダッシュボードなどへ発展させることができます。

