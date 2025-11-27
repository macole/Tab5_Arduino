void setup() {
  pinMode(35, INPUT);       // 35番ピンを入力用にする
  pinMode(5, OUTPUT);       // 5番ピンを出力用にする
  Serial.begin(115200);     // 通信速度を115200bpsにする
}

void loop() {
  int dac;                  // int型の変数dacを宣言する

  dac = analogRead(35);     // 電圧を読み取って変数dacに代入する
  Serial.print("CdS = ");   // 「CdS = 」をシリアルモニタに出力する
  Serial.print(dac);        // 変数dacの値をシリアルモニタに出力する
  Serial.print(", LED = "); // 「, LED = 」をシリアルモニタに出力する
  if (dac < 2000) {         // 変数datが2000より小さい(＝暗い)かどうかを判断する
    digitalWrite(5, HIGH);  // LEDを点灯する
    Serial.println("ON");   // シリアルモニタに「ON」と出力する
  }
  else {
    digitalWrite(5, LOW);   // LEDを消灯する
    Serial.println("OFF");  // シリアルモニタに「OFF」と出力する
  }
  delay(100);               // 0.1秒待つ
}
