#define USE_GOOGLE_HOME 1
#define SEND_ENV 1

#include <WiFi.h>              // WiFi
#include <WebServer.h>         // Webサーバー
#include <ESPmDNS.h>           // mDNS
#ifdef SEND_ENV
#include <HTTPClient.h>        // HTTPクライアント
#endif
#include <FS.h>                // SPIFFS
#include <Wire.h>              // I2C
#include <SPI.h>               // SPI
#include <Adafruit_Sensor.h>   // BME280
#include <Adafruit_BME280.h>   // BME280
#include <TFT_eSPI.h>          // カラー液晶
#ifdef USE_GOOGLE_HOME
#include <esp8266-google-home-notifier.h> // Google Home
#endif
#include <time.h>              // 日付・時刻

#define MQ135 35          // MQ-135の接続先ピン番号
#define AIR_WARNING 0.5   // 注意と判断する電圧
#define AIR_ERROR 1.0     // 異常と判断する電圧

// 環境に応じて書き換える部分
const char *ssid     = "your_ssid";
const char *password = "your_pass";
#ifdef USE_GOOGLE_HOME
const char *ghName   = "GoogleHomeの名前";
#endif
const char *mDNSName = "esp32env";
#ifdef SEND_ENV
const char *url = "http://192.168.1.123/esp32/env.php";
#endif

// 各オブジェクトに対応するグローバル変数
Adafruit_BME280 bme;       // BME280
WebServer server(80);      // Webサーバー
TFT_eSPI tft = TFT_eSPI(); // カラー液晶ディスプレイ
#ifdef USE_GOOGLE_HOME
GoogleHomeNotifier ghn;    // Google Home
#endif

// 各関数で使うグローバル変数
float temperature, humidity, pressure, air;
char temp_old[10], humid_old[10], pres_old[10], air_old[10];

// 基本的な初期化
void setup_basic() {
  // シリアルモニタの初期化
  Serial.begin(115200);
  // SPIFFSの初期化
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1);
  }
  // MQ135を接続したピンを入力用にする
  pinMode(MQ135, INPUT);
  // BME280の初期化
  bme.begin(0x76);
  // グローバル変数の初期化
  strcpy(temp_old, "");
  strcpy(humid_old, "");
  strcpy(pres_old, "");
  strcpy(air_old, "");
}

// 液晶ディスプレイの初期化
void setup_tft() {
  tft.init();
  // 表示の向きを設定(ピンを接続した方を左にして横向き)
  tft.setRotation(3);
  // 黒で消去
  tft.fillScreen(TFT_BLACK);
  // 「Initializing...」と表示
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("ipagothic44");
  tft.drawString("Initializing...", 0, 0, 1);
}

// WiFiの初期化
void setup_wifi() {
  // WiFiに接続する
  Serial.print("connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // 時計を合わせる
  configTime(9 * 3600, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
}

#ifdef USE_GOOGLE_HOME
// Google Homeに接続
void setup_google_home() {
  Serial.println("connecting to Google Home...");
  // Google Homeに接続する
  if (ghn.device(ghName , "ja") != true) {
    Serial.println(ghn.getLastError());
    return;
  }
  // 「GoogleHome接続OK」としゃべらせる
  if (ghn.notify("GoogleHome接続OK") != true) {
    Serial.println(ghn.getLastError());
    return;
  }
}
#endif

// Webサーバーの初期化
void setup_web_server() {
  // 「http://XXX.local/」に接続されたらhandleRoot関数を実行する
  server.on("/", handleRoot);
  // 「http://XXX.local/json」に接続されたらhandleJSON関数を実行する
  server.on("/json", handleJSON);
  // それ以外のアドレスに接続されたらhandleNotFound関数を実行する
  server.onNotFound(handleNotFound);
  // Webサーバーを起動
  server.begin();
}

// mDNSの初期化
void setup_mdns() {
  if (!MDNS.begin(mDNSName)) {
    Serial.println("mDNS failed.");
    while(1);
  }
  Serial.print("mDNS OK:");
  Serial.println(mDNSName);
  MDNS.addService("http", "tcp", 80);
}

// 固定の文字列の表示
void display_static() {
  tft.fillScreen(TFT_BLACK);
  tft.drawString("気温", 0, 0, 1);
  tft.drawString("湿度", 0, 60, 1);
  tft.drawString("気圧", 0, 120, 1);
  tft.drawString("空気", 0, 180, 1);
}

void setup() {
  // 基本的な初期化を行う
  setup_basic();
  // 液晶ディスプレイの初期化を行う
  setup_tft();
  // WiFiに接続する
  setup_wifi();
#ifdef USE_GOOGLE_HOME
  // Google Homeに接続する
  setup_google_home();
#endif
  // mDNSを初期化する
  setup_mdns();
  // Webサーバーを初期化する
  setup_web_server();
  // 固定の文字列を表示する
  display_static();
}

void sendFile(String fname) {
  // SPIFFSのindex.htmlファイルを開く
  File f = SPIFFS.open(fname, FILE_READ);
  if (f) {
    // ファイルを読み込んで変数htmlに代入する
    String html = "";
    while(f.available()){
      html.concat((char) f.read());
    }
    // ファイルを閉じる
    f.close();
    // 変数htmlの内容をクライアントに出力する
    server.send(200, "text/html", html);
  }
  else {
    // ファイルを開けなかった場合はエラーを出力する
    server.send(403, "text/plain", "index.html read error");
  }
}

void handleRoot(void) {
  sendFile("/index.html");
}

void handleJSON(void) {
  // 温度・湿度・気圧・空気状態の各値をJSON文字列で出力する
  char buf[10];
  String json = "{\"temperature\":";
  json.concat(temperature);
  json.concat(",\"humidity\":");
  json.concat(humidity);
  json.concat(",\"pressure\":");
  json.concat(pressure);
  json.concat(",\"air\":");
  dtostrf(air, 4, 2, buf);
  json.concat(buf);
  json.concat("}");
  server.send(200, "application/json", json);  
}

void handleNotFound(void) {
  server.send(404, "text/plain", "Not Found");
}

// センサーから温度・湿度・気圧・空気状況を読み込む
void read_env() {
  // MQ-135の値を読み込む
  int mq = analogRead(MQ135);
  air = (float) mq / 4096 * 3.3;
  // 温度・湿度・気圧を読み込む
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
}

void show_str(char *str, char *old_str, int x, int y) {
  // 前回表示した文字列と異なるかどうかを判断
  if (strcmp(str, old_str) != 0) {
    // 文字列を表示
    tft.drawString(str, x, y, 1);
    // 今回表示した文字列を記憶
    strcpy(old_str, str);  
  }
}

// 温度・湿度・気圧・空気状況を表示する
void show_env() {
  char buf[15];
  static long last_time = -99999999;

  long now_time = millis();
  // 前回表示してから1秒以上経過したかどうかを判断
  if (now_time - last_time > 1000) {
    // 右揃えにする
    tft.setTextDatum(TR_DATUM);
    // 文字色を黒地に白にする
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // 温度を表示する
    dtostrf(temperature, 6, 1, buf);
    strcat(buf, "℃");
    show_str(buf, temp_old, 320, 0);
    // 湿度を表示する
    dtostrf(humidity, 6, 1, buf);
    strcat(buf, "％");
    show_str(buf, humid_old, 320, 60);
    // 気圧を表示する
    dtostrf(pressure, 6, 1, buf);
    strcat(buf, "hPa");
    show_str(buf, pres_old, 320, 120);
    // 空気状態を表示する
    if (air < AIR_WARNING) {
      strcpy(buf, "正常");
    }
    else if (air < AIR_ERROR) {
      strcpy(buf, "注意");
      tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    }
    else {
      strcpy(buf, "異常");
      tft.setTextColor(TFT_WHITE, TFT_RED);
    }
    show_str(buf, air_old, 320, 180);
    last_time = now_time;
  }
}

#ifdef USE_GOOGLE_HOME
// 空気が汚れているときは通知する
void notify_air() {
  static long last_notify = -99999999;

  // 空気状態が警告か異常になっているかどうかを判断する
  if (air >= AIR_WARNING) {
    // 前回の通知から30秒以上たっているかどうかを判断する
    long now_time = millis();
    if (now_time - last_notify > 30000) {
      // 異常の場合の通知を行う
      if (air >= AIR_ERROR) {
        if (ghn.notify("空気がかなり汚れています。至急換気してください。") != true) {
          Serial.println(ghn.getLastError());
        }
      }
      // 警告の場合の通知を行う
      else if (air >= AIR_WARNING) {
        if (ghn.notify("空気が汚れています。換気してください。") != true) {
          Serial.println(ghn.getLastError());
        }
      }
      last_notify = now_time;
    }
  }
}
#endif

#ifdef SEND_ENV
// 温度・湿度・気圧・空気状況を送信する
void send_env() {
  static bool is_send = false;
  struct tm t;
  
  // 〇〇時X0分0秒かどうかを判断する
  if(getLocalTime(&t)){
    if (t.tm_sec == 0 && t.tm_min % 10 == 0) {
      // 送信済みかどうかを判断する
      if (is_send == false) {
        // 送信する文字列を作る
        String data = "year=";
        data.concat(t.tm_year + 1900);
        data.concat("&month=");
        data.concat(t.tm_mon + 1);
        data.concat("&day=");
        data.concat(t.tm_mday);
        data.concat("&hour=");
        data.concat(t.tm_hour);
        data.concat("&min=");
        data.concat(t.tm_min);
        data.concat("&sec=");
        data.concat(t.tm_sec);
        data.concat("&temperature=");
        data.concat(temperature);
        data.concat("&humidity=");
        data.concat(humidity);
        data.concat("&pressure=");
        data.concat(pressure);
        data.concat("&air=");
        data.concat(air);
        // POSTでデータを送信する
        HTTPClient http;
        if (http.begin(url)) {
          http.addHeader("Content-type", "application/x-www-form-urlencoded");
          int status = http.POST(data);
          if (status > 0) {
            if (status == HTTP_CODE_OK) {
              String html = http.getString();
              Serial.print(html);
              // 送信済み状態にする
              is_send = true;
            }
            else {
              Serial.print("HTTP Error ");
              Serial.println(status);
            }
          }
          else {
            Serial.println("Get Failed");
          }
          http.end();
        }
        else {
          Serial.print("Connect error");
        }
      }
    }
    // 〇〇時X0分0秒でない場合
    else {
      // 送信済み状態をリセットする
      is_send = false; 
    }
  }
}
#endif

void loop() {
  // センサーから温度・湿度・気圧・空気状況を読み込む
  read_env();
  // Webサーバーの処理を行う
  server.handleClient();
  // 温度・湿度・気圧・空気状況を表示する  
  show_env();
#ifdef USE_GOOGLE_HOME
  // 空気が汚れているときは通知する
  notify_air();
#endif
#ifdef SEND_ENV
  // 温度・湿度・気圧・空気状況を送信する
  send_env();
#endif
}
