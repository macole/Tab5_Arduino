#include "read_cds.h"

#ifdef ESP32
const int pin_no = 35;    // CdSセルを接続したピン
#else
const int pin_no = A0;    // CdSセルを接続したピン
#endif;

void setup() {
  Serial.begin(115200);   // シリアルモニタを初期化
}

void loop() {
  double cds;

  cds = read_cds(pin_no); // CdSセルの値を読み込む
  Serial.println(cds);    // 読み込んだ値をシリアルモニタに出力
  delay(1000);            // 1秒待つ
}
