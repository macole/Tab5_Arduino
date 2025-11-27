#include <WiFi.h>
#include <WebServer.h>

// 音出力関係の定数
#define LEDC_CHANNEL 0
#define LEDC_BIT 13
#define LEDC_FREQ 5000
#define SPEAKER_PIN 5
// アクセスポイントのESSIDとパスワード
const char* ssid = "your_ssid";
const char* pass = "your_pass";
// 音の情報
const String tones[] = { "c", "d", "e", "f", "g", "a", "b" };
const String tones_jp[] = { "ド", "レ", "ミ", "ファ", "ソ", "ラ", "シ" };
const int freqs[] = { 262, 294, 330, 349, 392, 440, 494 };
// WebServerクラスの変数
WebServer server(80);

void setup() {
  // シリアルポートの初期化
  Serial.begin(115200);
  // アクセスポイントに接続
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  // ESP32のIPアドレスを出力
  Serial.println("WiFi Connected.");
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());
  // PWMの初期化
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_BIT) ;
  ledcAttachPin(SPEAKER_PIN, LEDC_CHANNEL) ;
  // 処理するアドレスを定義
  server.on("/", handleRoot);
  server.on("/tone", handleTone);
  server.onNotFound(handleNotFound);
  // Webサーバーを起動
  server.begin();
}

void loop() {
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
  ledcWriteTone(LEDC_CHANNEL, freq);
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
  ledcWriteTone(LEDC_CHANNEL, 0);
}

void handleNotFound(void) {
  server.send(404, "text/plain", "Not Found");
}
