#include <BluetoothSerial.h>

#define LED_PIN 5

BluetoothSerial SerialBT;

void setup() {
  SerialBT.begin("ESP32LED");      // Bluetoothシリアルに「ESP32LED」という名前をつけて初期化
  Serial.begin(115200);            // シリアルモニタの初期化
  pinMode(LED_PIN, OUTPUT);        // LED用のピンの初期化
}

void loop() {
  if (SerialBT.available()) {      // Bluetoothシリアルに受信したかどうかを調べる
    char ch = SerialBT.read();     // 受信した文字を得る
    Serial.println(ch);            // 受信した文字をシリアルモニタに出力
    if (ch == '1') {               // 受信した文字が「1」の場合
      digitalWrite(LED_PIN, HIGH); // LEDを点灯する
    }
    else if (ch == '0') {          // 受信した文字が「0」の場合
      digitalWrite(LED_PIN, LOW);  // LEDを消灯する
    }
  }
}
