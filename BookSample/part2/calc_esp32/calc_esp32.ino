void setup() {
  pinMode(35, INPUT);      // 35番ピンを入力用にする
  Serial.begin(115200);    // 通信速度を115200bpsにする
}

void loop() {
  int dac;                 // int型の変数dacを宣言する
  float volt;              // float型の変数voltsを宣言する

  dac = analogRead(35);    // 電圧を読み取って変数dacに代入する
  volt = 3.3 * dac / 4096; // 読み取った値を電圧に変換する
  Serial.println(volt);    // 変数voltの値をシリアルモニタに出力する
  delay(1000);             // 1秒待つ
}
