void setup() {
  pinMode(A0, INPUT);    // A0ピンを入力用にする
  Serial.begin(115200);  // 通信速度を115200bpsにする
}

void loop() {
  int dac;

  dac = analogRead(A0);  // 電圧を読み取って変数dacに代入する
  Serial.println(dac);   // 変数dacの値をシリアルモニタに出力する
  delay(1000);           // 1秒待つ
}
