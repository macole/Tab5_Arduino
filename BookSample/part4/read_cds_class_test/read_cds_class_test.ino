#include "CdS.h"

#ifdef ESP32
const int pin_no = 35;    // CdSセルを接続したピン
#else
const int pin_no = A0;    // CdSセルを接続したピン
#endif;

CdS cds(pin_no);          // CdSクラスのオブジェクトを宣言して初期化

void setup() {
  Serial.begin(115200);   // シリアルモニタを初期化
  cds.begin();            // CdSセルを初期化
}

void loop() {
  double val;

  val = cds.read();       // CdSセルの値を読み込む
  Serial.println(val);    // 読み込んだ値をシリアルモニタに出力
  delay(1000);            // 1秒待つ
}
