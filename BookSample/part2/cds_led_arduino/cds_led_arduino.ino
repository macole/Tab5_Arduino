void setup() {
  pinMode(A0, INPUT);       // A0ピンを入力用にする
  pinMode(3, OUTPUT);       // 3番ピンを出力用にする
  Serial.begin(115200);     // 通信速度を115200bpsにする
}

void loop() {
  int dac;                  // int型の変数dacを宣言する

  dac = analogRead(A0);     // 電圧を読み取って変数dacに代入する
  Serial.print("CdS = ");   // 「CdS = 」をシリアルモニタに出力する
  Serial.print(dac);        // 変数dacの値をシリアルモニタに出力する
  Serial.print(", LED = "); // 「, LED = 」をシリアルモニタに出力する
  if (dac < 500) {          // 変数datが500より小さい(＝暗い)かどうかを判断する
    digitalWrite(3, HIGH);  // LEDを点灯する
    Serial.println("ON");   // シリアルモニタに「ON」と出力する
  }
  else {
    digitalWrite(3, LOW);   // LEDを消灯する
    Serial.println("OFF");  // シリアルモニタに「OFF」と出力する
  }
  delay(100);               // 0.1秒待つ
}
