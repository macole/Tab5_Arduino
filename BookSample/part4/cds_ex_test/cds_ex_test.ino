#include <CdSex.h>

#ifdef ESP32
const int pin_no = 35;    // CdSセルを接続したピン
#else
const int pin_no = A0;    // CdSセルを接続したピン
#endif;

CdSex cds(pin_no);        // CdSexクラスのオブジェクトを宣言して初期化

void setup() {
  Serial.begin(115200);   // シリアルモニタを初期化
  cds.begin();            // CdSセルを初期化
}

void loop() {
  int val;

  val = cds.read_raw();   // CdSセルの値を読み込む
  Serial.println(val);    // 読み込んだ値をシリアルモニタに出力
  delay(1000);            // 1秒待つ
}
