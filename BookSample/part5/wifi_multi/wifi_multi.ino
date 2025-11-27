#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;                         // WiFiMultiクラスの変数を宣言

void setup() {
    Serial.begin(115200);                    // シリアルポートの初期化
    wifiMulti.addAP("ssid1", "password1");   // 1つ目のアクセスポイントを追加
    wifiMulti.addAP("ssid2", "password2");   // 2つ目のアクセスポイントを追加
    wifiMulti.addAP("ssid3", "password3");   // 3つ目のアクセスポイントを追加
    if(wifiMulti.run() == WL_CONNECTED) {    // アクセスポイントに接続する
        Serial.print("WiFi connected to ");  // 「WiFi connected to」と出力
        Serial.println(WiFi.SSID());         // 接続したアクセスポイントのESSIDを出力
    }
}

void loop() {
}
