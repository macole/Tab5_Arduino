#include <SoftwareSerial.h>
#define SW_PIN 2
#define RX_PIN 3
#define TX_PIN 4

// 3番ピン(受信)／4番ピン(送信)でソフトウェアシリアルを行う
SoftwareSerial Serial2(RX_PIN, TX_PIN);

int sw_stat;

void setup() {
  Serial.begin(115200);          // シリアルモニタの初期化
  Serial2.begin(9600);           // ソフトウェアシリアルの初期化
  pinMode(SW_PIN, INPUT_PULLUP); // スイッチのピンを入力用にする
  sw_stat = HIGH;                // スイッチが押されていないことにする
}

void loop() {
  int sw;

  sw = digitalRead(SW_PIN);     // スイッチのピンの値を読み取る
  if (sw != sw_stat) {          // スイッチの状態が変化したかどうかを調べる
    sw_stat = sw;               // スイッチの状態を更新する
    if (sw == LOW) {            // スイッチが押された場合
      Serial.println("On");     // シリアルモニタに「On」と出力
      Serial2.print("on");      // ESP32に「on」を送信
    }
    else {                      // スイッチが離された場合
      Serial.println("Off");    // シリアルモニタに「Off」と出力
      Serial2.print("off");     // ESP32に「off」を送信
    }
  }
}
