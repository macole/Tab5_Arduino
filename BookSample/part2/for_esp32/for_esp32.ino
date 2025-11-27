void setup() {
  int led;                             // ピン番号を代入するための変数

  for (led = 12; led <= 14; led ++) {  // 変数ledの値を12～14で変化させて繰り返す
    pinMode(led, OUTPUT);              // ピンを出力用に設定
  }
}

void loop() {
  int led;                             // ピン番号を代入するための変数

  for (led = 12; led <= 14; led ++) {  // 変数ledの値を12～14で変化させて繰り返す
    digitalWrite(led, HIGH);           // LEDを点灯する
    delay(1000);                       // 1秒待つ
    digitalWrite(led, LOW);            // LEDを消灯する
  }
}
