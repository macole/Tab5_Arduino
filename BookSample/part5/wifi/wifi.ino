#include <WiFi.h>

void setup() {
  Serial.begin(115200);                   // シリアルポートの初期化
  WiFi.begin("my_essid", "my_password");  // アクセスポイントに接続する
  while (WiFi.status() != WL_CONNECTED) { // 接続完了したかどうかを判断
    delay(500);                           // 完了していなければ500ミリ秒待つ
  }
  Serial.println("WiFi Connected");       // シリアルモニタに「WiFi Connected」と出力
}

void loop() {
}
