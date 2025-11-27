#define PI 3.141592653

void setup() {
  Serial.begin(115200);  // シリアルモニタを初期化する
}

void loop() {
  for (int i = 0; i < 360; i++) {      // iの値を0~360で変化させながら繰り返す
    Serial.print(sin(PI / 180 * i));   // sinを求めてシリアルモニタに出力する
    Serial.print(",");                 // 「,」をシリアルモニタに出力する
    Serial.println(cos(PI / 180 * i)); // cosを求めてシリアルモニタに出力する
    delay(100);                        // 0.1秒待つ
  }
}
