void setup() {
  pinMode(5, OUTPUT);               // 5番ピンを出力用にする
  ledcSetup(0, 1000, 8);            // PWMのチャンネル0を、周波数1000Hz、8ビットに設定する
  ledcAttachPin(5, 0);              // 5番ピンにPWMのチャンネル0を割り当てる
}

void loop() {
  for (int i = 0; i <= 255; i++) {  // 変数iを0～255で変化させながら繰り返す
    ledcWrite(0, i);                // チャンネル0でアナログ出力を行う
    delay(10);                      // 0.01秒待つ
  }
}
