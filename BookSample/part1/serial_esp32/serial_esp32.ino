void setup() {
  pinMode(5, OUTPUT);        // 5番ピンを出力用にする
  Serial.begin(115200);      // 通信速度を115200bpsにする
  Serial.println("Ready");   // シリアルモニタに「Ready」という文字を送信する
}

void loop() {
  digitalWrite(5, HIGH);     // 5番ピンをオンにする
  Serial.println("LED ON");  // シリアルモニタに「LED ON」という文字を送信する  
  delay(1000);               // 1秒間待つ
  digitalWrite(5, LOW);      // 5番ピンをオフにする
  Serial.println("LED OFF"); // シリアルモニタに「LED OFF」という文字を送信する  
  delay(1000);               // 1秒間待つ
}
