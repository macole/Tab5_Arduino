#include "CdS.h"

CdS::CdS(int _pin) {
  pin = _pin;   // ピン番号を記憶する
}

void CdS::begin(void) {
  pinMode(pin, INPUT);  // ピンを入力用にする
}

double CdS::read(void) {
  int dac;
  double cds;
  
  dac = analogRead(pin);              // CdSの入力値を読み込む
  cds = (double) dac / dac_max * 100; // 読み込んだ値を0～100に変換する
  return cds;                         // 戻り値を返す
}
