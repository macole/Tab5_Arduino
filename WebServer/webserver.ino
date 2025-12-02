#include <M5Unified.h>
#include <WiFi.h>
#include <WebServer.h>

// WiFi認証情報は secrets.h から読み込みます
// WebServer ディレクトリ内の secrets.h.example をコピーして secrets.h を作成し、
// 実際の SSID とパスワードを設定してください。
#include "secrets.h"

// 音出力関係の定数
#define LEDC_BIT 13
#define LEDC_FREQ 5000
#define SPEAKER_PIN 5
// 音の情報
const String tones[] = { "c", "d", "e", "f", "g", "a", "b" };
const String tones_jp[] = { "ド", "レ", "ミ", "ファ", "ソ", "ラ", "シ" };
const int freqs[] = { 262, 294, 330, 349, 392, 440, 494 };
// WebServerクラスの変数
WebServer server(80);

void setup() {
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
  
  // アクセスポイントに接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.print(".");
  }
  
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
  
  // PWMの初期化
  ledcAttach(SPEAKER_PIN, LEDC_FREQ, LEDC_BIT) ;
  // 処理するアドレスを定義
  server.on("/", handleRoot);
  server.on("/tone", handleTone);
  server.onNotFound(handleNotFound);
  // Webサーバーを起動
  server.begin();
}

void loop() {
  M5.update();
  server.handleClient();
}

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

void handleNotFound(void) {
  server.send(404, "text/plain", "Not Found");
}
