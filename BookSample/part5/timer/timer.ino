#include <uTimerLib.h>

#ifdef ESP32
const int cds_pin = 35;
const int led_pin = 5;
const int thres = 2000;
#else
const int cds_pin = A0;
const int led_pin = 3;
const int thres = 500;
#endif

int led;
bool is_timer_on = false;

void blink() {
  led = ~led;                  // LEDの状態を反転する
  digitalWrite(led_pin, led);  // LEDの点灯／消灯を行う
}

void setup() {
  pinMode(cds_pin, INPUT);  // CdSセルのピンを入力用にする
  pinMode(led_pin, OUTPUT); // LEDのピンを出力用にする
  Serial.begin(115200);     // 通信速度を115200bpsにする
}

void loop() {
  int dac;                   // int型の変数dacを宣言する

  dac = analogRead(cds_pin); // 電圧を読み取って変数dacに代入する
  Serial.print("CdS = ");    // 「CdS = 」をシリアルモニタに出力する
  Serial.print(dac);         // 変数dacの値をシリアルモニタに出力する
  Serial.print(", LED = ");  // 「, LED = 」をシリアルモニタに出力する
  if (dac < thres) {         // 変数datがthresより小さい(＝暗い)かどうかを判断する
    if (!is_timer_on) {      // タイマー割り込みがオンになっていない場合のみ実行
      led = LOW;             // LEDの状態をオフにする
      TimerLib.setInterval_us(blink, 200000); // 0.2秒ごとにblink関数を実行する
      is_timer_on = true;    // タイマー割り込みオンの状態を記憶
    }
    Serial.println("ON");    // シリアルモニタに「ON」と出力する
  }
  else {
    is_timer_on = false;     // タイマー割り込みオフの状態を記憶
    TimerLib.clearTimer();   // タイマー割り込みを止める
    digitalWrite(led_pin, LOW); // LEDを消灯する
    Serial.println("OFF");   // シリアルモニタに「OFF」と出力する
  }
  delay(100);                // 0.1秒待つ
}
