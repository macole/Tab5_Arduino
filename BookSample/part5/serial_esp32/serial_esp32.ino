#define LED_PIN 2

void setup() {
  Serial.begin(115200);     // シリアルモニタを初期化
  Serial2.begin(9600);      // Serial2を初期化
  pinMode(LED_PIN, OUTPUT); // LEDを消灯
}

void loop() {
  String str;

  if (Serial2.available()) {       // Serial2にデータを受信している？
    str = Serial2.readString();    // 受信した文字列を変数strに代入
    Serial.println(str);           // シリアルモニタにstrの値を出力
    if (str.equals("on")) {        // 受信した文字列が「on」の場合
      digitalWrite(LED_PIN, HIGH); // LEDを点灯する
    }
    else if (str.equals("off")) {  // 受信した文字列が「off」の場合
      digitalWrite(LED_PIN, LOW);  // LEDを消灯する
    }
  }
}
