# WebServer プログラム解説

## プログラム全体の概要

このプログラムは、M5StackデバイスをWiFi経由でWebサーバーとして動作させ、ブラウザからアクセスして音を鳴らすことができるインタラクティブなWebアプリケーションです。HTTPリクエストを受け取り、PWM（Pulse Width Modulation）を使用してスピーカーから音を出力します。

## プログラムの流れ

### 1. 初期設定（1-17行目）

```1:17:webserver/webserver.ino
#include <M5Unified.h>
#include <WiFi.h>
#include <WebServer.h>

// 音出力関係の定数
#define LEDC_BIT 13
#define LEDC_FREQ 5000
#define SPEAKER_PIN 5
// アクセスポイントのESSIDとパスワード
const char* ssid = "TP-Link_3DE3";//"your_ssid";
const char* pass = "11411550";//"your_pass";
// 音の情報
const String tones[] = { "c", "d", "e", "f", "g", "a", "b" };
const String tones_jp[] = { "ド", "レ", "ミ", "ファ", "ソ", "ラ", "シ" };
const int freqs[] = { 262, 294, 330, 349, 392, 440, 494 };
// WebServerクラスの変数
WebServer server(80);
```

- **必要なライブラリ**:
  - `M5Unified.h`: M5Stack統合ライブラリ
  - `WiFi.h`: WiFi機能
  - `WebServer.h`: HTTPサーバー機能
- **音出力定数**:
  - `LEDC_BIT`: PWMの分解能（13ビット = 8192段階）
  - `LEDC_FREQ`: PWMの基本周波数（5000Hz）
  - `SPEAKER_PIN`: スピーカー接続ピン（5番ピン）
- **音階データ**:
  - `tones[]`: 音階の英語名（c, d, e, f, g, a, b）
  - `tones_jp[]`: 音階の日本語名（ド, レ, ミ, ファ, ソ, ラ, シ）
  - `freqs[]`: 各音階の周波数（Hz）
- **WebServer**: ポート80でHTTPサーバーを起動

### 2. setup()関数の処理（19-67行目）

#### 2-1. デバイス初期化（20-33行目）

```20:33:webserver/webserver.ino
  // M5Unifiedの初期化
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // シリアルポートの初期化
  Serial.begin(115200);
  delay(500);
  
  // ディスプレイの初期化
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.println("WiFi Connecting...");
  M5.Display.println(ssid);
```

- `M5.begin(cfg)`: M5Stackデバイスの初期化
- `Serial.begin(115200)`: シリアル通信の開始
- ディスプレイに接続状態を表示

#### 2-2. WiFi接続（35-40行目）

```35:40:webserver/webserver.ino
  // アクセスポイントに接続
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.print(".");
  }
```

- `WiFi.begin(ssid, pass)`: WiFi接続を開始
- 接続完了まで待機（ドットを表示）

#### 2-3. 接続情報表示（42-57行目）

```42:57:webserver/webserver.ino
  // ESP32のIPアドレスを出力
  Serial.println("WiFi Connected.");
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());
  
  // ディスプレイにIPアドレスを表示
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextSize(3);
  M5.Display.println("WiFi Connected!");
  M5.Display.println();
  M5.Display.setTextSize(3);
  M5.Display.print("IP: ");
  M5.Display.println(WiFi.localIP());
  M5.Display.println();
  M5.Display.println("Web Server Ready");
```

- 取得したIPアドレスをシリアルとディスプレイに表示
- Webサーバーの準備完了を表示

#### 2-4. PWM初期化とルート設定（59-66行目）

```59:66:webserver/webserver.ino
  // PWMの初期化
  ledcAttach(SPEAKER_PIN, LEDC_FREQ, LEDC_BIT) ;
  // 処理するアドレスを定義
  server.on("/", handleRoot);
  server.on("/tone", handleTone);
  server.onNotFound(handleNotFound);
  // Webサーバーを起動
  server.begin();
```

- `ledcAttach()`: PWMチャンネルをピンにアタッチ
- `server.on()`: URLパスとハンドラ関数を関連付け
- `server.onNotFound()`: 未定義のパスに対するハンドラを設定
- `server.begin()`: Webサーバーを起動

### 3. loop()関数の処理（69-72行目）

```69:72:webserver/webserver.ino
void loop() {
  M5.update();
  server.handleClient();
}
```

- `M5.update()`: M5Stackの状態を更新
- `server.handleClient()`: クライアントからのリクエストを処理

### 4. ハンドラ関数

#### 4-1. handleRoot()関数（74-100行目）

```74:100:webserver/webserver.ino
void handleRoot() {
  String html;
  int i;
  
  // HTMLを組み立てる
  html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<meta charset=\"utf-8\">";
  html += "<title>音を出す</title>";
  html += "</head>";
  html += "<body>";
  html += "<p>リンクをクリックすると音が出ます</p>";
  html += "<ul>";
  for (i = 0; i < 7; i++) {
    html += "<li><a href=\"/tone?t=";
    html += tones[i];
    html += "\">";
    html += tones_jp[i];
    html += "</a></li>";
  }
  html += "</ul>";
  html += "</body>";
  html += "</html>";
  // HTMLを出力する
  server.send(200, "text/html", html);
}
```

- **機能**: ルートパス（`/`）へのアクセス時にHTMLページを返す
- **HTML内容**:
  - UTF-8エンコーディング指定
  - 7つの音階へのリンクをリスト表示
  - 各リンクは`/tone?t=○`形式
- **レスポンス**: HTTPステータス200、Content-Type: text/html

#### 4-2. handleTone()関数（102-136行目）

```102:136:webserver/webserver.ino
void handleTone() {
  int i, freq;
  String t_name, msg;

  // 「/tone?t=○」のパラメータが指定されているかどうかを確認
  freq = 0;
  if (server.hasArg("t")) {
    // 「○」の値に応じて、
    // 音の名前と周波数を変数t_name／freqに代入
    for (i = 0; i < 7; i++) {
      if (server.arg("t").equals(tones[i])) {
        t_name = tones_jp[i];
        freq = freqs[i];
        break;
      }
    }
  }
  // 音を出す
  ledcWriteTone(SPEAKER_PIN, freq);
  if (freq == 0) {
    // freqが0の場合はエラーメッセージを変数msgに代入
    msg = "パラメータが正しく指定されていません";
  }
  else {
    // freqが0でなければ、「○の音を出しました」を変数msgに代入
    msg = t_name;
    msg += "の音を出しました";
  }
  // 変数msgの文字列を送信する
  server.send(200, "text/plain; charset=utf-8", msg);
  // 1秒待つ
  delay(1000);
  // 音を止める
  ledcWriteTone(SPEAKER_PIN, 0);
}
```

- **機能**: `/tone`パスへのアクセス時に音を鳴らす
- **処理フロー**:
  1. クエリパラメータ`t`を取得
  2. パラメータ値から対応する周波数を検索
  3. PWMで音を出力（1秒間）
  4. レスポンスを送信
  5. 音を停止
- **エラーハンドリング**: 無効なパラメータの場合はエラーメッセージを返す

#### 4-3. handleNotFound()関数（138-140行目）

```138:140:webserver/webserver.ino
void handleNotFound(void) {
  server.send(404, "text/plain", "Not Found");
}
```

- **機能**: 未定義のパスへのアクセス時に404エラーを返す

## 主要APIの詳細解説

### WebServer API（ESP32 Arduino Core）

#### `WebServer server(port)`
WebServerオブジェクトを生成します。
- **引数**: `uint16_t port`（HTTPポート番号、通常は80）
- **使用例**:
```cpp
WebServer server(80);  // ポート80でHTTPサーバーを起動
```

#### `server.begin()`
Webサーバーを起動します。
- **戻り値**: なし（void）
- **機能**: 指定されたポートでHTTPサーバーを開始
- **注意**: `setup()`内で一度だけ呼び出す

#### `server.handleClient()`
クライアントからのリクエストを処理します。
- **戻り値**: なし（void）
- **機能**: 受信したHTTPリクエストを処理し、対応するハンドラ関数を呼び出す
- **呼び出しタイミング**: `loop()`内で定期的に呼び出す必要がある

#### `server.on(path, handler)`
URLパスとハンドラ関数を関連付けます。
- **引数**:
  - `const char* path`: URLパス（例: "/", "/tone"）
  - `THandlerFunction handler`: ハンドラ関数（戻り値void、引数なし）
- **戻り値**: なし（void）
- **使用例**:
```cpp
server.on("/", handleRoot);
server.on("/tone", handleTone);
```

#### `server.onNotFound(handler)`
未定義のパスへのアクセス時のハンドラを設定します。
- **引数**: `THandlerFunction handler`（ハンドラ関数）
- **戻り値**: なし（void）
- **使用例**:
```cpp
server.onNotFound(handleNotFound);
```

#### `server.send(code, content_type, content)`
HTTPレスポンスを送信します。
- **引数**:
  - `int code`: HTTPステータスコード（200, 404など）
  - `const char* content_type`: Content-Typeヘッダー（例: "text/html", "text/plain"）
  - `const String& content`: レスポンスボディ
- **戻り値**: なし（void）
- **使用例**:
```cpp
server.send(200, "text/html", "<html>...</html>");
server.send(404, "text/plain", "Not Found");
```

#### `server.hasArg(name)`
クエリパラメータまたはPOSTパラメータが存在するか確認します。
- **引数**: `const String& name`（パラメータ名）
- **戻り値**: `bool`（存在する場合`true`）
- **使用例**:
```cpp
if (server.hasArg("t")) {
    // パラメータ"t"が存在する
}
```

#### `server.arg(name)`
クエリパラメータまたはPOSTパラメータの値を取得します。
- **引数**: `const String& name`（パラメータ名）
- **戻り値**: `String`（パラメータの値）
- **使用例**:
```cpp
String value = server.arg("t");
```

#### `server.method()`
HTTPメソッドを取得します。
- **戻り値**: `HTTPMethod`列挙型
  - `HTTP_GET`: GETリクエスト
  - `HTTP_POST`: POSTリクエスト
  - `HTTP_PUT`: PUTリクエスト
  - `HTTP_DELETE`: DELETEリクエスト
  - など

#### `server.uri()`
リクエストされたURIを取得します。
- **戻り値**: `String`（URIパス）

#### `server.client()`
接続しているクライアントオブジェクトを取得します。
- **戻り値**: `WiFiClient`（クライアントオブジェクトへの参照）

### PWM/LEDC API（ESP32 Arduino Core）

#### `ledcAttach(pin, freq, bit)`
PWMチャンネルをピンにアタッチします。
- **引数**:
  - `uint8_t pin`: ピン番号
  - `uint32_t freq`: PWM周波数（Hz）
  - `uint8_t bit`: 分解能（ビット数、8-16）
- **戻り値**: `int`（チャンネル番号、エラー時は-1）
- **機能**: 指定されたピンにPWM機能を割り当て
- **使用例**:
```cpp
ledcAttach(5, 5000, 13);  // ピン5に5000Hz、13ビット分解能のPWMを設定
```

#### `ledcWriteTone(pin, freq)`
指定された周波数の音を出力します。
- **引数**:
  - `uint8_t pin`: ピン番号
  - `double freq`: 周波数（Hz、0で停止）
- **戻り値**: なし（void）
- **機能**: PWMを使用して指定周波数の音を出力
- **使用例**:
```cpp
ledcWriteTone(5, 440);  // ピン5から440Hz（ラ）の音を出力
ledcWriteTone(5, 0);   // 音を停止
```

#### `ledcWrite(pin, duty)`
PWMのデューティ比を設定します。
- **引数**:
  - `uint8_t pin`: ピン番号
  - `uint32_t duty`: デューティ比（0-最大値）
- **戻り値**: なし（void）
- **機能**: PWMのデューティ比を設定（音の出力には`ledcWriteTone()`を使用）

### WiFi API（ESP32 Arduino Core）

#### `WiFi.begin(ssid, password)`
WiFi接続を開始します。
- **引数**:
  - `const char* ssid`: SSID
  - `const char* password`: パスワード
- **戻り値**: `wl_status_t`（接続状態）
- **注意**: 非同期処理のため、接続完了まで待つ必要がある

#### `WiFi.status()`
WiFi接続状態を取得します。
- **戻り値**: `wl_status_t`
  - `WL_CONNECTED`: 接続済み
  - `WL_DISCONNECTED`: 未接続
  - `WL_CONNECT_FAILED`: 接続失敗
  - など

#### `WiFi.localIP()`
ローカルIPアドレスを取得します。
- **戻り値**: `IPAddress`（IPアドレス）

## プログラムの特徴

1. **シンプルなWebインターフェース**: HTMLページを動的に生成
2. **インタラクティブな操作**: ブラウザからクリックで音を鳴らせる
3. **音階の実装**: 7つの音階（ドレミファソラシ）をサポート
4. **エラーハンドリング**: 無効なパラメータに対する適切な処理
5. **視覚的フィードバック**: ディスプレイに接続状態とIPアドレスを表示

## 音階と周波数の対応

| 音階 | 英語名 | 周波数（Hz） |
|------|--------|-------------|
| ド | C | 262 |
| レ | D | 294 |
| ミ | E | 330 |
| ファ | F | 349 |
| ソ | G | 392 |
| ラ | A | 440 |
| シ | B | 494 |

## 使用例

### ブラウザからのアクセス

1. **WiFi接続後、ディスプレイに表示されたIPアドレスを確認**
   - 例: `192.168.1.100`

2. **ブラウザでアクセス**
   - `http://192.168.1.100/` にアクセス
   - 音階のリストが表示される

3. **音を鳴らす**
   - リストから音階をクリック
   - 例: 「ド」をクリック → `/tone?t=c` にアクセス
   - スピーカーから「ド」の音が1秒間鳴る

### カスタマイズ例

#### 異なる音階を追加
```cpp
const String tones[] = { "c", "d", "e", "f", "g", "a", "b", "c2" };
const String tones_jp[] = { "ド", "レ", "ミ", "ファ", "ソ", "ラ", "シ", "ド（高）" };
const int freqs[] = { 262, 294, 330, 349, 392, 440, 494, 523 };
```

#### POSTリクエストの処理
```cpp
void handlePost() {
  if (server.method() == HTTP_POST) {
    String value = server.arg("freq");
    int freq = value.toInt();
    ledcWriteTone(SPEAKER_PIN, freq);
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  // ...
  server.on("/post", HTTP_POST, handlePost);
  // ...
}
```

#### JSONレスポンスの返却
```cpp
void handleJSON() {
  String json = "{";
  json += "\"status\":\"ok\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
  json += "}";
  server.send(200, "application/json", json);
}
```

#### 複数のエンドポイント
```cpp
void setup() {
  // ...
  server.on("/", handleRoot);
  server.on("/tone", handleTone);
  server.on("/status", handleStatus);
  server.on("/control", handleControl);
  server.onNotFound(handleNotFound);
  // ...
}
```

このプログラムにより、M5StackデバイスをWebサーバーとして動作させ、ブラウザからインタラクティブに音を鳴らすことができます。

