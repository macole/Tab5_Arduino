void setup() {
  pinMode(3, OUTPUT);      // 3番ピンを出力用にする
}

void loop() {
  digitalWrite(3, HIGH);   // 3番ピンをオンにする
  delay(1000);             // 1秒間待つ
  digitalWrite(3, LOW);    // 3番ピンをオフにする
  delay(1000);             // 1秒間待つ
}
