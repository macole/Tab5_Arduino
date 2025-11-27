// スイッチが押されたかどうかを表す変数
volatile boolean pushed = false;
// ピン番号
const int switch_pin = 2;
const int led_pin = 4;

void IRAM_ATTR onPushed() {
  // スイッチが押されたことを記憶
  pushed = true;
}

void setup() {
  // ピンの入出力モードの初期化
  pinMode(switch_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  // 2番ピン(0番割り込み)がHIGHからLOWになったらonPushed関数を実行
  attachInterrupt(switch_pin, onPushed, FALLING);
}

void loop() {
  // スイッチが押されたかどうかを判断
  if (pushed == true) {
    // LEDを1秒点灯させる
    digitalWrite(led_pin, HIGH);
    delay(1000);
    digitalWrite(led_pin, LOW);
    // スイッチが押されたことをクリア
    pushed = false;
  }
}
